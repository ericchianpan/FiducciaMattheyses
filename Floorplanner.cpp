#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <iterator>
#include <cmath>
#include <random>
#include <chrono>
#include <algorithm>
#include <ctime>
#include "Floorplanner.h"
#include "BinaryTree.h"
using namespace std;

Floorplanner::Floorplanner(string alpha_value)
{
    srand(time(NULL));
    alpha = stod(alpha_value);
    start = clock();
}

Floorplanner::~Floorplanner()
{

}

double Floorplanner::compute_HPWL(map<string, pair<int, int>> block_info, map<string, rectangle> coordinate)
{
    int HPWL = 0;
    for(unsigned int i = 0; i < net.size(); ++i)
    {
        double min_x = outline_width;
        double min_y = outline_height;
        double max_x = 0;
        double max_y = 0;
        for(auto it = net[i].begin(); it != net[i].end(); ++it)
        {
            if(block_info.find(*it) != block_info.end()) /* block */
            {
                rectangle block_rec = coordinate[*it]; 
                if(block_rec.left_down_x + block_info[*it].first / 2 < min_x) min_x = block_rec.left_down_x + block_info[*it].first / 2;
                if(block_rec.left_down_y + block_info[*it].second / 2 < min_y) min_y = block_rec.left_down_y + block_info[*it].second / 2;
                if(block_rec.left_down_x + block_info[*it].first / 2 > max_x) max_x = block_rec.left_down_x + block_info[*it].first / 2;
                if(block_rec.left_down_y + block_info[*it].second / 2 > max_y) max_y = block_rec.left_down_y + block_info[*it].second / 2;
            }
            else /* terminal */
            {
                auto terminal_it = terminal.find(*it);
                if(terminal_it->second.first < min_x) min_x = terminal_it->second.first;
                if(terminal_it->second.second < min_y) min_y = terminal_it->second.second;
                if(terminal_it->second.first > max_x) max_x = terminal_it->second.first;
                if(terminal_it->second.second > max_y) max_y = terminal_it->second.second;
            }
        }
        HPWL += (max_x - min_x) + (max_y - min_y);
    }
    return HPWL;
}

void Floorplanner::DFS(BinaryTree &binary_tree, tree_node *node, map<string, pair<int, int>> block_info, map<string, rectangle> &coordinate)
{
    if(node == NULL) return;
    string block_name = node->get_key();
    if(node->get_parent() != NULL)
    {
        if(node->child) /* node is left child */
        {
            node->x_coordinate = node->get_parent()->x_coordinate + block_info[node->get_parent()->get_key()].first;
            if(node->x_coordinate + block_info[block_name].first > binary_tree.max_x) 
                binary_tree.max_x = node->x_coordinate + block_info[block_name].first;
        }
        else /* node is right child */
        {
            node->x_coordinate = node->get_parent()->x_coordinate;
            if(node->x_coordinate + block_info[block_name].first > binary_tree.max_x) 
                binary_tree.max_x = node->x_coordinate + block_info[block_name].first;
        }
    }

    /* y coordinate contour */
    int node_x1 = node->x_coordinate;
    int node_x2 = node->x_coordinate + block_info[block_name].first;
    int current_max_y = 0;
    Node *search_node = binary_tree.doubly_linked_list.head->get_next();
    Node *new_node = new Node();
    Node *insert_prev = NULL;
    new_node->x1 = node_x1;
    new_node->x2 = node_x2;
    while(search_node != NULL)
    {
        if(node_x1 >= search_node->x2 || node_x2 <= search_node->x1)
        {
            insert_prev = search_node;
            search_node = search_node->get_next();
        }
        else
        {
            if(node_x1 == search_node->x1 && node_x2 == search_node->x2) /* exactly */ 
            {
                node->y_coordinate = search_node->y;
                search_node->y = search_node->y + block_info[block_name].second;
                if(search_node->y > binary_tree.max_y) binary_tree.max_y = search_node->y;
                delete new_node;
                break;
            }
            else if(search_node->x2 < node_x2) /* dominate */
            {
                if(search_node->y > current_max_y) current_max_y = search_node->y;
                Node *delete_node = search_node;
                search_node = search_node->get_next();

                if(delete_node->get_prev() != NULL)
                {
                    delete_node->get_prev()->set_next(NULL);
                }
                if(search_node != NULL)
                {
                    search_node->set_prev(NULL);
                }
                delete delete_node;
            }
            else // update
            {
                if(search_node->y > current_max_y) current_max_y = search_node->y;
                if(insert_prev != NULL)
                {
                    insert_prev->set_next(new_node);
                    new_node->set_prev(insert_prev);

                    new_node->set_next(search_node);
                    search_node->set_prev(new_node);
                    search_node->x1 = new_node->x2;
                }
                else
                {
                    binary_tree.doubly_linked_list.head->set_next(new_node);
                    new_node->set_prev(binary_tree.doubly_linked_list.head);

                    new_node->set_next(search_node);
                    search_node->set_prev(new_node);
                    search_node->x1 = new_node->x2;
                }

                node->y_coordinate = current_max_y;
                new_node->y = current_max_y + block_info[block_name].second;
                if(new_node->y > binary_tree.max_y) binary_tree.max_y = new_node->y;
                break;
            }
        }
    }

    rectangle block_rectangle(node->x_coordinate, node->y_coordinate, node->x_coordinate + block_info[block_name].first, node->y_coordinate + block_info[block_name].second);
    coordinate.insert(make_pair(block_name, block_rectangle));

    DFS(binary_tree, node->get_left(), block_info, coordinate);
    DFS(binary_tree, node->get_right(), block_info, coordinate);
}

