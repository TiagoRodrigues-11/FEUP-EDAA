#include "KdTree.h"
#include <cmath>
#include <iostream>

template <class Point>
KdTree<Point>::KdTree() {
    this->root = nullptr;
}

template <class Point>
KdTree<Point>::KdTree(std::vector<Point> points) {
    this->root = nullptr;
    for (size_t i = 0; i < points.size(); i++) {
        this->insert(points[i]);
    }
}

template <class Point>
KdTree<Point>::~KdTree() {
    delete root;
}

// TODO: make operator < for Point class
template <class Point>
bool KdTree<Point>::inRange(Point point, Point min, Point max) {
    for (int i = 0; i < point.dimensions(); i++) {
        if (point[i] < min[i] || point[i] > max[i]) {
            return false;
        }
    }
    return true;
}

template<class Point>
double distance(Point point1, Point point2) {
    double sum = 0;
    for (int i = 0; i < point1.dimensions(); i++) {
        sum += pow(point1[i] - point2[i], 2);
    }
    return sqrt(sum);
}


template <class Point>
void KdTree<Point>::insert(Point point) {
    if (this->root == nullptr) {
        this->root = new KdTreeNode<Point>(point);
    } else {
        this->_insert(this->root, point, 0, point.dimensions());
    }
}

template <class Point>
void KdTree<Point>::_insert(KdTreeNode<Point>* node, Point point, int depth, int k) {
    if (node == nullptr) {
        node = new KdTreeNode<Point>(point);
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
void KdTree<Point>::rangeSearch(Point point, Point min, Point max, int depth, std::vector<Point> &points) {
    if (this->root == nullptr) {
        return;
    }

    // Check if current node is in range
    if (inRange(point, min, max)) {
        points.push_back(point);
    }

    // Check whether the left or right subtree is in range
    int axis = depth % point.dimensions();
    if (min[axis] <= point[axis]) {
        this->rangeSearch(point->left, min, max, depth + 1, points);
    }
    if (max[axis] >= point[axis]) {
        this->rangeSearch(point->right, min, max, depth + 1, points);
    }

}

template <class Point>
void KdTree<Point>::print(KdTreeNode<Point>* node, int depth) {
    if (node == nullptr) {
        return;
    }
    for (int i = 0; i < depth; i++) {
        std::cout << " ";
    }
    std::cout << node->point << std::endl;
    this->print(node->left, depth + 1);
    this->print(node->right, depth + 1);
}

template <class Point>
Point KdTree<Point>::nearestNeighborSearch(KdTreeNode<Point> node, Point query, int depth, double best_dist) {
    if (this->root == nullptr) {
        return;
    }
    Point best_point;

    // Check if current node is closer than best_dist
    double dist = distance(node->point, query);
    if (dist < best_dist) {
        best_dist = dist;
        best_point = node->point;
    }

    // Check which side of the tree to search first
    int axis = depth % query.dimensions();
    KdTreeNode<Point>* good_side;
    KdTreeNode<Point>* bad_side;
    if (query[axis] <= node->point[axis]) {
        good_side = node->left;
        bad_side = node->right;
    } else {
        good_side = node->right;
        bad_side = node->left;
    }

    // Search good side first
    Point good_side_best = this->nearestNeighborSearch(good_side, query, depth + 1, best_dist);
    double good_side_best_dist = distance(good_side_best, query);
    if (good_side_best_dist < best_dist) {
        best_dist = good_side_best_dist;
        best_point = good_side_best;
    }

    // Check if bad side is worth searching
    if (distance(query[axis], node->point[axis]) < best_dist) {
        Point bad_side_best = this->nearestNeighborSearch(bad_side, query, depth + 1, best_dist);
        double bad_side_best_dist = distance(bad_side_best, query);
        if (bad_side_best_dist < best_dist) {
            best_dist = bad_side_best_dist;
            best_point = bad_side_best;
        }
    }
}




