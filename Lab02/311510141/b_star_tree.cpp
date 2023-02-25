#include "b_star_tree.h"

const double INF = 1e8;

int random_function(int i){
    return rand() % i;
}

Node :: Node (string temp_name, int temp_width, int temp_height, int temp_idx){
    name = temp_name;
    width = temp_width;
    height = temp_height;
    idx = temp_idx;

    ptr_left = ptr_right = ptr_parent = 0;
    x1 = x2 = y1 = y2 = 0;
    mid_x = mid_y = 0;

}


Term :: Term(string _name, int _x, int _y){
    
    name = _name;
    x = _x;
    y = _y;

}

Btree :: Btree(){
    root = 0;
    t_width = t_height = 0;
    cost = total_wireLength = 0;
    total_area = 0;
}



void Btree :: print_node_list(){


    int size = node_list.size();
    for(int i = 0; i < size; i++){
        cout << node_list[i]->idx << endl;
    }

}



void Btree :: print_node_map(){

    map<string, Node*>::iterator iter;

    for(iter = node_map.begin(); iter != node_map.end(); iter++){
        
        cout << iter->first << " " << iter->second->width << " " << iter->second->height << endl;
    
    }
    cout << endl;
}

void Btree :: print_term_map(){

    map<string, Term*>::iterator iter;

    for(iter = term_map.begin(); iter != term_map.end(); iter++){
        
        cout << iter->first << " " << iter->second->x << " " << iter->second->y << endl;
    
    }
    cout << endl;
}

void Btree :: print_net_list(){

    int size = net_list.size();
    for(int i = 0; i < size; i ++){

        cout << "net_list[" << i << "] : " << endl;
        cout << "terms : " << endl;
        int size_terms = net_list[i]->terms.size();
        for(int j = 0; j < size_terms; j++){
            cout << net_list[i]->terms[j]->name << endl;
        }
        int size_nodes = net_list[i]->nodes.size();
        cout << "nodes : " << endl;
        for(int k = 0; k < size_nodes; k++){
            cout << net_list[i]->nodes[k] << endl;
        }
        cout << endl;
    }

}


void Btree :: build_initial_Graph(Node* node, bool fromright){

    if(node->idx == 0){

        contour.clear();
        t_width = t_height = 0;
        node->x1 = node->y1 = 0;

        int size = node->width;
        while(contour.size() < size){
            contour.push_back(0);
        }

    }else{

        if(fromright){
            node->x1 = node->ptr_parent->x1;
        }else{
            node->x1 = node->ptr_parent->x2;
        }
        node->y1 = 0;
        for(int i = node->x1; i < node->x1 + node->width; i++){
            while(contour.size() < i+1){
                contour.push_back(0);
            }

            if(node->y1 < contour[i]){
                node->y1 = contour[i];
            }
        }

    }
    node->x2 = node->x1 + node->width;
    node->y2 = node->y1 + node->height;
    node->mid_x = (double)(node->x1 + node->x2) / 2;
    node->mid_y = (double)(node->y1 + node->y2) / 2;



    for(int i = node->x1; i < node->x2; i++){
        if(node->y2 > contour[i]){
            contour[i] = node->y2;
        }
    }
    
}

void Btree :: initialTree(){

    Node* current;
    Node* left_most = node_list[0];
    Node* right_most = node_list[0];


    for(int i = 0; i < block_num; i++){

        current = node_list[i];

        if(left_most->x2 + node_list[i]->width > outline_width){

            right_most->ptr_right = current;
            current->ptr_parent = right_most;
            left_most = right_most = current;
            build_initial_Graph(node_list[i], true);
            continue;

        }

        left_most->ptr_left = current;
        current->ptr_parent = left_most;
        left_most = current;

       
        build_initial_Graph(node_list[i], false);

            if(node_list[i]->x2 > t_width){
                t_width = node_list[i]->x2;
            }
        
        if(node_list[i]->y2 > t_height){
            t_height = node_list[i]->y2;
        }

        
    }
    node_list[0]->ptr_parent = 0;

}

