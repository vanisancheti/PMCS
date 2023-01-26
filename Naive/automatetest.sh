make clean
make

for ((index = 0; index < 1; index++))
do
    # 1. Generate Graph test
    ## 1a. Generate random parameter
    D=100
    L=5
    E=5
    V=6
    i=8
    T=5
    
    support=$(( 5*$D/100 ))
    echo "Runnning -D $D -E $E -i $i -L $L -T $T -V $V -support $support"
    
    ## 1b. Generate testcase 
    rm ./Data/test2.txt
    ../ggen-i386-Linux -D $((D)) -E $((E)) -i $((i)) -L $((L)) -T $((T)) -V $((V)) | perl ../convert.pl > ./Data/test2.txt
    
    # 2. Run Code
    for partition in 2
    do
        echo "Running for partition $partition"

        rm local_test.txt;
        rm ./Outputs/output_test.txt

        # echo "Running Local Computation"
        # ../../Margin_Secondary_Memory_Version/MCS_MLC/margin_local $((D)) ./Data/test2.txt -s $((support)) -c -o ./Outputs/output_test.txt -p $((partition)) -l local_test.txt > ./Outputs/run_log_local_test.txt
        # localgraphs=$(cat ./Outputs/run_log_local_test.txt | grep "NO of local MFS:*" | cut -d' ' -f 5  )

        echo "Running Local Computation"
        ./naive $((D)) ./Data/test2.txt -s $((support)) -c -o ./Outputs/output_test.txt -p $((partition)) -l local_test.txt > ./Outputs/run_log_local_test.txt
        localgraphs=$(cat ./Outputs/run_log_local_test.txt | grep "NO of local MFS:*" | cut -d' ' -f 5  )
        echo "Running Global Computation"
        ./naive $((D)) ./Data/test2.txt -s $((support)) -c -o ./Outputs/output_test.txt -p $((partition)) -g local_test.txt $((localgraphs)) > ./Outputs/run_log_test.txt

        echo ""
        localtime=$(cat ./Outputs/run_log_local_test.txt | grep "Total Time Taken for local computation for each partition*" | cut -d':' -f 2  )
        echo "For support $support partition $partition Local time taken: $localtime"
        globaltime=$(cat ./Outputs/run_log_test.txt | grep "Total Time Taken for global computation*" | cut -d':' -f 2)
        echo "For support $support partition $partition all level Global Computation time taken: $globaltime"
    done
done