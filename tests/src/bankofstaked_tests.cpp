#include <boost/test/unit_test.hpp>
#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include "contracts.hpp"

#include "Runtime/Runtime.h"

#include <fc/variant_object.hpp>

using namespace eosio::testing;
using namespace eosio;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;
using namespace std;

using mvo = fc::mutable_variant_object;

class bankofstaked_tester : public tester
{
  public:
    bankofstaked_tester()
    {
       produce_blocks(2);

        create_accounts({N(alice), N(bob), N(carol), N(eosio.token), N(bankofstaked)});
        produce_blocks(2);

        set_code(N(eosio.token), contracts::token_wasm());
        set_abi(N(eosio.token), contracts::token_abi().data());

        const auto &t = control->db().get<account_object, by_name>(N(eosio.token));
        abi_def abi;
        BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(t.abi, abi), true);
        token_abi_ser.set_abi(abi, abi_serializer_max_time);

        set_code(N(bankofstaked), contracts::bank_wasm());
        set_abi(N(bankofstaked), contracts::bank_abi().data());

        auto token = create(N(alice), asset::from_string("10000000.0000 EOS"));
        produce_blocks(1);
        issue(N(alice), N(alice), asset::from_string("500.0000 EOS"), "hola");
        issue(N(alice), N(bob), asset::from_string("5000.0000 EOS"), "hola");
        issue(N(alice), N(carol), asset::from_string("50000.0000 EOS"), "hola");
        produce_blocks(1);

        const auto &accnt = control->db().get<account_object, by_name>(N(bankofstaked));

        abi_def bank_abi;
        BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, bank_abi), true);
        abi_ser.set_abi(bank_abi, abi_serializer_max_time);
    }

    action_result create(account_name issuer,
                         asset maximum_supply)
    {

        return push_token_action(N(eosio.token), N(create), mvo()("issuer", issuer)("maximum_supply", maximum_supply));
    }

    action_result issue(account_name issuer, account_name to, asset quantity, string memo)
    {
        return push_token_action(issuer, N(issue), mvo()("to", to)("quantity", quantity)("memo", memo));
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

    action_result push_token_action(const account_name &signer, const action_name &name, const variant_object &data)
    {
        string action_type_name = token_abi_ser.get_action_type(name);

        action act;
        act.account = N(eosio.token);
        act.name = name;
        act.data = token_abi_ser.variant_to_binary(action_type_name, data, abi_serializer_max_time);

        return base_tester::push_action(std::move(act), uint64_t(signer));
    }

    action_result transfer(account_name from,
                           account_name to,
                           asset quantity,
                           string memo)
    {
        return push_token_action(from, N(transfer), mvo()("from", from)("to", to)("quantity", quantity)("memo", memo));
    }

    fc::variant get_creditor(const account_name &act)
    {
        vector<char> data = get_row_by_account(N(bankofstaked), 921459758687, N(creditor), act);
        return data.empty() ? EMPTY : abi_ser.binary_to_variant("creditor", data, abi_serializer_max_time);
    }

    fc::variant get_blacklist(const account_name &act)
    {
        vector<char> data = get_row_by_account(N(bankofstaked), 921459758687, N(blacklist), act);
        return data.empty() ? EMPTY : abi_ser.binary_to_variant("blacklist", data, abi_serializer_max_time);
    }

    fc::variant get_whitelist(const account_name &act)
    {
        vector<char> data = get_row_by_account(N(bankofstaked), 921459758687, N(whitelist), act);
        return data.empty() ? EMPTY : abi_ser.binary_to_variant("whitelist", data, abi_serializer_max_time);
    }

    fc::variant get_account(account_name acc, const string &symbolname)
    {
        auto symb = eosio::chain::symbol::from_string(symbolname);
        auto symbol_code = symb.to_symbol_code().value;
        vector<char> data = get_row_by_account(N(eosio.token), acc, N(accounts), symbol_code);
        return data.empty() ? EMPTY : token_abi_ser.binary_to_variant("account", data, abi_serializer_max_time);
    }


    vector<char> get_row_by_account(uint64_t code, uint64_t scope, uint64_t table, const account_name &act) const
    {
        vector<char> data;
        const auto &db = control->db();
        const auto *t_id = db.find<chain::table_id_object, chain::by_code_scope_table>(boost::make_tuple(code, scope, table));
        if (!t_id)
        {
            return data;
        }
        //FC_ASSERT( t_id != 0, "object not found" );

        const auto &idx = db.get_index<chain::key_value_index, chain::by_scope_primary>();

        auto itr = idx.lower_bound(boost::make_tuple(t_id->id, act));
        if (itr == idx.end() || itr->t_id != t_id->id || act.value != itr->primary_key)
        {
            return data;
        }

        data.resize(itr->value.size());
        memcpy(data.data(), itr->value.data(), data.size());
        return data;
    }

    vector<char> get_row_by_creditor(uint64_t code, uint64_t scope, uint64_t table) const
    {
        vector<char> data;
        const auto &db = control->db();
        const auto *t_id = db.find<chain::table_id_object, chain::by_code_scope_table>(boost::make_tuple(code, scope, table));
        if (!t_id)
        {
            return data;
        }

        const auto &idx = db.get_index<chain::key_value_index, chain::by_scope_primary>();

        auto itr = idx.lower_bound(boost::make_tuple(t_id->id));
        if (itr == idx.end() || itr->t_id != t_id->id)
        {
            return data;
        }

        data.resize(itr->value.size());
        memcpy(data.data(), itr->value.data(), data.size());
        return data;
    }

    abi_serializer token_abi_ser;
    abi_serializer abi_ser;
    fc::variant EMPTY = fc::variant(0).as_string().substr(0,8);
};

