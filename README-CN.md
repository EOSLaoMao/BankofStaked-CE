<p align="center">
  <img width="200" height="200" src="./BankofStaked-logo.png">
</p>

<div align="center">
	<h1>Bank of Staked</h1>
	<h2>CPU&NET 自助租赁合约</h2>
</div>


<a href="./README.md">English Version</a>

### 关于
BankofStaked 智能合约旨在向 EOS 用户和开发者提供便捷的 CPU 和 NET 资源租赁服务。

该合约由 `EOSLaoMao` 团队开发。

### 设计思路


BankofStaked 的设计初衷，是希望提供便捷的 CPU 和 NET 自助租赁体验。用户只需要向 BankofStaked 合约发送一笔符合要求的 EOS 转账，即可完成 CPU 和 NET 资源的租赁，并且，在租赁到期之后，合约也将自动取消 CPU 和 NET 的抵押，实现完全的自动化。你可以把它视为一台 CPU & NET 资源的自动贩卖机。

BankofStaked 合约的主要业务逻辑由下面 3 张数据表（table）构成：

#### 1. Plan 表

Plan 用于存储 BankofStaked 合约目前可用的价格方案，其主要字段如下：

```
price			asset; // 价格（单位为 EOS）
cpu			asset; // 该价格下提供的 CPU（单位为 EOS）
net			asset; // 该价格下提供的 NET（单位为 EOS）
duration		unit64; // 服务有效期（单位为分钟）
is_free			uint64; // 服务是否免费，如果 is_free 字段为 1， 用户转入 BankofStaked 合约的 EOS 将瞬间自动返还
...
```

#### 2. Creditor 表



`Creditor` 表存储的是真正给用户提供 CPU 和 NET 抵押的账户。

当合约收到符合条件的 EOS 转账的时候，合约将从 `Creditor` 表中查找符合条件的“贷出人”账户，该账户将自动为用户抵押相应的 CPU 和 NET 资源。在服务到期之后，“贷出人”账户也将自动解除这笔抵押。

```
account			account_name;
is_active		uint64;
...
```

`is_active` 字段表示该“贷出人”账户是否提供租赁服务。

在生产环境中，`Creditor` 表中应该提供多位“贷出人”，并采用合适的轮换策略，以保证非活跃的贷出人账户中解除抵押尚未到账的 EOS 及时到账。

#### 3. Order 表

Order 表存储的，是服务还在有效期内的订单。其主要字段如下：

```
buyer				account_name;
creditor			account_name;
beneficiary			account_name;
cpu_staked			asset;
net_staked			asset;
expire_at			uint64;
...
```

`buyer` 存储的是下单账户，也就是向 BankofStaked 合约转账 EOS 购买租赁服务的账户。（如果有退款发生，退款也将返还给 `buyer` 账户）

`beneficiary` 存储的是该订单真正的受益账户，`buyer` 可以在转账的 memo 中指定受益账户，未指定的话，`beneficiary` 跟 `buyer` 相同。

`creditor` 存储的是真正给 `beneficiary` 做抵押的“贷出人”账户。

`cpu_staked` 和 `net_staked` 记录了抵押的 CPU 和 NET 的多少。

`expire_at` 表示租赁服务的过期时间，到期后，订单记录将被自动删除。


除了上述的 3 张主要的数据表以外，还有一些辅助的数据表，比如：

`freelock` 表，用来给免费的 Plan 加锁，防止单一用户滥用。

`history` 表，用来记录过期之后被删除的历史订单。

`blacklist` 表，用于管理黑名单，禁止特定用户使用租赁服务。

![Process](./Order-Process-of-BankofStaked.svg)


# EOS 主网上的 BankofStaked 合约

### 如何使用

我们已经将 BankofStaked 合约部署在了 EOS 主网，以及 Kylin 和 Jungle 测试网络：

EOS 主网：https://www.myeoskit.com/#/tx/bankofstaked

Jungle 测试网：https://jungle.bloks.io/account/bankofstaked

Kylin 测试网：https://kylin.bloks.io/account/bankofstaked



#### 第一步：查询可用的价格方案

查询 `Plan` 表，获取当前可用的价格方案：


```
cleos -u https://api1.eosasia.one get table bankofstaked bankofstaked plan

{
  "rows": [{
      "id": 0,
      "price": "0.1000 EOS",
      "is_free": "1",
      "cpu": "1.0000 EOS",
      "net": "0.0000 EOS",
      "duration": 60,
      "created_at": 1535965927,
      "updated_at": 1535965927
    },
  ],
  "more": false
}
```

`price` 表示你需要转给 `bankofstaked` 的 EOS，`cpu` and `net` 表示该价格下，你将获得的 CPU 和 NET 资源，`duration` 表示租赁期限，单位为分钟。

`is_free` 表示该价格计划是否免费（用户成功购买免费的价格计划之后，会立即获得全额的退款）

我们将很快提供更多的价格方案。

#### 第二步：转账购买 CPU&NET 资源


目前 BankofStaked 在 EOS 主网上暂只提供 1 个免费计划。你可以向 `bankofstaked` 账户转账 `0.1 EOS` 立即获得 `1 EOS` 的 CPU 抵押一小时服务。并且 `0.1 EOS` 的转账也将被合约瞬间返还。

你可以直接通过 EOS 钱包进行转账，也可以使用命令行的方式：

```
cleos -u https://api1.eosasia.one transfer 你的账户 bankofstaked "0.1 EOS" -p 你的账户@active
```


转账成功后，你将立即获得全额的返还，并且立即获得 `1 EOS` CPU 的抵押。一个小时之后，该抵押将自动解除。（注：同一个账户每 24 个小时，只能购买一次免费服务）



---

如果你是 BP 或者 EOS 持有者，想要跟我们一起向用户提供免费的救急抵押服务，或者向开发者提供租赁服务，欢迎联系我们：contact@eoslaomao.com


built with love by EOSLaoMao Team. :)

Icon made by Freepik from www.flaticon.com, special thanks~