void Btree::build_Graph(Node* node, bool formright){


    if(node->idx == root){
        contour.clear();
        t_width = t_height = 0;
        node->x1 = node->y1 = 0;
        
        int size = node->width;
        while(contour.size() < size){
            contour.push_back(0);
        }
    }else{
        if(formright){

            node->x1 = node->ptr_parent->x1;

        }else{

            node->x1 = node->ptr_parent->x2;

        }

        node->y1 = 0;
        for(int i = node->x1; i < node->x1 + node->width; i++){
            while(contour.size() < i+1){
                contour.push_back(0);
            }

            if(node->y1 < contour[i]){
                node->y1 = contour[i];
            }
        }

    }
    node->x2 = node->x1 + node->width;
    node->y2 = node->y1 + node->height;
    node->mid_x = (double)(node->x1 + node->x2) / 2;
    node->mid_y = (double)(node->y1 + node->y2) / 2;

    // << "(" << node->x1 << " " << node->y1 << ")" << "(" << node->x2 << " " << node->y2 << ")" << endl;

    for(int i = node->x1; i < node->x2; i++){
        if(node->y2 > contour[i]){
            contour[i] = node->y2;
        }
    }

    if(node->x2 > t_width){
        t_width = node->x2;
    }
        
    if(node->y2 > t_height){
        t_height = node->y2;
    }

    if(!node->ptr_left){

    }else{
        build_Graph(node->ptr_left, false);
    }

    if(!node->ptr_right){

    }
    else{
        build_Graph(node->ptr_right, true);
    }

}


void Btree :: op1_rotate(int node_idx){
    Node* target_node = node_list[node_idx];
    int temp = target_node->width;
    target_node->width = target_node->height;
    target_node->height = temp; 
}

void Btree :: op2_move(int node1_idx, int node2_idx){

    op2_parent_Node = 0;
    op2_child_Node = 0;

    //////////////delete
    if(node_list[node1_idx]->ptr_left == 0 && node_list[node1_idx]->ptr_right == 0){
        /////no child

        Node* parent_node = node_list[node1_idx]->ptr_parent;


        if(parent_node->ptr_left == node_list[node1_idx]){

            parent_node->ptr_left = 0;
            op2_original_side = false;


        }else if(parent_node->ptr_right == node_list[node1_idx]){

            parent_node->ptr_right = 0;
            op2_original_side = true;


        }else{
            cout << "Error, node is not parent's child." << endl;
            exit(1);
        }

        op2_parent_Node = parent_node;
        

    }else if(node_list[node1_idx]->ptr_left != 0 && node_list[node1_idx]->ptr_right != 0){
        /////two child


    }else{
        /////one child
        Node* child_node;
        if(node_list[node1_idx]->ptr_left != 0){

            child_node = node_list[node1_idx]->ptr_left;
            op2_original_child_side = false;
        
        }else{

            child_node = node_list[node1_idx]->ptr_right;
            op2_original_child_side = true;

        }

        Node* parent_node = node_list[node1_idx]->ptr_parent;

        if(node1_idx == root){
            root = child_node->idx;
            
        }else{

            if(parent_node->ptr_left == node_list[node1_idx]){

                parent_node->ptr_left = child_node;

            }else if(parent_node->ptr_right == node_list[node1_idx]){

                parent_node->ptr_right = child_node;

            }else{

                cout << "Error, node is not parent's child also not root." << endl;
                    exit(1);
            }

        }

        child_node->ptr_parent = parent_node;


        op2_parent_Node = parent_node;
        op2_child_Node = child_node;
    }

    //////////////insert
    int random_situation = rand() % 4;
    Node* child_node;
    if(random_situation == 0){

        ////////////////left left
        child_node = node_list[node2_idx]->ptr_left;
        node_list[node1_idx]->ptr_left = child_node;
        node_list[node1_idx]->ptr_right = 0;
        node_list[node2_idx]->ptr_left = node_list[node1_idx];

    }else if(random_situation == 1){

        ////////////////right left
        child_node = node_list[node2_idx]->ptr_right;
        node_list[node1_idx]->ptr_left = child_node;
        node_list[node1_idx]->ptr_right = 0;
        node_list[node2_idx]->ptr_right = node_list[node1_idx];

    }else if(random_situation == 2){

        ////////////////left right
        child_node = node_list[node2_idx]->ptr_left;
        node_list[node1_idx]->ptr_left = 0;
        node_list[node1_idx]->ptr_right = child_node;
        node_list[node2_idx]->ptr_left = node_list[node1_idx];

    }else{

        ////////////////right right
        child_node = node_list[node2_idx]->ptr_right;
        node_list[node1_idx]->ptr_left = 0;
        node_list[node1_idx]->ptr_right = child_node;
        node_list[node2_idx]->ptr_right = node_list[node1_idx];

    }

    node_list[node1_idx]->ptr_parent = node_list[node2_idx];

    if(child_node != 0){
        child_node->ptr_parent = node_list[node1_idx];
    }

}

