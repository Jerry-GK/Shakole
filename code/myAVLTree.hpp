//This header non-recursively implements an AVL tree 
//Support insertion and find, used for searching terms in posting list in this project
#pragma once
#include <iostream>
using namespace std;
#define MAX_SIZE 1000
typedef pair<string,int> ElementType;

int max_ele(int a, int b)
{
    return a>b?a:b;
}
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

AVLTree::AVLTree()
{
    this->root=NULL;
    NodesNum=0;
}

int AVLTree::getBF(Tree T)
{
    if(T==NULL)
        return 0;//bf value of NULL is defined to be 0
    else
    {
        int leftDepth=T->left==NULL?-1:T->left->depth;
        int rightDepth=T->right==NULL?-1:T->right->depth;
        return leftDepth-rightDepth;
    }
}

void AVLTree::Insert(ElementType newval)
{
    TreeNode* myParent=NULL;
    TreeNode** temp=&root;
    //find the correct position to insert
    while(*temp!=NULL)
    {
        myParent = *temp;
        if(newval>(*temp)->val)
        {
            temp=&((*temp)->right);
        }
        else
        {
            temp=&((*temp)->left);
        }
    }
    //allocate space for the new node
    *temp = new TreeNode;
    if(*temp==NULL)
    {
        cout<<"Error: Malloc Space Overflow!"<<endl;
        return;
    }
    (*temp)->val=newval;
    (*temp)->parent = myParent;
    (*temp)->depth=0;//depth of a leaf is 0
    (*temp)->left=(*temp)->right=NULL;

    this->Update_depth_Insert(*temp);//find the unblanced node(if exist) caused by insertion and rotate once to correct

    NodesNum++;
    //cout<<"successfully insert "<<newval<<",height = "<<this->root->depth<<endl;
}

int AVLTree::Find(string word)
{
    TreeNode* temp=root;
    while(temp!=NULL)
    {
        if(temp->val.first==word)
            return temp->val.second;
        else if(temp->val.first>word)
            temp=temp->left;
        else
            temp=temp->right;
    }
    return -1; 
}

void AVLTree::Update_depth_Insert(TreeNode* start)//update the depth of nodes from root to start->parent  
{
    ElementType maker;//trouble maker value(inserted value)
    if(start!=NULL)
    {
        maker=start->val;
        start=start->parent;
    }
    while(start!=NULL)
    {
        int leftDepth = (start->left)==NULL?-1:start->left->depth;
        int rightDepth = (start->right)==NULL?-1:start->right->depth;
        ElementType finder=start->val;
        int BF=leftDepth-rightDepth;
        if(abs(BF)>1)
        {
            //do rotation according to the relationship of the position of root, root's children and the inserted node
            if(maker<finder&&maker<start->left->val)
                this->LL_rotate(start);
            else if(maker<finder&&maker>start->left->val)
                this->LR_rotate(start);
            else if(maker>finder&&maker<start->right->val)
                this->RL_rotate(start);
            else if(maker>finder&&maker>start->right->val)
                this->RR_rotate(start);
            else
            {
                cout<<"Error:Unknown condition"<<endl;//this should never happen
            }
        }
        else
            //if rotation happens, depth of relative nodes will be updated in the rotation function
            start->depth=max_ele(leftDepth+1,rightDepth+1);
        start=start->parent;//continue go up to check balance
    }
}

//single rotation
void AVLTree::RR_rotate(Tree& T)
{
    if(T==NULL||T->right==NULL)
    {
        cout<<"Error:RR Rotation problem"<<endl;
        return;
    }

    //reconstruct the lower part
    Tree rightNode=T->right;
    T->right=rightNode->left;
    rightNode->left=T;
    T=rightNode;//T is now the new root

    //update parent message
    T->parent=T->left->parent;
    T->left->parent=T;
    if(T->left->right!=NULL)
        T->left->right->parent=T->left;
    
    //outer link
    if(T->parent==NULL)
    {
        this->root=T;
    }
    else if(T->parent->left==T->left)
    {
        T->parent->left=T;
    }
    else if(T->parent->right==T->left)
    {
        T->parent->right=T;
    }

    //update depth
    int lld = (T->left->left)==NULL?-1:T->left->left->depth;
    int lrd = (T->left->right)==NULL?-1:T->left->right->depth;
    int rd = (T->right)==NULL?-1:T->right->depth;
    T->left->depth=max_ele(lld+1,lrd+1);
    T->depth=max_ele(T->left->depth+1,rd+1);
  
    //Update_depth(T);
}

void AVLTree::LL_rotate(Tree& T)
{
    if(T==NULL||T->left==NULL)
    {
        cout<<"Error:LL Rotation problem"<<endl;
        return;
    }

    //reconstruct the lower part
    Tree leftNode=T->left;
    T->left=leftNode->right;
    leftNode->right=T;
    T=leftNode;//T is now the new root

    //update parent message
    T->parent=T->right->parent;
    T->right->parent=T;
    if(T->right->left!=NULL)
        T->right->left->parent=T->right;

    //outer link
    if(T->parent==NULL)
    {
        this->root=T;
    }
    else if(T->parent->left==T->right)
    {
        T->parent->left=T;
    }
    else if(T->parent->right==T->right)
    {
        T->parent->right=T;
    }

    //update depth
    int rrd = (T->right->right)==NULL?-1:T->right->right->depth;
    int rld = (T->right->left)==NULL?-1:T->right->left->depth;
    int ld = (T->left)==NULL?-1:T->left->depth;
    T->right->depth=max_ele(rrd+1,rld+1);
    T->depth=max_ele(T->right->depth+1,ld+1);

    //Update_depth(T);
}

//double rotation
void AVLTree::RL_rotate(Tree& T)
{
    LL_rotate(T->right);
    return RR_rotate(T);
}

void AVLTree::LR_rotate(Tree& T)
{
    RR_rotate(T->left);
    return LL_rotate(T);
}