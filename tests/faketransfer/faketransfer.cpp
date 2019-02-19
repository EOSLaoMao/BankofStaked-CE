#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>

namespace play
{

using namespace std;
using namespace eosio;

class[[eosio::contract("faketransfer")]] faketransfer : public contract
{
  public:
    using contract::contract;

    [[eosio::action]] void transfer(name from, name to, asset quantity, string memo) {
        require_recipient("bankofstaked"_n);
    }

};

} // namespace play

extern "C" { 
   void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
        switch( action ) {
            EOSIO_DISPATCH_HELPER( play::faketransfer, (transfer) )
        }
   }
}
