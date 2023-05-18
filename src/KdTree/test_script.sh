#!/bin/bash

# This script runs the KdTree program with various parameters and saves the results running time in a file.

echo "Running KdTree tests..."

popularities=(50)
sample_sizes=(50 100 500 1000 2000 4000 6000 8000 10000)
num_threads=(16)
min_points_thread=(100000)
test_files=("standard_test_input.lf")

# popularities=(0 25 50 60 70 80 90)
# sample_sizes=(50 100 500 1000 2000 4000 6000 8000 10000)
# num_threads=(1 2 4 8 16)
# min_points_thread=(10000 25000 50000 75000 100000 200000 400000)
# test_files=("standard_test_input")

rm -rf KdTree_results.txt

for test_file in "${test_files[@]}"
do
    for popularity in "${popularities[@]}"
    do
        for sample_size in "${sample_sizes[@]}"
        do
            for num_thread in "${num_threads[@]}"
            do
                for min_points_thread in "${min_points_thread[@]}"
                do
                    echo "Running KdTree 10 times with popularity: $popularity, sample size: $sample_size, num threads: $num_thread, min points per thread: $min_points_thread, test file: $test_file..."
                    echo "TEST $popularity $sample_size $num_thread $min_points_thread $test_file" >> KdTree_results.txt
                    for i in {1..10} # test 10 times for each parameter combination
                    do
                        echo "RUN $i" >> KdTree_results.txt
                        ./KdTree $popularity -s $sample_size -t $num_thread --ts $min_points_thread --test $test_file > KdTree_output.txt
                        grep -f grep_time_patterns.lf KdTree_output.txt >> KdTree_results.txt
                    done
                done
            done
        done
    done
done

rm -rf KdTree_output.txt
echo "Done."