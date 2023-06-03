# Graphics imports
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import pandas as pd
import os

def create_dataframe_from_values(save_csv=False, path=os.path.join(os.getcwd(), ('src\\KdTree\\KdTree_results.txt'))):
    benchmarking_results = pd.DataFrame(columns=['Sample Size', 'Minimum Points per Thread', 'Number of Threads', 'Popularity', 'Query Time (s)', 'Build Time (s)', 'kNN Time (s)', 'Range Search Time (s)', 'Number of tracks'])
    
    with open(path, 'r') as benchmarking_file:
        data = {'Query Time (s)' : [], 'Build Time (s)' : [], 'kNN Time (s)' : [], 'Range Search Time (s)' : []}
        for line in benchmarking_file:
            line = line.strip()
            if line.startswith('TEST'):
                if data['Query Time (s)'] != []:
                    median_query_time = np.median(data['Query Time (s)'])
                    median_build_time = np.median(data['Build Time (s)'])
                    median_knn_time = np.median(data['kNN Time (s)']) if data['kNN Time (s)'] != [] else 0
                    median_range_search_time = np.median(data['Range Search Time (s)']) if data['Range Search Time (s)'] != [] else 0
                    
                    data['Query Time (s)'] = [median_query_time]
                    data['Build Time (s)'] = [median_build_time]
                    data['kNN Time (s)'] = [median_knn_time]
                    data['Range Search Time (s)'] = [median_range_search_time]
                    
                    benchmarking_results = pd.concat([benchmarking_results, pd.DataFrame.from_dict(data)])                           
                    data = {'Query Time (s)' : [], 'Build Time (s)' : [], 'kNN Time (s)' : [], 'Range Search Time (s)' : []}

                    if line == 'TEST':
                        break
                
                _, popularity, sample_size, threads, min_points, _ = line.split()
                data['Sample Size'] = [int(sample_size)]
                data['Minimum Points per Thread'] = [int(min_points)]
                data['Number of Threads'] = [int(threads)]
                data['Popularity'] = [int(popularity)]
                
                # Values come from running count on the dataset
                if (int(popularity) == 0):
                    data['Number of tracks'] = [int(8737156)]
                elif (int(popularity) == 25):
                    data['Number of tracks'] = [int(629313)]
                elif (int(popularity) == 50):
                    data['Number of tracks'] = [int(77892)]
                elif (int(popularity) == 70):
                    data['Number of tracks'] = [int(6920)]
                elif (int(popularity) == 90):
                    data['Number of tracks'] = [int(82)]
                
            elif line.startswith('Retrieved'):
                data['Query Time (s)'].append(float(line.split(':')[1]))
            elif line.startswith('Built'):
                data['Build Time (s)'].append(float(line.split(':')[1]))
            elif line.startswith('Finished searching for nearest neighbours in (s):'):
                data['kNN Time (s)'].append(float(line.split(':')[1]))
            elif line.startswith('Finished searching for range in (s):'):
                data['Range Search Time (s)'].append(float(line.split(':')[1]))
    
    if(save_csv):
        benchmarking_results.to_csv('src\\data_handling\\' + path.split('\\')[-1][:-3] + 'csv', index=False)
    return benchmarking_results

benchmarking_results = create_dataframe_from_values()
range_search_results = create_dataframe_from_values(path=os.path.join(os.getcwd(), ('src\\KdTree\\KdTree_range_results.txt')))

def create_execution_time_according_to_number_of_tracks_graph(knn_dataframe, range_dataframe, sample_size=50, n_threads=16, min_points=100000):
    knn_values = knn_dataframe[(knn_dataframe['Sample Size'] == sample_size) & (knn_dataframe['Number of Threads'] == n_threads) & (knn_dataframe['Minimum Points per Thread'] == min_points)]
    knn_values = knn_values.drop(columns=['Range Search Time (s)'])
    knn_values = knn_values.sort_values('Number of tracks')
    range_values = range_dataframe[(range_dataframe['Sample Size'] == sample_size) and (range_dataframe['Number of Threads'] == n_threads) and (range_dataframe['Minimum Points per Thread'] == min_points)]
    range_values = range_values.drop(columns=['Query Time (s)', 'Build Time (s)', 'kNN Time (s)'])
    range_values = range_values.sort_values('Number of tracks')
    # merge the two dataframes where the sample size, number of threads, and minimum points per thread are the same
    values = pd.merge(knn_values, range_values, on=['Sample Size', 'Minimum Points per Thread', 'Number of Threads', 'Popularity', 'Number of tracks'], suffixes=('_knn', '_range'))
    values = values.sort_values('Number of tracks')

    # plot graph with number of tracks as x-axis and query time, build time, knn time, and range search time as y-axis
    ax1 = values.plot(x='Number of tracks', y=['Query Time (s)', 'Build Time (s)', 'kNN Time (s)', 'Range Search Time (s)'], kind='bar', color=['red', 'blue', 'green', 'pink'], label=['Query Time', 'Build Time', 'kNN Time', 'Range Search Time'])
    plt.xlabel('Number of tracks')
    plt.ylabel('Time (s)')
    plt.title(f'Sample Size: {sample_size}, Number of Threads: {n_threads}, Minimum Points per Thread: {min_points}')
    plt.legend()

    for p in ax1.patches:
        ax1.annotate(format(p.get_height(), '.5f'), (p.get_x() + p.get_width() / 2., p.get_height()), ha = 'center', va = 'bottom', rotation = 'vertical', xytext = (0, 10), textcoords = 'offset points')

    plt.ylim(top=ax1.get_ylim()[1] * 1.1)

    plt.show()

