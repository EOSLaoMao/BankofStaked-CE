/**
 *  @file bankofstaked.hpp
 */
#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/multi_index.hpp>

#define EOS_SYMBOL symbol("EOS", 4)

using namespace eosio;

namespace bank
{
static const name CODE_ACCOUNT = "bankofstaked"_n;
static const name RAM_PAYER = "bankofstaked"_n;
static const name MASK_TRANSFER = "masktransfer"_n;
static const name STAKED_INCOME = "stakedincome"_n;
static const name EOSIO = "eosio"_n;
static const name EOSIO_TOKEN = "eosio.token"_n;
static const uint64_t FREE_PLAN_AMOUNT = 1000;
static const uint64_t SECONDS_PER_MIN = 60;
static const uint64_t SECONDS_PER_DAY = 24 * 3600;
static const uint64_t MAX_FREE_ORDERS = 5;
static const uint64_t MAX_PAID_ORDERS = 20;
static const uint64_t TRUE = 1;
static const uint64_t FALSE = 0;
static const uint64_t CHECK_MAX_DEPTH = 3;
static const uint64_t MAX_EOS_BALANCE = 500 * 10000; // 500 EOS at most
static const uint64_t MIN_FREE_CREDITOR_BALANCE = 10 * 10000; // 10 EOS at least
static const uint64_t DEFAULT_DIVIDEND_PERCENTAGE = 90; // 90% income will be allocated to creditor

// To protect your table, you can specify different scope as random numbers
static const uint64_t SCOPE = 921459758687;

struct [[eosio::table, eosio::contract("bankofstaked")]] freelock
{
  name beneficiary; // account who received CPU&NET
  uint64_t created_at;      // unix time, in seconds
  uint64_t expire_at;       // unix time, in seconds

  uint64_t primary_key() const { return beneficiary.value; }
  uint64_t get_expire_at() const { return expire_at; }

  EOSLIB_SERIALIZE(freelock, (beneficiary)(created_at)(expire_at));
};

typedef multi_index<"freelock"_n, freelock,
                    indexed_by<"expire.at"_n, const_mem_fun<freelock, uint64_t, &freelock::get_expire_at>>>
    freelock_table;

struct [[eosio::table, eosio::contract("bankofstaked")]] order
{
  uint64_t id;
  name buyer;
  asset price{0, EOS_SYMBOL};              // amount of EOS paied
  uint64_t is_free;         // default is FALSE, for free plan, when service expired, it will do a auto refund
  name creditor;    // account who delegated CPU&NET
  name beneficiary; // account who received CPU&NET
  uint64_t plan_id;         // foreignkey of table plan
  asset cpu_staked{0, EOS_SYMBOL};         // amount of EOS staked for cpu
  asset net_staked{0, EOS_SYMBOL};         // amount of EOS staked for net
  uint64_t created_at;      // unix time, in seconds
  uint64_t expire_at;       // unix time, in seconds

  auto primary_key() const { return id; }
  uint64_t get_buyer() const { return buyer.value; }
  uint64_t get_beneficiary() const { return beneficiary.value; }
  uint64_t get_expire_at() const { return expire_at; }

  EOSLIB_SERIALIZE(order, (id)(buyer)(price)(is_free)(creditor)(beneficiary)(plan_id)(cpu_staked)(net_staked)(created_at)(expire_at));
};

typedef multi_index<"order"_n, order,
                    indexed_by<"buyer"_n, const_mem_fun<order, uint64_t, &order::get_buyer>>,
                    indexed_by<"expire.at"_n, const_mem_fun<order, uint64_t, &order::get_expire_at>>,
                    indexed_by<"beneficiary"_n, const_mem_fun<order, uint64_t, &order::get_beneficiary>>>
    order_table;

struct [[eosio::table, eosio::contract("bankofstaked")]] history
{
  uint64_t id;
  string content;      // content
  uint64_t created_at; // unix time, in seconds

  auto primary_key() const { return id; }
  EOSLIB_SERIALIZE(history, (id)(content)(created_at));
};
typedef multi_index<"history"_n, history> history_table;

struct [[eosio::table, eosio::contract("bankofstaked")]] plan
{
  uint64_t id;
  asset price{0, EOS_SYMBOL};         // amount of EOS paied
  asset cpu{0, EOS_SYMBOL};           // amount of EOS staked for cpu
  asset net{0, EOS_SYMBOL};           // amount of EOS staked for net
  uint64_t duration;   // affective time, in minutes
  uint64_t is_free;    // default is FALSE, for free plan, when service expired, it will do a auto refund
  uint64_t is_active;  // on active plan could be choosen
  uint64_t created_at; // unix time, in seconds
  uint64_t updated_at; // unix time, in seconds

