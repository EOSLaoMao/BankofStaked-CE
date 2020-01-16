#include <eosiolib/transaction.hpp>
#include <eosio.token/eosio.token.hpp>
#include <eosio.system/eosio.system.hpp>
#include "../include/bankofstaked/bankofstaked.hpp"
#include "lock.cpp"
#include "utils.cpp"
#include "validation.cpp"
#include "safedelegatebw.hpp"

using namespace eosio;
using namespace eosiosystem;
using namespace bank;
using namespace lock;
using namespace utils;
using namespace validation;

class [[eosio::contract]] bankofstaked : contract
{

public:
  using contract::contract;

  [[eosio::action]]
  void clearhistory(uint64_t max_depth)
  {
    require_auth(CODE_ACCOUNT);
    uint64_t depth = 0;
    history_table o(CODE_ACCOUNT, SCOPE);
    while (o.begin() != o.end())
    {
      depth += 1;
      if(depth > max_depth) {
        break;
      }
      auto itr = o.end();
      itr--;
      o.erase(itr);
      history_table o(CODE_ACCOUNT, SCOPE);
    }
  }

  // DEBUG only, action to empty entires in both tables
  [[eosio::action]]
  void empty()
  {
    require_auth(CODE_ACCOUNT);
    /*
    plan_table p(CODE_ACCOUNT, CODE_ACCOUNT);
    while (p.begin() != p.end())
    {
      auto itr = p.end();
      itr--;
      p.erase(itr);
      plan_table p(CODE_ACCOUNT, CODE_ACCOUNT);
    }
    order_table o(CODE_ACCOUNT, SCOPE);
    while (o.begin() != o.end())
    {
      auto itr = o.end();
      itr--;
      o.erase(itr);
      order_table o(CODE_ACCOUNT, SCOPE);
    }

    plan_table p(CODE_ACCOUNT, CODE_ACCOUNT);
    while (p.begin() != p.end())
    {
      auto itr = p.end();
      itr--;
      p.erase(itr);
      plan_table p(CODE_ACCOUNT, CODE_ACCOUNT);
    }

    creditor_table c(CODE_ACCOUNT, SCOPE);
    while (c.begin() != c.end())
    {
      auto itr = c.end();
      itr--;
      c.erase(itr);
      creditor_table c(CODE_ACCOUNT, SCOPE);
    }
    freelock_table c(CODE_ACCOUNT, SCOPE);
    while (c.begin() != c.end())
    {
      auto itr = c.end();
      itr--;
      c.erase(itr);
      freelock_table c(CODE_ACCOUNT, SCOPE);
    }
    */
  }

  [[eosio::action]]
  void test(name creditor)
  {
    require_auth(CODE_ACCOUNT);

    validate_creditor(creditor);

    plan_table p(CODE_ACCOUNT, CODE_ACCOUNT.value);
    auto idx = p.get_index<"price"_n>();
    auto plan = idx.find(FREE_PLAN_AMOUNT);

    //INLINE ACTION to test delegate CPU&NET for creditor itself
    if (is_safe_creditor(creditor)) {
      INLINE_ACTION_SENDER(safedelegatebw, delegatebw)
      (creditor, {{creditor, "creditorperm"_n}}, {creditor, plan->net, plan->cpu});
    } else {
      INLINE_ACTION_SENDER(eosiosystem::system_contract, delegatebw)
      (EOSIO, {{creditor, "creditorperm"_n}}, {creditor, creditor, plan->net, plan->cpu, false});
    }

    INLINE_ACTION_SENDER(eosiosystem::system_contract, undelegatebw)
    (EOSIO, {{creditor, "creditorperm"_n}}, {creditor, creditor, plan->net, plan->cpu});

  }

  [[eosio::action]]
  void rotate(name creditor, uint64_t for_free)
  {
    require_auth(CODE_ACCOUNT);

    validate_creditor(creditor);
  }

