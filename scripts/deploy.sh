API=${1:-http://localhost:8888}
cleos -u $API system newaccount voter3 bankofstaked EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-cpu "100 EOS" --stake-net "100 EOS" --buy-ram "100 EOS"
cleos -u $API set contract bankofstaked bankofstaked
