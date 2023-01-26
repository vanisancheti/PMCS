#ifndef _EXPAND_CUT_H
#define _EXPAND_CUT_H

#include <unistd.h>
int maxNoOfEdges, maxNoOfNodes;
// To search for cuts, you need to search in both cutEdgeIndex and
// cutsOnEmptyGraph
//
// Stores the Triplet value of the subgraph in knownFreqGraphs at position
// [n1][n2] of cutEdgeIndex where n1 and n2 are the nodes incident on the
// cut edge
vector < vector < vector<Triplet> > > cutEdgeIndex;
// The node IDs by adding which to the empty graph forms the cut pair
vector<int> cutsOnEmptyGraph;

int noOfCuts,noOfExpandCuts;
vector<vector<int> > cntFreq;
vector<vector<int> > cntInfreq;

// Number of candidate subgraphs which are unmerged with the maximal
int noOfUnmergedCuts;
int localNoOfExpCuts;

// To store maximal frequent subgraphs. At the end of expand cut on a G_i,
// new maximal frequent subgraphs are found which results in some of the old
// ones becoming invalid. Then, the corresponding position of invalid ones are
// marked by 0 bit in validMaxFreq and the memory of that vector position is freed in maxFreq
vector<Graph*> maxFreq;
// Boolean vector to mark 0 or 1 for the maxFreq
vector<bool> validMaxFreq;
// Number of positions marked with 1 in validMaxFreq vector
int noOfValidMaxFreq;
int numOfValidCandidateGraphs;
// Frequency of the maxFreq entries
vector<int> maxFreq_FreqVal;
vector<bool> newMaxFreq;
vector<Graph*> st_vec;
// Print Functions
void PrintKnownFreqGraph(map<int, map<int, knownGraphNode*> > &knownGraphsMap, Triplet t);
void PrintKnownInfreqGraph(map<int, map<int, knownGraphNode*> > &knownGraphsMap, Triplet t);
void PrintJustCuts();
void PrintCuts(map<int, map<int, knownGraphNode*> > &knownGraphsMap);
void PrintMaxFreqFile(char *fileName);
void PrintMaxFreq();

void InitializeCutEdgeIndex();
void FreeCutEdgeIndex();

void FindMaxFreqAndMerge(map<int, map<int, knownGraphNode*> > &knownGraphsMap, int partition_id);
bool IsThisGraphFrequentAndFound(map<int, map<int, knownGraphNode*> > &knownGraphsMap, Graph *g, Triplet *trip);
bool IsThisGraphInfrequentAndFound(map<int, map<int, knownGraphNode*> > &knownGraphsMap,Graph *g, Triplet *trip);
bool FindOnlyFrequency(map<int, map<int, knownGraphNode*> > &knownGraphsMap ,Graph *graph_g, Triplet *trip);

bool FindOnlyFrequencyDontAdd(Graph *graph_g);

int ComputeFrequency(Graph *graph_g);
int ComputeFrequencyWithGraphID(Graph *graph_g, vector<int> &FoundGraphID);

bool FindFrequency(map<int, map<int, knownGraphNode*> > &knownGraphsMap, Graph *graph_g, Triplet *trip);

void AddThisToCut(Triplet trip, int sourceID, int targetID);
bool IsThisCutExisting(Triplet trip, int sourceID, int targetID);

bool BGetRepresentativeCut_TopDown(map<int, map<int, knownGraphNode*> > &knownGraphsMap ,Graph *graph_g, Triplet *childTrip, Triplet *parentTrip, int *sourceID, int *targetID);
bool BGetRepresentativeCut_BottomUp(map<int, map<int, knownGraphNode*> > &knownGraphsMap, Graph *graph_g, Triplet *childTrip, Triplet *parentTrip, int *sourceID, int *targetID);
bool BGetRepresentativeCut(map<int, map<int, knownGraphNode*> > &knownGraphsMap, Graph *graph_g, Triplet *childTrip, Triplet *parentTrip, int *sourceID, int *targetID, bool topDown_b);

void StartExpandCut(int partition_id);
void ExpandCut(Triplet freqTrip, Triplet inFreqTrip, int sourceID, int targetID, bool bFlagCheck,
		int noOfSeenGraphs, vector<Graph*> seenGraphs, vector<bool> isSeenGraphFreq,
		vector<int> isSeenAlreadyThere,map<int, map<int, knownGraphNode*> > &knownGraphsMap);

void PrintKnownFreqGraph(map<int, map<int, knownGraphNode*> > &knownGraphsMap, Triplet t){
	knownFreqGraphs[t.GetFirst()][t.GetSecond()][t.GetThird()]->Print();
	cout << endl;
}

void PrintKnownInfreqGraph(map<int, map<int, knownGraphNode*> > &knownGraphsMap, Triplet t){
	knownGraphsMap[t.GetFirst()][t.GetSecond()]->knownInfreqGraphs[t.GetThird()]->Print();
	cout << endl;
}

void PrintJustCuts(){
	cout << "NO OF Cuts: " << noOfCuts << endl;
	for(int i=0; i <= maxNoOfNodes; i++)
		for(int j=0; j <= maxNoOfNodes; j++)
		{
			for(int a=0; a < cutEdgeIndex[i][j].size(); a++)
			{
				cout << "Edge: " << i << " - " << j << " -- " << a << "/: Size: " << cutEdgeIndex[i][j].size() << endl;
				cutEdgeIndex[i][j][a].Print();
				cout << endl;
			}
		}
}

void PrintCuts(map<int, map<int, knownGraphNode*> > &knownGraphsMap){
	cout << "Local No Of Exp. Cuts: " << localNoOfExpCuts << " - NO OF Cuts: " << noOfCuts << endl;
	for(int i=0; i <= maxNoOfNodes; i++)
		for(int j=0; j <= maxNoOfNodes; j++)
		{
			for(int a=0; a < cutEdgeIndex[i][j].size(); a++)
			{
				cout << "Edge: " << i << " - " << j << " -- " << a << "/: Size: " << cutEdgeIndex[i][j].size() << endl;
				int r = cutEdgeIndex[i][j][a].GetFirst();
				int c = cutEdgeIndex[i][j][a].GetSecond();
				int indexID = cutEdgeIndex[i][j][a].GetThird();
				cout << "\tGraph: ";

				knownGraphsMap[r][c]->knownFreqGraphs[indexID]->Print();
				cout << endl;
			}
		}
}

void PrintMaxFreqFile(char *fileName){
	int tempCount=0;
	for(int i=0; i < maxFreq.size(); i++)
	{
		if(!validMaxFreq[i])
			continue;
		ofstream ofs;
		ofs.open(fileName,ofstream::app);
		ofs << endl;
		if(shouldCount)
		{
			vector<int> FoundGraph;
			ComputeFrequencyWithGraphID(maxFreq[i], FoundGraph);
			ofs << "t # " << tempCount << endl;
			ofs << "Freq: " << maxFreq_FreqVal[i] << endl;
			vector<int>::iterator it;
			it = FoundGraph.begin();
			while(it!=FoundGraph.end()){
				ofs << "Found in Graph with Id: " << *it << endl; 
				it++;
			}
		}
		else
			ofs << "t # " << tempCount << endl;
		ofs.close();
		maxFreq[i]->PrintFile(fileName);
		tempCount++;
	}
}

void PrintMaxFreq(){
	int tempCount=0;
	for(int i=0; i < maxFreq.size(); i++)
	{
		if(!validMaxFreq[i])
			continue;
		if(shouldCount)
		{
			cout << "t # " << tempCount << endl;
			cout << "Freq: " << maxFreq_FreqVal[i] << endl;
		}
		else
			cout << "t # " << tempCount << endl;

		maxFreq[i]->Print();
	}
}

