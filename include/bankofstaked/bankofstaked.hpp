/**
 *  @file bankofstaked.hpp
 */
#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/multi_index.hpp>

#define EOS_SYMBOL S(4, EOS)

using namespace eosio;

namespace bank
{
static const account_name CODE_ACCOUNT = N(bankofstaked);
static const account_name RAM_PAYER = N(bankofstaked);
static const account_name MASK_TRANSFER = N(masktransfer);
static const account_name STAKED_INCOME = N(stakedincome);
static const account_name EOSIO = string_to_name("eosio");
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

// @abi table freelock i64
struct freelock
{
  account_name beneficiary; // account who received CPU&NET
  uint64_t created_at;      // unix time, in seconds
  uint64_t expire_at;       // unix time, in seconds

  account_name primary_key() const { return beneficiary; }
  uint64_t get_expire_at() const { return expire_at; }

  EOSLIB_SERIALIZE(freelock, (beneficiary)(created_at)(expire_at));
};

typedef multi_index<N(freelock), freelock,
                    indexed_by<N(expire_at), const_mem_fun<freelock, uint64_t, &freelock::get_expire_at>>>
    freelock_table;

// @abi table order i64
struct order
{
  uint64_t id;
  account_name buyer;
  asset price;              // amount of EOS paied
  uint64_t is_free;         // default is FALSE, for free plan, when service expired, it will do a auto refund
  account_name creditor;    // account who delegated CPU&NET
  account_name beneficiary; // account who received CPU&NET
  uint64_t plan_id;         // foreignkey of table plan
  asset cpu_staked;         // amount of EOS staked for cpu
  asset net_staked;         // amount of EOS staked for net
  uint64_t created_at;      // unix time, in seconds
  uint64_t expire_at;       // unix time, in seconds

  auto primary_key() const { return id; }
  account_name get_buyer() const { return buyer; }
  account_name get_beneficiary() const { return beneficiary; }
  uint64_t get_expire_at() const { return expire_at; }

  EOSLIB_SERIALIZE(order, (id)(buyer)(price)(is_free)(creditor)(beneficiary)(plan_id)(cpu_staked)(net_staked)(created_at)(expire_at));
};

typedef multi_index<N(order), order,
                    indexed_by<N(buyer), const_mem_fun<order, account_name, &order::get_buyer>>,
                    indexed_by<N(expire_at), const_mem_fun<order, uint64_t, &order::get_expire_at>>,
                    indexed_by<N(beneficiary), const_mem_fun<order, account_name, &order::get_beneficiary>>>
    order_table;

// @abi table history
struct history
{
  uint64_t id;
  string content;      // content
  uint64_t created_at; // unix time, in seconds

  auto primary_key() const { return id; }
  EOSLIB_SERIALIZE(history, (id)(content)(created_at));
};
typedef multi_index<N(history), history> history_table;

// @abi table plan i64
struct plan
{
  uint64_t id;
  asset price;         // amount of EOS paied
  asset cpu;           // amount of EOS staked for cpu
  asset net;           // amount of EOS staked for net
  uint64_t duration;   // affective time, in minutes
  uint64_t is_free;    // default is FALSE, for free plan, when service expired, it will do a auto refund
  uint64_t is_active;  // on active plan could be choosen
  uint64_t created_at; // unix time, in seconds
  uint64_t updated_at; // unix time, in seconds

  auto primary_key() const { return id; }
  uint64_t get_price() const { return (uint64_t)price.amount; }
  EOSLIB_SERIALIZE(plan, (id)(price)(cpu)(net)(duration)(is_free)(is_active)(created_at)(updated_at));
};
typedef multi_index<N(plan), plan,
                    indexed_by<N(price), const_mem_fun<plan, uint64_t, &plan::get_price>>>
    plan_table;

// @abi table safecreditor i64
struct safecreditor
{
  account_name account;
  uint64_t created_at; // unix time, in seconds
  uint64_t updated_at; // unix time, in seconds

  account_name primary_key() const { return account; }

  EOSLIB_SERIALIZE(safecreditor, (account)(created_at)(updated_at));
};
typedef multi_index<N(safecreditor), safecreditor> safecreditor_table;

// @abi table dividend i64
struct dividend 
{
  account_name account;
  uint64_t percentage; // percentage of income allocating to creditor

  account_name primary_key() const { return account; }

  EOSLIB_SERIALIZE(dividend, (account)(percentage));
};
typedef multi_index<N(dividend), dividend> dividend_table;

// @abi table creditor i64
struct creditor
{
  account_name account;
  uint64_t is_active;
  uint64_t for_free;         // default is FALSE, for_free means if this creditor provide free staking or not
  string free_memo;    // memo for refund transaction
  asset balance;              // amount of EOS paied
  asset cpu_staked;              // amount of EOS paied
  asset net_staked;              // amount of EOS paied
  asset cpu_unstaked;              // amount of EOS paied
  asset net_unstaked;              // amount of EOS paied
  uint64_t created_at; // unix time, in seconds
  uint64_t updated_at; // unix time, in seconds

  account_name primary_key() const { return account; }
  uint64_t get_is_active() const { return is_active; }
  uint64_t get_updated_at() const { return updated_at; }

  EOSLIB_SERIALIZE(creditor, (account)(is_active)(for_free)(free_memo)(balance)(cpu_staked)(net_staked)(cpu_unstaked)(net_unstaked)(created_at)(updated_at));
};

typedef multi_index<N(creditor), creditor,
                    indexed_by<N(is_active), const_mem_fun<creditor, uint64_t, &creditor::get_is_active>>,
                    indexed_by<N(updated_at), const_mem_fun<creditor, uint64_t, &creditor::get_updated_at>>>
    creditor_table;

// @abi table blacklist i64
struct blacklist
{
  account_name account;
  uint64_t created_at; // unix time, in seconds

  account_name primary_key() const { return account; }
  EOSLIB_SERIALIZE(blacklist, (account)(created_at));
};
typedef multi_index<N(blacklist), blacklist> blacklist_table;

// @abi table whitelist i64
struct whitelist
{
  account_name account;
  uint64_t capacity; // max in-use free orders
  uint64_t updated_at; // unix time, in seconds
  uint64_t created_at; // unix time, in seconds

  account_name primary_key() const { return account; }
  EOSLIB_SERIALIZE(whitelist, (account)(capacity)(updated_at)(created_at));
};
typedef multi_index<N(whitelist), whitelist> whitelist_table;

}// namespace bank

// @abi table recipient i64
struct recipient
{
  account_name creditor;
  account_name recipient_account;
  uint64_t created_at; // unix time, in seconds
  uint64_t updated_at; // unix time, in seconds

  account_name primary_key() const { return creditor; }

  EOSLIB_SERIALIZE(recipient, (creditor)(recipient_account)(created_at)(updated_at));
};
typedef multi_index<N(recipient), recipient> recipient_table;


