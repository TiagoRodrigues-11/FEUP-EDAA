#ifndef KDTREE_H
#define KDTREE_H

#include <vector>
#include "KdTreeNode.h"

template <class Point>
class KdTree {
    private:
        KdTreeNode<Point>* root;
        void _insert(KdTreeNode<Point>* node, Point point, int depth, int k);
        void _remove(KdTreeNode<Point>* node, Point point, int depth, int k);
        bool inRange(Point point, Point min, Point max);
        
    public:
        KdTree();
        KdTree(std::vector<Point> points);
        ~KdTree();
        void insert(Point point);
        void remove(Point point);
        void print(KdTreeNode<Point>* node, int depth);
        void rangeSearch(Point point, Point min, Point max, int depth, std::vector<Point> &points);
        Point nearestNeighborSearch(KdTreeNode<Point> node, Point query, int depth, double best_dist);
        
};


#endif