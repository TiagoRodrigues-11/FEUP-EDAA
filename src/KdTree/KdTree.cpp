#include "KdTree.h"

// KdTreeNode Class

template <class Point>
KdTreeNode::KdTreeNode(Point point) {
    this->point = point;
    this->left = NULL;
    this->right = NULL;
}

template <class Point>
KdTreeNode::~KdTreeNode() {
    delete left;
    delete right;
}


// KdTree Class


template <class Point>
KdTree::KdTree() {
    this->root = NULL;
}

template <class Point>
KdTree::~KdTree() {
    delete root;
}

// TODO: make operator < for Point class
bool KdTree::inRange(Point point, Point min, Point max) {
    for (int i = 0; i < point.dimensions(); i++) {
        if (point[i] < min[i] || point[i] > max[i]) {
            return false;
        }
    }
    return true;
}

template <class Point>
void KdTree::insert(Point point) {
    if (this->root == NULL) {
        this->root = new KdTreeNode(point);
    } else {
        this->_insert(this->root, point, 0, point.dimensions());
    }
}

template <class Point>
void KdTree::_insert(KdTreeNode* node, Point point, int depth, int k) {
    if (node == NULL) {
        node = new KdTreeNode(point);
    } else {
        int axis = depth % k;
        if (point[axis] <= node->point[axis]) {
            this->_insert(node->left, point, depth + 1, k);
        } else {
            this->_insert(node->right, point, depth + 1, k);
        }
    }
}

template <class Point>
void KdTree::rangeSearch(Point point, Point min, Point max, int depth, vector<Point> &points) {
    if (this->root == NULL) {
        return;
    }

    // Check if current node is in range
    if (inRange(point, min, max)) {
        points.push_back(point);
    }

    // Check whether the left or right subtree is in range
    if (min[depth % k] <= point[depth % k]) {
        this->rangeSearch(node->left, min, max, depth + 1, points);
    }
    if (max[depth % k] >= point[depth % k]) {
        this->rangeSearch(node->right, min, max, depth + 1, points);
    }

}

template <class Point>
Point KdTree::nearestNeighborSearch(KTreeNode node, Point query, int depth, double best_dist) {
    if (this->root == NULL) {
        return;
    }

    // Check if current node is closer than best_dist
    double dist = node->point.distance(query);
    if (dist < best_dist) {
        best_dist = dist;
    }

    Point nearestNeighbor = node->point;

    // Determine which subtree to search first
    int axis = depth % k;
    KdTreeNode* good_side = nullptr;
    KdTreeNode* bad_side = nullptr;

    if (query[axis] <= node->point[axis]) {
        good_side = node->left;
        bad_side = node->right;
    } else {
        good_side = node->right;
        bad_side = node->left;
    }

    // Search good side first
    Point best_point_good = this->nearestNeighborSearch(good_side, query, depth + 1, best_dist);
    double dist_good_side = best_point_good.distance(query);
    if( dist_good_side < best_dist) {
        best_dist = dist_good_side;
        nearestNeighbor = best_point_good;
    }

    // Check if bad side is worth searching
    if (abs(query[axis] - node->point[axis]) < best_dist) {
        Point best_point_bad = this->nearestNeighborSearch(bad_side, query, depth + 1, best_dist);
        double dist_bad_side = best_point_bad.distance(query);
        if (dist_bad_side < best_dist) {
            best_dist = dist_bad_side;
            nearestNeighbor = best_point_bad;
        }
    }

    return nearestNeighbor;
}





