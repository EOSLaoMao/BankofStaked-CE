API=${1:-http://localhost:8888}
sleep 1
cleos -u $API system newaccount voter3 bankofstaked EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-cpu "100 EOS" --stake-net "100 EOS" --buy-ram "100 EOS"
cleos -u $API system newaccount voter3 goodcreditor EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-cpu "100 EOS" --stake-net "100 EOS" --buy-ram "100 EOS"
cleos -u $API system newaccount voter3 safecreditor EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-cpu "100 EOS" --stake-net "100 EOS" --buy-ram "100 EOS"
cleos -u $API system newaccount voter3 bestcreditor EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-cpu "100 EOS" --stake-net "100 EOS" --buy-ram "100 EOS"
cleos -u $API system newaccount voter3 creditortest EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-cpu "100 EOS" --stake-net "100 EOS" --buy-ram "100 EOS"
cleos -u $API system newaccount voter3 creditorgood EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-cpu "100 EOS" --stake-net "100 EOS" --buy-ram "100 EOS"
cleos -u $API system newaccount voter3 creditorsafe EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-cpu "100 EOS" --stake-net "100 EOS" --buy-ram "100 EOS"
cleos -u $API system newaccount voter3 creditorbest EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-cpu "100 EOS" --stake-net "100 EOS" --buy-ram "100 EOS"
cleos -u $API transfer voter3 creditorbest "2.5 EOS"
cleos -u $API transfer voter3 creditorgood "2 EOS"
cleos -u $API transfer voter3 creditortest "1 EOS"
cleos -u $API transfer voter3 creditorsafe "100 EOS"
cleos -u $API transfer voter3 safecreditor "10 EOS"
cleos -u $API transfer voter3 goodcreditor "3 EOS"
cleos -u $API transfer voter3 bestcreditor "0.1 EOS"
sleep 1
cleos -u $API set contract bankofstaked bankofstaked
sleep 1
cleos -u $API system newaccount voter3 stakedincome EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --buy-ram "100 EOS" --stake-cpu "100 EOS" --stake-net "100 EOS"
sleep 1
cleos -u $API system newaccount voter3 masktransfer EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --buy-ram "100 EOS" --stake-cpu "100 EOS" --stake-net "100 EOS"
sleep 1

cleos -u $API set contract masktransfer proxytoken
sleep 1
./proxytoken/perms.sh masktransfer EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
sleep 1

cleos -u $API set contract voter1 safedelegatebw
sleep 1
./safedelegatebw/delegate_perm.sh voter1
sleep 1
./safedelegatebw/creditor_perm.sh voter1
sleep 1
./scripts/creditor_perm.sh voter2 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
./scripts/creditor_perm.sh goodcreditor EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
./scripts/creditor_perm.sh bestcreditor EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
./scripts/creditor_perm.sh safecreditor EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
./scripts/creditor_perm.sh creditorgood EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
./scripts/creditor_perm.sh creditorbest EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
./scripts/creditor_perm.sh creditorsafe EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
./scripts/creditor_perm.sh creditortest EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
sleep 1
./scripts/bank_perm.sh bankofstaked EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
sleep 1
cleos -u $API push action bankofstaked addcreditor '{"account": "voter1", "for_free": 1, "free_memo": "A gift from EOSLaoMao team"}' -p bankofstaked
sleep 1
cleos -u $API push action bankofstaked addcreditor '{"account": "voter2", "for_free": 0, "free_memo": ""}' -p bankofstaked
sleep 1
cleos -u $API push action bankofstaked addcreditor '{"account": "safecreditor", "for_free": 0, "free_memo": ""}' -p bankofstaked
sleep 1
cleos -u $API push action bankofstaked addcreditor '{"account": "goodcreditor", "for_free": 0, "free_memo": ""}' -p bankofstaked
sleep 1
cleos -u $API push action bankofstaked addcreditor '{"account": "bestcreditor", "for_free": 0, "free_memo": ""}' -p bankofstaked
sleep 1
cleos -u $API push action bankofstaked addcreditor '{"account": "creditortest", "for_free": 0, "free_memo": ""}' -p bankofstaked
sleep 1
cleos -u $API push action bankofstaked addcreditor '{"account": "creditorgood", "for_free": 0, "free_memo": ""}' -p bankofstaked
sleep 1
cleos -u $API push action bankofstaked addcreditor '{"account": "creditorbest", "for_free": 0, "free_memo": ""}' -p bankofstaked
sleep 1
cleos -u $API push action bankofstaked addcreditor '{"account": "creditorsafe", "for_free": 0, "free_memo": ""}' -p bankofstaked
sleep 1
cleos -u $API push action bankofstaked activate '{"account": "voter1"}' -p bankofstaked
sleep 1
cleos -u $API push action bankofstaked activate '{"account": "voter2"}' -p bankofstaked
sleep 1
cleos -u $API push action bankofstaked addsafeacnt '{"account": "voter1"}' -p bankofstaked
sleep 1

cleos -u $API push action bankofstaked setplan '{"price": "0.1000 EOS", "cpu": "0.5000 EOS", "net": "0.5000 EOS", "duration": 1, "is_free": true}' -p bankofstaked
sleep 1
cleos -u $API push action bankofstaked setplan '{"price": "0.2000 EOS", "cpu": "36.0000 EOS", "net": "4.0000 EOS", "duration": 1, "is_free": false}' -p bankofstaked
sleep 1
cleos -u $API push action bankofstaked activateplan '{"price": "0.1000 EOS", "is_active": true}' -p bankofstaked
sleep 1
cleos -u $API push action bankofstaked activateplan '{"price": "0.2000 EOS", "is_active": true}' -p bankofstaked
sleep 1


# add voter3 to whitelist table
cleos -u $API push action bankofstaked addwhitelist '{"account": "voter3", "capacity": 1000}' -p bankofstaked
sleep 1
