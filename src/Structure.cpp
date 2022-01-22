#include "Structure.h"
#include "Utility.h"
#include "GlobalVariable.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <memory>
#include <stack>
using namespace std;

int MyRandom (int i) { return std::rand()%i;}

void Node::printContent() {
    cout << "Name: " << component_.name << endl;
    cout << "Width: " << component_.width << endl;
    cout << "Length: " << component_.length << endl;
    cout << "Height: " << component_.height << endl;
    cout << "Primary/Second: " << component_.voltage << endl;
}

Line Node::getTopLine() {
    Vertex begin = {this->coor_X_, this->coor_Y_ + this->width_};
    Vertex end = {this->coor_X_ + this->length_, this->coor_Y_ + this->width_};
    Line horizon_line = {begin, end};
    return horizon_line;
}

BStarTree::BStarTree(vector<Component> component_list) {
    vector<int> list;
    vector<Node*> exist_nodes;
    Node *insert;
    list.reserve(component_list.size());
    exist_nodes.reserve(component_list.size());
    int exist_num;
    int branch;

    for (int i = 0; i < component_list.size(); i++)
        list.push_back(i);

    random_shuffle(list.begin(), list.end(), MyRandom);

    if (kSingleDoubleSided == 1) {
        /*** Double-Sided ***/
        root = new Node(component_list.at(list.at(0)));
        Node *front_root = new Node(component_list.at(list.at(1)));
        Node *back_root = new Node(component_list.at(list.at(2)));
        front_root->angle_ = 0;
        back_root->angle_ = 0;
        root->left_child_ = front_root;
        root->right_child_ = back_root;
        front_root->parent_ = root;
        back_root->parent_ = root;
        exist_nodes.push_back(root);
        exist_nodes.push_back(front_root);
        exist_nodes.push_back(back_root);
        /******************************/
    }
    else {
        /*** Single-Sided ***/
        root = new Node(component_list.at(list.at(0)));
        exist_nodes.push_back(root);
        /******************************/
    }

    for (int i = 1; i < component_list.size(); i++)
    {   
        branch = rand() % 2;
        // cout << "Insert branch: " << branch << endl;
        exist_num = exist_nodes.size();
        insert = exist_nodes.at(rand() % exist_num);
        Node *insert_node = new Node(component_list.at(list.at(i)));
        if (branch == 0) {
            insert = LeftMost(insert);
            insert->left_child_ = insert_node;
            insert_node->parent_ = insert;
        }
        else {
            insert = RightMost(insert);
            insert->right_child_ = insert_node;
            insert_node->parent_ = insert;
        }
        exist_nodes.push_back(insert_node);
    }
}

// BStarTree::BStarTree(vector<Component> component_list, string filename){
//     vector<Node*> exist_nodes;
//     Node *insert;
//     exist_nodes.reserve(component_list.size());
//     int exist_num;
//     int branch;
//     vector<string> name;
//     vector<int> angle;
//     vector<string> left_child;
//     vector<string> right_child;
//     ifstream inFile(filename);
//     while (!inFile.eof()){
//         string temp_name;
//         // cout << "input ok" << endl;
//         getline(inFile,temp_name,',');
//         name.push_back(temp_name);
//         cout << temp_name << " ";
//         getline(inFile,temp_name,',');
//         if(temp_name.compare("0") == 0){
//             angle.push_back(0);
//         }else{
//             angle.push_back(1);
//         }
//         getline(inFile,temp_name,',');
//         left_child.push_back(temp_name);
//         getline(inFile,temp_name,'\n');
//         right_child.push_back(temp_name);
//     }
//     root = new Node(component_list.at(0));
//     GenerateChild(root, component_list, name, angle,  left_child, right_child);
// }

BStarTree::~BStarTree() {
    deleteTree(root);
}

void BStarTree::deleteTree(Node *leaf) {
    if (leaf != NULL){
        deleteTree(leaf->left_child_);
        deleteTree(leaf->right_child_);
        delete leaf;
    }
}

Node *BStarTree::search(string name, Node *node) {
    Node *temp = nullptr;
    if (node){
        if (name == node->component_.name)
            return node;
        else
        {
            if (node->left_child_)
            {
                temp = search(name, node->left_child_);
                if (temp) return temp;
            }
            if (node->right_child_)
            {
                temp = search(name, node->right_child_);
                if (temp) return temp;
            }
            return temp;
        }
    }
    else 
        return temp;
}

Node *BStarTree::search(string name) {
    return search(name, root);
}

void BStarTree::printTree() {
    printTree(root);
    cout << endl;
}

void BStarTree::printTree(Node *current) {    // Pre-Order Traversal
    if (current){
        cout << current->component_.name << " ";
        // cout << current->component_.length << " ";
        // cout << current->component_.width << " ";
        printTree(current->left_child_);
        printTree(current->right_child_);
    }
}

// void BStarTree::outputTree()
// {
//     ofstream ExpResult("Debug.csv");
//     vector<string> name;
//     vector<int> angle;
//     vector<string> left_child;
//     vector<string> right_child;
//     outputTree(root,name,angle,left_child,right_child);
//     for(int i = 0; i < name.size(); i++){
//         ExpResult << name.at(i) << "," << angle.at(i) << "," << left_child.at(i) << "," << right_child.at(i) << "\n";
//     }
//     ExpResult.close();
// }

void BStarTree::outputTree(Node *current, vector<string> &name, vector<int> &angle, vector<string> &left_child, vector<string> &right_child){
    if (current){
        name.push_back(current->component_.name);
        angle.push_back(current->angle_);
        if(current->left_child_){
            left_child.push_back(current->left_child_->component_.name);
        }else{
            left_child.push_back("none");
        }
        if(current->right_child_){
            right_child.push_back(current->right_child_->component_.name);
        }else{
            right_child.push_back("none");
        }
        outputTree(current->left_child_,name,angle,left_child,right_child);
        outputTree(current->right_child_,name,angle,left_child,right_child);
    }
}

void BStarTree::readAngle(Node *current) {
    if (current){
        // if (current->Pri_Sec == "P") {
        //     current->margin_ = 1;
        // } else if (current->Pri_Sec == "S") {
        //     current->margin_ = 0.5;
        // } else if (current->Pri_Sec == "N") {
        //     current->margin_ = 0.5;
        // } else {
        //     current->margin_ = 0;
        // }
        current->margin_ = kmargin;

        current->height_ = current->component_.height;
        current->vol_ = current->component_.voltage;

        if (current->angle_ == 0){
            current->width_ = current->component_.width + 2 * current->margin_;
            current->length_ = current->component_.length + 2 * current->margin_;
        }
        else if (current->angle_ == 1) {
            current->width_ = current->component_.length + 2 * current->margin_;
            current->length_ = current->component_.width + 2 * current->margin_;
        }
        readAngle(current->left_child_);
        readAngle(current->right_child_);
    }
}

