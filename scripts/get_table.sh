#/bin/bash

API=${1:-http://localhost:8888}
limit=${2:-100}
v=921459758687; k=creditor; declare "table_$k=$v";
v=921459758687; k=safecreditor; declare "table_$k=$v";
v=921459758687; k=history; declare "table_$k=$v";
v=921459758687; k=order; declare "table_$k=$v";
v=921459758687; k=freelock; declare "table_$k=$v";
v=921459758687; k=blacklist; declare "table_$k=$v";
v=921459758687; k=whitelist; declare "table_$k=$v";
v=bankofstaked; k=plan; declare "table_$k=$v";


for name in creditor safecreditor plan order history freelock blacklist whitelist
do
  echo "==============TABLE "$name"========"
  scope="table_$name"
  cleos -u $API get table bankofstaked ${!scope} $name -l " $limit"
  echo "------------------------------------"
  echo
done
