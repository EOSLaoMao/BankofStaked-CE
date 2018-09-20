<p align="center">
  <img width="200" height="200" src="./BankofStaked-logo.png">
</p>

<div align="center">
	<h1>Bank of Staked</h1>
	<h2>a self-serve CPU&NET Vending Machine</h2>
</div>

<a href="./README.md">English README</a>
<a href="./README-CN.md">中文版 README</a>


### Bank of Staked 에 대해서
뱅크 오브 스테이크 (Bank of Staked)는 EOS 사용자와 개발자 모두에게 값싼 CPU 및 NET 리스를 제공하기 위해 EOSLaoMao 팀이 작성한 EOS 스마트 컨트랙트입니다.


### Design

Bank of Staked의 UX는 모든 계정이 간단한 과정을 통해 CPU 와 NET을 자동으로 위임 받을 수 있게 하는 목표를 달성하고자 합니다. 이러한 목표에는 취소 프로세스의 자동실행도 포함됩니다.

스테이크 뱅크의 주요 로직은 다음 세 테이블을 통해 실현됩니다:

#### 1. Plan Table(계획 테이블)

계획 테이블은 사용자가 선택할 수 있는 모든 사용가능한 계획을 보유하고 있으며, 주요 필드는 다음과 같습니다:

```
price			asset; //plan price
cpu			asset; // delegated cpu this plan offers.
net			asset; // delegated net this plan offers.
duration		unit64; //the period of time service gonna last, in minutes.
is_free			uint64; //free plan or not, if is_free is 1, order will be auto refunded.
...
```

#### 2. Creditor Table (채권자 테이블)

채권자는 CPU 와 NET을 위임해주고 위임 받는 실제 주인들입니다. 유효한 양도 거래가 이루어지면 스마트 컨트랙트는 `is _active` 채권자를 찾아 해당 채권자의 계좌를 통해 자동으로 자원을 위임합니다.


```
account			account_name;
is_active		uint64;
...
```

`is_active`는 채권자가 위임을 할 준비가 되어있는지 알려줍니다.

프로덕션 환경에서는 채권자가 항상 3일 기준으로 바뀌게 하여 비활성화 상태인 채권자에게도 토큰을 돌려줄 수 있도록 합니다.

#### 3. Order Table (주문 테이블)

주문 테이블은 활성 주문 기록으로 구성되며, 활성이라는 것은 만료되지 않은 주문을 말합니다.

```
buyer				account_name;
creditor			account_name;
beneficiary			account_name;
cpu_staked			asset;
net_staked			asset;
expire_at			uint64;
...
```

구매자는 처음에 이체를 실제로 실행한 계정입니다. 환불 거래가 발생하면 토큰도 구매자 계정으로 환급됩니다.

수혜자는 CPU 와 NET을 실제로 위임받는 계정입니다. 구매자는 원하는 경우에 이 계좌를 송금 메모에 지정할 수 있습니다.

채권자는 토큰을 위임한 계좌입니다.

`cpu_staked` 와 `net_staked`는 이 순서대로 위임된 CPU 와 NET을 말합니다.

`expire_at` 은 주문이 만료되는 시기입니다. 주문이 만료되면 주문 기록이 주문 표에서 삭제됩니다.


이 계약을 용이하게 하는 다른 몇 가지 표가 더 있습니다. 예를 들어,

24시간 동안 각 계정의 무료 계획을 잠그는 데 사용되는 `freelock` table 이 있고,

이미 삭제된 만료된 주문의 메타 데이터를 저장하는 데 사용되는 `history` table,

Bank of Staked에 특정 계정을 블랙리스트 할 수 있는 `blacklist`가 있습니다.


![Process](./Order-Process-of-BankofStaked.svg)


# Bank of Staked on Mainnet

### How to use it

저희는 Banked of Staked를 메인넷에 올렸고, 아래의 링크에서 확인하실 수 있습니다: 

https://www.myeoskit.com/#/tx/bankofstaked


#### 1.Check available price plan

계획 테이블 쿼리를 통해 이용 가능한 계획 확인:

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

가격은 `duration`(특정 기간/분)동안 특정 `CPU`와 `NET`을 얻기 위해 `bankofstaked`로 전송해야 하는 EOS를 나타냅니다.

`is_free` 는 환불을 받을 수 있는지 알려줍니다 (환불이 적용되었다면 바로 진행됩니다).

현재 우리는 1개의 무료 계획(plan)을 제공하고 있습니다: `0.1 EOS`를 `60 min of 1 EOS CPU`에 스테이킹하면, 전송 후 0.1 EOS를 바로 환불받으실 수 있습니다.

다른 계획들이 빠른 시일 내에 추가될 것입니다.

#### 2.EOS를 전송하고 위임받으세요!

`bankofstaked`에 `0.1 EOS`를 전송하면, 스테이크된 `1 EOS`의 `cpu`를 `60 minutes`(60 분) 동안 이용하실 수 있습니다.

노트: `bankofstaked`는 시간이 되면 자동으로 토큰을 환급합니다.



```
cleos -u https://api1.eosasia.one transfer YOUR_ACCOUNT bankofstaked "0.1 EOS" -p YOUR_ACCOUNT@active
```

0.1EOS를 전송하면, 1 EOS의 CPU를 위임받을 수 있고, 60분 후에 지연 거래를 사용하여 자동으로 환급이 진행됩니다.
토큰을 전송한 후, `1 EOS of CPU`를 위임받을 수 있고, 60분 후에, 지연 거래(deferred transaction)를 사용하여 자동으로 환급이 진행됩니다.

---

당신이 EOS 채권자 기금에 기부하고자 하는 경우 (위임, 양도는 필요하지 않음), 또는 BP와 사용자들과 개발자들을 도울 의사가 있다면 contact@eoslaomao.com으로 문의해주세요.

EOSLaoMao 팀이 사랑으로 개발했습니다. :)

아이콘은 www.flaticon.com의 Freepik이 만들었습니다, 감사의 말씀 전합니다~

