#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <tuple>
#include <string>
#include <map>
#include <fstream>
#include "GlobalVariable.h"
#include "Utility.h"
#include "Structure.h"
using namespace std;

/* Exp Settings */
const string kSeedFile = "./seed.txt";
const string kComponentFile = "./n300.csv";
const bool kSingleDoubleSided = 0;  // 0 for Single-Sided; 1 for Double-Sided
const bool kReadSeed = 0;   // 0 for random seed; 1 for read seed from txt
const double kmargin = 0;   // 0 for Benchmark exp.

/* counter */
int kGeneration_count;
int kEvaluation_count;
double kTimer;
double kClock_sec;

// anytime behavior
vector<double> anytime_behavior_area;
vector<double> anytime_behavior_WL;
// vector<double> anytime_behavior_area_by_sec;
vector<double> evaluation_record;

// Success rate experiment
vector<double> left_subtree_crossover_SuccessRate;
vector<double> ordered_crossover_SuccessRate;
vector<double> subtree_exchange_crossover_SuccessRate;
vector<double> swap_node_mutation_SuccessRate;
vector<double> insert_mutation_SuccessRate;
vector<double> bitwise_mutation_SuccessRate;
vector<double> scramble_mutation_SuccessRate;
vector<double> swap_branch_mutation_SuccessRate;
vector<double> shift_subtree_mutation_SuccessRate;
vector<double> swap_subtree_SuccessRate;

// Survival rate experiment
double LeftSubtreeCrossover_Survive = 0;
double OrderedCrossover_Survive = 0;
double SubtreeExchangeCrossover_Survive = 0;
double swapNodeMutation_Survive = 0;
double insertMutation_Survive = 0;
double bitwiseMutation_Survive = 0;
double scrambleMutation_Survive = 0;
double swapBranchMutation_Survive = 0;
double shiftSubtreeMutation_Survive = 0;
double swapSubtreeMutation_sameLevel_Survive = 0;
double swapSubtreeMutation_diffLevel_Survive = 0;
double shuffleSubtreeMutation_sameLevel_Survive = 0;

vector<double> LeftSubtreeCrossover_SurvivalRate;
vector<double> OrderedCrossover_SurvivalRate;
vector<double> SubtreeExchangeCrossover_SurvivalRate;
vector<double> swapNodeMutation_SurvivalRate;
vector<double> insertMutation_SurvivalRate;
vector<double> bitwiseMutation_SurvivalRate;
vector<double> scrambleMutation_SurvivalRate;
vector<double> swapBranchMutation_SurvivalRate;
vector<double> shiftSubtreeMutation_SurvivalRate;
vector<double> swapSubtreeMutation_SurvivalRate;
/******************************/

/* SuccessRate Exp Parameter */
double kCrossover_Count = 0;
double kMutation_Count = 0;
double kLeftSubtreeCrossover_Count = 0;
double kOrderedCrossover_Count = 0;
double kSubtreeExchangeCrossover_Count = 0;
double kSwapNode_Count = 0;
double kInsert_Count = 0;
double kBitwise_Count = 0;
double kScramble_Count = 0;
double kSwapBranch_Count = 0;
double kShiftSubtree_Count = 0;
double kSwapSubtree_Count = 0;

double kLeftSubtreeCrossover_SuccessCount = 0;
double kOrderedCrossover_SuccessCount = 0;
double kSubtreeExchangeCrossover_SuccessCount = 0;
double kSwapNode_SuccessCount = 0;
double kInsert_SuccessCount = 0;
double kBitwise_SuccessCount = 0;
double kScramble_SuccessCount = 0;
double kSwapBranch_SuccessCount = 0;
double kShiftSubtree_SuccessCount = 0;
double kSwapSubtree_SuccessCount = 0;

