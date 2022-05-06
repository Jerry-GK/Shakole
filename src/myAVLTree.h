//This header non-recursively implements an AVL tree 
//Support insertion and find, used for searching terms in posting list in this project
#pragma once
#include <iostream>
using namespace std;
#define MAX_SIZE 1000
typedef pair<string,int> ElementType;

int max_ele(int a, int b);

struct _Node
{
    ElementType val;//content value
    int depth;//depth of a node. 0 for leaves, -1 for NULL(also represents the height of a tree whose root is this node)
    _Node* left;//pointer to the left child
    _Node* right;//pointer to the right child
    _Node* parent;//NULL for the root
};
typedef struct _Node TreeNode;
typedef TreeNode* Tree;


class AVLTree
{
public:
    Tree root;//root points to the root node of the AVL tree
    int NodesNum;//total nodes number

    AVLTree();
    void Insert(ElementType val);//insert into the tree, find the first unblanced node(finder) and do only one rotation
    int Find(string word);

    //Rotations
    void RR_rotate(Tree& T);
    void LL_rotate(Tree& T);
    void RL_rotate(Tree& T);
    void LR_rotate(Tree& T);
    int getBF(Tree T);//get the BF value (height of left subtree - height of right subtree)
    void Update_depth_Insert(TreeNode* start);//depth-update and single-rotation-control function for insertion
};