void FindMaxFreqAndMerge(map<int, map<int, knownGraphNode*> > &knownGraphsMap, int partition_id){
	// Finding local maximal frequent subgraphs
	vector<Triplet> cutsInThisLevel;
	for(int i=0; i <= maxNoOfEdges; i++)
	{
		for(int j=1; j <= maxNoOfNodes; j++)
		{
			if(findIfExist(knownGraphsMap, i, j))
			{
				for(int a=knownGraphsMap[i][j]->knownFreqGraphs_startindex; a < knownGraphsMap[i][j]->knownFreqGraphs.size(); a++)
				{

					bool flag = true;
					for(int k=i; k <=maxNoOfEdges; k++)
					{
						for(int l=j; l<=maxNoOfNodes; l++)
						{
							if(findIfExist(knownGraphsMap, k, l))
							{
								for(int b=knownGraphsMap[k][l]->knownFreqGraphs_startindex; b < knownGraphsMap[k][l]->knownFreqGraphs.size(); b++)
								{
									if( (k==i) && (l==j) && (a >= b) )
										continue;

									if(knownGraphsMap[i][j]->knownFreqGraphs[a]->BIsCodeSubGraphOf(knownGraphsMap[k][l]->knownFreqGraphs[b]))
									{
										flag = false;
										break;
									}
									else if(knownGraphsMap[i][j]->knownFreqGraphs[a]->BIsSubGraphOf(knownGraphsMap[k][l]->knownFreqGraphs[b]))
									{
										flag = false;
										break;
									}
								}
							}
							// We can break once we found that
							// knownFreqGraphs[i][j][a] is to be removed
							if(!flag)
								break;
						}
						if(!flag)
							break;
					}
					if(flag) // is local maximal
					{
						Triplet t(i,j,a);
						cutsInThisLevel.push_back(t);
					}
				}
			}
		}
	}

	#if 0 // debug
		cout << "Printing the local max: " << cutsInThisLevel.size() << endl;
		for(int i=0; i<cutsInThisLevel.size(); i++)
		{
			cout << "Local Max " << i << endl;
			PrintKnownFreqGraph(knownGraphsMap, cutsInThisLevel[i]);
		}
	#endif
	// tempBool stores all the valid local maximals initialized to true. when
	// on merging with global maximal, a local maximal becomes invalid the
	// corresponding position in tempBool is set to false
	vector<bool> tempBool;
	for(int i=0; i < cutsInThisLevel.size(); i++)
		tempBool.push_back(true);

	// For each local maximal L
	//      For each global maximal G
	//          if L is subgraph of G
	//              set tempBool of L to false
	//          else if G is subgraph of L
	//              set validMaxFreq of G to false
	for(int i=0; i < cutsInThisLevel.size(); i++)
	{
		bool flag = true;

		for(int j=0; j < maxFreq.size(); j++)
		{
			if(!validMaxFreq[j])
				continue;
			if(knownGraphsMap[cutsInThisLevel[i].GetFirst()][cutsInThisLevel[i].GetSecond()]->knownFreqGraphs[cutsInThisLevel[i].GetThird()]->BIsSubGraphOf(maxFreq[j]))
			{
				flag = false;
				break;
			}
			else if(maxFreq[j]->BIsSubGraphOf(knownGraphsMap[cutsInThisLevel[i].GetFirst()][cutsInThisLevel[i].GetSecond()]->knownFreqGraphs[cutsInThisLevel[i].GetThird()]))
			{
				validMaxFreq[j] = false;
				delete maxFreq[j];
				noOfValidMaxFreq--;
			}
		}
		if(!flag)
			tempBool[i] = false;
	}

	// Copying the valid local maximal to maxFreq
	// copy the rest
	for(int i=0; i < cutsInThisLevel.size(); i++)
	{
		if(!tempBool[i])
			continue;
		Graph *g = new Graph();
		g->Copy(knownGraphsMap[cutsInThisLevel[i].GetFirst()][cutsInThisLevel[i].GetSecond()]->knownFreqGraphs[cutsInThisLevel[i].GetThird()]);
		maxFreq.push_back(g);
		if(shouldCount)
			maxFreq_FreqVal.push_back(knownGraphsMap[cutsInThisLevel[i].GetFirst()][cutsInThisLevel[i].GetSecond()]->knownFreqGraphs_FreqVal[cutsInThisLevel[i].GetThird()]);
		validMaxFreq.push_back(true);
		noOfValidMaxFreq++;
	}
}

void InitializeCutEdgeIndex(){
	for(int i=0; i <= maxNoOfNodes; i++)
	{
		vector< vector<Triplet> > tempRow;
		for(int j=0; j <= maxNoOfNodes; j++)
		{
			vector<Triplet> tempEntry;
			tempRow.push_back(tempEntry);
		}
		cutEdgeIndex.push_back(tempRow);
	}
}

void get_all_embeddings_all_graphs(int start, pair< pair<int,int>, pair<int,int> > edge, vector< map< int, vector< pair< map<int, int>, map<int, int> > > > > &allMFS_embeddings){
	for(int i=0;i<noOfdbAll_i;i++)
	{
		map< int, vector< pair< map<int, int>, map<int, int> > > > main_storage;
		if(start != 0)
			main_storage = allMFS_embeddings[i];
		dbAll_g[i].build_hash(start, edge, main_storage, maxFreq, validMaxFreq, newMaxFreq);
		if(start == 0)
			allMFS_embeddings.push_back(main_storage);
		else
			allMFS_embeddings[i] = main_storage;
	}
	return;
}

#if 0
void FreeKnownGraphs(){
	for(int i=0; i <= maxNoOfEdges; i++)
	{
		for(int j=1; j <= maxNoOfNodes; j++)
		{
			for(int k=0; k < knownFreqGraphs[i][j].size(); k++)
				knownFreqGraphs[i][j][k]->~Graph();
			for(int k=0; k < knownInfreqGraphs[i][j].size(); k++)
				knownInfreqGraphs[i][j][k]->~Graph();
			// new
			knownFreqGraphs[i][j].erase(knownFreqGraphs[i][j].begin(),knownFreqGraphs[i][j].end());
			knownInfreqGraphs[i][j].erase(knownInfreqGraphs[i][j].begin(),knownInfreqGraphs[i][j].end());
			knownFreqGraphs_FreqVal[i][j].erase(knownFreqGraphs_FreqVal[i][j].begin(),knownFreqGraphs_FreqVal[i][j].end());
		}
		knownFreqGraphs[i].erase(knownFreqGraphs[i].begin(), knownFreqGraphs[i].end());
		knownInfreqGraphs[i].erase(knownInfreqGraphs[i].begin(), knownInfreqGraphs[i].end());
		knownFreqGraphs_FreqVal[i].erase(knownFreqGraphs_FreqVal[i].begin(), knownFreqGraphs_FreqVal[i].end());
	}
	knownFreqGraphs.erase(knownFreqGraphs.begin(), knownFreqGraphs.end());
	knownInfreqGraphs.erase(knownInfreqGraphs.begin(), knownInfreqGraphs.end());
	knownFreqGraphs_FreqVal.erase(knownFreqGraphs_FreqVal.begin(), knownFreqGraphs_FreqVal.end());
}
#endif

void FreeCutEdgeIndex(){
	for(int i=0; i <= maxNoOfNodes; i++)
	{
		for(int j=0; j <= maxNoOfNodes; j++)
		{
				cutEdgeIndex[i][j].erase(cutEdgeIndex[i][j].begin(), cutEdgeIndex[i][j].end());
				vector<Triplet>().swap(cutEdgeIndex[i][j]);
		}
		cutEdgeIndex[i].erase(cutEdgeIndex[i].begin(), cutEdgeIndex[i].end());
		vector<vector <Triplet> >().swap(cutEdgeIndex[i]);
	}
	cutEdgeIndex.erase(cutEdgeIndex.begin(), cutEdgeIndex.end());
	vector<vector<vector <Triplet> > >().swap(cutEdgeIndex);
}

bool IsThisGraphFrequentAndFound(map<int, map<int, knownGraphNode*> > &knownGraphsMap, Graph *g, Triplet *trip){

	int r = g->IGetNoOfAllEdges();
	int c = g->IGetNoOfValidNodes();

	if(findIfExist(knownGraphsMap, r, c))
	{
		for(int i=0; i < knownGraphsMap[r][c]->knownFreqGraphs.size(); i++)
		{
			if(g->BIsExactlyEqualTo(knownGraphsMap[r][c]->knownFreqGraphs[i]))
			{
				trip->SetValues(r,c,i);
				return true;
			}
		}

		#if 0
			for(int i=0; i < knownGraphsMap[r][c]->knownFreqGraphs.size(); i++)
			{
				if(g->GetNewGraphCode() == knownGraphsMap[r][c]->knownFreqGraphs[i]->GetNewGraphCode())
				{
					if(g->BIsIsomorphicTo(knownGraphsMap[r][c]->knownFreqGraphs[i]))
					{
						Graph *temp_g = new Graph();
						temp_g->Copy(g);
						trip->SetValues(r,c,knownGraphsMap[r][c]->knownFreqGraphs.size());

						knownGraphsMap[r][c]->knownFreqGraphs.push_back(temp_g);
						if(shouldCount)
							knownGraphsMap[r][c]->knownFreqGraphs_FreqVal.push_back(knownGraphsMap[r][c]->knownFreqGraphs_FreqVal[i]);
						return true;
					}
				}
			}
		#endif
	}
	return false;
}

