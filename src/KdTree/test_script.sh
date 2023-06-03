#!/bin/bash

# This script runs the KdTree program with various parameters and saves the results running time in a file.

echo "Running KdTree tests..."

popularities=(0 25 50 70 90)
sample_sizes=(50 500 4000 10000)
num_threads=(1 2 4 8 16)
min_points_threads=(10000 50000 100000 400000)
test_files=("standard_test_input.lf")

other_popularities=(0 1 10 20 30 40 50 60 70 80 90)
other_sample_sizes=(1 5 10 20 50)

rm -rf KdTree_results.txt

for test_file in "${test_files[@]}"
do
    for popularity in "${popularities[@]}"
    do
        for sample_size in "${sample_sizes[@]}"
        do
            for num_thread in "${num_threads[@]}"
            do
                for min_points in "${min_points_threads[@]}"
                do
                    if [ $num_thread -eq 1 ] && [ $min_points -gt 10000 ]
                    then
                        continue
                    fi
                    echo "Running KdTree 3 times with popularity: $popularity, sample size: $sample_size, num threads: $num_thread, min points per thread: $min_points, test file: $test_file..."
                    echo "TEST $popularity $sample_size $num_thread $min_points $test_file" >> KdTree_results.txt
                    for i in {1..3} # test 3 times for each parameter combination
                    do
                        echo "RUN $i" >> KdTree_results.txt
                        ./KdTree $popularity -s $sample_size -t $num_thread --ts $min_points --test $test_file > KdTree_output.txt
                        grep -f grep_time_patterns.lf KdTree_output.txt >> KdTree_results.txt
                    done
                done
            done
        done
    done
done

echo TEST >> KdTree_results.txt

rm -rf KdTree_range_results.txt

for popularity in "${popularities[@]}"
do
    echo "Running KdTree 3 times with popularity: $popularity, sample size: 50, num threads: 16, min points per thread: 100000, test file: range_search_test_input.lf..."
    echo "TEST $popularity 50 16 100000 range_search_test_input.lf" >> KdTree_range_results.txt
    for i in {1..3} # test 3 times for each parameter combination
    do
        echo "RUN $i" >> KdTree_range_results.txt
        ./KdTree $popularity -s 50 -t 16 --ts 100000 --test range_search_test_input.lf > KdTree_output.txt
        grep -f grep_time_patterns.lf KdTree_output.txt >> KdTree_range_results.txt
    done
done

echo TEST >> KdTree_range_results.txt

rm -rf KdTree_build_time_complexity_results.txt

for popularity in "${other_popularities[@]}"
do
    echo "Running KdTree 3 times with popularity: $popularity, sample size: 50, num threads: 1, min points per thread: 100000, test file: standard_test_input.lf..."
    echo "TEST $popularity 50 1 100000 standard_test_input.lf" >> KdTree_build_time_complexity_results.txt
    for i in {1..3} # test 3 times for each parameter combination
    do
        echo "RUN $i" >> KdTree_build_time_complexity_results.txt
        ./KdTree $popularity -s 50 -t 1 --ts 100000 --test standard_test_input.lf > KdTree_output.txt
        grep -f grep_time_patterns.lf KdTree_output.txt >> KdTree_build_time_complexity_results.txt
    done
done

echo TEST >> KdTree_build_time_complexity_results.txt

rm -rf KdTree_other_sample_size_results.txt

for sample_size in "${other_sample_sizes[@]}"
do
    echo "Running KdTree 3 times with popularity: 0, sample size: $sample_size, num threads: 4, min points per thread: 50000, test file: standard_test_input.lf..."
    echo "TEST 0 $sample_size 4 50000 standard_test_input.lf" >> KdTree_other_sample_size_results.txt
    for i in {1..3} # test 3 times for each parameter combination
    do
        echo "RUN $i" >> KdTree_other_sample_size_results.txt
        ./KdTree 0 -s $sample_size -t 4 --ts 50000 --test standard_test_input.lf > KdTree_output.txt
        grep -f grep_time_patterns.lf KdTree_output.txt >> KdTree_other_sample_size_results.txt
    done
done

echo TEST >> KdTree_other_sample_size_results.txt

rm -rf KdTree_output.txt
echo "Done."