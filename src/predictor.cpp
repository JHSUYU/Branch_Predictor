//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include <cmath>
#include<math.h>
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
  // In Gshare, we use hashmap to implement the count table
  std::map<uint32_t, int8_t> BranchHistoryTable;
} Gshare;

Gshare gshare;


typedef struct Tournament
{
  uint32_t globalHistoryRecord;
  //In tournament, we use matrix to implement the count table
  int8_t* localBranchCountTable;
  int8_t* globalBranchCountTable;
  uint32_t* localPatternHistoryTable;
  int8_t* chooser;
} Tournament;

Tournament tournament;

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
  else if(bpType==TOURNAMENT){
    tournament.globalHistoryRecord=0;
    tournament.localPatternHistoryTable=new uint32_t[uint32_t(pow(2,pcIndexBits))];
    for(int i=0;i<pow(2,pcIndexBits);i++){
      tournament.localPatternHistoryTable[i]=0;
    }
    tournament.localBranchCountTable=new int8_t[uint32_t(pow(2,lhistoryBits))];
    for(int i=0;i<pow(2,lhistoryBits);i++){
      tournament.localBranchCountTable[i]=WN;
    }
    tournament.globalBranchCountTable=new int8_t[uint32_t(pow(2,ghistoryBits))];
    for(int i=0;i<pow(2,ghistoryBits);i++){
      tournament.globalBranchCountTable[i]=WN;
    }
    tournament.chooser=new int8_t[uint32_t(pow(2,ghistoryBits))];
    for(int i=0;i<pow(2,ghistoryBits);i++){
      tournament.chooser[i]=WLocal;
    }
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
    int8_t pred_result = gshare.BranchHistoryTable.count(bht_index) == 0 ? WN : gshare.BranchHistoryTable[bht_index];
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
  {
    uint32_t localPatternHistoryIndex=pc<<(LENGTH-pcIndexBits)>>(LENGTH-pcIndexBits);
    uint32_t localBranchCountIndex=(tournament.localPatternHistoryTable[localPatternHistoryIndex])<<(LENGTH-lhistoryBits)>>(LENGTH-lhistoryBits);
    int8_t localPredResult=tournament.localBranchCountTable[localBranchCountIndex];
    uint32_t globalIndex=tournament.globalHistoryRecord<<(LENGTH-ghistoryBits)>>(LENGTH-ghistoryBits);
    int8_t globalPredResult=tournament.globalBranchCountTable[globalIndex];
    int8_t choose=tournament.chooser[globalIndex];
    int8_t pred_result;
    if(choose>=WGlobal){
      pred_result=globalPredResult;
    }
    else{
      pred_result=localPredResult;
    }
    if(pred_result>=WT){
      return TAKEN;
    }
    else{
      return NOTTAKEN;
    }
  }
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

void train_tournament_predictor(uint32_t pc, uint8_t outcome){
  uint32_t globalIndex=tournament.globalHistoryRecord<<(LENGTH-ghistoryBits)>>(LENGTH-ghistoryBits);
  int8_t global_pred_result=tournament.globalBranchCountTable[globalIndex];
  uint32_t localPatternHistoryIndex=pc<<(LENGTH-pcIndexBits)>>(LENGTH-pcIndexBits);
  uint32_t localPatternHistory=tournament.localPatternHistoryTable[localPatternHistoryIndex];
  uint32_t localBranchCountIndex=localPatternHistory<<(LENGTH-lhistoryBits)>>(LENGTH-lhistoryBits);
  int8_t local_pred_result=tournament.localBranchCountTable[localBranchCountIndex];
  int8_t choose=tournament.chooser[globalIndex];
  if(outcome==(int8_t(local_pred_result/2)) && outcome!=(int8_t(global_pred_result/2)) ){
    choose=std::max(int8_t(SLocal),--choose);
  }
  else if(outcome!=(int8_t(local_pred_result/2)) && outcome==(int8_t(global_pred_result/2))){
    choose=std::min(int8_t(SGlobal),++choose);
  }
  tournament.chooser[globalIndex]=choose;

  if(outcome==TAKEN){
    global_pred_result++;
    local_pred_result++;
    if(global_pred_result>ST){
      tournament.globalBranchCountTable[globalIndex]=ST;
    }
    else{
      tournament.globalBranchCountTable[globalIndex]=global_pred_result;
    }
    if(local_pred_result>ST){
      tournament.localBranchCountTable[localBranchCountIndex]=ST;
    }
    else{
      tournament.localBranchCountTable[localBranchCountIndex]=local_pred_result;
    }
  }
  else{
    global_pred_result--;
    local_pred_result--;
    if(global_pred_result<SN){
      tournament.globalBranchCountTable[globalIndex]=SN;
    }
    else{
      tournament.globalBranchCountTable[globalIndex]=global_pred_result;
    }
    if(local_pred_result<SN){
      tournament.localBranchCountTable[localBranchCountIndex]=SN;
    }
    else{
      tournament.localBranchCountTable[localBranchCountIndex]=local_pred_result;
    }
  }

  tournament.localPatternHistoryTable[localPatternHistoryIndex]=localPatternHistory<<1|outcome;
  tournament.globalHistoryRecord=tournament.globalHistoryRecord<<1|outcome;
}

void train_predictor(uint32_t pc, uint8_t outcome)
{
  if (bpType == GSHARE)
  {
    train_gshare_predictor(pc, outcome);
  }
  else if(bpType=TOURNAMENT){
    train_tournament_predictor(pc,outcome);
  }
  //
  // TODO: Implement Predictor training
  //
}
