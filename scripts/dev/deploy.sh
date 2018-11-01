API=${1:-http://localhost:8888}
cleos -u $API system newaccount voter3 bankofstaked EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-cpu "100 EOS" --stake-net "100 EOS" --buy-ram "100 EOS"
cleos -u $API set contract bankofstaked bankofstaked
cleos -u $API system newaccount voter3 masktransfer EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --buy-ram "100 EOS" --stake-cpu "100 EOS" --stake-net "100 EOS"

cleos -u $API set contract masktransfer proxytoken
./proxytoken/perms.sh masktransfer EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV

cleos -u $API set contract voter1 safedelegatebw
./safedelegatebw/setup_perm.sh voter1 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
./scripts/creditor_perm.sh voter1 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV true
./scripts/creditor_perm.sh voter2 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
./scripts/bank_perm.sh bankofstaked EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos -u $API push action bankofstaked addcreditor '{"account": "voter1", "for_free": 1, "free_memo": "A gift from EOSLaoMao team"}' -p bankofstaked
cleos -u $API push action bankofstaked addcreditor '{"account": "voter2", "for_free": 0, "free_memo": ""}' -p bankofstaked
cleos -u $API push action bankofstaked activate '{"account": "voter1"}' -p bankofstaked
cleos -u $API push action bankofstaked activate '{"account": "voter2"}' -p bankofstaked
cleos -u $API push action bankofstaked addsafeacnt '{"account": "voter1"}' -p bankofstaked

cleos -u $API push action bankofstaked setplan '{"price": "0.1000 EOS", "cpu": "0.5000 EOS", "net": "0.5000 EOS", "duration": 1, "is_free": true}' -p bankofstaked
cleos -u $API push action bankofstaked setplan '{"price": "0.2000 EOS", "cpu": "36.0000 EOS", "net": "4.0000 EOS", "duration": 1, "is_free": false}' -p bankofstaked
cleos -u $API push action bankofstaked activateplan '{"price": "0.1000 EOS", "is_active": true}' -p bankofstaked
cleos -u $API push action bankofstaked activateplan '{"price": "0.2000 EOS", "is_active": true}' -p bankofstaked


# add voter3 to whitelist table
cleos -u $API push action bankofstaked addwhitelist '{"account": "voter3", "capacity": 1000}' -p bankofstaked
