#ifndef B_STAR_TREE_H
#define B_STAR_TREE_H
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <limits>
#include <cmath>
using namespace std;


class Node
{
public:

    Node* ptr_left;
    Node* ptr_right;
    Node* ptr_parent;
    int x1, x2, y1, y2;

    string name;
    double mid_x, mid_y;

    int idx;
    int width, height;
    Node(string, int, int, int);



};

class Term 
{
public:
    Term(string , int , int );

    string name;
    int x;
    int y;
}; 

class Net
{
public:
    Net(){};

    vector<Term*> terms;
    vector<string> nodes;
    

};


class Btree
{
private:

public:
    
    static int outline_width, outline_height;
    static int block_num;
    static double alpha;

    int t_width, t_height;
    int total_wireLength;
    int total_area;
    int cost;
    double no_fixed_cost;
    int root;
    vector<Node*> node_list;
    vector<Net*> net_list;
    vector<int> contour;
    map<string, Node*> node_map;
    map<string, Term*> term_map;

    Btree();
    void initialTree();
    void build_initial_Graph(Node*, bool);
    void build_Graph(Node*, bool);

    bool inOutline();
    void computewireLength();
    void computeArea();
    double computeNoFixedCost();
    int computeCost();
    
    void op1_rotate(int);
    void op2_move(int, int);
    void op3_swap(int, int);
    void op2_move_recover(int);
    Node* op2_parent_Node;
    Node* op2_child_Node;
    bool op2_original_side;
    bool op2_original_child_side;


    void print_node_list();
    void print_node_map();
    void print_term_map();
    void print_net_list();
    void print_btree();
    void print_btree(Node*, int);
    
};

#endif