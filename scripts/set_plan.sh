API=${1:-http://localhost:8888}
cleos -u $API push action bankofstaked setplan '{"price": "0.2000 EOS", "cpu": "14.0000 EOS", "net": "1.0000 EOS", "duration": 10080, "is_free": false}' -p bankofstaked
cleos -u $API push action bankofstaked setplan '{"price": "0.5000 EOS", "cpu": "38.0000 EOS", "net": "2.0000 EOS", "duration": 10080, "is_free": false}' -p bankofstaked
cleos -u $API push action bankofstaked setplan '{"price": "1.0000 EOS", "cpu": "95.0000 EOS", "net": "5.0000 EOS", "duration": 10080, "is_free": false}' -p bankofstaked
cleos -u $API push action bankofstaked setplan '{"price": "2.0000 EOS", "cpu": "210.0000 EOS", "net": "10.0000 EOS", "duration": 10080, "is_free": false}' -p bankofstaked
