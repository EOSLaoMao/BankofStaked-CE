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

  //get active creditor from creditor table
  account_name get_active_creditor(uint64_t for_free)
  {
    uint64_t active = TRUE;
    creditor_table c(code_account, SCOPE_CREDITOR>>1);
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
    // update creditor if balance is outdated
    creditor_table c(code_account, SCOPE_CREDITOR>>1);
    auto creditor_itr = c.find(owner);
    if(creditor_itr != c.end() && creditor_itr->balance != balance) {
      c.modify(creditor_itr, ram_payer, [&](auto &i) {
        i.balance = balance;
        i.updated_at = now();
      });
    }
    return balance;
  }

  void activate_creditor(account_name account)
  {
    creditor_table c(code_account, SCOPE_CREDITOR>>1);

    auto creditor = c.find(account);
    //make sure specified creditor exists
    eosio_assert(creditor != c.end(), "account not found in creditor table");

    //activate creditor, deactivate others
    auto itr = c.end();
    while (itr != c.begin())
    {
      itr--;
      if (itr->for_free != creditor->for_free)
      {
        continue;
      }

      if(itr->account==creditor->account) {
        c.modify(itr, ram_payer, [&](auto &i) {
          i.is_active = TRUE;
          i.balance = get_balance(itr->account);
          i.updated_at = now();
        });
      }
      else
      {
        if(itr->is_active == FALSE)
        {
           continue;
        }
        c.modify(itr, ram_payer, [&](auto &i) {
          i.is_active = FALSE;
          i.balance = get_balance(itr->account);
          i.updated_at = now();
        });
      }
    }
  }

  //get min paid creditor balance
  uint64_t get_min_paid_creditor_balance()
  {

    uint64_t balance = 10000 * 10000; // 10000 EOS
    plan_table p(code_account, code_account);
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

  //rotate active creditor
  void rotate_creditor()
  {
    creditor_table c(code_account, SCOPE_CREDITOR>>1);
    auto free_creditor = get_active_creditor(TRUE);
    auto paid_creditor = get_active_creditor(FALSE);

    asset free_balance = get_balance(free_creditor);
    asset paid_balance = get_balance(paid_creditor);
    uint64_t min_paid_creditor_balance = get_min_paid_creditor_balance();
    //uint64_t min_paid_creditor_balance = MIN_FREE_CREDITOR_BALANCE;
    auto free_rotated = free_balance.amount > MIN_FREE_CREDITOR_BALANCE ?TRUE:FALSE;
    auto paid_rotated = paid_balance.amount > min_paid_creditor_balance ?TRUE:FALSE;
    auto idx = c.get_index<N(updated_at)>();
    auto itr = idx.begin();
    auto count = 0;
    while (itr != idx.end())
    {
      count += 1;
      auto username = name{itr->account};
      std::string from_name = username.to_string();
      if(itr->for_free == TRUE)
      {
        if(free_rotated == TRUE){itr++;continue;}
        auto balance = get_balance(itr->account);
        if (itr->account != free_creditor && balance.amount >= MIN_FREE_CREDITOR_BALANCE)
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
        if (itr->account != paid_creditor && balance.amount >= min_paid_creditor_balance)
        {
          activate_creditor(itr->account);
          paid_rotated = TRUE;
        }
        itr++;
      }
    }
  }
}
