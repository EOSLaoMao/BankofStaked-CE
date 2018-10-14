API=${1:-http://localhost:8888}
cleos -u $API push action bankofstaked setplan '{"price": "0.1000 EOS", "cpu": "0.5000 EOS", "net": "0.5000 EOS", "duration": 360, "is_free": true}' -p bankofstaked
cleos -u $API push action bankofstaked setplan '{"price": "0.0900 EOS", "cpu": "36.0000 EOS", "net": "4.0000 EOS", "duration": 10080, "is_free": false}' -p bankofstaked
cleos -u $API push action bankofstaked setplan '{"price": "0.2000 EOS", "cpu": "36.0000 EOS", "net": "4.0000 EOS", "duration": 10080, "is_free": false}' -p bankofstaked
cleos -u $API push action bankofstaked setplan '{"price": "0.5000 EOS", "cpu": "100.0000 EOS", "net": "10.0000 EOS", "duration": 10080, "is_free": false}' -p bankofstaked
cleos -u $API push action bankofstaked setplan '{"price": "1.0000 EOS", "cpu": "220.0000 EOS", "net": "20.0000 EOS", "duration": 10080, "is_free": false}' -p bankofstaked
cleos -u $API push action bankofstaked setplan '{"price": "2.0000 EOS", "cpu": "480.0000 EOS", "net": "40.0000 EOS", "duration": 10080, "is_free": false}' -p bankofstaked
cleos -u $API push action bankofstaked activateplan '{"price": "0.0900 EOS", "is_active": false}' -p bankofstaked
cleos -u $API push action bankofstaked activateplan '{"price": "0.1000 EOS", "is_active": true}' -p bankofstaked
cleos -u $API push action bankofstaked activateplan '{"price": "0.2000 EOS", "is_active": true}' -p bankofstaked
cleos -u $API push action bankofstaked activateplan '{"price": "0.5000 EOS", "is_active": true}' -p bankofstaked
cleos -u $API push action bankofstaked activateplan '{"price": "1.0000 EOS", "is_active": true}' -p bankofstaked
cleos -u $API push action bankofstaked activateplan '{"price": "2.0000 EOS", "is_active": true}' -p bankofstaked
