#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <tuple>
#include <map>
#include <fstream>
#include <time.h>
#include "Utility.h"
#include "Structure.h"
#include "GlobalVariable.h"
using namespace std;

// alias g++11="g++ -std=c++11"

/* Hyperparameter */
// const string kComponentFile = "./ami49.csv";
const string kComponentFile = "./ami33.csv";
const bool kSingleDoubleSided = 0; // 0 for Single-Sided; 1 for Double-Sided
const double kmargin = 0;
int iteration_count;
int evaluation_count;
double timer;
double clock_sec;
int iter;


/* SA Hyperparameter */
// const double kTargetUtil = 0.97;
// const int kIterationTime = 5000;
const int kEvaluation_limit = 100000;
const double kInitTemp = 1000;
const double kAlpha = 0.85;
const int kExpTime = 10;
// const int kTimeLimit = 70;


/* EC Hyperparameter ===No Use Here== */
const int kGeneration = 1;
const int kPopSize = 100;
double kProbCrossover = 0;
const int kCrossoverTime = kPopSize / 2;
double kProbMutation = 0.7;
const double kProbAngleMutation = 0.5;
const double kProbBitFlip = 0.3;
const int k_kTournament = 3;

int main(){

    /*** Declaration ***/
    srand((unsigned)time(0));
    vector<Component> kComponent_list= ReadDim(kComponentFile);                // stores all component info: ID, W, L, H
    const double kTotalArea = GetTotalArea(kComponentFile);    // REMEMBER TO CHECK HOW MANY HEADER ROWS: MCNC 1, AcBel 4.
    // map< string, vector<string> > Netlist = GetNetlist(kNetlistFile);                    // [net_name, net_members]
    vector<double> anytime_behavior_total_area;
    vector<double> clock_vector_sec;
    vector<double> anytime_behavior_area;
    vector<double> evaluation_record;
    /******************************/
    
    /*** Write experiment settings ***/
    ofstream ExpResult("ami33_SA_exp1.csv");
    // ExpResult << "Iteration Time" << "," << kIterationTime << "\n";
    ExpResult << "Initial Temperature" << "," << kInitTemp << "\n";
    ExpResult << "Cooling Schedule" << "," << "exponential multiplicative cooling " << endl;
    ExpResult << "Alpha" << "," << kAlpha << "\n";
    ExpResult <<  "\n";
    /******************************/

    for(int EXP = 0; EXP < kExpTime; EXP++){

        // Initialize counting parameters
        iteration_count = 0;
        evaluation_count = 0;
        timer = 0;
        clock_sec = 0;
        iter = 0;

        clock_t start_time = clock();

        /*** Initialization ***/
        BStarTree *best_tree;
        BStarTree *current_tree;
        BStarTree *nbr_tree;
        current_tree = new BStarTree(kComponent_list);
        current_tree->evaluateTotalArea();
        evaluation_count++;
        best_tree = current_tree;
        /******************************/
        // clock_vector_sec.push_back(clock_sec);
        // anytime_behavior_area_by_sec.push_back(kTotalArea / best_tree->area_);
        

        /*** Starts Iteration ***/
        // for(int iter = 0; iter < kIterationTime; iter++){
        // while (clock_sec < kTimeLimit) {
        while (evaluation_count < kEvaluation_limit) {
            
            iter++;

            // if(iter % 1000 == 0){
            //     cout << "Iteration times : " << iter;
            //     cout << "  Area: " << kTotalArea / best_tree->area_ << endl;
            //     cout << "kTotalArea " << kTotalArea << endl;
            // }
            //cout << "Iteration times : " << EXP << " " << iter << endl;

            /*** Get neighbor ***/
            nbr_tree = GetNeighbor(current_tree);
            nbr_tree->evaluateTotalArea();
            evaluation_count++;
            
            /*** Update the best tree ***/
            if(nbr_tree->area_ < best_tree->area_){
                best_tree = nbr_tree;
            }
            
            /*** Caculate the probability of uphill moving ***/
            double temperature = CoolingSchedule(iter, kInitTemp, kAlpha);
            double acpt_prob = AcceptProb(temperature, current_tree->area_, nbr_tree->area_);

            double prob = (double) rand() / (RAND_MAX + 1.0);

            /*** Uphill moving ***/
            if(prob <= acpt_prob){
                current_tree = nbr_tree;
            }

            // clock_t mid_time = clock();
            // timer = double(mid_time - start_time) / CLOCKS_PER_SEC;
            // if (timer >= clock_sec) {
            //     clock_sec += 0.5;
            //     clock_vector_sec.push_back(clock_sec);
            //     anytime_behavior_area_by_sec.push_back(kTotalArea / best_tree->area_);
            // }

            if(evaluation_count == 100) {
                evaluation_record.push_back(evaluation_count);
                anytime_behavior_area.push_back(kTotalArea / best_tree->area_);
                anytime_behavior_total_area.push_back(best_tree->area_);
            }

            if (evaluation_count % 1000 == 0) {
                evaluation_record.push_back(evaluation_count);
                anytime_behavior_area.push_back(kTotalArea / best_tree->area_);
                anytime_behavior_total_area.push_back(best_tree->area_);
                cout << "evaluation_count: " << evaluation_count << " Area Utilization: " << kTotalArea / best_tree->area_ << endl;
            }
            
            //ExpResult << "Fitness" << "," << best_tree->fitness << "\n";
        }
        /******************************/

        clock_t end_time = clock();

        /*** Let output show the best layout ***/
        // best_tree->evaluate(Netlist, M503_pin_location, T501_pin_location);
        // best_tree->evaluateTotalArea();
        // evaluation_count++;
        // for (int i = 0; i < anytime_behavior_area_by_sec.size(); i++) {
        //     ExpResult << clock_vector_sec.at(i) << "," << anytime_behavior_area_by_sec.at(i) << "\n";
        // }
        cout << EXP << " Area: " << kTotalArea / best_tree->area_ << "Total Time: " << double(end_time - start_time) /CLOCKS_PER_SEC << endl;
        ExpResult << "Evaluation" << "," << "Area uti" << "," << "Total Area" << "\n";
        for (int i = 0; i < anytime_behavior_area.size(); i++) {
            ExpResult << evaluation_record.at(i) << "," << anytime_behavior_area.at(i) << "," << anytime_behavior_total_area.at(i) << "\n";
        }

        /*** Reset Exp Parameter ***/
        anytime_behavior_total_area.clear();
        clock_vector_sec.clear();
        anytime_behavior_area.clear();
        evaluation_record.clear();
    }
    /*** Experiments Ends ***/
    
    ExpResult.close();
    return 0;
}