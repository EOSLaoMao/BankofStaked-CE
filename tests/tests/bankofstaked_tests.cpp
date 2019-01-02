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
        produce_blocks();

        create_accounts({N(alice), N(bob), N(carol), N(eosio.token), N(bankofstaked)});
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
        produce_blocks();

        set_code(N(eosio.token), contracts::token_wasm());
        set_abi(N(eosio.token), contracts::token_abi().data());

        const auto &t = control->db().get<account_object, by_name>(N(eosio.token));
        abi_def abi;
        BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(t.abi, abi), true);
        token_abi_ser.set_abi(abi, abi_serializer_max_time);

        set_code(N(bankofstaked), contracts::bank_wasm());
        set_abi(N(bankofstaked), contracts::bank_abi().data());
        produce_blocks();

        auto token = create(N(alice), asset::from_string("10000000.0000 EOS"));
        produce_blocks(1);
        issue(N(alice), N(alice), asset::from_string("500.0000 EOS"), "hola");
        issue(N(alice), N(bob), asset::from_string("5000.0000 EOS"), "hola");
        issue(N(alice), N(carol), asset::from_string("50000.0000 EOS"), "hola");
        produce_blocks();

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

    fc::variant get_safecreditor(const account_name &act)
    {
        vector<char> data = get_row_by_account(N(bankofstaked), 921459758687, N(safecreditor), act);
        return data.empty() ? EMPTY : abi_ser.binary_to_variant("safecreditor", data, abi_serializer_max_time);
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

    fc::variant get_plan(const asset& price)
    {
        vector<char> data = get_row_by_secondary_key(N(bankofstaked), N(bankofstaked), N(plan), (uint64_t)price.get_amount());
        return data.empty() ? EMPTY : abi_ser.binary_to_variant("plan", data, abi_serializer_max_time);
    }

    fc::variant get_account(account_name acc, const string &symbolname)
    {
        auto symb = eosio::chain::symbol::from_string(symbolname);
        auto symbol_code = symb.to_symbol_code().value;
        vector<char> data = get_row_by_account(N(eosio.token), acc, N(accounts), symbol_code);
        return data.empty() ? EMPTY : token_abi_ser.binary_to_variant("account", data, abi_serializer_max_time);
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

    abi_serializer token_abi_ser;
    abi_serializer abi_ser;
    fc::variant EMPTY = fc::variant(0).as_string().substr(0,8);
};

BOOST_AUTO_TEST_SUITE(bankofstaked_tests)

// test action delcreditor
BOOST_FIXTURE_TEST_CASE(rotate_test, bankofstaked_tester)
try
{
    // add 2 creditors, alice/bob
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "alice")("for_free", 0)("free_memo", ""));
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "bob")("for_free", 0)("free_memo", ""));

    push_action(N(bankofstaked), N(rotate), mvo()("creditor", "bob")("for_free", 0));

    // can't rotate non exist creditor
    BOOST_REQUIRE_EXCEPTION( push_action(N(bankofstaked), N(rotate), mvo()("creditor", "carol")("for_free", 0)),
        eosio_assert_message_exception,
        eosio_assert_message_is("account does not exist in creditor table")
    );
}
FC_LOG_AND_RETHROW()


// test action addwhitelist
BOOST_FIXTURE_TEST_CASE(addwhitelist_test, bankofstaked_tester)
try
{
    // add 2 whitelist account, alice/bob
    push_action(N(bankofstaked), N(addwhitelist), mvo()("account", "alice")("capacity", 100));
    push_action(N(bankofstaked), N(addwhitelist), mvo()("account", "bob")("capacity", 1000));

    auto whitelist = get_whitelist("alice");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("account", "alice")
        ("capacity", 100),
        whitelist
    );

    whitelist = get_whitelist("bob");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("account", "bob")
        ("capacity", 1000),
        whitelist
    );

    // update whitelist
    push_action(N(bankofstaked), N(addwhitelist), mvo()("account", "alice")("capacity", 500));
    whitelist = get_whitelist("alice");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("account", "alice")
        ("capacity", 500),
        whitelist
    );
}
FC_LOG_AND_RETHROW()


BOOST_FIXTURE_TEST_CASE(delwhitelist_test, bankofstaked_tester)
try
{
    // add 2 whitelist account, alice/bob
    push_action(N(bankofstaked), N(addwhitelist), mvo()("account", "alice")("capacity", 100));

    auto whitelist = get_whitelist("alice");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("account", "alice")
        ("capacity", 100),
        whitelist
    );

    push_action(N(bankofstaked), N(delwhitelist), mvo()("account", "alice")("capacity", 100));

    whitelist = get_blacklist("alice");
    BOOST_REQUIRE_EQUAL(whitelist, "0");

    // can't delete non exist whitelist
    BOOST_REQUIRE_EXCEPTION( push_action(N(bankofstaked), N(delwhitelist), mvo()
            ("account", "bob")
            ("capacity", 100)),
        eosio_assert_message_exception,
        eosio_assert_message_is("account not found in whitelist table")
    );

}
FC_LOG_AND_RETHROW()


