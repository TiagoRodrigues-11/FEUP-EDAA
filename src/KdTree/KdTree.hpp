#pragma once

#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <thread>
#include <atomic>
#include <set>
#include <string>
#include <limits>
#include <queue>

#define MAX_THREADS std::thread::hardware_concurrency() - 1

std::atomic<unsigned int> num_threads_atomic(0);

double sort_time[16] = {0};

// TODO check pointers
template <class Point>
class ComparePointsClosestFirst {
    Point * point;

    public:
        ComparePointsClosestFirst(Point * point) {
            this->point = point;
        }

        bool operator() (Point * point1, Point * point2) {
            return distance(*point1, *point) >= distance(*point2, *point);
        }
};

template <class Point>
class ComparePointsFarthestFirst {
    Point * point;

    public:
        ComparePointsFarthestFirst(Point * point) {
            this->point = point;
        }

        bool operator() (Point * point1, Point * point2) {
            return distance(*point1, *point) < distance(*point2, *point);
        }
};

template <class Point>
double distance(Point point1, Point point2)
{
    double sum = 0;
    for (int i = 0; i < point1.dimensions(); i++)
    {
        sum += pow(point1[i] - point2[i], 2);
    }
    return sqrt(sum);
}

template <class Point>
class KdTree;

template <class Point>
class KdTreeNode
{
private:
    Point * point;
    KdTreeNode<Point> *parent; // nullptr if root
    KdTreeNode<Point> *left;
    KdTreeNode<Point> *right;

public:
    KdTreeNode(Point *point, KdTreeNode<Point> *parent = nullptr);
    ~KdTreeNode();

    void print();

    friend class KdTree<Point>;
};

template <class Point>
KdTreeNode<Point>::KdTreeNode(Point *point, KdTreeNode<Point> *parent)
{
    this->point = point;
    this->parent = parent;
    this->left = nullptr;
    this->right = nullptr;
}

template <class Point>
KdTreeNode<Point>::~KdTreeNode()
{
    delete point;
    delete left;
    delete right;
}

template <class Point>
void KdTreeNode<Point>::print()
{
    std::cout << *point << std::endl;
}

template <class Point>
class KdTree
{
private:
    KdTreeNode<Point> *root;
    void _insert(KdTreeNode<Point> *node, Point point, int depth, int k);
    void _remove(KdTreeNode<Point> *node, Point point, int depth, int k);
    bool inRange(Point point, Point min, Point max);
    void buildTree(std::vector<Point *> &points, KdTreeNode<Point> *&node, int depth, int thread_no, KdTreeNode<Point> *parent, bool right = true);
    void _splitVector(std::vector<Point *> &points, int depth, size_t sample_size, std::vector<Point *> &leftPoints, std::vector<Point *> &rightPoints);
    KdTreeNode<Point> * _traverseTreeToLeaf(KdTreeNode<Point> *node, Point *point, int depth = 0);
public:
    KdTree();
    KdTree(std::vector<Point*> &points);
    ~KdTree();
    void insert(Point point);
    void remove(Point point);
    void print(KdTreeNode<Point> *node, int depth);
    void rangeSearch(Point point, Point min, Point max, int depth, std::vector<Point> &points);
    std::priority_queue<Point *, std::vector<Point *>, ComparePointsClosestFirst<Point>> kNearestNeighborSearch(KdTreeNode<Point> *node, Point *query, int k = 1, KdTreeNode<Point> * cutoff = nullptr);
    KdTreeNode<Point> *getRoot() { return root;}
};

template <class Point>
KdTree<Point>::KdTree()
{
    this->root = nullptr;
}

template <class Point>
KdTree<Point>::KdTree(std::vector<Point*> &points){
    this->root = nullptr;

    this->buildTree(points, this->root, 0, 0, nullptr, true);

    std::cout << "Number of threads: " << num_threads_atomic << std::endl;

    for (size_t i = 0; i <= MAX_THREADS; i++)
    {
        std::cout << "Thread " << i << " sort time: " << sort_time[i] << std::endl;
    }
}

