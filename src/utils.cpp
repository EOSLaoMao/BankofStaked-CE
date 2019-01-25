using namespace eosio;
using namespace eosiosystem;
using namespace bank;

namespace utils
{
  //try to get beneficiary from memo, otherwise, use sender.
  account_name get_beneficiary(const std::string &memo, account_name sender)
  {
    account_name to = sender;
    if (memo.length() > 0)
    {
      to = string_to_name(memo.c_str());
      eosio_assert( is_account( to ), "to account does not exist");
    }
    return to;
  }

  //get income recipient for creditor
  account_name get_recipient(account_name creditor)
  {
    recipient_table i(CODE_ACCOUNT, CODE_ACCOUNT);
    account_name recipient = creditor;
    auto itr = i.find(creditor);
    if(itr != i.end()) {
        recipient = itr->recipient_account;
    }
    eosio_assert( is_account( recipient ), "recipient account does not exist");
    return recipient;
  }

  //get active creditor from creditor table
  account_name get_active_creditor(uint64_t for_free)
  {
    uint64_t active = TRUE;
    creditor_table c(CODE_ACCOUNT, SCOPE);
    auto idx = c.get_index<N(is_active)>();
    auto itr = idx.begin();
    account_name creditor;
    while (itr != idx.end())
    {
      if(itr->is_active != TRUE)
      {
         itr++;
         continue;
      }

      if(itr->for_free == for_free) {
        creditor = itr->account;
        break;
      }
      itr++;
    }
    return creditor;
  }

  //get account EOS balance
  asset get_balance(account_name owner)
  {
    auto symbol = symbol_type(system_token_symbol);
    eosio::token t(N(eosio.token));
    auto balance = t.get_balance(owner, symbol.name());
    return balance;
  }

  //get account EOS balance
  asset update_balance(account_name owner)
  {
    auto balance = get_balance(owner);
    // update creditor if update is true
    creditor_table c(CODE_ACCOUNT, SCOPE);
    auto creditor_itr = c.find(owner);
    if(creditor_itr != c.end() && creditor_itr->balance != balance) {
      c.modify(creditor_itr, RAM_PAYER, [&](auto &i) {
        i.balance = balance;
        i.updated_at = now();
      });
    }
    return balance;
  }



  //get creditor with balance >= to_delegate
  account_name get_qualified_paid_creditor(asset to_delegate)
  {
    uint64_t active = TRUE;
    creditor_table c(CODE_ACCOUNT, SCOPE);
    auto idx = c.get_index<N(is_active)>();
    auto itr = idx.begin();
    account_name creditor;
    while (itr != idx.end())
    {
      asset balance = get_balance(itr->account);
      if(itr->for_free == FALSE && balance >= to_delegate) {
        creditor = itr->account;
        break;
      }
      itr++;
    }
    return creditor;
  }

  //get creditor income
  asset get_income(account_name creditor, asset price)
  {
    dividend_table c(CODE_ACCOUNT, CODE_ACCOUNT);
    uint64_t amount = price.amount;
    auto itr = c.find(creditor);
    if(itr != c.end()) {
        price.amount = amount * itr->percentage / 100;
    } else {
        price.amount = amount * DEFAULT_DIVIDEND_PERCENTAGE / 100;
    }
    return price;
  }

  void activate_creditor(account_name account)
  {
    creditor_table c(CODE_ACCOUNT, SCOPE);

    auto creditor = c.find(account);
    //make sure specified creditor exists
    eosio_assert(creditor != c.end(), "account not found in creditor table");

    eosio::transaction out;
    //activate creditor, deactivate others
    auto itr = c.end();
    while (itr != c.begin())
    {
      itr--;
      if (itr->for_free != creditor->for_free) {
        continue;
      }

      if(itr->account==creditor->account) {
        c.modify(itr, RAM_PAYER, [&](auto &i) {
          i.is_active = TRUE;
          i.balance = get_balance(itr->account);
          i.updated_at = now();
        });
        action act1 = action(
          permission_level{ CODE_ACCOUNT, N(bankperm) },
          CODE_ACCOUNT,
          N(rotate),
          std::make_tuple(itr->account, itr->for_free)
        );
        out.actions.emplace_back(act1);
      } else {
        if(itr->is_active == FALSE) {
           continue;
        }
        c.modify(itr, RAM_PAYER, [&](auto &i) {
          i.is_active = FALSE;
          i.balance = get_balance(itr->account);
          i.updated_at = now();
        });
      }
    }
    out.send((uint128_t(CODE_ACCOUNT) << 64) | current_time(), CODE_ACCOUNT, true);
  }

  //get min paid creditor balance
  uint64_t get_min_paid_creditor_balance()
  {

    uint64_t balance = 10000 * 10000; // 10000 EOS
    plan_table p(CODE_ACCOUNT, CODE_ACCOUNT);
    eosio_assert(p.begin() != p.end(), "plan table is empty!");
    auto itr = p.begin();
    while (itr != p.end())
    {
      auto required = itr->cpu.amount + itr->net.amount;
      if (itr->is_free == false && itr->is_active && required < balance) {
        balance = required;
      }
      itr++;
    }
    return balance;
  }

  //check creditor enabled safedelegate or not
  bool is_safe_creditor(account_name creditor)
  {
    safecreditor_table s(CODE_ACCOUNT, SCOPE);
    auto itr = s.find(creditor);
    if(itr == s.end()){
      return false;
    } else {
      return true;
    }
  }

  //rotate active creditor
  void rotate_creditor()
  {
    creditor_table c(CODE_ACCOUNT, SCOPE);
    auto free_creditor = get_active_creditor(TRUE);
    auto paid_creditor = get_active_creditor(FALSE);

    asset free_balance = get_balance(free_creditor);
    asset paid_balance = get_balance(paid_creditor);
    uint64_t min_paid_creditor_balance = get_min_paid_creditor_balance();
    auto free_rotated = free_balance.amount > MIN_FREE_CREDITOR_BALANCE ?TRUE:FALSE;
    auto paid_rotated = paid_balance.amount > min_paid_creditor_balance ?TRUE:FALSE;
    auto idx = c.get_index<N(updated_at)>();
    auto itr = idx.begin();
    while (itr != idx.end())
    {
      if(itr->for_free == TRUE)
      {
        if(free_rotated == TRUE){itr++;continue;}
        auto balance = get_balance(itr->account);
        if (itr->account != free_creditor && balance.amount > MIN_FREE_CREDITOR_BALANCE)
        {
          activate_creditor(itr->account);
          free_rotated = TRUE;
        }
        itr++;
      }
      else
      {
        if(paid_rotated == TRUE){itr++;continue;}
        auto balance = get_balance(itr->account);
        if (itr->account != paid_creditor && balance.amount > min_paid_creditor_balance)
        {
          activate_creditor(itr->account);
          paid_rotated = TRUE;
        }
        itr++;
      }
    }
  }
}
