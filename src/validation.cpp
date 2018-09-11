using namespace eosio;
using namespace eosiosystem;
using namespace bank;
using namespace utils;

namespace validation
{
  // check freelock
  void validate_freelock(account_name beneficiary)
  {
    freelock_table f(code_account, SCOPE_FREELOCK>>1);
    auto itr = f.find(beneficiary);
    eosio_assert(itr == f.end(), "free plan is avaliable every 24 hours");
  }

  // check blacklist
  void validate_blacklist(account_name account)
  {
    blacklist_table b(code_account, SCOPE_BLACKLIST>>1);
    auto itr = b.find(account);
    eosio_assert(itr == b.end(), "something wrong with your account");
  }

  //make sure BUYER's affective records is no more than MAX_PAID_ORDERS
  void validate_buyer(account_name buyer)
  {
    eosio_assert(buyer != code_account, "buyer cannot be bankofstaked");

    //validate blacklist
    validate_blacklist(buyer);

    order_table o(code_account, SCOPE_ORDER>>1);
    auto idx = o.get_index<N(buyer)>();
    auto first = idx.lower_bound(buyer);
    auto last = idx.upper_bound(buyer);
    uint64_t count = 0;
    for(;first!=last;++first)
    {
      count += 1;
    }
    eosio_assert(count < MAX_PAID_ORDERS, "5 affective orders per buyer at most");
  }


  //make sure BENEFICIARY's affective records is no more than MAX_PAID_ORDERS
  void validate_beneficiary(account_name beneficiary, account_name creditor)
  {
    eosio_assert(beneficiary != code_account, "cannot delegate to bankofstaked");
    eosio_assert(beneficiary != creditor, "cannot delegate to creditor");

    //validate blacklist
    validate_blacklist(beneficiary);

    //make sure the account has less than MAX_BALANCE EOS in balance
    //disabled temperarily because of account not found issue
    /*
    auto balance = get_balance(beneficiary);
    print("balance:", balance.amount);
    eosio_assert(balance.amount<MAX_EOS_BALANCE, "beneficiary should have no more than 500 EOS");
    */

    order_table o(code_account, SCOPE_ORDER>>1);
    auto idx = o.get_index<N(beneficiary)>();
    auto first = idx.lower_bound(beneficiary);
    auto last = idx.upper_bound(beneficiary);
    uint64_t count = 0;
    for(;first!=last;++first)
    {
      count += 1;
    }
    eosio_assert(count < MAX_PAID_ORDERS, "5 affective orders per beneficiary at most");
  }

  //validate Plan asset fields
  void validate_asset(asset price,
                      asset cpu,
                      asset net)
  {
    eosio_assert(price.is_valid(), "invalid price");
    eosio_assert(cpu.is_valid(), "invalid cpu");
    eosio_assert(net.is_valid(), "invalid net");
    eosio_assert(price.amount >= 100 && price.amount <= 1000000, "price should between 0.01 EOS and 100 EOS");
  }
}