void Floorplanner::parser(string input_block, string input_net)
{
    fstream block_in(input_block, fstream::in);
    if(!block_in) { cout << "can't open the input block file" << endl; exit(1); }
    char buffer[50];
    while(block_in >> buffer)
    {
        string token(buffer);
        if(token == "Outline:") 
        { 
            block_in >> buffer;
            token = buffer;
            outline_width = stoi(token);
            block_in >> buffer;
            token = buffer;
            outline_height = stoi(token);
            continue; 
        }
        if(token == "NumBlocks:") 
        { 
            block_in >> buffer;
            token = buffer;
            block_num = stoi(buffer);
            continue;
        }
        if(token == "NumTerminals:")
        {
            block_in >> buffer;
            token = buffer;
            terminal_num = stoi(buffer);
            continue;
        }
        string macro_or_terminal = token;
        block_in >> buffer;
        token = buffer;
        if(token == "terminal")
        {
            block_in >> buffer;
            int terminal_x = atoi(buffer);
            block_in >> buffer;
            int terminal_y = atoi(buffer);
            terminal.insert(make_pair(macro_or_terminal, make_pair(terminal_x, terminal_y)));
        }
        else
        {
            int macro_width = stoi(token);
            block_in >> buffer;
            int macro_height = atoi(buffer);
            block.insert(make_pair(macro_or_terminal, make_pair(macro_width, macro_height)));
            macro.push_back(macro_or_terminal);
            sorted_macro.push_back(make_pair(make_pair(macro_width, macro_height), macro_or_terminal));
        }
    }

    block_in.close();

    fstream net_in(input_net, fstream::in);
    if(!net_in) { cout << "can't open the input block file" << endl; exit(1); }
    int net_vector_count = 0;
    while(net_in >> buffer)
    {
        string token(buffer);
        if(token == "NumNets:")
        {
            net_in >> buffer;
            int net_num = atoi(buffer);
            net.resize(net_num);
        }
        if(token == "NetDegree:")
        {
            net_in >> buffer;
            int net_deg = atoi(buffer);
            for(int i = 0; i < net_deg; ++i)
            {
                net_in >> buffer;
                string macro(buffer);
                net[net_vector_count].insert(macro);
            }
            ++net_vector_count;
        }
    }

    net_in.close();
}