BOOST_FIXTURE_TEST_CASE(addcreditor_test, bankofstaked_tester)
try
{
    // add 3 creditors, alice/bob/carol
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "alice")("for_free", 1)("free_memo", "lucky you!"));
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "bob")("for_free", 0)("free_memo", "hell yeah!"));
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "carol")("for_free", 1)("free_memo", "oh"));

    auto creditor = get_creditor("alice");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("is_active", 0)
        ("for_free", 1)
        ("free_memo", "lucky you!")
        ("account", "alice")
        ("balance", "500.0000 EOS")
        ("cpu_staked", "0.0000 EOS")
        ("net_staked", "0.0000 EOS")
        ("cpu_unstaked", "0.0000 EOS")
        ("net_unstaked", "0.0000 EOS"),
        creditor
    );

    creditor = get_creditor("bob");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("is_active", 0)
        ("for_free", 0)
        ("free_memo", "")
        ("account", "bob")
        ("balance", "5000.0000 EOS")
        ("cpu_staked", "0.0000 EOS")
        ("net_staked", "0.0000 EOS")
        ("cpu_unstaked", "0.0000 EOS")
        ("net_unstaked", "0.0000 EOS"),
        creditor
    );

    creditor = get_creditor("carol");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("is_active", 0)
        ("for_free", 1)
        ("free_memo", "oh")
        ("account", "carol")
        ("balance", "50000.0000 EOS")
        ("cpu_staked", "0.0000 EOS")
        ("net_staked", "0.0000 EOS")
        ("cpu_unstaked", "0.0000 EOS")
        ("net_unstaked", "0.0000 EOS"),
        creditor
    );

    BOOST_REQUIRE_EXCEPTION( push_action(N(bankofstaked), N(addcreditor), mvo()
            ("account", "carol")
            ("for_free", 1)
            ("free_memo", "oh")),
        eosio_assert_message_exception,
        eosio_assert_message_is("account already exist in creditor table")
    );
}
FC_LOG_AND_RETHROW()


BOOST_FIXTURE_TEST_CASE(addsafeacnt_test, bankofstaked_tester)
try
{
    // add 3 creditors, alice/bob/carol
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "alice")("for_free", 1)("free_memo", "lucky you!"));
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "bob")("for_free", 0)("free_memo", "hell yeah!"));

    auto creditor = get_creditor("alice");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("is_active", 0)
        ("for_free", 1)
        ("free_memo", "lucky you!")
        ("account", "alice")
        ("balance", "500.0000 EOS")
        ("cpu_staked", "0.0000 EOS")
        ("net_staked", "0.0000 EOS")
        ("cpu_unstaked", "0.0000 EOS")
        ("net_unstaked", "0.0000 EOS"),
        creditor
    );

    creditor = get_creditor("bob");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("is_active", 0)
        ("for_free", 0)
        ("free_memo", "")
        ("account", "bob")
        ("balance", "5000.0000 EOS")
        ("cpu_staked", "0.0000 EOS")
        ("net_staked", "0.0000 EOS")
        ("cpu_unstaked", "0.0000 EOS")
        ("net_unstaked", "0.0000 EOS"),
        creditor
    );

    push_action(N(bankofstaked), N(addsafeacnt), mvo()("account", "bob"));

    auto safecreditor = get_safecreditor("bob");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("account", "bob"),
        safecreditor
    );

    safecreditor = get_safecreditor("alice");
    BOOST_REQUIRE_EQUAL(safecreditor, "0");


    BOOST_REQUIRE_EXCEPTION( push_action(N(bankofstaked), N(addsafeacnt), mvo()("account", "carol")),
        eosio_assert_message_exception,
        eosio_assert_message_is("account does not exist in creditor table")
    );
}
FC_LOG_AND_RETHROW()