void BStarTree::evaluateArea(int side) {
    vector<Node*> node_vector;              // for .js to plot
    stack<Node*> node_stack;
    vector<Line> horizon_contour;           
    vector<Line> new_horizon_contour;
    double current_x;
    double current_y;

    // Define three pre-placed screw holes (exclude the bottom left root).
    // May import from csv.
    // Vertex screwHole_1_bl = {24.2, 0};    // { PCB_x - 3, 0 }
    // Vertex screwHole_1_tr = {27.2, 3};    // { PCB_x, 3 }
    // Vertex screwHole_2_bl = {0, 29.5};    // { 0, PCB_y - 3 }
    // Vertex screwHole_2_tr = {3, 32.5};    // { 3, PCB_y }
    // Vertex screwHole_3_bl = {24.2, 29.5};   // { PCB_x - 3, PCB_y - 3 }
    // Vertex screwHole_3_tr = {27.2, 32.5};   // { PCB_x, PCB_y }
    // Line screwHole_1 = {screwHole_1_bl, screwHole_1_tr};
    // Line screwHole_2 = {screwHole_2_bl, screwHole_2_tr};
    // Line screwHole_3 = {screwHole_3_bl, screwHole_3_tr};
    // vector<Line> preplaced_module;
    // preplaced_module.push_back(screwHole_1);
    // preplaced_module.push_back(screwHole_2);
    // preplaced_module.push_back(screwHole_3);

    // Initialize root and push into stack
    readAngle(this->root);
    if (side == 0) {
        this->root->left_child_->branch_ = "root";
        this->root->left_child_->coor_X_ = 2;
        this->root->left_child_->coor_Y_ = 2;
        this->root->left_child_->coor_Z_ = 0;
        node_stack.push(this->root->left_child_);
    }
    else if (side == 1) {
        this->root->right_child_->branch_ = "root";
        this->root->right_child_->coor_X_ = 2;
        this->root->right_child_->coor_Y_ = 2;
        this->root->right_child_->coor_Z_ = 1;
        node_stack.push(this->root->right_child_);
    }
    else if (side == 2) {
        this->root->branch_ = "root";
        this->root->coor_X_ = 0;
        this->root->coor_Y_ = 0;
        this->root->coor_Z_ = 0;
        node_stack.push(this->root);
    }

    while (node_stack.empty() == false) {
        new_horizon_contour.clear();
        Node* temp = node_stack.top();
        node_vector.push_back(node_stack.top());
        node_stack.pop();
        bool check_pp = false;
        bool no_overlap = true;     // add line directly if line has no overlap with horizon contour
        
        // get bottom-left position & top line
        current_x = temp->coor_X_;
        current_y = temp->coor_Y_;
        Line horizon_line = temp->getTopLine();

        // start updating horizon contour
        if (horizon_contour.size() == 0) {      // root: no further line existed.
            new_horizon_contour.push_back(horizon_line);
        } 
        else {
            // update y position
            for (int i = 0; i < horizon_contour.size(); i++) {
                if (current_y < horizon_contour[i].begin.y && Overlap(horizon_contour[i], horizon_line) > 0) {
                    current_y = horizon_contour[i].begin.y;
                }
            }
            temp->coor_Y_ = current_y;
            horizon_line = temp->getTopLine();
            
            // check any overlap with pre-placed components
            // for (int i = 0; i < preplaced_module.size(); i++) {
            //     if (PreplacedCheck(temp, preplaced_module.at(i))){
            //         check_pp = true;
            //         preplaced_module.at(i).begin.y += 3;
            //         horizon_line = preplaced_module.at(i);
            //         node_stack.push(temp);
            //         node_vector.pop_back();
            //     }
            // }

            // update horizon contour
            for (int i = 0; i < horizon_contour.size(); i++) {
                if (horizon_contour[i].begin.y < horizon_line.begin.y && Overlap(horizon_contour[i], horizon_line) > 0) {
                    no_overlap = false;
                    if (horizon_contour[i].begin.x >= horizon_line.begin.x && horizon_contour[i].end.x <= horizon_line.end.x) {
                        // new line is longer than one of the segment of horizon contour
                        if (NotSameLine(new_horizon_contour.back(), horizon_line)) {
                            new_horizon_contour.push_back(horizon_line);
                        }
                    } else if (horizon_contour[i].begin.x <= horizon_line.begin.x && horizon_contour[i].end.x >= horizon_line.end.x) {
                        // new line is shorter than one of the segment of horizon contour
                        if (NotSameLine(new_horizon_contour.back(), horizon_line)) {
                            new_horizon_contour.push_back(horizon_line);
                        }
                        Vertex begin = {horizon_line.end.x, horizon_contour[i].begin.y};
                        Line line_segment = {begin, horizon_contour[i].end};
                        new_horizon_contour.push_back(line_segment);
                    } else if (horizon_contour[i].begin.x >= horizon_line.begin.x && horizon_contour[i].end.x >= horizon_line.end.x) {
                        // new line is on the left of one of the segment of horizon contour
                        if (NotSameLine(new_horizon_contour.back(), horizon_line)) {
                            new_horizon_contour.push_back(horizon_line);
                        }
                        Vertex begin = {horizon_line.end.x, horizon_contour[i].begin.y};
                        Line line_segment = {begin, horizon_contour[i].end};
                        new_horizon_contour.push_back(line_segment);
                    }
                } 
                else {
                    new_horizon_contour.push_back(horizon_contour[i]);
                }
            }
            if (no_overlap) {
                // cout << "no_overlap, add directly" << endl;
                new_horizon_contour.push_back(horizon_line);
            }
        }

        // cout << "Before sorting." << endl;
        // for (int i = 0; i < new_horizon_contour.size(); i++) {
        //     cout << "begin (" << new_horizon_contour.at(i).begin.x << ", " << new_horizon_contour.at(i).begin.y << "), end (" << new_horizon_contour.at(i).end.x << ", " << new_horizon_contour.at(i).end.y << ")" << endl;
        // }

        sort(new_horizon_contour.begin(), new_horizon_contour.end(), ContourCompare);

        // cout << "After sorting." << endl;
        // for (int i = 0; i < new_horizon_contour.size(); i++) {
        //     cout << "begin (" << new_horizon_contour.at(i).begin.x << ", " << new_horizon_contour.at(i).begin.y << "), end (" << new_horizon_contour.at(i).end.x << ", " << new_horizon_contour.at(i).end.y << ")" << endl;
        // }

        // update horizon contour
        horizon_contour.clear();
        for (int i = 0; i < new_horizon_contour.size(); i++) {
            horizon_contour.push_back(new_horizon_contour[i]);
        }

        if (!check_pp) {
            if (temp->right_child_) {
                node_stack.push(temp->right_child_);
                temp->right_child_->branch_ = "right";
                temp->right_child_->coor_X_ = current_x;
                temp->right_child_->coor_Y_ = current_y + temp->width_;

                if (temp->right_child_->component_.name == "T501" || temp->right_child_->component_.name == "M503" ) {
                    tuple<double, double> coor = make_tuple(temp->right_child_->coor_X_, temp->right_child_->coor_Y_);
                    this->comp_coordinate_[temp->right_child_->component_.name] = coor;
                }
                else {
                    tuple<double, double> coor = make_tuple(temp->right_child_->coor_X_ + temp->right_child_->length_ / 2, temp->right_child_->coor_Y_ + temp->right_child_->width_ / 2);
                    this->comp_coordinate_[temp->right_child_->component_.name] = coor;
                }
                

                if (side == 0 || side == 2)
                    temp->right_child_->coor_Z_ = 0;
                else
                    temp->right_child_->coor_Z_ = 1;
                
            }
            if (temp->left_child_) {
                node_stack.push(temp->left_child_);
                temp->left_child_->branch_ = "left";
                temp->left_child_->coor_X_ = current_x + temp->length_;
                temp->left_child_->coor_Y_ = current_y;

                if (temp->left_child_->component_.name == "T501" || temp->left_child_->component_.name == "M503" ) {
                    tuple<double, double> coor = make_tuple(temp->left_child_->coor_X_, temp->left_child_->coor_Y_);
                    this->comp_coordinate_[temp->left_child_->component_.name] = coor;
                }
                else {
                    // component center => comp_coordinate_
                    tuple<double, double> coor = make_tuple(temp->left_child_->coor_X_ + temp->left_child_->length_ / 2, temp->left_child_->coor_Y_ + temp->left_child_->width_ / 2);
                    this->comp_coordinate_[temp->left_child_->component_.name] = coor;
                }
                

                if (side == 0 || side == 2)
                    temp->left_child_->coor_Z_ = 0;
                else
                    temp->left_child_->coor_Z_ = 1;
            }
        }
    }

    double MAX_X = 0;
    double MAX_Y = 0;
    for (int i = 0; i < horizon_contour.size(); i++) {
        MAX_X = max(MAX_X, horizon_contour[i].end.x);
        MAX_Y = max(MAX_Y, horizon_contour[i].end.y);
    }

    if (side == 0 || side == 2) {
        this->front_Max_X_ = MAX_X;
        this->front_Max_Y_ = MAX_Y;
        this->front_area_ = MAX_X * MAX_Y;
        // cout << this->front_Max_X_ << ", " << this->front_Max_Y_ << endl;
        // if (MAX_X < 30 && MAX_Y < 40)
        //     this->front_area_ = MAX_X * MAX_Y;
        // else
        //     this->front_area_ = MAX_X * MAX_Y + 1000;

        writeJS(node_vector, horizon_contour, this->front_area_, side);
    }
    else {
        this->back_Max_X_ = MAX_X;
        this->back_Max_Y_ = MAX_Y;
        this->back_area_ = MAX_X * MAX_Y;
        // if (MAX_X < 30 && MAX_Y < 40)
        //     this->back_area_ = MAX_X * MAX_Y;
        // else
        //     this->back_area_ = MAX_X * MAX_Y + 1000;

        writeJS(node_vector, horizon_contour, this->back_area_, side);
    }
}

