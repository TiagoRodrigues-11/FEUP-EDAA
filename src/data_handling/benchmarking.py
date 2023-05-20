# Graphics imports
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import pandas as pd
import os

def create_dataframe_from_values(save_csv=False):
    benchmarking_results = pd.DataFrame(columns=['Sample Size', 'Minimum Points per Thread', 'Number of Threads', 'Popularity', 'Query Time (s)', 'Build Time (s)', 'kNN Time (s)'])
    path = os.path.join(os.getcwd(), ('src\\KdTree\\KdTree_results.txt'))
    
    with open(path, 'r') as benchmarking_file:
        data = {'Query Time (s)' : [], 'Build Time (s)' : [], 'kNN Time (s)' : []}
        for line in benchmarking_file:
            line = line.strip()
            if line.startswith('TEST'):
                if data['Query Time (s)'] != []:
                    median_query_time = np.median(data['Query Time (s)'])
                    median_build_time = np.median(data['Build Time (s)'])
                    median_knn_time = np.median(data['kNN Time (s)'])
                    
                    data['Query Time (s)'] = [median_query_time]
                    data['Build Time (s)'] = [median_build_time]
                    data['kNN Time (s)'] = [median_knn_time]
                    
                    benchmarking_results = pd.concat([benchmarking_results, pd.DataFrame.from_dict(data)])                           
                    data = {'Query Time (s)' : [], 'Build Time (s)' : [], 'kNN Time (s)' : []}
                
                _, popularity, sample_size, threads, min_points, _ = line.split()
                data['Sample Size'] = [int(sample_size)]
                data['Minimum Points per Thread'] = [int(min_points)]
                data['Number of Threads'] = [int(threads)]
                data['Popularity'] = [int(popularity)]
            elif line.startswith('Retrieved'):
                data['Query Time (s)'].append(float(line.split(':')[1]))
            elif line.startswith('Built'):
                data['Build Time (s)'].append(float(line.split(':')[1]))
            elif line.startswith('Finished'):
                data['kNN Time (s)'].append(float(line.split(':')[1]))
    
    if(save_csv):
        benchmarking_results.to_csv('src\\data_handling\\KdTree_results.csv', index=False)
    return benchmarking_results

benchmarking_results = create_dataframe_from_values()

# create graphs from dataframe
def create_graph_sample_size_build_time(dataframe, popularity=0):
    groups = dataframe.groupby(['Popularity', 'Number of Threads', 'Minimum Points per Thread'])
    for name, group in groups:
        print(group.head())
        
        """ group.plot(x='Sample Size', y='Built Time (s)', kind='scatter', color='blue', label='Built Time')
        plt.xlabel('Sample Size')
        plt.ylabel('Build Time (s)')
        plt.title(f'Popularity: {name}')
        plt.legend()
        plt.show()
        break """

def create_execution_time_according_to_popularity_graph(dataframe, sample_size=50, n_threads=16, min_points=100000):
    values = dataframe[(dataframe['Sample Size'] == sample_size) & (dataframe['Number of Threads'] == n_threads) & (dataframe['Minimum Points per Thread'] == min_points)]
    print(values.head())
    # plot graph with popularity as x-axis and query time, build time, and knn time as y-axis
    values.plot(x='Popularity', y=['Query Time (s)', 'Build Time (s)', 'kNN Time (s)'], kind='bar', color=['red', 'blue', 'green'], label=['Query Time', 'Build Time', 'kNN Time'])
    plt.xlabel('Popularity')
    plt.ylabel('Time (s)')
    plt.title(f'Sample Size: {sample_size}, Number of Threads: {n_threads}, Minimum Points per Thread: {min_points}')
    plt.legend()

    for p in ax.patches:
        ax.annotate(format(p.get_height(), '.2f'), (p.get_x() + p.get_width() / 2., p.get_height()), ha = 'center', va = 'center', xytext = (0, 10), textcoords = 'offset points')
        
    plt.show()


#print(benchmarking_results.head())
#create_graph_sample_size_build_time(benchmarking_results)
create_execution_time_according_to_popularity_graph(benchmarking_results, sample_size=50, n_threads=16, min_points=100000)
