

g++ -O2 -pg serial.cpp -o serial
g++ -O2 -pg standard.cpp -o standard 

./serial
gprof serial gmon.out > analysis_serial.txt

./standard
gprof standard gmon.out > analysis_standard.txt

rm serial
rm standard
rm gmon.out