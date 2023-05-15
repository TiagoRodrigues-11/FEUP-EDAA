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
#include <utility>
#include <cmath>
#include <map>

#define MAX_THREADS std::thread::hardware_concurrency() - 1

std::atomic<unsigned int> num_threads_atomic(0);

double sort_time[16] = {0};

/**
 * @brief Class used to sort priority queue by distance with the closest point first
 */
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

/**
 * @brief Class used to sort priority queue by distance with the farthest point first
 */
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

/**
 * @brief Distance function used to calculate the distance between two points. Information for different cases is taken from API documentation.
 */
template <class Point>
double distance(Point point1, Point point2)
{
    double sum = 0;
    for (int i = 0; i < point1.dimensions(); i++)
    {
        if (i == 3) { // instrumentalness
            if ((point1[i] < 0.5 && point2[i] < 0.5) || (point1[i] > 0.5 && point2[i] > 0.5)) { //same category
                sum += pow(point1[i] - point2[i], 2);
            } else { //different category
                sum += pow(point1[i] - point2[i], 2) * 1.5;
            }
        }   
        else if(i == 4) { //liveness - values above 0.8 means it was most likely performed live
            if((point1[i] > 0.8 && point2[i] > 0.8) || (point1[i] < 0.8 && point2[i] < 0.8)){ //same category
                sum += pow(point1[i] - point2[i], 2);
            } else { //different category
                sum += pow(point1[i] - point2[i], 2) * 1.5;
            }
        } else if (i == 5) { //loudness
            sum += pow(point1[i]/60 - point2[i]/60, 2);
        }
        else if (i == 6) { //speechiness
            // values below 0.33 are intrumentals, between 0.33 and 0.66 are mixed, and above 0.66 are pure speech
            // increase distance if they are not in the same category
            if ((point1[i] < 0.33 && point2[i] < 0.33) || (point1[i] > 0.66 && point2[i] > 0.66) || (point1[i] > 0.33 && point1[i] < 0.66 && point2[i] > 0.33 && point2[i] < 0.66)) {
                sum += pow(point1[i] - point2[i], 2);
            } else {
                sum += pow(point1[i] - point2[i], 2) * 1.5;
            }
        }        
        else if (i == 7) { //tempo
            sum += pow(point1[i]/250 - point2[i]/250, 2);
        } else if (i == 9) { //time signature
            sum += pow(point1[i]/5 - point2[i]/5, 2);
        } else if (i = 10) { //mode
            sum *= point1[i] == point2[i] ? 1 : 1.5; // if same mode, multiply by 1, else multiply by 1.5 (change weights later)
        }
        else {
            sum += pow(point1[i] - point2[i], 2);
        }
    }
    return sqrt(sum);
}

/**
 * @brief Function used to copy a point and change a specific dimension to a specific value 
 */
template <class Point>
Point copyPointWithDimension(Point point, int dimension, double value) {
    Point new_point = point;
    new_point.setDimension(dimension, value);
    return new_point;
}

/**
 * @brief Forward declaration of KdTree class
 */
template <class Point>
class KdTree;

/**
 * @brief Class used to represent a node in the KdTree
 */
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
    Point getPoint() { return *point; }

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

/**
 * @brief Class used to represent a KdTree
 */
