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
#define SAMPLE_SIZE 1000
#define THREAD_SPLIT_THRESHOLD 10000

std::atomic<unsigned int> numeThreadsAtomic(0);

double sortTime[16] = {0};

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
    int dimensions = (int)(point1.dimensions());
    for (int i = 0; i < dimensions; i++)
    {
        std::string dimensionName = point1.getDimensionName(i);
        // All this values come from the API 
        if(dimensionName == "instrumentalness"){
            // Same category
            if ((point1[i] < 0.5 && point2[i] < 0.5) || (point1[i] > 0.5 && point2[i] > 0.5)) { 
                sum += pow(point1[i] - point2[i], 2);
                break;
            }
            // Different Category
            sum += pow(point1[i] - point2[i], 2) * 1.5;
        } else if (dimensionName == "liveness"){
            // Same category - Values above 0.8 means it was most likely performed live
            if((point1[i] > 0.8 && point2[i] > 0.8) || (point1[i] < 0.8 && point2[i] < 0.8)){ 
                sum += pow(point1[i] - point2[i], 2);
                break;
            } 
            // Different Category
            sum += pow(point1[i] - point2[i], 2) * 1.5;
        } else if (dimensionName == "loudness") {
            sum += pow(point1[i]/60 - point2[i]/60, 2);
        } else if (dimensionName == "speechiness"){
            // Values below 0.33 are intrumentals, between 0.33 and 0.66 are mixed, and above 0.66 are pure speech
            // Increase distance if they are not in the same category
            if ((point1[i] < 0.33 && point2[i] < 0.33) || (point1[i] > 0.66 && point2[i] > 0.66) || (point1[i] > 0.33 && point1[i] < 0.66 && point2[i] > 0.33 && point2[i] < 0.66)) {
                sum += pow(point1[i] - point2[i], 2);
                break;
            } 
            sum += pow(point1[i] - point2[i], 2) * 1.5;
        } else if (dimensionName == "tempo") {
            sum += pow(point1[i]/250 - point2[i]/250, 2);
        } else if (dimensionName == "time_signature") {
            sum += pow(point1[i]/5 - point2[i]/5, 2);
        } else if (dimensionName == "mode") {
            // If same mode, multiply by 1, else multiply by 1.5
            sum *= point1[i] == point2[i] ? 1 : 1.5; 
        } else {
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
    Point newPoint = point;
    newPoint.setDimension(dimension, value);
    return newPoint;
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

    void print(int depth = 0);

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
void KdTreeNode<Point>::print(int depth)
{
    // Name - Author
    std::cout << this->getPoint().getName() << " - " << this->getPoint().getArtist() << std::endl;
    // Other attributes

    for(int i = 0; i < (int)(this->getPoint().dimensions()); i++){
        for(int i = 0; i < depth; i++) std::cout << " ";
        std::cout << this->getPoint().getDimensionName(i) << ": " << (this->getPoint())[i] << std::endl;
    }

    std::cout << std::endl;
}

/**
 * @brief Class used to represent a KdTree
 */
template <class Point>
class KdTree
{
private:
    KdTreeNode<Point> *root;
    const unsigned int maxNumThreads;
    const size_t sampleSize;
    const unsigned int threadSplitThreshold;
    bool inRange(Point *point, Point *min, Point *max);
    void buildTree(std::vector<Point *> &points, KdTreeNode<Point> *&node, int depth, int threadNo, KdTreeNode<Point> *parent);
    Point* splitVector(std::vector<Point *> &points, int depth, std::vector<Point *> &leftPoints, std::vector<Point *> &rightPoints);
    std::vector<Point*> reportSubtree(KdTreeNode<Point> *node);
    std::pair<KdTreeNode<Point> *, int> traverseTreeToLeaf(KdTreeNode<Point> *node, Point *point, int depth = 0);
    bool subregionContained(Point * min, Point * max, std::map<int, std::pair<double, double>> kdTreeRange);
    bool subregionIntersects(Point * min, Point * max, std::map<int, std::pair<double, double>> kdTreeRange);
public:
    KdTree(unsigned int maxNumThreads = MAX_THREADS, size_t sampleSize = SAMPLE_SIZE, unsigned int threadSplitThreshold = THREAD_SPLIT_THRESHOLD);
    KdTree(std::vector<Point*> &points, unsigned int maxNumThreads = MAX_THREADS, size_t sampleSize = SAMPLE_SIZE, unsigned int threadSplitThreshold = THREAD_SPLIT_THRESHOLD);
    ~KdTree();
    void print(KdTreeNode<Point> *node, int depth);
    std::vector<Point*> rangeSearch(KdTreeNode<Point> *node, Point * min, Point * max, std::map<int, std::pair<double, double>> kdTreeRange, int depth = 0);
    std::priority_queue<Point *, std::vector<Point *>, ComparePointsClosestFirst<Point>> kNearestNeighborSearch(KdTreeNode<Point> *node, Point *query, size_t k = 1, KdTreeNode<Point> * cutoff = nullptr, int depth = 0);
    KdTreeNode<Point> *getRoot() { return root;}
};

template <class Point>
KdTree<Point>::KdTree(unsigned int maxNumThreads, size_t sampleSize, unsigned int threadSplitThreshold) : maxNumThreads(maxNumThreads > MAX_THREADS ? MAX_THREADS : maxNumThreads), sampleSize(sampleSize > SAMPLE_SIZE ? SAMPLE_SIZE : sampleSize), threadSplitThreshold(threadSplitThreshold > THREAD_SPLIT_THRESHOLD ? THREAD_SPLIT_THRESHOLD : threadSplitThreshold)
{
    this->root = nullptr;
}

template <class Point>
KdTree<Point>::KdTree(std::vector<Point*> &points, unsigned int maxNumThreads, size_t sampleSize, unsigned int threadSplitThreshold) : maxNumThreads(maxNumThreads > MAX_THREADS ? MAX_THREADS : maxNumThreads), sampleSize(sampleSize > SAMPLE_SIZE ? SAMPLE_SIZE : sampleSize), threadSplitThreshold(threadSplitThreshold > THREAD_SPLIT_THRESHOLD ? THREAD_SPLIT_THRESHOLD : threadSplitThreshold){
    this->root = nullptr;

    this->buildTree(points, this->root, 0, 0, nullptr);

    std::cout << "Number of threads: " << numeThreadsAtomic << std::endl;

    for (size_t i = 0; i <= this->maxNumThreads; i++)
    {
        if(sortTime[i] != 0) {
            std::cout << "Thread " << i << " sort time: " << sortTime[i] << std::endl;
        }
    }
    
}

/**
 * @brief Split a vector of points into two vectors based on the median value of the points discovered by random sampling along a given axis
 * 
 * @param points Vector of points to split
 * @param depth Tree depth (used to determine axis to split on)
 * @param sampleSize Size of the sample to take
 * @param leftPoints Vector of points to store the left points in
 * @param rightPoints Vector of points to store the right points in
*/
/*
Time Complexity: O(n logn)
 - Random sample: O(sampleSize)
 - Sort: O(n logn) = O(sampleSize log sampleSize)
 - Partition: O(n)
    -> O(sampleSize + sampleSize log sampleSize + n) = O(s log s + n ) -> Epah acho que está certo

Space Complexity: O(n)
 - usedIndices: O(sampleSize)
 - 
 - 
*/

template <class Point>
Point* KdTree<Point>::splitVector(std::vector<Point *> &points, int depth, std::vector<Point *> &leftPoints, std::vector<Point *> &rightPoints)
{
    // Get splitting axis
    int dimensions = (int)((*points[0]).dimensions());
    int axis = depth % dimensions;

    size_t size = points.size();
    size_t localSampleSize = this->sampleSize;

    // If the size of the vector is less than the sample size, just use the whole vector
    if (size < localSampleSize)
    {
        localSampleSize = size;
    }

    std::set<int> usedIndices;
    std::vector<Point *> sample;

    // Randomly sample points from the vector -> O(sampleSize)
    while (sample.size() < localSampleSize)
    {
        while (true)
        {
            // Generate a random index between 0 and size - 1 (inclusive)
            size_t randomIndex = (size_t) rand() % size;

            // If the index has not already been used, add it to the sample
            if (usedIndices.find((int)(randomIndex)) == usedIndices.end())
            {
                usedIndices.insert((int)(randomIndex));
                sample.push_back(points[randomIndex]);
                break;
            }
        }
    }

    /** TODO: Alternative sampling method
     * Alternative sampling method
    
    std::vector<Point *> sample = points;

    std::random_shuffle(sample.begin(), sample.end());
    sample.resize(sampleSize);

     * Alternative sampling method

    std::random_device rd;
    std::mt19937 generator(rd());

    std::vector<T> sample;
    sample.reserve(sampleSize);

    std::sample(points.begin(), points.end(), std::back_inserter(sample),
                sampleSize, generator);
    */

    // Sort the sample along the given axis -> O(sampleSize * log(sampleSize))
    std::sort(sample.begin(), sample.end(), [axis](Point * a, Point *b) {
        return (*a)[axis] < (*b)[axis];
    });

    // Get the median value of the sample
    double median = (*sample[localSampleSize / 2])[axis];

    // Reorder the vector so that the elements before the median are smaller than the median and the elements after the median are greater than the median -> O(size)
    typename std::vector<Point*>::iterator medianIterator = std::partition(points.begin(), points.end(), [median, axis](Point * a) {
        return (*a)[axis] <= median;
    });

    leftPoints.assign(points.begin(), medianIterator);

    if (medianIterator == points.end())
        rightPoints.clear();
    else
        rightPoints.assign(medianIterator, points.end());
    
    for (auto it = leftPoints.begin(); it != leftPoints.end(); ++it)
    {
        if ((**it)[axis] == median) {
            Point * medianPoint = *it;
            leftPoints.erase(it);
            return medianPoint;
        }
    }
    return nullptr;
}


/**
 * @brief Build the tree
 * 
 * @param points The points to build the tree from
 * @param node The current node
 * @param depth The current depth
 * @param threadNo The current thread number
 * @param parent The parent of the current node
 * @param right Whether the current node is the right child of its parent
 *
*/
/*
Time Complexity: O(n logn)
 -> splitVector: O(n logn)
 -> recurvisely call buildTree: O(logn)
    -> O(n logn + logn) = O(n logn)
Space Complexity: 

*/
template <class Point>
void KdTree<Point>::buildTree(std::vector<Point*> &points, KdTreeNode<Point>* &node, int depth, int threadNo, KdTreeNode<Point> *parent)
{    
    // Case: reached the leaf 
    if (points.size() == 1)
    {
        KdTreeNode<Point> * nodeObj = new KdTreeNode<Point>(points[0], parent);
        node = nodeObj;
        return;
    }
    else if (points.empty())
    {
        return;
    }
    
    // Start clock for thread
    auto start = std::chrono::system_clock::now();

    std::vector<Point*> leftPoints;
    std::vector<Point*> rightPoints;

    // Split the vector into two vectors, using random sampling to determine the median
    Point *median = this->splitVector(points, depth, leftPoints, rightPoints);

    points.clear();

    // End clock for thread
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsedSeconds = end - start;

    // Store the time taken to split
    sortTime[threadNo] += ((double)elapsedSeconds.count());

    // Create the new node and set it as the parent's child
    KdTreeNode<Point> * nodeObj = new KdTreeNode<Point>(median, parent);

    node = nodeObj;

    // Determine whether to spawn a new thread or not
    if (numeThreadsAtomic < this->maxNumThreads && (leftPoints.size() + rightPoints.size() > this->threadSplitThreshold))
    {
        numeThreadsAtomic++;

        // Use thread to build left subtree
        std::thread leftThread(&KdTree<Point>::buildTree, this, std::ref(leftPoints), std::ref(node->left), depth + 1, threadNo + 1, node);
        
        // Build right subtree in current thread
        this->buildTree(rightPoints, std::ref(node->right), depth + 1, threadNo, node);

        leftThread.join();

        numeThreadsAtomic--;
    }
    else
    {
        this->buildTree(leftPoints, std::ref(node->left), depth + 1, threadNo, node);
        this->buildTree(rightPoints, std::ref(node->right), depth + 1, threadNo, node);
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
/*
Time Complexity: O(d) -> Constant time as the number of dimensions is constant -> O(1)
Space Complexity: O(1)
*/
template <class Point>
bool KdTree<Point>::inRange(Point *point, Point *min, Point *max)
{
    int dimensions = (int)(point->dimensions());
    for (int i = 0; i < dimensions; i++)
    {
        if ((*point)[i] < (*min)[i] || (*point)[i] > (*max)[i])
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief Returns the points of the subtree in a vector
 * 
 * @param node The root of the subtree
 */
/*
Time Complexity: O(n)
Space Complexity: O(n) 
 - where n is the number of nodes in the subtree
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
/*
Time complexity: O(dimension) -> Constant time -> O(1)
Space complexity: O(1) -> Constant space
*/
template <class Point>
bool KdTree<Point>::subregionContained(Point * min, Point * max, std::map<int, std::pair<double, double>> kdTreeRange) {
    // Go through each dimension
    int dimensions = (int)(min->dimensions());
    for (int i = 0; i < dimensions; i++) {
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
/*
Time complexity: O(dimension) -> Constant time -> O(1)
Space complexity: O(1) -> Constant space
*/
template <class Point>
bool KdTree<Point>::subregionIntersects(Point * min, Point * max, std::map<int, std::pair<double, double>> kdTreeRange) {
    
    int dimensions = (int)(min->dimensions());
    // Go through each dimension
    for (int i = 0; i < dimensions; i++) {
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
/*
Time complexity: Worst Case O(n), Average Case O(log(n)) -> ACHO QUE ISTO ESTÁ CORRETO
AVERAGE CASE -> "This can be affected by the balancedness of the tree and the distribution of points within the specified range." 
    "It's important to note that the concept of "average case" can vary depending on the specific application and the characteristics of the input data. 
    Analyzing the average case complexity often involves considering statistical properties and assumptions about the input distribution."
        (ChatFDP - 18/05/2023)
 - inRange: O(dimension) -> Constant time -> O(1)
 - subregionContained: O(dimension) -> Constant time -> O(1)
 - subregionIntersects: O(dimension) -> Constant time -> O(1)
 - rangeSearch: O(n) -> n is the number of nodes in the kd tree -> TODO: Tenho que ver melhor
 - reportSubtree: O(n) -> n is the number of nodes in the kd tree
  -> O(dimension) + O(dimension) + O(dimension) + O(n) + O(n) = O(n)
Space complexity: Worst case O(n), Average case O(log(n)) -> ACHO QUE ISTO ESTÁ CORRETO
 - reportSubTree: Worst Case O(n)
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
    int dimensions = (int)(node->point)->dimensions();
    int axis = depth % dimensions;

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
    node->print(depth);
    for (int i = 0; i < depth; i++)
    {
        std::cout << " ";
    }
    std::cout << "Split axis: " << node->point->getDimensionName(depth % (int)(node->point->dimensions())) << std::endl;
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
/*
Time complexity: O(log(n))
 - Recursion: O(log(n))
Space Complexity: O(1)
*/
template <class Point>
std::pair<KdTreeNode<Point> *, int> KdTree<Point>::traverseTreeToLeaf(KdTreeNode<Point> *node, Point * point, int depth) {
    
    int dimensions = (int)(point->dimensions());
    int axis = depth % dimensions;
    // If the point is lesser than or equal to the current node, go left
    if ((*point)[axis] <= (*(node->point))[axis]) {
        // If the left node is null
        if (node->left == nullptr) {
            // If the right node is not null, go right
            if (node->right != nullptr) {
                return traverseTreeToLeaf(node->right, point, depth + 1);
            }
            // If the right node is null, return the current node (leaf)
            return std::pair(node, depth);
        }
        // If the left node is not null, go left
        return traverseTreeToLeaf(node->left, point, depth + 1);
    }
    // If the point is greater than the current node, go right
    else {
        // If the right node is null
        if (node->right == nullptr) {
            // If the left node is not null, go left
            if (node->left != nullptr) {
                return traverseTreeToLeaf(node->left, point, depth + 1);
            }
            // If the left node is null, return the current node (leaf) 
            return std::pair(node, depth);
        }
        // If the right node is not null, go right
        return traverseTreeToLeaf(node->right, point, depth + 1);
    }
}

/**
 * @brief kNearestNeighborSearch - returns the k nearest neighbors of a query point
 * @param node - the root of the tree
 * @param query - the query point
 * @param k - the number of nearest neighbors to return
 * @param cutoff - the node to stop searching at
*/
/*
Time Complexity:
 - traverseTreeToLeaf: O(log(n))
Space Complexity: O(k)

*/
template <class Point>
std::priority_queue<Point *, std::vector<Point *>, ComparePointsClosestFirst<Point>> KdTree<Point>::kNearestNeighborSearch(KdTreeNode<Point> *node, Point * query, size_t k, KdTreeNode<Point> * cutoff, int depth)
{
    // Traverse tree to leaf
    std::pair<KdTreeNode<Point> *, int> leafDepthPair = traverseTreeToLeaf(node, query, depth);
    KdTreeNode<Point> * leaf = leafDepthPair.first;
    int currentDepth = leafDepthPair.second;

    // Initialize priority queue with ComparePointsFarthestFirst so queue.top() is the farthest point currently in the queue
    std::priority_queue<Point *, std::vector<Point *>, ComparePointsFarthestFirst<Point>> queue{ComparePointsFarthestFirst<Point>(query)};

    // Initialize current and previous nodes
    KdTreeNode<Point> *current = leaf, *previous = nullptr;
    
    queue.push(current->point);

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
        KdTreeNode<Point> *otherChild = current->left == previous ? current->right : current->left;

        currentDepth--;
        
        if (otherChild == nullptr) {
            continue;
        }
        
        int dimensions = (int)(query->dimensions());
        int newDim = currentDepth % dimensions;
        
        // Check if there can be a closer point in other branch
        // Check if hyperplane of the new node can have a closer point than the farthest point in the queue
        // To do this we check if the distance between the query point and the hyperplane is less than the distance between the query point and the farthest point in the queue
        if (distance((*query), copyPointWithDimension(*query, newDim, (*(current->point))[newDim])) < distance((*query), (*queue.top()))) {
            // If there can be a closer point, search the other branch
            std::priority_queue<Point *, std::vector<Point *>, ComparePointsClosestFirst<Point>> otherPoints = kNearestNeighborSearch(otherChild, query, k, current, currentDepth + 1);

            // Add the points from the other branch to the queue
            while (!otherPoints.empty()) {
                Point *point = otherPoints.top();
                otherPoints.pop();
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
