#include "Utility.h"
#include "Structure.h"
#include "GlobalVariable.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <math.h>
#include <typeinfo>

using namespace std;

int utilrandom (int i) { return std::rand()%i;}

vector< tuple<double, double, double> > GetWeightVector(string filename) {
    ifstream inFile(filename);
    string temp;
    vector< tuple<double, double, double> > weight_vector;
    double area_weight, wirelength_weight, PS_weight;

    getline(inFile, temp, '\n');

    while (!inFile.eof()) {
        getline(inFile, temp, ',');
        area_weight = stod(temp);
        getline(inFile, temp, ',');
        wirelength_weight = stod(temp);
        getline(inFile, temp, '\n');
        PS_weight = stod(temp);
        
        tuple<double, double, double> wv = make_tuple(area_weight, wirelength_weight, PS_weight);
        weight_vector.push_back(wv);
    }
    return weight_vector;

}

int GetTotalAmount(string filename) {
    ifstream inFile(filename);
    string line;
    int rows = 0;
    while (getline(inFile, line)){
        rows++;
    }
    return rows-1;
}

map<string, vector<string> > GetNetlist(string netlist_name) {

    ifstream inFile(netlist_name);
    string temp;
    string net_name;
    map<string, vector<string> > netlist;
    vector<string> tempNet;
    
    while (!inFile.eof())
    {
        getline(inFile, temp);

        if (temp[0] == '(') {
            getline(inFile, temp);
            net_name = temp;
            getline(inFile, temp);

            while (temp[0] != ')') {

                // Exclude pin
                // dash = temp.find("-");
                // temp = temp.substr(0, dash);
                // End of Exclude pin

                tempNet.push_back(temp);

                getline(inFile, temp);
            }

            netlist[net_name] = tempNet;
            tempNet.clear();

        }
    }

    // for (auto net = netlist.rbegin(); net != netlist.rend(); net++) {
    //     cout << (*net).first << endl;
    //     for (int i = 0; i < (*net).second.size(); i++) {
    //         cout << (*net).second.at(i) << endl;
    //     }
    //     cout << endl;
    // }

    return netlist;
}

vector<Component> ReadDim(string filename) {
    ifstream inFile(filename);
    string temp;
    Component c_temp;
    vector<Component> data;

    // if (inFile.is_open()) {cout << "File has been opened." << endl;}
    // else {cout << "NO FILE IS OPENED." << endl;}

    getline(inFile, temp, '\n');
    while (!inFile.eof())
    {
        getline ( inFile, c_temp.name, ',' );
        getline ( inFile, c_temp.color, ',' );
        getline ( inFile, temp, ',' );
        c_temp.width = stod(temp);
        getline ( inFile, temp, ',' );
        c_temp.length = stod(temp);
        getline ( inFile, temp, ',' );
        c_temp.height = stod(temp);
        getline ( inFile, temp, '\n' );
        c_temp.voltage = stoi(temp);

        data.push_back(c_temp);
    }
    return data;
}

double GetTotalArea(string filename) {
    ifstream inFile(filename);
    string temp;
    double width;
    double length;
    double total_area = 0;

    for (int i = 0; i < 1; i++) {
        getline(inFile, temp, '\n');
    }
    while (!inFile.eof())
    {
        getline ( inFile, temp, ',' );
        getline ( inFile, temp, ',' );
        getline ( inFile, temp, ',' );
        width = stod(temp);
        getline ( inFile, temp, ',' );
        length = stod(temp);
        getline ( inFile, temp, '\n' );

        total_area += width * length;
    }

    return total_area;
}

map< int, tuple<double, double> > ReadPin(string name) {
    ifstream inFile("./" + name + ".csv");
    map< int, tuple<double, double> > pin_location;
    string temp1, temp2;
    double x, y;
    int num = 0;

    getline(inFile, temp1, '\n');
    while (!inFile.eof()) {
        num++;
        getline(inFile, temp1, ',' );
        x = stod(temp1);
        getline(inFile, temp2, '\n' );
        y = stod(temp2);
        pin_location[num] = make_tuple(x, y);
    }
    return pin_location;
}

Node* LeftMost(Node* node) {
    if (node->left_child_ == nullptr)
    {
        // cout << node->component_.name << " left most" << endl;
        return node;
    }
    else 
    {
        // cout << node->component_.name << " find left most" << endl;
        node = LeftMost(node->left_child_);
        return node;
    }
}

Node* RightMost(Node* node) {
    if (node->right_child_ == nullptr)
    {
        // cout << node->component_.name << " right most" << endl;
        return node;
    }
    else
    {
        // cout << node->component_.name << " find right most" << endl;
        node = RightMost(node->right_child_);
        return node;
    } 
}