void Floorplanner::random_binary_tree(tree_node *&root)
{
    bool first_iteration = true;
    for(unsigned int i = 0; i < macro.size(); ++i)
    {
        if(first_iteration)
        {
            root->set_key(macro[i]);
            first_iteration = false;
            continue;
        }

        tree_node *insert_pos = root;
        tree_node *insert_node = new tree_node(macro[i]);

        while(true)
        {
            int r = rand();
            if((r & 1) == 0)
            {
                if(insert_pos->get_left() == NULL) 
                { 
                    insert_pos->set_left(insert_node); 
                    insert_node->set_parent(insert_pos);
                    insert_node->child = true;
                    break; 
                }
                else 
                { 
                    insert_pos = insert_pos->get_left(); 
                }
            }
            else
            {
                if(insert_pos->get_right() == NULL) 
                { 
                    insert_pos->set_right(insert_node); 
                    insert_node->set_parent(insert_pos);
                    insert_node->child = false;
                    break; 
                }
                else 
                { 
                    insert_pos = insert_pos->get_right(); 
                }
            }
        }
    }
}

void Floorplanner::gnuplot(BinaryTree &binary_tree, map<string, rectangle> coordinate, string outputfile)
{
    fstream fout(outputfile, fstream::out);
    unsigned int i = 1;
    for(auto it = coordinate.begin(); it != coordinate.end(); ++it)
    {
        int x1 = it->second.left_down_x;
        int y1 = it->second.left_down_y;
        int x2 = it->second.right_up_x;
        int y2 = it->second.right_up_y;
        fout << "set object " << i << " rect from " << x1 << "," << y1 << " to " << x2 << "," << y2 << " fc rgb \"gold\"" << endl;
        i++;
    }
    fout << "set xrange [0:" << binary_tree.max_x << "]" << endl; 
    fout << "set yrange [0:" << binary_tree.max_y << "]" << endl; 
    fout << "plot sin(x) lt -1";
    fout.close();
}

void Floorplanner::operation_one(string block_key, map<string, pair<int, int>> &operation_block)
{
    auto it = operation_block.find(block_key);
    int width_temp = it->second.first;
    it->second.first = it->second.second;
    it->second.second = width_temp;
}

void Floorplanner::operation_two(BinaryTree &binary_tree_copy)
{
    // binary_tree_copy.print(binary_tree_copy.root->get_next());
    tree_node *delete_leaf = binary_tree_copy.root->get_next();
    while(true)
    {
        if(delete_leaf->get_left() == NULL && delete_leaf->get_right() == NULL)
            break;
        int r = rand();
        if((r & 1) == 0)
        {
            if(delete_leaf->get_left() != NULL) { delete_leaf = delete_leaf->get_left(); }
            else { delete_leaf = delete_leaf->get_right();}
        }
        else
        {
            if(delete_leaf->get_right() != NULL) { delete_leaf = delete_leaf->get_right(); }
            else { delete_leaf = delete_leaf->get_left(); }
        }
    }

    if(delete_leaf->child == true)
    {
        delete_leaf->get_parent()->set_left(NULL);
        delete_leaf->set_parent(NULL);
    }
    else
    {
        delete_leaf->get_parent()->set_right(NULL);
        delete_leaf->set_parent(NULL);
    }

    tree_node *insert_pos = binary_tree_copy.root->get_next();
    while(true)
    {
        int keep_on_prob = rand();
        if((keep_on_prob & 1) == 0) /* keep search */ 
        {
            int dir_prob = rand();
            if((dir_prob & 1) == 0) /* go left */
            {
                if(insert_pos->get_left() == NULL) { break; }
                else { insert_pos = insert_pos->get_left(); }
            }
            else /* go right */
            {
                if(insert_pos->get_right() == NULL) { break; }
                else { insert_pos = insert_pos->get_right(); }
            }
        }
        else /* down */
        {
            break;
        }
    }

    int dir_prob = rand();
    if((dir_prob & 1) == 0) 
    {
        if(insert_pos->get_left() == NULL)
        {
            insert_pos->set_left(delete_leaf);
            delete_leaf->set_parent(insert_pos);
            delete_leaf->child = true;
        }
        else
        {
            insert_pos->get_left()->set_parent(delete_leaf);
            delete_leaf->set_left(insert_pos->get_left());
            insert_pos->set_left(delete_leaf);
            delete_leaf->set_parent(insert_pos);
            delete_leaf->child = true;
        }
    }
    else
    {
        if(insert_pos->get_right() == NULL)
        {
            insert_pos->set_right(delete_leaf);
            delete_leaf->set_parent(insert_pos);
            delete_leaf->child = false;
        }
        else
        {
            insert_pos->get_right()->set_parent(delete_leaf);
            delete_leaf->set_right(insert_pos->get_right());
            insert_pos->set_right(delete_leaf);
            delete_leaf->set_parent(insert_pos);
            delete_leaf->child = false;
        }
    }
}