bool IsThisGraphInfrequentAndFound(map<int, map<int, knownGraphNode*> > &knownGraphsMap, Graph *g, Triplet *trip){
	int r = g->IGetNoOfAllEdges();
	int c = g->IGetNoOfValidNodes();

	if(findIfExist(knownGraphsMap, r, c))
	{
		for(int i=0; i < knownGraphsMap[r][c]->knownInfreqGraphs.size(); i++)
		{
			if(g->BIsExactlyEqualTo(knownGraphsMap[r][c]->knownInfreqGraphs[i]))
			{
				trip->SetValues(r,c,i);
				return true;

			}
		}
	#if 0
		for(int i=0; i < knownGraphsMap[r][c]->knownInfreqGraphs.size(); i++)
		{
			if(g->GetNewGraphCode() == knownGraphsMap[r][c]->knownInfreqGraphs[i]->GetNewGraphCode())
			{
				if(g->BIsIsomorphicTo(knownGraphsMap[r][c]->knownInfreqGraphs[i]))
				{
					Graph *temp_g = new Graph();
					temp_g->Copy(g);
					trip->SetValues(r,c,knownGraphsMap[r][c]->knownInfreqGraphs.size());

					knownGraphsMap[r][c]->knownInfreqGraphs.push_back(temp_g);
					return true;
				}
			}
		}
	#endif
	}
	return false;
}

bool FindOnlyFrequency(map<int, map<int, knownGraphNode*> > &knownGraphsMap,Graph *graph_g, Triplet *trip){
	int freq_i=0,lostfreq_i=0;

	if(graph_g->GetGraphType() == GRAPH_EMPTY)
		freq_i = minSupport_i;
	else
	{
		int r = graph_g->IGetNoOfAllEdges();
		int c = graph_g->IGetNoOfValidNodes();
		if(findIfExist(knownGraphsMap, r, c))
		{
			for(int i=0; i < knownGraphsMap[r][c]->knownFreqGraphs.size(); i++)
			{
				if(graph_g->GetNewGraphCode() == knownGraphsMap[r][c]->knownFreqGraphs[i]->GetNewGraphCode())
				{
					if(graph_g->BIsIsomorphicTo(knownGraphsMap[r][c]->knownFreqGraphs[i]))
					{
						Graph *temp_g = new Graph();
						temp_g->Copy(graph_g);
						trip->SetValues(r,c,knownGraphsMap[r][c]->knownFreqGraphs.size());
						knownGraphsMap[r][c]->knownFreqGraphs.push_back(temp_g);
						if(shouldCount)
							knownGraphsMap[r][c]->knownFreqGraphs_FreqVal.push_back(knownGraphsMap[r][c]->knownFreqGraphs_FreqVal[i]);
						return true;
					}
				}
			}

			for(int i=0; i < knownGraphsMap[r][c]->knownInfreqGraphs.size(); i++)
			{

				if(graph_g->GetNewGraphCode() == knownGraphsMap[r][c]->knownInfreqGraphs[i]->GetNewGraphCode())
				{
					if(graph_g->BIsIsomorphicTo(knownGraphsMap[r][c]->knownInfreqGraphs[i]))
					{
						Graph *temp_g = new Graph();
						temp_g->Copy(graph_g);
						trip->SetValues(r,c,knownGraphsMap[r][c]->knownInfreqGraphs.size());

						knownGraphsMap[r][c]->knownInfreqGraphs.push_back(temp_g);
						return false;
					}
				}
			}
		}

		for(int i=0; i < noOfdbAll_i; i++)
		{
			if(graph_g->IGetGraphID() == dbAll_g[i].IGetGraphID())
				freq_i++;
			else if(graph_g->BIsSubGraphOf(&(dbAll_g[i])))
				freq_i++;
			else
				lostfreq_i++;
		}
	}
	if(freq_i >= minSupport_i)
	{
		Graph *temp_g = new Graph();
		temp_g->Copy(graph_g);
		int r = graph_g->IGetNoOfAllEdges();
		int c = graph_g->IGetNoOfValidNodes();
		if(findIfExist(knownGraphsMap, r, c))
			trip->SetValues(r,c,knownGraphsMap[r][c]->knownFreqGraphs.size());
		else{
			trip->SetValues(r,c,0);
			knownGraphsMap[r][c] = new knownGraphNode();
			InitializeknownGraphNode(knownGraphsMap[r][c], r, c);
		}
		knownGraphsMap[r][c]->knownFreqGraphs.push_back(temp_g);
		if(shouldCount)
		{
			knownGraphsMap[r][c]->knownFreqGraphs_FreqVal.push_back(freq_i);
		}
		return true;
	}
	else
	{
		Graph *temp_g = new Graph();
		temp_g->Copy(graph_g);
		int r = graph_g->IGetNoOfAllEdges();
		int c = graph_g->IGetNoOfValidNodes();
		if(findIfExist(knownGraphsMap, r, c))
			trip->SetValues( r,c,knownGraphsMap[r][c]->knownInfreqGraphs.size());
		else
		{
			trip->SetValues(r,c,0);
			knownGraphsMap[r][c] = new knownGraphNode();
			InitializeknownGraphNode(knownGraphsMap[r][c], r, c);	
		}
		
		knownGraphsMap[r][c]->knownInfreqGraphs.push_back(temp_g);
		// std::cout << "exiting 4" << '\n';
		return false;
	}
}

bool FindOnlyFrequencyDontAdd(Graph *graph_g){
	// either 'check_b' is false or its not found in the list of knowngraphs
	int freq_i=0,lostfreq_i=0;
	if(graph_g->GetGraphType() == GRAPH_EMPTY)
		freq_i = minSupport_i;
	else
	{
		for(int i=0; i < noOfdbAll_i; i++)
		{
			if(graph_g->IGetGraphID() == dbAll_g[i].IGetGraphID())
				freq_i++;
			else if(graph_g->BIsSubGraphOf(&(dbAll_g[i])))
				freq_i++;
			else
				lostfreq_i++;

			// if -c option is not specified, then break once you know its
			// frequent. Else compute the exact frequency
			if(shouldCount == false)
			{
				if(freq_i >= minSupport_i)
					break;
				if(lostfreq_i > noOfdbAll_i-minSupport_i)
				{
					freq_i = minSupport_i-1;
					break;
				}
			}
		}
	}

	if(freq_i >= minSupport_i)
		return true;
	else
		return false;
}

int ComputeFrequency(Graph *graph_g){
	// either 'check_b' is false or its not found in the list of knowngraphs
	int freq_i=0,lostfreq_i=0;
	if(graph_g->GetGraphType() == GRAPH_EMPTY)
		freq_i = minSupport_i;
	else
	{
		// Not found or check_b is set false so have to compute from the database
		// The actual value of the frequency is not computed. Only if its above
		// or below the frequency is found
		for(int i=0; i < noOfdbAll_i; i++)
		{
			if(graph_g->IGetGraphID() == dbAll_g[i].IGetGraphID())
				freq_i++;
			else if(graph_g->BIsSubGraphOf(&(dbAll_g[i])))
				freq_i++;
			else
				lostfreq_i++;
		}
	}
	return freq_i;
}

int ComputeFrequencyWithGraphID(Graph *graph_g, vector<int> &FoundGraphID){
	// either 'check_b' is false or its not found in the list of knowngraphs
	int freq_i=0,lostfreq_i=0;
	if(graph_g->GetGraphType() == GRAPH_EMPTY)
		freq_i = minSupport_i;
	else
	{
		// Not found or check_b is set false so have to compute from the database
		// The actual value of the frequency is not computed. Only if its above
		// or below the frequency is found
		for(int i=0; i < noOfdbAll_i; i++)
		{
			if(graph_g->IGetGraphID() == dbAll_g[i].IGetGraphID()){
				freq_i++;
				FoundGraphID.push_back(dbAll_g[i].IGetGraphID());
			}
			else if(graph_g->BIsSubGraphOf(&(dbAll_g[i]))){
				freq_i++;
				FoundGraphID.push_back(dbAll_g[i].IGetGraphID());
			}
			else
				lostfreq_i++;
		}
	}
	return freq_i;
}

