#include <eosiolib/currency.hpp>
#include <eosiolib/transaction.hpp>
#include <eosio.token/eosio.token.hpp>
#include <eosio.system/eosio.system.hpp>
#include <../include/bankofstaked/bankofstaked.hpp>
#include <lock.cpp>
#include <utils.cpp>
#include <validation.cpp>
#include <safedelegatebw.cpp>

using namespace eosio;
using namespace eosiosystem;
using namespace bank;
using namespace lock;
using namespace utils;
using namespace validation;

class bankofstaked : contract
{

public:
  using contract::contract;
  bankofstaked(name self) : contract(self) {}


  // @abi action clearhistory
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
  // @abi action empty
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

  // @abi action test
  void test(account_name creditor)
  {
    require_auth(CODE_ACCOUNT);

    validate_creditor(creditor);

    plan_table p(CODE_ACCOUNT, CODE_ACCOUNT);
    auto idx = p.get_index<N(price)>();
    auto plan = idx.find(FREE_PLAN_AMOUNT);

    //INLINE ACTION to test delegate CPU&NET for creditor itself
    if (is_safe_creditor(creditor)) {
      INLINE_ACTION_SENDER(safedelegatebw, delegatebw)
      (creditor, {{creditor, N(creditorperm)}}, {creditor, plan->net, plan->cpu});
    } else {
      INLINE_ACTION_SENDER(eosiosystem::system_contract, delegatebw)
      (EOSIO, {{creditor, N(creditorperm)}}, {creditor, creditor, plan->net, plan->cpu, false});
    }

    INLINE_ACTION_SENDER(eosiosystem::system_contract, undelegatebw)
    (EOSIO, {{creditor, N(creditorperm)}}, {creditor, creditor, plan->net, plan->cpu});

  }

  // @abi action rotate
  void rotate(account_name creditor, uint64_t for_free)
  {
    require_auth(CODE_ACCOUNT);

    validate_creditor(creditor);
  }

  // @abi action check
  void check(account_name creditor)
  {
    require_auth(CODE_ACCOUNT);

    validate_creditor(creditor);

    order_table o(CODE_ACCOUNT, SCOPE);
    uint64_t depth = 0;
    std::vector<uint64_t> order_ids;

    // order ordered by expire_at
    auto idx = o.get_index<N(expire_at)>();
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
    update_balance(creditor);
  }

  // @abi action forcexpire
  void forcexpire(const std::vector<uint64_t>& order_ids=std::vector<uint64_t>())
  {
    require_auth(CODE_ACCOUNT);

    //force expire provided orders
    undelegate(order_ids, 0);
    expire_freelock();
    rotate_creditor();
  }

  // @abi action expireorder
  void expireorder(uint64_t id)
  {
    require_auth(CODE_ACCOUNT);

    std::string content = "";
    order_table o(CODE_ACCOUNT, SCOPE);
    auto order = o.find(id);
    eosio_assert(order != o.end(), "order entry not found!!!");

    //save order meta to history
    //buyer|creditor|beneficiary|plan_id|price|cpu|net|created_at|expire_at
    content += (name{order->buyer}).to_string();
    content += "|" + (name{order->creditor}).to_string();
    content += "|" + (name{order->beneficiary}).to_string();
    content += "|" + std::to_string(order->plan_id);
    content += "|" + std::to_string(order->price.amount);
    content += order->is_free==TRUE?"|free":"|paid";
    content += "|" + std::to_string(order->cpu_staked.amount);
    content += "|" + std::to_string(order->net_staked.amount);
    content += "|" + std::to_string(order->created_at);
    content += "|" + std::to_string(order->expire_at);

    // updated cpu_staked/net_staked/cpu_unstaked/net_unstaked of creditor entry
    creditor_table c(CODE_ACCOUNT, SCOPE);
    auto creditor_itr = c.find(order->creditor);
    c.modify(creditor_itr, RAM_PAYER, [&](auto &i) {
      i.cpu_staked -= order->cpu_staked;
      i.net_staked -= order->net_staked;
      i.cpu_unstaked += order->cpu_staked;
      i.net_unstaked += order->net_staked;
      i.balance = get_balance(order->creditor);
      i.updated_at = now();
    });

    //delete order entry
    o.erase(order);

    // save order mete data to history table
    history_table h(CODE_ACCOUNT, SCOPE);
    h.emplace(RAM_PAYER, [&](auto &i) {
      i.id = h.available_primary_key();
      i.content = content;
      i.created_at = now();
    });
  }