void BStarTree::evaluateTotalArea() {

    this->comp_coordinate_.clear();

    if (kSingleDoubleSided == 1){
        int side = 0;
        this->evaluateArea(side);
        side = 1;
        this->evaluateArea(side);
        
        if (this->front_area_ > this->back_area_)
            this->area_ = this->front_area_;
        else
            this->area_ = this->back_area_;
    }
    else if (kSingleDoubleSided == 0) {
        int side = 2;
        this->evaluateArea(side);
        this->area_ = this->front_area_;
    }
}

void BStarTree::evaluateWireLength(map< string, vector<string> > netlist, map< int, tuple<double, double> > M503_Pin_Location, map< int, tuple<double, double> > T501_Pin_Location) {
    /* Steps:   
        1. Find the nodes/components in the net.
        2. Keep the coordinates. (left-bottom & right-top of the component)
        3. Evaluate the net length. (HPWL)
        4. Sum total wire length.
    */
    map< string, vector<string> >::iterator net_iter;
    map< int, tuple<double, double> >::iterator pin_iter;
    vector< tuple<double, double> > comp_in_net;    // Coordinate of every component in each net.
    double net_length = 0;
    double total_net_length = 0;
    string component_name, pin_num;
    int pin;

    for (net_iter = netlist.begin(); net_iter != netlist.end(); net_iter++) {
        // cout << net_iter->first << endl;
        for (int c = 0; c < net_iter->second.size(); c++) {
            if (net_iter->second.at(c).at(4) == '-') {
                component_name = component_name.assign(net_iter->second.at(c), 0, 4);
            }
            else if (net_iter->second.at(c).at(5) == '-') {
                component_name = component_name.assign(net_iter->second.at(c), 0, 5);
            }
            // cout << component_name << endl;
            if (component_name == "C509" || component_name == "C510") {
                double width = get<0>(this->comp_coordinate_.find("T501")->second) + 11;
                double length = get<1>(this->comp_coordinate_.find("T501")->second) + 10.25;
                // cout << "(" << width << ", " << length << ")" << endl;
                comp_in_net.push_back(make_tuple(width, length));
            }
            else if (component_name == "M503") {
                pin_num = pin_num.assign(net_iter->second.at(c), 5, 6);
                pin = stoi(pin_num);
                pin_iter = M503_Pin_Location.find(pin);
                comp_in_net.push_back(pin_iter->second);
            }
            else if (component_name == "T501") {
                pin_num = pin_num.assign(net_iter->second.at(c), 5, 6);
                pin = stoi(pin_num);
                pin_iter = T501_Pin_Location.find(pin);
                comp_in_net.push_back(pin_iter->second);
            }
            else {
                double bl_x, bl_y, tr_x, tr_y;  // bottom_left & top_right of component
                bl_x = get<0>(this->comp_coordinate_.find(component_name)->second) - this->search(component_name)->length_ / 2;
                tr_x = get<0>(this->comp_coordinate_.find(component_name)->second) + this->search(component_name)->length_ / 2;
                bl_y = get<1>(this->comp_coordinate_.find(component_name)->second) - this->search(component_name)->width_ / 2;
                tr_y = get<1>(this->comp_coordinate_.find(component_name)->second) + this->search(component_name)->width_ / 2;
                comp_in_net.push_back(make_tuple(bl_x, bl_y));
                comp_in_net.push_back(make_tuple(tr_x, tr_y));
            }
            // cout << "(" << get<0>(this->comp_coordinate_.find(net_iter->second.at(c))->second) << ", " << get<1>(this->comp_coordinate_.find(net_iter->second.at(c))->second) << ")" <<endl;
        }
        // cout << endl;
        net_length = CalcuHPWL(comp_in_net);
        comp_in_net.clear();
        total_net_length += net_length;
        // cout << total_net_length << endl;
    }
    this->wire_length_ = total_net_length;
}