bool FindFrequency(map<int, map<int, knownGraphNode*> > &knownGraphsMap, Graph *graph_g, Triplet *trip){
	if(IsThisGraphFrequentAndFound(knownGraphsMap, graph_g, trip))
	{
		return true;
	}
	if(IsThisGraphInfrequentAndFound(knownGraphsMap, graph_g, trip))
	{
		return false;
	}

	return FindOnlyFrequency(knownGraphsMap, graph_g, trip);
}

void AddThisToCut(Triplet trip, int sourceID, int targetID){
	noOfUnmergedCuts++;
	if(targetID == -1)
	{
		cutsOnEmptyGraph.push_back(sourceID);
		return;
	}
	cutEdgeIndex[sourceID][targetID].push_back(trip);
	noOfCuts++;
}

bool IsThisCutExisting(Triplet trip, int sourceID, int targetID){
	if(targetID == -1)
	{
		for(int i=0; i < cutsOnEmptyGraph.size(); i++)
			if(cutsOnEmptyGraph[i] == sourceID)
				return true;
		return false;
	}

	for(int i=0; i < cutEdgeIndex[sourceID][targetID].size(); i++)
	{
		if(cutEdgeIndex[sourceID][targetID][i].Equals(trip))
			return true;
	}
	return false;
}

bool BGetRepresentativeCut_TopDown(map<int, map<int, knownGraphNode* > > &knownGraphsMap, Graph *graph_g, Triplet *childTrip, Triplet *parentTrip, int *sourceID, int *targetID){
	Triplet superTrip;
	if(FindFrequency(knownGraphsMap,graph_g, &superTrip))
		return false; // graph itself is frequent


	while(knownGraphsMap[superTrip.GetFirst()][superTrip.GetSecond()]->knownInfreqGraphs[superTrip.GetThird()]->GetGraphType() != GRAPH_EMPTY)
	{
		Graph *child_g = new Graph();
		knownGraphsMap[superTrip.GetFirst()][superTrip.GetSecond()]->knownInfreqGraphs[superTrip.GetThird()]->GetOnlyOneChild(child_g);
		if(child_g->GetGraphType() == GRAPH_EMPTY)
		{
			parentTrip->Copy(superTrip);
			childTrip->SetValues(-1,-1,-1);
			(*sourceID) = child_g->GetNodeAddedOrRemoved();
			(*targetID) = -1;

			return true;
		}
		Triplet subTrip;
		if(FindFrequency(knownGraphsMap,child_g, &subTrip))
		{
			parentTrip->Copy(superTrip);
			childTrip->Copy(subTrip);
			child_g->StoreEdgeAddedOrRemoved(sourceID, targetID);
			delete child_g;
			return true;
		}
		delete child_g;
		superTrip.Copy(subTrip);
		// otherwise infrequent. trip is already updated
	}
	ErrorExit("Graph::BGetRepresentativeCut_TopDown", "Error in finding the representative");
}

bool BGetRepresentativeCut_BottomUp(map<int, map<int, knownGraphNode*> > &knownGraphsMap, Graph *graph_g, Triplet *childTrip, Triplet *parentTrip, int *sourceID, int *targetID){
	// find the empty graph child of graph_g
	Graph *empty_g = new Graph();
	graph_g->GetEmptyChild(empty_g);
	empty_g->SetGraphCode("");
	Graph *parent_g = new Graph();
	empty_g->GetOnlyOneParent(parent_g);


	Triplet subTrip;
	if(!FindFrequency(knownGraphsMap,parent_g, &subTrip)) // parent is infrequent
	{
		if(!findIfExist(knownGraphsMap, 0, 0)){
			knownGraphsMap[0][0] = new knownGraphNode();
			InitializeknownGraphNode(knownGraphsMap[0][0], 0, 0);
		}

		int tempIndex = knownGraphsMap[0][0]->knownFreqGraphs.size();
		knownGraphsMap[0][0]->knownFreqGraphs.push_back(empty_g);
		childTrip->SetValues(0,0,tempIndex);
		parentTrip->Copy(subTrip);
		(*sourceID) = parent_g->GetNodeAddedOrRemoved();
		(*targetID) = -1;

		delete parent_g;
		return true;
	}

	delete parent_g;
	while(knownGraphsMap[subTrip.GetFirst()][subTrip.GetSecond()]->knownFreqGraphs[subTrip.GetThird()]->BItIsNotG_i())
	{
		Graph *graphParent_g = new Graph();
		knownGraphsMap[subTrip.GetFirst()][subTrip.GetSecond()]->knownFreqGraphs[subTrip.GetThird()]->GetOnlyOneParent(graphParent_g);
		Triplet superTrip;
		if(!FindFrequency(knownGraphsMap,graphParent_g, &superTrip)) // infrequent
		{
			childTrip->Copy(subTrip);
			parentTrip->Copy(superTrip);
			graphParent_g->StoreEdgeAddedOrRemoved(sourceID, targetID);
			delete graphParent_g;
			return true;
		}
		delete graphParent_g;
		subTrip.Copy(superTrip);
	}
	return false;
}

bool BGetRepresentativeCut(map<int, map<int, knownGraphNode*> > &knownGraphsMap, Graph *graph_g, Triplet *childTrip, Triplet *parentTrip, int *sourceID, int *targetID, bool topDown_b){
	if(topDown_b)
	{
		return BGetRepresentativeCut_TopDown(knownGraphsMap ,graph_g, childTrip, parentTrip, sourceID, targetID);
	}
	else
	{
		return BGetRepresentativeCut_BottomUp(knownGraphsMap ,graph_g, childTrip, parentTrip, sourceID, targetID);
	}
}

