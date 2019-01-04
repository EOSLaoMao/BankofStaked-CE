#include <boost/test/unit_test.hpp>
#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include "contracts.hpp"
#include "eosio.system_tester.hpp"

#include "Runtime/Runtime.h"

#include <fc/variant_object.hpp>

using namespace eosio::testing;
using namespace eosio;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;
using namespace std;
using namespace eosio_system;

using mvo = fc::mutable_variant_object;

class bankofstaked_tester : public eosio_system_tester
{
  public:
    bankofstaked_tester()
    {
        produce_blocks();

        eosio_system_tester::create_account_with_resources( N(bankofstaked), config::system_account_name, core_sym::from_string("100.0000"), false );
        eosio_system_tester::create_account_with_resources(
            N(alice.111111),
            config::system_account_name,
            core_sym::from_string("10.0000"),
            false,
            core_sym::from_string("10.0000"),
            core_sym::from_string("10.0000")
        );
        eosio_system_tester::create_account_with_resources(
            N(bob.11111111),
            config::system_account_name,
            core_sym::from_string("10.0000"),
            false,
            core_sym::from_string("10.0000"),
            core_sym::from_string("10.0000")
        );
        eosio_system_tester::create_account_with_resources(
            N(carol.111111),
            config::system_account_name,
            core_sym::from_string("10.0000"),
            false,
            core_sym::from_string("10.0000"),
            core_sym::from_string("10.0000")
        );

        eosio_system_tester::issue(N(bankofstaked), core_sym::from_string("50.0000"));
        eosio_system_tester::issue(N(alice.111111), core_sym::from_string("500.0000"));
        eosio_system_tester::issue(N(bob.11111111), core_sym::from_string("5000.0000"));
        eosio_system_tester::issue(N(carol.111111), core_sym::from_string("50000.0000"));

        produce_blocks();

        authority auth;
        fc::from_variant(
            fc::json::from_string(R"({"threshold": 1,"keys":[],"accounts": [{"permission":{"actor":"bankofstaked","permission":"eosio.code"},"weight":1}], "waits":[]})"),
            auth
        );

        set_authority(N(bankofstaked), N(bankperm), auth, config::active_name);
        link_authority(N(bankofstaked), N(eosio.token), N(bankperm), N(transfer));
        link_authority(N(bankofstaked), N(eosio), N(bankperm), N(delegatebw));
        link_authority(N(bankofstaked), N(eosio), N(bankperm), N(undelegatebw));
        link_authority(N(bankofstaked), N(bankofstaked), N(bankperm), N(expireorder));
        link_authority(N(bankofstaked), N(bankofstaked), N(bankperm), N(check));
        link_authority(N(bankofstaked), N(bankofstaked), N(bankperm), N(rotate));

        set_authority(N(bob.11111111), N(creditorperm), auth, config::active_name);
        set_authority(N(carol.111111), N(creditorperm), auth, config::active_name);

        link_authority(N(bob.11111111), N(eosio), N(creditorperm), N(delegatebw));
        link_authority(N(carol.111111), N(eosio), N(creditorperm), N(delegatebw));
        link_authority(N(bob.11111111), N(eosio), N(creditorperm), N(undelegatebw));
        link_authority(N(carol.111111), N(eosio), N(creditorperm), N(undelegatebw));

        produce_blocks();

        set_code(N(bankofstaked), contracts::bank_wasm());
        set_abi(N(bankofstaked), contracts::bank_abi().data());
        produce_blocks();

        const auto &accnt = control->db().get<account_object, by_name>(N(bankofstaked));
        abi_def bank_abi;
        BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, bank_abi), true);
        bank_abi_ser.set_abi(bank_abi, abi_serializer_max_time);
    }

    transaction_trace_ptr push_action(const account_name &signer, const action_name &name, const variant_object &data, bool auth = true)
    {
        vector<account_name> accounts;
        if (auth)
            accounts.push_back(signer);
        auto trace = base_tester::push_action(N(bankofstaked), name, accounts, data);
        produce_block();
        BOOST_REQUIRE_EQUAL(true, chain_has_transaction(trace->id));
        return trace;
    }

    fc::variant get_creditor(const account_name &act)
    {
        vector<char> data = get_row_by_account(N(bankofstaked), 921459758687, N(creditor), act);
        return data.empty() ? EMPTY : bank_abi_ser.binary_to_variant("creditor", data, abi_serializer_max_time);
    }

    fc::variant get_safecreditor(const account_name &act)
    {
        vector<char> data = get_row_by_account(N(bankofstaked), 921459758687, N(safecreditor), act);
        return data.empty() ? EMPTY : bank_abi_ser.binary_to_variant("safecreditor", data, abi_serializer_max_time);
    }

    fc::variant get_blacklist(const account_name &act)
    {
        vector<char> data = get_row_by_account(N(bankofstaked), 921459758687, N(blacklist), act);
        return data.empty() ? EMPTY : bank_abi_ser.binary_to_variant("blacklist", data, abi_serializer_max_time);
    }

    fc::variant get_whitelist(const account_name &act)
    {
        vector<char> data = get_row_by_account(N(bankofstaked), 921459758687, N(whitelist), act);
        return data.empty() ? EMPTY : bank_abi_ser.binary_to_variant("whitelist", data, abi_serializer_max_time);
    }

    fc::variant get_plan(const asset& price)
    {
        vector<char> data = get_row_by_secondary_key(N(bankofstaked), N(bankofstaked), N(plan), (uint64_t)price.get_amount());
        return data.empty() ? EMPTY : bank_abi_ser.binary_to_variant("plan", data, abi_serializer_max_time);
    }

    fc::variant get_order(account_name buyer)
    {
        vector<char> data = get_row_by_secondary_key(N(bankofstaked), 921459758687, N(order), buyer.value);
        return data.empty() ? EMPTY : bank_abi_ser.binary_to_variant("order", data, abi_serializer_max_time);
    }

    fc::variant get_account(account_name acc, const string &symbolname)
    {
        auto symb = eosio::chain::symbol::from_string(symbolname);
        auto symbol_code = symb.to_symbol_code().value;
        vector<char> data = get_row_by_account(N(eosio.token), acc, N(accounts), symbol_code);
        return data.empty() ? EMPTY : eosio_system_tester::token_abi_ser.binary_to_variant("account", data, abi_serializer_max_time);
    }

    vector<char> get_row_by_secondary_key( uint64_t code, uint64_t scope, uint64_t table, uint64_t key ) const {
        vector<char> data;
        const auto& db = control->db();
        const auto* t_id = db.find<chain::table_id_object, chain::by_code_scope_table>( boost::make_tuple( code, scope, table ) );
        if ( !t_id ) {
            return data;
        }

        const auto& idx = db.get_index<index64_index>().indices().get<by_secondary>();

        auto itr = idx.find( boost::make_tuple( t_id->id, key ) );
        if (itr != idx.end()) {
            const auto *obj = db.find<key_value_object, by_scope_primary>(boost::make_tuple(t_id->id, itr->primary_key));
            if (obj) {
                data.resize( obj->value.size() );
                memcpy( data.data(), obj->value.data(), data.size() );
            }
        }

        return data;
    }

    // abi_serializer token_abi_ser;
    abi_serializer bank_abi_ser;
    fc::variant EMPTY = fc::variant(0).as_string().substr(0,8);
};