template <class Point>
void KdTree<Point>::_splitVector(std::vector<Point *> &points, int depth, size_t sample_size, std::vector<Point *> &leftPoints, std::vector<Point *> &rightPoints)
{
    int axis = depth % (*points[0]).dimensions();

    size_t size = points.size();

    if (size < sample_size)
    {
        sample_size = size;
    }

    std::set<int> used_indices;
    std::vector<Point *> sample;

    while (sample.size() < sample_size)
    {
        while (true)
        {
            // Generate a random index between 0 and size - 1 (inclusive)
            size_t random_index = (size_t) rand() % size;

            if (used_indices.find(random_index) == used_indices.end())
            {
                used_indices.insert(random_index);
                sample.push_back(points[random_index]);
                break;
            }
        }
    }

    std::sort(sample.begin(), sample.end(), [axis](Point * a, Point *b) {
        return (*a)[axis] < (*b)[axis];
    });

    double median = (*sample[sample_size / 2])[axis];

    // Reorder the vector so that the elements before the median are smaller than the median and the elements after the median are greater than the median
    typename std::vector<Point*>::iterator median_iterator = std::partition(points.begin(), points.end(), [median, axis](Point * a) {
        return (*a)[axis] <= median;
    });

    leftPoints.assign(points.begin(), median_iterator);

    if (median_iterator == points.end())
        rightPoints.clear();
    else
        rightPoints.assign(median_iterator, points.end());
}

/**
 * @brief Build the tree
 * @param points The points to build the tree from
 * @param node The current node
 * @param depth The current depth
 * @param thread_no The current thread number
 * @param parent The parent of the current node
 * @param right Whether the current node is the right child of its parent
*/
template <class Point>
void KdTree<Point>::buildTree(std::vector<Point*> &points, KdTreeNode<Point>* &node, int depth, int thread_no, KdTreeNode<Point> *parent,bool right)
{    
    // Case: reached the leaf 
    if (points.size() == 1)
    {
        KdTreeNode<Point> * node_obj = new KdTreeNode<Point>(points[0], parent);
        node = node_obj;
        return;
    }
    if (points.empty())
    {
        return;
    }
    
    auto start = std::chrono::system_clock::now();

    std::vector<Point*> leftPoints;
    std::vector<Point*> rightPoints;

    this->_splitVector(points, depth, 1000, leftPoints, rightPoints);

    Point *median = leftPoints[leftPoints.size() - 1];

    leftPoints.pop_back();

    points.clear();

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;

    sort_time[thread_no] += ((double)elapsed_seconds.count());

    KdTreeNode<Point> * node_obj = new KdTreeNode<Point>(median, parent);

    node = node_obj;

    if (num_threads_atomic < MAX_THREADS && leftPoints.size() + rightPoints.size() > 100000)
    {
        num_threads_atomic++;

        std::thread leftThread(&KdTree<Point>::buildTree, this, std::ref(leftPoints), std::ref(node->left), depth + 1, thread_no + 1, node, false);
        
        this->buildTree(rightPoints, std::ref(node->right), depth + 1, thread_no, node);

        leftThread.join();

        num_threads_atomic--;

    }
    else
    {
        this->buildTree(leftPoints, std::ref(node->left), depth + 1, thread_no, node, false);
        this->buildTree(rightPoints, std::ref(node->right), depth + 1, thread_no, node);
    }

    return;
}

template <class Point>
KdTree<Point>::~KdTree()
{
    delete root;
}

// TODO: make operator < for Point class
template <class Point>
bool KdTree<Point>::inRange(Point point, Point min, Point max)
{
    for (int i = 0; i < point.dimensions(); i++)
    {
        if (point[i] < min[i] || point[i] > max[i])
        {
            return false;
        }
    }
    return true;
}


template <class Point>
void KdTree<Point>::insert(Point point)
{
    if (this->root == nullptr)
    {
        this->root = new KdTreeNode<Point>(point);
    }
    else
    {
        this->_insert(this->root, point, 0, point.dimensions());
    }
}

