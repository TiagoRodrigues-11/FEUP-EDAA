#ifndef KDTREE_H
#define KDTREE_H

#include <vector>

template <class Point>
class KdTreeNode {
    private: 
        Point Point
        KdTreeNode* left;
        KdTreeNode* right;

    public:
        KdTreeNode(Point point);
        ~KdTreeNode();
};


template <class Point>
class KdTree {
    private:
        KTreeNode* root;
        void _insert(KdTreeNode* node, Point point, int depth, int k);
        void _remove(KdTreeNode* node, Point point, int depth, int k);
        bool inRange(Point point, Point min, Point max);
    public:
        KdTree();
        ~KdTree();
        void insert(Point point);
        void remove(Point point);
        void rangeSearch(Point point, Point min, Point max, int depth, std::vector<Point> &points);
        Point nearestNeighborSearch(KTreeNode node, Point query, int depth, double best_dist);
        
}


#endif