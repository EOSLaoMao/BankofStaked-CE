API=${1:-http://localhost:8888}
cleos -u $API push action bankofstaked addcreditor '{"account": "charity.bank", "for_free": 1, "free_memo": "A gift from EOSLaoMao team"}' -p bankofstaked
cleos -u $API push action bankofstaked addcreditor '{"account": "staking.bank", "for_free": 0, "free_memo": ""}' -p bankofstaked
cleos -u $API push action bankofstaked addcreditor '{"account": "eosbeijinghp", "for_free": 1, "free_memo": "gift from EOS Beijing, EOS Navigation: https://www.shensi.com"}' -p bankofstaked
cleos -u $API push action bankofstaked addcreditor '{"account": "eosbeijingbk", "for_free": 0, "free_memo": ""}' -p bankofstaked
cleos -u $API push action bankofstaked addcreditor '{"account": "meetone1free", "for_free": 1, "free_memo": "A gift from MEET.ONE team"}' -p bankofstaked
cleos -u $API push action bankofstaked addcreditor '{"account": "eosbixinbank", "for_free": 1, "free_memo": "A gift from EOSBIXIN team"}' -p bankofstaked
cleos -u $API push action bankofstaked addcreditor '{"account": "jedaaastaked", "for_free": 1, "free_memo": "A gift from JEDA team with love"}' -p bankofstaked
cleos -u $API push action bankofstaked addcreditor '{"account": "cannonstaked", "for_free": 1, "free_memo": "A gift from EOSCannon team"}' -p bankofstaked
cleos -u $API push action bankofstaked addcreditor '{"account": "acroeosrndev", "for_free": 1, "free_memo": "A gift from AcroEOS"}' -p bankofstaked
cleos -u $API push action bankofstaked addcreditor '{"account": "bankofeosys2", "for_free": 1, "free_memo": "A gift from EOSYS"}' -p bankofstaked
cleos -u $API push action bankofstaked addcreditor '{"account": "bankofeosys1", "for_free": 0, "free_memo": ""}' -p bankofstaked
cleos -u $API push action bankofstaked addcreditor '{"account": "eosasia.bp", "for_free": 1, "free_memo": "A gift from block producer eosasia11111"}' -p bankofstaked
cleos -u $API push action bankofstaked addcreditor '{"account": "eoseco.bp", "for_free": 1, "free_memo": "A gift from EOSeco"}' -p bankofstaked
cleos -u $API push action bankofstaked addcreditor '{"account": "eospacestake", "for_free": 1, "free_memo": "A gift from EOSpace"}' -p bankofstaked
cleos -u $API push action bankofstaked addcreditor '{"account": "bankofstkarg", "for_free": 1, "free_memo": "A gift from EOS Argentina"}' -p bankofstaked
#cleos -u $API push action bankofstaked activate '{"account": "charity.bank"}' -p bankofstaked
#cleos -u $API push action bankofstaked activate '{"account": "staking.bank"}' -p bankofstaked
#cleos -u $API push action bankofstaked activate '{"account": "eosbeijinghp"}' -p bankofstaked
#cleos -u $API push action bankofstaked activate '{"account": "acroeosrndev"}' -p bankofstaked