template <class Point>
void KdTree<Point>::_insert(KdTreeNode<Point> *node, Point point, int depth, int k)
{
    if (node == nullptr)
    {
        node = new KdTreeNode<Point>(point);
    }
    else
    {
        int axis = depth % k;
        if (point[axis] <= node->point[axis])
        {
            this->_insert(node->left, point, depth + 1, k);
        }
        else
        {
            this->_insert(node->right, point, depth + 1, k);
        }
    }
}

template <class Point>
void KdTree<Point>::rangeSearch(Point point, Point min, Point max, int depth, std::vector<Point> &points)
{
    if (this->root == nullptr)
    {
        return;
    }

    // Check if current node is in range
    if (inRange(point, min, max))
    {
        points.push_back(point);
    }

    // Check whether the left or right subtree is in range
    int axis = depth % point.dimensions();
    if (min[axis] <= point[axis])
    {
        this->rangeSearch(point->left, min, max, depth + 1, points);
    }
    if (max[axis] >= point[axis])
    {
        this->rangeSearch(point->right, min, max, depth + 1, points);
    }
}

template <class Point>
void KdTree<Point>::print(KdTreeNode<Point> *node, int depth)
{
    if (node == nullptr)
    {
        return;
    }
    for (int i = 0; i < depth; i++)
    {
        std::cout << " ";
    }
    std::cout << node->point << std::endl;
    this->print(node->left, depth + 1);
    this->print(node->right, depth + 1);
}

template <class Point>
KdTreeNode<Point> * KdTree<Point>::_traverseTreeToLeaf(KdTreeNode<Point> *node, Point * point, int depth) {
    int axis = depth % point->dimensions();
    if ((*point)[axis] <= (*(node->point))[axis]) {
        if (node->left == nullptr) {
            return node;
        }
        return _traverseTreeToLeaf(node->left, point, depth + 1);
    } else {
        if (node->right == nullptr) {
            return node;
        }
        return _traverseTreeToLeaf(node->right, point, depth + 1);
    }
}

/**
 * @brief kNearestNeighborSearch - returns the k nearest neighbors of a query point
 * @param node - the root of the tree
 * @param query - the query point
 * @param depth - the depth of the current node
 * @param best_dist - the distance of the current best point
 * @param k - the number of nearest neighbors to return
*/
template <class Point>
std::priority_queue<Point *, std::vector<Point *>, ComparePointsClosestFirst<Point>> KdTree<Point>::kNearestNeighborSearch(KdTreeNode<Point> *node, Point * query, int k, KdTreeNode<Point> * cutoff)
{
    KdTreeNode<Point> *leaf = _traverseTreeToLeaf(node, query);

    std::priority_queue<Point *, std::vector<Point *>, ComparePointsFarthestFirst<Point>> queue{ComparePointsFarthestFirst<Point>(query)};
    KdTreeNode<Point> *current = leaf, *previous = nullptr;

    while(current->parent != cutoff) {
        previous = current;
        current = current->parent;
        
        queue.push(current->point);

        if (queue.size() > k) {
            queue.pop();
        }

        // Check other branch
        KdTreeNode<Point> *other_child = current->left == previous ? current->right : current->left;

        if (other_child == nullptr) {
            continue;
        }

        // Check if there can be a closer point in other branch
        // TODO: check if condition is correct
        if (distance(*query, *(other_child->point)) < distance(*query, *(queue.top()))) {
            std::priority_queue<Point *, std::vector<Point *>, ComparePointsClosestFirst<Point>> other_points = kNearestNeighborSearch(other_child, query, k, current);

            while (!other_points.empty()) {
                Point *point = other_points.top();
                other_points.pop();
                queue.push(point);

                if (queue.size() > k) {
                    queue.pop();
                }
            }
        }
    }

    std::priority_queue<Point *, std::vector<Point *>, ComparePointsClosestFirst<Point>> result{ComparePointsClosestFirst<Point>(query)};
    
    while (!queue.empty()) {
        result.push(queue.top());
        queue.pop();
    }

    return result;
}
