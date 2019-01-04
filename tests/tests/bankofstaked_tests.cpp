#include <boost/test/unit_test.hpp>
#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include "contracts.hpp"
#include "bankofstaked_tester.hpp"

#include "Runtime/Runtime.h"

#include <fc/variant_object.hpp>

using namespace eosio::testing;
using namespace eosio;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;
using namespace std;

using mvo = fc::mutable_variant_object;

BOOST_AUTO_TEST_SUITE(bankofstaked_tests)

// test action delcreditor
BOOST_FIXTURE_TEST_CASE(rotate_test, bankofstaked_tester)
try
{
    // add 2 creditors, alice/bob
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "alice.111111")("for_free", 0)("free_memo", ""));
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "bob.11111111")("for_free", 0)("free_memo", ""));

    push_action(N(bankofstaked), N(rotate), mvo()("creditor", "bob.11111111")("for_free", 0));

    // can't rotate non exist creditor
    BOOST_REQUIRE_EXCEPTION( push_action(N(bankofstaked), N(rotate), mvo()("creditor", "carol.111111")("for_free", 0)),
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
    push_action(N(bankofstaked), N(addwhitelist), mvo()("account", "alice.111111")("capacity", 100));
    push_action(N(bankofstaked), N(addwhitelist), mvo()("account", "bob.11111111")("capacity", 1000));

    auto whitelist = get_whitelist("alice.111111");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("account", "alice.111111")
        ("capacity", 100),
        whitelist
    );

    whitelist = get_whitelist("bob.11111111");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("account", "bob.11111111")
        ("capacity", 1000),
        whitelist
    );

    // update whitelist
    push_action(N(bankofstaked), N(addwhitelist), mvo()("account", "alice.111111")("capacity", 500));
    whitelist = get_whitelist("alice.111111");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("account", "alice.111111")
        ("capacity", 500),
        whitelist
    );
}
FC_LOG_AND_RETHROW()


BOOST_FIXTURE_TEST_CASE(delwhitelist_test, bankofstaked_tester)
try
{
    // add 2 whitelist account, alice/bob
    push_action(N(bankofstaked), N(addwhitelist), mvo()("account", "alice.111111")("capacity", 100));

    auto whitelist = get_whitelist("alice.111111");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("account", "alice.111111")
        ("capacity", 100),
        whitelist
    );

    push_action(N(bankofstaked), N(delwhitelist), mvo()("account", "alice.111111")("capacity", 100));

    whitelist = get_blacklist("alice.111111");
    BOOST_REQUIRE_EQUAL(whitelist, "0");

    // can't delete non exist whitelist
    BOOST_REQUIRE_EXCEPTION( push_action(N(bankofstaked), N(delwhitelist), mvo()
            ("account", "bob.11111111")
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
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "alice.111111")("for_free", 1)("free_memo", "lucky you!"));
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "bob.11111111")("for_free", 0)("free_memo", "hell yeah!"));
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "carol.111111")("for_free", 1)("free_memo", "oh"));

    auto creditor = get_creditor("alice.111111");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("is_active", 0)
        ("for_free", 1)
        ("free_memo", "lucky you!")
        ("account", "alice.111111")
        ("balance", "500.0000 EOS")
        ("cpu_staked", "0.0000 EOS")
        ("net_staked", "0.0000 EOS")
        ("cpu_unstaked", "0.0000 EOS")
        ("net_unstaked", "0.0000 EOS"),
        creditor
    );

    creditor = get_creditor("bob.11111111");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("is_active", 0)
        ("for_free", 0)
        ("free_memo", "")
        ("account", "bob.11111111")
        ("balance", "5000.0000 EOS")
        ("cpu_staked", "0.0000 EOS")
        ("net_staked", "0.0000 EOS")
        ("cpu_unstaked", "0.0000 EOS")
        ("net_unstaked", "0.0000 EOS"),
        creditor
    );

    creditor = get_creditor("carol.111111");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("is_active", 0)
        ("for_free", 1)
        ("free_memo", "oh")
        ("account", "carol.111111")
        ("balance", "50000.0000 EOS")
        ("cpu_staked", "0.0000 EOS")
        ("net_staked", "0.0000 EOS")
        ("cpu_unstaked", "0.0000 EOS")
        ("net_unstaked", "0.0000 EOS"),
        creditor
    );

    BOOST_REQUIRE_EXCEPTION( push_action(N(bankofstaked), N(addcreditor), mvo()
            ("account", "carol.111111")
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
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "alice.111111")("for_free", 1)("free_memo", "lucky you!"));
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "bob.11111111")("for_free", 0)("free_memo", "hell yeah!"));

    auto creditor = get_creditor("alice.111111");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("is_active", 0)
        ("for_free", 1)
        ("free_memo", "lucky you!")
        ("account", "alice.111111")
        ("balance", "500.0000 EOS")
        ("cpu_staked", "0.0000 EOS")
        ("net_staked", "0.0000 EOS")
        ("cpu_unstaked", "0.0000 EOS")
        ("net_unstaked", "0.0000 EOS"),
        creditor
    );

    creditor = get_creditor("bob.11111111");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("is_active", 0)
        ("for_free", 0)
        ("free_memo", "")
        ("account", "bob.11111111")
        ("balance", "5000.0000 EOS")
        ("cpu_staked", "0.0000 EOS")
        ("net_staked", "0.0000 EOS")
        ("cpu_unstaked", "0.0000 EOS")
        ("net_unstaked", "0.0000 EOS"),
        creditor
    );

    push_action(N(bankofstaked), N(addsafeacnt), mvo()("account", "bob.11111111"));

    auto safecreditor = get_safecreditor("bob.11111111");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("account", "bob.11111111"),
        safecreditor
    );

    safecreditor = get_safecreditor("alice.111111");
    BOOST_REQUIRE_EQUAL(safecreditor, "0");


    BOOST_REQUIRE_EXCEPTION( push_action(N(bankofstaked), N(addsafeacnt), mvo()("account", "carol.111111")),
        eosio_assert_message_exception,
        eosio_assert_message_is("account does not exist in creditor table")
    );
}
FC_LOG_AND_RETHROW()