BStarTree* CopyTree(BStarTree* tree) {
    BStarTree* new_tree = new BStarTree;
    stack<Node*> old_tree_stack;
    stack<Node*> new_tree_stack;
    old_tree_stack.push(tree->root);
    new_tree_stack.push(new_tree->root);
    new_tree->area_ = tree->area_;
    new_tree->front_area_ = tree->front_area_;
    new_tree->back_area_ = tree->back_area_;
    new_tree->wire_length_ = tree->wire_length_;
    new_tree->voltage_ = tree->voltage_;
    new_tree->fitness_ = tree->fitness_;
    
    while (old_tree_stack.empty() == false)
    {
        Node* old_temp = old_tree_stack.top();
        Node* new_temp = new_tree_stack.top();

        new_temp->component_ = old_temp->component_;
        new_temp->angle_ = old_temp->angle_;
        old_tree_stack.pop();
        new_tree_stack.pop();

        if (old_temp->right_child_)
        {
            new_temp->right_child_ = new Node;
            new_temp->right_child_->parent_ = new_temp;
            old_tree_stack.push(old_temp->right_child_);
            new_tree_stack.push(new_temp->right_child_);
        }
            
        if (old_temp->left_child_)
        {
            new_temp->left_child_ = new Node;
            new_temp->left_child_->parent_ = new_temp;
            old_tree_stack.push(old_temp->left_child_);
            new_tree_stack.push(new_temp->left_child_);
        }
    }
    return new_tree;
}

vector<Node*> ExtractTree(Node* root) {
    vector<Node*> node_permu;
    stack<Node*> node_stack;
    node_stack.push(root);

    // Pre-order iterate
    while (node_stack.empty() == false)
    {
        Node* temp = node_stack.top();
        // cout << temp->component_.name << ' ';
        node_permu.push_back(node_stack.top());
        node_stack.pop();

        if (temp->right_child_)
            node_stack.push(temp->right_child_);
        if (temp->left_child_)
            node_stack.push(temp->left_child_);
    }
    // cout <<endl;

    return node_permu;
}

double CalcuCentralization(vector< tuple<double, double> > primary_part) {
    double centralization = 0.0;
    double X = 0;
    double Y = 0;

    for (int i = 0; i < primary_part.size(); i++) {
        // cout << "comp coor (" << get<0>(primary_part.at(i)) << ", " << get<1>(primary_part.at(i)) << ")" << endl;
        X += get<0>(primary_part.at(i));
        Y += get<1>(primary_part.at(i));
    }
    X /= primary_part.size();
    Y /= primary_part.size();
    // cout << "(X, Y) = " << "(" << X << ", " << Y << ")" << endl;

    for (int i = 0; i < primary_part.size(); i++) {
        centralization += sqrt(pow(X - get<0>(primary_part.at(i)), 2) + pow(Y - get<1>(primary_part.at(i)), 2));
    }
    // cout << "net length: " << centralization << endl;

    return centralization;
}

double CalcuHPWL(vector< tuple<double, double> > comp_in_net) {
    double bl_x, bl_y, tr_x, tr_y, WL;
    bl_x = get<0>(comp_in_net.at(0));
    tr_x = bl_x;
    bl_y = get<1>(comp_in_net.at(0));
    tr_y = bl_y;

    for (int i = 1; i < comp_in_net.size(); i++) {
        if (bl_x > get<0>(comp_in_net.at(i)))
            bl_x = get<0>(comp_in_net.at(i));
        if (bl_y > get<1>(comp_in_net.at(i)))
            bl_y = get<1>(comp_in_net.at(i));
        if (tr_x < get<0>(comp_in_net.at(i)))
            tr_x = get<0>(comp_in_net.at(i));
        if (tr_y < get<1>(comp_in_net.at(i)))
            tr_y = get<1>(comp_in_net.at(i));
    }

    WL = (tr_x - bl_x) + (tr_y - bl_y);
    // cout << "WL = " << WL << endl;

    return WL;
}

double Overlap(Line line1, Line line2) {
    return max(0.0, min(line1.end.x, line2.end.x) - max(line1.begin.x, line2.begin.x));
}

bool NotSameLine(Line line1, Line line2) {
    if (line1.begin.x == line2.begin.x && line1.begin.y == line2.begin.y && line1.end.x == line2.end.x && line1.end.y == line2.end.y) {
        return false;
    } else {
        return true;
    }
}