void Floorplanner::operation_three(BinaryTree &binary_tree_copy)
{
    tree_node *swap_node_one = binary_tree_copy.root->get_next();
    tree_node *swap_node_two = binary_tree_copy.root->get_next();
    while(true)
    {
        int keep_on_prob = rand();
        if((keep_on_prob & 1) == 0) /* keep search */ 
        {
            int dir_prob = rand();
            if((dir_prob & 1) == 0) /* go left */
            {
                if(swap_node_one->get_left() == NULL) { break; }
                else { swap_node_one = swap_node_one->get_left(); }
            }
            else /* go right */
            {
                if(swap_node_one->get_right() == NULL) { break; }
                else { swap_node_one = swap_node_one->get_right(); }
            }
        }
        else /* down */
        {
            break;
        }
    }
    while(true)
    {
        int keep_on_prob = rand();
        if((keep_on_prob & 1) == 0) /* keep search */ 
        {
            int dir_prob = rand();
            if((dir_prob & 1) == 0) /* go left */
            {
                if(swap_node_two->get_left() == NULL) { break; }
                else { swap_node_two = swap_node_two->get_left(); }
            }
            else /* go right */
            {
                if(swap_node_two->get_right() == NULL) { break; }
                else { swap_node_two = swap_node_two->get_right(); }
            }
        }
        else /* down */
        {
            if(swap_node_one->get_key() == swap_node_two->get_key()) continue;
            break;
        }
    }
    string tmp = swap_node_one->get_key();
    swap_node_one->set_key(swap_node_two->get_key());
    swap_node_two->set_key(tmp);
}

void Floorplanner::copy_binary_tree(tree_node *orig_node, tree_node *copy_node)
{
    if(orig_node == NULL) 
        return;
    if(orig_node->get_next() != NULL) /* root */
    {
        tree_node *new_node = new tree_node(orig_node->get_next()->get_key());
        copy_node->set_next(new_node);
    }
    if(orig_node->get_left() != NULL)
    {
        tree_node *new_left_node = new tree_node(orig_node->get_left()->get_key());
        copy_node->set_left(new_left_node);
        new_left_node->set_parent(copy_node);
        if(orig_node->get_left()->child == true) { new_left_node->child = true; }
        else { new_left_node->child = false; }
    }
    if(orig_node->get_right() != NULL)
    {
        tree_node *new_right_node = new tree_node(orig_node->get_right()->get_key());
        copy_node->set_right(new_right_node);
        new_right_node->set_parent(copy_node);
        if(orig_node->get_right()->child == true) { new_right_node->child = true; }
        else { new_right_node->child = false; }
    }
    copy_binary_tree(orig_node->get_left(), copy_node->get_left());
    copy_binary_tree(orig_node->get_right(), copy_node->get_right());
    copy_binary_tree(orig_node->get_next(), copy_node->get_next());
}

void Floorplanner::generate_initial_solution(BinaryTree &binary_tree)
{
    sort(sorted_macro.begin(), sorted_macro.end());
    reverse(sorted_macro.begin(), sorted_macro.end());

    tree_node *insert_x;
    tree_node *insert_y;
    for(unsigned int i = 0; i < sorted_macro.size(); ++i)
    {
        if(i == 0)
        {
            tree_node *new_node = new tree_node(sorted_macro[i].second);
            binary_tree.root->set_next(new_node);
            insert_x = new_node;
            insert_y = new_node;
            continue;
        }
        if(insert_x->x_coordinate + block[insert_x->get_key()].first + sorted_macro[i].first.first <= outline_width)
        {
            tree_node *new_node = new tree_node(sorted_macro[i].second);
            insert_x->set_left(new_node);
            new_node->set_parent(insert_x);
            new_node->x_coordinate = insert_x->x_coordinate + block[insert_x->get_key()].first;
            new_node->child = true;
            insert_x = new_node;
        }
        else
        {
            tree_node *new_node = new tree_node(sorted_macro[i].second);
            insert_y->set_right(new_node);
            new_node->set_parent(insert_y);
            new_node->x_coordinate = block[insert_y->get_key()].first; 
            new_node->child = false;
            insert_y = new_node;
            insert_x = new_node;
        }
    }
}