  [[eosio::action]]
  void check(name creditor)
  {
    require_auth(CODE_ACCOUNT);

    validate_creditor(creditor);

    order_table o(CODE_ACCOUNT, SCOPE);
    uint64_t depth = 0;
    std::vector<uint64_t> order_ids;

    // order ordered by expire_at
    auto idx = o.get_index<"expire.at"_n>();
    auto itr = idx.begin();
    //force expire at most CHECK_MAX_DEPTH orders
    while (itr != idx.end() && depth < CHECK_MAX_DEPTH)
    {
      if(now() >= itr->expire_at) {
        order_ids.emplace_back(itr->id);
      }
      depth++;
      itr++;
    }
    undelegate(order_ids, 0);
    expire_freelock();
    rotate_creditor();
    get_balance(creditor);
  }

  [[eosio::action]]
  void forcexpire(const std::vector<uint64_t>& order_ids=std::vector<uint64_t>())
  {
    require_auth(CODE_ACCOUNT);

    //force expire provided orders
    undelegate(order_ids, 0);
    expire_freelock();
    rotate_creditor();
  }

  [[eosio::action]]
  void expireorder(uint64_t id)
  {
    require_auth(CODE_ACCOUNT);

    order_table o(CODE_ACCOUNT, SCOPE);
    auto order = o.find(id);
    eosio_assert(order != o.end(), "order entry not found!!!");


    // updated cpu_staked/net_staked/cpu_unstaked/net_unstaked of creditor entry
    creditor_table c(CODE_ACCOUNT, SCOPE);
    auto creditor_itr = c.find(order->creditor.value);
    c.modify(creditor_itr, RAM_PAYER, [&](auto &i) {
      i.cpu_staked -= order->cpu_staked;
      i.net_staked -= order->net_staked;
      i.cpu_unstaked += order->cpu_staked;
      i.net_unstaked += order->net_staked;
      i.balance = get_balance(order->creditor);
      i.updated_at = now();
    });

    save_order_history_table(&(*order));

    //delete order entry
    o.erase(order);
  }

  [[eosio::action]]
  void addwhitelist(name account, uint64_t capacity)
  {
    require_auth(CODE_ACCOUNT);
    whitelist_table w(CODE_ACCOUNT, SCOPE);
    auto itr = w.find(account.value);
    if(itr == w.end()) {
      w.emplace(RAM_PAYER, [&](auto &i) {
        i.account = account;
        i.capacity = capacity;
        i.created_at = now();
        i.updated_at = now();
      });
    } else {
      w.modify(itr, RAM_PAYER, [&](auto &i) {
        i.capacity = capacity;
        i.updated_at = now();
      });
    }
  }

  [[eosio::action]]
  void delwhitelist(name account, uint64_t capacity)
  {
    require_auth(CODE_ACCOUNT);
    whitelist_table w(CODE_ACCOUNT, SCOPE);
    auto itr = w.find(account.value);
    eosio_assert(itr != w.end(), "account not found in whitelist table");
    //delelete whitelist entry
    w.erase(itr);
  }

  [[eosio::action]]
  void addcreditor(name account, uint64_t for_free, std::string free_memo)
  {
    require_auth(CODE_ACCOUNT);
    creditor_table c(CODE_ACCOUNT, SCOPE);
    auto itr = c.find(account.value);
    eosio_assert(itr == c.end(), "account already exist in creditor table");

    c.emplace(RAM_PAYER, [&](auto &i) {
      i.is_active = FALSE;
      i.for_free = for_free?TRUE:FALSE;
      i.free_memo = for_free?free_memo:"";
      i.account = account;
      i.balance = get_balance(account);
      i.created_at = now();
      i.updated_at = 0; // set to 0 for creditor auto rotation
    });
  }

  [[eosio::action]]
  void addsafeacnt(name account)
  {
    require_auth(CODE_ACCOUNT);

    validate_creditor(account);

    safecreditor_table s(CODE_ACCOUNT, SCOPE);
    s.emplace(RAM_PAYER, [&](auto &i) {
      i.account = account;
      i.created_at = now();
      i.updated_at = now();
    });
  }