char* hex_color_code() {
    char* hex = new char[6];
    char hex_char[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    int i;
    for (i = 0; i < 6; i++) {
        hex[i] = hex_char[rand()%16];
    }
    return hex;
}

void writeJS(vector<Node*> node_vector, vector<Line> horizon_contour, double area, int side) {
    string str_side;

    if (side == 2) {
        str_side = "front";
        fstream file("webGL/kSingleDoubleSided.js", ios::out);
        file << "const kSingleDoubleSided = " << 0 << ";";
        file.close();
    }
    else {
        if (side == 0) {
            str_side = "front";
        }
        else {
            str_side = "back";
        }
        fstream file("webGL/kSingleDoubleSided.js", ios::out);
        file << "const kSingleDoubleSided = " << 1 << ";";
        file.close();
    }

    fstream file("webGL/" + str_side + "_data.js", ios::out);
    file << str_side + "_data = ";
    file << "{" << "\n";
    for (int i = 0; i < node_vector.size(); i++) {
        // char* hex = hex_color_code();

        file << "    ";
        file <<"\"" << node_vector[i]->component_.name << "\"" << ": ";
        file << "{" << "\n";
        file << "        ";
        file << "\"size\"" << ": " << "[" <<  node_vector[i]->length_ - 2 * node_vector[i]->margin_ << ", " << node_vector[i]->width_ - 2 * node_vector[i]->margin_ << ", " << node_vector[i]->height_ << "]" << ",\n";
        file << "        ";
        file << "\"position\"" << ": " << "[" <<  node_vector[i]->coor_X_ + node_vector[i]->margin_ << ", " << node_vector[i]->coor_Y_ + node_vector[i]->margin_ << ", " << node_vector[i]->coor_Z_ << "]" << ",\n";
        file << "        ";
        file << "\"angle\"" << ": " << "\"" << node_vector[i]->angle_ << "\"" << ",\n";
        file << "        ";
        file << "\"color\"" << ": " << "\"" << "0x" << node_vector[i]->component_.color << "\"" << ",\n";
        if (node_vector[i]->left_child_) {
            file << "        ";
            file << "\"leftChild\"" << ": " << "\"" <<  node_vector[i]->left_child_->component_.name << "\"" << ",\n";
        } else {
            file << "        ";
            file << "\"leftChild\"" << ": " << "null" << ",\n";
        }

        if (node_vector[i]->right_child_) {
            file << "        ";
            file << "\"rightChild\"" << ": " << "\"" <<  node_vector[i]->right_child_->component_.name << "\"" << ",\n";
        } else {
            file << "        ";
            file << "\"rightChild\"" << ": " << "null" << ",\n";
        }

        file << "    ";
        file << "}" << "\n";
        if (i != node_vector.size()-1) {
            file << "    ";
            file << "," << "\n";
        }
    }
    file << "}";
    file << ";" << "\n";

    file << "\n";

    file << str_side + "_horizon_contour = ";
    file << "{" << "\n";
    for (int i = 0; i < horizon_contour.size(); i++) {
        file << "    ";
        file <<"\"" << i << "\"" << ": ";
        file << "{" << "\n";
        file << "        ";
        file << "\"begin\"" << ": " << "[" <<  horizon_contour[i].begin.x << ", " <<  horizon_contour[i].begin.y << "]" << ",\n";
        file << "        ";
        file << "\"end\"" << ": " << "[" <<  horizon_contour[i].end.x << ", " <<  horizon_contour[i].end.y << "]" << ",\n";
        file << "        ";
        file << "\"color\"" << ": " << "\"" << "red" << "\"" << ",\n";
        file << "    ";
        file << "}" << "\n";
        if (i != horizon_contour.size()-1) {
            file << "    ";
            file << "," << "\n";
        }
    }
    file << "}";
    file << ";" << "\n";

    file << "\n";

    file << str_side + "_area = " << area;
    file << ";" << "\n";

    file << "\n";

    file << str_side + "_root = " << "\"" << node_vector[0]->component_.name << "\"";
    file << ";" << "\n";


    file.close();
}

bool SortPopArea(BStarTree const *tree_1, BStarTree const *tree_2){
    return tree_1->area_ < tree_2->area_;
}

bool SortPopWL(BStarTree const *tree_1, BStarTree const *tree_2){
    return tree_1->wire_length_ < tree_2->wire_length_;
}

bool SortPop(BStarTree const *tree_1, BStarTree const *tree_2){
    return tree_1->fitness_ < tree_2->fitness_;
}

bool ContourCompare(Line a, Line b) {
    return a.begin.x < b.begin.x;
}

bool PreplacedCheck(Node* node, Line prePlaced) {
    Vertex l1 = {node->coor_X_, node->coor_Y_};
    Vertex r1 = {node->coor_X_ + node->length_, node->coor_Y_ + node->width_};
    Vertex l2 = prePlaced.begin;
    Vertex r2 = prePlaced.end;
    if (l2.y == r2.y)
        return false;
    if (l1.x >= r2.x || l2.x >= r1.x)
        return false;
    if (l1.y >= r2.y || l2.y >= r1.y)
        return false;
    return true;
}

vector<int> UniformParentSelection() {
    int parent_1 = 0, parent_2 = 0;
    while (parent_1 == parent_2){
        parent_1 = rand() % kPopSize;
        parent_2 = rand() % kPopSize;
    }

    vector<int> Parent;
    Parent.push_back(parent_1);
    Parent.push_back(parent_2);

    return Parent;
}

vector<int> kTournamentSelection() {
    vector<int> Parent;
    Parent.reserve(2);
    vector<int> index_arr;
    index_arr.reserve(k_kTournament);
    bool check;

    while (Parent.size() < 2)
    {
        do{
            index_arr.clear();
            check = 0;
            for (int i = 0; i < k_kTournament; i++){
                index_arr.push_back(rand() % kPopSize);
            }
            sort(index_arr.begin(), index_arr.end());
            for (int i = 0; i < k_kTournament-1; i++){
                if (index_arr.at(i) == index_arr.at(i+1)){
                    check = 1;
                }
            }
        } while (check == 1);

        Parent.push_back(index_arr.front());
        index_arr.clear();
        
        if (Parent.size() == 2)
            if (Parent.front() == Parent.back())
                Parent.pop_back();
    }
    return Parent;
}

vector<BStarTree*> Crossover(vector<BStarTree*> population, const int kComponentNum) {
    vector<BStarTree*> offspring;
    // vector<int> parent_index = UniformParentSelection();
    vector<int> parent_index = kTournamentSelection();

    // cout << "parent 1 " << parent_index.at(0) << endl;
    // cout << "parent 2 " << parent_index.at(1) << endl;
    double pci = (double) rand() / (RAND_MAX + 1.0);
    int crossover_type = 0;

    /*** Success Rate Exp ***/
    double parent_area;
    if (population.at(parent_index.at(0))->area_ > population.at(parent_index.at(1))->area_)
        parent_area = population.at(parent_index.at(0))->area_;
    else 
        parent_area = population.at(parent_index.at(1))->area_;
    /******************************/
    
    if (pci < kProbCrossover){
        // kCrossover_Count += 2;

        if (kSingleDoubleSided == 0) 
            crossover_type = rand() % 3;
        else if (kSingleDoubleSided == 1) 
            crossover_type = 1;
        
        if (crossover_type == 0){
            // cout << "Left Subtree Crossover" << endl;
            // kLeftSubtreeCrossover_Count += 2;
            offspring.push_back(population.at(parent_index.at(0))->leftSubtreeCrossover(population.at(parent_index.at(1))));
            // offspring.back()->Xtype_ = crossover_type;
            // offspring.back()->evaluateTotalArea();
            // if (offspring.back()->area_ < parent_area)
            //     kLeftSubtreeCrossover_SuccessCount++;
                // kLeftSubtreeCrossover_SuccessCount += (parent_area - offspring.back()->area_);

            offspring.push_back(population.at(parent_index.at(1))->leftSubtreeCrossover(population.at(parent_index.at(0))));
            // offspring.back()->Xtype_ = crossover_type;
            // offspring.back()->evaluateTotalArea();
            // if (offspring.back()->area_ < parent_area)
            //     kLeftSubtreeCrossover_SuccessCount++;
                // kLeftSubtreeCrossover_SuccessCount += (parent_area - offspring.back()->area_);
        }
        else if (crossover_type == 1) {
            // cout << "Ordered Crossover" << endl;
            // kOrderedCrossover_Count += 2;
            offspring.push_back(population.at(parent_index.at(0))->orderedCrossover(population.at(parent_index.at(1)), kComponentNum));
            // offspring.back()->Xtype_ = crossover_type;
            // offspring.back()->evaluateTotalArea();
            // if (offspring.back()->area_ < parent_area)
            //     kOrderedCrossover_SuccessCount++;
                // kOrderedCrossover_SuccessCount += (parent_area - offspring.back()->area_);
            
            offspring.push_back(population.at(parent_index.at(1))->orderedCrossover(population.at(parent_index.at(0)), kComponentNum));
            // offspring.back()->Xtype_ = crossover_type;
            // offspring.back()->evaluateTotalArea();
            // if (offspring.back()->area_ < parent_area)
            //     kOrderedCrossover_SuccessCount++;
                // kOrderedCrossover_SuccessCount += (parent_area - offspring.back()->area_);
        }
        else if (crossover_type == 2) {
            // cout << "Subtree Exchange Crossover" << endl;
            // kSubtreeExchangeCrossover_Count += 2;
            offspring.push_back(population.at(parent_index.at(0))->subtreeExchangeCrossover(population.at(parent_index.at(1)), kComponentNum));
            // offspring.back()->Xtype_ = crossover_type;
            // offspring.back()->evaluateTotalArea();
            // if (offspring.back()->area_ < parent_area)
            //     kSubtreeExchangeCrossover_SuccessCount++;
                // kSubtreeExchangeCrossover_SuccessCount += (parent_area - offspring.back()->area_);
            
            offspring.push_back(population.at(parent_index.at(1))->subtreeExchangeCrossover(population.at(parent_index.at(0)), kComponentNum));
            // offspring.back()->Xtype_ = crossover_type;
            // offspring.back()->evaluateTotalArea();
            // if (offspring.back()->area_ < parent_area)
            //     kSubtreeExchangeCrossover_SuccessCount++;
                // kSubtreeExchangeCrossover_SuccessCount += (parent_area - offspring.back()->area_);
            
        }
    }
    else {
        offspring.push_back(CopyTree(population.at(parent_index.at(0))));
        offspring.back()->Xtype_ = 99;
        offspring.push_back(CopyTree(population.at(parent_index.at(1))));
        offspring.back()->Xtype_ = 99;
    }
    return offspring;
}

vector<BStarTree*> Mutation(vector<BStarTree*> offspring, const int kComponentNum) {
    vector<BStarTree*> new_offspring;
    // cout << "offspring num before mutation: " << offspring.size() << endl;

    // Random mutation type
    for (int child = 0; child < offspring.size(); child++){
        double pmi = (double) rand() / (RAND_MAX + 1.0);
        if (pmi < kProbMutation){
            // kMutation_Count++;
            int mutation_type = rand() % 7;
            // cout << "mutation_type: " << mutation_type << endl;
            switch (mutation_type)
            {
            case 0:
                // cout << "swap node mutation" << endl;
                // kSwapNode_Count++;

                new_offspring.push_back(offspring.at(child)->swapNodeMutation());
                // new_offspring.back()->Mtype_ = mutation_type;
                // new_offspring.back()->evaluateTotalArea();
                // kEvaluation_count++;

                // if (new_offspring.back()->area_ < offspring.at(child)->area_)
                //     kSwapNode_SuccessCount++;
                    // kSwapNode_SuccessCount += (offspring.at(child)->area_ - new_offspring.back()->area_);
                
                break;

            case 1:
                // cout << "insert mutation" << endl;
                // kInsert_Count++;

                new_offspring.push_back(offspring.at(child)->insertMutation());
                // new_offspring.back()->Mtype_ = mutation_type;
                // new_offspring.back()->evaluateTotalArea();
                // kEvaluation_count++;

                // if (new_offspring.back()->area_ < offspring.at(child)->area_)
                //     kInsert_SuccessCount++;
                    // kInsert_SuccessCount += (offspring.at(child)->area_ - new_offspring.back()->area_);
                
                break;

            case 2:
                // cout << "Bitwise mutation" << endl;
                // kBitwise_Count++;

                new_offspring.push_back(offspring.at(child)->bitwiseMutation());
                // new_offspring.back()->Mtype_ = mutation_type;
                // new_offspring.back()->evaluateTotalArea();
                // kEvaluation_count++;

                // if (new_offspring.back()->area_ < offspring.at(child)->area_)
                //     kBitwise_SuccessCount++;
                    // kBitwise_SuccessCount += (offspring.at(child)->area_ - new_offspring.back()->area_);
                
                break;

            case 3:
                // cout << "scramble mutation" << endl;
                // kScramble_Count++;

                new_offspring.push_back(offspring.at(child)->scrambleMutation());
                // new_offspring.back()->Mtype_ = mutation_type;
                // new_offspring.back()->evaluateTotalArea();
                // kEvaluation_count++;

                // if (new_offspring.back()->area_ < offspring.at(child)->area_)
                //     kScramble_SuccessCount++;
                    // kScramble_SuccessCount += (offspring.at(child)->area_ - new_offspring.back()->area_);
                
                break;
            case 4:
                // cout << "swap branch mutation" << endl;
                // kSwapBranch_Count++;

                new_offspring.push_back(offspring.at(child)->swapBranchMutation());
                // new_offspring.back()->Mtype_ = mutation_type;
                // new_offspring.back()->evaluateTotalArea();
                // kEvaluation_count++;

                // if (new_offspring.back()->area_ < offspring.at(child)->area_)
                //     kSwapBranch_SuccessCount++;
                    // kSwapBranch_SuccessCount += (offspring.at(child)->area_ - new_offspring.back()->area_);
                
                break;

            case 5:
                // cout << "shift subtree mutation" << endl;
                // kShiftSubtree_Count++;

                new_offspring.push_back(offspring.at(child)->shiftSubtreeMutation());
                // new_offspring.back()->Mtype_ = mutation_type;
                // new_offspring.back()->evaluateTotalArea();
                // kEvaluation_count++;

                // if (new_offspring.back()->area_ < offspring.at(child)->area_)
                //     kShiftSubtree_SuccessCount++;
                    // kShiftSubtree_SuccessCount += (offspring.at(child)->area_ - new_offspring.back()->area_);
                
                break;

            case 6:
                // cout << "swap Subtree Mutation_SameLevel" << endl;
                // kSwapSubtree_Count++;

                int randlevel = rand() % 2;
                if (randlevel)
                    new_offspring.push_back(offspring.at(child)->swapSubtreeMutation_sameLevel());
                else
                    new_offspring.push_back(offspring.at(child)->swapSubtreeMutation_diffLevel());

                // new_offspring.push_back(offspring.at(child)->swapSubtreeMutation_sameLevel());
                // new_offspring.back()->Mtype_ = mutation_type;
                // new_offspring.back()->evaluateTotalArea();
                // kEvaluation_count++;

                // if (new_offspring.back()->area_ < offspring.at(child)->area_)
                //     kSwapSubtree_SuccessCount++;
                    // kSwapSubtree_SuccessCount += (offspring.at(child)->area_ - new_offspring.back()->area_);
                
                break;

            // case 7:
            //     // cout << "swap Subtree Mutation_DifferentLevel" << endl;
            //     kSwapSubtree_DiffLevel_Count++;

            //     new_offspring.push_back(offspring.at(child)->swapSubtreeMutation_diffLevel());
            //     new_offspring.back()->Mtype_ = mutation_type;
            //     new_offspring.back()->evaluateTotalArea();
            //     // kEvaluation_count++;

            //     if (new_offspring.back()->area_ < offspring.at(child)->area_)
            //         kSwapSubtree_DiffLevel_SuccessCount++;
            //         // kSwapSubtree_DiffLevel_SuccessCount += (offspring.at(child)->area_ - new_offspring.back()->area_);
                
            //     break;
            
            // case 8:
            //     // cout << "Shuffle Subtree Mutation_SameLevel" << endl;
            //     kShuffleSubtree_SameLevel_Count++;

            //     new_offspring.push_back(offspring.at(child)->shuffleSubtreeMutation_sameLevel());
            //     new_offspring.back()->Mtype_ = mutation_type;
            //     new_offspring.back()->evaluateTotalArea();
            //     // kEvaluation_count++;

            //     if (new_offspring.back()->area_ < offspring.at(child)->area_)
            //         kShuffleSubtree_SameLevel_SuccessCount++;
            //         // kShuffleSubtree_SameLevel_SuccessCount += (offspring.at(child)->area_ - new_offspring.back()->area_);
                
            //     break;
            }
        }
        else {
            new_offspring.push_back(offspring.at(child));
            new_offspring.back()->Mtype_ = 99;
        }
    }

    // Bitwise mutation
    // for (int child = 0; child < offspring.size(); child++){
    //     double pami = (double) rand() / (RAND_MAX + 1.0);
    //     if (pami < kProbAngleMutation){
    //         // cout << "Bitwise mutation" << endl;
    //         new_offspring.push_back(offspring.at(child)->bitwiseMutation());
    //         // new_offspring.back()->countNode();
    //         offspring.erase(offspring.begin()+child);
    //     }
    // }

    return new_offspring;
}

BStarTree* ModifyTree(BStarTree* tree, vector<Node*> node_permu) {
    BStarTree* new_tree = CopyTree(tree);
    stack<Node*> node_stack;
    node_stack.push(new_tree->root);
    int ptr = 0;
    while (node_stack.empty() == false)
    {
        Node* temp = node_stack.top();
        if (temp->component_.name != node_permu.at(ptr)->component_.name || temp->angle_ != node_permu.at(ptr)->angle_) {
            temp->component_ = node_permu.at(ptr)->component_;
            temp->angle_ = node_permu.at(ptr)->angle_;
        }
        node_stack.pop();
        if (temp->right_child_)
            node_stack.push(temp->right_child_);
        if (temp->left_child_)
            node_stack.push(temp->left_child_);
        ptr++;
    }
    return new_tree;
}

BStarTree* ModifyDoubleSidedTree(BStarTree* tree, vector<Node*> node_permu_front, vector<Node*> node_permu_back) {
    BStarTree* new_tree = CopyTree(tree);
    stack<Node*> node_stack;

    // Modify front tree
    node_stack.push(new_tree->root->left_child_);
    int ptr = 0;
    while (node_stack.empty() == false)
    {
        Node* temp = node_stack.top();
        if (temp->component_.name != node_permu_front.at(ptr)->component_.name || temp->angle_ != node_permu_front.at(ptr)->angle_)
        {
            temp->component_ = node_permu_front.at(ptr)->component_;
            temp->angle_ = node_permu_front.at(ptr)->angle_;
        }
        node_stack.pop();
        if (temp->right_child_)
            node_stack.push(temp->right_child_);
        if (temp->left_child_)
            node_stack.push(temp->left_child_);
        ptr++;
    }

    // Modify back tree
    node_stack.push(new_tree->root->right_child_);
    ptr = 0;
    while (node_stack.empty() == false)
    {
        Node* temp = node_stack.top();
        if (temp->component_.name != node_permu_back.at(ptr)->component_.name || temp->angle_ != node_permu_back.at(ptr)->angle_)
        {
            temp->component_ = node_permu_back.at(ptr)->component_;
            temp->angle_ = node_permu_back.at(ptr)->angle_;
        }
        node_stack.pop();
        if (temp->right_child_)
            node_stack.push(temp->right_child_);
        if (temp->left_child_)
            node_stack.push(temp->left_child_);
        ptr++;
    }

    return new_tree;
}

int CountLevel(Node* node, Node* root) {
    int level = 0;
    Node *temp = node;
    while (temp->component_.name != root->component_.name) {
        level++;
        temp = temp->parent_;
    }
    return level;
}

bool CheckAncestor(Node* node1, Node* node2, Node* root){
    bool check = 1;
    Node *temp = node1;
    while (temp->parent_->component_.name != root->component_.name) {
        if (temp->component_.name == node2->component_.name) check = 0;
        //1 not ancestor ;0 is ancestor
        temp = temp->parent_;
    }
    temp = node2;
    while (temp->parent_->component_.name != root->component_.name) {
        if (temp->component_.name == node1->component_.name) check = 0;
        //1 not ancestor ;0 is ancestor
        temp = temp->parent_;
    }
    // if(check) cout << "temp is ancestor of select node" << endl;
    // else cout << "select node is available" << endl; 
    return check;
}

vector<Node*> FindNBNode(Node* root) {    
    vector<Node*> node_permu;
    stack<Node*> node_stack;
    node_stack.push(root);

    // Pre-order iterate
    while (node_stack.empty() == false)
    {
        Node* temp = node_stack.top();
        if (temp->right_child_ == nullptr || temp->left_child_ == nullptr)
            node_permu.push_back(temp);
        node_stack.pop();

        if (temp->right_child_)
            node_stack.push(temp->right_child_);
        if (temp->left_child_)
            node_stack.push(temp->left_child_);
    }

    return node_permu;
}

vector<Node*> Find2BNode(Node* root) {
    vector<Node*> node_permu;
    stack<Node*> node_stack;
    node_stack.push(root);

    // Pre-order iterate
    while (node_stack.empty() == false)
    {
        Node* temp = node_stack.top();
        if (temp->right_child_ != nullptr && temp->left_child_ != nullptr)
            node_permu.push_back(temp);
        node_stack.pop();
        
        if (temp->right_child_)
            node_stack.push(temp->right_child_);
        if (temp->left_child_)
            node_stack.push(temp->left_child_);
    }
    
    return node_permu;
}

vector<Node*> FindDLNode(Node* root, Node* node) {
    vector<Node*> node_permu;
    stack<Node*> node_stack; 
    node_stack.push(root);

    // Pre-order iterate
    int level = CountLevel(node,root);
    while (node_stack.empty() == false) {   
        Node* temp = node_stack.top();
        // if not same level && not root && not ancestor
        if (level != CountLevel(temp,root) && CountLevel(temp,root) >= 2 && CheckAncestor(temp,node,root)) 
            node_permu.push_back(temp);
        node_stack.pop();
        if (temp->right_child_)
            node_stack.push(temp->right_child_);
        if (temp->left_child_)
            node_stack.push(temp->left_child_);        
    }

    return node_permu;
}

vector<Node*> FindLevel4Node(Node* root) {
    vector<Node*> node_permu;
    stack<Node*> node_stack; 
    node_stack.push(root);

    // Pre-order iterate
    while (node_stack.empty() == false) {   
        Node* temp = node_stack.top();
        // if not same level && not root && not ancestor
        if (CountLevel(temp,root) >= 4) //(kSingleDoubleSided)?2:1
            node_permu.push_back(temp);
        node_stack.pop();
        if (temp->right_child_)
            node_stack.push(temp->right_child_);
        if (temp->left_child_)
            node_stack.push(temp->left_child_);        
    }

    return node_permu;
}

vector<Node*> FindLevelNode(Node* root, int level, string name) {
    vector<Node*> node_permu;
    stack<Node*> node_stack;
    node_stack.push(root);

    // Pre-order iterate
    while (node_stack.empty() == false)
    {
        Node* temp = node_stack.top();
        if (level == CountLevel(temp,root) && temp->component_.name != name) 
            node_permu.push_back(temp);
        node_stack.pop();

        if (temp->right_child_)
            node_stack.push(temp->right_child_);
        if (temp->left_child_)
            node_stack.push(temp->left_child_);
    }

    return node_permu;
}

vector<string> GetLeftSubtree(BStarTree* tree) {
    BStarTree* new_tree = CopyTree(tree);
    Node *current = new_tree->root;
    vector<string> left_component;

    left_component.push_back(current->component_.name);

    if (current->left_child_ == nullptr)
    {
        return left_component;
    }
    else
    {
        stack<Node*> node_stack;
        current = current->left_child_;
        node_stack.push(current);

        while (node_stack.empty() == false)
        {
            Node* temp = node_stack.top();
            left_component.push_back(temp->component_.name);
            node_stack.pop();

            if (temp->right_child_)
                node_stack.push(temp->right_child_);
            if (temp->left_child_)
                node_stack.push(temp->left_child_);
        }
        return left_component;
    }
}

vector<int> FindNeighbors(int individual, vector< tuple<double, double, double> > weight_vector, int T) {
    vector<int> weight_vector_neighbors;
    weight_vector_neighbors.reserve(T);
    weight_vector_neighbors.clear();
    // vector<double> distance;
    map<double, int> distance;      // distance, i

    for (int i = 0; i < weight_vector.size(); i++) {
        if (i == individual) {
            distance[9999] = i;
        }
        else {
            double x_2 = pow(get<0>(weight_vector.at(individual)) - get<0>(weight_vector.at(i)), 2.0);
            double y_2 = pow(get<1>(weight_vector.at(individual)) - get<1>(weight_vector.at(i)), 2.0);
            double z_2 = pow(get<2>(weight_vector.at(individual)) - get<2>(weight_vector.at(i)), 2.0);
            distance[sqrt(x_2 + y_2 + z_2)] = i;
        }
    }
    
    for (map<double, int>::iterator iter = distance.begin(); iter != distance.end(); iter++) {
        while (weight_vector_neighbors.size() < T) {
            // cout << "dist: " << iter->first << " index: " << iter->second << endl;
            weight_vector_neighbors.push_back(iter->second);
            break;
        }
    }

    /* Check neighbors */
    // for (int i = 0; i < weight_vector_neighbors.size(); i++) {
    //     cout << weight_vector_neighbors.at(i) << " ";
    // }
    // cout << endl;

    return weight_vector_neighbors;
}

vector<BStarTree*> UpdateEP(vector<BStarTree*> external_population, vector<BStarTree*> offspring) {
    /* If external population is empty. */
    if (external_population.size() == 0) {
        external_population.push_back(offspring.at(0));
    }
    else {
        /* Remove from EP all the vectors dominated by F(y'). */
        for (int i = 0; i < external_population.size(); i++) {
            if (external_population.at(i)->area_ > offspring.front()->area_ && external_population.at(i)->wire_length_ > offspring.front()->wire_length_ && external_population.at(i)->voltage_ > offspring.front()->voltage_) {
                external_population.erase(external_population.begin()+i);
            }
        }
        /* Push y' to EP if no vectors in EP dominates F(y'). */
        int domin = 0;
        // cout << "offspring: " << offspring.at(0)->area_ << ", " << offspring.at(0)->wire_length_ << ", " << offspring.at(0)->voltage_ << endl;
        for (int i = 0; i < external_population.size(); i++) {
            // cout << "EP: " << external_population.at(i)->area_ << ", " << external_population.at(i)->wire_length_ << ", " << external_population.at(i)->voltage_ << endl;
            if (external_population.at(i)->area_ < offspring.front()->area_ || external_population.at(i)->wire_length_ < offspring.front()->wire_length_ || external_population.at(i)->voltage_ < offspring.front()->voltage_) {
                domin = 0;
            }
            else {
                domin = 1;
                break;
            }
        }
        if (domin == 0) {
            external_population.push_back(offspring.front());
        }
    }
    return external_population;
}

// Functions of SA

double CoolingSchedule(int iteration, double init_temperature, double alpha) {
    return init_temperature * pow(alpha, iteration);
}

double AcceptProb(double temperature, double cur_energy, double nbr_energy) {
    double prob = exp((cur_energy-nbr_energy)/temperature);
    return min(1.0, prob);
}

BStarTree *GetNeighbor(BStarTree *current_tree) {
    BStarTree *dup_tree = CopyTree(current_tree);

    int operation = rand() % 3;

    if(operation == 0)
        return dup_tree -> swapNodeMutation();
    else if(operation == 1)
        return dup_tree -> insertMutation();
    else
        return dup_tree -> bitwiseOperation();
}

void GenerateChild(Node *current, vector<Component> component_list, vector<string> name, vector<int> angle, vector<string> left_child, vector<string> right_child){
    
    cout << current->component_.name <<endl;
    int left_child_index = 0;
    int right_child_index = 0;
    string left_child_name = "none";
    string right_child_name = "none";
    for(int i = 0; i < name.size(); i++){
        if(current->component_.name == name.at(i)){
            left_child_name = left_child.at(i);
            right_child_name = right_child.at(i);
            //cout << current->component_.name << endl;
            current->angle_ = angle.at(i);
            break;
        }
    }
    
    if(left_child_name != "none"){
        for(int i = 0; i < component_list.size(); i++){
            if(left_child_name == component_list.at(i).name){
                left_child_index = i;
                break;
            }
        }
        Node *left_insert_node = new Node(component_list.at(left_child_index));
        current->left_child_ = left_insert_node;
        left_insert_node->parent_ = current;
        GenerateChild(left_insert_node, component_list, name, angle, left_child, right_child);
    }
    if(right_child_name != "none"){
        for(int i = 0; i < component_list.size(); i++){
            if(right_child_name == component_list.at(i).name){
                right_child_index = i;
                break;
            }
        }
        Node *right_insert_node = new Node(component_list.at(right_child_index));
        current->right_child_ = right_insert_node;
        right_insert_node->parent_ = current;
        GenerateChild(right_insert_node, component_list, name, angle, left_child, right_child);
    }
}

// Functions of fast SA

double FastSACoolingSchedule(int iteration, double init_probability, double c, double k, double avg_uphillcost, double avg_cost){
    double temperature;
    temperature = - avg_uphillcost / log(init_probability);

    if(iteration == 0){
        return temperature;
    }else if(iteration <= k){
        cout << temperature * avg_cost / (iteration * c) << endl;
        return temperature * avg_cost / (iteration * c);
    }else{
        return temperature * avg_cost / iteration;
    }
}

double CalculateMean(vector<double> input_list){
    double sum = accumulate(input_list.begin(), input_list.end(), 0.0);
    double mean = sum / input_list.size();
    return mean;
}

double NormalizeCostChange(vector<double> input_list){
    
    if(input_list.size() > 1){
        double max = *max_element(input_list.begin(), input_list.end());
        double min = *min_element(input_list.begin(), input_list.end());
        double normalized = (input_list.back() - min) / (max - min);

        return normalized;
    }
    else
        return input_list.back();
}