void Floorplanner::generate_initial_solution_v2(BinaryTree &binary_tree)
{
    sort(sorted_macro.begin(), sorted_macro.end());
    reverse(sorted_macro.begin(), sorted_macro.end());

    tree_node *insert_left;
    tree_node *insert_right;
    for(unsigned int i = 0; i < sorted_macro.size(); ++i)
    {
        if(i == 0)
        {
            tree_node *new_node = new tree_node(sorted_macro[i].second);
            binary_tree.root->set_next(new_node);
            continue;
        }
        insert_left = binary_tree.root->get_next();
        insert_right = binary_tree.root->get_next();
        while(insert_right != NULL)
        {
            while(insert_left->get_left() != NULL)
            {
                insert_left = insert_left->get_left();
            }
            if(insert_left->x_coordinate + block[insert_left->get_key()].first + sorted_macro[i].first.first <= outline_width)
            {
                tree_node *new_node = new tree_node(sorted_macro[i].second);
                insert_left->set_left(new_node);
                new_node->set_parent(insert_left);
                new_node->x_coordinate = insert_left->x_coordinate + block[insert_left->get_key()].first;
                new_node->child = true;
                break;
            }
            if(insert_right->get_right() == NULL)
            {
                tree_node *new_node = new tree_node(sorted_macro[i].second);
                insert_right->set_right(new_node);
                new_node->set_parent(insert_right);
                new_node->x_coordinate = insert_right->x_coordinate;
                new_node->child = false;
                break;
            }
            insert_right = insert_right->get_right();
            insert_left = insert_right;
        }
    }
}

void Floorplanner::generate_initial_solution_v3(BinaryTree &binary_tree)
{
    for(int i = 0; i < sorted_macro.size(); ++i)
    {
        int width = sorted_macro[i].first.first;
        int height = sorted_macro[i].first.second;
        sorted_macro[i].first.first = width * height; 
        sorted_macro[i].first.second = width;
    }

    sort(sorted_macro.begin(), sorted_macro.end());
    reverse(sorted_macro.begin(), sorted_macro.end());

    tree_node *insert_left;
    tree_node *insert_right;
    for(unsigned int i = 0; i < sorted_macro.size(); ++i)
    {
        if(i == 0)
        {
            tree_node *new_node = new tree_node(sorted_macro[i].second);
            binary_tree.root->set_next(new_node);
            continue;
        }
        insert_left = binary_tree.root->get_next();
        insert_right = binary_tree.root->get_next();
        while(insert_right != NULL)
        {
            while(insert_left->get_left() != NULL)
            {
                insert_left = insert_left->get_left();
            }
            if(insert_left->x_coordinate + block[insert_left->get_key()].first + sorted_macro[i].first.second <= outline_width)
            {
                tree_node *new_node = new tree_node(sorted_macro[i].second);
                insert_left->set_left(new_node);
                new_node->set_parent(insert_left);
                new_node->x_coordinate = insert_left->x_coordinate + block[insert_left->get_key()].first;
                new_node->child = true;
                break;
            }
            if(insert_right->get_right() == NULL)
            {
                tree_node *new_node = new tree_node(sorted_macro[i].second);
                insert_right->set_right(new_node);
                new_node->set_parent(insert_right);
                new_node->x_coordinate = insert_right->x_coordinate;
                new_node->child = false;
                break;
            }
            insert_right = insert_right->get_right();
            insert_left = insert_right;
        }
    }
}

