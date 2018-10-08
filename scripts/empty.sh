API=${1:-http://localhost:8888}
cleos -u $API push action bankofstaked empty "" -p bankofstaked
