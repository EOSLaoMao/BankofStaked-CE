#!/bin/bash

ACCOUNT=$1
PKEY=${2:-EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV}
PERM_ACCOUNT=${3:-bankofstaked}
PERMISSION=${4:-eosio.code}

cleos set account permission $ACCOUNT orderperm '{"threshold": 1,"keys": [{"key": "'$PKEY'","weight": 1}],"accounts": [{"permission":{"actor":"'$PERM_ACCOUNT'","permission":"'$PERMISSION'"},"weight":1}]}'  "active" -p $ACCOUNT@active
cleos set action permission $ACCOUNT bankofstaked customorder orderperm -p $ACCOUNT@active