  [[eosio::action]]
  void delsafeacnt(name account)
  {
    require_auth(CODE_ACCOUNT);
    safecreditor_table s(CODE_ACCOUNT, SCOPE);
    auto itr = s.find(account.value);
    eosio_assert(itr != s.end(), "account does not exist in safecreditor table");
    s.erase(itr);
  }

  [[eosio::action]]
  void setrecipient(name creditor, name recipient)
  {
    require_auth(CODE_ACCOUNT);

    validate_creditor(creditor);

    recipient_table r(CODE_ACCOUNT, CODE_ACCOUNT.value);
    auto itr = r.find(creditor.value);
    if(itr == r.end()) {
        r.emplace(RAM_PAYER, [&](auto &i) {
          i.creditor = creditor;
          i.recipient_account = recipient;
          i.created_at = now();
          i.updated_at = now();
        });
    } else {
      r.modify(itr, RAM_PAYER, [&](auto &i) {
        i.recipient_account = recipient;
        i.updated_at = now();
      });
    }
  }


 [[eosio::action]] 
 void delorders(const std::vector<uint64_t> &order_ids = std::vector<uint64_t>()) 
 {
    require_auth(CODE_ACCOUNT);
    if (order_ids.size() == 0)
    {
      return;
    }

    order_table o(CODE_ACCOUNT, SCOPE);

    for (int i = 0; i < order_ids.size(); i++)
    {
      uint64_t order_id = order_ids[i];
      auto order = o.find(order_id);
      eosio_assert(order != o.end(), "order entry not found!!!");
      save_order_history_table(&(*order));
      o.erase(order);
    }
  }


  [[eosio::action]]
  void delrecipient(name creditor)
  {
    require_auth(CODE_ACCOUNT);
    recipient_table r(CODE_ACCOUNT, CODE_ACCOUNT.value);
    auto itr = r.find(creditor.value);
    eosio_assert(itr != r.end(), "recipient entry not found!!!");
    r.erase(itr);
  }

  [[eosio::action]]
  void delcreditor(name account)
  {
    require_auth(CODE_ACCOUNT);
    creditor_table c(CODE_ACCOUNT, SCOPE);
    auto itr = c.find(account.value);
    eosio_assert(itr!= c.end(), "account not found in creditor table");
    eosio_assert(itr->is_active == FALSE, "cannot delete active creditor");
    //delelete creditor entry
    c.erase(itr);
  }

  [[eosio::action]]
  void undelegatebw(name creditor, name beneficiary, asset net, asset cpu)
  {
    require_auth(CODE_ACCOUNT);
    // force undelegatebw, used to debug or fix data integrity issue
    INLINE_ACTION_SENDER(eosiosystem::system_contract, undelegatebw)
    (EOSIO, {{creditor, "creditorperm"_n}}, {creditor, beneficiary, net, cpu});
  }


  [[eosio::action]]
  void addblacklist(name account)
  {
    require_auth(CODE_ACCOUNT);
    blacklist_table b(CODE_ACCOUNT, SCOPE);
    auto itr = b.find(account.value);
    eosio_assert(itr == b.end(), "account already exist in blacklist table");

    // add entry
    b.emplace(RAM_PAYER, [&](auto &i) {
      i.account = account;
      i.created_at = now();
    });
  }


  [[eosio::action]]
  void delblacklist(name account)
  {
    require_auth(CODE_ACCOUNT);
    blacklist_table b(CODE_ACCOUNT, SCOPE);

    //make sure specified blacklist account exists
    auto itr = b.find(account.value);
    eosio_assert(itr!= b.end(), "account not found in blacklist table");
    //delelete entry
    b.erase(itr);
  }


  [[eosio::action]]
  void activate(name account)
  {
    require_auth(CODE_ACCOUNT);
    activate_creditor(account);
  }


