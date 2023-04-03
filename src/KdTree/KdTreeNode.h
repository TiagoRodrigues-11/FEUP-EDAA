#ifndef KDTREENODE_H
#define KDTREENODE_H

template <class Point>
class KdTreeNode {
    private: 
        Point point;
        KdTreeNode<Point>* left;
        KdTreeNode<Point>* right;

    public:
        KdTreeNode(Point point);
        ~KdTreeNode();

        void print();
};

#endif