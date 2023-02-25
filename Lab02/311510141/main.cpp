#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <cmath>
#include <time.h>

#include "b_star_tree.h"

const double r = 0.6; // temperature reduction rate
const double P = 0.95;  // initial temperature parameter
const int k = 5; // trials parameter per reduction of temperature
const double EPSILON = 1e-8; // end temperature 


using namespace std;




int Btree::outline_width;
int Btree::outline_height;
int Btree::block_num;
double Btree::alpha;

int main(int argc, char* argv[]){
    
    

    /*ifstream FILEIN;
    FILEIN.open(argv[1], ios::in);*/
    //double alpha;
    //alpha = stod(argv[1]);
    //cout << "0" << endl;
    ifstream FILEIN;
    FILEIN.open(argv[2], ios::in);

    string dump;
    int outline_width, outline_height;
    int block_num, term_num;
    int net_num, deg_num;

    //clock_t start_time = clock();

    FILEIN >> dump;
    FILEIN >> outline_width >> outline_height;
    FILEIN >> dump;
    FILEIN >> block_num;
    FILEIN >> dump;
    FILEIN >> term_num;

    if(block_num == 49){
        srand(18);
    }else{
        srand(time(NULL));
    }

    Btree::outline_width = outline_width;
    Btree::outline_height = outline_height;
    Btree::block_num = block_num;
    Btree::alpha = stod(argv[1]); 
    //Btree::r_star = 1.0 * outline_height / outline_width
    //cout << "1" << endl;
    ////////////////////////////////////////parse nodes

    Btree* tree = new Btree();
    Node* node;
    string temp_name;
    int temp_width,  temp_height;
    for(int i = 0; i < block_num; i++){

        FILEIN >> temp_name >> temp_width >> temp_height;
        node = new Node(temp_name, temp_width, temp_height, i);
        tree->node_list.push_back(node);
        tree->node_map.insert(pair<string, Node*>(temp_name, node));
    }

    //tree->print_node_list();
    //tree->print_node_map();
    //cout << "2" << endl;
    ////////////////////////////////////////parse terminals
    Term* term;
    int x, y;
    for(int i = 0; i < term_num; i++){
        
        FILEIN >> temp_name >> dump >> x >> y;
        term = new Term(temp_name, x, y);
        tree->term_map.insert(pair<string, Term*>(temp_name, term));
    }

    //tree->print_term_map();

    FILEIN.close();

    FILEIN.open(argv[3], ios::in);
    FILEIN >> dump;
    FILEIN >> net_num;
    Net* net;
    //cout << "3" << endl;
    map<string, Term*>& _term_map = tree->term_map;
    //map<string, Node*>& _node_map = tree->node_map;
    
    for(int i = 0; i < net_num; i++){
        FILEIN >> dump;
        FILEIN >> deg_num;
        net = new Net();
        for(int j = 0; j < deg_num; j++){

            FILEIN >> temp_name;
            if(_term_map.find(temp_name) != _term_map.end())
                net->terms.push_back(_term_map[temp_name]);
            else
                net->nodes.push_back(temp_name);
        }
        tree->net_list.push_back(net);
    }
    FILEIN.close();

    /////////////////////////////////////////build b-star trees
    tree->initialTree();


    tree->build_Graph(tree->node_list[tree->root], false);


    ////////////////////////////////////////SimulatedAnnealing
    
    int initial_cost = tree->computeCost();
    int initial_nofixed_cost = tree->computeNoFixedCost();


    const double T0 = 100000/*-(initial_cost * block_num) / log(P)*/;
    double T = T0;
    const int N = 100;

    int MT;
    int uphill;
    int reject;

    clock_t SA_initial_time = clock();
    const int TIME_LIMIT = 297; //5 minutes
    int runtime = 0;
    int prev_cost = initial_cost;
    double prev_nofixed_cost = initial_nofixed_cost;
    bool is_inOutline = false;
    bool prev_inOutline = false;

    do{

        MT = 0;
        uphill = 0;
        reject = 0;

        do{

            // random pick operations and operates
            int op_pick = rand() % 3;
            int rand_node1_idx;
            int rand_node2_idx;

            // execute operation
            if(op_pick == 0){
                
                //cout << "choose op1" << endl;
                rand_node1_idx = rand() % block_num;

                tree->op1_rotate(rand_node1_idx);

            }else if(op_pick == 1){

                //cout << "choose op2" << endl;
                do{
                    rand_node1_idx = rand() % block_num;
                }while((tree->node_list[rand_node1_idx]->ptr_left != 0 && tree->node_list[rand_node1_idx]->ptr_right != 0));

                do{
                    rand_node2_idx = rand() % block_num;
                    if(rand_node1_idx == rand_node2_idx){
                
                    rand_node2_idx = (rand_node2_idx + 1) % block_num;
            
                    }

                }while((tree->node_list[rand_node2_idx]->ptr_left != 0 && tree->node_list[rand_node2_idx]->ptr_right != 0));

                tree->op2_move(rand_node1_idx, rand_node2_idx);

                if(rand_node1_idx == tree->root){
                    //tree->print_btree();
                    break;
                }

            }else if(op_pick == 2){

                //cout << "choose op3" << endl;
                rand_node1_idx = rand() % block_num;
                rand_node2_idx = rand() % block_num;
                if(rand_node1_idx == rand_node2_idx){
                
                rand_node2_idx = (rand_node2_idx + 1) % block_num;
            
                }

                tree->op3_swap(rand_node1_idx, rand_node2_idx);

            }else{
                cout << "Error, a wrong opration occur." << endl;
                exit(1);
            }

            MT += 1;

            tree->build_Graph(tree->node_list[tree->root], false);
            int new_cost = tree->computeCost();
            int new_nofixed_cost = tree->computeNoFixedCost();
            is_inOutline = tree->inOutline();

            int delta_cost = new_cost - prev_cost;
            double delta_nofixed_cost = new_nofixed_cost - prev_nofixed_cost;


            if(prev_inOutline){

                //cout << "原本在邊界內了" << endl;
                if(!is_inOutline){
                    
                    reject += 1;

                    //cout<< "未進入邊界," ;


                    if(op_pick == 0){
                        
                    //cout << "recover op1" << endl;

                    tree->op1_rotate(rand_node1_idx);

                    }else if(op_pick == 1){

                    //cout << "recover op2" << endl;

                    tree->op2_move_recover(rand_node1_idx);

                    }else if(op_pick == 2){

                    //cout << "recover op3" << endl;

                    tree->op3_swap(rand_node1_idx, rand_node2_idx);



                    }else{
                    cout << "Error, a wrong opration recover." << endl;
                    exit(1);
                    }

                    tree->build_Graph(tree->node_list[tree->root], false);
                    new_cost = tree->computeCost();

                    tree->cost = new_cost;
                    prev_cost = new_cost;

                    //cout << "返回原本cost : " << new_cost << endl;

                }else{

                    //cout<< "有進入邊界" << endl;

                    //cout << "邊界內cost function : " << endl;
                    //cout << "prev_cost: "<< prev_cost << endl;
                    //cout << "cost: "<< new_cost << endl;
                    //cout << delta_cost << endl;


                    prev_inOutline = true;
                    double prob = (double) rand() / RAND_MAX;
                    //cout << "壞的接受機率為 : " << prob << endl;
                     //cout << "邊界機率為 : " <<  exp(-1 * delta_cost/ 10000);

                    if(delta_cost < 0 || prob < exp(-1 * delta_cost/ 10000)){

                        if(delta_cost >= 0){

                            uphill += 1;

                            //cout << exp(-1 * delta_cost/ T) << endl;
                            tree->cost = new_cost;
                            prev_cost = new_cost;

                            //cout << "壞的接受," << " 新的cost : "<< new_cost << endl;

                        }else{

                            //cout << exp(-1 * delta_cost/ T) << endl;
                            tree->cost = new_cost;
                            prev_cost = new_cost;

                            //cout << "小於原本cost," << " 新的cost : "<< new_cost << endl;

                        }


                    }else{

                        reject += 1;

                        //cout << "大於等於原本cost,";

                        if(op_pick == 0){
                        
                        //cout << "recover op1" << endl;

                        tree->op1_rotate(rand_node1_idx);

                        }else if(op_pick == 1){

                        //cout << "recover op2" << endl;

                        tree->op2_move_recover(rand_node1_idx);

                        }else if(op_pick == 2){

                        //cout << "recover op3" << endl;

                        tree->op3_swap(rand_node1_idx, rand_node2_idx);

                        }else{
                        cout << "Error, a wrong opration recover." << endl;
                        exit(1);
                        }

                        tree->build_Graph(tree->node_list[tree->root], false);
                        new_cost = tree->computeCost();

                        tree->cost = new_cost;
                        prev_cost = new_cost;

                        //cout << "返回原本cost : " << new_cost << endl;

                    }
                }

            }else{
                
                if(!is_inOutline){

                    //cout<< "未進入邊界," ;

                    //cout << "邊界外cost function : " << endl;
                    //cout << "prev_nofixed_cost: "<< prev_nofixed_cost << endl;
                    //cout << "new_nofixed_cost: "<< new_nofixed_cost << endl;
                    //cout << delta_nofixed_cost << endl;

                    double prob = (double) rand() / RAND_MAX;
                    //cout << "壞的接受機率為 : " << prob << endl;
                    //cout << "邊界機率為 : " <<  exp(-1 * delta_nofixed_cost/ 100);

                    if(delta_nofixed_cost < 0 || prob < exp(-1 * delta_nofixed_cost/ 100)){

                        if(delta_nofixed_cost >= 0){

                            uphill += 1;
                            //cout << "壞的接受," << " 新的no_fixed_cost : "<< new_nofixed_cost << endl;

                        }else{

                            //cout << "小於no_fixed_cost," << " 新的no_fixed_cost : "<< new_nofixed_cost << endl;
                        }

                        tree->no_fixed_cost = new_nofixed_cost;
                        prev_nofixed_cost = new_nofixed_cost;


                    }else{

                        //cout << "大於等於原本no_fixed_cost,";

                        if(op_pick == 0){
                        
                        //cout << "recover op1" << endl;

                        tree->op1_rotate(rand_node1_idx);

                        }else if(op_pick == 1){

                        //cout << "recover op2" << endl;

                        tree->op2_move_recover(rand_node1_idx);
                        //cout << "recover op2 end" << endl;
                        //if(rand_node1_idx == 0){
                        //    cout << tree->root;
                        //    tree->print_btree();
                        //    break;
                        //}

                        }else if(op_pick == 2){

                        //cout << "recover op3" << endl;

                        tree->op3_swap(rand_node1_idx, rand_node2_idx);



                        }else{
                        cout << "Error, a wrong opration recover." << endl;
                        exit(1);
                        }

                        //cout << "我有進來" << endl;
                        //cout << tree->root << endl;
                        tree->build_Graph(tree->node_list[tree->root], false);
                        new_nofixed_cost = tree->computeNoFixedCost();

                        tree->no_fixed_cost = new_nofixed_cost;

                        //cout << "返回原本new_nofixed_cost : " << new_nofixed_cost << endl;
                        if(prev_nofixed_cost != new_nofixed_cost){
                            //cout << "沒有成功回復" << endl;
                            //cout << tree->node_list[rand_node1_idx]->name << endl;
                            //cout << "換回來後" << endl;
                            //cout << tree->t_width << " " << tree->t_height << endl;
                            //tree->print_btree(); 
                            break;
                        }
                        //tree->print_btree(); 
                        prev_nofixed_cost = new_nofixed_cost;


                        reject += 1;
                    }

                }else{

                    //cout<< "有進入邊界" << endl;


                    //cout << "邊界內cost function : " << endl;
                    //cout << "prev_cost: "<< prev_cost << endl;
                    //cout << "cost: "<< new_cost << endl;
                    //cout << delta_cost << endl;


                    prev_inOutline = true;
                    double prob = (double) rand() / RAND_MAX;
                    //cout << "壞的接受機率為 : " << prob << endl;
                    //cout << "邊界機率為 : " <<  exp(-1 * delta_cost/ 10000);

                    if(delta_cost < 0 || prob < exp(-1 * delta_cost/ 10000)){

                        if(delta_cost >= 0){

                            uphill += 1;

                            //cout << exp(-1 * delta_cost/ T) << endl;
                            tree->cost = new_cost;
                            prev_cost = new_cost;

                           // cout << "壞的接受," << " 新的cost : "<< new_cost << endl;

                        }else{

                            //cout << exp(-1 * delta_cost/ T) << endl;
                            tree->cost = new_cost;
                            prev_cost = new_cost;

                            //cout << "小於原本cost," << " 新的cost : "<< new_cost << endl;

                        }


                    }else{

                        reject += 1;

                        //cout << "大於等於原本cost,";

                        if(op_pick == 0){
                        
                        //cout << "recover op1" << endl;

                        tree->op1_rotate(rand_node1_idx);

                        }else if(op_pick == 1){

                        //cout << "recover op2" << endl;

                        tree->op2_move_recover(rand_node1_idx);

                        }else if(op_pick == 2){

                        //cout << "recover op3" << endl;

                        tree->op3_swap(rand_node1_idx, rand_node2_idx);

                        }else{
                        cout << "Error, a wrong opration recover." << endl;
                        exit(1);
                        }

                        tree->build_Graph(tree->node_list[tree->root], false);
                        new_cost = tree->computeCost();

                        tree->cost = new_cost;
                        prev_cost = new_cost;

                        //cout << "返回原本cost : " << new_cost << endl;

                    }
                }
            }
            //cout << "現在溫度 : " << T << endl;
            //cout << "初始溫度 : " << T0 << endl;
        }while(uphill <= N && MT <= 2 * N);

        T *= r;
        
        runtime = (clock() - SA_initial_time) / CLOCKS_PER_SEC;

    }while(T >= EPSILON && ((float)reject / MT <= 0.98 || prev_inOutline == false)&& runtime < TIME_LIMIT);

    tree->computeCost();
    //cout << "執行時間 :" << runtime << "秒"<< endl;
    //cout << "退火溫度 :" << T << endl;
    /*
    if(T < EPSILON){

        cout << "達到Freeze狀態" << endl;

    }else if((float)reject / MT > 0.95){

        cout << "拒絕次數 : " << reject << endl; 
        cout << "擾動次數 : " << MT << endl; 

        cout << "找到Min值" << endl;

    }else if( runtime >= TIME_LIMIT){

        cout << runtime << endl;
        cout << "超出運算時間" << endl;

    }

    cout << "程式總執行時間" << (clock() - start_time )/ CLOCKS_PER_SEC << endl;*/

    //cout << tree->t_width * tree->t_height << endl;

    // write output to file
    ofstream FILEOUT;
    FILEOUT.open(argv[4], ios::out);
    FILEOUT << tree->cost << endl;
    FILEOUT << tree->total_wireLength << endl;
    FILEOUT << tree->total_area << endl;
    FILEOUT << tree->t_width << " " << tree->t_height << endl;
    FILEOUT << runtime << endl;
    for (vector<Node*>::iterator it = tree->node_list.begin(); it < tree->node_list.end(); it++)
        FILEOUT << (*it)->name << " " << (*it)->x1 << " " << (*it)->y1 << " " << (*it)->x2 << " " << (*it)->y2 << endl;
    FILEOUT.close();
    


}