  [[eosio::action]]
  void setplan(asset price,
               asset cpu,
               asset net,
               uint64_t duration,
               bool is_free)
  {
    require_auth(CODE_ACCOUNT);
    validate_asset(price, cpu, net);
    plan_table p(CODE_ACCOUNT, CODE_ACCOUNT.value);
    auto idx = p.get_index<"price"_n>();
    auto itr = idx.find(price.amount);
    if (itr == idx.end())
    {
      p.emplace(RAM_PAYER, [&](auto &i) {
        i.id = p.available_primary_key();
        i.price = price;
        i.cpu = cpu;
        i.net = net;
        i.duration = duration;
        i.is_active = FALSE;
        i.is_free = is_free?TRUE:FALSE;
        i.created_at = now();
        i.updated_at = now();
      });
    }
    else
    {
      idx.modify(itr, RAM_PAYER, [&](auto &i) {
        i.cpu = cpu;
        i.net = net;
        i.duration = duration;
        i.is_free = is_free?TRUE:FALSE;
        i.updated_at = now();
      });
    }
  }
  
  [[eosio::action]]
  void activateplan(asset price, bool is_active)
  {
    require_auth(CODE_ACCOUNT);
    eosio_assert(price.is_valid(), "invalid price");
    plan_table p(CODE_ACCOUNT, CODE_ACCOUNT.value);
    auto idx = p.get_index<"price"_n>();
    auto itr = idx.find(price.amount);
    eosio_assert(itr != idx.end(), "price not found");

    idx.modify(itr, RAM_PAYER, [&](auto &i) {
     i.is_active = is_active?TRUE:FALSE;
     i.updated_at = now();
    });
  }

  [[eosio::action]]
  void customorder(name beneficiary, asset quantity, asset cpu, asset net, int64_t duration)
  {
    require_auth(CODE_ACCOUNT);
    bool is_free = false;

    // limit max quantity and resources
    eosio_assert(quantity <= asset{4000000, EOS_SYMBOL}, "quantity exceeds limit");
    eosio_assert(cpu <= asset{1980000000, EOS_SYMBOL}, "cpu exceeds limit");
    eosio_assert(net <= asset{20000000, EOS_SYMBOL}, "net exceeds limit");
    eosio_assert(duration >= 10080, "duration less than limit");

    // make sure beneficiary is a valid account
    eosio_assert( is_account( beneficiary ), "to account does not exist");

    //get active creditor
    name creditor = get_active_creditor(is_free);

    //plan is not free, make sure creditor has enough balance to delegate
    asset to_delegate = cpu + net;
    if(get_balance(creditor) < to_delegate) {
      creditor = get_qualified_paid_creditor(to_delegate);
    }

    //make sure creditor is a valid account
    eosio_assert( is_account( creditor ), "creditor account does not exist");

    //validate beneficiary
    //1. beneficiary shouldnt be CODE_ACCOUNT
    //2. beneficiary shouldnt be in blacklist
    //3. each beneficiary could only have 5 affective orders at most
    validate_beneficiary(beneficiary, creditor, is_free);

    //INLINE ACTION to delegate CPU&NET for beneficiary account
    if (is_safe_creditor(creditor)) {
      INLINE_ACTION_SENDER(safedelegatebw, delegatebw)
      (creditor, {{creditor, "creditorperm"_n}}, {beneficiary, net, cpu});
    } else {
      INLINE_ACTION_SENDER(eosiosystem::system_contract, delegatebw)
      (EOSIO, {{creditor, "creditorperm"_n}}, {creditor, beneficiary, net, cpu, false});
    }

    //INLINE ACTION to call check action of `bankofstaked`
    INLINE_ACTION_SENDER(bankofstaked, check)
    (CODE_ACCOUNT, {{CODE_ACCOUNT, "bankperm"_n}}, {creditor});

    // add cpu_staked&net_staked to creditor entry
    creditor_table c(CODE_ACCOUNT, SCOPE);
    auto creditor_itr = c.find(creditor.value);
    c.modify(creditor_itr, RAM_PAYER, [&](auto &i) {
      i.cpu_staked += cpu;
      i.net_staked += net;
      i.balance = get_balance(creditor);
      i.updated_at = now();
    });

    //create Order entry
    uint64_t order_id;
    order_table o(CODE_ACCOUNT, SCOPE);
    o.emplace(RAM_PAYER, [&](auto &i) {
      i.id = o.available_primary_key();
      i.buyer = CODE_ACCOUNT;
      i.price = quantity;
      i.creditor = creditor;
      i.beneficiary = beneficiary;
      i.plan_id = std::numeric_limits<uint64_t>::max();
      i.cpu_staked = cpu;
      i.net_staked = net;
      i.is_free = is_free;
      i.created_at = now();
      i.expire_at = now() + duration * SECONDS_PER_MIN;

      order_id = i.id;
    });

    //deferred transaction to auto undelegate after expired
    std::vector<uint64_t> order_ids;
    order_ids.emplace_back(order_id);
    undelegate(order_ids, duration);
  }