  auto primary_key() const { return id; }
  uint64_t get_price() const { return (uint64_t)price.amount; }
  EOSLIB_SERIALIZE(plan, (id)(price)(cpu)(net)(duration)(is_free)(is_active)(created_at)(updated_at));
};
typedef multi_index<"plan"_n, plan,
                    indexed_by<"price"_n, const_mem_fun<plan, uint64_t, &plan::get_price>>>
    plan_table;

struct [[eosio::table, eosio::contract("bankofstaked")]] safecreditor
{
  name account;
  uint64_t created_at; // unix time, in seconds
  uint64_t updated_at; // unix time, in seconds

  uint64_t primary_key() const { return account.value; }

  EOSLIB_SERIALIZE(safecreditor, (account)(created_at)(updated_at));
};
typedef multi_index<"safecreditor"_n, safecreditor> safecreditor_table;

struct [[eosio::table, eosio::contract("bankofstaked")]] dividend
{
  name account;
  uint64_t percentage; // percentage of income allocating to creditor

  uint64_t primary_key() const { return account.value; }

  EOSLIB_SERIALIZE(dividend, (account)(percentage));
};
typedef multi_index<"dividend"_n, dividend> dividend_table;

struct [[eosio::table, eosio::contract("bankofstaked")]] creditor
{
  name account;
  uint64_t is_active;
  uint64_t for_free;         // default is FALSE, for_free means if this creditor provide free staking or not
  string free_memo;    // memo for refund transaction
  asset balance{0, EOS_SYMBOL};              // amount of EOS paied
  asset cpu_staked{0, EOS_SYMBOL};              // amount of EOS paied
  asset net_staked{0, EOS_SYMBOL};              // amount of EOS paied
  asset cpu_unstaked{0, EOS_SYMBOL};              // amount of EOS paied
  asset net_unstaked{0, EOS_SYMBOL};              // amount of EOS paied
  uint64_t created_at; // unix time, in seconds
  uint64_t updated_at; // unix time, in seconds

  uint64_t primary_key() const { return account.value; }
  uint64_t get_is_active() const { return is_active; }
  uint64_t get_updated_at() const { return updated_at; }

  EOSLIB_SERIALIZE(creditor, (account)(is_active)(for_free)(free_memo)(balance)(cpu_staked)(net_staked)(cpu_unstaked)(net_unstaked)(created_at)(updated_at));
};

typedef multi_index<"creditor"_n, creditor,
                    indexed_by<"is.active"_n, const_mem_fun<creditor, uint64_t, &creditor::get_is_active>>,
                    indexed_by<"updated.at"_n, const_mem_fun<creditor, uint64_t, &creditor::get_updated_at>>>
    creditor_table;

struct [[eosio::table, eosio::contract("bankofstaked")]] blacklist
{
  name account;
  uint64_t created_at; // unix time, in seconds

  uint64_t primary_key() const { return account.value; }
  EOSLIB_SERIALIZE(blacklist, (account)(created_at));
};
typedef multi_index<"blacklist"_n, blacklist> blacklist_table;

struct [[eosio::table, eosio::contract("bankofstaked")]] whitelist
{
  name account;
  uint64_t capacity; // max in-use free orders
  uint64_t updated_at; // unix time, in seconds
  uint64_t created_at; // unix time, in seconds

  uint64_t primary_key() const { return account.value; }
  EOSLIB_SERIALIZE(whitelist, (account)(capacity)(updated_at)(created_at));
};
typedef multi_index<"whitelist"_n, whitelist> whitelist_table;

}// namespace bank

struct [[eosio::table, eosio::contract("bankofstaked")]] recipient
{
  name creditor;
  name recipient_account;
  uint64_t created_at; // unix time, in seconds
  uint64_t updated_at; // unix time, in seconds

  uint64_t primary_key() const { return creditor.value; }

  EOSLIB_SERIALIZE(recipient, (creditor)(recipient_account)(created_at)(updated_at));
};
typedef multi_index<"recipient"_n, recipient> recipient_table;