BOOST_FIXTURE_TEST_CASE(delsafeacnt_test, bankofstaked_tester)
try
{
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "bob.11111111")("for_free", 0)("free_memo", "hell yeah!"));

    auto creditor = get_creditor("bob.11111111");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("is_active", 0)
        ("for_free", 0)
        ("free_memo", "")
        ("account", "bob.11111111")
        ("balance", "5000.0000 EOS")
        ("cpu_staked", "0.0000 EOS")
        ("net_staked", "0.0000 EOS")
        ("cpu_unstaked", "0.0000 EOS")
        ("net_unstaked", "0.0000 EOS"),
        creditor
    );

    push_action(N(bankofstaked), N(addsafeacnt), mvo()("account", "bob.11111111"));

    auto safecreditor = get_safecreditor("bob.11111111");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("account", "bob.11111111"),
        safecreditor
    );

    push_action(N(bankofstaked), N(delsafeacnt), mvo()("account", "bob.11111111"));
    safecreditor = get_safecreditor("bob.11111111");
    BOOST_REQUIRE_EQUAL(safecreditor, "0");


    BOOST_REQUIRE_EXCEPTION( push_action(N(bankofstaked), N(delsafeacnt), mvo()("account", "alice.111111")),
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
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "alice.111111")("for_free", 0)("free_memo", ""));
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "bob.11111111")("for_free", 0)("free_memo", ""));

    auto creditor = get_creditor("bob.11111111");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("is_active", 0)
        ("account", "bob.11111111")
        ("balance", "5000.0000 EOS")
        ("cpu_staked", "0.0000 EOS")
        ("net_staked", "0.0000 EOS")
        ("cpu_unstaked", "0.0000 EOS")
        ("net_unstaked", "0.0000 EOS"),
        creditor
    );

    // del creditor bob
    push_action(N(bankofstaked), N(delcreditor), mvo()("account", "bob.11111111"));
    //after deletion, bob should be EMPTY
    creditor = get_creditor("bob.11111111");
    BOOST_REQUIRE_EQUAL(creditor, "0");

    // can't delete non exist creditor
    BOOST_REQUIRE_EXCEPTION( push_action(N(bankofstaked), N(delcreditor), mvo()("account", "carol.111111")),
        eosio_assert_message_exception,
        eosio_assert_message_is("account not found in creditor table")
    );

    // can't delete active creditor
    push_action(N(bankofstaked), N(activate), mvo()("account", "alice.111111"));

    creditor = get_creditor("alice.111111");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("is_active", 1)
        ("account", "alice.111111")
        ("balance", "500.0000 EOS")
        ("cpu_staked", "0.0000 EOS")
        ("net_staked", "0.0000 EOS")
        ("cpu_unstaked", "0.0000 EOS")
        ("net_unstaked", "0.0000 EOS"),
        creditor
    );

    BOOST_REQUIRE_EXCEPTION( push_action(N(bankofstaked), N(delcreditor), mvo()("account", "alice.111111")),
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
    push_action(N(bankofstaked), N(addblacklist), mvo()("account", "alice.111111"));
    push_action(N(bankofstaked), N(addblacklist), mvo()("account", "bob.11111111"));

    auto blacklist = get_blacklist("alice.111111");
    BOOST_REQUIRE_EQUAL(blacklist["account"], "alice.111111");
    blacklist = get_blacklist("bob.11111111");
    BOOST_REQUIRE_EQUAL(blacklist["account"], "bob.11111111");


    BOOST_REQUIRE_EXCEPTION( push_action(N(bankofstaked), N(addblacklist), mvo()("account", "bob.11111111")),
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
    push_action(N(bankofstaked), N(addblacklist), mvo()("account", "alice.111111"));
    push_action(N(bankofstaked), N(addblacklist), mvo()("account", "bob.11111111"));

    auto blacklist = get_blacklist("alice.111111");
    BOOST_REQUIRE_EQUAL(blacklist["account"], "alice.111111");
    blacklist = get_blacklist("bob.11111111");
    BOOST_REQUIRE_EQUAL(blacklist["account"], "bob.11111111");

    // del blacklist bob
    push_action(N(bankofstaked), N(delblacklist), mvo()("account", "bob.11111111"));
    blacklist = get_blacklist("alice.111111");
    BOOST_REQUIRE_EQUAL(blacklist["account"], "alice.111111");
    //after deletion, bob should be EMPTY
    blacklist = get_blacklist("bob.11111111");
    BOOST_REQUIRE_EQUAL(blacklist, "0");

    BOOST_REQUIRE_EXCEPTION( push_action(N(bankofstaked), N(delblacklist), mvo()("account", "carol.111111")),
        eosio_assert_message_exception,
        eosio_assert_message_is("account not found in blacklist table")
    );
}
FC_LOG_AND_RETHROW()


BOOST_FIXTURE_TEST_CASE(activate_test, bankofstaked_tester)
try
{
    // add 3 creditors, alice/bob/carol
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "alice.111111")("for_free", 0)("free_memo", ""));
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "bob.11111111")("for_free", 0)("free_memo", ""));

    auto creditor = get_creditor("alice.111111");
    BOOST_REQUIRE_EQUAL(creditor["is_active"], 0);
    creditor = get_creditor("bob.11111111");
    BOOST_REQUIRE_EQUAL(creditor["is_active"], 0);

    //set bob as active creditor
    push_action(N(bankofstaked), N(activate), mvo()("account", "bob.11111111"));
    creditor = get_creditor("alice.111111");
    BOOST_REQUIRE_EQUAL(creditor["is_active"], 0);
    creditor = get_creditor("bob.11111111");
    BOOST_REQUIRE_EQUAL(creditor["is_active"], 1);


    BOOST_REQUIRE_EXCEPTION( push_action(N(bankofstaked), N(activate), mvo()("account", "carol.111111")),
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

BOOST_FIXTURE_TEST_CASE(buy_non_free_plan_test, bankofstaked_tester)
try
{
    push_action(N(bankofstaked), N(setplan), mvo()
        ("price", "0.1000 EOS")
        ("cpu", "1.0000 EOS")
        ("net", "1.0000 EOS")
        ("duration", 1)
        ("is_free", 1));
    push_action(N(bankofstaked), N(setplan), mvo()
        ("price", "1.0000 EOS")
        ("cpu", "10.0000 EOS")
        ("net", "10.0000 EOS")
        ("duration", 2)
        ("is_free", 0));

    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "bob.11111111")("for_free", 1)("free_memo", "hell yeah!"));
    push_action(N(bankofstaked), N(addcreditor), mvo()("account", "carol.111111")("for_free", 0)("free_memo", "should be ignored"));

    push_action(N(bankofstaked), N(activateplan), mvo()
        ("price", "0.1000 EOS")
        ("is_active", 1));
    push_action(N(bankofstaked), N(activateplan), mvo()
        ("price", "1.0000 EOS")
        ("is_active", 1));

    push_action(N(bankofstaked), N(activate), mvo()("account", "bob.11111111"));
    push_action(N(bankofstaked), N(activate), mvo()("account", "carol.111111"));
    produce_blocks();

    eosio_system_tester::transfer(N(alice.111111), N(bankofstaked), core_sym::from_string("1.0000"), N(alice.111111));
    produce_blocks();

    auto creditor = get_creditor("carol.111111");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("is_active", 1)
        ("for_free", 0)
        ("free_memo", "")
        ("account", "carol.111111")
        ("balance", "50000.0000 EOS")
        ("cpu_staked", "10.0000 EOS")
        ("net_staked", "10.0000 EOS")
        ("cpu_unstaked", "0.0000 EOS")
        ("net_unstaked", "0.0000 EOS"),
        creditor
    );

    auto order = get_order("alice.111111");
    REQUIRE_MATCHING_OBJECT(mvo()
        ("buyer", "alice.111111")
        ("price", "1.0000 EOS")
        ("is_free", 0)
        ("creditor", "carol.111111")
        ("beneficiary", "alice.111111")
        ("cpu_staked", "10.0000 EOS")
        ("net_staked", "10.0000 EOS"),
        order
    );

    // produce_blocks(300);
    // creditor = get_creditor("carol.111111");
    // REQUIRE_MATCHING_OBJECT(mvo()
    //     ("is_active", 1)
    //     ("for_free", 0)
    //     ("free_memo", "")
    //     ("account", "carol.111111")
    //     ("balance", "50000.0000 EOS")
    //     ("cpu_staked", "0.0000 EOS")
    //     ("net_staked", "0.0000 EOS")
    //     ("cpu_unstaked", "10.0000 EOS")
    //     ("net_unstaked", "10.0000 EOS"),
    //     creditor
    // );

}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