def create_execution_time_according_to_number_of_tracks_graph_range_search(dataframe, sample_size=50, n_threads=16, min_points=100000):
    values = dataframe[(dataframe['Sample Size'] == sample_size) & (dataframe['Number of Threads'] == n_threads) & (dataframe['Minimum Points per Thread'] == min_points)]
    values = values.sort_values('Number of tracks') 
    print(values)
    # plot graph with number of tracks as x-axis and query time, build time, and knn time as y-axis
    ax1 = values.plot(x='Number of tracks', y='Range Search Time (s)', kind='bar', color='red', label='Range Search Time')
    plt.xlabel('Number of tracks')
    plt.ylabel('Time (s)')
    plt.title(f'Sample Size: {sample_size}, Number of Threads: {n_threads}, Minimum Points per Thread: {min_points}')
    plt.legend()

    for p in ax1.patches:
        ax1.annotate(format(p.get_height(), '.5f'), (p.get_x() + p.get_width() / 2., p.get_height()), ha = 'center', va = 'bottom', rotation = 'vertical', xytext = (0, 10), textcoords = 'offset points')

    plt.ylim(top=ax1.get_ylim()[1] * 1.1)

    plt.show()

def create_execution_time_according_to_number_of_threads_graph(dataframe, sample_size=50, min_points=10000, popularity=0):
    values = dataframe[(dataframe['Sample Size'] == sample_size) & (dataframe['Minimum Points per Thread'] == min_points) & (dataframe['Popularity'] == popularity)]
    values = values.sort_values('Number of Threads')
    # plot graph with number of tracks as x-axis and query time, build time, and knn time as y-axis
    ax1 = values.plot(x='Number of Threads', y='Build Time (s)', kind='bar', color='blue', label='Build Time')
    plt.xlabel('Number of threads')
    plt.ylabel('Time (s)')
    plt.title(f'Sample Size: {sample_size}, Minimum Points per Thread: {min_points}, Popularity: {popularity}')
    plt.legend()

    for p in ax1.patches:
        ax1.annotate(format(p.get_height(), '.5f'), (p.get_x() + p.get_width() / 2., p.get_height()), ha = 'center', va = 'bottom', rotation = 'vertical', xytext = (0, 10), textcoords = 'offset points')

    plt.ylim(top=ax1.get_ylim()[1] * 1.1)

    plt.show()

def create_knn_size_sample_size_graph(dataframe, n_threads=16, min_points=100000, popularity=0):
    values = dataframe[(dataframe['Number of Threads'] == n_threads) & (dataframe['Minimum Points per Thread'] == min_points) & (dataframe['Popularity'] == popularity)]
    values = values.sort_values('Sample Size')
    # plot graph with sample size as x-axis and knn time as y-axis
    ax1 = values.plot(x='Sample Size', y='kNN Time (s)', kind='bar', color='green', label='kNN Time')
    plt.xlabel('Sample Size')
    plt.ylabel('kNN Time (s)')
    plt.title(f'Number of Threads: {n_threads}, Minimum Points per Thread: {min_points}, Popularity: {popularity}')
    plt.legend()

    for p in ax1.patches:
        ax1.annotate(format(p.get_height(), '.5f'), (p.get_x() + p.get_width() / 2., p.get_height()), ha = 'center', va = 'bottom', rotation = 'vertical', xytext = (0, 10), textcoords = 'offset points')

    plt.ylim(top=ax1.get_ylim()[1] * 1.1)

    plt.show()

