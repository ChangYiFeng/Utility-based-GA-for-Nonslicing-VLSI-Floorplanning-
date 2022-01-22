#ifndef DTMUTILITY_H
#define DTMUTILITY_H

#include "dtmStructure.h"
#include <tuple>
#include <map>
#include <stack>
#include <string>
#include <vector>

using namespace std;

/*** Helper Functions ***/

// Count component number.
int GetTotalAmount(string filename);
double GetTotalArea(string filename);

// Read and load component dimensions. Read pin location.
vector<Component> ReadDim(string filename);
map< int, tuple<double, double> > ReadPin(string name);

// Deterministic: Read operator choosing probability
vector<vector<double> > ReadProb(string filename);

// Deep copy tree
BStarTree* CopyTree(BStarTree* tree);

// Traverse tree with pre-order iterate using stack and save in a vector.
vector<Node*> ExtractTree(Node* root);

// Deepcopy a tree and modify node permutation.
BStarTree* ModifyTree(BStarTree* tree, vector<Node*> node_permu);
BStarTree* ModifyDoubleSidedTree(BStarTree* tree, vector<Node*> node_permu_front, vector<Node*> node_permu_back);

// Find nodes with null branch.
vector<Node*> FindNBNode(Node* root);

// Find nodes with two branches.
vector<Node*> Find2BNode(Node* root);

// Find nodes in same level.
vector<Node*> FindLevelNode(Node* root, int level, string name);

// Find nodes in different level.
vector<Node*> FindDLNode(Node* root, Node* node);
vector<Node*> FindLevel4Node(Node* root);

// Check if one of the selected nodes is ancestor of the other.
bool CheckAncestor(Node* node1, Node* node2, Node* root);

// Get the level of the node for different level subtree swap
int CountLevel(Node* node, Node* root);

// Find the most left leaf under the node.
Node* LeftMost(Node* node);

// Find the most right leaf under the node.
Node* RightMost(Node* node);

// Get left subtree. (LeftSubtreeCrossover)
vector<string> GetLeftSubtree(BStarTree* tree);

/*** Functions for placement ***/
double Overlap(Line line1, Line line2);
bool NotSameLine(Line line1, Line line2);
char* hex_color_code();
void writeJS(vector<Node*> node_vector, vector<Line> horizon_contour, double area, int side);
bool PreplacedCheck(Node* node, Line prePlaced);
bool ContourCompare(Line a, Line b);

/*** Functions for primary and secondary evaluation & wire length evaluation. ***/
map<string, vector<string> > GetNetlist(string netlist_name);
double CalcuCentralization(vector< tuple<double, double> > primary_part);
double CalcuHPWL(vector< tuple<double, double> > comp_in_net);

// Sort population by fitness
bool SortPopArea(BStarTree const *tree_1, BStarTree const *tree_2);
bool SortPopWL(BStarTree const *tree_1, BStarTree const *tree_2);
bool SortPop(BStarTree const *tree_1, BStarTree const *tree_2);

// Parent Selection
vector<int> UniformParentSelection();
vector<int> kTournamentSelection();

// Crossover
vector<BStarTree*> Crossover(vector<BStarTree*> population, const int kComponentNum, int crossover_type);

// Mutation
BStarTree* Mutation(BStarTree* indidividual, int mutation_type);

// MOEA/D
vector< tuple<double, double, double> > GetWeightVector(string filename);
vector<int> FindNeighbors(int individual, vector< tuple<double, double, double> > weight_vector, int T);
vector<BStarTree*> UpdateEP(vector<BStarTree*> external_population, vector<BStarTree*> offspring);

// Simulated Annealing
double CoolingSchedule(int iteration, double init_temperature, double alpha);
double AcceptProb(double temperature, double cur_energy, double nbr_energy);
BStarTree* GetNeighbor(BStarTree *current_tree);

// Debug
void GenerateChild(Node *current, vector<Component> component_list, vector<string> name, vector<int> angle, vector<string> left_child, vector<string> right_child);

#endif