#include <eosiolib/currency.hpp>
#include <eosiolib/transaction.hpp>
#include <eosio.token/eosio.token.hpp>
#include <eosio.system/eosio.system.hpp>
#include <../include/bankofstaked/bankofstaked.hpp>
#include <lock.cpp>
#include <utils.cpp>
#include <validation.cpp>

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
  void clearhistory()
  {
    require_auth(code_account);
    uint64_t depth = 0;
    history_table o(code_account, SCOPE_ORDER>>1);
    while (o.begin() != o.end())
    {
      depth += 1;
      if(depth >100) {
        break;
      }
      auto itr = o.end();
      itr--;
      o.erase(itr);
      order_table o(code_account, SCOPE_ORDER>>1);
    }
  }

  // DEBUG only, action to empty entires in both tables
  // @abi action empty
  void empty()
  {
    require_auth(code_account);
    /*
    plan_table p(code_account, code_account);
    while (p.begin() != p.end())
    {
      auto itr = p.end();
      itr--;
      p.erase(itr);
      plan_table p(code_account, code_account);
    }
    order_table o(code_account, SCOPE_ORDER>>1);
    while (o.begin() != o.end())
    {
      auto itr = o.end();
      itr--;
      o.erase(itr);
      order_table o(code_account, SCOPE_ORDER>>1);
    }

    plan_table p(code_account, code_account);
    while (p.begin() != p.end())
    {
      auto itr = p.end();
      itr--;
      p.erase(itr);
      plan_table p(code_account, code_account);
    }

    creditor_table c(code_account, SCOPE_CREDITOR>>1);
    while (c.begin() != c.end())
    {
      auto itr = c.end();
      itr--;
      c.erase(itr);
      creditor_table c(code_account, SCOPE_CREDITOR>>1);
    }
    freelock_table c(code_account, SCOPE_FREELOCK>>1);
    while (c.begin() != c.end())
    {
      auto itr = c.end();
      itr--;
      c.erase(itr);
      freelock_table c(code_account, SCOPE_FREELOCK>>1);
    }
    */
  }


  // @abi action check
  void check()
  {
    require_auth(code_account);

    order_table o(code_account, SCOPE_ORDER>>1);
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
    print(117, " before undelegate | ");
    undelegate(order_ids, 0);
    print(119, " before expire freelock | ");
    expire_freelock();
    print(119, " before rotate creditor | ");
    rotate_creditor();
  }


  // @abi action expireorder
  void expireorder(uint64_t id)
  {
    require_auth(code_account);

    std::string content = "";
    order_table o(code_account, SCOPE_ORDER>>1);
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
    creditor_table c(code_account, SCOPE_CREDITOR>>1);
    auto creditor_itr = c.find(order->creditor);
    c.modify(creditor_itr, ram_payer, [&](auto &i) {
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
    history_table h(code_account, SCOPE_HISTORY>>1);
    h.emplace(ram_payer, [&](auto &i) {
      i.id = h.available_primary_key();
      i.content = content;
      i.created_at = now();
    });
  }

  // @abi action addwhitelist
  void addwhitelist(account_name account, uint64_t capacity)
  {
    require_auth(code_account);
    whitelist_table w(code_account, SCOPE_WHITELIST>>1);
    auto itr = w.find(account);
    if(itr == w.end()) {
      w.emplace(ram_payer, [&](auto &i) {
        i.account = account;
        i.capacity = capacity;
        i.created_at = now();
        i.updated_at = now();
      });
    } else {
      w.modify(itr, ram_payer, [&](auto &i) {
        i.capacity = capacity;
        i.updated_at = now();
      });
    }
  }

  // @abi action addcreditor
  void addcreditor(account_name account, uint64_t for_free, std::string free_memo)
  {
    require_auth(code_account);
    creditor_table c(code_account, SCOPE_CREDITOR>>1);
    auto itr = c.find(account);
    eosio_assert(itr == c.end(), "account already exist in creditor table");

    c.emplace(ram_payer, [&](auto &i) {
      i.is_active = FALSE;
      i.for_free = for_free?TRUE:FALSE;
      i.free_memo = for_free?free_memo:"";
      i.account = account;
      i.balance = get_balance(account);
      i.created_at = now();
      i.updated_at = now();
    });
  }


  // @abi action delcreditor
  void delcreditor(account_name account)
  {
    require_auth(code_account);
    creditor_table c(code_account, SCOPE_CREDITOR>>1);
    auto itr = c.find(account);
    eosio_assert(itr!= c.end(), "account not found in creditor table");
    eosio_assert(itr->is_active == FALSE, "cannot delete active creditor");
    //delelete creditor entry
    c.erase(itr);
  }


  // @abi action addblacklist
  void addblacklist(account_name account)
  {
    require_auth(code_account);
    blacklist_table b(code_account, SCOPE_BLACKLIST>>1);
    auto itr = b.find(account);
    eosio_assert(itr == b.end(), "account already exist in blacklist table");

    // add entry
    b.emplace(ram_payer, [&](auto &i) {
      i.account = account;
      i.created_at = now();
    });
  }


  // @abi action delblacklist
  void delblacklist(account_name account)
  {
    require_auth(code_account);
    blacklist_table b(code_account, SCOPE_BLACKLIST>>1);

    //make sure specified blacklist account exists
    auto itr = b.find(account);
    eosio_assert(itr!= b.end(), "account not found in blacklist table");
    //delelete entry
    b.erase(itr);
  }


  // @abi action activate
  void activate(account_name account)
  {
    require_auth(code_account);
    activate_creditor(account);
  }


  // @abi action setplan
  void setplan(asset price,
               asset cpu,
               asset net,
               uint64_t duration,
               bool is_free)
  {
    require_auth(code_account);
    validate_asset(price, cpu, net);
    plan_table p(code_account, code_account);
    auto idx = p.get_index<N(price)>();
    auto itr = idx.find(price.amount);
    if (itr == idx.end())
    {
      p.emplace(ram_payer, [&](auto &i) {
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
      idx.modify(itr, ram_payer, [&](auto &i) {
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
    require_auth(code_account);
    eosio_assert(price.is_valid(), "invalid price");
    plan_table p(code_account, code_account);
    auto idx = p.get_index<N(price)>();
    auto itr = idx.find(price.amount);
    eosio_assert(itr != idx.end(), "price not found");

    idx.modify(itr, ram_payer, [&](auto &i) {
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
          (addcreditor)
          (delcreditor)
          (addblacklist)
          (delblacklist)
          (activate)
          (check)
          (clearhistory));
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

    order_table o(code_account, SCOPE_ORDER>>1);
    plan_table p(code_account, code_account);

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
        permission_level{ code_account, N(bankperm) },
        code_account, N(expireorder),
        std::make_tuple(order_id)
      );
      out.actions.emplace_back(act2);

      //if order is_free is not free, transfer income to creditor
      if (order.is_free == FALSE)
      {
        auto plan = p.get(order.plan_id);

        auto username = name{order.creditor};
        std::string recipient_name = username.to_string();
        std::string memo = recipient_name + " bankofstaked income";
        action act3 = action(
          permission_level{ code_account, N(bankperm) },
          N(eosio.token), N(transfer),
          std::make_tuple(code_account, safe_transfer_account, order.price, memo)
        );
        out.actions.emplace_back(act3);
      }
    }

    if(duration > 0) {
      out.delay_sec = duration * SECONDS_PER_MIN;
    }
    out.send((uint128_t(code_account) << 64) | current_time() | nonce, code_account, true);
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
      plan_table p(code_account, code_account);
      auto idx = p.get_index<N(price)>();
      auto plan = idx.find(t.quantity.amount);
      eosio_assert(plan->is_active == TRUE, "plan is in-active");
      eosio_assert(plan != idx.end(), "invalid price");

      account_name beneficiary = get_beneficiary(t.memo, buyer);
      account_name eosio = string_to_name("eosio");

      // if plan is free, validate there is no Freelock for this beneficiary
      if(plan->is_free == TRUE)
      {
        validate_freelock(beneficiary);
      }

      //get creditor
      account_name creditor = get_active_creditor(plan->is_free);


      //validate buyer
      //1. buyer shouldnt be code_account
      //2. buyer shouldnt be in blacklist
      //3. each buyer could only have 5 affective orders at most
      validate_buyer(buyer);

      //validate beneficiary
      //1. beneficiary shouldnt be code_account
      //2. beneficiary shouldnt be in blacklist
      //3. each beneficiary could only have 5 affective orders at most
      validate_beneficiary(beneficiary, creditor);

      //INLINE ACTION to delegate CPU&NET for beneficiary account
      INLINE_ACTION_SENDER(eosiosystem::system_contract, delegatebw)
      (eosio, {{creditor, N(creditorperm)}}, {creditor, beneficiary, plan->net, plan->cpu, false});

      //INLINE ACTION to call check action of `bankofstaked`
      INLINE_ACTION_SENDER(bankofstaked, check)
      (code_account, {{code_account, N(bankperm)}}, {});

      // add cpu_staked&net_staked to creditor entry
      creditor_table c(code_account, SCOPE_CREDITOR>>1);
      auto creditor_itr = c.find(creditor);
      c.modify(creditor_itr, ram_payer, [&](auto &i) {
        i.cpu_staked += plan->cpu;
        i.net_staked += plan->net;
        i.balance = get_balance(creditor);
        i.updated_at = now();
      });

      //create Order entry
      uint64_t order_id;
      order_table o(code_account, SCOPE_ORDER>>1);
      o.emplace(ram_payer, [&](auto &i) {
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
        creditor_table c(code_account, SCOPE_CREDITOR>>1);
        std::string free_memo = c.get(creditor).free_memo;
        auto username = name{buyer};
        std::string buyer_name = username.to_string();
        std::string memo = buyer_name + " " + free_memo;
        INLINE_ACTION_SENDER(eosio::token, transfer)
        (N(eosio.token), {{code_account, N(bankperm)}}, {code_account, safe_transfer_account, plan->price, memo});
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
