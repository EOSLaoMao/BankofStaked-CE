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
  account_name get_active_creditor()
  {
    uint64_t active = TRUE;
    creditor_table c(code_account, SCOPE_CREDITOR>>1);
    auto idx = c.get_index<N(is_active)>();
    auto creditor = idx.get(active);
    return creditor.account;
  }

  //get account EOS balance
  asset get_balance(account_name owner)
  {
    auto symbol = symbol_type(system_token_symbol);
    eosio::token t(N(eosio.token));
    return t.get_balance(owner, symbol.name());
  }
}
