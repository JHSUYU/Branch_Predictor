//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include <cmath>
#include "predictor.h"
#include <map>

//
// TODO:Student Information
//
const char *studentName = "NAME";
const char *studentID = "PID";
const char *email = "EMAIL";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = {"Static", "Gshare",
                         "Tournament", "Custom"};

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
// TODO: Add your own Branch Predictor data structures here
//

typedef struct Gshare
{
  uint32_t GlobalHistoryRecord;
  std::map<uint32_t, uint32_t> BranchHistoryTable;
} Gshare;

Gshare gshare;

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void init_predictor()
{
  if (bpType == GSHARE)
  {
    gshare.GlobalHistoryRecord = 0;
  }

  //
  // TODO: Initialize Branch Predictor Data Structures
  //
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  // TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  switch (bpType)
  {
  case STATIC:
    return TAKEN;
  case GSHARE:
  {
    uint8_t displacement_bit_num = 32 - ghistoryBits;
    uint32_t bht_index = gshare.GlobalHistoryRecord ^ ((pc << displacement_bit_num) >> displacement_bit_num);
    // if BHT does not contain the index, pred_result will be 1(weak not taken)
    uint32_t pred_result = gshare.BranchHistoryTable.count(bht_index) == 0 ? WN : gshare.BranchHistoryTable[bht_index];
    if (pred_result <= WN)
    {
      return NOTTAKEN;
    }
    else
    {
      return TAKEN;
    }
  }

  case TOURNAMENT:
  case CUSTOM:
  default:
    break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//

void train_gshare_predictor(uint32_t pc, uint8_t outcome)
{
  uint8_t displacement_bit_num = 32 - ghistoryBits;
  //use lower bit of pc to XOR with global history record
  uint32_t bht_index = gshare.GlobalHistoryRecord ^ ((pc << displacement_bit_num) >> displacement_bit_num);
  int8_t new_value;
  if (outcome == TAKEN)
  {
    // default value of bht is wake not taken
    if (gshare.BranchHistoryTable.count(bht_index) == 0)
    {
      new_value = WT;
    }
    else
    {
      new_value = ++gshare.BranchHistoryTable[bht_index];
      if (new_value > 3)
      {
        new_value = int8_t(3);
      }
    }
    gshare.BranchHistoryTable[bht_index] = new_value;
  }
  else
  {
    if (gshare.BranchHistoryTable.count(bht_index) == 0)
    {
      new_value = SN;
    }
    else
    {
      new_value = --gshare.BranchHistoryTable[bht_index];
      if (new_value < 0)
      {
        new_value = int8_t(0);
      }
    }
    gshare.BranchHistoryTable[bht_index] = new_value;
  }
  gshare.GlobalHistoryRecord = (gshare.GlobalHistoryRecord << 1) | uint32_t(outcome);
}

void train_predictor(uint32_t pc, uint8_t outcome)
{
  if (bpType == GSHARE)
  {
    train_gshare_predictor(pc, outcome);
  }
  //
  // TODO: Implement Predictor training
  //
}
