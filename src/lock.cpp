using namespace eosio;
using namespace eosiosystem;
using namespace bank;

namespace lock
{
  //add freelock entry
  void add_freelock(account_name beneficiary)
  {
    freelock_table f(CODE_ACCOUNT, SCOPE);
    f.emplace(RAM_PAYER, [&](auto &i) {
      i.beneficiary = beneficiary;
      i.created_at = now();
      i.expire_at = i.created_at + SECONDS_PER_DAY;
    });
  }

  //delete expired freelock entries
  void expire_freelock()
  {
    uint64_t depth = 0;
    uint64_t n = now();
    freelock_table f(CODE_ACCOUNT, SCOPE);
    auto idx = f.get_index<N(expire_at)>();
    auto last = idx.upper_bound(n);
    auto itr = idx.lower_bound(0);
    while(itr!=last && depth < CHECK_MAX_DEPTH)
    {
      idx.erase(itr);
      itr = idx.lower_bound(0);
      depth += 1;
    }
  }
}
