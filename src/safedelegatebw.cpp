#include <eosiolib/eosio.hpp>
#include <eosiolib/currency.hpp>
#include <eosio.system/eosio.system.hpp>
#include <eosio.token/eosio.token.hpp>

using namespace eosio;
using namespace eosiosystem;
using std::string;

class safedelegatebw : contract {
public:
    using contract::contract;
    safedelegatebw( name self ) : contract(self){}

    // @abi action delegatebw
    void delegatebw(account_name to,
                    asset net_weight,
                    asset cpu_weight){

      require_auth(_self);

      INLINE_ACTION_SENDER(eosiosystem::system_contract, delegatebw)
      (N(eosio), {{_self, N(delegateperm)}}, {_self, to, net_weight, cpu_weight, false});
    }
};
