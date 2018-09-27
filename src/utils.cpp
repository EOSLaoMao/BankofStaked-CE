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
    return t.get_balance(owner, symbol.name());
  }

  //rotate active creditor
  void rotate_creditor()
  {
    creditor_table c(code_account, SCOPE_CREDITOR>>1);
    auto free_creditor = get_active_creditor(TRUE);
    auto paid_creditor = get_active_creditor(FALSE);

    auto idx = c.get_index<N(updated_at)>();
    auto itr = idx.begin();
    asset free_balance = get_balance(free_creditor);
    asset paid_balance = get_balance(free_creditor);
    auto free_rotated = free_balance.amount > MIN_CREDITOR_BALANCE ?TRUE:FALSE;
    auto paid_rotated = paid_balance.amount > MIN_CREDITOR_BALANCE ?TRUE:FALSE;

    while (itr != idx.end())
    {
      if(itr->for_free == TRUE)
      {
        if(free_rotated == TRUE){itr++;continue;}
        if (itr->account != free_creditor)
        {
          idx.modify(itr, ram_payer, [&](auto &i) {
            i.is_active = TRUE;
            i.balance = get_balance(itr->account);
            i.updated_at = now();
          });
          free_rotated = TRUE;
        }
      }
      else
      {
        if(paid_rotated == TRUE){itr++;continue;}
        if (itr->account != paid_creditor)
        {
          idx.modify(itr, ram_payer, [&](auto &i) {
            i.is_active = TRUE;
            i.balance = get_balance(itr->account);
            i.updated_at = now();
          });
          paid_rotated = TRUE;
        }
      }
      
    }
  }
}
