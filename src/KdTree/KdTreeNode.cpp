#include "KdTreeNode.h"
#include <iostream>

template <class Point>
KdTreeNode<Point>::KdTreeNode(Point point) {
    this->point = point;
    this->left = nullptr;
    this->right = nullptr;
}

template <class Point>
KdTreeNode<Point>::~KdTreeNode() {
    delete left;
    delete right;
}

template <class Point>
void KdTreeNode<Point>::print() {
    std::cout << point << std::endl;
}