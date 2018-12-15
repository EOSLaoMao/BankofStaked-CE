API=${1:-http://localhost:8888}
cleos -u $API push action -s -j -d bankofstaked setplan '{"price": "0.2000 EOS", "cpu": "22.0000 EOS", "net": "2.0000 EOS", "duration": 10080, "is_free": false}' -p bankofstaked >> plan.json
cleos -u $API push action -s -j -d bankofstaked setplan '{"price": "0.5000 EOS", "cpu": "58.0000 EOS", "net": "2.0000 EOS", "duration": 10080, "is_free": false}' -p bankofstaked >> plan.json
cleos -u $API push action -s -j -d bankofstaked setplan '{"price": "1.0000 EOS", "cpu": "118.0000 EOS", "net": "2.0000 EOS", "duration": 10080, "is_free": false}' -p bankofstaked >> plan.json
cleos -u $API push action -s -j -d bankofstaked setplan '{"price": "2.0000 EOS", "cpu": "238.0000 EOS", "net": "2.0000 EOS", "duration": 10080, "is_free": false}' -p bankofstaked >> plan.json
cleos -u $API push action -s -j -d bankofstaked setplan '{"price": "180.0000 EOS", "cpu": "9900.0000 EOS", "net": "100.0000 EOS", "duration": 40320, "is_free": false}' -p bankofstaked >> plan.json
cleos -u $API push action -s -j -d bankofstaked setplan '{"price": "800.0000 EOS", "cpu": "49500.0000 EOS", "net": "500.0000 EOS", "duration": 40320, "is_free": false}' -p bankofstaked >> plan.json
#cleos -u $API push action -s -j -d bankofstaked activateplan '{"price": "130.0000 EOS", "is_active": false}' -p bankofstaked >> plan.json
#cleos -u $API push action -s -j -d bankofstaked activateplan '{"price": "580.0000 EOS", "is_active": false}' -p bankofstaked >> plan.json
#cleos -u $API push action -s -j -d bankofstaked activateplan '{"price": "180.0000 EOS", "is_active": true}' -p bankofstaked >> plan.json
#cleos -u $API push action -s -j -d bankofstaked activateplan '{"price": "780.0000 EOS", "is_active": true}' -p bankofstaked >> plan.json
