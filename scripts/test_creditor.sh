ACCOUNT=$1
API=${2:-http://localhost:8888}
cleos -u $API push action bankofstaked addcreditor '{"account": "'$ACCOUNT'", "for_free": 0, "free_memo": ""}' -p bankofstaked -s -j -d > testcreditor.json
cleos -u $API push action bankofstaked addsafeacnt '{"account": "'$ACCOUNT'"}' -p bankofstaked -s -j -d >> testcreditor.json
cleos -u $API push action bankofstaked test '{"creditor": "'$ACCOUNT'"}' -p bankofstaked -s -j -d >> testcreditor.json