template <class Point>
class KdTree
{
private:
    KdTreeNode<Point> *root;
    // TODO _insert and remove functions aren't used anywhere or properly implemented
    void _insert(KdTreeNode<Point> *node, Point point, int depth, int k);
    void _remove(KdTreeNode<Point> *node, Point point, int depth, int k);
    bool inRange(Point *point, Point *min, Point *max);
    void buildTree(std::vector<Point *> &points, KdTreeNode<Point> *&node, int depth, int thread_no, KdTreeNode<Point> *parent, bool right = true);
    void _splitVector(std::vector<Point *> &points, int depth, size_t sample_size, std::vector<Point *> &leftPoints, std::vector<Point *> &rightPoints);
    std::vector<Point*> reportSubtree(KdTreeNode<Point> *node);
    std::pair<KdTreeNode<Point> *, int> _traverseTreeToLeaf(KdTreeNode<Point> *node, Point *point, int depth = 0);
    bool subregionContained(Point * min, Point * max, std::map<int, std::pair<double, double>> kdTreeRange);
    bool subregionIntersects(Point * min, Point * max, std::map<int, std::pair<double, double>> kdTreeRange);
public:
    KdTree();
    KdTree(std::vector<Point*> &points);
    ~KdTree();
    void insert(Point point);
    void remove(Point point);
    void print(KdTreeNode<Point> *node, int depth);
    std::vector<Point*> rangeSearch(KdTreeNode<Point> *node, Point * min, Point * max, std::map<int, std::pair<double, double>> kdTreeRange, int depth = 0);
    std::priority_queue<Point *, std::vector<Point *>, ComparePointsClosestFirst<Point>> kNearestNeighborSearch(KdTreeNode<Point> *node, Point *query, int k = 1, KdTreeNode<Point> * cutoff = nullptr, int depth = 0);
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

/**
 * @brief Split a vector of points into two vectors based on the median value of the points discovered by random sampling along a given axis
 * 
 * @param points Vector of points to split
 * @param depth Tree depth (used to determine axis to split on)
 * @param sample_size Size of the sample to take
 * @param leftPoints Vector of points to store the left points in
 * @param rightPoints Vector of points to store the right points in
 */
template <class Point>
void KdTree<Point>::_splitVector(std::vector<Point *> &points, int depth, size_t sample_size, std::vector<Point *> &leftPoints, std::vector<Point *> &rightPoints)
{
    // Get splitting axis
    int axis = depth % (*points[0]).dimensions();

    size_t size = points.size();

    // If the size of the vector is less than the sample size, just use the whole vector
    if (size < sample_size)
    {
        sample_size = size;
    }

    std::set<int> used_indices;
    std::vector<Point *> sample;

    // Randomly sample points from the vector
    while (sample.size() < sample_size)
    {
        while (true)
        {
            // Generate a random index between 0 and size - 1 (inclusive)
            size_t random_index = (size_t) rand() % size;

            // If the index has not already been used, add it to the sample
            if (used_indices.find(random_index) == used_indices.end())
            {
                used_indices.insert(random_index);
                sample.push_back(points[random_index]);
                break;
            }
        }
    }

    // Sort the sample along the given axis
    std::sort(sample.begin(), sample.end(), [axis](Point * a, Point *b) {
        return (*a)[axis] < (*b)[axis];
    });

    // Get the median value of the sample
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

// TODO dafuq is right for?
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
void KdTree<Point>::buildTree(std::vector<Point*> &points, KdTreeNode<Point>* &node, int depth, int thread_no, KdTreeNode<Point> *parent, bool right)
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
    
    // Start clock for thread
    auto start = std::chrono::system_clock::now();

    std::vector<Point*> leftPoints;
    std::vector<Point*> rightPoints;

    // Split the vector into two vectors, using random sampling to determine the median
    this->_splitVector(points, depth, 1000, leftPoints, rightPoints);

    Point *median = leftPoints[leftPoints.size() - 1];

    leftPoints.pop_back();

    points.clear();

    // End clock for thread
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;

    // Store the time taken to split
    sort_time[thread_no] += ((double)elapsed_seconds.count());

    // Create the new node and set it as the parent's child
    KdTreeNode<Point> * node_obj = new KdTreeNode<Point>(median, parent);

    node = node_obj;

    // Determine whether to spawn a new thread or not
    if (num_threads_atomic < MAX_THREADS && leftPoints.size() + rightPoints.size() > 100000)
    {
        num_threads_atomic++;

        // Use thread to build left subtree
        std::thread leftThread(&KdTree<Point>::buildTree, this, std::ref(leftPoints), std::ref(node->left), depth + 1, thread_no + 1, node, false);
        
        // Build right subtree in current thread
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

/**
 * @brief Check if a point is in the range of a given min and max point
 * 
 * @param point Point to check
 * @param min Min point
 * @param max Max point
 */
template <class Point>
bool KdTree<Point>::inRange(Point *point, Point *min, Point *max)
{
    for (int i = 0; i < point->dimensions(); i++)
    {
        if ((*point)[i] < (*min)[i] || (*point)[i] > (*max)[i])
        {
            return false;
        }
    }
    return true;
}

// TODO do these two functions make sense?
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

/**
 * @brief Returns the points of the subtree in a vector
 * 
 * @param node The root of the subtree
 */
template <class Point>
std::vector<Point*> KdTree<Point>::reportSubtree(KdTreeNode<Point> *node) {
    std::vector<Point*> points;
    points.push_back(node->point);
    if (node->left != nullptr) {
        std::vector<Point*> leftPoints = reportSubtree(node->left);
        points.insert(points.end(), leftPoints.begin(), leftPoints.end());
    }
    if (node->right != nullptr) {
        std::vector<Point*> rightPoints = reportSubtree(node->right);
        points.insert(points.end(), rightPoints.begin(), rightPoints.end());
    }
    return points;
}

/**
 * @brief Check if a subregion of the kd tree is contained in a given range
 * 
 * @param min Min point of the range
 * @param max Max point of the range
 * @param kdTreeRange Map with the min and max of each dimension of the kd tree subregion
 */
template <class Point>
bool KdTree<Point>::subregionContained(Point * min, Point * max, std::map<int, std::pair<double, double>> kdTreeRange) {
    // Go through each dimension
    for (int i = 0; i < min->dimensions(); i++) {
        std::pair<double, double> dimensionRange = kdTreeRange[i];

        // If the min value of the interval in this dimension is greater than the min value of the subregion, then the range is not contained
        if ((*min)[i] > dimensionRange.first) {
            return false;
        }

        // If the max value of the interval in this dimension is less than the max value of the subregion, then the range is not contained
        if ((*max)[i] < dimensionRange.second) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Check if a subregion of the kd tree intersects a given range
 * 
 * @param min Min point of the range
 * @param max Max point of the range
 * @param kdTreeRange Map with the min and max of each dimension of the kd tree subregion
 */
template <class Point>
bool KdTree<Point>::subregionIntersects(Point * min, Point * max, std::map<int, std::pair<double, double>> kdTreeRange) {
    // Go through each dimension
    for (int i = 0; i < min->dimensions(); i++) {
        std::pair<double, double> dimensionRange = kdTreeRange[i];

        // If the min value of the subregion is greater or equal than the min value of the interval in this dimension, then the range intersects
        if (dimensionRange.first >= (*min)[i]) {
            return true;
        }

        // If the max value of the subregion is less or equal than the max value of the interval in this dimension, then the range intersects
        if (dimensionRange.second <= (*max)[i]) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Returns the points of the kd tree in a given range
 * 
 * @param node Root of the kd tree
 * @param min Min point of the range
 * @param max Max point of the range
 * @param kdTreeRange Range of the subregion of the kd tree currently being explored
 * @param depth Depth of the node in the kd tree
 */
template <class Point>
std::vector<Point*> KdTree<Point>::rangeSearch(KdTreeNode<Point> *node, Point* min, Point* max, std::map<int, std::pair<double, double>> kdTreeRange, int depth)
{   
    std::vector<Point*> points;

    // If the node is in the range, add it to the vector
    if(inRange(node->point, min, max)){
        points.push_back(node->point);
    }

    // Get the axis of the node
    int axis = depth % (node->point)->dimensions();

    // If the left child is not null, check if it is contained or intersects the range
    if (node->left != nullptr) {
        std::map<int, std::pair<double, double>> leftKdTreeRange(kdTreeRange);
        // Update subregion currently being explored
        leftKdTreeRange[axis].second = (*(node->point))[axis];

        // If left child is contained, report the subtree
        if (subregionContained(min, max, leftKdTreeRange)) {
            std::vector<Point*> leftPoints = reportSubtree(node->left);
            points.insert(points.end(), leftPoints.begin(), leftPoints.end());
        }
        // If left child intersects, search the subtree
        else if (subregionIntersects(min, max, leftKdTreeRange)) {
            std::vector<Point*> leftPoints = rangeSearch(node->left, min, max, leftKdTreeRange, depth + 1);
            points.insert(points.end(), leftPoints.begin(), leftPoints.end());
        }
    }

    // If the right child is not null, check if it is contained or intersects the range
    if (node->right != nullptr) {
        std::map<int, std::pair<double, double>> rightKdTreeRange(kdTreeRange);
        // Update subregion currently being explored
        rightKdTreeRange[axis].first = (*(node->point))[axis];

        // If right child is contained, report the subtree
        if (subregionContained(min, max, rightKdTreeRange)) {
            std::vector<Point*> rightPoints = reportSubtree(node->right);
            points.insert(points.end(), rightPoints.begin(), rightPoints.end());
        }
        // If right child intersects, search the subtree
        else if (subregionIntersects(min, max, rightKdTreeRange)) {
            std::vector<Point*> rightPoints = rangeSearch(node->right, min, max, rightKdTreeRange, depth + 1);
            points.insert(points.end(), rightPoints.begin(), rightPoints.end());
        }
    }

    return points;
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

/**
 * @brief Traverse tree to leaf according to the point
 * 
 * @param node Root of the tree
 * @param point Point to be searched
 * @param depth Depth of the node in the tree
 * @return std::pair<KdTreeNode<Point>*, int> Pair with the leaf node and its depth
 */
template <class Point>
std::pair<KdTreeNode<Point> *, int> KdTree<Point>::_traverseTreeToLeaf(KdTreeNode<Point> *node, Point * point, int depth) {
    int axis = depth % point->dimensions();
    // If the point is lesser than or equal to the current node, go left
    if ((*point)[axis] <= (*(node->point))[axis]) {
        // If the left node is null
        if (node->left == nullptr) {
            // If the right node is not null, go right
            if (node->right != nullptr) {
                return _traverseTreeToLeaf(node->right, point, depth + 1);
            }
            // If the right node is null, return the current node (leaf)
            return std::pair(node, depth);
        }
        // If the left node is not null, go left
        return _traverseTreeToLeaf(node->left, point, depth + 1);
    }
    // If the point is greater than the current node, go right
    else {
        // If the right node is null
        if (node->right == nullptr) {
            // If the left node is not null, go left
            if (node->left != nullptr) {
                return _traverseTreeToLeaf(node->left, point, depth + 1);
            }
            // If the left node is null, return the current node (leaf) 
            return std::pair(node, depth);
        }
        // If the right node is not null, go right
        return _traverseTreeToLeaf(node->right, point, depth + 1);
    }
}

/**
 * @brief kNearestNeighborSearch - returns the k nearest neighbors of a query point
 * @param node - the root of the tree
 * @param query - the query point
 * @param k - the number of nearest neighbors to return
 * @param cutoff - the node to stop searching at
*/
template <class Point>
std::priority_queue<Point *, std::vector<Point *>, ComparePointsClosestFirst<Point>> KdTree<Point>::kNearestNeighborSearch(KdTreeNode<Point> *node, Point * query, int k, KdTreeNode<Point> * cutoff, int depth)
{
    // Traverse tree to leaf
    std::pair<KdTreeNode<Point> *, int> leaf_depth_pair = _traverseTreeToLeaf(node, query, depth);
    KdTreeNode<Point> * leaf = leaf_depth_pair.first;
    int current_depth = leaf_depth_pair.second;

    // Initialize priority queue with ComparePointsFarthestFirst so queue.top() is the farthest point currently in the queue
    std::priority_queue<Point *, std::vector<Point *>, ComparePointsFarthestFirst<Point>> queue{ComparePointsFarthestFirst<Point>(query)};

    // Initialize current and previous nodes
    KdTreeNode<Point> *current = leaf, *previous = nullptr;

    // TODO: n deviamos tar a adicionar o 1o ponto a queue?

    // While we haven't reached the cutoff node we keep going up the tree
    while(current->parent != cutoff) {
        previous = current;
        current = current->parent;
        
        queue.push(current->point);

        // If queue is full, pop the farthest point
        if (queue.size() > k) {
            queue.pop();
        }

        // Check other branch
        KdTreeNode<Point> *other_child = current->left == previous ? current->right : current->left;

        current_depth--;
        
        if (other_child == nullptr) {
            continue;
        }
        
        int new_dim = current_depth % query->dimensions();
        
        // Check if there can be a closer point in other branch
        // Check if hyperplane of the new node can have a closer point than the farthest point in the queue
        // To do this we check if the distance between the query point and the hyperplane is less than the distance between the query point and the farthest point in the queue
        if (distance((*query), copyPointWithDimension(*query, new_dim, (*(current->point))[new_dim])) < distance((*query), (*queue.top()))) {
            // If there can be a closer point, search the other branch
            std::priority_queue<Point *, std::vector<Point *>, ComparePointsClosestFirst<Point>> other_points = kNearestNeighborSearch(other_child, query, k, current, current_depth + 1);

            // Add the points from the other branch to the queue
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

    // Place points in a priority queue with ComparePointsClosestFirst so queue.top() is the closest point currently in the queue
    std::priority_queue<Point *, std::vector<Point *>, ComparePointsClosestFirst<Point>> result{ComparePointsClosestFirst<Point>(query)};
    
    while (!queue.empty()) {
        result.push(queue.top());
        queue.pop();
    }

    return result;
}
