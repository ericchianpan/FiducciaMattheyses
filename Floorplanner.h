#ifndef Floorplanner_H
#define Floorplanner_H
#include <string>
#include <map>
#include <vector>
#include <set>
#include "BinaryTree.h"
using namespace std;

class rectangle{
public:
    rectangle() {};
    rectangle(int x1, int y1, int x2, int y2)
    {
        left_down_x = x1;
        left_down_y = y1;
        right_up_x = x2;
        right_up_y = y2;
    }
    ~rectangle() {};
    int left_down_x = 0;
    int left_down_y = 0;
    int right_up_x = 0;
    int right_up_y = 0;
private:
};

class Floorplanner{
public:
    Floorplanner(string alpha_value);
    ~Floorplanner();
    double compute_HPWL(map<string, pair<int, int>> block_info, map<string, rectangle> coordinate);
    string outputfile;
    void parser(string input_block, string input_net);
    void random_binary_tree(tree_node *&root);
    void DFS(BinaryTree &binary_tree, tree_node *node, map<string, pair<int, int>> block_info, map<string, rectangle> &coordinate);
    void gnuplot(BinaryTree &binary_tree, map<string, rectangle> coordinate, string outputfile);
    void simulated_annealing();
    void operation_one(string block_key, map<string, pair<int, int>> &operation_block); /* rotate macro */
    void operation_two(BinaryTree &binary_tree); /* delete and insert node */
    void operation_three(BinaryTree &binary_tree); /* swap two node */
    void copy_binary_tree(tree_node *orig_node, tree_node *copy_node);
    void generate_initial_solution(BinaryTree &binary_tree);
    void generate_initial_solution_v2(BinaryTree &binary_tree);
    void generate_initial_solution_v3(BinaryTree &binary_tree);
    void output(BinaryTree binary_tree, map<string, rectangle>, double wirelength);
private:
    int outline_width;
    int outline_height;
    int block_num;
    int terminal_num;
    double current_cost;
    double alpha;
    double normal_area, normal_wirelength;
    clock_t start;
    vector<string> macro; /* for random */
    vector<set<string>> net;
    vector<pair<pair<int, int>, string>> sorted_macro; /* for generate initial floorplan */
    map<string, pair<int, int>> block; /* width, height */
    map<string, pair<int, int>> terminal; /* x coordinate, y coordinate */
    map<string, rectangle> block_coordinate; 
};

#endif