void BStarTree::evaluatePnS() {
    // Evaluate the intersecting area of primary parts and secondary parts.
    // Minimize primary parts area.
    /* Steps:   
        1. Find the nodes/components in the primary parts.
        2. Keep the coordinates. (center of the component)
        3. Evaluate the Centralization. (Manhattan distance/Euclidean distance)
    */
    vector< tuple<double, double> > primary_part;
    stack<Node*> node_stack;
    node_stack.push(this->root);

    // cout << "---" << endl;
    while (node_stack.empty() == false) {
        Node* temp = node_stack.top();
        // cout << temp->component_.name << endl;
        // cout << temp->vol_ << endl;

        if (temp->vol_ == 1) {
            primary_part.push_back(this->comp_coordinate_.find(temp->component_.name)->second);
        }

        node_stack.pop();
        if (temp->right_child_)
            node_stack.push(temp->right_child_);
        if (temp->left_child_)
            node_stack.push(temp->left_child_);
    }
    // cout << "---" << endl;

    // cout << primary_part.size() << endl;
    // for (int k = 0; k < primary_part.size(); k++) {
    //     cout << get<0>(primary_part.at(k)) << ", " << get<1>(primary_part.at(k)) << endl;
    // }

    this->voltage_ = CalcuCentralization(primary_part);
    primary_part.clear();
}

void BStarTree::evaluate(map< string, vector<string> > netlist, map< int, tuple<double, double> > M503_pin_location, map< int, tuple<double, double> > T501_pin_location) {
    // cout << "area evaluation" << endl;
    this->evaluateTotalArea();
    // cout << "WL evaluation" << endl;
    this->evaluateWireLength(netlist, M503_pin_location, T501_pin_location);
    // cout << "PnS evaluation" << endl;
    this->evaluatePnS();
    this->fitness_ = this->area_ / 750 * 0.4 + this->wire_length_ / 270 *0.4 + this->voltage_ / 9 * 0.2; // TODO normalize: student-t
}

void BStarTree::evaluate_MOEAD(tuple<double, double, double> weight_vector, map< string, vector<string> > netlist, map< int, tuple<double, double> > M503_pin_location, map< int, tuple<double, double> > T501_pin_location) {
    this->evaluateTotalArea();
    this->evaluateWireLength(netlist, M503_pin_location, T501_pin_location);
    this->evaluatePnS();
    this->fitness_ = this->area_ / 350 * get<0>(weight_vector) + this->wire_length_ / 170 * get<1>(weight_vector) + this->voltage_ / 30 * get<2>(weight_vector);
}

BStarTree *BStarTree::swapNodeMutation() {
    // cout << "Swap Node Mutation" << endl;
    BStarTree *dup_tree = CopyTree(this);
    BStarTree *new_tree = nullptr;

    if (kSingleDoubleSided == 0) {
        vector<Node*> node_permu;
        node_permu = ExtractTree(dup_tree->root);

        int amount = node_permu.size();

        int mutate_p1 = 0, mutate_p2 = 0;
        while (mutate_p1 == mutate_p2 || mutate_p1 == 0 || mutate_p2 == 0){
            mutate_p1 = rand() % amount;
            mutate_p2 = rand() % amount;
        }

        // cout << node_permu.at(mutate_p1)->component_.name << " swap with " << node_permu.at(mutate_p2)->component_.name << endl;

        swap(node_permu.at(mutate_p1), node_permu.at(mutate_p2));
        new_tree = ModifyTree(this, node_permu);

        return new_tree;
    }
    else if (kSingleDoubleSided == 1) {
        vector<Node*> node_permu_front;
        vector<Node*> node_permu_back;
        node_permu_front = ExtractTree(dup_tree->root->left_child_);
        node_permu_back = ExtractTree(dup_tree->root->right_child_);

        int front_size = node_permu_front.size();
        int back_size = node_permu_back.size();
        node_permu_front.insert(node_permu_front.end(), node_permu_back.begin(), node_permu_back.end());
        node_permu_back.clear();
        int amount = node_permu_front.size();

        int mutate_p1 = 0, mutate_p2 = 0;
        while (mutate_p1 == mutate_p2 || mutate_p1 == 0 || mutate_p2 == 0 || mutate_p1 == front_size || mutate_p2 == front_size){
            mutate_p1 = rand() % amount;
            mutate_p2 = rand() % amount;
        }

        // cout << node_permu_front.at(mutate_p1)->component_.name << " swap with " << node_permu_front.at(mutate_p2)->component_.name << endl;

        swap(node_permu_front.at(mutate_p1), node_permu_front.at(mutate_p2));

        int n = 0;
        while (node_permu_back.size() != back_size) {
            node_permu_back.push_back(node_permu_front.at(front_size + n));
            n++;
        }
        new_tree = ModifyDoubleSidedTree(this, node_permu_front, node_permu_back);
    }

    return new_tree;
}

BStarTree *BStarTree::insertMutation() {
    // cout << "Insert Mutation" << endl;
    BStarTree *dup_tree = CopyTree(this);
    BStarTree *new_tree = nullptr;

    if (kSingleDoubleSided == 0) {
        vector<Node*> node_permu;
        node_permu = ExtractTree(dup_tree->root);

        int amount = node_permu.size();
        int mutate_p1 = 0, mutate_p2 = 0;
        while (mutate_p1 == mutate_p2 || mutate_p1 == mutate_p2 +1 || mutate_p2 == mutate_p1 +1 || mutate_p1 == 0 || mutate_p2 == 0 ) {
            mutate_p1 = rand() % amount;
            mutate_p2 = rand() % amount;
        }

        int pivot = mutate_p2;
        Node *key_node = node_permu.at(mutate_p2);

        // cout << "Insertion point: " << node_permu.at(mutate_p1)->component_.name << " and " << node_permu.at(mutate_p2)->component_.name << endl;

        if (mutate_p1 < mutate_p2){
            while (pivot > mutate_p1) {
                node_permu.at(pivot) = node_permu.at(pivot-1);
                pivot--;
            }
            node_permu.at(mutate_p1+1) = key_node;
        }
        else {
            while (pivot < mutate_p1) {
                node_permu.at(pivot) = node_permu.at(pivot+1);
                pivot++;
            }
            node_permu.at(mutate_p1-1) = key_node;
        }

        new_tree = ModifyTree(this, node_permu);

    }
    else if (kSingleDoubleSided == 1) {

        vector<Node*> node_permu_front;
        vector<Node*> node_permu_back;
        node_permu_front = ExtractTree(dup_tree->root->left_child_);
        node_permu_back = ExtractTree(dup_tree->root->right_child_);
        node_permu_front.erase(node_permu_front.begin());
        node_permu_back.erase(node_permu_back.begin());

        int front_size = node_permu_front.size();
        int back_size = node_permu_back.size();
        node_permu_front.insert(node_permu_front.end(), node_permu_back.begin(), node_permu_back.end());
        node_permu_back.clear();
        int amount = node_permu_front.size();

        int mutate_p1 = 0, mutate_p2 = 0;
        while (mutate_p1 == mutate_p2 || mutate_p1 == mutate_p2 +1 || mutate_p2 == mutate_p1 +1) {
            mutate_p1 = rand() % amount;
            mutate_p2 = rand() % amount;
        }

        int pivot = mutate_p2;
        Node *key_node = node_permu_front.at(mutate_p2);

        // cout << "Insertion point: " << node_permu_front.at(mutate_p1)->component_.name << " and " << node_permu_front.at(mutate_p2)->component_.name << endl;

        if (mutate_p1 < mutate_p2){
            while (pivot > mutate_p1) {
                node_permu_front.at(pivot) = node_permu_front.at(pivot-1);
                pivot--;
            }
            node_permu_front.at(mutate_p1+1) = key_node;
        }
        else {
            while (pivot < mutate_p1) {
                node_permu_front.at(pivot) = node_permu_front.at(pivot+1);
                pivot++;
            }
            node_permu_front.at(mutate_p1-1) = key_node;
        }

        int n = 0;
        while (node_permu_back.size() != back_size) {
            node_permu_back.push_back(node_permu_front.at(front_size + n));
            n++;
        }

        node_permu_front.insert(node_permu_front.begin(), this->root->left_child_);
        node_permu_back.insert(node_permu_back.begin(), this->root->right_child_);

        new_tree = ModifyDoubleSidedTree(this, node_permu_front, node_permu_back);
    }

    return new_tree;
}

