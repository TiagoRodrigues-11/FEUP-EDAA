#! /bin/sh

# This script runs the KdTree program with various parameters and saves the results running time in a file.

echo "Running KdTree tests..."

popularities=(80 90)
sample_sizes=(8000 10000)
num_threads=(8 16)
min_points_thread=(200000 400000)

# popularities=(0 25 50 60 70 80 90)
# sample_sizes=(50 100 500 1000 2000 4000 6000 8000 10000)
# num_threads=(1 2 4 8 16)
# min_points_thread=(10000 25000 50000 75000 100000 200000 400000)

for popularity in "${popularities[@]}"
do
    for sample_size in "${sample_sizes[@]}"
    do
        for num_thread in "${num_threads[@]}"
        do
            for min_points_thread in "${min_points_thread[@]}"
            do
                echo "Running KdTree with popularity: $popularity, sample size: $sample_size, num threads: $num_thread, min points per thread: $min_points_thread"
                ./KdTree $popularity -s $sample_size -t $num_thread -ts $min_points_thread >> KdTree_results.txt
            done
        done
    done
done