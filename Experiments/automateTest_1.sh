#!/bin/bash
#SBATCH -A research
#SBATCH -n 10
#SBATCH --mem-per-cpu=2048
#SBATCH --time=2-00:00:00
#SBATCH --mail-type=END
#SBATCH --gres=gpu:1


for index in 10
do
    D=$index
    L=2
    T=2
    i=5
    E=3
    V=3
    partition=2
    support=$(((50*$D)/100))

    ### 1. Generate testcase 
    rm ./Data/test_$((index)).txt
    ../ggen-i386-Linux -D $((D)) -E $((E)) -i $((i)) -L $((L)) -T $((T)) -V $((V)) | perl ../convert.pl > ./Data/test_$((index)).txt
    echo "Runnning -D $D -E $E -i $i -L $L -T $T -V $V -support $support -partition $partition"

    ### 2a. Local computation using gSpan
    # rm local_gSpan.txt;
    # rm ./Outputs/run_log_local_gSpan.txt
    # timeout 1000 ./LC_gSpan $((D)) ./Data/test_$((index)).txt -s $((support)) -c -o ./Outputs/output_gSpan.txt -p $((partition)) -l local_gSpan.txt > ./Outputs/run_log_local_gSpan.txt
    # result=$?
    # if [[ $result -eq 124 ]]; then
    #     echo "gSpan Taking more that 1000"
    #     break
    # fi

    # gSpantime=$(cat ./Outputs/run_log_local_gSpan.txt | grep "sec*" | cut -f 4 | cut -d's' -f 1  )
    # localgraphs=$(cat ./Outputs/run_log_local_gSpan.txt | grep "NO of local MFS:*" | cut -d' ' -f 5  )
    # completelocaltime=$(cat ./Outputs/run_log_local_gSpan.txt | grep "Complete Time Taken for local computation for each partition*" | cut -d':' -f 2  )
    # localtime=$(cat ./Outputs/run_log_local_gSpan.txt | grep "Total Time Taken for local computation for each partition*" | cut -d':' -f 2  )

    # tot=0
    # for gst in ${gSpantime[@]}; 
    # do
    #     tot=$(echo "$tot+$gst" | bc)
    # done

    # actualTime=$(echo "$tot+$localtime" | bc)
    # echo "no of Graphs: $localgraphs actual time: $actualTime total time: $completelocaltime gSpantime: $gSpantime localtime: $localtime"
    # EVR=$(cat ./Outputs/run_log_local_gSpan.txt | grep "EVR*")
    # levels=$(cat ./Outputs/run_log_local_gSpan.txt | grep "Level*")
    # echo "EVR :: $EVR"
    # echo "$levels"
    # echo ""

    ### 2b. Local Computation using Margin
    # rm local_margin.txt
    # rm ./Outputs/output_margin.txt
    # rm ./Outputs/run_log_local_margin.txt
    # timeout 1000 ./LC_margin $((D)) ./Data/test_$((index)).txt -s $((support)) -c -o ./Outputs/output_margin.txt -p $((partition)) -l local_margin.txt > ./Outputs/run_log_local_margin.txt

    # result=$?
    # if [[ $result -eq 124 ]]; then
    #     echo "MARGIN Taking more that 1000"
    #     continue
    # fi

    # localgraphs=$(cat ./Outputs/run_log_local_margin.txt | grep "NO of local MFS:*" | cut -d' ' -f 5  )
    # localtime=$(cat ./Outputs/run_log_local_margin.txt | grep "Total Time Taken for local computation for each partition*" | cut -d':' -f 2  )
    # echo "MARGIN no of Graphs: $localgraphs total time: $localtime"

    # result=$(timeout 5 ../Compare_outputs/compare ./local_margin.txt ./local_test_gSpan.txt | grep "SUCCESS*" |wc -w)
    # echo ""
    # if [[ $result -eq 0 ]]; then
    #     echo "ERROR"
    # else
    #     echo "SUCCESS"
    # fi

    ### 3a. Global Computation using Naive Iterative Approach
    rm ./Outputs/output_naive.txt
    rm ./Outputs/run_log_naive.txt
    timeout 1000 ./GC_naive $((D)) ./Data/test_$((index)).txt -s $((support)) -c -o ./Outputs/output_naive.txt -p $((partition)) -g local_gSpan.txt $((localgraphs)) > ./Outputs/run_log_naive.txt
    result=$?
    if [[ $result -eq 124 ]]; then
        echo "Naive Iterative Approach Taking more that 1000 time"
        continue
    fi

    round=$(cat ./Outputs/run_log_naive.txt | grep "Total number of loadings*" | cut -d':' -f 2)
    localtime=$(cat ./Outputs/run_log_naive.txt | grep "Total Time Taken for local computation for each partition*" | cut -d':' -f 2)
    globaltime=$(cat ./Outputs/run_log_naive.txt | grep "Total Time Taken for global computation*" | cut -d':' -f 2 | cut -d' ' -f 2)
    totaltime=$(cat ./Outputs/run_log_naive.txt | grep "Total Time Taken for the overall margin algorithm*" | cut -d':' -f 2)
    levels=$(cat ./Outputs/run_log_naive.txt | grep "OutputLevel*")
    graphgenerated=$(cat ./Outputs/run_log_naive.txt | grep -m 1 "Total graphs generated in global computation*" | cut -d':' -f 2)
    subgraphiso=$(cat ./Outputs/run_log_naive.txt | grep -m 1 "Total subgraph isomorphism checks*" | cut -d':' -f 2)
    iso=$(cat ./Outputs/run_log_naive.txt | grep "Total isomorphism checks*" | cut -d':' -f 2)
    tot=$(echo "$actualTime+$globaltime" | bc )

    echo "For support $support partition $partition Naive Iterative Approach Global Computation time taken: $globaltime loadings:$round graphgenerated: $graphgenerated  subgraphiso: $subgraphiso iso: $iso"
    echo "TOTAL TIME (l+g): $tot"
    echo "$levels"
    echo ""

    ### 3b. Global Computation using PMCS approach
    rm ./Outputs/output_PMCS.txt
    rm ./Outputs/run_log_PMCS.txt
    timeout 1000 ./GC_PMCS $((D)) ./Data/test_$((index)).txt -s $((support)) -c -o ./Outputs/output_PMCS.txt -p $((partition)) -g local_gSpan.txt $((localgraphs)) > ./Outputs/run_log_PMCS.txt
    result=$?
    if [[ $result -eq 124 ]]; then
        echo "PMCS Approach Taking more that 1000 time"
        continue
    fi

    globaltime=$(cat ./Outputs/run_log_PMCS.txt | grep "Total Time Taken for global computation*" | cut -d':' -f 2)
    MCS_graphadded=$(cat ./Outputs/run_log_PMCS.txt | grep "No. of graphs added in GlobalMaxFreq after MCS Computation: *" | cut -d':' -f 2)
    MFS_graphadded=$(cat ./Outputs/run_log_PMCS.txt | grep "No. of graphs added in GlobalMaxFreq after MFS Computation: *" | cut -d':' -f 2)
    subgraphiso=$(cat ./Outputs/run_log_PMCS.txt | grep "Total subgraph isomorphism checks *:" | cut -d':' -f 2)
    iso=$(cat ./Outputs/run_log_PMCS.txt | grep "Total isomorphism checks *:" | cut -d':' -f 2)
    levels=$(cat ./Outputs/run_log_PMCS.txt | grep "OutputLevel*")
    FreqGraphs=$(cat ./Outputs/run_log_PMCS.txt | grep "Graph Freq:*")
    tot=$(echo "$actualTime+$globaltime" | bc)
    
    echo "For support $support partition $partition PMCS Approach Global Computation time taken: $globaltime MCS_graphadded: $MCS_graphadded MFS_graphadded: $MFS_graphadded subgraphiso: $subgraphiso iso: $iso"
    echo "TOTAL TIME (l+g): $tot"
    echo "$levels"
    echo "$FreqGraphs"
    
    result=$(timeout 5 ../Compare_outputs/compare ./Outputs/output_naive.txt ./Outputs/output_PMCS.txt | grep "SUCCESS*" |wc -w)
    echo ""
    if [[ $result -eq 0 ]]; then
        echo "ERROR"
    else
        echo "SUCCESS"
    fi
done