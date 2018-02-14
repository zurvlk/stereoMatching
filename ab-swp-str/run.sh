#!/bin/bash

start_time=`date +%s`
count=1
result=`date +%Y%m%d_%H-%M-%S`.log
info=ab-swp
echo "---ab-swp---"
dterm=0
for vterm in 0 1
do
    lambda=2

    if [ $vterm -eq 0 ]; then
        T=8
    else
        T=7
    fi
    job_start=`date +%s`
    
    ./a-estr ../input/tsukuba_ output/${info}_tsukuba_${dterm}_${vterm}_${lambda}_${T}.bmp  16 $lambda ${T} ${dterm} ${vterm} 5　>> log/${info}_tsukuba.log
    job_end=`date +%s`
    time=$((job_end - job_start))
    count=`expr $count + 1`
    echo "tsukuba, lambda=${lambda} T=${T} [${time}s]"

done


for vterm in 0 1
do
    if [ $vterm -eq 0 ]; then
        lambda=2
        T=8
    else
        lambda=3
        T=7
    fi

    job_start=`date +%s`
    ./a-estr ../input/venus_ output/${info}_venus_${dterm}_${vterm}_${lambda}_${T}.bmp 8 $lambda $T ${dterm} ${vterm} 7　>> log/${info}_venus.log
    job_end=`date +%s`
    time=$((job_end - job_start));
    count=`expr $count + 1`
    echo "venus T=${T}, lambda=${lambda} [${time}s]";

done



for vterm in 0 1
do
    lambda=2
    T=9

    job_start=`date +%s`
    ./a-estr ../input/teddy_ output/${info}_teddy_${dterm}_${vterm}_${lambda}_${T}.bmp 4 $lambda $T ${dterm} ${vterm} 20 >> log/${info}_teddy.log
    job_end=`date +%s`
    time=$((job_end - job_start));
    count=`expr $count + 1`
    echo "teddy T=${T}, lambda=${lambda} [${time}s]";

done


end_time=`date +%s`
time=$((end_time - start_time));

echo "全ての処理が完了しました(${1})! 総所要時間[${time}s]"
echo "------"