  //token received
  void received_token(name from, name to, asset quantity, string memo)
  {
    //validation token transfer, only accept EOS transfer
    eosio_assert(quantity.symbol == EOS_SYMBOL, "only accept EOS transfer");

    if (to == _self)
    {
      name buyer = from;
      //if token comes from fundstostake, do nothing, just take it :)
      if (from == "fundstostake"_n)
      {
        return;
      }
      //validate plan, is_active should be TRUE
      plan_table p(CODE_ACCOUNT, CODE_ACCOUNT.value);
      auto idx = p.get_index<"price"_n>();
      auto plan = idx.find(quantity.amount);
      eosio_assert(plan->is_active == TRUE, "plan is in-active");
      eosio_assert(plan != idx.end(), "invalid price");

      name beneficiary = get_beneficiary(memo, buyer);

      // if plan is free, validate there is no Freelock for this beneficiary
      if(plan->is_free == TRUE)
      {
        validate_freelock(beneficiary);
      }

      //get active creditor
      name creditor = get_active_creditor(plan->is_free);

      //if plan is not free, make sure creditor has enough balance to delegate
      if(plan->is_free == FALSE)
      {
          asset to_delegate = plan->cpu + plan->net;
          if(get_balance(creditor) < to_delegate) {
            creditor = get_qualified_paid_creditor(to_delegate);
          }
      }

      //make sure creditor is a valid account
      eosio_assert( is_account( creditor ), "creditor account does not exist");

      //validate buyer
      //1. buyer shouldnt be CODE_ACCOUNT
      //2. buyer shouldnt be in blacklist
      //3. each buyer could only have 5 affective orders at most
      validate_buyer(buyer, plan->is_free);

      //validate beneficiary
      //1. beneficiary shouldnt be CODE_ACCOUNT
      //2. beneficiary shouldnt be in blacklist
      //3. each beneficiary could only have 5 affective orders at most
      validate_beneficiary(beneficiary, creditor, plan->is_free);

      //INLINE ACTION to delegate CPU&NET for beneficiary account
      if (is_safe_creditor(creditor)) {
        INLINE_ACTION_SENDER(safedelegatebw, delegatebw)
        (creditor, {{creditor, "creditorperm"_n}}, {beneficiary, plan->net, plan->cpu});
      } else {
        INLINE_ACTION_SENDER(eosiosystem::system_contract, delegatebw)
        (EOSIO, {{creditor, "creditorperm"_n}}, {creditor, beneficiary, plan->net, plan->cpu, false});
      }

      //INLINE ACTION to call check action of `bankofstaked`
      INLINE_ACTION_SENDER(bankofstaked, check)
      (CODE_ACCOUNT, {{CODE_ACCOUNT, "bankperm"_n}}, {creditor});

      // add cpu_staked&net_staked to creditor entry
      creditor_table c(CODE_ACCOUNT, SCOPE);
      auto creditor_itr = c.find(creditor.value);
      c.modify(creditor_itr, RAM_PAYER, [&](auto &i) {
        i.cpu_staked += plan->cpu;
        i.net_staked += plan->net;
        i.balance = get_balance(creditor);
        i.updated_at = now();
      });

      //create Order entry
      uint64_t order_id;
      order_table o(CODE_ACCOUNT, SCOPE);
      o.emplace(RAM_PAYER, [&](auto &i) {
        i.id = o.available_primary_key();
        i.buyer = buyer;
        i.price = plan->price;
        i.creditor = creditor;
        i.beneficiary = beneficiary;
        i.plan_id = plan->id;
        i.cpu_staked = plan->cpu;
        i.net_staked = plan->net;
        i.is_free = plan->is_free;
        i.created_at = now();
        i.expire_at = now() + plan->duration * SECONDS_PER_MIN;

        order_id = i.id;
      });

      if(plan->is_free == TRUE)
      {
        // if plan is free, add a Freelock entry
        add_freelock(beneficiary);
        // auto refund immediately
        //INLINE ACTION to auto refund
        creditor_table c(CODE_ACCOUNT, SCOPE);
        std::string free_memo = c.get(creditor.value).free_memo;
        std::string buyer_name = buyer.to_string();
        std::string memo = buyer_name + " " + free_memo;
        INLINE_ACTION_SENDER(eosio::token, transfer)
        (EOSIO_TOKEN, {{CODE_ACCOUNT, "bankperm"_n}}, {CODE_ACCOUNT, MASK_TRANSFER, plan->price, memo});
      }

      //deferred transaction to auto undelegate after expired
      std::vector<uint64_t> order_ids;
      order_ids.emplace_back(order_id);
      undelegate(order_ids, plan->duration);
    }
  }

private:

