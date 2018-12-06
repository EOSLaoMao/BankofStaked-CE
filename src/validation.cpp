using namespace eosio;
using namespace eosiosystem;
using namespace bank;
using namespace utils;

namespace validation
{
  // check freelock
  void validate_freelock(account_name beneficiary)
  {
    freelock_table f(CODE_ACCOUNT, SCOPE);
    auto itr = f.find(beneficiary);
    eosio_assert(itr == f.end(), "free plan is avaliable every 24 hours for each beneficiary");
  }

  // check blacklist
  void validate_blacklist(account_name account)
  {
    blacklist_table b(CODE_ACCOUNT, SCOPE);
    auto itr = b.find(account);
    eosio_assert(itr == b.end(), "something wrong with your account");
  }

  //get BUYER's free order amount limit
  uint64_t get_free_order_cap(account_name buyer)
  {
    uint64_t max_orders = MAX_FREE_ORDERS;
    whitelist_table w(CODE_ACCOUNT, SCOPE);
    auto itr = w.find(buyer);
    if(itr != w.end())
    {
       max_orders = itr->capacity;
    }
    return max_orders;
  }

  //make sure BUYER's affective records is no more than get_free_order_cap(BUYER)
  void validate_buyer(account_name buyer, uint64_t is_free)
  {
    eosio_assert(buyer != CODE_ACCOUNT, "buyer cannot be bankofstaked");

    //validate blacklist
    validate_blacklist(buyer);

    // for paid orders, check MAX_PAID_ORDERS
    // for free orders, check get_free_order_cap()
    uint64_t max_orders = MAX_PAID_ORDERS;
    if(is_free == TRUE) {
      max_orders = get_free_order_cap(buyer);
    }
    std::string suffix = " affective orders at most for each buyer";
    std::string error_msg = std::to_string(max_orders) + suffix;

    order_table o(CODE_ACCOUNT, SCOPE);
    auto idx = o.get_index<N(buyer)>();
    auto first = idx.lower_bound(buyer);
    auto last = idx.upper_bound(buyer);
    uint64_t count = 0;
    while(first != last && first != idx.end())
    {
      if(first->is_free == is_free)
      {
        count += 1;
      }
      first++;
    }
    eosio_assert(count < max_orders, error_msg.c_str());
  }

  //make sure BENEFICIARY's affective free orders is no more than MAX_FREE_ORDERS
  void validate_beneficiary(account_name beneficiary, account_name creditor, uint64_t is_free)
  {
    eosio_assert(beneficiary != CODE_ACCOUNT, "cannot delegate to bankofstaked");
    eosio_assert(beneficiary != creditor, "cannot delegate to creditor");

    //validate blacklist
    validate_blacklist(beneficiary);

    // for paid orders, check MAX_PAID_ORDERS
    // for free orders, check MAX_FREE_ORDERS
    uint64_t max_orders = MAX_PAID_ORDERS;
    if(is_free == TRUE) {
      max_orders = MAX_FREE_ORDERS;
    }

    //make sure the account has less than MAX_BALANCE EOS in balance
    //disabled temperarily because of account not found issue
    /*
    auto balance = get_balance(beneficiary);
    print("balance:", balance.amount);
    eosio_assert(balance.amount<MAX_EOS_BALANCE, "beneficiary should have no more than 500 EOS");
    */

    order_table o(CODE_ACCOUNT, SCOPE);
    auto idx = o.get_index<N(beneficiary)>();
    auto first = idx.lower_bound(beneficiary);
    auto last = idx.upper_bound(beneficiary);
    uint64_t count = 0;
    while(first != last && first != idx.end())
    {
      if(first->is_free == is_free)
      {
        count += 1;
      }
      first++;
    }
    std::string suffix = " affective orders at most for each beneficiary";
    std::string error_msg = std::to_string(max_orders) + suffix;
    eosio_assert(count < max_orders, error_msg.c_str());
  }


  //validate Plan asset fields
  void validate_asset(asset price,
                      asset cpu,
                      asset net)
  {
    eosio_assert(price.is_valid(), "invalid price");
    eosio_assert(cpu.is_valid(), "invalid cpu");
    eosio_assert(net.is_valid(), "invalid net");
    eosio_assert(price.amount >= 100 && price.amount <= 10000000, "price should between 0.01 EOS and 1000 EOS");
  }

  //validate account exist in creditor table
  void validate_creditor(account_name creditor)
  {
    creditor_table c(CODE_ACCOUNT, SCOPE);
    auto itr = c.find(creditor);
    eosio_assert(itr != c.end(), "account does not exist in creditor table");
  }
}
