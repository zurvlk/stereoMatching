#!/bin/bash

start_time=`date +%s`
count=1
result=`date +%Y%m%d_%H-%M-%S`.log
echo "---RangeSwap---"
dterm=0
for segm in -1 0 2 3 4
do
    for vterm in 1 2
    do
        lambda=2

        if [ $vterm -eq 0]; then
            T=8
        else
            T=7
        fi

        for pair in 0 1
        do
            job_start=`date +%s`
            
            ./grsa ../input/tsukuba_ output/${segm}_tsukuba_${dterm}_${vterm}_${lambda}_${T}.bmp ${T} 0 16 $lambda ${T} ${dterm} ${vterm} 5 ${segm} output/afterab_${segm}_tsukuba_${dterm}_${vterm}_${lambda}_${T}.bmp　${allpairs} >> log/${segm}_${vterm}_tsukuba.log
            job_end=`date +%s`
            time=$((job_end - job_start))
            count=`expr $count + 1`
            echo "tsukuba, lambda=${lambda} T=${T} [${time}s]"
        done

    done


    for vterm in 1 2
    do
        if [ $vterm -eq 0 ]; then
            lambda=2
            T=8
        else
            lambda=3
            T=7
        fi

        for pair in 0 1
        do
            job_start=`date +%s`
            ./grsa ../input/venus_ output/${segm}_venus_${dterm}_${vterm}_${lambda}_${T}.bmp $T 0 8 $lambda ${T} ${dterm} ${vterm} 7 ${segm} output/afterab_${segm}_venus_${dterm}_${vterm}_${lambda}_${T}.bmp ${allpairs} >> log/${segm}_${vterm}_venus.log
            job_end=`date +%s`
            time=$((job_end - job_start));
            count=`expr $count + 1`
            echo "venus T=${T}, lambda=${lambda} [${time}s]";
        done

    done



    for vterm in 1 2
    do
        lambda=2
        T=9

        job_start=`date +%s`
        ./grsa ../input/teddy_ output/${segm}_teddy_${dterm}_${vterm}_${lambda}_${T}.bmp $T 0 4 $lambda ${T} ${dterm} ${vterm} 20 ${segm} afterab_output/${segm}_teddy_${dterm}_${vterm}_${lambda}_${T}.bmp ${allpairs} >> log/${segm}_${vterm}_teddy.log
        job_end=`date +%s`
        time=$((job_end - job_start));
        count=`expr $count + 1`
        echo "teddy T=${T}, lambda=${lambda} [${time}s]";

    done
done

end_time=`date +%s`
time=$((end_time - start_time));
echo "全ての処理が完了しました(RangeSwap)! 総所要時間[${time}s]"
echo "------"