BStarTree *BStarTree::scrambleMutation() {
    // cout << "Scramble Mutation" << endl;
    BStarTree *dup_tree = CopyTree(this);
    BStarTree *new_tree = nullptr;

    if (kSingleDoubleSided == 0) {
        vector<Node*> node_permu;
        node_permu = ExtractTree(dup_tree->root);
        int amount = node_permu.size();
        vector<Node*> scramble_list;

        int mutate_p1 = 0, mutate_p2 = 0;
        while (mutate_p1 == mutate_p2 || mutate_p1 == 0 || mutate_p2 == 0){
            mutate_p1 = rand() % amount;
            mutate_p2 = rand() % amount;
        }
        if (mutate_p2 < mutate_p1){
            swap(mutate_p1, mutate_p2);
        }

        // cout << "m1: " << mutate_p1 << "  m2: " << mutate_p2 << endl;

        int pivot = mutate_p1;
        while (pivot <= mutate_p2){
            scramble_list.push_back(node_permu.at(pivot));
            pivot++;
        }

        random_shuffle(scramble_list.begin(), scramble_list.end(), MyRandom);

        pivot = mutate_p1;
        while (pivot <= mutate_p2){
            node_permu.at(pivot) = scramble_list.at(pivot-mutate_p1);
            pivot++;
        }

        new_tree = ModifyTree(this, node_permu);

    }
    else if (kSingleDoubleSided == 1) {
        vector<Node*> node_permu_front = ExtractTree(dup_tree->root->left_child_);
        vector<Node*> node_permu_back = ExtractTree(dup_tree->root->right_child_);
        node_permu_front.erase(node_permu_front.begin());
        node_permu_back.erase(node_permu_back.begin());

        int front_size = node_permu_front.size();
        int back_size = node_permu_back.size();
        node_permu_front.insert(node_permu_front.end(), node_permu_back.begin(), node_permu_back.end());
        node_permu_back.clear();
        int amount = node_permu_front.size();

        vector<Node*> scramble_list;
        int mutate_p1 = 0, mutate_p2 = 0;
        while (mutate_p1 == mutate_p2 || mutate_p1 == 0 || mutate_p2 == 0){
            mutate_p1 = rand() % amount;
            mutate_p2 = rand() % amount;
        }
        if (mutate_p2 < mutate_p1){
            swap(mutate_p1, mutate_p2);
        }

        // cout << "m1: " << mutate_p1 << "  m2: " << mutate_p2 << endl;

        int pivot = mutate_p1;
        while (pivot <= mutate_p2){
            scramble_list.push_back(node_permu_front.at(pivot));
            pivot++;
        }

        random_shuffle(scramble_list.begin(), scramble_list.end(), MyRandom);

        pivot = mutate_p1;
        while (pivot <= mutate_p2){
            node_permu_front.at(pivot) = scramble_list.at(pivot-mutate_p1);
            pivot++;
        }

        int n = 0;
        while (node_permu_back.size() != back_size) {
            node_permu_back.push_back(node_permu_front.at(front_size + n));
            n++;
        }

        node_permu_front.insert(node_permu_front.begin(), this->root->left_child_);
        node_permu_back.insert(node_permu_back.begin(), this->root->right_child_);

        new_tree = ModifyDoubleSidedTree(this, node_permu_front, node_permu_back);
    }

    return new_tree;
}

// for SA local search
BStarTree *BStarTree::bitwiseOperation() {
    // cout << "Bit Wise Operation" << endl;
    BStarTree *dup_tree = CopyTree(this);
    BStarTree *new_tree = nullptr;

    if (kSingleDoubleSided == 0) {
        vector<Node*> node_permu;
        node_permu = ExtractTree(dup_tree->root);
        int amount = node_permu.size();
        int operation_pos = 0;
        while (operation_pos == 0)
            operation_pos = rand() % amount;
        
        Node *rotate_node = node_permu.at(operation_pos);
        rotate_node->angle_ = 1 - rotate_node->angle_;

        new_tree = ModifyTree(this, node_permu);

    }
    else if (kSingleDoubleSided == 1) {
        vector<Node*> node_permu_front;
        vector<Node*> node_permu_back;
        node_permu_front = ExtractTree(dup_tree->root->left_child_);
        node_permu_back = ExtractTree(dup_tree->root->right_child_);

        int front_size = node_permu_front.size();
        int back_size = node_permu_back.size();
        node_permu_front.insert(node_permu_front.end(), node_permu_back.begin(), node_permu_back.end());
        node_permu_back.clear();
        int amount = node_permu_front.size();

        int operation_pos = 0;
        while (operation_pos == 0 || operation_pos == front_size)
            operation_pos = rand() % amount;
        
        Node *rotate_node = node_permu_front.at(operation_pos);
        rotate_node->angle_ = 1 - rotate_node->angle_;

        int n = 0;
        while (node_permu_back.size() != back_size) {
            node_permu_back.push_back(node_permu_front.at(front_size + n));
            n++;
        }
        new_tree = ModifyDoubleSidedTree(this, node_permu_front, node_permu_back);
    }
    
    return new_tree;

}

BStarTree *BStarTree::bitwiseMutation() {
    BStarTree *new_tree = CopyTree(this);
    bitFlip(new_tree->root);
    
    return new_tree;
}

void BStarTree::bitFlip(Node *current) {
    if (current->branch_ != "root"){
        double bit_flip = (double) rand() / (RAND_MAX + 1.0);
        if (bit_flip <= kProbBitFlip){
            // cout << "bit flip" << endl;
            if (current->angle_ == 0)
                current->angle_ = 1;
            else current->angle_ = 0;
        }

        if (current->left_child_)
            bitFlip(current->left_child_);
        if (current->right_child_)
            bitFlip(current->right_child_);
    }
}

BStarTree *BStarTree::swapBranchMutation() {
    /* Steps:
    1. CopyTree
    2. Find Level List - Find nodes with two branches
    3. rand a node
    4. swap l/r child
    5. return
    */

    // cout << "Swap Branch Mutation" << endl;
    BStarTree *new_tree = CopyTree(this);
    vector<Node*> node_list = Find2BNode(new_tree->root);

    if (node_list.size()){
        Node *target_node = node_list[rand() % node_list.size()];
        // new_tree->printTree();
        // cout << endl;
        Node *temp = new Node;
        temp = target_node->right_child_;
        target_node->right_child_ = target_node->left_child_;
        target_node->left_child_ = temp;
        // new_tree->printTree();
        // cout << endl;
    }
    return new_tree;
}

