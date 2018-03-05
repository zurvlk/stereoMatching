#!/bin/bash

start_time=`date +%s`
count=1
result=`date +%Y%m%d_%H-%M-%S`.log
echo "---RangeSwap---"
dterm=0

if [ ! -e grsa ]; then
    echo "実行ファイル:grsaが存在しません.再度makeしコンパイルを行ってください"
    exit 0
fi

for allpairs in 0 1 2
do
    for j in 0 1 2 3 4
    do
        for vterm in 1 2
        do
            lambda=2

            if [ $vterm -eq 0 ]; then
                T=8
            else
                T=7
            fi

            for pair in 0 1
            do
                job_start=`date +%s`
                
                ./grsa ../input/tsukuba output/${j}_tsukuba_${dterm}_${vterm}_${lambda}_${T}_${allpairs}.bmp ${T} 16 $lambda ${T} ${dterm} ${vterm} 5 ${j} ${allpairs} >> log/${j}_${vterm}_tsukuba.log
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
                ./grsa ../input/venus output/${j}_venus_${dterm}_${vterm}_${lambda}_${T}_${allpairs}.bmp $T 8 $lambda ${T} ${dterm} ${vterm} 7 ${j} ${allpairs} >> log/${j}_${vterm}_venus.log
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
            ./grsa ../input/teddy output/${j}_teddy_${dterm}_${vterm}_${lambda}_${T}_${allpairs}.bmp $T 4 $lambda ${T} ${dterm} ${vterm} 20 ${j} ${allpairs} >> log/${j}_${vterm}_teddy.log
            job_end=`date +%s`
            time=$((job_end - job_start));
            count=`expr $count + 1`
            echo "teddy T=${T}, lambda=${lambda} [${time}s]";

        done
    done
done

end_time=`date +%s`
time=$((end_time - start_time));
echo "全ての処理が完了しました(RangeSwap)! 総所要時間[${time}s]"
echo "------"