BOOST_AUTO_TEST_SUITE(bankofstaked_tests)

BOOST_FIXTURE_TEST_CASE(addcreditor_test, bankofstaked_tester)
try
{
    // add 3 creditors, alice/bob/carol
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "alice")("for_free", 1)("free_memo", "lucky you!"), config::active_name);
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "bob")("for_free", 0)("free_memo", "hell yeah!"), config::active_name);
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "carol")("for_free", 1)("free_memo", "oh"), config::active_name);

    auto creditor = get_creditor("alice");
    BOOST_REQUIRE_EQUAL(creditor["is_active"], 0);
    BOOST_REQUIRE_EQUAL(creditor["for_free"], 1);
    BOOST_REQUIRE_EQUAL(creditor["free_memo"], "lucky you!");
    BOOST_REQUIRE_EQUAL(creditor["account"], "alice");
    BOOST_REQUIRE_EQUAL(creditor["balance"], "500.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["cpu_staked"], "0.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["net_staked"], "0.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["cpu_unstaked"], "0.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["net_unstaked"], "0.0000 EOS");

    creditor = get_creditor("bob");
    BOOST_REQUIRE_EQUAL(creditor["is_active"], 0);
    BOOST_REQUIRE_EQUAL(creditor["for_free"], 0);
    BOOST_REQUIRE_EQUAL(creditor["free_memo"], "");
    BOOST_REQUIRE_EQUAL(creditor["account"], "bob");
    BOOST_REQUIRE_EQUAL(creditor["balance"], "5000.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["cpu_staked"], "0.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["net_staked"], "0.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["cpu_unstaked"], "0.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["net_unstaked"], "0.0000 EOS");

    creditor = get_creditor("carol");
    BOOST_REQUIRE_EQUAL(creditor["is_active"], 0);
    BOOST_REQUIRE_EQUAL(creditor["for_free"], 1);
    BOOST_REQUIRE_EQUAL(creditor["free_memo"], "oh");
    BOOST_REQUIRE_EQUAL(creditor["account"], "carol");
    BOOST_REQUIRE_EQUAL(creditor["balance"], "50000.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["cpu_staked"], "0.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["net_staked"], "0.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["cpu_unstaked"], "0.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["net_unstaked"], "0.0000 EOS");
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(activate_test, bankofstaked_tester)
try
{
    // add 3 creditors, alice/bob/carol
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "alice")("for_free", 0)("free_memo", ""), config::active_name);
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "bob")("for_free", 0)("free_memo", ""), config::active_name);
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "carol")("for_free", 0)("free_memo", ""), config::active_name);
    auto creditor = get_creditor("alice");
    BOOST_REQUIRE_EQUAL(creditor["is_active"], 0);
    creditor = get_creditor("bob");
    BOOST_REQUIRE_EQUAL(creditor["is_active"], 0);
    creditor = get_creditor("carol");
    BOOST_REQUIRE_EQUAL(creditor["is_active"], 0);

    //set bob as active creditor
    push_action(N(bankofstaked), N(activate), mvo()("account", "bob"), config::active_name);
    creditor = get_creditor("alice");
    BOOST_REQUIRE_EQUAL(creditor["is_active"], 0);
    creditor = get_creditor("bob");
    BOOST_REQUIRE_EQUAL(creditor["is_active"], 1);
    creditor = get_creditor("carol");
    BOOST_REQUIRE_EQUAL(creditor["is_active"], 0);
}
FC_LOG_AND_RETHROW()

