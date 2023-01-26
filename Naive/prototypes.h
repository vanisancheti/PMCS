/*
 * File Info: prototypes.h
 * Description: This file contains the prototypes of the global functions.
 * */

#ifndef _GLOBAL_FUNCTIONS_H_
#define _GLOBAL_FUNCTIONS_H_

void Error(char *c_fnName, char *c_errorMsg);
void ErrorExit(char *c_fnName, char *c_errorMsg);
bool BIsPresentInNodeIDVector(vector<NodeID> array_VnID, NodeID search_nID);
void PrintLine();

#endif