BStarTree *BStarTree::shiftSubtreeMutation() {
    /* Steps:
    1. CopyTree, ExtractTree (pop root)
    2. rand a node
    3. cut links -> parent tree, subtree
    4. Extract parent tree, append available node
    5. rand a node / rand a branch
    6. connect subtree
    7. return
    */

    // cout << "Shift Subtree Mutation" << endl;
    BStarTree *new_tree = CopyTree(this);
    vector<Node*> node_list;

    if (kSingleDoubleSided == 0) {
        node_list = ExtractTree(new_tree->root);
        node_list.erase(node_list.begin());
    }
    else if (kSingleDoubleSided == 1) {
        node_list = ExtractTree(new_tree->root->left_child_);
        vector<Node*> node_permu_back = ExtractTree(new_tree->root->right_child_);
        node_list.erase(node_list.begin());
        node_permu_back.erase(node_permu_back.begin());
        node_list.insert(node_list.end(), node_permu_back.begin(), node_permu_back.end());
    }

    Node *temp = node_list[rand() % node_list.size()];

    if (temp->parent_->right_child_ && temp->parent_->right_child_->component_.name == temp->component_.name){
        // cout << "temp is right child" << endl;
        temp->parent_->right_child_ = nullptr;
        temp->parent_ = nullptr;
    }
    else if (temp->parent_->left_child_ && temp->parent_->left_child_->component_.name == temp->component_.name){
        // cout << "temp is left child" << endl;
        temp->parent_->left_child_ = nullptr;
        temp->parent_ = nullptr;
    }

    vector<Node*> available_node_list = FindNBNode(new_tree->root);
    Node *targe_node = available_node_list[rand() % available_node_list.size()];

    if (targe_node->right_child_ == nullptr && targe_node->left_child_ != nullptr){
        targe_node->right_child_ = temp;
        temp->parent_ = targe_node;
    }
    else if (targe_node->left_child_ == nullptr && targe_node->right_child_ != nullptr){
        targe_node->left_child_ = temp;
        temp->parent_ = targe_node;
    }
    else if (targe_node->left_child_ == nullptr && targe_node->right_child_ == nullptr){
        if ((rand() % 2) == 0){
            targe_node->right_child_ = temp;
            temp->parent_ = targe_node;
        }
        else {
            targe_node->left_child_ = temp;
            temp->parent_ = targe_node;
        }
    }
    
    // new_tree->printTree();
    // cout << endl;
    
    return new_tree;
}

void BStarTree::countNode() {
    this->node_count_ = 0;
    countNode(root);
    cout << this->node_count_;
}

void BStarTree::countNode(Node *current) {
    if (current){
        this->node_count_++;
        countNode(current->left_child_);
        countNode(current->right_child_);
    }
}

BStarTree *BStarTree::swapSubtreeMutation_diffLevel() {
    /* Steps:
    1. CopyTree, ExtractTree (pop root)
    2. rand a node
    3. find available nodes in different level
    4. rand a node from 3.
    5. cut links and connect two subtree
    6. return
    */
    
    BStarTree *new_tree = CopyTree(this);
    vector<Node*> node_list;
    
    if (kSingleDoubleSided == 0) {
        node_list = FindLevel4Node(new_tree->root);
        // node_list.erase(node_list.begin());
    }
    else if (kSingleDoubleSided == 1) {
        node_list = ExtractTree(new_tree->root->left_child_);
        vector<Node*> node_permu_back = ExtractTree(new_tree->root->right_child_);
        node_list.erase(node_list.begin());
        node_permu_back.erase(node_permu_back.begin());
        node_list.insert(node_list.end(), node_permu_back.begin(), node_permu_back.end());
    }
    vector<Node*> avail_node_list;
    Node *temp2;
    Node *temp;
    do{
        temp = node_list[rand() % node_list.size()];
        avail_node_list = FindDLNode(new_tree->root, temp);
        temp2 = avail_node_list[rand() % avail_node_list.size()];
    }while(avail_node_list.size()==0);
    if (temp->parent_->right_child_ && temp->parent_->right_child_->component_.name == temp->component_.name){
        // cout << "temp is right child" << endl;
        temp->parent_->right_child_ = temp2;
    }
    else if (temp->parent_->left_child_ && temp->parent_->left_child_->component_.name == temp->component_.name){
        // cout << "temp is left child" << endl;
        temp->parent_->left_child_ = temp2;
    }
    if (temp2->parent_->right_child_ && temp2->parent_->right_child_->component_.name == temp2->component_.name){
        // cout << "temp2 is right child" << endl;
        temp2->parent_->right_child_ = temp;
    }
    else if (temp2->parent_->left_child_ && temp2->parent_->left_child_->component_.name == temp2->component_.name){
        // cout << "temp2 is left child" << endl;
        temp2->parent_->left_child_ = temp;
    }
    Node *temp3 = new Node;
    temp3 = temp2->parent_; 
    temp2->parent_ = temp->parent_;
    temp->parent_ = temp3;

    return new_tree;
}

BStarTree *BStarTree::swapSubtreeMutation_sameLevel() {
    /* Steps:
    1. CopyTree, ExtractTree (pop root)
    2. rand a node
    3. find available nodes in same level
    4. rand a node from 3.
    5. cut links and connect two subtree
    6. return
    */

    BStarTree *new_tree = CopyTree(this);
    vector<Node*> node_list;

    if (kSingleDoubleSided == 0) {
        node_list = ExtractTree(new_tree->root);
        node_list.erase(node_list.begin());
    }
    else if (kSingleDoubleSided == 1) {
        node_list = ExtractTree(new_tree->root->left_child_);
        vector<Node*> node_permu_back = ExtractTree(new_tree->root->right_child_);
        node_list.erase(node_list.begin());
        node_permu_back.erase(node_permu_back.begin());
        node_list.insert(node_list.end(), node_permu_back.begin(), node_permu_back.end());
    }

    Node *temp = node_list[rand() % node_list.size()];
    int level = CountLevel(temp,new_tree->root);

    vector<Node*> available_node_list = FindLevelNode(new_tree->root, level, temp->component_.name);
    while(available_node_list.size() == 0){
        // no same level node, rand again
        temp = node_list[rand() % node_list.size()];
        level = CountLevel(temp,new_tree->root);
        // cout<< "level:" << level << endl;
        available_node_list = FindLevelNode(new_tree->root,level,temp->component_.name);
    }

    Node *temp2 = available_node_list[rand() % available_node_list.size()];

    if (temp->parent_->right_child_ && temp->parent_->right_child_->component_.name == temp->component_.name){
        // cout << "temp is right child" << endl;
        temp->parent_->right_child_ = temp2;
    }
    else if (temp->parent_->left_child_ && temp->parent_->left_child_->component_.name == temp->component_.name){
        // cout << "temp is left child" << endl;
        temp->parent_->left_child_ = temp2;
    }
    if (temp2->parent_->right_child_ && temp2->parent_->right_child_->component_.name == temp2->component_.name){
        // cout << "temp2 is right child" << endl;
        temp2->parent_->right_child_ = temp;
    }
    else if (temp2->parent_->left_child_ && temp2->parent_->left_child_->component_.name == temp2->component_.name){
        // cout << "temp2 is left child" << endl;
        temp2->parent_->left_child_ = temp;
    }

    Node *temp3 = new Node;
    temp3 = temp2->parent_; 
    temp2->parent_ = temp->parent_;
    temp->parent_ = temp3;

    return new_tree;
}

