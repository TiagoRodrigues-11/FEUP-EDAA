#pragma once

#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <thread>
#include <atomic>

#define MAX_THREADS std::thread::hardware_concurrency() - 1

std::atomic<unsigned int> num_threads_atomic(0);

double sort_time[16] = {0};

template <class Point>
class KdTree;

template <class Point>
class KdTreeNode
{
private:
    Point point;
    KdTreeNode<Point> *left;
    KdTreeNode<Point> *right;

public:
    KdTreeNode(Point point);
    ~KdTreeNode();

    void print();

    friend class KdTree<Point>;
};

template <class Point>
KdTreeNode<Point>::KdTreeNode(Point point)
{
    this->point = point;
    this->left = nullptr;
    this->right = nullptr;
}

template <class Point>
KdTreeNode<Point>::~KdTreeNode()
{
    delete left;
    delete right;
}

template <class Point>
void KdTreeNode<Point>::print()
{
    std::cout << point << std::endl;
}

template <class Point>
class KdTree
{
private:
    KdTreeNode<Point> *root;
    void _insert(KdTreeNode<Point> *node, Point point, int depth, int k);
    void _remove(KdTreeNode<Point> *node, Point point, int depth, int k);
    bool inRange(Point point, Point min, Point max);
    void buildTree(typename std::vector<Point>::iterator _begin, typename std::vector<Point>::iterator _end, KdTreeNode<Point> *node, int depth, int thread_no);

public:
    KdTree();
    KdTree(std::vector<Point> &points);
    ~KdTree();
    void insert(Point point);
    void remove(Point point);
    void print(KdTreeNode<Point> *node, int depth);
    void rangeSearch(Point point, Point min, Point max, int depth, std::vector<Point> &points);
    Point nearestNeighborSearch(KdTreeNode<Point> node, Point query, int depth, double best_dist);
    KdTreeNode<Point> *getRoot() { return root;}
};

template <class Point>
KdTree<Point>::KdTree()
{
    this->root = nullptr;
}

template <class Point>
KdTree<Point>::KdTree(std::vector<Point> &points)
{
    this->root = nullptr;

    // for (size_t i = 0; i < points.size(); i++) { //I THINK WITH THIS THE TREE ISN'T BALANCED
    //     this->insert(points[i]);
    // }

    this->buildTree(points.begin(), points.end(), root, 0, 0);

    for (size_t i = 0; i < MAX_THREADS; i++)
    {
        std::cout << "Thread " << i << " sort time: " << sort_time[i] << std::endl;
    }
}

template <class Point>
void KdTree<Point>::buildTree(typename std::vector<Point>::iterator _begin, typename std::vector<Point>::iterator _end, KdTreeNode<Point> *node, int depth, int thread_no)
{
    if (_begin == _end)
    {
        node = nullptr;
        return;
    }

    int axis = depth % _begin->dimensions();

    auto start = std::chrono::system_clock::now();

    // Make a KdTree
    std::sort(_begin, _end, [axis](Point a, Point b) { // TO IMPROVE - FOR BETTER PERFORMACE - IT TAKES A LOT OF TIME
        return a[axis] < b[axis];
    });

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;

    sort_time[thread_no] += ((double)elapsed_seconds.count());

    int median = ((int)(_end - _begin)) / 2;

    node = new KdTreeNode<Point>(*(_begin + median));

    if (num_threads_atomic < MAX_THREADS && _end - _begin > 100000)
    {
        num_threads_atomic++;

        std::thread leftThread(&KdTree<Point>::buildTree, this, _begin, _begin + median, node->left, depth + 1, thread_no + 1);

        this->buildTree(_begin + median + 1, _end, node->right, depth + 1, thread_no);

        leftThread.join();

        num_threads_atomic--;
    }
    else
    {
        this->buildTree(_begin, _begin + median, node->left, depth + 1, thread_no);
        this->buildTree(_begin + median + 1, _end, node->right, depth + 1, thread_no);
    }
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

/* template <class Point>
Point KdTree<Point>::nearestNeighborSearch(KdTreeNode<Point> node, Point query, int depth, double best_dist)
{
    if (this->root == nullptr)
    {
        return;
    }
    Point best_point;

    // Check if current node is closer than best_dist
    double dist = distance(node->point, query);
    if (dist < best_dist)
    {
        best_dist = dist;
        best_point = node->point;
    }

    // Check which side of the tree to search first
    int axis = depth % query.dimensions();
    KdTreeNode<Point> *good_side;
    KdTreeNode<Point> *bad_side;
    if (query[axis] <= node->point[axis])
    {
        good_side = node->left;
        bad_side = node->right;
    }
    else
    {
        good_side = node->right;
        bad_side = node->left;
    }

    // Search good side first
    Point good_side_best = this->nearestNeighborSearch(good_side, query, depth + 1, best_dist);
    double good_side_best_dist = distance(good_side_best, query);
    if (good_side_best_dist < best_dist)
    {
        best_dist = good_side_best_dist;
        best_point = good_side_best;
    }

    // Check if bad side is worth searching
    if (distance(query[axis], node->point[axis]) < best_dist)
    {
        Point bad_side_best = this->nearestNeighborSearch(bad_side, query, depth + 1, best_dist);
        double bad_side_best_dist = distance(bad_side_best, query);
        if (bad_side_best_dist < best_dist)
        {
            best_dist = bad_side_best_dist;
            best_point = bad_side_best;
        }
    }

    return best_point;
} */