def create_build_time_graph(dataframe, sample_size=50, n_threads=16, min_points=100000):
    values = dataframe[(dataframe['Sample Size'] == sample_size) & (dataframe['Number of Threads'] == n_threads) & (dataframe['Minimum Points per Thread'] == min_points)]
    values = values.sort_values('Number of tracks')
    # plot graph with number of tracks as x-axis and build time as y-axis
    ax1 = values.plot(x='Number of tracks', y='Build Time (s)', kind='bar', color='blue', label='Build Time')
    plt.xlabel('Number of tracks')
    plt.ylabel('Build Time (s)')
    plt.title(f'Sample Size: {sample_size}, Number of Threads: {n_threads}, Minimum Points per Thread: {min_points}')
    plt.legend()

    for p in ax1.patches:
        ax1.annotate(format(p.get_height(), '.5f'), (p.get_x() + p.get_width() / 2., p.get_height()), ha = 'center', va = 'bottom', rotation = 'vertical', xytext = (0, 10), textcoords = 'offset points')

    plt.ylim(top=ax1.get_ylim()[1] * 1.1)

    plt.show()

def create_graph_compare_build_time_with_complexity(dataframe, sample_size=50, n_threads=1, min_points=10000):
    values = dataframe[(dataframe['Sample Size'] == sample_size) & (dataframe['Number of Threads'] == n_threads) & (dataframe['Minimum Points per Thread'] == min_points)]
    values = values.sort_values('Number of tracks')
    # plot graph with number of tracks as x-axis and build time as y-axis as points
    ax1 = values.plot(x='Number of tracks', y='Build Time (s)', kind='scatter', color='blue', label='Build Time')
    plt.xlabel('Number of tracks')
    plt.ylabel('Build Time (s)')
    plt.title(f'Sample Size: {sample_size}, Number of Threads: {n_threads}, Minimum Points per Thread: {min_points}')
    plt.legend()

    for p in ax1.patches:
        ax1.annotate(format(p.get_height(), '.5f'), (p.get_x() + p.get_width() / 2., p.get_height()), ha = 'center', va = 'bottom', rotation = 'vertical', xytext = (0, 10), textcoords = 'offset points')
    
    # Fake Data
    x = np.linspace(0, 8700000, 1000)
    y = x * np.log(x) / 8700000
    plt.plot(x, y, "--", color='red', label='O(nlogn)')
    plt.legend()


    plt.ylim(top=ax1.get_ylim()[1] * 1.1)

    plt.show()



def create_knn_time_graph(dataframe, sample_size=50, n_threads=1, min_point=10000):
    values = dataframe[(dataframe['Sample Size'] == sample_size) & (dataframe['Number of Threads'] == n_threads) & (dataframe['Minimum Points per Thread'] == min_point)]
    values = values.sort_values('Number of tracks')
    # plot graph with number of tracks as x-axis and knn time as y-axis
    ax1 = values.plot(x='Number of tracks', y='kNN Time (s)', kind='bar', color='green', label='kNN Time')
    plt.xlabel('Number of tracks')
    plt.ylabel('kNN Time (s)')
    plt.title(f'Sample Size: {sample_size}, Number of Threads: {n_threads}, Minimum Points per Thread: {min_point}')
    plt.legend()

    for p in ax1.patches:
        ax1.annotate(format(p.get_height(), '.5f'), (p.get_x() + p.get_width() / 2., p.get_height()), ha = 'center', va = 'bottom', rotation = 'vertical', xytext = (0, 10), textcoords = 'offset points')

    plt.ylim(top=ax1.get_ylim()[1] * 1.1)

    plt.show()


#print(benchmarking_results.head())
#create_graph_sample_size_build_time(benchmarking_results)
#create_execution_time_according_to_number_of_tracks_graph(benchmarking_results, range_search_results, sample_size=50, n_threads=16, min_points=100000)
#create_execution_time_according_to_number_of_threads_graph(benchmarking_results, sample_size=50, min_points=10000, popularity=0)
#create_execution_time_according_to_number_of_tracks_graph_range_search(range_search_results, sample_size=50, n_threads=16, min_points=100000)
#create_knn_size_sample_size_graph(benchmarking_results, n_threads=4, min_points=50000, popularity=0)
#create_build_time_graph(benchmarking_results, sample_size=50, n_threads=1, min_points=10000)
#create_knn_time_graph(benchmarking_results, sample_size=50, n_threads=1, min_point=10000)
create_graph_compare_build_time_with_complexity(benchmarking_results, sample_size=50, n_threads=1, min_points=10000)