BStarTree *BStarTree::shuffleSubtreeMutation_sameLevel() {
    BStarTree *new_tree = CopyTree(this);
    vector<Node*> node_list;

    if (kSingleDoubleSided == 0) {
        node_list = ExtractTree(new_tree->root);
        node_list.erase(node_list.begin());
    }
    else if (kSingleDoubleSided == 1) {
        node_list = ExtractTree(new_tree->root->left_child_);
        vector<Node*> node_permu_back = ExtractTree(new_tree->root->right_child_);
        node_list.erase(node_list.begin());
        node_permu_back.erase(node_permu_back.begin());
        node_list.insert(node_list.end(), node_permu_back.begin(), node_permu_back.end());
    }

    Node *temp = node_list[rand() % node_list.size()];
    int level = CountLevel(temp,new_tree->root);

    vector<Node*> available_node_list = FindLevelNode(new_tree->root,level,temp->component_.name);
    while(available_node_list.size() == 0){
        // no same level node, rand again
        temp = node_list[rand() % node_list.size()];
        level = CountLevel(temp,new_tree->root);
        // cout<< "level:" << level << endl;
        available_node_list = FindLevelNode(new_tree->root,level,temp->component_.name);
    }
    available_node_list.push_back(temp);
    
    vector<int> index;
    index.reserve(available_node_list.size());
    for (int i = 0; i < available_node_list.size(); i++){
        index.push_back(i);
    }
    random_shuffle(index.begin(), index.end(), MyRandom);

    for (int j = 0; j < available_node_list.size(); j++){
        temp = available_node_list[j];
        if (temp->parent_->right_child_ && temp->parent_->right_child_->component_.name == temp->component_.name){
            temp->parent_->right_child_ = available_node_list[index[j]];
        }
        else if (temp->parent_->left_child_ && temp->parent_->left_child_->component_.name == temp->component_.name){
            temp->parent_->left_child_ = available_node_list[index[j]];
        }
    }
    vector<Node*> parent_list;
    for (int j = 0; j < available_node_list.size(); j++){
        parent_list.push_back(available_node_list[j]->parent_);
    }
    for (int j = 0; j < available_node_list.size(); j++){
        available_node_list[index[j]]->parent_ = parent_list[j];
    }

    return new_tree;
}

void BStarTree::deleteNode(string name) {
    /* Steps:
       - Find the node we want to delete.
       - Four cases:
        * node with left & right child.
        * node with left/right child only.
        * leaf node.
    */

    Node *delete_node;
    // cout << name << endl;
    // this->printTree();
    delete_node = search(name);
    // cout << delete_node->component.name << " found" << endl;
    // cout << "start judging cases: ";

    // leaf node
    if (delete_node->left_child_ == nullptr && delete_node->right_child_ == nullptr)
    {
        // cout << "no child" << endl;
        if (delete_node != root) // if not root
        {
            if (delete_node->parent_->left_child_ == delete_node)
            {
                delete_node->parent_->left_child_ = nullptr;
            }
            else if (delete_node->parent_->right_child_ == delete_node)
            {
                delete_node->parent_->right_child_ = nullptr;
            }
            else {cout << "deleNode function error 1." << endl;}
        }
        else {
            // cout << "This is the last node." << endl;
            this->root = nullptr;
        }
    }

    // right child only
    else if (delete_node->left_child_ == nullptr && delete_node->right_child_ != nullptr)
    {
        // cout << "right child" << endl;
        if (delete_node == root)
        {
            // cout << "delete node is root" << endl;
            root = delete_node->right_child_;
            delete_node->right_child_->parent_ = nullptr;
        }
        else if (delete_node->parent_->left_child_ == delete_node)
        {
            delete_node->parent_->left_child_ = delete_node->right_child_;
            delete_node->right_child_->parent_ = delete_node->parent_;
            delete_node->parent_ = nullptr;
            delete_node->right_child_ = nullptr;
        }
        else if (delete_node->parent_->right_child_ == delete_node)
        {
            delete_node->parent_->right_child_ = delete_node->right_child_;
            delete_node->right_child_->parent_ = delete_node->parent_;
            delete_node->parent_ = nullptr;
            delete_node->right_child_ = nullptr;
        }
    }
    // left child only
    else if (delete_node->right_child_ == nullptr && delete_node->left_child_ != nullptr)
    {
        // cout << "left child" << endl;
        if (delete_node == root)
        {
            root = delete_node->left_child_;
            delete_node->left_child_->parent_ = nullptr;
        }
        else if (delete_node->parent_->left_child_ == delete_node)
        {
            delete_node->parent_->left_child_ = delete_node->left_child_;
            delete_node->left_child_->parent_ = delete_node->parent_;
            delete_node->parent_ = nullptr;
            delete_node->left_child_ = nullptr;
        }
        else if (delete_node->parent_->right_child_ == delete_node)
        {
            delete_node->parent_->right_child_ = delete_node->left_child_;
            delete_node->left_child_->parent_ = delete_node->parent_;
            delete_node->parent_ = nullptr;
            delete_node->left_child_ = nullptr;
        }
    }
    else //both children exist
    {
        // cout << "both child" << endl;
        Node *right_most = RightMost(delete_node);
        if (right_most->left_child_ != nullptr)
        {
            right_most->right_child_ = right_most->left_child_;
        }
        while (right_most != delete_node)
        {
            if (right_most->parent_->left_child_ != nullptr)
            {
                right_most->left_child_ = right_most->parent_->left_child_;
                right_most->left_child_->parent_ = right_most;
            }
            else right_most->left_child_ = nullptr;
            right_most = right_most->parent_;
        }
        if (delete_node == root)
        {
            delete_node->right_child_->parent_ = nullptr;
            root = delete_node->right_child_;
        }
        else
        {
            delete_node->right_child_->parent_ = delete_node->parent_;
            if (delete_node->parent_->right_child_ == delete_node)
            {
                delete_node->parent_->right_child_ = delete_node->right_child_;
            } 
            else if (delete_node->parent_->left_child_ == delete_node)
            {
                delete_node->parent_->left_child_ = delete_node->right_child_;
            }
            
        }
    }
    delete delete_node;
}

