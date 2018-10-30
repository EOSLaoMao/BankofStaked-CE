rm multi_*
./scripts/check.sh > multi_before
for i in a b c d e f g h i j k l m n o p q r s t u v w x y z
do
  for j in a b c d e f g h i j k l m n o p q r s t u v w x y z
  do
    name=testaccoua$i$j
    cleos system newaccount voter3 $name EOS5M6M6WPV2ncSquidJTGmV8xeoFRdjAK9M2veZusYoneQCLYziq EOS5M6M6WPV2ncSquidJTGmV8xeoFRdjAK9M2veZusYoneQCLYziq --stake-net "10 EOS" --stake-cpu "10 EOS" --buy-ram "10 EOS"
    cleos transfer voter3 bankofstaked '0.1 EOS' "$name"
  done
done
./scripts/check.sh > multi_after
sleep 70
./scripts/check.sh > multi_undelegate
sleep 70
./scripts/check.sh > multi_refund
