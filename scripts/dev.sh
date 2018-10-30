./scripts/deploy.sh
./scripts/set_plan.sh
API=${1:-http://localhost:8888}
cleos -u $API push action bankofstaked addcreditor '{"account": "voter1", "for_free": 1, "free_memo": "A gift from EOSLaoMao team"}' -p bankofstaked
cleos -u $API push action bankofstaked addcreditor '{"account": "voter2", "for_free": 0, "free_memo": ""}' -p bankofstaked
cleos -u $API push action bankofstaked addcreditor '{"account": "freestaking1", "for_free": 1, "free_memo": "A gift from EOSLaoMao team"}' -p bankofstaked
cleos -u $API push action bankofstaked addcreditor '{"account": "fundstaking1", "for_free": 0, "free_memo": ""}' -p bankofstaked
cleos -u $API push action bankofstaked addcreditor '{"account": "freestaking2", "for_free": 1, "free_memo": "A gift from EOSLaoMao team"}' -p bankofstaked
cleos -u $API push action bankofstaked addcreditor '{"account": "fundstaking2", "for_free": 0, "free_memo": ""}' -p bankofstaked
cleos -u $API push action bankofstaked activate '{"account": "voter1"}' -p bankofstaked
cleos -u $API push action bankofstaked activate '{"account": "voter2"}' -p bankofstaked
./scripts/set_perm.sh bankofstaked
./scripts/add_perm.sh voter1
./scripts/add_perm.sh voter2
./scripts/add_perm.sh freestaking1
./scripts/add_perm.sh freestaking2
./scripts/add_perm.sh fundstaking1
./scripts/add_perm.sh fundstaking2
