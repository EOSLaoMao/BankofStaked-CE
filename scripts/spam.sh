API=${1:-http://localhost:8888}
for from in teststaking1 teststaking2 teststaking3 teststaking4 teststaking5
do
  price="0.0100 EOS"
  for to in eosecoeoseco eosbixincool eospaceioeos eosasia11111 eosstorebest 
  do
    cleos -u $API transfer $from bankofstaked "$price" "$to"
  done

  price="1.0000 EOS"
  for to in eoslaomaocom eoshuobipool eosfaucet111 eosio.faucet superoneiobp 
  do
    cleos -u $API transfer $from bankofstaked "$price" "$to"
  done

  price="2.0000 EOS"
  for to in eoseekingapp imtyeenoprom 1billionaire eoschinabank millionnaire 
  do
    cleos -u $API transfer $from bankofstaked "$price" "$to"
  done

  price="1.0000 EOS"
  for to in seekingtoken superoneioai huang54321ll superoneiopp superoneiokk 
  do
    cleos -u $API transfer $from bankofstaked "$price" "$to"
  done

  price="2.0000 EOS"
  for to in eoshuobinode huang54321pp superoneiott sosososososo superoneiouu 
  do
    cleos -u $API transfer $from bankofstaked "$price" "$to"
  done
done
