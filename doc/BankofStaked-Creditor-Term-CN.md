# BankofStaked Creditor 公开计划

## 关于 BankofStaked

BankofStaked 是由 EOSLaoMao 团队开发并维护的一款 EOS 资源自助租赁智能合约（[https://eoslaomao.com/bankofstaked](https://eoslaomao.com/bankofstaked)）。 
该合约巧妙地利用 EOS 权限系统，实现了无需转币，零资金风险，且不影响投票权的自动租赁功能。

BankofStaked 上线两个月以来（2018 年 10 月正式上线），已经累计自动完成了 40,000 多笔订单，累计自动贷出 3,000,000 EOS。同时，BankofStaked 还联合了 EOS 社区的 27 个 BP 向用户提供免费的 CPU 救急计划。截至目前，BankofStaked 已经给超过 30,000 个 EOS 账户提供了免费救急服务。MEET.ONE, imToken, EOS.LIVE 等钱包工具也已经接入该功能。

### Creditor 开放计划

随着 EOS 生态的发展，BankofStaked 的业务量迅速增长，BankofStaked 目前的存量 EOS 已经无法满足日益增长的业务需求。为此我们现将 BankofStaked 的 Creditor 功能向 EOS 社区限量开放。

第一期开放额度为 100 万 EOS，满额即止。由于 Creditor 账户的设置需要一定的 cleos 命令行工具基础，请务必评估风险后，自行决定是否参与。由于自己操作或设置不当导致的问题，BankofStaked 概不负责。

Creditor 定义: 经 BankofStaked 团队审核通过，加入到 BankofStaked 出租池，通过 BankofStaked 智能合约自动出租 EOS 获取收益的 EOS 账户。

![#f03c15](https://placehold.it/15/f03c15/000000?text=+) 友情提醒：BankofStaked 不会以任何名义让 Creditor 向我方进行转账，请知悉。

### Creditor 要求

1. 有一个独立的 EOS 账户用做 Creditor，且账户余额大于 20,000 EOS。
2. 该 Creditor 账户需部署 BankofStaked 提供的安全抵押合约（详情参见：[SafeDelegatebw](https://github.com/EOSLaoMao/SafeDelegatebw)）。
3. 该 Creditor 账户需授权安全抵押合约的抵押和解抵押权限给 BankofStaked（详情参见：[SafeDelegatebw](https://github.com/EOSLaoMao/SafeDelegatebw)）。
4. 该 Creditor 账户需确保账户自身具有足够的 RAM/CPU/NET 资源（详情参见：[Creditor 资源指引](./BankofStaked-Creditor-Resource-Guide.md)）。

### Creditor 义务

1. 加入时，需满足上述 Creditor 要求，且需要通过 BankofStaked 团队的审核。
2. 在提供服务期间，Creditor 不可以私自将正在出租的 EOS 提前解除抵押。
3. 在提供服务期间，Creditor 不可以私自将抵押和解抵押权限从 BankofStaked 智能合约撤除。
4. 退出 Creditor 需提前申请，并需等待正在出租的 EOS 到期自动解除抵押之后，方可退出。

### Creditor 权利

1. 在遵守上述 Creditor 义务的前提下，享有 EOS 租金收益的 90%。租金收益会在每笔订单到期之后自动分配至 Cerditor 账户。
2. 监督 BankofStaked 团队的开发进度。

### BankofStaked 权利

1. 享有出租方案的定价权。
2. 享有将违反规则的 Creditor 踢除的权利。

### BankofStaked 义务

1. 维护 BankofStaked 项目的发展。
2. 开发 BankofStaked 新功能，修复可能的问题。

其他未尽事宜，以 [BankofStaked Ricardian Contract](../rc/bankofstaked-ricardian-clauses.md) 为准。

有意参与者，请加入 Telegram 群聊：[https://t.me/BOSCreditor](https://t.me/BOSCreditor)

![#f03c15](https://placehold.it/15/f03c15/000000?text=+) 友情提醒：BankofStaked 不会以任何名义让 Creditor 向我方进行转账，请知悉。