  void save_order_history_table(const order *order){

    
    std::string content = "";
    //save order meta to history
    //buyer|creditor|beneficiary|plan_id|price|cpu|net|created_at|expire_at
    content += order->buyer.to_string();
    content += "|" + order->creditor.to_string();
    content += "|" + order->beneficiary.to_string();
    content += "|" + std::to_string(order->plan_id);
    content += "|" + std::to_string(order->price.amount);
    content += order->is_free==TRUE?"|free":"|paid";
    content += "|" + std::to_string(order->cpu_staked.amount);
    content += "|" + std::to_string(order->net_staked.amount);
    content += "|" + std::to_string(order->created_at);
    content += "|" + std::to_string(order->expire_at);

    // save order mete data to history table
    history_table h(CODE_ACCOUNT, SCOPE);
    h.emplace(RAM_PAYER, [&](auto &i) {
      i.id = h.available_primary_key();
      i.content = content;
      i.created_at = now();
    });
  }

  //undelegate Orders specified by order_ids
  //(if duration > 0)
  //deferred transaction to auto undelegate after expired
  //(if duration == 0)
  //inline transaction to undelegate immediately
  void undelegate(const std::vector<uint64_t>& order_ids=std::vector<uint64_t>(), uint64_t duration=0)
  {
    if(order_ids.size() == 0) {
      return;
    }

    order_table o(CODE_ACCOUNT, SCOPE);
    plan_table p(CODE_ACCOUNT, CODE_ACCOUNT.value);

    if(duration > 0) {
      eosio::transaction out;
      uint64_t nonce = 0;

      for(int i=0; i<order_ids.size(); i++)
      {
        uint64_t order_id = order_ids[i];
        nonce += order_id;
        // get order entry
        auto order = o.get(order_id);

        // undelegatebw action
        action act1 = action(
          permission_level{ order.creditor, "creditorperm"_n },
          "eosio"_n, "undelegatebw"_n,
          std::make_tuple(order.creditor, order.beneficiary, order.net_staked, order.cpu_staked)
        );
        out.actions.emplace_back(act1);
        //delete order entry
        action act2 = action(
          permission_level{ CODE_ACCOUNT, "bankperm"_n },
          CODE_ACCOUNT, "expireorder"_n,
          std::make_tuple(order_id)
        );
        out.actions.emplace_back(act2);

        //if order is_free is not free, transfer income to creditor
        if (order.is_free == FALSE)
        {
          auto username = get_recipient(order.creditor);
          std::string recipient_name = username.to_string();
          std::string memo = recipient_name + " bankofstaked income";

          // transfer income to creditor
          asset income = get_income(order.creditor, order.price);
          eosio_assert(income <= order.price, "income should not be greater than price");
          action act3 = action(
            permission_level{ CODE_ACCOUNT, "bankperm"_n },
            "eosio.token"_n, "transfer"_n,
            std::make_tuple(CODE_ACCOUNT, MASK_TRANSFER, income, memo)
          );
          out.actions.emplace_back(act3);

          // transfer reserved fund to reserved_account
          asset reserved = order.price - income;
          eosio_assert(reserved <= order.price, "reserved should not be greater than price");
          recipient_name = STAKED_INCOME.to_string();
          memo = recipient_name + " bankofstaked reserved";
          action act4 = action(
            permission_level{ CODE_ACCOUNT, "bankperm"_n },
            "eosio.token"_n, "transfer"_n,
            std::make_tuple(CODE_ACCOUNT, MASK_TRANSFER, reserved, memo)
          );
          out.actions.emplace_back(act4);

        }
      }
      out.delay_sec = duration * SECONDS_PER_MIN;
      out.send((uint128_t(CODE_ACCOUNT.value) << 64) | current_time() | nonce, CODE_ACCOUNT, true);
    } else {

      for(int i=0; i<order_ids.size(); i++) {
        uint64_t order_id = order_ids[i];
        // get order entry
        auto order = o.get(order_id);

        // undelegatebw inline action
        INLINE_ACTION_SENDER(eosiosystem::system_contract, undelegatebw)
        (EOSIO, {{order.creditor, "creditorperm"_n}}, {order.creditor, order.beneficiary, order.net_staked, order.cpu_staked});

        //delete order inline action

        INLINE_ACTION_SENDER(bankofstaked, expireorder)
        (CODE_ACCOUNT, {{CODE_ACCOUNT, "bankperm"_n}}, {order.id});

        //if order is_free is not free, transfer income to creditor
        if (order.is_free == FALSE) {
          auto username = get_recipient(order.creditor);
          std::string recipient_name = username.to_string();
          std::string memo = recipient_name + " bankofstaked income";

          // transfer income to creditor
          asset income = get_income(order.creditor, order.price);
          eosio_assert(income <= order.price, "income should not be greater than price");
          INLINE_ACTION_SENDER(eosio::token, transfer)
          (EOSIO_TOKEN, {{CODE_ACCOUNT, "bankperm"_n}}, {CODE_ACCOUNT, MASK_TRANSFER, income, memo});

          // transfer reserved fund to reserved_account
          asset reserved = order.price - income;
          eosio_assert(reserved <= order.price, "reserved should not be greater than price");
          recipient_name = STAKED_INCOME.to_string();
          memo = recipient_name + " bankofstaked reserved";

          INLINE_ACTION_SENDER(eosio::token, transfer)
          (EOSIO_TOKEN, {{CODE_ACCOUNT, "bankperm"_n}}, {CODE_ACCOUNT, MASK_TRANSFER, reserved, memo});
        }
      }
    }
  }

};

extern "C" {
  void apply(uint64_t receiver, uint64_t code, uint64_t action) {
    if (code == "eosio.token"_n.value && action == "transfer"_n.value) {
      eosio::execute_action(
        name(receiver), name(code), &bankofstaked::received_token
      );
    }

    if (code == receiver) {
      switch (action) {
        EOSIO_DISPATCH_HELPER(bankofstaked,
          (clearhistory)
          (empty)
          (test)
          (rotate)
          (check)
          (forcexpire)
          (expireorder)
          (addwhitelist)
          (delwhitelist)
          (addcreditor)
          (addsafeacnt)
          (delsafeacnt)
          (delcreditor)
          (delorders)
          (addblacklist)
          (delblacklist)
          (setplan)
          (activate)
          (activateplan)
          (setrecipient)
          (delrecipient)
          (customorder)
          (undelegatebw)
        )
      }
    }
  }
}
