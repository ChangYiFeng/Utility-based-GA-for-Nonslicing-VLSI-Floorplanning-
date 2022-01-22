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
const string kComponentFile = "./n300.csv";
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
const int kEvaluation_limit = 80000;
const double kInitTemp = 1000;
const double kAlpha = 0.85;
const int kExpTime = 30;
// const int kTimeLimit = 70;

/* Fast SA Hyperparameter */
const int kPrerunIteration = 1000;
const int kConstantK = 7;
const int kConstantC = 100;
const double kInitProbability = 0.9999;

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
    vector<double> anytime_behavior_area_by_sec;
    vector<double> clock_vector_sec;
    vector<double> anytime_behavior_area;
    vector<double> evaluation_record;
    /******************************/
    
    /*** Write experiment settings ***/
    ofstream ExpResult("FSAn300exp.csv");
    // ExpResult << "Iteration Time" << "," << kIterationTime << "\n";
    ExpResult << "Initial Temperature" << "," << kInitTemp << "\n";
    ExpResult << "Cooling Schedule" << "," << "exponential multiplicative cooling " << endl;
    ExpResult << "Alpha" << "," << kAlpha << "\n";
    ExpResult <<  "\n";
    ExpResult << "Evaluation" << "," << "Area uti" << "\n";
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
        BStarTree *prerun_tree;
        BStarTree *best_tree;
        BStarTree *current_tree;
        BStarTree *nbr_tree;

        prerun_tree = new BStarTree(kComponent_list);
        current_tree = new BStarTree(kComponent_list);

        current_tree->evaluateTotalArea();
        evaluation_count++;
        best_tree = current_tree;

        double avg_uphill_cost = 0.0;
        vector<double> cost_change_history;
        /******************************/
        // clock_vector_sec.push_back(clock_sec);
        // anytime_behavior_area_by_sec.push_back(kTotalArea / best_tree->area_);

        /*** Prerun ***/

        for(int it = 0; it < kPrerunIteration; it++){
            prerun_tree->evaluateTotalArea();
            evaluation_count++;
            BStarTree *temp_tree = GetNeighbor(prerun_tree);
            temp_tree->evaluateTotalArea();
            evaluation_count++;

            double cost_change = (temp_tree->area_ - prerun_tree->area_) / 37585;

            if(cost_change > 0){
                cost_change_history.push_back(cost_change);
            }
            prerun_tree = GetNeighbor(prerun_tree);
            
            if (evaluation_count % 300 == 0) {
                ExpResult << evaluation_count << "," << kTotalArea / best_tree->area_ << "\n";
                // evaluation_record.push_back(evaluation_count);
                // anytime_behavior_area.push_back(kTotalArea / best_tree->area_);
                cout << "evaluation_count: " << evaluation_count << " Area Utilization: " << kTotalArea / best_tree->area_ << endl;
            }
        }

        avg_uphill_cost = CalculateMean(cost_change_history);
        cout << avg_uphill_cost << endl;
        cost_change_history.clear();
        ExpResult << evaluation_count << "," << kTotalArea / best_tree->area_ << "\n";
        cout << "best area " << best_tree->area_ << endl;
        

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

            /*** Calculate normalized cost change ***/
            double cost_change = (nbr_tree->area_ - current_tree->area_) / 330000;
            cost_change_history.push_back(cost_change);
            cost_change = CalculateMean(cost_change_history);
            
            /*** Update the best tree ***/
            if(nbr_tree->area_ < best_tree->area_)
                best_tree = nbr_tree;
            
            /*** Caculate the probability of uphill moving ***/
            double temperature = FastSACoolingSchedule(iter, kInitProbability, kConstantC, kConstantK, avg_uphill_cost, cost_change);
            double acpt_prob = AcceptProb(temperature, current_tree->area_ / 330000, nbr_tree->area_ / 330000);

            double prob = (double) rand() / (RAND_MAX + 1.0);

            /*** Uphill moving ***/
            if(prob <= acpt_prob)
                current_tree = nbr_tree;
            

            // clock_t mid_time = clock();
            // timer = double(mid_time - start_time) / CLOCKS_PER_SEC;
            // if (timer >= clock_sec) {
            //     clock_sec += 0.5;
            //     clock_vector_sec.push_back(clock_sec);
            //     anytime_behavior_area_by_sec.push_back(kTotalArea / best_tree->area_);
            // }

            if (evaluation_count % 300 == 0) {
                ExpResult << evaluation_count << "," << kTotalArea / best_tree->area_ << "\n";
                // evaluation_record.push_back(evaluation_count);
                // anytime_behavior_area.push_back(kTotalArea / best_tree->area_);
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
        // ExpResult << "Evaluation" << "," << "Area uti" << "\n";
        // for (int i = 0; i < anytime_behavior_area.size(); i++) {
        //     ExpResult << evaluation_record.at(i) << "," << anytime_behavior_area.at(i) << "\n";
        // }

        /*** Reset Exp Parameter ***/
        anytime_behavior_area_by_sec.clear();
        clock_vector_sec.clear();
        anytime_behavior_area.clear();
        evaluation_record.clear();
    }
    /*** Experiments Ends ***/
    
    ExpResult.close();
    return 0;
}