BOOST_FIXTURE_TEST_CASE(delsafeacnt_test, bankofstaked_tester)
try
{
    // add 3 creditors, alice/bob/carol
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "bob")("for_free", 0)("free_memo", "hell yeah!"));

    auto creditor = get_creditor("bob");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("is_active", 0)
        ("for_free", 0)
        ("free_memo", "")
        ("account", "bob")
        ("balance", "5000.0000 EOS")
        ("cpu_staked", "0.0000 EOS")
        ("net_staked", "0.0000 EOS")
        ("cpu_unstaked", "0.0000 EOS")
        ("net_unstaked", "0.0000 EOS"),
        creditor
    );

    push_action(N(bankofstaked), N(addsafeacnt), mvo()("account", "bob"));

    auto safecreditor = get_safecreditor("bob");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("account", "bob"),
        safecreditor
    );

    push_action(N(bankofstaked), N(delsafeacnt), mvo()("account", "bob"));
    safecreditor = get_safecreditor("bob");
    BOOST_REQUIRE_EQUAL(safecreditor, "0");


    BOOST_REQUIRE_EXCEPTION( push_action(N(bankofstaked), N(delsafeacnt), mvo()("account", "alice")),
        eosio_assert_message_exception,
        eosio_assert_message_is("account does not exist in safecreditor table")
    );
}
FC_LOG_AND_RETHROW()


// test action delcreditor
BOOST_FIXTURE_TEST_CASE(delcreditor_test, bankofstaked_tester)
try
{
    // add 2 creditors, alice/bob
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "alice")("for_free", 0)("free_memo", ""));
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "bob")("for_free", 0)("free_memo", ""));

    auto creditor = get_creditor("bob");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("is_active", 0)
        ("account", "bob")
        ("balance", "5000.0000 EOS")
        ("cpu_staked", "0.0000 EOS")
        ("net_staked", "0.0000 EOS")
        ("cpu_unstaked", "0.0000 EOS")
        ("net_unstaked", "0.0000 EOS"),
        creditor
    );

    // del creditor bob
    push_action(N(bankofstaked), N(delcreditor), mvo()("account", "bob"));
    //after deletion, bob should be EMPTY
    creditor = get_creditor("bob");
    BOOST_REQUIRE_EQUAL(creditor, "0");

    // can't delete non exist creditor
    BOOST_REQUIRE_EXCEPTION( push_action(N(bankofstaked), N(delcreditor), mvo()("account", "carol")),
        eosio_assert_message_exception,
        eosio_assert_message_is("account not found in creditor table")
    );

    // can't delete active creditor
    push_action(N(bankofstaked), N(activate), mvo()("account", "alice"));

    creditor = get_creditor("alice");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("is_active", 1)
        ("account", "alice")
        ("balance", "500.0000 EOS")
        ("cpu_staked", "0.0000 EOS")
        ("net_staked", "0.0000 EOS")
        ("cpu_unstaked", "0.0000 EOS")
        ("net_unstaked", "0.0000 EOS"),
        creditor
    );

    BOOST_REQUIRE_EXCEPTION( push_action(N(bankofstaked), N(delcreditor), mvo()("account", "alice")),
        eosio_assert_message_exception,
        eosio_assert_message_is("cannot delete active creditor")
    );
}
FC_LOG_AND_RETHROW()


// test action addblacklist
BOOST_FIXTURE_TEST_CASE(addblacklist_test, bankofstaked_tester)
try
{
    // add 2 accounts to blacklist table, alice/bob
    push_action(N(bankofstaked), N(addblacklist), mvo()("account", "alice"));
    push_action(N(bankofstaked), N(addblacklist), mvo()("account", "bob"));

    auto blacklist = get_blacklist("alice");
    BOOST_REQUIRE_EQUAL(blacklist["account"], "alice");
    blacklist = get_blacklist("bob");
    BOOST_REQUIRE_EQUAL(blacklist["account"], "bob");


    BOOST_REQUIRE_EXCEPTION( push_action(N(bankofstaked), N(addblacklist), mvo()("account", "bob")),
        eosio_assert_message_exception,
        eosio_assert_message_is("account already exist in blacklist table")
    );
}
FC_LOG_AND_RETHROW()


// test action delblacklist
BOOST_FIXTURE_TEST_CASE(delblacklist_test, bankofstaked_tester)
try
{
    // add 2 accounts to blacklist table, alice/bob
    push_action(N(bankofstaked), N(addblacklist), mvo()("account", "alice"));
    push_action(N(bankofstaked), N(addblacklist), mvo()("account", "bob"));

    auto blacklist = get_blacklist("alice");
    BOOST_REQUIRE_EQUAL(blacklist["account"], "alice");
    blacklist = get_blacklist("bob");
    BOOST_REQUIRE_EQUAL(blacklist["account"], "bob");

    // del blacklist bob
    push_action(N(bankofstaked), N(delblacklist), mvo()("account", "bob"));
    blacklist = get_blacklist("alice");
    BOOST_REQUIRE_EQUAL(blacklist["account"], "alice");
    //after deletion, bob should be EMPTY
    blacklist = get_blacklist("bob");
    BOOST_REQUIRE_EQUAL(blacklist, "0");

    BOOST_REQUIRE_EXCEPTION( push_action(N(bankofstaked), N(delblacklist), mvo()("account", "carol")),
        eosio_assert_message_exception,
        eosio_assert_message_is("account not found in blacklist table")
    );
}
FC_LOG_AND_RETHROW()