void StartExpandCut(int partition_id){
	noOfExpandCuts=0;
	noOfUnmergedCuts=0;
	noOfValidMaxFreq = 0;
	maxFreq.clear();
	validMaxFreq.clear();
	numOfValidCandidateGraphs = 0;
	// determine if its top down or bottom up for representative
	bool reprTopDown_b = LowFreq(minSupport_i, noOfdbAll_i);
	struct tms ts11,ts12;
	// Find the representative for each graph and call the expand cut
	cout << "Running Naive......" << endl;
	map<int, map<int, knownGraphNode*> >:: iterator it1;
	for(int i=0; i < noOfdbAll_i; i++)
	{
		cout <<"Running Graph "<< i <<endl;

		printf("\r\t%.3f%",(double)((double)i*100/(double)noOfdbAll_i));
		fflush(stdout);
		maxNoOfEdges = dbAll_g[i].IGetNoOfAllEdges();
		maxNoOfNodes = dbAll_g[i].IGetNoOfAllNodes();
		InitializeCutEdgeIndex();

		it1 = knownGraphsMap.begin();
		while(it1 != knownGraphsMap.end())
		{
			map<int, knownGraphNode*>:: iterator it2;
			it2 = it1->second.begin();
			while(it2 != it1->second.end())
			{
				it2->second->knownFreqGraphs_startindex = it2->second->knownFreqGraphs.size();
				it2->second->knownFreqGraphs_FreqVal_startindex = it2->second->knownFreqGraphs_FreqVal.size();
				it2->second->knownInfreqGraphs_startindex = it2->second->knownInfreqGraphs.size();
				it2->second->knownFreqGraphEmbeddings_startindex = it2->second->knownFreqGraphEmbeddings.size();
				it2->second->knownInFreqGraphEmbeddings_startindex = it2->second->knownInFreqGraphEmbeddings.size();
				it2++;
			}
			it1++;
		}

		noOfCuts = 0;
		localNoOfExpCuts=0;
		Triplet freqTrip,inFreqTrip;
		int sourceID, targetID;

		times(&ts11);
		if(BGetRepresentativeCut(knownGraphsMap,&dbAll_g[i], &freqTrip, &inFreqTrip, &sourceID, &targetID, reprTopDown_b))
		{
			std::cout << "Start of expand cut!!" << '\n';
			vector<bool> dummy1;
			vector<int> dummy2;
			ExpandCut(freqTrip, inFreqTrip, sourceID, targetID, false,-1, st_vec, dummy1, dummy2, knownGraphsMap);
			cout << "End of Expand Cut for graph " << i << endl;
		}
		times(&ts12);
		double time_load1 = ((double)(ts12.tms_utime-ts11.tms_utime)/(double)(double)sysconf(_SC_CLK_TCK))+((double)(ts12.tms_stime-ts11.tms_stime)/(double)(double)sysconf(_SC_CLK_TCK));
		std::cout << "Time taken    " << time_load1 << '\n';

		std::cout << "Merging start!!" << '\n';
		times(&ts11);
		FindMaxFreqAndMerge(knownGraphsMap, partition_id);
		times(&ts12);
		time_load1 = ((double)(ts12.tms_utime-ts11.tms_utime)/(double)(double)sysconf(_SC_CLK_TCK))+((double)(ts12.tms_stime-ts11.tms_stime)/(double)(double)sysconf(_SC_CLK_TCK));
		std::cout << "Time taken    " << time_load1 << '\n';
		std::cout << "Merging done!!" << '\n';
		std::cout << "local expand cuts" << localNoOfExpCuts << '\n';

		FreeCutEdgeIndex();
		cutsOnEmptyGraph.erase(cutsOnEmptyGraph.begin(), cutsOnEmptyGraph.end());
		vector<int>().swap(cutsOnEmptyGraph);
		cout << "No Of Max Freq. Subgraphs : " << noOfValidMaxFreq << endl;

		// free memory	

		while(knownGraphsMap.size() != 0)
		{
			it1 = knownGraphsMap.begin();
			map<int, knownGraphNode*>:: iterator it2;
			while(it1->second.size() !=0 )
			{
				it2 = it1->second.begin();
				int val =  it2->second->knownFreqGraphs.size();
				for(int k=0; k < val; k++)
					it2->second->knownFreqGraphs[k]->~Graph();

				val = it2->second->knownInfreqGraphs.size();
				for(int k=0; k < val; k++)
					it2->second->knownInfreqGraphs[k]->~Graph();

				it2->second->knownFreqGraphs.erase(it2->second->knownFreqGraphs.begin(),it2->second->knownFreqGraphs.end());
				vector<Graph*>().swap(it2->second->knownFreqGraphs);
				it2->second->knownInfreqGraphs.erase(it2->second->knownInfreqGraphs.begin(),it2->second->knownInfreqGraphs.end());
				vector<Graph*>().swap(it2->second->knownInfreqGraphs);

				it2->second->knownFreqGraphs_FreqVal.erase(it2->second->knownFreqGraphs_FreqVal.begin(),it2->second->knownFreqGraphs_FreqVal.end());
				vector<int>().swap(it2->second->knownFreqGraphs_FreqVal);
				it1->second.erase(it2);
			}
			knownGraphsMap.erase(it1);
		}
		seenKnownFreqGraphs.clear();

		it1 = knownGraphsMap.begin();
		while(it1 != knownGraphsMap.end())
		{
			map<int, knownGraphNode*>:: iterator it2;
			it2 = it1->second.begin();
			while(it2 != it1->second.end())
			{
				it2->second->knownFreqGraphs_startindex = 0;
				it2->second->knownFreqGraphs_FreqVal_startindex = 0;
				it2->second->knownInfreqGraphs_startindex = 0;
				it2->second->knownFreqGraphEmbeddings_startindex = 0;
				it2->second->knownInFreqGraphEmbeddings_startindex = 0;
				it2++;
			}
			it1++;
		}
	}
	
	for(int i=0; i < maxFreq.size(); i++)
	{
		if(!validMaxFreq[i])
			continue;

		int sizeOfTempGraph = maxFreq[i]->IGetNoOfAllEdges();
		bool flag = true;
		map<int, vector<Graph*> >::iterator it1 = global_local_maxfreq.begin();
		map<int, vector<vector<int> > >::iterator it2 = global_local_maxFreq_partition_wise_count.begin();
		while(it1!=global_local_maxfreq.end())
		{
			int index_found = -1;
			if(it1->first >= sizeOfTempGraph)
			{
				for(int j=0;j<it1->second.size();j++)
				{
					if(maxFreq[i]->BIsLabelSubGraph(it1->second[j]))
					{
						if(maxFreq[i]->BIsSubGraphOf(it1->second[j]))
						{
							flag = false;
							if(it1->first == sizeOfTempGraph)
								index_found = j;
							break;
						}
					}				
				}
			}
			else
			{
				for(int j=0;j<it1->second.size();j++)
				{	
					if(it1->second[j]->BIsLabelSubGraph(maxFreq[i]))
					{ 
						if(it1->second[j]->BIsSubGraphOf(maxFreq[i]))
						{
							it1->second.erase(it1->second.begin()+j);
							it2->second.erase(it2->second.begin()+j);
							j--;
						}
					}				
				}
			}
			if(!flag)
			{
				if(index_found!=-1)
				{
					it2->second[index_found][partition_id] = maxFreq_FreqVal[i];
				}
				break;
			}
			it1++;
			it2++;
		}
		
		if(flag)
		{
			Graph *g = new Graph();
			g->Copy(maxFreq[i]);
			global_local_maxfreq[sizeOfTempGraph].push_back(g);
			vector<int> temppartitioncount(num_of_partition, -1);
			temppartitioncount[partition_id] = maxFreq_FreqVal[i];
			global_local_maxFreq_partition_wise_count[sizeOfTempGraph].push_back(temppartitioncount);
			numOfValidCandidateGraphs++;
		}
	}

	cout << "--------------------------------------------" << endl;
	cout << "No Of Max Freq. Subgraphs in this partition: " << numOfValidCandidateGraphs << endl;
}

