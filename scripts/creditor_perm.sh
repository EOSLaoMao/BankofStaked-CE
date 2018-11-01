#!/bin/bash

ACCOUNT=$1
PKEY=${2:-EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV}
SAFE=${3:-false}
API=${4:-http://localhost:8888}
PERM_ACCOUNT=${5:-bankofstaked}
PERMISSION=${6:-eosio.code}

cleos set account permission $ACCOUNT creditorperm '{"threshold": 1,"keys": [{"key": "'$PKEY'","weight": 1}],"accounts": [{"permission":{"actor":"'$PERM_ACCOUNT'","permission":"'$PERMISSION'"},"weight":1}]}'  "active" -p $ACCOUNT@active
if [ "$SAFE" = true ] ; then
  cleos set action permission $ACCOUNT $ACCOUNT delegatebw creditorperm -p $ACCOUNT@active
else
  cleos set action permission $ACCOUNT eosio delegatebw creditorperm -p $ACCOUNT@active
fi
cleos set action permission $ACCOUNT eosio undelegatebw creditorperm -p $ACCOUNT@active
