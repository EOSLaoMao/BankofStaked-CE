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
| acroeosrndev | 500.0016 EOS | 9.33 | 80.00 | ❌ |
| bankofeosys2 | 300.1014 EOS | 4.83 | 48.02 | ❌ |
| bankofstkarg | 500.0015 EOS | 32.03 | 80.00 | ❌ |
| bosauthority | 89.0103 EOS | 11.03 | 14.24 | ❌ |
| cannonstaked | 200.0013 EOS | 11.39 | 32.00 | ❌ |
| charity.bank | 10100.0015 EOS | 200.82 | 1616.00 | ❌ |
| dublinstaked | 506.0003 EOS | 79.02 | 80.96 | ❌ |
| eos42reserve | 8520.0041 EOS | 7.96 | 1363.20 | ❌ |
| eosasia.bp | 498.0016 EOS | 88.40 | 79.68 | ✅ |
| eosbeijinghp | 100.0015 EOS | 58.34 | 16.00 | ✅ |
| eosbixinbank | 500.0013 EOS | 9.63 | 80.00 | ❌ |
| eoscafestake | 1005.0012 EOS | 9.33 | 160.80 | ❌ |
| eoseco.bp | 1001.0013 EOS | 112.39 | 160.16 | ❌ |
| eosgravitygo | 100.0015 EOS | 9.32 | 16.00 | ❌ |
| eospacestake | 100.2013 EOS | 6.95 | 16.03 | ❌ |
| eosriobrfree | 5.0006 EOS | 7.20 | 0.80 | ✅ |
| eostribefree | 102.0003 EOS | 11.32 | 16.32 | ❌ |
| freestakeswe | 1004.0003 EOS | 319.76 | 160.64 | ✅ |
| fundstostake | 21.9016 EOS | 71.78 | 3.50 | ✅ |
| jedaaastaked | 500.0007 EOS | 5.35 | 80.00 | ❌ |
| meetone1free | 500.0007 EOS | 103.89 | 80.00 | ✅ |



