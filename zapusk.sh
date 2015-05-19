rm out/*
rm result
g++ lab.cpp -o prog -g 2> errors
./prog 25 25 26 26 5 0.1
gnuplot result