void Btree :: op3_swap(int node1_idx, int node2_idx){
    
    string temp_name = node_list[node2_idx]->name;
    int temp_width = node_list[node2_idx]->width;
    int temp_height = node_list[node2_idx]->height;

    node_list[node2_idx]->name = node_list[node1_idx]->name;
    node_list[node2_idx]->width = node_list[node1_idx]->width;
    node_list[node2_idx]->height = node_list[node1_idx]->height;

    node_list[node1_idx]->name = temp_name;
    node_list[node1_idx]->width = temp_width;
    node_list[node1_idx]->height = temp_height;

}


void Btree :: op2_move_recover(int node1_idx){

    //////////////delete
    if(node_list[node1_idx]->ptr_left == 0 && node_list[node1_idx]->ptr_right == 0){
        /////no child
        Node* parent_node = node_list[node1_idx]->ptr_parent;
        if(parent_node->ptr_left == node_list[node1_idx]){

            parent_node->ptr_left = 0;
            node_list[node1_idx]->ptr_parent = 0;

        }else if(parent_node->ptr_right == node_list[node1_idx]){

            parent_node->ptr_right = 0;
            node_list[node1_idx]->ptr_parent = 0;

        }else{

            cout << "Error, node is not parent's child." << endl;
            exit(1);
        }


    }else if(node_list[node1_idx]->ptr_left != 0 && node_list[node1_idx]->ptr_right != 0){
        /////two child



    }else{
        /////one child

        Node* child_node;
        if(node_list[node1_idx]->ptr_left != 0){

            child_node = node_list[node1_idx]->ptr_left;
            node_list[node1_idx]->ptr_left = 0;
        
        }else{

            child_node = node_list[node1_idx]->ptr_right;
            node_list[node1_idx]->ptr_right = 0;

        }

        Node* parent_node = node_list[node1_idx]->ptr_parent;

        if(parent_node->ptr_left == node_list[node1_idx]){

            parent_node->ptr_left = child_node;
            node_list[node1_idx]->ptr_parent = 0;

        }else if(parent_node->ptr_right == node_list[node1_idx]){

            parent_node->ptr_right = child_node;
            node_list[node1_idx]->ptr_parent = 0;

        }else{

            cout << "Error, node is not parent's child." << endl;
            exit(1);
        }

        child_node->ptr_parent = parent_node;


    }

    //////////////insert

    if(op2_parent_Node == 0){

        if(op2_original_child_side == false){

            op2_child_Node->ptr_parent = node_list[node1_idx];
            node_list[node1_idx]->ptr_left = op2_child_Node;

        }else{

            op2_child_Node->ptr_parent = node_list[node1_idx];
            node_list[node1_idx]->ptr_right = op2_child_Node;

        }

        root = node1_idx;


    }else{


        if(op2_child_Node == 0){

            if(op2_original_side == false){

                op2_parent_Node->ptr_left = node_list[node1_idx];
                node_list[node1_idx]->ptr_parent = op2_parent_Node;

            }else{
 
                op2_parent_Node->ptr_right = node_list[node1_idx];
                node_list[node1_idx]->ptr_parent = op2_parent_Node;

            }

        }else{

            if(op2_parent_Node->ptr_left == op2_child_Node){

                op2_parent_Node->ptr_left = node_list[node1_idx];
                op2_child_Node->ptr_parent = node_list[node1_idx];
                node_list[node1_idx]->ptr_parent = op2_parent_Node;

                if(op2_original_child_side == false){
                    
                    node_list[node1_idx]->ptr_left = op2_child_Node;

                }else if(op2_original_child_side == true){

                    node_list[node1_idx]->ptr_right = op2_child_Node;

                }



            }else if(op2_parent_Node->ptr_right == op2_child_Node){

                op2_parent_Node->ptr_right = node_list[node1_idx];
                op2_child_Node->ptr_parent = node_list[node1_idx];
                node_list[node1_idx]->ptr_parent = op2_parent_Node;

                if(op2_original_child_side == false){
                    
                    node_list[node1_idx]->ptr_left = op2_child_Node;

                }else if(op2_original_child_side == true){

                    node_list[node1_idx]->ptr_right = op2_child_Node;

                }

            }
        }
    }
    //print_btree();
}


void Btree::computeArea(){

    total_area = t_height * t_width;

}