  // @abi action addwhitelist
  void addwhitelist(account_name account, uint64_t capacity)
  {
    require_auth(CODE_ACCOUNT);
    whitelist_table w(CODE_ACCOUNT, SCOPE);
    auto itr = w.find(account);
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

  // @abi action delwhitelist
  void delwhitelist(account_name account, uint64_t capacity)
  {
    require_auth(CODE_ACCOUNT);
    whitelist_table w(CODE_ACCOUNT, SCOPE);
    auto itr = w.find(account);
    eosio_assert(itr != w.end(), "account not found in whitelist table");
    //delelete whitelist entry
    w.erase(itr);
  }

  // @abi action addcreditor
  void addcreditor(account_name account, uint64_t for_free, std::string free_memo)
  {
    require_auth(CODE_ACCOUNT);
    creditor_table c(CODE_ACCOUNT, SCOPE);
    auto itr = c.find(account);
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

  // @abi action addsafeacnt
  void addsafeacnt(account_name account)
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

  // @abi action delsafeacnt
  void delsafeacnt(account_name account)
  {
    require_auth(CODE_ACCOUNT);
    safecreditor_table s(CODE_ACCOUNT, SCOPE);
    auto itr = s.find(account);
    eosio_assert(itr != s.end(), "account does not exist in safecreditor table");
    s.erase(itr);
  }

  // @abi action setrecipient
  void setrecipient(account_name creditor, account_name recipient)
  {
    require_auth(CODE_ACCOUNT);

    validate_creditor(creditor);

    recipient_table r(CODE_ACCOUNT, CODE_ACCOUNT);
    auto itr = r.find(creditor);
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


  // @abi action delrecipient
  void delrecipient(account_name creditor)
  {
    require_auth(CODE_ACCOUNT);
    recipient_table r(CODE_ACCOUNT, CODE_ACCOUNT);
    auto itr = r.find(creditor);
    eosio_assert(itr != r.end(), "recipient entry not found!!!");
    r.erase(itr);
  }

  // @abi action delcreditor
  void delcreditor(account_name account)
  {
    require_auth(CODE_ACCOUNT);
    creditor_table c(CODE_ACCOUNT, SCOPE);
    auto itr = c.find(account);
    eosio_assert(itr!= c.end(), "account not found in creditor table");
    eosio_assert(itr->is_active == FALSE, "cannot delete active creditor");
    //delelete creditor entry
    c.erase(itr);
  }


  // @abi action addblacklist
  void addblacklist(account_name account)
  {
    require_auth(CODE_ACCOUNT);
    blacklist_table b(CODE_ACCOUNT, SCOPE);
    auto itr = b.find(account);
    eosio_assert(itr == b.end(), "account already exist in blacklist table");

    // add entry
    b.emplace(RAM_PAYER, [&](auto &i) {
      i.account = account;
      i.created_at = now();
    });
  }


  // @abi action delblacklist
  void delblacklist(account_name account)
  {
    require_auth(CODE_ACCOUNT);
    blacklist_table b(CODE_ACCOUNT, SCOPE);

    //make sure specified blacklist account exists
    auto itr = b.find(account);
    eosio_assert(itr!= b.end(), "account not found in blacklist table");
    //delelete entry
    b.erase(itr);
  }


  // @abi action activate
  void activate(account_name account)
  {
    require_auth(CODE_ACCOUNT);
    activate_creditor(account);
  }


  // @abi action setplan
  void setplan(asset price,
               asset cpu,
               asset net,
               uint64_t duration,
               bool is_free)
  {
    require_auth(CODE_ACCOUNT);
    validate_asset(price, cpu, net);
    plan_table p(CODE_ACCOUNT, CODE_ACCOUNT);
    auto idx = p.get_index<N(price)>();
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
  
  // @abi action activateplan
  void activateplan(asset price, bool is_active)
  {
    require_auth(CODE_ACCOUNT);
    eosio_assert(price.is_valid(), "invalid price");
    plan_table p(CODE_ACCOUNT, CODE_ACCOUNT);
    auto idx = p.get_index<N(price)>();
    auto itr = idx.find(price.amount);
    eosio_assert(itr != idx.end(), "price not found");

    idx.modify(itr, RAM_PAYER, [&](auto &i) {
     i.is_active = is_active?TRUE:FALSE;
     i.updated_at = now();
    });
  }


  //entry point of bankofstaked contract
  void apply(account_name contract, account_name action)
  {
    if (action == N(transfer) and contract == N(eosio.token))
    {
      received_token(unpack_action_data<currency::transfer>());
      return;
    }

    if (contract != _self)
      return;

    auto &thiscontract = *this;
    switch (action)
    {
      EOSIO_API(bankofstaked,
          (empty)
          (setplan)
          (activateplan)
          (expireorder)
          (addwhitelist)
          (delwhitelist)
          (addcreditor)
          (addsafeacnt)
          (delsafeacnt)
          (delcreditor)
          (addblacklist)
          (delblacklist)
          (activate)
          (check)
          (test)
          (rotate)
          (clearhistory)
          (forcexpire)
          (setrecipient)
          (delrecipient));
    };
  }

private:

  //undelegate Orders specified by order_ids
  //deferred(if duration > 0) transaction to auto undelegate after expired
  void undelegate(const std::vector<uint64_t>& order_ids=std::vector<uint64_t>(), uint64_t duration=0)
  {
    if(order_ids.size() == 0) 
    {
      return;
    }
    eosio::transaction out;

    order_table o(CODE_ACCOUNT, SCOPE);
    plan_table p(CODE_ACCOUNT, CODE_ACCOUNT);

    uint64_t nonce = 0;

    for(int i=0; i<order_ids.size(); i++)
    {
      uint64_t order_id = order_ids[i];
      nonce += order_id;
      // get order entry
      auto order = o.get(order_id);

      // undelegatebw action
      action act1 = action(
        permission_level{ order.creditor, N(creditorperm) },
        N(eosio), N(undelegatebw),
        std::make_tuple(order.creditor, order.beneficiary, order.net_staked, order.cpu_staked)
      );
      out.actions.emplace_back(act1);
      //delete order entry
      action act2 = action(
        permission_level{ CODE_ACCOUNT, N(bankperm) },
        CODE_ACCOUNT, N(expireorder),
        std::make_tuple(order_id)
      );
      out.actions.emplace_back(act2);

      //if order is_free is not free, transfer income to creditor
      if (order.is_free == FALSE)
      {
        auto plan = p.get(order.plan_id);

        auto username = name{get_recipient(order.creditor)};
        std::string recipient_name = username.to_string();
        std::string memo = recipient_name + " bankofstaked income";

        // transfer income to creditor
        asset income = get_income(order.creditor, order.price);
        eosio_assert(income <= order.price, "income should not be greater than price");
        action act3 = action(
          permission_level{ CODE_ACCOUNT, N(bankperm) },
          N(eosio.token), N(transfer),
          std::make_tuple(CODE_ACCOUNT, MASK_TRANSFER, income, memo)
        );
        out.actions.emplace_back(act3);

        // transfer reserved fund to STAKED_INCOME
        asset reserved = order.price - income;
        eosio_assert(reserved <= order.price, "reserved should not be greater than price");
        username = name{STAKED_INCOME};
        recipient_name = username.to_string();
        memo = recipient_name + " bankofstaked reserved";
        action act4 = action(
          permission_level{ CODE_ACCOUNT, N(bankperm) },
          N(eosio.token), N(transfer),
          std::make_tuple(CODE_ACCOUNT, MASK_TRANSFER, reserved, memo)
        );
        out.actions.emplace_back(act4);

      }
    }

    if(duration > 0) {
      out.delay_sec = duration * SECONDS_PER_MIN;
    }
    out.send((uint128_t(CODE_ACCOUNT) << 64) | current_time() | nonce, CODE_ACCOUNT, true);
  }

  //token received
  void received_token(const currency::transfer &t)
  {
    //validation token transfer, only accept EOS transfer
    //eosio_assert(t.quantity.symbol==symbol_type(system_token_symbol), "only accept EOS transfer");
    eosio_assert(t.quantity.symbol==EOS_SYMBOL, "only accept EOS transfer");

    if (t.to == _self)
    {
      account_name buyer = t.from;
      //if token comes from fundstostake, do nothing, just take it :)
      if (t.from == N(fundstostake))
      {
        return;
      }
      //validate plan, is_active should be TRUE
      plan_table p(CODE_ACCOUNT, CODE_ACCOUNT);
      auto idx = p.get_index<N(price)>();
      auto plan = idx.find(t.quantity.amount);
      eosio_assert(plan->is_active == TRUE, "plan is in-active");
      eosio_assert(plan != idx.end(), "invalid price");

      account_name beneficiary = get_beneficiary(t.memo, buyer);

      // if plan is free, validate there is no Freelock for this beneficiary
      if(plan->is_free == TRUE)
      {
        validate_freelock(beneficiary);
      }

      //get active creditor
      account_name creditor = get_active_creditor(plan->is_free);

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
        (creditor, {{creditor, N(creditorperm)}}, {beneficiary, plan->net, plan->cpu});
      } else {
        INLINE_ACTION_SENDER(eosiosystem::system_contract, delegatebw)
        (EOSIO, {{creditor, N(creditorperm)}}, {creditor, beneficiary, plan->net, plan->cpu, false});
      }

      //INLINE ACTION to call check action of `bankofstaked`
      INLINE_ACTION_SENDER(bankofstaked, check)
      (CODE_ACCOUNT, {{CODE_ACCOUNT, N(bankperm)}}, {creditor});

      // add cpu_staked&net_staked to creditor entry
      creditor_table c(CODE_ACCOUNT, SCOPE);
      auto creditor_itr = c.find(creditor);
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
        std::string free_memo = c.get(creditor).free_memo;
        auto username = name{buyer};
        std::string buyer_name = username.to_string();
        std::string memo = buyer_name + " " + free_memo;
        INLINE_ACTION_SENDER(eosio::token, transfer)
        (N(eosio.token), {{CODE_ACCOUNT, N(bankperm)}}, {CODE_ACCOUNT, MASK_TRANSFER, plan->price, memo});
      }

      //deferred transaction to auto undelegate after expired
      std::vector<uint64_t> order_ids;
      order_ids.emplace_back(order_id);
      undelegate(order_ids, plan->duration);
    }
  }
};

extern "C"
{
  [[noreturn]] void apply(uint64_t receiver, uint64_t code, uint64_t action) {
    bankofstaked c(receiver);
    c.apply(code, action);
    eosio_exit(0);
  }
}