BOOST_FIXTURE_TEST_CASE(activate_test, bankofstaked_tester)
try
{
    // add 3 creditors, alice/bob/carol
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "alice")("for_free", 0)("free_memo", ""));
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "bob")("for_free", 0)("free_memo", ""));

    auto creditor = get_creditor("alice");
    BOOST_REQUIRE_EQUAL(creditor["is_active"], 0);
    creditor = get_creditor("bob");
    BOOST_REQUIRE_EQUAL(creditor["is_active"], 0);

    //set bob as active creditor
    push_action(N(bankofstaked), N(activate), mvo()("account", "bob"));
    creditor = get_creditor("alice");
    BOOST_REQUIRE_EQUAL(creditor["is_active"], 0);
    creditor = get_creditor("bob");
    BOOST_REQUIRE_EQUAL(creditor["is_active"], 1);


    BOOST_REQUIRE_EXCEPTION( push_action(N(bankofstaked), N(activate), mvo()("account", "carol")),
        eosio_assert_message_exception,
        eosio_assert_message_is("account not found in creditor table")
    );
}
FC_LOG_AND_RETHROW()


BOOST_FIXTURE_TEST_CASE(setplan_test, bankofstaked_tester)
try
{
    push_action(N(bankofstaked), N(setplan), mvo()
        ("price", "0.1000 EOS")
        ("cpu", "1.0000 EOS")
        ("net", "1.0000 EOS")
        ("duration", 300)
        ("is_free", 1));
    auto plan = get_plan(asset::from_string("0.1000 EOS"));
    REQUIRE_MATCHING_OBJECT(mvo()
        ("price", "0.1000 EOS")
        ("cpu", "1.0000 EOS")
        ("net", "1.0000 EOS")
        ("duration", 300)
        ("is_free", 1),
        plan
    );

    push_action(N(bankofstaked), N(setplan), mvo()
        ("price", "0.1000 EOS")
        ("cpu", "5.0000 EOS")
        ("net", "5.0000 EOS")
        ("duration", 400)
        ("is_free", 0));
    plan = get_plan(asset::from_string("0.1000 EOS"));
    REQUIRE_MATCHING_OBJECT(mvo()
        ("price", "0.1000 EOS")
        ("cpu", "5.0000 EOS")
        ("net", "5.0000 EOS")
        ("duration", 400)
        ("is_free", 0),
        plan
    );

    BOOST_REQUIRE_EXCEPTION( push_action( N(bankofstaked), N(setplan), mvo()
            ("price", "0.0099 EOS")
            ("cpu", "5.0000 EOS")
            ("net", "5.0000 EOS")
            ("duration", 400)
            ("is_free", 0)
        ),
        eosio_assert_message_exception,
        eosio_assert_message_is("price should between 0.01 EOS and 1000 EOS")
    );

    BOOST_REQUIRE_EXCEPTION( push_action( N(bankofstaked), N(setplan), mvo()
            ("price", "1000.0001 EOS")
            ("cpu", "5.0000 EOS")
            ("net", "5.0000 EOS")
            ("duration", 400)
            ("is_free", 0)
        ),
        eosio_assert_message_exception,
        eosio_assert_message_is("price should between 0.01 EOS and 1000 EOS")
    );
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(activateplan_test, bankofstaked_tester)
try
{
    push_action(N(bankofstaked), N(setplan), mvo()
        ("price", "0.1000 EOS")
        ("cpu", "1.0000 EOS")
        ("net", "1.0000 EOS")
        ("duration", 300)
        ("is_free", 1));
    auto plan = get_plan(asset::from_string("0.1000 EOS"));
    REQUIRE_MATCHING_OBJECT(mvo()
        ("price", "0.1000 EOS")
        ("cpu", "1.0000 EOS")
        ("net", "1.0000 EOS")
        ("is_active", 0)
        ("duration", 300)
        ("is_free", 1),
        plan
    );

    push_action(N(bankofstaked), N(activateplan), mvo()
        ("price", "0.1000 EOS")
        ("is_active", 1));
    plan = get_plan(asset::from_string("0.1000 EOS"));
    REQUIRE_MATCHING_OBJECT(mvo()
        ("price", "0.1000 EOS")
        ("cpu", "1.0000 EOS")
        ("net", "1.0000 EOS")
        ("is_active", 1)
        ("duration", 300)
        ("is_free", 1),
        plan
    );
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