void Floorplanner::output(BinaryTree binary_tree, map<string, rectangle> coordinate, double wirelength)
{
    fstream fout(outputfile, fstream::out);
    fout << alpha * (binary_tree.max_x * binary_tree.max_y / normal_area) + (1 - alpha) * (wirelength / normal_wirelength) << endl;
    fout << wirelength << endl;
    fout << binary_tree.max_x * binary_tree.max_y << endl;
    fout << binary_tree.max_x << " " << binary_tree.max_y << endl;
    clock_t finsh = clock();
    fout << (float)(finsh - start) / CLOCKS_PER_SEC << endl;
    for(auto it = coordinate.begin(); it != coordinate.end(); ++it)
    {
        fout << it->first << " ";
        fout << it->second.left_down_x << " ";
        fout << it->second.left_down_y << " ";
        fout << it->second.right_up_x << " ";
        fout << it->second.right_up_y << endl;
    }
    fout.close();
}

void Floorplanner::simulated_annealing()
{
    /* initial solution */
    BinaryTree binary_tree;
    generate_initial_solution_v3(binary_tree);
    map<string, rectangle> binary_tree_coordinate;
    DFS(binary_tree, binary_tree.root->get_next(), block, binary_tree_coordinate);
    gnuplot(binary_tree, binary_tree_coordinate, "initial.gp");
    current_cost = abs(binary_tree.max_y - outline_height) * outline_width + abs(binary_tree.max_x - outline_width) * outline_height; 

    normal_area = binary_tree.max_x * binary_tree.max_y;
    normal_wirelength = compute_HPWL(block, binary_tree_coordinate);

    /* initial parameter */
    double T = 10000;
    double t_frozen = 0.01;
    double r = 0.999;
    int count = 0;

    int move_count = 3;
    double current_optimal;
    current_optimal = current_cost;

    while(1)
    {
        BinaryTree binary_tree_copy;
        copy_binary_tree(binary_tree.root, binary_tree_copy.root);
        map<string, pair<int, int>> operation_block = block;
        map<string, rectangle> random_coordinate;
        int rand_num = rand() % 3;
        for(int move = 0; move < move_count; ++move)
        {
            switch(rand_num)
            {
                case 0:
                    {
                        int rand_block;
                        rand_block = rand() % macro.size();
                        operation_one(macro[rand_block], operation_block);
                        break;
                    }
                case 1:
                    {
                        operation_two(binary_tree_copy);
                        break;
                    }
                case 2:
                    {
                        operation_three(binary_tree_copy);
                        break;
                    }
            }
        }
        DFS(binary_tree_copy, binary_tree_copy.root->get_next(), operation_block, random_coordinate);
        if(binary_tree_copy.max_x <= outline_width && binary_tree_copy.max_y <= outline_height)
        {
            double wirelength = compute_HPWL(operation_block, random_coordinate);
            gnuplot(binary_tree_copy, random_coordinate, "result.gp");
            output(binary_tree_copy, random_coordinate, wirelength);
            break;
        }

        double copy_cost = abs(binary_tree_copy.max_y - outline_height) * outline_width + abs(binary_tree_copy.max_x - outline_width) * outline_height;
        double delta = copy_cost - current_cost;
        double random_prob = rand() % 100 + 1;
        double accept_prob = exp((-1) * delta / T) * 100;

        if(delta < 0)
        {
            block.clear();
            block = operation_block;
            tree_node *orig_root = binary_tree.root->get_next();
            tree_node *copy_root = binary_tree_copy.root->get_next();
            binary_tree.root->set_next(copy_root);
            binary_tree_copy.root->set_next(orig_root);
            binary_tree_copy.clear(binary_tree_copy.root->get_next());
            binary_tree.max_x = binary_tree_copy.max_x;
            binary_tree.max_y = binary_tree_copy.max_y;
            binary_tree_copy.root->set_next(NULL);
            current_cost = copy_cost;
        }

        if(current_cost < current_optimal)
        {
            current_optimal = current_cost;
            count = 0;
        }
        if(current_cost == current_optimal)
        {
            count++;
        }
        if(count == 10000)
        {
            count = 0;
            binary_tree.clear(binary_tree.root->get_next());
            generate_initial_solution_v3(binary_tree);
            binary_tree.max_x = 0;
            binary_tree.max_y = 0;
            DFS(binary_tree, binary_tree.root->get_next(), block, binary_tree_coordinate);
            current_cost = abs(binary_tree.max_y - outline_height) * outline_width + abs(binary_tree.max_x - outline_width) * outline_height; 
            current_optimal = current_cost;
        }
    }
}
