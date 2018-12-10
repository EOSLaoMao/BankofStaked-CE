# BankofStaked Creditor Resource Guide



## RAM

### free creditor

`
RAM = (liquid_balance) * 0.16 (kb)
`

eg. creditor account with 1000 liquid EOS should also have 160kb unused RAM in this account initially.


### paid creditor

`
RAM = (liquid_balance) * 0.12 / 30 (kb)
`

*30 is the minimum EOS quanity for each paid order.

eg. creditor account with 10000 liquid EOS should also have 40kb unused RAM in this account initially.


# CPU

This is a tricky one. You have to make sure your creditor account has enough CPU for auto-undelegation, otherwise auto-undelegation will not be executed. Good news is BankofStaked has a built-in passive check to make sure to trigger undelegation again and again.



# Current creditor resource status

Here is the creditor RAM resource summary, please purchase enough ram for your account according to this chart if your account has no enough RAM.


## FREE CREDITORS

| Account | Balance | RAM Owned(kb) | RAM Requird(kb) | Enough RAM? |
| ------- | ------- | --------- | ----------- | ----------- |
| acroeosrndev | 500.0023 EOS | 42.54 | 80.00 | ❌ |
| bankofeosys2 | 300.1019 EOS | 4.83 | 48.02 | ❌ |
| bankofstkarg | 500.0024 EOS | 86.75 | 80.00 | ✅ |
| bosauthority | 89.0118 EOS | 11.03 | 14.24 | ❌ |
| cannonstaked | 200.0020 EOS | 32.30 | 32.00 | ✅ |
| charity.bank | 10100.0026 EOS | 200.82 | 1616.00 | ❌ |
| dublinstaked | 506.0017 EOS | 79.02 | 80.96 | ❌ |
| eos42reserve | 8520.0052 EOS | 7.96 | 1363.20 | ❌ |
| eosasia.bp | 498.0027 EOS | 88.40 | 79.68 | ✅ |
| eosbeijinghp | 100.0028 EOS | 58.34 | 16.00 | ✅ |
| eosbixinbank | 500.0025 EOS | 89.23 | 80.00 | ✅ |
| eoscafestake | 994.8943 EOS | 168.52 | 159.18 | ✅ |
| eoseco.bp | 951.0023 EOS | 635.15 | 152.16 | ✅ |
| eosgravitygo | 100.0024 EOS | 9.32 | 16.00 | ❌ |
| eospacestake | 100.2025 EOS | 16.90 | 16.03 | ✅ |
| eosriobrfree | 405.0016 EOS | 7.51 | 64.80 | ❌ |
| eostribefree | 102.0017 EOS | 11.32 | 16.32 | ❌ |
| freestakeswe | 1004.0016 EOS | 319.76 | 160.64 | ✅ |
| fundstostake | 12.9024 EOS | 71.78 | 2.06 | ✅ |
| jedaaastaked | 367.3101 EOS | 84.05 | 58.77 | ✅ |
| meetone1free | 500.0010 EOS | 103.89 | 80.00 | ✅ |


## PAID CREDITORS
| Account | Balance | RAM Owned(kb) | RAM Requird(kb) | Enough RAM? |
| ------- | ------- | --------- | ----------- | ----------- |
| bankofeosys1 | 1026.2817 EOS | 10.53 | 4.11 | ✅ |
| cyberneticsx | 4470.5650 EOS | 24.30 | 17.88 | ✅ |
| dublinsaving | 20000.9284 EOS | 4865.03 | 80.00 | ✅ |
| eosbeijingbk | 1062.1522 EOS | 13.17 | 4.25 | ✅ |
| eosriostaked | 14159.9581 EOS | 10.20 | 56.64 | ❌ |
| eostribecred | 1014.1014 EOS | 11.32 | 4.06 | ✅ |
| jedacreditor | 40021.5302 EOS | 113.82 | 160.09 | ❌ |
| paidstakeswe | 13161.7804 EOS | 319.76 | 52.65 | ✅ |
| serenityhome | 13247.9882 EOS | 108.11 | 52.99 | ✅ |
| staking.bank | 34551.5327 EOS | 173.50 | 138.21 | ✅ |
| stakingfunda | 19195.5208 EOS | 129.28 | 76.78 | ✅ |
| stakingfundb | 10195.8706 EOS | 119.57 | 40.78 | ✅ |
| stakingfundc | 10167.2706 EOS | 119.57 | 40.67 | ✅ |
| stakingfundd | 30172.1106 EOS | 119.57 | 120.69 | ❌ |
| stakingfunde | 10189.1007 EOS | 119.57 | 40.76 | ✅ |
| stakingfundf | 20432.8848 EOS | 119.57 | 81.73 | ✅ |
| stakingfundg | 10248.2183 EOS | 119.57 | 40.99 | ✅ |
| stakingfundh | 10174.4407 EOS | 119.57 | 40.70 | ✅ |
| stakingfundi | 17153.0407 EOS | 119.57 | 68.61 | ✅ |
| sxzzsxzzsxz1 | 151002.7887 EOS | 116.50 | 604.01 | ❌ |
| sxzzsxzzsxzz | 101917.5803 EOS | 110.92 | 407.67 | ❌ |