BStarTree *BStarTree::leftSubtreeCrossover(BStarTree *parent_2) {
    /* Steps:
       - vector left_component;
       - deep copy current tree (parent 1) and parent 2 as child_tree and right_subtree
       - child_tree => get left subtree
       - right_subtree => delete repeated nodes
       - Connect.
    */

    vector<string> left_component;
    left_component = GetLeftSubtree(this);
    // cout << "Left subtree: " << endl;
    // for (int i = 0; i < left_component.size(); i++)
    // {
    //     cout << left_component.at(i) << ' ';
    // }
    // cout << endl;
    // cout << endl;

    BStarTree *child = CopyTree(this);
    BStarTree *right_subtree = CopyTree(parent_2);

    // cout << "Right subtree: " << endl;
    // right_subtree->printTree();

    for (int i = 0; i < left_component.size(); i++)
    {
        // right_subtree->printTree();
        right_subtree->deleteNode(left_component.at(i));
    }
    if (right_subtree->root)
    {
        child->root->right_child_ = right_subtree->root;
        right_subtree->root->parent_ = child->root;
    }
    else
    {
        child->root->right_child_ = nullptr;
    }
    
    return child;
}

BStarTree *BStarTree::orderedCrossover(BStarTree *parent_2, int amount) {
    int cut_point = 1 + rand() % (amount - 4);      // cut point range: 1 ~ (total-3)
    // cout << "cut point: " << cut_point << endl;
    BStarTree *dup_tree_1 = CopyTree(this);
    BStarTree *dup_tree_2 = CopyTree(parent_2);
    BStarTree *new_tree = nullptr;
    
    if(kSingleDoubleSided == 0){
        vector<Node*> node_permu_1 = ExtractTree(dup_tree_1->root);
        vector<Node*> node_permu_2 = ExtractTree(dup_tree_2->root);
        vector<Node*> tail_node_permu;
        // tail_node_permu.reserve(amount-cut_point);

        for (int i = 0; i < node_permu_2.size(); i++){
            for (int j = cut_point; j < node_permu_1.size(); j++){
                // cout << p2_node_permu.at(i)->component.name << endl;
                // cout << p1_node_permu.at(j)->component.name << endl;
                if (node_permu_2.at(i)->component_.name == node_permu_1.at(j)->component_.name){
                    tail_node_permu.push_back(node_permu_2.at(i));
                }
            }
        }
        for (int k = 0; k < tail_node_permu.size(); k++){
            node_permu_1[cut_point+k] = tail_node_permu[k];
        }
        new_tree = ModifyTree(this, node_permu_1);
    }
    else if(kSingleDoubleSided == 1){
        vector<Node*> node_permu_front_1 = ExtractTree(dup_tree_1->root->left_child_);
        vector<Node*> node_permu_back_1 = ExtractTree(dup_tree_1->root->right_child_);

        node_permu_front_1.erase(node_permu_front_1.begin());
        node_permu_back_1.erase(node_permu_back_1.begin());
        int front_size_1 = node_permu_front_1.size();
        int back_size_1 = node_permu_back_1.size();
        node_permu_front_1.insert(node_permu_front_1.end(), node_permu_back_1.begin(), node_permu_back_1.end());
        node_permu_back_1.clear();

        vector<Node*> node_permu_front_2 = ExtractTree(dup_tree_2->root->left_child_);
        vector<Node*> node_permu_back_2 = ExtractTree(dup_tree_2->root->right_child_);

        node_permu_front_2.erase(node_permu_front_2.begin());
        node_permu_back_2.erase(node_permu_back_2.begin());
        node_permu_front_2.insert(node_permu_front_2.end(), node_permu_back_2.begin(), node_permu_back_2.end());
        node_permu_back_2.clear();

        vector<Node*> tail_node_permu;
        // tail_node_permu.reserve(amount-cut_point);

        for (int i = 0; i < node_permu_front_2.size(); i++){
            for (int j = cut_point; j < node_permu_front_1.size(); j++){
                // cout << p2_node_permu.at(i)->component_.name << endl;
                // cout << p1_node_permu.at(j)->component_.name << endl;
                if (node_permu_front_2.at(i)->component_.name == node_permu_front_1.at(j)->component_.name){
                    tail_node_permu.push_back(node_permu_front_2.at(i));
                }
            }
        }

        for (int k = 0; k < tail_node_permu.size(); k++){
            node_permu_front_1[cut_point+k] = tail_node_permu[k];
        }

        int n = 0;
        while (node_permu_back_1.size() != back_size_1) {
            node_permu_back_1.push_back(node_permu_front_1.at(front_size_1 + n));
            n++;
        }

        node_permu_front_1.insert(node_permu_front_1.begin(), this->root->left_child_);
        node_permu_back_1.insert(node_permu_back_1.begin(), this->root->right_child_);

        new_tree = ModifyDoubleSidedTree(this, node_permu_front_1, node_permu_back_1);        
    }
    return new_tree;
}

BStarTree *BStarTree::subtreeExchangeCrossover(BStarTree *parent_2, int amount) {
    /* Steps:
       - picked node: randomly choose a non-root node from p1
       - child delete all node in picked node list, except picked node

    */

    Node* picked_node;
    vector<Node*> node_list;
    vector<Node*> picked_node_list;

    BStarTree *p1 = CopyTree(this);

    node_list = ExtractTree(p1->root);
    node_list.erase(node_list.begin());

    picked_node = node_list.at(rand() % (amount-1));
    // cout << "picked_node: " << picked_node->component_.name << endl;
    picked_node_list = ExtractTree(picked_node);

    BStarTree *child = CopyTree(parent_2);

    // cout << "picked node list: ";
    // delete nodes that appear in picked_node_list from parent2
    for(int i = 0; i < picked_node_list.size(); i++){
        // cout << picked_node_list.at(i)->component_.name << " ";
        if(picked_node_list.at(i)->component_.name != picked_node->component_.name){
            child->deleteNode(picked_node_list.at(i)->component_.name);
        }
    }

    Node *left_subtree = nullptr;
    Node *right_subtree = nullptr;

    Node *target = child->search(picked_node->component_.name);

    if(target->left_child_){
        // cout << "left subtree" << endl;
        left_subtree = target->left_child_;
    }
    if(target->right_child_) {
        // cout << "right subtree" << endl;
        right_subtree = target->right_child_;
    }

    if (target->parent_ != nullptr) {
        if(target->parent_->left_child_ && target->parent_->left_child_->component_.name == picked_node->component_.name){
            target->parent_->left_child_ = picked_node;
            picked_node->parent_ = target->parent_;
        }else if(target->parent_->right_child_ && target->parent_->right_child_->component_.name == picked_node->component_.name){
            target->parent_->right_child_ = picked_node;
            picked_node->parent_ = target->parent_;
        }
        else
            cout << "something wrong" << endl;
    }
    else if (target->parent_ == nullptr) {
        picked_node->parent_ = nullptr;
        child->root = picked_node;
    }

    vector<Node*> leaf_list;
    vector<Node*> child_node_list = ExtractTree(child->root);

    for(int i=0;i<child_node_list.size();i++){
        if(child_node_list.at(i)->left_child_ == nullptr && child_node_list.at(i)->right_child_ == nullptr){
            leaf_list.push_back(child_node_list.at(i));
        }
    }

    Node *leaf_node = leaf_list.at(rand() % leaf_list.size());

    if(left_subtree){
        leaf_node->left_child_ = left_subtree;
        left_subtree->parent_ = leaf_node;
    }
    if(right_subtree){
        leaf_node->right_child_ = right_subtree;
        right_subtree->parent_ = leaf_node;
    }

    return child;
}
