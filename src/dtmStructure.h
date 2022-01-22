#ifndef DTMSTRUCTURE_H
#define DTMSTRUCTURE_H
#include <string>
#include <vector>
#include <map>
#include <tuple>
using namespace std;

class BStarTree;

typedef struct Component
{
    string name;
    string color;
    double width;
    double length;
    double height;
    int voltage;
    
}Component;

typedef struct Vertex {
    double x;
    double y;
}Vertex;

typedef struct Line {
    Vertex begin;
    Vertex end;
}Line;

class Node
{
    private:
        
    public:
        Node *left_child_;
        Node *right_child_;
        Node *parent_;
        Component component_;
        string branch_;
        int angle_;
        double coor_X_;
        double coor_Y_;
        int coor_Z_;
        double margin_;
        double width_;
        double length_;
        double height_;
        int vol_;

        void printContent(); 
        Node():left_child_(0), right_child_(0), parent_(0){};
        Node(Component comp):left_child_(nullptr), right_child_(nullptr), parent_(nullptr){component_ = comp; angle_ = rand() % 2; margin_ = 0.05;};
        Line getTopLine();

        friend class BStarTree;
};

class BStarTree
{
    private:
        // Recursive functions
        void deleteTree(Node *leaf);
        void printTree(Node *current);
        void checkAngle(Node *current);
        void bitFlip(Node *current);
        Node *search(string name, Node *node);
        void readAngle(Node *current);
        void countNode(Node *current);
        int node_count_;
        void outputTree(Node *current, vector<string> &name, vector<int> &angle, vector<string> &left_child, vector<string> &right_child);

    public:
        Node *root;
        double front_Max_X_;
        double front_Max_Y_;
        double back_Max_X_;
        double back_Max_Y_;
        double front_area_;
        double back_area_;
        double area_;
        map <string, tuple<double, double> > comp_coordinate_;
        double wire_length_;
        double voltage_;     // Primary or Second parts
        double fitness_;
        bool isChild_;
        int Xtype_;
        int Mtype_;

        BStarTree():root(new Node){};
        BStarTree(Node *n):root(n){};
        BStarTree(vector<Component> component_list);
        BStarTree(vector<Component> component_list, string filename);
        ~BStarTree();

        // Basic functions
        void printTree();
        void outputTree();
        void checkAngle();
        void deleteNode(string name);
        void countNode();
        Node *search(string name);
        void evaluateArea(int side);
        void evaluateTotalArea();
        void evaluateWireLength(map< string, vector<string> > netlist, map< int, tuple<double, double> > M503_pin_location, map< int, tuple<double, double> > T501_pin_location);
        void evaluatePnS();
        void evaluate(map< string, vector<string> > netlist, map< int, tuple<double, double> > M503_pin_location, map< int, tuple<double, double> > T501_pin_location);
        void evaluate_MOEAD(tuple<double, double, double> weight_vector, map< string, vector<string> > netlist, map< int, tuple<double, double> > M503_pin_location, map< int, tuple<double, double> > T501_pin_location);

        // Mutation
        BStarTree *swapNodeMutation();
        BStarTree *insertMutation();
        BStarTree *scrambleMutation();
        BStarTree *bitwiseMutation();
        BStarTree *swapBranchMutation();
        BStarTree *shiftSubtreeMutation();
        BStarTree *swapSubtreeMutation_sameLevel();
        BStarTree *swapSubtreeMutation_diffLevel();
        BStarTree *shuffleSubtreeMutation_sameLevel();
        BStarTree *bitwiseOperation();       // SA local search

        // Crossover
        BStarTree *leftSubtreeCrossover(BStarTree *parent_2);
        BStarTree *orderedCrossover(BStarTree *parent_2, int amount);
        BStarTree *subtreeExchangeCrossover(BStarTree *parent_2, int amount);
};

#endif