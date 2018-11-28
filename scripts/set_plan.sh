API=${1:-http://localhost:8888}
cleos -u $API push action -s -j -d bankofstaked setplan '{"price": "0.2000 EOS", "cpu": "55.0000 EOS", "net": "5.0000 EOS", "duration": 10080, "is_free": false}' -p bankofstaked >> plan.json
cleos -u $API push action -s -j -d bankofstaked setplan '{"price": "0.5000 EOS", "cpu": "165.0000 EOS", "net": "15.0000 EOS", "duration": 10080, "is_free": false}' -p bankofstaked >> plan.json
cleos -u $API push action -s -j -d bankofstaked setplan '{"price": "1.0000 EOS", "cpu": "370.0000 EOS", "net": "30.0000 EOS", "duration": 10080, "is_free": false}' -p bankofstaked >> plan.json
cleos -u $API push action -s -j -d bankofstaked setplan '{"price": "2.0000 EOS", "cpu": "850.0000 EOS", "net": "50.0000 EOS", "duration": 10080, "is_free": false}' -p bankofstaked >> plan.json
cleos -u $API push action -s -j -d bankofstaked setplan '{"price": "80.0000 EOS", "cpu": "9900.0000 EOS", "net": "100.0000 EOS", "duration": 10080, "is_free": false}' -p bankofstaked >> plan.json
cleos -u $API push action -s -j -d bankofstaked setplan '{"price": "80.0000 EOS", "cpu": "9900.0000 EOS", "net": "100.0000 EOS", "duration": 10080, "is_free": false}' -p bankofstaked >> plan.json
cleos -u $API push action -s -j -d bankofstaked activateplan '{"price": "80.0000 EOS", "is_active": true}' -p bankofstaked >> plan.json
cleos -u $API push action -s -j -d bankofstaked activateplan '{"price": "200.0000 EOS", "is_active": false}' -p bankofstaked >> plan.json