/* EC Hyperparameter */
// const double kTargetUtil = 0.95;
const int kExpTime = 30;
const int kGeneration = 800;
const int kPopSize = 100;
double kProbCrossover = 0.4;
const int kCrossoverTime = kPopSize / 2;
double kProbMutation = 0.9;
const double kProbAngleMutation = 0.5;
const double kProbBitFlip = 0.3;
const int k_kTournament = 5;


/* main function */
int main(int argc, char *argv[]) {

    /*** Random Seed Setting ***/
    if(kReadSeed){
        string temp;
        ifstream SeedRecord(kSeedFile);
        getline(SeedRecord, temp, ',');
        getline(SeedRecord, temp, '\n');
        int seed = stoi(temp);
        cout << "seed = " << seed << endl;
        srand(seed);
    }
    else {
        int seed;
        ofstream SeedRecord("seed.txt");
        srand(seed = (unsigned)time(0));
        SeedRecord << "Seed" << "," << seed;
        SeedRecord.close(); 
    }

    /*** Declaration ***/
    const int kComponentNum = GetTotalAmount(kComponentFile);   // REMEMBER TO CHECK HOW MANY HEADER ROWS: MCNC 1, AcBel 4.
    vector<Component> kComponent_list= ReadDim(kComponentFile);                // stores all component info: ID, W, L, H
    const double kTotalArea = GetTotalArea(kComponentFile);    // REMEMBER TO CHECK HOW MANY HEADER ROWS: MCNC 1, AcBel 4. (GetTotalAmount, GetTotalArea)

    /*** Write experiment settings ***/
    ofstream SuccessRExpResult("OpSR_n300_10Success-count.csv");
    SuccessRExpResult << "Total Generation" << "," << kGeneration << "\n";
    SuccessRExpResult << "Population Size" << "," << kPopSize << "\n";
    SuccessRExpResult << "Crossover Rate" << "," << kProbCrossover << "\n";
    SuccessRExpResult << "Mutation Rate" << "," << kProbMutation << "\n";
    SuccessRExpResult << "Angle Mutation Rate" << "," << kProbAngleMutation << "\n";
    SuccessRExpResult << "Angle Bit Flip Rate" << "," << kProbBitFlip << "\n";
    SuccessRExpResult << "k Tournament Selection" << "," << k_kTournament << "\n";

    // ofstream SurvivalExpResult("OpSR_n300_Survival.csv");
    // SurvivalExpResult << "Total Generation" << "," << kGeneration << "\n";
    // SurvivalExpResult << "Population Size" << "," << kPopSize << "\n";
    // SurvivalExpResult << "Crossover Rate" << "," << kProbCrossover << "\n";
    // SurvivalExpResult << "Mutation Rate" << "," << kProbMutation << "\n";
    // SurvivalExpResult << "Angle Mutation Rate" << "," << kProbAngleMutation << "\n";
    // SurvivalExpResult << "Angle Bit Flip Rate" << "," << kProbBitFlip << "\n";
    // SurvivalExpResult << "k Tournament Selection" << "," << k_kTournament << "\n";
    /******************************/

    /*** Experiment Starts ***/
    for (int Exp = 0; Exp < kExpTime; Exp++) {
        // Initialize counters
        kGeneration_count = 0;
        kEvaluation_count = 0;
        kTimer = 0;
        kClock_sec = 0;

        clock_t start_time = clock();
        cout << "iteration: " << Exp << endl;
        vector<BStarTree*> population;
        vector<BStarTree*> offspring;
        vector<BStarTree*> new_offspring;

        /*** Initialization ***/
        for (int i = 0; i < kPopSize; i++) {
            BStarTree *tree = new BStarTree(kComponent_list);
            tree->evaluateTotalArea();
            kEvaluation_count++;
            population.push_back(tree);
            population.back()->isChild_ = false;
            population.back()->Xtype_ = 99;
            population.back()->Mtype_ = 99;
        }
        sort(population.begin(), population.end(), SortPopArea);    // SortPopArea / SortPopWL / SortPop
        anytime_behavior_area.push_back(kTotalArea / population.front()->area_);
        // anytime_behavior_WL.push_back(population.front()->wire_length_);
        // evaluation_record.push_back(kEvaluation_count);
        // clock_vector_sec.push_back(kClock_sec);
        // anytime_behavior_area_by_sec.push_back(kTotalArea / population.front()->area_);

        /*** Exp Termination Criteria ***/    // Stop by Generation setting/ Area uti/ Run time
        // for (int gen = 0; gen < kGeneration; gen++) {
        // while (kTotalArea / population.front()->area_ < kTargetUtil && kGeneration_count < kGeneration && kClock_sec < 40) {
        while (kGeneration_count < kGeneration) {

            kGeneration_count++;
            offspring.clear();
            new_offspring.clear();

            /* CROSSOVER */
            for (int time = 0; time < kCrossoverTime; time++){      // control lambda (lambda= 2 * crossover_times)
                new_offspring = Crossover(population, kComponentNum);
                offspring.push_back(new_offspring.at(0));
                offspring.push_back(new_offspring.at(1));
                new_offspring.clear();
            }

            /* MUTATION */
            offspring = Mutation(offspring, kComponentNum);

            /* EVALUATE and SURVIVOR SELECTION */
            for (int i = 0; i < offspring.size(); i++) {
                offspring.at(i)->evaluateTotalArea();
                kEvaluation_count++;
                offspring.at(i)->isChild_ = true;
                population.push_back(offspring.at(i));
            }
            sort(population.begin(), population.end(), SortPopArea);    // SortPopArea / SortPopWL / SortPop
            population.erase(population.begin()+kPopSize, population.end());

            /*** Operator function test ***/
            // population.front()->printTree();
            // population.push_back(population.at(0)->orderedCrossover(population.at(1), kComponentNum));
            // population.back()->printTree();
            /******************************/

            /*** Survival rate ***/
            // for (int i = 0; i < population.size(); i++) {
            //     if (population.at(i)->isChild_) {
            //         switch (population.at(i)->Xtype_)
            //         {
            //         case 0:
            //             LeftSubtreeCrossover_Survive += 1;
            //             break;
                    
            //         case 1:
            //             OrderedCrossover_Survive += 1;
            //             break;
                    
            //         case 2:
            //             SubtreeExchangeCrossover_Survive += 1;
            //             break;
            //         }

            //         switch (population.at(i)->Mtype_)
            //         {
            //         case 0:
            //             swapNodeMutation_Survive += 1;
            //             break;
            //         case 1:
            //             insertMutation_Survive += 1;
            //             break;
            //         case 2:
            //             bitwiseMutation_Survive += 1;
            //             break;
            //         case 3:
            //             scrambleMutation_Survive += 1;
            //             break;
            //         case 4:
            //             swapBranchMutation_Survive += 1;
            //             break;
            //         case 5:
            //             shiftSubtreeMutation_Survive += 1;
            //             break;
            //         case 6:
            //             swapSubtreeMutation_sameLevel_Survive += 1;
            //             break;
            //         case 7:
            //             swapSubtreeMutation_diffLevel_Survive += 1;
            //             break;
            //         case 8:
            //             shuffleSubtreeMutation_sameLevel_Survive += 1;
            //             break;
            //         }
            //     }
            //     population.at(i)->isChild_ = false;
            // }
            // anytime_behavior_area.push_back(kTotalArea / population.front()->area_);
            // anytime_behavior_WL.push_back(population.front()->wire_length_);
            // LeftSubtreeCrossover_SurvivalRate.push_back(LeftSubtreeCrossover_Survive / kLeftSubtreeCrossover_Count);
            // OrderedCrossover_SurvivalRate.push_back(OrderedCrossover_Survive / kOrderedCrossover_Count);
            // SubtreeExchangeCrossover_SurvivalRate.push_back(SubtreeExchangeCrossover_Survive / kSubtreeExchangeCrossover_Count);
            // swapNodeMutation_SurvivalRate.push_back(swapNodeMutation_Survive / kSwapNode_Count);
            // insertMutation_SurvivalRate.push_back(insertMutation_Survive / kInsert_Count);
            // bitwiseMutation_SurvivalRate.push_back(bitwiseMutation_Survive / kBitwise_Count);
            // scrambleMutation_SurvivalRate.push_back(scrambleMutation_Survive / kScramble_Count);
            // swapBranchMutation_SurvivalRate.push_back(swapBranchMutation_Survive / kSwapBranch_Count);
            // shiftSubtreeMutation_SurvivalRate.push_back(shiftSubtreeMutation_Survive / kShiftSubtree_Count);
            // swapSubtreeMutation_sameLevel_SurvivalRate.push_back(swapSubtreeMutation_Survive / kSwapSubtree_Count);
            
            // kLeftSubtreeCrossover_Count = 0;
            // kOrderedCrossover_Count = 0;
            // kSubtreeExchangeCrossover_Count = 0;
            // kSwapNode_Count = 0;
            // kInsert_Count = 0;
            // kBitwise_Count = 0;
            // kScramble_Count = 0;
            // kSwapBranch_Count = 0;
            // kShiftSubtree_Count = 0;
            // kSwapSubtree_Count = 0;

            // LeftSubtreeCrossover_Survive = 0;
            // OrderedCrossover_Survive = 0;
            // SubtreeExchangeCrossover_Survive = 0;
            // swapNodeMutation_Survive = 0;
            // insertMutation_Survive = 0;
            // bitwiseMutation_Survive = 0;
            // scrambleMutation_Survive = 0;
            // swapBranchMutation_Survive = 0;
            // shiftSubtreeMutation_Survive = 0;
            // swapSubtreeMutation_sameLevel_Survive = 0;
            // swapSubtreeMutation_diffLevel_Survive = 0;
            // shuffleSubtreeMutation_sameLevel_Survive = 0;
            /******************************/

            /*** Evaluation Process Monitoring and Animation Making ***/
            // population.front()->evaluateTotalArea();
            // cout << Exp << " Generation: " << kGeneration_count << endl;
            // cout << Exp <<  " Evaluation times: " << kEvaluation_count << endl;
            // cout << "Best Area: " << population.front()->area_ << endl;
            // cout << Exp <<  " Area Utilization: " << kTotalArea / population.front()->area_ << endl;
            // cout << "Best Wire Length: " << population.front()->wire_length_ << endl;
            // clock_t mid_time = clock();
            // kTimer = double(mid_time - start_time) / CLOCKS_PER_SEC;
            // if (kTimer >= kClock_sec) {
            //     kClock_sec += 0.5;
            //     clock_vector_sec.push_back(kClock_sec);
            //     anytime_behavior_area_by_sec.push_back(kTotalArea / population.front()->area_);
            // }
            // if (kEvaluation_count % 100 == 0) {
            //     evaluation_record.push_back(kEvaluation_count);
            //     anytime_behavior_area.push_back(kTotalArea / population.front()->area_);
            //     cout << Exp << " Generation: " << kGeneration_count <<  " Area Utilization: " << kTotalArea / population.front()->area_ << endl;
            // }

            /*** Success rate & Survival rate EXP ***/
            if (kGeneration_count % 10 == 0) {
                anytime_behavior_area.push_back(kTotalArea / population.front()->area_);
                left_subtree_crossover_SuccessRate.push_back(kLeftSubtreeCrossover_SuccessCount / kCrossover_Count);
                ordered_crossover_SuccessRate.push_back(kOrderedCrossover_SuccessCount / kCrossover_Count);
                subtree_exchange_crossover_SuccessRate.push_back(kSubtreeExchangeCrossover_SuccessCount / kCrossover_Count);
                swap_node_mutation_SuccessRate.push_back(kSwapNode_SuccessCount / kMutation_Count);
                insert_mutation_SuccessRate.push_back(kInsert_SuccessCount / kMutation_Count);
                bitwise_mutation_SuccessRate.push_back(kBitwise_SuccessCount / kMutation_Count);
                scramble_mutation_SuccessRate.push_back(kScramble_SuccessCount / kMutation_Count);
                swap_branch_mutation_SuccessRate.push_back(kSwapBranch_SuccessCount / kMutation_Count);
                shift_subtree_mutation_SuccessRate.push_back(kShiftSubtree_SuccessCount / kMutation_Count);
                swap_subtree_SuccessRate.push_back(kSwapSubtree_SuccessCount / kMutation_Count);

                kCrossover_Count = 0;
                kMutation_Count = 0;

                kLeftSubtreeCrossover_SuccessCount = 0;
                kOrderedCrossover_SuccessCount = 0;
                kSubtreeExchangeCrossover_SuccessCount = 0;
                kSwapNode_SuccessCount = 0;
                kInsert_SuccessCount = 0;
                kBitwise_SuccessCount = 0;
                kScramble_SuccessCount = 0;
                kSwapBranch_SuccessCount = 0;
                kShiftSubtree_SuccessCount = 0;
                kSwapSubtree_SuccessCount = 0;
            }
            /******************************/
        }
        clock_t end_time = clock();
        // Check result after one run
        // cout << Exp <<  " Total Time: " << double(end_time - start_time) / CLOCKS_PER_SEC << endl;
        // cout << Exp << " Generation: " << kGeneration_count << endl;
        // cout << Exp <<  " Evaluation times: " << kEvaluation_count << endl;
        // cout << " Best Area: " << population.front()->area_ << endl;
        // cout << Exp <<  " Area Utilization: " << kTotalArea / population.front()->area_ << endl;

        /*** Write Exp results to csv ***/
        SuccessRExpResult << "Total Time" << "," << double(end_time - start_time) / CLOCKS_PER_SEC << "\n";
        SuccessRExpResult << "Total Generation" << "," << kGeneration_count << "\n";
        // SurvivalExpResult << "Total Time" << "," << double(end_time - start_time) / CLOCKS_PER_SEC << "\n";
        // SurvivalExpResult << "Total Generation" << "," << kGeneration_count << "\n";

        // ExpResult << "Evaluation" << "," << "Area uti" << "\n";
        // for (int i = 0; i < anytime_behavior_area.size(); i++) {
        //     ExpResult << evaluation_record.at(i) << "," << anytime_behavior_area.at(i) << "\n";
        // }
        // ExpResult << "Time" << "," << "Area" << "\n";
        // for (int i = 0; i < anytime_behavior_area_by_sec.size(); i++) {
        //     ExpResult << clock_vector_sec.at(i) << "," << anytime_behavior_area_by_sec.at(i) << "\n";
        // }
        // ExpResult << "\n";

        SuccessRExpResult << "left_subtree_crossover_SuccessRate" << "," << "ordered_crossover_SuccessRate" << "," << "subtree_exchange_crossover_SuccessRate" << "," << "swap_node_mutation_SuccessRate" << "," << "insert_mutation_SuccessRate" << "," << "bitwise_mutation_SuccessRate" << "," << "scramble_mutation_SuccessRate" << "," << "swap_branch_mutation_SuccessRate" << "," << "shift_subtree_mutation_SuccessRate" << "," << "swap_subtree_SuccessRate" << "," << "Area" << "\n";
        for (int i = 0; i < left_subtree_crossover_SuccessRate.size(); i++) {
            SuccessRExpResult << left_subtree_crossover_SuccessRate.at(i) << "," << ordered_crossover_SuccessRate.at(i) << "," << subtree_exchange_crossover_SuccessRate.at(i) << "," << swap_node_mutation_SuccessRate.at(i) << "," << insert_mutation_SuccessRate.at(i) << "," << bitwise_mutation_SuccessRate.at(i) << "," << scramble_mutation_SuccessRate.at(i) << "," << swap_branch_mutation_SuccessRate.at(i) << "," << shift_subtree_mutation_SuccessRate.at(i) << "," << swap_subtree_SuccessRate.at(i) << "," << anytime_behavior_area.at(i) << "\n";
        }
        // SurvivalExpResult << "LeftSubtreeCrossover_SurvivalRate" << "," << "OrderedCrossover_SurvivalRate" << "," << "SubtreeExchangeCrossover_SurvivalRate" << "," << "swapNodeMutation_SurvivalRate" << "," << "insertMutation_SurvivalRate" << "," << "bitwiseMutation_SurvivalRate" << "," << "scrambleMutation_SurvivalRate" << "," << "swapBranchMutation_SurvivalRate" << "," << "shiftSubtreeMutation_SurvivalRate" << "," << "swapSubtreeMutation_sameLevel_SurvivalRate" << "," << "swapSubtreeMutation_diffLevel_SurvivalRate" << "," << "shuffleSubtreeMutation_sameLevel_SurvivalRate" << "," << "Area" << "\n";
        // for (int i = 0; i < LeftSubtreeCrossover_SurvivalRate.size(); i++) {
        //     SurvivalExpResult << LeftSubtreeCrossover_SurvivalRate.at(i) << "," << OrderedCrossover_SurvivalRate.at(i) << "," << SubtreeExchangeCrossover_SurvivalRate.at(i) << "," << swapNodeMutation_SurvivalRate.at(i) << "," << insertMutation_SurvivalRate.at(i) << "," << bitwiseMutation_SurvivalRate.at(i) << "," << scrambleMutation_SurvivalRate.at(i) << "," << swapBranchMutation_SurvivalRate.at(i) << "," << shiftSubtreeMutation_SurvivalRate.at(i) << "," << swapSubtreeMutation_sameLevel_SurvivalRate.at(i) << "," << swapSubtreeMutation_diffLevel_SurvivalRate.at(i) << "," << shuffleSubtreeMutation_sameLevel_SurvivalRate.at(i) << "," << anytime_behavior_area.at(i) << "\n";
        // }
        /******************************/

        /*** Print Anytime Behavior ***/
        // cout << "Anytime Behavior: " << endl;
        // for (int i = 0; i < generation_count; i++)
        // {
        //     cout << "Gen " << i << " Area: " << anytime_behavior_area.at(i) << endl;
        //     cout << "Gen " << i << " WL: " << anytime_behavior_WL.at(i) << endl;
        // }
        // cout << endl << endl;

        /*** Reset Exp Parameter ***/
        anytime_behavior_area.clear();
        evaluation_record.clear();
        // anytime_behavior_WL.clear();
        population.clear();
        offspring.clear();
        new_offspring.clear();
        // anytime_behavior_area_by_sec.clear();

        LeftSubtreeCrossover_SurvivalRate.clear();
        OrderedCrossover_SurvivalRate.clear();
        SubtreeExchangeCrossover_SurvivalRate.clear();
        swapNodeMutation_SurvivalRate.clear();
        insertMutation_SurvivalRate.clear();
        bitwiseMutation_SurvivalRate.clear();
        scrambleMutation_SurvivalRate.clear();
        swapBranchMutation_SurvivalRate.clear();
        shiftSubtreeMutation_SurvivalRate.clear();
        swapSubtreeMutation_SurvivalRate.clear();

        left_subtree_crossover_SuccessRate.clear();
        ordered_crossover_SuccessRate.clear();
        subtree_exchange_crossover_SuccessRate.clear();
        swap_node_mutation_SuccessRate.clear();
        insert_mutation_SuccessRate.clear();
        bitwise_mutation_SuccessRate.clear();
        scramble_mutation_SuccessRate.clear();
        swap_branch_mutation_SuccessRate.clear();
        shift_subtree_mutation_SuccessRate.clear();
        swap_subtree_SuccessRate.clear();
        
        /*** End of one single run ***/
    }
    /*** Experiments Ends ***/

    SuccessRExpResult.close();
    // SurvivalExpResult.close();
    return 0;
}