void Btree::computewireLength(){
    
    int left;
    int right;
    int top;
    int bottom;
    int total_length = 0;
    //int i = 0;
    //double HPWL = 0;
    for (vector<Net*>::iterator iter_net = net_list.begin(); iter_net < net_list.end(); iter_net++){

        left = bottom = INF;
        right = top = -INF;
        //cout << left << " " << right << " " << bottom << " " << top << endl;
        vector<Term*>& terms = (*iter_net)->terms;
        for(vector<Term*>::iterator iter_term = terms.begin(); iter_term < terms.end(); iter_term++){
            //cout << (*iter_term)->name << endl;
            //cout << (*iter_term)->x  << " " << (*iter_term)->y << endl;
            if((*iter_term)->x < left){
                left = (*iter_term)->x;   
            }
            if((*iter_term)->x > right){
                right = (*iter_term)->x;   
            }
            if((*iter_term)->y < bottom){
                bottom = (*iter_term)->y;   
            }
            if((*iter_term)->y > top){
                top = (*iter_term)->y;   
            }
            //cout << left << " " << right << " " << bottom << " " << top << endl;
        }

        //vector<Node*>& nodes = (*iter_net)->nodes;

        /*map<string, Node*> block_data_map;

        for(int i = 0; i < block_num; i++){

            block_data_map.insert(pair<string, Node*>(node_list[i]->name, node_list[i]));

        }*/

        int nodes_number = (*iter_net)->nodes.size();
        //for(vector<Node*>::iterator iter_node = nodes.begin(); iter_node < nodes.end(); iter_node++){
        for(int i = 0; i < nodes_number ; i++){

            int node_list_size = node_list.size();
            for(int j = 0; j < node_list_size; j++){
        
            //map<string, Node*>::iterator iter_net_block = block_data_map.find((*iter_net)->nodes[i]);
            
            
                if((*iter_net)->nodes[i] == node_list[j]->name){
            //cout << (*iter_node)->name << endl;
            //cout << (*iter_node)->mid_x  << " " << (*iter_node)->mid_y << endl;
                    if( node_list[j]->mid_x < left){
                        left =  node_list[j]->mid_x;   
                    }
                    if( node_list[j]->mid_x > right){
                        right =  node_list[j]->mid_x;   
                    }
                    if( node_list[j]->mid_y < bottom){
                        bottom = node_list[j]->mid_y;   
                    }
                    if( node_list[j]->mid_y > top){
                        top =  node_list[j]->mid_y;   
                    }
                }
            }  
            //cout << left << " " << right << " " << bottom << " " << top << endl;
        }
        
        if(left == INF && bottom == INF && right == -INF && top == -INF){


            total_length += 0;
            cout << "HPWL have a problem." << endl;
            break;

        }else{

            //cout << i + 1  << " " ;//<< (right - left) << " " <<(top - bottom) << endl;
            //HPWL = (right - left) + (top - bottom);
            //cout << HPWL << endl;
            total_length += (right - left) + (top - bottom);
            
        
        }
        //i += 1;
        //cout << total_length << endl;
    }
    
    total_wireLength = total_length;
    
}

int Btree::computeCost(){

    computeArea();
    computewireLength();
    cost =  int(alpha*(t_height * t_width) + (1.0 - alpha) * total_wireLength);
    return cost;

}

double Btree::computeNoFixedCost(){

    computeArea();
    computewireLength();
    double delta_width = t_width - outline_width;
    double delta_height = t_height - outline_height;
    double width_squart;
    double height_squart;

    if(delta_width > 0){
        width_squart = pow(delta_width, 2);
    }else{
        width_squart = 0;
    }

    if(delta_height > 0){
        height_squart = pow(delta_height, 2);
    }else{
        height_squart = 0;   
    }

    return (width_squart + height_squart);

}

bool Btree::inOutline(){

    if (t_width <= outline_width && t_height <= outline_height){
        return true;
    }else{
        return false;
    }
}


void Btree::print_btree(){
    int order = 0;
    print_btree(node_list[root], order);
}

void Btree::print_btree(Node* current_node, int order){
    
    if(current_node != 0){
        cout << current_node->name << " " << "idx : " << current_node->idx << " " << "mid_x :" << current_node->mid_x << " mid_y : " << current_node->mid_y << endl;
        cout << "x1 : "<< current_node->x1 <<"x2 : "<< current_node->x2 << "y1 : "<< current_node->y1 << "y2 : "<< current_node->y2 << endl;
        cout << order << endl;
        order += 1;
        print_btree(current_node->ptr_left, order);
        print_btree(current_node->ptr_right, order);
    }
}