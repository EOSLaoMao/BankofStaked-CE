<p align="center">
  <img width="200" height="200" src="./BankofStaked-logo.png">
</p>

<div align="center">
	<h1>Bank of Staked</h1>
	<h2>a self-serve CPU&NET Vending Machine</h2>
</div>

<a href="./README-CN.md">中文版 README</a>

<a href="./README-KR.md">한국의 README</a>

### About
Bank of Staked is an EOS smart contract aiming to provide cheap CPU&NET lease to both EOS users and developers. This contract is build by `EOSLaoMao Team`.

Bank of Staked is now live on EOS Mainnet providing 1 free emergency plan and 3 paid plans. With 20+ block producers providing free creditors,  free emergency plan is now able to servce 20K EOS accounts simultaneously.

Check it out: https://eoslaomao.com/bankofstaked

For more details, check announcement here: https://steemit.com/eos/@eoslaomao/announcing-bankofstaked-a-self-serve-cpu-and-net-resource-vending-machine-supported-by-block-producers

### Design

The user experience Bank of Staked wants to achieve is that any account could get CPU&NET delegated automatically through a simple transfer, no more action needed. And the undelegate process will also be triggered automatically.

The main logic of Bank of Staked are realized through the following three tables:

#### 1. Plan Table

Plan table holds all available plans user can choose, main fields are:

```
price			asset; //plan price
cpu			asset; // delegated cpu this plan offers.
net			asset; // delegated net this plan offers.
duration		unit64; //the period of time service gonna last, in minutes.
is_free			uint64; //free plan or not, if is_free is 1, order will be auto refunded.
...
```

#### 2. Creditor Table

`Creditors` are the actual accounts who delegate and undelegate. When a valid transfer happens, the contract will try to find `active` creditor to do an auto delegation using that creditor account.

```
account			account_name;
is_active		uint64;
...
```

`is_active` indicates if this creditor is ready to serve new orders.

in production, you should always have creditors shifting like X days in a roll(X depends on plans it provide), so that non-active creditors have enough time to get their undelegated token back.

#### 3. Order Table

Order table consists of active order records, by active, it means these orders are not expired.

```
buyer				account_name;
creditor			account_name;
beneficiary			account_name;
cpu_staked			asset;
net_staked			asset;
expire_at			uint64;
...
```

`buyer` is the account who did the actual transfer in the first place, if any refund happens, token will be refunded to `buyer` account also.

`beneficiary` is the actual account who get cpu and net delegated. `buyer` can specify this account in transfer memo if he/she like.

`creditor` is the account who did delegation.

`cpu_staked` and `net_staked` is CPU&NET delegated in this order.

`expire_at` is when this order will expire. After order expired, order record will be deleted from Order Table.


There are also several other tables facilitating this contract. such as,

`freelock` table, used to lock free plan for each account for 24 hours.

`history` table, used to store meta data of deleted expired order.

`blacklist`, used to blacklist certain account from using `bankofstaked` contract.


![Process](./Order-Process-of-BankofStaked.svg)