// test action delcreditor
BOOST_FIXTURE_TEST_CASE(delcreditor_test, bankofstaked_tester)
try
{
    // add 2 creditors, alice/bob
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "alice")("for_free", 0)("free_memo", ""), config::active_name);
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "bob")("for_free", 0)("free_memo", ""), config::active_name);
    auto creditor = get_creditor("alice");
    BOOST_REQUIRE_EQUAL(creditor["is_active"], 0);
    BOOST_REQUIRE_EQUAL(creditor["account"], "alice");
    BOOST_REQUIRE_EQUAL(creditor["balance"], "500.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["cpu_staked"], "0.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["net_staked"], "0.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["cpu_unstaked"], "0.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["net_unstaked"], "0.0000 EOS");
    creditor = get_creditor("bob");
    BOOST_REQUIRE_EQUAL(creditor["is_active"], 0);
    BOOST_REQUIRE_EQUAL(creditor["account"], "bob");
    BOOST_REQUIRE_EQUAL(creditor["balance"], "5000.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["cpu_staked"], "0.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["net_staked"], "0.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["cpu_unstaked"], "0.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["net_unstaked"], "0.0000 EOS");

    // del creditor bob
    push_action(N(bankofstaked), N(delcreditor), mvo()("account", "bob"), config::active_name);
    creditor = get_creditor("alice");
    BOOST_REQUIRE_EQUAL(creditor["is_active"], 0);
    BOOST_REQUIRE_EQUAL(creditor["account"], "alice");
    BOOST_REQUIRE_EQUAL(creditor["balance"], "500.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["cpu_staked"], "0.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["net_staked"], "0.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["cpu_unstaked"], "0.0000 EOS");
    BOOST_REQUIRE_EQUAL(creditor["net_unstaked"], "0.0000 EOS");
    //after deletion, bob should be EMPTY
    creditor = get_creditor("bob");
    BOOST_REQUIRE_EQUAL(creditor, "0");
}
FC_LOG_AND_RETHROW()


// test action addblacklist
BOOST_FIXTURE_TEST_CASE(addblacklist_test, bankofstaked_tester)
try
{
    // add 2 accounts to blacklist table, alice/bob
    push_action(N(bankofstaked), N(addblacklist), mvo()("account", "alice"), config::active_name);
    push_action(N(bankofstaked), N(addblacklist), mvo()("account", "bob"), config::active_name);

    auto blacklist = get_blacklist("alice");
    BOOST_REQUIRE_EQUAL(blacklist["account"], "alice");
    blacklist = get_blacklist("bob");
    BOOST_REQUIRE_EQUAL(blacklist["account"], "bob");
}
FC_LOG_AND_RETHROW()

// test action delblacklist
BOOST_FIXTURE_TEST_CASE(delblacklist_test, bankofstaked_tester)
try
{

    // add 2 accounts to blacklist table, alice/bob
    push_action(N(bankofstaked), N(addblacklist), mvo()("account", "alice"), config::active_name);
    push_action(N(bankofstaked), N(addblacklist), mvo()("account", "bob"), config::active_name);

    auto blacklist = get_blacklist("alice");
    BOOST_REQUIRE_EQUAL(blacklist["account"], "alice");
    blacklist = get_blacklist("bob");
    BOOST_REQUIRE_EQUAL(blacklist["account"], "bob");

    // del blacklist bob
    push_action(N(bankofstaked), N(delblacklist), mvo()("account", "bob"), config::active_name);
    blacklist = get_blacklist("alice");
    BOOST_REQUIRE_EQUAL(blacklist["account"], "alice");
    //after deletion, bob should be EMPTY
    blacklist = get_blacklist("bob");
    BOOST_REQUIRE_EQUAL(blacklist, "0");
}
FC_LOG_AND_RETHROW()


// test action addwhitelist
BOOST_FIXTURE_TEST_CASE(addwhitelist_test, bankofstaked_tester)
try
{
    // add 2 whitelist account, alice/bob
    push_action(N(bankofstaked), N(addwhitelist), mvo()("account", "alice")("capacity", 100), config::active_name);
    push_action(N(bankofstaked), N(addwhitelist), mvo()("account", "bob")("capacity", 1000), config::active_name);

    auto whitelist = get_whitelist("alice");
    BOOST_REQUIRE_EQUAL(whitelist["account"], "alice");
    BOOST_REQUIRE_EQUAL(whitelist["capacity"], 100);

    whitelist = get_whitelist("bob");
    BOOST_REQUIRE_EQUAL(whitelist["account"], "bob");
    BOOST_REQUIRE_EQUAL(whitelist["capacity"], 1000);
}
FC_LOG_AND_RETHROW()


BOOST_AUTO_TEST_SUITE_END()
