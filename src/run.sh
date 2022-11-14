echo "Gshare on fp_1 \n:"
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --gshare:13
echo "Gshare on fp_2 \n:"
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --gshare:13
echo "Gshare on int_1 \n:"
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --gshare:13
echo "Gshare on int_2 \n:"
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --gshare:13
echo "Gshare on mm_1 \n"
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --gshare:13
echo "Gshare on mm_2 \n"
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --gshare:13


echo "\n\n"
echo "tournament on fp_1\n"
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --tournament:9:10:10
echo "tournament on fp_2\n"
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --tournament:9:10:10
echo "tournament on int_1\n"
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --tournament:9:10:10
echo "tournament on int_2\n"
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --tournament:9:10:10
echo "tournament on fp_1.bz2:"
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --tournament:9:10:10
echo "\ntournament on mm_2.bz2:"
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --tournament:9:10:10