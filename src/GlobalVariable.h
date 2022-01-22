#ifndef GLOBALVARIBLE_H
#define GLOBALVARIBLE_H
using namespace std;

/* Exp Settings */
extern const bool kSingleDoubleSided; // 0 for Single-Sided; 1 for Double-Sided
extern const double kmargin;
extern int kEvaluation_count;

/* EC Hyperparameter */
extern const int kExpTime;
extern const int kGeneration;
extern const int kPopSize;
extern double kProbCrossover;
extern const int kCrossoverTime;
extern double kProbMutation;
extern const double kProbAngleMutation;
extern const double kProbBitFlip;
extern const int k_kTournament;

extern double kCrossover_Count;
extern double kMutation_Count;
extern double kLeftSubtreeCrossover_Count;
extern double kOrderedCrossover_Count;
extern double kSubtreeExchangeCrossover_Count;
extern double kSwapNode_Count;
extern double kInsert_Count;
extern double kBitwise_Count;
extern double kScramble_Count;
extern double kSwapBranch_Count;
extern double kShiftSubtree_Count;
extern double kSwapSubtree_Count;

extern double kLeftSubtreeCrossover_SuccessCount;
extern double kOrderedCrossover_SuccessCount;
extern double kSubtreeExchangeCrossover_SuccessCount;
extern double kSwapNode_SuccessCount;
extern double kInsert_SuccessCount;
extern double kBitwise_SuccessCount;
extern double kScramble_SuccessCount;
extern double kSwapBranch_SuccessCount;
extern double kShiftSubtree_SuccessCount;
extern double kSwapSubtree_SuccessCount;

#endif