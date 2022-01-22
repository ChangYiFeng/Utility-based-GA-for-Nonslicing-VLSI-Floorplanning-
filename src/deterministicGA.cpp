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
#include "dtmUtility.h"
#include "dtmStructure.h"
using namespace std;

/* Exp Settings */
const string kSeedFile = "./seed.txt";
const string kComponentFile = "./ami33.csv";
const string X_prob_filename = "./ami49_X_prob.csv";
const string M_prob_filename = "./ami49_M_prob.csv";
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
vector<double> anytime_behavior_total_area;
vector<double> evaluation_record;

/* EC Hyperparameter */
// const double kTargetUtil = 0.95;
const int kExpTime = 10;
const int kGeneration = 1000;
const int kPopSize = 100;
const double kProbAngleMutation = 0.5;
const double kProbBitFlip = 0.3;
const int k_kTournament = 5;
double kProbCrossover;
const int kCrossoverTime = kPopSize / 2;
double kProbMutation;
int Xrand, Mrand;


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
    constexpr int FLOAT_MIN = 0;
    constexpr int FLOAT_MAX = 1;
    vector<vector<double> > X_prob = ReadProb(X_prob_filename);
    vector<vector<double> > M_prob = ReadProb(M_prob_filename);

    /*** Write experiment settings ***/
    // ofstream ExpResult("OpSR_ami49_deterministic_1000gen.csv");
    ofstream ExpResult("ami33_ga_1000gen1.csv");
    ExpResult << "Total Generation" << "," << kGeneration << "\n";
    ExpResult << "Population Size" << "," << kPopSize << "\n";
    ExpResult << "Angle Mutation Rate" << "," << kProbAngleMutation << "\n";
    ExpResult << "Angle Bit Flip Rate" << "," << kProbBitFlip << "\n";
    ExpResult << "k Tournament Selection" << "," << k_kTournament << "\n";
    /******************************/

    /*** Experiment Starts ***/
    for (int Exp = 0; Exp < kExpTime; Exp++) {

        // Initialize counters
        kGeneration_count = 0;

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
        }
        sort(population.begin(), population.end(), SortPopArea);    // SortPopArea / SortPopWL / SortPop
        evaluation_record.push_back(kEvaluation_count);
        anytime_behavior_area.push_back(kTotalArea / population.front()->area_);
        anytime_behavior_total_area.push_back(population.front()->area_);
        int crossover_type;
        int mutation_type;
        float RAND;
        float XRoulette1, XRoulette2;
        float MRoulette1, MRoulette2, MRoulette3, MRoulette4, MRoulette5, MRoulette6; 

        /*** Exp Termination Criteria ***/    // Stop by Generation setting/ Area uti/ Run time
        // for (int gen = 0; gen < kGeneration; gen++) {
        // while (kTotalArea / population.front()->area_ < kTargetUtil && kGeneration_count < kGeneration && kClock_sec < 40) {
        while (kGeneration_count < kGeneration) {

            kGeneration_count++;
            offspring.clear();
            new_offspring.clear();

            if (kGeneration_count <= 10000) {
                kProbCrossover = 0.5;
                kProbMutation = 0.9;

                /* CROSSOVER */
                for (int time = 0; time < kCrossoverTime; time++){      // control lambda (lambda= 2 * crossover_times)
                    crossover_type = rand() % 3;
                    // RAND = FLOAT_MIN + (float)(rand()) / ((float)(RAND_MAX/(FLOAT_MAX - FLOAT_MIN)));
                    // XRoulette1 = X_prob.at(0).at(0);
                    // XRoulette2 = XRoulette1 + X_prob.at(0).at(1);
                    // if (RAND < XRoulette1)
                    //     crossover_type = 0;
                    // else if (RAND < XRoulette2)
                    //     crossover_type = 1;
                    // else
                    //     crossover_type = 2;
                    
                    // cout << "RAND: " << RAND << "  XRoulette1: " << XRoulette1 << "  XRoulette2: " << XRoulette2 << "  crossover_type: " << crossover_type << endl;

                    new_offspring = Crossover(population, kComponentNum, crossover_type);
                    offspring.push_back(new_offspring.at(0));
                    offspring.push_back(new_offspring.at(1));
                    new_offspring.clear();
                }
                /* MUTATION */
                for (int child = 0; child < offspring.size(); child++) {
                    mutation_type = rand() % 9;
                    // RAND = FLOAT_MIN + (float)(rand()) / ((float)(RAND_MAX/(FLOAT_MAX - FLOAT_MIN)));
                    // MRoulette1 = M_prob.at(0).at(0);
                    // MRoulette2 = MRoulette1 + M_prob.at(0).at(1);
                    // MRoulette3 = MRoulette2 + M_prob.at(0).at(2);
                    // MRoulette4 = MRoulette3 + M_prob.at(0).at(3);
                    // MRoulette5 = MRoulette4 + M_prob.at(0).at(4);
                    // MRoulette6 = MRoulette5 + M_prob.at(0).at(5);
                    // if (RAND < MRoulette1)
                    //     mutation_type = 0;
                    // else if (RAND < MRoulette2)
                    //     mutation_type = 1;
                    // else if (RAND < MRoulette3)
                    //     mutation_type = 2;
                    // else if (RAND < MRoulette4)
                    //     mutation_type = 3;
                    // else if (RAND < MRoulette5)
                    //     mutation_type = 4;
                    // else if (RAND < MRoulette6)
                    //     mutation_type = 5;
                    // else
                    //     mutation_type = 6;

                    offspring.at(child) = Mutation(offspring.at(child), mutation_type);
                }
            }

            else if (kGeneration_count > 100 && kGeneration_count <= 200) {
                kProbCrossover = 0.4;
                kProbMutation = 0.9;
                
                /* CROSSOVER */
                for (int time = 0; time < kCrossoverTime; time++){      // control lambda (lambda= 2 * crossover_times)
                    // Xrand = rand() % 7; // LeftSubtree:Ordered:SubtreeExchange = 1:2:3
                    // if (Xrand == 0)
                    //     crossover_type = 0;
                    // else if (Xrand == 1 || Xrand == 2 || Xrand == 3 || Xrand == 4)
                    //     crossover_type = 1;
                    // else
                    //     crossover_type = 2;
                    RAND = FLOAT_MIN + (float)(rand()) / ((float)(RAND_MAX/(FLOAT_MAX - FLOAT_MIN)));
                    XRoulette1 = X_prob.at(1).at(0);
                    XRoulette2 = XRoulette1 + X_prob.at(1).at(1);
                    if (RAND < XRoulette1)
                        crossover_type = 0;
                    else if (RAND < XRoulette2)
                        crossover_type = 1;
                    else
                        crossover_type = 2;
                    
                    new_offspring = Crossover(population, kComponentNum, crossover_type);
                    offspring.push_back(new_offspring.at(0));
                    offspring.push_back(new_offspring.at(1));
                    new_offspring.clear();
                }
                /* MUTATION */
                for (int child = 0; child < offspring.size(); child++) {
                    // Mrand = rand() % 9;
                    // switch (Mrand)
                    // {
                    // case 0: case 1:
                    //     mutation_type = 0;
                    //     break;
                    // case 2: case 3:
                    //     mutation_type = 1;
                    //     break;
                    // case 4:
                    //     mutation_type = 2;
                    //     break;
                    // case 5:
                    //     mutation_type = 3;
                    //     break;
                    // case 6:
                    //     mutation_type = 4;
                    //     break;
                    // case 7:
                    //     mutation_type = 5;
                    //     break;
                    // case 8:
                    //     mutation_type = 6;
                    //     break;
                    // }
                    RAND = FLOAT_MIN + (float)(rand()) / ((float)(RAND_MAX/(FLOAT_MAX - FLOAT_MIN)));
                    MRoulette1 = M_prob.at(1).at(0);
                    MRoulette2 = MRoulette1 + M_prob.at(1).at(1);
                    MRoulette3 = MRoulette2 + M_prob.at(1).at(2);
                    MRoulette4 = MRoulette3 + M_prob.at(1).at(3);
                    MRoulette5 = MRoulette4 + M_prob.at(1).at(4);
                    MRoulette6 = MRoulette5 + M_prob.at(1).at(5);
                    if (RAND < MRoulette1)
                        mutation_type = 0;
                    else if (RAND < MRoulette2)
                        mutation_type = 1;
                    else if (RAND < MRoulette3)
                        mutation_type = 2;
                    else if (RAND < MRoulette4)
                        mutation_type = 3;
                    else if (RAND < MRoulette5)
                        mutation_type = 4;
                    else if (RAND < MRoulette6)
                        mutation_type = 5;
                    else
                        mutation_type = 6;

                    offspring.at(child) = Mutation(offspring.at(child), mutation_type);
                }
            }

            else if (kGeneration_count > 200 && kGeneration_count <= 300) {
                kProbCrossover = 0.4;
                kProbMutation = 0.9;
                
                /* CROSSOVER */
                for (int time = 0; time < kCrossoverTime; time++){      // control lambda (lambda= 2 * crossover_times)
                    RAND = FLOAT_MIN + (float)(rand()) / ((float)(RAND_MAX/(FLOAT_MAX - FLOAT_MIN)));
                    XRoulette1 = X_prob.at(2).at(0);
                    XRoulette2 = XRoulette1 + X_prob.at(2).at(1);
                    if (RAND < XRoulette1)
                        crossover_type = 0;
                    else if (RAND < XRoulette2)
                        crossover_type = 1;
                    else
                        crossover_type = 2;
                    
                    new_offspring = Crossover(population, kComponentNum, crossover_type);
                    offspring.push_back(new_offspring.at(0));
                    offspring.push_back(new_offspring.at(1));
                    new_offspring.clear();
                }
                /* MUTATION */
                for (int child = 0; child < offspring.size(); child++) {
                    RAND = FLOAT_MIN + (float)(rand()) / ((float)(RAND_MAX/(FLOAT_MAX - FLOAT_MIN)));
                    MRoulette1 = M_prob.at(2).at(0);
                    MRoulette2 = MRoulette1 + M_prob.at(2).at(1);
                    MRoulette3 = MRoulette2 + M_prob.at(2).at(2);
                    MRoulette4 = MRoulette3 + M_prob.at(2).at(3);
                    MRoulette5 = MRoulette4 + M_prob.at(2).at(4);
                    MRoulette6 = MRoulette5 + M_prob.at(2).at(5);
                    if (RAND < MRoulette1)
                        mutation_type = 0;
                    else if (RAND < MRoulette2)
                        mutation_type = 1;
                    else if (RAND < MRoulette3)
                        mutation_type = 2;
                    else if (RAND < MRoulette4)
                        mutation_type = 3;
                    else if (RAND < MRoulette5)
                        mutation_type = 4;
                    else if (RAND < MRoulette6)
                        mutation_type = 5;
                    else
                        mutation_type = 6;

                    offspring.at(child) = Mutation(offspring.at(child), mutation_type);
                }
            }

            else if (kGeneration_count > 300 && kGeneration_count <= 400) {
                kProbCrossover = 0.3;
                kProbMutation = 0.95;
                
                /* CROSSOVER */
                for (int time = 0; time < kCrossoverTime; time++){      // control lambda (lambda= 2 * crossover_times)
                    RAND = FLOAT_MIN + (float)(rand()) / ((float)(RAND_MAX/(FLOAT_MAX - FLOAT_MIN)));
                    XRoulette1 = X_prob.at(3).at(0);
                    XRoulette2 = XRoulette1 + X_prob.at(3).at(1);
                    if (RAND < XRoulette1)
                        crossover_type = 0;
                    else if (RAND < XRoulette2)
                        crossover_type = 1;
                    else
                        crossover_type = 2;
                    
                    new_offspring = Crossover(population, kComponentNum, crossover_type);
                    offspring.push_back(new_offspring.at(0));
                    offspring.push_back(new_offspring.at(1));
                    new_offspring.clear();
                }
                /* MUTATION */
                for (int child = 0; child < offspring.size(); child++) {
                    RAND = FLOAT_MIN + (float)(rand()) / ((float)(RAND_MAX/(FLOAT_MAX - FLOAT_MIN)));
                    MRoulette1 = M_prob.at(3).at(0);
                    MRoulette2 = MRoulette1 + M_prob.at(3).at(1);
                    MRoulette3 = MRoulette2 + M_prob.at(3).at(2);
                    MRoulette4 = MRoulette3 + M_prob.at(3).at(3);
                    MRoulette5 = MRoulette4 + M_prob.at(3).at(4);
                    MRoulette6 = MRoulette5 + M_prob.at(3).at(5);
                    if (RAND < MRoulette1)
                        mutation_type = 0;
                    else if (RAND < MRoulette2)
                        mutation_type = 1;
                    else if (RAND < MRoulette3)
                        mutation_type = 2;
                    else if (RAND < MRoulette4)
                        mutation_type = 3;
                    else if (RAND < MRoulette5)
                        mutation_type = 4;
                    else if (RAND < MRoulette6)
                        mutation_type = 5;
                    else
                        mutation_type = 6;

                    offspring.at(child) = Mutation(offspring.at(child), mutation_type);
                }
            }

            else if (kGeneration_count > 400 && kGeneration_count <= 500) {
                kProbCrossover = 0.3;
                kProbMutation = 0.9;
                
                /* CROSSOVER */
                for (int time = 0; time < kCrossoverTime; time++){      // control lambda (lambda= 2 * crossover_times)
                    RAND = FLOAT_MIN + (float)(rand()) / ((float)(RAND_MAX/(FLOAT_MAX - FLOAT_MIN)));
                    XRoulette1 = X_prob.at(4).at(0);
                    XRoulette2 = XRoulette1 + X_prob.at(4).at(1);
                    if (RAND < XRoulette1)
                        crossover_type = 0;
                    else if (RAND < XRoulette2)
                        crossover_type = 1;
                    else
                        crossover_type = 2;
                    
                    new_offspring = Crossover(population, kComponentNum, crossover_type);
                    offspring.push_back(new_offspring.at(0));
                    offspring.push_back(new_offspring.at(1));
                    new_offspring.clear();
                }
                /* MUTATION */
                for (int child = 0; child < offspring.size(); child++) {
                    RAND = FLOAT_MIN + (float)(rand()) / ((float)(RAND_MAX/(FLOAT_MAX - FLOAT_MIN)));
                    MRoulette1 = M_prob.at(4).at(0);
                    MRoulette2 = MRoulette1 + M_prob.at(4).at(1);
                    MRoulette3 = MRoulette2 + M_prob.at(4).at(2);
                    MRoulette4 = MRoulette3 + M_prob.at(4).at(3);
                    MRoulette5 = MRoulette4 + M_prob.at(4).at(4);
                    MRoulette6 = MRoulette5 + M_prob.at(4).at(5);
                    if (RAND < MRoulette1)
                        mutation_type = 0;
                    else if (RAND < MRoulette2)
                        mutation_type = 1;
                    else if (RAND < MRoulette3)
                        mutation_type = 2;
                    else if (RAND < MRoulette4)
                        mutation_type = 3;
                    else if (RAND < MRoulette5)
                        mutation_type = 4;
                    else if (RAND < MRoulette6)
                        mutation_type = 5;
                    else
                        mutation_type = 6;

                    offspring.at(child) = Mutation(offspring.at(child), mutation_type);
                }
            }

            else if (kGeneration_count > 500 && kGeneration_count <= 600) {
                kProbCrossover = 0.3;
                kProbMutation = 0.95;
                
                /* CROSSOVER */
                for (int time = 0; time < kCrossoverTime; time++){      // control lambda (lambda= 2 * crossover_times)
                    RAND = FLOAT_MIN + (float)(rand()) / ((float)(RAND_MAX/(FLOAT_MAX - FLOAT_MIN)));
                    XRoulette1 = X_prob.at(5).at(0);
                    XRoulette2 = XRoulette1 + X_prob.at(5).at(1);
                    if (RAND < XRoulette1)
                        crossover_type = 0;
                    else if (RAND < XRoulette2)
                        crossover_type = 1;
                    else
                        crossover_type = 2;
                    
                    new_offspring = Crossover(population, kComponentNum, crossover_type);
                    offspring.push_back(new_offspring.at(0));
                    offspring.push_back(new_offspring.at(1));
                    new_offspring.clear();
                }
                /* MUTATION */
                for (int child = 0; child < offspring.size(); child++) {
                    RAND = FLOAT_MIN + (float)(rand()) / ((float)(RAND_MAX/(FLOAT_MAX - FLOAT_MIN)));
                    MRoulette1 = M_prob.at(5).at(0);
                    MRoulette2 = MRoulette1 + M_prob.at(5).at(1);
                    MRoulette3 = MRoulette2 + M_prob.at(5).at(2);
                    MRoulette4 = MRoulette3 + M_prob.at(5).at(3);
                    MRoulette5 = MRoulette4 + M_prob.at(5).at(4);
                    MRoulette6 = MRoulette5 + M_prob.at(5).at(5);
                    if (RAND < MRoulette1)
                        mutation_type = 0;
                    else if (RAND < MRoulette2)
                        mutation_type = 1;
                    else if (RAND < MRoulette3)
                        mutation_type = 2;
                    else if (RAND < MRoulette4)
                        mutation_type = 3;
                    else if (RAND < MRoulette5)
                        mutation_type = 4;
                    else if (RAND < MRoulette6)
                        mutation_type = 5;
                    else
                        mutation_type = 6;

                    offspring.at(child) = Mutation(offspring.at(child), mutation_type);
                }
            }

            else if (kGeneration_count > 600 && kGeneration_count <= 700) {
                kProbCrossover = 0.3;
                kProbMutation = 0.95;
                
                /* CROSSOVER */
                for (int time = 0; time < kCrossoverTime; time++){      // control lambda (lambda= 2 * crossover_times)
                    RAND = FLOAT_MIN + (float)(rand()) / ((float)(RAND_MAX/(FLOAT_MAX - FLOAT_MIN)));
                    XRoulette1 = X_prob.at(6).at(0);
                    XRoulette2 = XRoulette1 + X_prob.at(6).at(1);
                    if (RAND < XRoulette1)
                        crossover_type = 0;
                    else if (RAND < XRoulette2)
                        crossover_type = 1;
                    else
                        crossover_type = 2;
                    
                    new_offspring = Crossover(population, kComponentNum, crossover_type);
                    offspring.push_back(new_offspring.at(0));
                    offspring.push_back(new_offspring.at(1));
                    new_offspring.clear();
                }
                /* MUTATION */
                for (int child = 0; child < offspring.size(); child++) {
                    RAND = FLOAT_MIN + (float)(rand()) / ((float)(RAND_MAX/(FLOAT_MAX - FLOAT_MIN)));
                    MRoulette1 = M_prob.at(6).at(0);
                    MRoulette2 = MRoulette1 + M_prob.at(6).at(1);
                    MRoulette3 = MRoulette2 + M_prob.at(6).at(2);
                    MRoulette4 = MRoulette3 + M_prob.at(6).at(3);
                    MRoulette5 = MRoulette4 + M_prob.at(6).at(4);
                    MRoulette6 = MRoulette5 + M_prob.at(6).at(5);
                    if (RAND < MRoulette1)
                        mutation_type = 0;
                    else if (RAND < MRoulette2)
                        mutation_type = 1;
                    else if (RAND < MRoulette3)
                        mutation_type = 2;
                    else if (RAND < MRoulette4)
                        mutation_type = 3;
                    else if (RAND < MRoulette5)
                        mutation_type = 4;
                    else if (RAND < MRoulette6)
                        mutation_type = 5;
                    else
                        mutation_type = 6;

                    offspring.at(child) = Mutation(offspring.at(child), mutation_type);
                }
            }

            else {
                kProbCrossover = 0.3;
                kProbMutation = 0.95;
                
                /* CROSSOVER */
                for (int time = 0; time < kCrossoverTime; time++){      // control lambda (lambda= 2 * crossover_times)
                    // Xrand = rand() % 2;     // LeftSubtree:Ordered:SubtreeExchange = 1:2:4
                    // if (Xrand == 0)
                    //     crossover_type = 1;
                    // // else if (Xrand == 1 || Xrand == 2)
                    // //     crossover_type = 1;
                    // else
                    //     crossover_type = 2;
                    RAND = FLOAT_MIN + (float)(rand()) / ((float)(RAND_MAX/(FLOAT_MAX - FLOAT_MIN)));
                    XRoulette1 = X_prob.at(7).at(0);
                    XRoulette2 = XRoulette1 + X_prob.at(7).at(1);
                    if (RAND < XRoulette1)
                        crossover_type = 0;
                    else if (RAND < XRoulette2)
                        crossover_type = 1;
                    else
                        crossover_type = 2;

                    new_offspring = Crossover(population, kComponentNum, crossover_type);
                    offspring.push_back(new_offspring.at(0));
                    offspring.push_back(new_offspring.at(1));
                    new_offspring.clear();
                }
                /* MUTATION */
                for (int child = 0; child < offspring.size(); child++) {
                    // Mrand = rand() % 17;
                    // switch (Mrand)
                    // {
                    // case 0: case 1: case 2: case 3:
                    //     mutation_type = 0;
                    //     break;
                    // case 4: case 5: case 6: case 7:
                    //     mutation_type = 1;
                    //     break;
                    // case 8:
                    //     mutation_type = 2;
                    //     break;
                    // case 9: case 10:
                    //     mutation_type = 3;
                    //     break;
                    // case 11: case 12:
                    //     mutation_type = 4;
                    //     break;
                    // case 13: case 14:
                    //     mutation_type = 5;
                    //     break;
                    // case 15: case 16:
                    //     mutation_type = 6;
                    //     break;
                    // }
                    RAND = FLOAT_MIN + (float)(rand()) / ((float)(RAND_MAX/(FLOAT_MAX - FLOAT_MIN)));
                    MRoulette1 = M_prob.at(7).at(0);
                    MRoulette2 = MRoulette1 + M_prob.at(7).at(1);
                    MRoulette3 = MRoulette2 + M_prob.at(7).at(2);
                    MRoulette4 = MRoulette3 + M_prob.at(7).at(3);
                    MRoulette5 = MRoulette4 + M_prob.at(7).at(4);
                    MRoulette6 = MRoulette5 + M_prob.at(7).at(5);
                    if (RAND < MRoulette1)
                        mutation_type = 0;
                    else if (RAND < MRoulette2)
                        mutation_type = 1;
                    else if (RAND < MRoulette3)
                        mutation_type = 2;
                    else if (RAND < MRoulette4)
                        mutation_type = 3;
                    else if (RAND < MRoulette5)
                        mutation_type = 4;
                    else if (RAND < MRoulette6)
                        mutation_type = 5;
                    else
                        mutation_type = 6;

                    offspring.at(child) = Mutation(offspring.at(child), mutation_type);
                }
            }

            /* EVALUATE and SURVIVOR SELECTION */
            for (int i = 0; i < offspring.size(); i++) {
                offspring.at(i)->evaluateTotalArea();
                kEvaluation_count++;
                // offspring.at(i)->isChild_ = true;
                population.push_back(offspring.at(i));
            }
            sort(population.begin(), population.end(), SortPopArea);    // SortPopArea / SortPopWL / SortPop
            population.erase(population.begin()+kPopSize, population.end());

            // cout << Exp << " Generation: " << kGeneration_count << endl;
            // cout << Exp <<  " Area Utilization: " << kTotalArea / population.front()->area_ << endl;
            if (kEvaluation_count % 1000 == 0) {
                evaluation_record.push_back(kEvaluation_count);
                anytime_behavior_area.push_back(kTotalArea / population.front()->area_);
                anytime_behavior_total_area.push_back(population.front()->area_);
                cout << Exp << " generation: " << kGeneration_count <<  " Area Utilization: " << kTotalArea / population.front()->area_ << endl;
            }
        }
        clock_t end_time = clock();
        ExpResult << "Total Time" << "," << double(end_time - start_time) / CLOCKS_PER_SEC << "\n";
        ExpResult << "Evaluation count" << "," << "Area uti" << "," << "Total Area" << "\n";
        for (int i = 0; i < anytime_behavior_area.size(); i++) {
            ExpResult << evaluation_record.at(i) << "," << anytime_behavior_area.at(i) << "," << anytime_behavior_total_area.at(i) << "\n";
        }

        /*** Reset Exp Parameter ***/
        anytime_behavior_area.clear();
        // anytime_behavior_WL.clear();
        population.clear();
        offspring.clear();
        
    }

    return 0;
}