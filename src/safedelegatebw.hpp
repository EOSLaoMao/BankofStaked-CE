#include <eosiolib/eosio.hpp>
#include <eosio.system/eosio.system.hpp>
#include <eosio.token/eosio.token.hpp>

using namespace eosio;
using namespace eosiosystem;
using std::string;

class [[eosio::contract]] safedelegatebw : contract {
public:
    using contract::contract;

    [[eosio::action]]
    void delegatebw(name to,
                    asset net_weight,
                    asset cpu_weight){

      require_auth(_self);

      INLINE_ACTION_SENDER(eosiosystem::system_contract, delegatebw)
      ("eosio"_n, {{_self, "delegateperm"_n}}, {_self, to, net_weight, cpu_weight, false});
    }
};
