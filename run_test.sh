#!/bin/sh

# START SERVER OPTIONS

# leaving empty for default values to kick in

# END SERVER OPTIONS

if [ -f ./build/test/test ];
then
    ./build/test/test
else
    echo "./build/test/test not found"
    exit
fi

summary=summary.log

echo "#### Start Test Suite ####" >> $summary

echo "Running APM setup"
cat /proc/cpuinfo | grep -m 1 "model name" >> $summary
cat /proc/cpuinfo | grep -m 1 "cpu cores" >> $summary
cat /proc/meminfo | grep -m 1 "MemTotal:" >> $summary
cat /proc/meminfo | grep -m 1 "SwapTotal:" >> $summary
echo "[Start HTTP Settings]" >> $summary
cat ./run_server.sh | grep export | awk '{print $2}' >> $summary
echo "[End HTTP Settings]\n" >> $summary

if [ -f ./build/test/setup ];
then
    ./build/test/setup >> $summary
else
    echo "./build/test/setup not found"
fi

echo "Running latency test"
if [ -f ./build/test/latency ];
then
    ./build/test/latency >> $summary
else
    echo "./build/test/latency not found"
fi

echo "Running request throughput test"
if [ -f ./build/test/request ];
then
    ./build/test/request >> $summary
else
    echo "./build/test/request not found"
fi

echo "Running response throughput test"
if [ -f ./build/test/response ];
then
    ./build/test/response >> $summary
else
    echo "./build/test/response not found"
fi
echo "" >> $summary

echo "Running data throughput test - 1MB Writes"
echo "Data Throughput - 1MB Writes" >> $summary
fio --name=random-write \
    --ioengine=posixaio \
    --rw=randwrite \
    --bs=1M \
    --numjobs=1 \
    --size=2g \
    --iodepth=1 \
    --runtime=60 \
    --time_based \
    --end_fsync=1 > fio1_temp

awk '/IOPS/ {print $2,$3,$4}' fio1_temp >> $summary
tail -n 2 fio1_temp >> $summary
rm random-write*
rm fio1_temp
echo '\n'>> $summary

echo "Running data throughput test - 1MB Reads"
echo "Data Throughput - 1MB Reads" >> $summary
fio --name=random-read \
    --ioengine=posixaio \
    --rw=randread \
    --bs=1M \
    --numjobs=1 \
    --size=2g \
    --iodepth=1 \
    --runtime=60 \
    --time_based \
    --end_fsync=1 > fio2_temp

awk '/IOPS/ {print $2,$3,$4}' fio2_temp >> $summary
tail -n 2 fio2_temp >> $summary
rm random-read*
rm fio2_temp
echo '\n'>> $summary

echo "Running data throughput test - 64KB Writes"
echo "Data Throughput - 64kb Writes" >> $summary
fio --name=random-write \
    --ioengine=posixaio \
    --rw=randwrite \
    --bs=64k \
    --numjobs=1 \
    --size=2g \
    --iodepth=1 \
    --runtime=60 \
    --time_based \
    --end_fsync=1 > fio3_temp

awk '/IOPS/ {print $2,$3,$4}' fio3_temp >> $summary
tail -n 2 fio3_temp >> $summary
rm random-write*
rm fio3_temp
echo '\n'>> $summary

echo "Running data throughput test - 64KB Reads"
echo "Data Throughput - 64kb Reads" >> $summary
fio --name=random-read \
    --ioengine=posixaio \
    --rw=randread \
    --bs=64k \
    --numjobs=1 \
    --size=2g \
    --iodepth=1 \
    --runtime=60 \
    --time_based \
    --end_fsync=1 > fio4_temp

awk '/IOPS/ {print $2,$3,$4}' fio4_temp >> $summary
tail -n 2 fio4_temp >> $summary
rm random-read*
rm fio4_temp
echo '\n'>> $summary

echo "Running data throughput test - 4KB Writes"
echo "Data Throughput - 4kb Writes" >> $summary
fio --name=random-write \
    --ioengine=posixaio \
    --rw=randwrite \
    --bs=4k \
    --numjobs=1 \
    --size=2g \
    --iodepth=1 \
    --runtime=60 \
    --time_based \
    --end_fsync=1 > fio5_temp

awk '/IOPS/ {print $2,$3,$4}' fio5_temp >> $summary
tail -n 2 fio5_temp >> $summary
rm random-write*
rm fio5_temp
echo '\n'>> $summary

echo "Running data throughput test - 4KB Reads"
echo "Data Throughput - 4kb Reads" >> $summary
fio --name=random-read \
    --ioengine=posixaio \
    --rw=randread \
    --bs=4k \
    --numjobs=1 \
    --size=2g \
    --iodepth=1 \
    --runtime=60 \
    --time_based \
    --end_fsync=1 > fio6_temp

awk '/IOPS/ {print $2,$3,$4}' fio6_temp >> $summary
tail -n 2 fio6_temp >> $summary
rm random-read*
rm fio6_temp
echo '\n'>> $summary

echo "#### End Test Suite #### \n" >> $summary