void ExpandCut(Triplet freqTrip, Triplet inFreqTrip, int sourceID, int targetID, bool bFlagCheck,
		int noOfSeenGraphs, vector<Graph*> seenGraphs, vector<bool> isSeenGraphFreq,
		vector<int> isSeenAlreadyThere,map<int, map<int, knownGraphNode*> > &knownGraphsMap){
	/********************** Algorithm ************************
	 *
	 * 1. Add (indexCutFreq_i -- indexCutInfreq_i) to the list of cuts
	 *
	 * 2. Find all children of indexCutsInfreq_i
	 *      -   Let F1, F2, ... be the frequent children
	 *      -       G1, G2, ... be the infrequent children
	 *
	 * 3. For each Fi do
	 *      3.1. Add (Fi -- indexCutInfreq_i) to the list of cuts
	 *      3.2. Find all parents of Fi
	 *           -  Let D1, D2, ... be the frequent parents
	 *           -  Let E1, E2, ... be the infrequent parents
	 *          3.2.1. For each Ei do
	 *                  3.2.1.1. Call 'ExpandCut(Fi -- Ei)'
	 *          3.2.2. For each Di do
	 *                  3.2.2.1. Find common parent of Di and indexCutInfreq_i: Ci
	 *                  3.2.2.2. Call 'ExpandCut(Di -- Ci)'
	 *
	 * old_4. For each Gi do
	 *      old_4.1. Find common child of indexCutFreq_i and Gi: Hi
	 *      old_4.2. Call 'ExpandCut(Hi -- Gi)'
	 *
	 * 4. For each Gi do
	 *      4.1. Find all the children H1, H2, ...
	 *      4.2. Find the frequent child Hi and call Expand Cut (Hi -- Gi)
	 *           (if none of the children H1, H2, ... are frequent return error)
	 ******************** End of Algorithm *********************
	 * */

	#if 0
		knownGraphsMap[freqTrip.GetFirst()][freqTrip.GetSecond()]->knownFreqGraphs[freqTrip.GetThird()]->Print();
		cout << " -> ";
		knownGraphsMap[inFreqTrip.GetFirst()][inFreqTrip.GetSecond()]->knownInfreqreqGraphs[inFreqTrip.GetThird()]->Print();
		cout << endl;
	#endif

	#if 0 // debug
		cout<<"*********************************************"<<endl;
		cout << "----------Enter Expand Cut:\n";
		freqTrip.Print();
		cout << "\t\t\t";
		inFreqTrip.Print();

		cout << "----------------------" << endl;
		cout << "In Expand Cut: " << endl;
		cout << "Freq Graph : ";

		cout << "----------------------" << endl;
		cout<<"*********************************************"<<endl;
		getchar();
		getchar();
	#endif

	if(IsThisCutExisting(freqTrip, sourceID, targetID))
		return;

	noOfExpandCuts++;
	localNoOfExpCuts++;
	/*
	 * 1. Add (indexCutFreq_i -- indexCutInfreq_i) to the list of cuts
	 * */
	AddThisToCut(freqTrip, sourceID, targetID);

	int tempInt;
	/*
	 * 2. Find all children of indexCutsInfreq_i
	 *      -   Let F1, F2, ... be the frequent children
	 *      -       G1, G2, ... be the infrequent children
	 * */
	int noOfChildren_i;
	vector<Graph*> children_g;
	vector<Triplet> F_Vi, G_Vi; // F_Vi: frequent. G_Vi: infrequent
	vector<int> edgeForF_ViSource,edgeForF_ViTarget;
	if(noOfSeenGraphs == -1)
	{
		children_g = knownGraphsMap[inFreqTrip.GetFirst()][inFreqTrip.GetSecond()]->knownInfreqGraphs[inFreqTrip.GetThird()]->GetChildren(&noOfChildren_i);
		for(int i=0; i < noOfChildren_i; i++)
		{
			Triplet childTrip;
			// new implementation
			if(IsThisGraphFrequentAndFound(knownGraphsMap, children_g[i], &childTrip))
			{
				if(knownGraphsMap[freqTrip.GetFirst()][freqTrip.GetSecond()]->knownFreqGraphs[freqTrip.GetThird()]->BIsCodeSubGraphOf(children_g[i]))
				{
					F_Vi.push_back(childTrip);
					int temp1,temp2;
					children_g[i]->StoreEdgeAddedOrRemoved(&temp1,&temp2);
					edgeForF_ViSource.push_back(temp1);
					edgeForF_ViTarget.push_back(temp2);
				}
				else // Add this to the cut
				{
					if( children_g[i]->IGetNoOfValidNodes()==1)
					{
						assert(children_g[i]->IGetNoOfAllEdges()==0);
						F_Vi.push_back(childTrip);
						int temp3,temp4;
						children_g[i]->StoreEdgeAddedOrRemoved(&temp3,&temp4);
						edgeForF_ViSource.push_back(temp3);
						edgeForF_ViTarget.push_back(temp4);
					}
					else//optimisation. if freq node visited and not part of the calling cut then just add to cut. not to call expand cut
					{
						int temp1,temp2;
						children_g[i]->StoreEdgeAddedOrRemoved(&temp1,&temp2);
						if(!IsThisCutExisting(childTrip, temp1, temp2))
							AddThisToCut(childTrip, temp1, temp2);
					}
				}
			}
			else if(IsThisGraphInfrequentAndFound(knownGraphsMap, children_g[i], &childTrip));
			else if(FindOnlyFrequency(knownGraphsMap, children_g[i], &childTrip))
			{
				if(!bFlagCheck)
				{
					F_Vi.push_back(childTrip);
					int temp1,temp2;
					children_g[i]->StoreEdgeAddedOrRemoved(&temp1,&temp2);
					edgeForF_ViSource.push_back(temp1);
					edgeForF_ViTarget.push_back(temp2);
				}
				else // Add this to the cut
				{
					int temp1,temp2;
					children_g[i]->StoreEdgeAddedOrRemoved(&temp1,&temp2);
					if(!IsThisCutExisting(childTrip, temp1, temp2))
						AddThisToCut(childTrip, temp1, temp2);
				}
			}
			else
			{
        		G_Vi.push_back(childTrip);
      		}
			children_g[i]->~Graph();
		}
		#if 0 // debug
			cout << "3: No Of F_Vi: " << F_Vi.size() << endl;
			for(int i=0; i < F_Vi.size(); i++)
			{
				cout << "F_Vi: " << i << endl;
				F_Vi[i].Print();
				cout << endl;
				PrintKnownFreqGraph(knownGraphsMap, F_Vi[i]);
			}
		#endif
	}
	else // noOfSeenGraphs != -1
	{
		for(int i=0; i < noOfSeenGraphs; i++)
		{
			Triplet childTrip;
			// new implementation
			if(isSeenGraphFreq[i] && (isSeenAlreadyThere[i] != -1))
			{
				if(knownGraphsMap[freqTrip.GetFirst()][freqTrip.GetSecond()]->knownFreqGraphs[freqTrip.GetThird()]->BIsCodeSubGraphOf(seenGraphs[i]))
				{
					F_Vi.push_back(freqTrip);
					int temp1,temp2;
					seenGraphs[i]->StoreEdgeAddedOrRemoved(&temp1,&temp2);
					edgeForF_ViSource.push_back(temp1);
					edgeForF_ViTarget.push_back(temp2);
				}
				else // add this to cut
				{
					int r = seenGraphs[i]->IGetNoOfAllEdges();
					int c = seenGraphs[i]->IGetNoOfValidNodes();

					int indexID;
					if(findIfExist(knownGraphsMap, r ,c ))
						indexID = knownGraphsMap[r][c]->knownFreqGraphs.size();
					else
						indexID = 0;

					Triplet childTrip(r,c, indexID);
					int temp1,temp2;
					seenGraphs[i]->StoreEdgeAddedOrRemoved(&temp1,&temp2);
					if(!IsThisCutExisting(childTrip, temp1, temp2))
						AddThisToCut(childTrip, temp1, temp2);

					// New Code: The following could be an optimization.
					// But due to this return, some problems are coming.
					// So as of now disabling it. Need to check it back.
					//return;
				}
			}
			else if( (!isSeenGraphFreq[i]) && (isSeenAlreadyThere[i] != -1));
			// frequent but not already there
			else if(isSeenGraphFreq[i] && (isSeenAlreadyThere[i] == -1))
			{
				if(!bFlagCheck)
				{
					int r = seenGraphs[i]->IGetNoOfAllEdges();
					int c = seenGraphs[i]->IGetNoOfValidNodes();
					int indexID;
					if(findIfExist(knownGraphsMap, r ,c ))
						indexID = knownGraphsMap[r][c]->knownFreqGraphs.size();
					else{
						indexID = 0;
						knownGraphsMap[r][c] = new knownGraphNode();
						InitializeknownGraphNode(knownGraphsMap[r][c], r, c);
					}

					knownGraphsMap[r][c]->knownFreqGraphs.push_back(seenGraphs[i]);

					if(shouldCount)
					{
						int tempFreq = ComputeFrequency(seenGraphs[i]);
						knownGraphsMap[r][c]->knownFreqGraphs_FreqVal.push_back(tempFreq);
					}

					Triplet childTrip(r,c, indexID);
					F_Vi.push_back(childTrip);
					int temp1,temp2;
					seenGraphs[i]->StoreEdgeAddedOrRemoved(&temp1,&temp2);
					edgeForF_ViSource.push_back(temp1);
					edgeForF_ViTarget.push_back(temp2);
				}
				else // add this to the cut
				{
					int r = seenGraphs[i]->IGetNoOfAllEdges();
					int c = seenGraphs[i]->IGetNoOfValidNodes();
					int indexID;
					if(findIfExist(knownGraphsMap, r ,c ))
						indexID = knownGraphsMap[r][c]->knownFreqGraphs.size();
					else
						indexID = 0;

					Triplet childTrip(r,c, indexID);
					int temp1,temp2;
					seenGraphs[i]->StoreEdgeAddedOrRemoved(&temp1,&temp2);
					if(!IsThisCutExisting(childTrip, temp1, temp2))
						AddThisToCut(childTrip, temp1, temp2);
				}
			}
			// infrequent but not there already
			else
			{
				int r = seenGraphs[i]->IGetNoOfAllEdges();
				int c = seenGraphs[i]->IGetNoOfValidNodes();

				int indexID;
				if(findIfExist(knownGraphsMap, r ,c ))
					indexID = knownGraphsMap[r][c]->knownInfreqGraphs.size();
				else{
					indexID = 0;
					knownGraphsMap[r][c] = new knownGraphNode();
					InitializeknownGraphNode(knownGraphsMap[r][c], r, c);
				}

				knownGraphsMap[r][c]->knownInfreqGraphs.push_back(seenGraphs[i]);
				Triplet childTrip(r,c, indexID);
				G_Vi.push_back(childTrip);
			}
		}
	}

	#if 0 // debug
		cout << "3: No Of F_Vi: " << F_Vi.size() << endl;
		for(int i=0; i < F_Vi.size(); i++)
		{
			cout << "F_Vi: " << i << endl;
			F_Vi[i].Print();
			cout << endl;
			PrintKnownFreqGraph(knownGraphsMap, F_Vi[i]);
		}

		cout << "3: No Of G_Vi: " << G_Vi.size() << endl;
		for(int i=0; i < G_Vi.size(); i++)
		{
			cout << "G_Vi: " << i << endl;
			PrintKnownInfreqGraph(knownGraphsMap, G_Vi[i]);
		}
		getchar(); getchar();
	#endif

	/*
	 * 3. For each Fi do
	 *      3.1. Add (Fi -- indexCutInfreq_i) to the list of cuts
	 *      3.2. Find all parents of Fi
	 *           -  Let D1, D2, ... be the frequent parents
	 *           -  Let E1, E2, ... be the infrequent parents
	 *          3.2.1. For each Ei do
	 *                  3.2.1.1. Call 'ExpandCut(Fi -- Ei)'
	 *          3.2.2. For each Di do
	 *                  3.2.2.1. Find common parent of Di and indexCutInfreq_i: Ci
	 *                  3.2.2.2. Call 'ExpandCut(Di -- Ci)'
	 * */

	for(int i=0; i < F_Vi.size(); i++)
	{
		if(!IsThisCutExisting(F_Vi[i], edgeForF_ViSource[i], edgeForF_ViTarget[i]))
			AddThisToCut(F_Vi[i], edgeForF_ViSource[i], edgeForF_ViTarget[i]);

		if(knownGraphsMap[F_Vi[i].GetFirst()][F_Vi[i].GetSecond()]->knownFreqGraphs[F_Vi[i].GetThird()]->GetGraphType() != GRAPH_EMPTY)
		{
			vector<Graph*> parentsOfFi;
			vector<int> toBeAdded;

			if(knownGraphsMap[F_Vi[i].GetFirst()][F_Vi[i].GetSecond()]->knownFreqGraphs[F_Vi[i].GetThird()]->GetFirstFreqParent(noOfdbAll_i, dbAll_g, knownGraphsMap, minSupport_i, &parentsOfFi, &toBeAdded)==-1)
			{
				vector<Triplet> setOfE_Vi;
				vector<int> sourceIDs,targetIDs;

				// call expand cut on each of the infrequent ones
				// 3.2.1
				for(int j=0; j < parentsOfFi.size(); j++)
				{
					int r = parentsOfFi[j]->IGetNoOfAllEdges();
					int c = parentsOfFi[j]->IGetNoOfValidNodes();
					int indexID;

					if(toBeAdded[j] == -1) // to be added to infrequent ones
					{
						if(findIfExist(knownGraphsMap, r, c))
							indexID = knownGraphsMap[r][c]->knownInfreqGraphs.size();
						else{
							indexID = 0;
							knownGraphsMap[r][c] = new knownGraphNode();
						}
						knownGraphsMap[r][c]->knownInfreqGraphs.push_back(parentsOfFi[j]);
					}
					else
						indexID = toBeAdded[j];

					Triplet E_Vi(r, c, indexID);
					setOfE_Vi.push_back(E_Vi);
					int temp1,temp2;
					parentsOfFi[j]->StoreEdgeAddedOrRemoved(&temp1,&temp2);
					sourceIDs.push_back(temp1);
					targetIDs.push_back(temp2);
				}

				for(int j=0; j<setOfE_Vi.size(); j++)
				{
					if(!IsThisCutExisting(F_Vi[i], sourceIDs[j], targetIDs[j]))
					{
						vector<bool> dummy1;
						vector<int> dummy2;
						ExpandCut(F_Vi[i], setOfE_Vi[j], sourceIDs[j], targetIDs[j], false, -1, st_vec, dummy1, dummy2, knownGraphsMap);
					}
				}
				setOfE_Vi.erase(setOfE_Vi.begin(), setOfE_Vi.end());
				vector<Triplet>().swap(setOfE_Vi);
				if(sourceIDs.size() != 0)
				{
					sourceIDs.erase(sourceIDs.begin(), sourceIDs.end());
					targetIDs.erase(targetIDs.begin(), targetIDs.end());
					vector<int>().swap(sourceIDs);
					vector<int>().swap(targetIDs);
				}
			}
			else
			{
				int r = parentsOfFi[0]->IGetNoOfAllEdges();
				int c = parentsOfFi[0]->IGetNoOfValidNodes();
				int indexID;
				if(toBeAdded[0] == -1) // to be added to frequent ones
				{
					if(findIfExist(knownGraphsMap, r, c))
						indexID = knownGraphsMap[r][c]->knownFreqGraphs.size();
					else{
						indexID = 0;
						knownGraphsMap[r][c] = new knownGraphNode();
						InitializeknownGraphNode(knownGraphsMap[r][c], r, c);
					}

					knownGraphsMap[r][c]->knownFreqGraphs.push_back(parentsOfFi[0]);

					if(shouldCount)
					{
						int tempFreq = ComputeFrequency(parentsOfFi[0]);
						knownGraphsMap[r][c]->knownFreqGraphs_FreqVal.push_back(tempFreq);
					}
				}
				else
					indexID = toBeAdded[0];

				Triplet D_Vi(r, c, indexID);
				int temp1, temp2;
				parentsOfFi[0]->StoreEdgeAddedOrRemoved(&temp1, &temp2);
				if(knownGraphsMap[inFreqTrip.GetFirst()][inFreqTrip.GetSecond()]->knownInfreqGraphs[inFreqTrip.GetThird()]->GetGraphType() == GRAPH_SINGLE_NODE)
				{
					Graph *common_g = new Graph();
					if(common_g->BGetCommonParentSingleNodes(
								(knownGraphsMap[D_Vi.GetFirst()][D_Vi.GetSecond()]->knownFreqGraphs[D_Vi.GetThird()]),
								(knownGraphsMap[inFreqTrip.GetFirst()][inFreqTrip.GetSecond()]->knownInfreqGraphs[inFreqTrip.GetThird()])))
					{

						Triplet commonTrip;
						FindFrequency(knownGraphsMap, common_g, &commonTrip); // always be infrequent
						int commonCutSourceID, commonCutTargetID;
						common_g->StoreEdgeAddedOrRemoved(&commonCutSourceID, &commonCutTargetID);
						if(!IsThisCutExisting(D_Vi, commonCutSourceID, commonCutTargetID))
						{
							vector<bool> dummy1;
							vector<int> dummy2;
							ExpandCut(D_Vi, commonTrip, commonCutSourceID, commonCutTargetID, true, -1, st_vec, dummy1, dummy2, knownGraphsMap);
						}
					}
					common_g->~Graph();
				}
				else
				{
					Graph *common_g = new Graph();
					common_g->GetCommonParent(
							knownGraphsMap[D_Vi.GetFirst()][D_Vi.GetSecond()]->knownFreqGraphs[D_Vi.GetThird()],
							knownGraphsMap[inFreqTrip.GetFirst()][inFreqTrip.GetSecond()]->knownInfreqGraphs[inFreqTrip.GetThird()]);

					Triplet commonTrip;
					FindFrequency(knownGraphsMap, common_g, &commonTrip);

					int commonCutSourceID, commonCutTargetID;
					common_g->StoreEdgeAddedOrRemoved(&commonCutSourceID, &commonCutTargetID);
					if(!IsThisCutExisting(D_Vi, commonCutSourceID, commonCutTargetID))
					{
						vector<bool> dummy1;
						vector<int> dummy2;
						ExpandCut(D_Vi, commonTrip, commonCutSourceID, commonCutTargetID, true, -1, st_vec, dummy1, dummy2,knownGraphsMap);
					}

					common_g->~Graph();

				}
			}

		}
		else // if this is empty
		{
			#if 1
					// 3.2. Find all parents of Fi
					int noOfParents_i;
					vector<Graph*> parents_g;
					vector<Triplet> D_Vi, E_Vi; // D_Vi: frequent. E_Vi: infrequent
					vector<int> edgeForE_ViSource,edgeForE_ViTarget;
					parents_g = knownGraphsMap[F_Vi[i].GetFirst()][F_Vi[i].GetSecond()]->knownFreqGraphs[F_Vi[i].GetThird()]->GetParents(&noOfParents_i);

					/*
					*  Let D1, D2, ... be the frequent parents
					*  Let E1, E2, ... be the infrequent parents
					* */
					int flag_b=false;
					for(int j=0; j < noOfParents_i; j++)
					{
						Triplet parentTrip;
						if(FindFrequency(knownGraphsMap,parents_g[j], &parentTrip))
						{
							if(flag_b)
								;
							else if(parents_g[j]->BIsExactlyEqualTo(knownGraphsMap[inFreqTrip.GetFirst()][inFreqTrip.GetSecond()]->knownInfreqGraphs[inFreqTrip.GetThird()]))
								;
							else // already flag_b is false
							{
								D_Vi.push_back(parentTrip);
								// Old Code: I think this is also some optimization. But
								// this caused some problem. So commenting it. Need to check this.
								//flag_b = true;
								flag_b = false;
							}
						}
						else
						{
							E_Vi.push_back(parentTrip);
							int temp1,temp2;
							parents_g[j]->StoreEdgeAddedOrRemoved(&temp1,&temp2);
							edgeForE_ViSource.push_back(temp1);
							edgeForE_ViTarget.push_back(temp2);
						}
						delete parents_g[j]; // Satya: 1st Feb
					}

					/*
					* 3.2.1. For each Ei do
					* 3.2.1.1. Call 'ExpandCut(Fi -- Ei)'
					* */
					for(int j=0; j < E_Vi.size(); j++)
					{
						if(!IsThisCutExisting(F_Vi[i], edgeForE_ViSource[j], edgeForE_ViTarget[j]))
						{
							vector<bool> dummy1;
							vector<int> dummy2;
							ExpandCut(F_Vi[i], E_Vi[j], edgeForE_ViSource[j], edgeForE_ViTarget[j], false, -1, st_vec, dummy1, dummy2, knownGraphsMap);
							// calling ExpandCut on just one E_Vi is sufficient
							// probably this is true. Check
							// break;
						}
					}

					/*
					* 3.2.2. For each Di do
					*      3.2.2.1. Find common parent of Di and indexCutInfreq_i: Ci
					*      3.2.2.2. Call 'ExpandCut(Di -- Ci)'
					* */
					if(knownGraphsMap[inFreqTrip.GetFirst()][inFreqTrip.GetSecond()]->knownInfreqGraphs[inFreqTrip.GetThird()]->GetGraphType() == GRAPH_SINGLE_NODE)
					{
						for(int j=0; j < D_Vi.size(); j++)
						{
							Graph *common_g = new Graph();
							if(common_g->BGetCommonParentSingleNodes(
										(knownGraphsMap[D_Vi[j].GetFirst()][D_Vi[j].GetSecond()]->knownFreqGraphs[D_Vi[j].GetThird()]),
										(knownGraphsMap[inFreqTrip.GetFirst()][inFreqTrip.GetSecond()]->knownInfreqGraphs[inFreqTrip.GetThird()])))
							{
								Triplet commonTrip;
								FindFrequency(knownGraphsMap, common_g, &commonTrip); // always be infrequent
								int commonCutSourceID, commonCutTargetID;
								common_g->StoreEdgeAddedOrRemoved(&commonCutSourceID, &commonCutTargetID);
								if(!IsThisCutExisting(D_Vi[j], commonCutSourceID, commonCutTargetID))
								{
									vector<bool> dummy1;
									vector<int> dummy2;
									// Old Code: 'true' for bCheckFlag. This could be some optimization.
									// Need to check it
									ExpandCut(D_Vi[j], commonTrip, commonCutSourceID, commonCutTargetID, true, -1, st_vec, dummy1, dummy2, knownGraphsMap);
								}
							}
							common_g->~Graph();
						}
					}
					else
					{
						for(int j=0; j < D_Vi.size(); j++)
						{
							if((knownGraphsMap[D_Vi[j].GetFirst()][D_Vi[j].GetSecond()]->knownFreqGraphs[D_Vi[j].GetThird()])->BIsExactlyEqualTo(knownGraphsMap[inFreqTrip.GetFirst()][inFreqTrip.GetSecond()]->knownInfreqGraphs[inFreqTrip.GetThird()]))
								continue;

							Graph *common_g = new Graph();

							common_g->GetCommonParent(
									knownGraphsMap[D_Vi[j].GetFirst()][D_Vi[j].GetSecond()]->knownFreqGraphs[D_Vi[j].GetThird()],
									knownGraphsMap[inFreqTrip.GetFirst()][inFreqTrip.GetSecond()]->knownInfreqGraphs[inFreqTrip.GetThird()]);

							Triplet commonTrip;
							FindFrequency(knownGraphsMap, common_g, &commonTrip);

							int commonCutSourceID, commonCutTargetID;
							common_g->StoreEdgeAddedOrRemoved(&commonCutSourceID, &commonCutTargetID);
							if(!IsThisCutExisting(D_Vi[j], commonCutSourceID, commonCutTargetID))
							{
								vector<bool> dummy1;
								vector<int> dummy2;
								ExpandCut(D_Vi[j], commonTrip, commonCutSourceID, commonCutTargetID, true, -1, st_vec, dummy1, dummy2,knownGraphsMap);
							}
							common_g->~Graph();
						}
					}
			#endif
		}
	}

	/*
	 * 4. For each Gi do
	 *      4.1. Find first frequent child H1 and call Expand Cut (Hi -- Gi)
	 *           (if none of the children H1, H2, ... are frequent return error)
	 * */

	/*
	 * CHANGE: Check if the grandChildren[j] is already in the known freq
	 * graphs before you push it into the known frequent graphs
	 * */
	for(int i=0; i < G_Vi.size(); i++)
	{
		//To find the first frequent child by not adding the infrequent children found until then to the infreqmatrix.
		int noOfGrandChildren_i;
		vector<Graph*> grandChildren_g;

		grandChildren_g = knownGraphsMap[G_Vi[i].GetFirst()][G_Vi[i].GetSecond()]->knownInfreqGraphs[G_Vi[i].GetThird()]->GetChildren(&noOfGrandChildren_i);


		vector<bool> isFrequent;
		vector<int> alreadyThere;
		int frequentID=-1;
		bool toBePushed = false;
		int r = -1, c = -1, indexID = -1;
		for(int j=0; j < noOfGrandChildren_i; j++)
		{
			Triplet tempTrip;
			if(IsThisGraphFrequentAndFound(knownGraphsMap, grandChildren_g[j],&tempTrip))
			{
				isFrequent.push_back(true);
				alreadyThere.push_back(tempTrip.GetThird());
				if(frequentID == -1)
				{
					frequentID = j;
					indexID = tempTrip.GetThird();
				}
			}
			else if(IsThisGraphInfrequentAndFound(knownGraphsMap, grandChildren_g[j], &tempTrip))
			{
				isFrequent.push_back(false);
				alreadyThere.push_back(tempTrip.GetThird());
			}
			else if(FindOnlyFrequencyDontAdd(grandChildren_g[j])) // frequent
			{
				isFrequent.push_back(true);
				alreadyThere.push_back(-1);
				if(frequentID == -1) // remember the first frequent one
				{
					frequentID = j;
					toBePushed = true;
				}
			}
			else // infrequent
			{
				isFrequent.push_back(false);
				alreadyThere.push_back(-1);
			}
		}

		if(frequentID != -1)
		{
			r = grandChildren_g[frequentID]->IGetNoOfAllEdges();
			c = grandChildren_g[frequentID]->IGetNoOfValidNodes();
			if(toBePushed)
			{
				// first add this to list of known frequent graphs
				if(findIfExist(knownGraphsMap, r, c)){
					indexID = knownGraphsMap[r][c]->knownFreqGraphs.size();
				}
				else{
					indexID = 0;
					knownGraphsMap[r][c] = new knownGraphNode();
					InitializeknownGraphNode(knownGraphsMap[r][c], r, c);
				}

				knownGraphsMap[r][c]->knownFreqGraphs.push_back(grandChildren_g[frequentID]);
				if(shouldCount)
				{
					int tempFreq = ComputeFrequency(grandChildren_g[frequentID]);
					knownGraphsMap[r][c]->knownFreqGraphs_FreqVal.push_back(tempFreq);
				}
			}

			alreadyThere[frequentID] = indexID;
			Triplet grandTriplet(r,c,indexID);
			int grandSourceID, grandTargetID;
			grandChildren_g[frequentID]->StoreEdgeAddedOrRemoved(&grandSourceID, &grandTargetID);
			if(!IsThisCutExisting(grandTriplet, grandSourceID, grandTargetID))
			{
				// pass the other parameters also
				ExpandCut(grandTriplet, G_Vi[i], grandSourceID, grandTargetID, false, noOfGrandChildren_i, grandChildren_g, isFrequent, alreadyThere, knownGraphsMap);
			}
		}
	}

	#if 0 // debug
		cout << "**********Exit Expand Cut:\n";
		freqTrip.Print();
		cout << "\t\t\t";
		inFreqTrip.Print();

		cout << "----------------------" << endl;
		cout << "Freq Graph : ";
		knownGraphsMap[freqTrip.GetFirst()][freqTrip.GetSecond()]->knownFreqGraphs[freqTrip.GetThird()]->Print();
		cout << endl << "Inreq Graph : ";
		knownGraphsMap[inFreqTrip.GetFirst()][inFreqTrip.GetSecond()]->knownInfreqGraphs[inFreqTrip.GetThird()]->Print();
		cout << "Source: " << sourceID << "- Target: " << targetID << endl;
		PrintCuts(knownGraphsMap);
		cout << "**********************" << endl;
	#endif

}

#endif
