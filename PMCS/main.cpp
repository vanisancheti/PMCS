/*
 * File Info: main.cpp
 * Description: This file contains the 'main()' function. The starting point of the code.
 * */
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <cassert>
#include <string.h>
#include <algorithm>
#include <time.h>
#include <sys/times.h>
#include "Graph.h"

int noOfdbAll_i;
int num_of_partition;
Graph *dbAll_g;
Graph *max_graphsdbAll_g;
int minSupport_i;
int total_graphs;
int local_graphs;
bool isOutputFile;
char *outputFile_c;
bool isLocalFile;
char *localFile_c;
bool isLocalComputation;
bool shouldCount;

typedef struct cand_graph {
	Graph* g;
	map<int, vector<struct cand_graph*> > subgraphs;
	int validity;
	boost::dynamic_bitset<> graphIDsBitset;
	bool localFreq;
	struct cand_graph* parent;

	cand_graph(){
		localFreq = false;
		parent = NULL;
	}

	~cand_graph()
    {
		delete g;
		for(int i=0;i<subgraphs.size();i++){
			subgraphs[i].erase(subgraphs[i].begin(), subgraphs[i].end());
			vector<struct cand_graph*>().swap(subgraphs[i]);
		}
    } 
} cand_graph;

map<int, vector<cand_graph*> > globalMaxFreq;

map<int, vector<Graph*> > global_maxfreq; 
map<int, vector<int> > validglobal_maxfreq; // 0 Infreq; 1 Freq; 2 Subgraph of Freq 

vector < vector < vector<Graph*> > > knownFreqGraphs;
vector < vector < vector<int> > >  knownFreqGraphs_FreqVal;
vector < vector < vector<Graph*> > > knownInfreqGraphs;
vector <vector <vector <Embedding> > > knownFreqGraphEmbeddings;
vector <vector <vector <Embedding> > > knownInFreqGraphEmbeddings;
vector <vector <vector<bool> > > seenKnownFreqGraphs;

map<int, map<int, knownGraphNode*> > knownGraphsMap;

#include "Vector2D.h"
#include "Triplet.h"
#include "prototypes.h"
#include "expand_cut.h"


void PrintUsage(char *bin_name)
{
	cout << "Usage: " << bin_name << " <num_graphs> <graphs_file> -s <sup_val> [-c] [-o <output_file>] [-p <partition>] [-l/-g <local_file> <localcount>]" << endl;
	cout << "Usage: " << bin_name << " -help" << endl;
	exit(0);
}

main(int argc, char **argv)
{
	cout << "Number of Arguments: " << argc << endl;
    if(argc == 7)
    {
        if(strcmp(argv[3],"-s")!=0)
					PrintUsage(argv[0]);

        minSupport_i = (int)atoi(argv[4]);

        shouldCount = false;
		isLocalComputation = false;
        isOutputFile = false;
		isLocalFile = false;
		num_of_partition = (int)atoi(argv[6]);
    }
    else if(argc == 8)
			PrintUsage(argv[0]);
    else if(argc == 9)
    {
			if(strcmp(argv[3],"-s")!=0)
				PrintUsage(argv[0]);
			if(strcmp(argv[5],"-o")!=0)
				PrintUsage(argv[0]);

      minSupport_i = (int)atoi(argv[4]);
      shouldCount = false;

      isLocalComputation = false;
	  isLocalFile = false;
	  isOutputFile = true;
      outputFile_c = (char*)malloc(sizeof(char)*(strlen(argv[6])+1));
      strcpy(outputFile_c, argv[6]);
	  num_of_partition = (int)atoi(argv[8]);
    }
    else if(argc == 10)
    {
		if(strcmp(argv[3],"-s")!=0)
			PrintUsage(argv[0]);
      	if(strcmp(argv[5],"-c")!=0)
			PrintUsage(argv[0]);
		else if(strcmp(argv[6],"-o")!=0)
			PrintUsage(argv[0]);

      minSupport_i = (int)atoi(argv[4]);
      shouldCount = true;

	  isLocalComputation = false;
	  isLocalFile = false;
      isOutputFile = true;
      outputFile_c = (char*)malloc(sizeof(char)*(strlen(argv[7])+1));
      strcpy(outputFile_c, argv[7]);
	  num_of_partition = (int)atoi(argv[9]);
    }
	else if(argc == 12 || argc == 13)
    {
		cout << argv[10] << endl;
		if(strcmp(argv[3],"-s")!=0)
			PrintUsage(argv[0]);
      	if(strcmp(argv[5],"-c")!=0)
			PrintUsage(argv[0]);
		else if(strcmp(argv[6],"-o")!=0)
			PrintUsage(argv[0]);
		else if(strcmp(argv[10],"-l")!=0 && strcmp(argv[10],"-g")!=0)
			PrintUsage(argv[0]);

      minSupport_i = (int)atoi(argv[4]);
      shouldCount = true;

      isOutputFile = true;
      outputFile_c = (char*)malloc(sizeof(char)*(strlen(argv[7])+1));
      strcpy(outputFile_c, argv[7]);

	  if(strcmp(argv[10],"-l") == 0)
	  	isLocalComputation = true;
	  else{
	  	isLocalComputation = false;
		if(argc == 13)
			local_graphs = (int)atoi(argv[12]);
		else
			PrintUsage(argv[0]);
	  }
	
	  isLocalFile = true;
	  localFile_c = (char*)malloc(sizeof(char)*(strlen(argv[11])+1));
	  strcpy(localFile_c, argv[11]);

	  num_of_partition = (int)atoi(argv[9]);
	  cout << "Done reading arguments" << endl;
    }
    else if(argc == 2)
    {
				if ( (strcmp(argv[1],"-help")==0) ||
                (strcmp(argv[1], "-h")==0) )
        {
            PrintUsage(argv[0]);
        }
        else
            PrintUsage(argv[0]);
    }
    else
			PrintUsage(argv[0]);

	int tempcurGraph_i;
    tempcurGraph_i = (int)atoi(argv[1]);
	ifstream read_ifs;
	read_ifs.rdbuf()->pubsetbuf(0, 0);
	read_ifs.open(argv[2]);
    if(!read_ifs)
    {
        cout << "Unable to open " << argv[2] << endl;
        exit(0);
    }

    struct tms tms1, tms2, tms3, tms4, tms5, tms6, tms7, tms8, tms9;
	// tms1: time at the start of local computation
	// tms2: time at the start of loading a partition
	// tms3: time at the end of loading a parition
		// tms3 - tms2: time to load that particular partition
	// tms4: time at the end of expand cut on that partition
		// tms4 - tms3: expand cut time taken
	// tms5: time at the end of local MFS run (i.e at the end of local computation of maximal frequent graphs)
		// tms5 - tms1: total time take for loading each partition and running local MFS on it
	// tms6: time at the end of the global computation.
		// tms6-tms5: total time taken for global computation
		// tms6-tms1: total time taken
	// tms7: time at the end of the MCS computation with all the database graphs
	// tms8: time at the start of the MCS iteration for computing the global max freq graphs
	// tms9: time at the end of the MCS iteration for computing the global max freq graphs

	intialize_num();

	map<pair<pair<int, int>, int>, int> edge_embeddings;
	double time_Local = 0;
    times(&tms1);
    int curGraph_i = 0;
    string temp_read;
    read_ifs >> temp_read;
	int edge_cnt = 0;
	int global_min_support = minSupport_i;
	
	total_graphs = tempcurGraph_i;
	int count_per_partition = total_graphs/num_of_partition;

	map<int, map<int, knownGraphNode*> >:: iterator it1;
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

	// Module 1: finding the local maximal frequent graphs	
	if(!isLocalFile || (isLocalFile && isLocalComputation))
	{
		for(int j=0;j<num_of_partition;j++)
		{
			times(&tms2);
			noOfdbAll_i = 0;
			count_per_partition = (total_graphs - curGraph_i)/(num_of_partition-j);
			dbAll_g = new Graph[count_per_partition];
			cout << "start new partition with " << count_per_partition << " number of graphs" << endl;

			while(noOfdbAll_i < count_per_partition)
			{
				vector<int> node_ids(0), node_labs(0), edge_n1(0), edge_n2(0), edge_labs(0);
				string read1;
				read_ifs >> read1;
				if(read1 == "#")
				{
					int read2;
					read_ifs >> read2;
					assert(read2 == curGraph_i);

					string nextRead;
					read_ifs >> nextRead;
					while(nextRead != "t")
					{
						if(nextRead == "v" || nextRead == ":v")
						{
							int read_nodeID, read_nodeLab;
							read_ifs >> read_nodeID >> read_nodeLab;
							node_ids.push_back(read_nodeID);
							node_labs.push_back(read_nodeLab);
						}
						else if(nextRead == "u")
						{
							int read_edgeN1, read_edgeN2, read_edgeLab;
							read_ifs >> read_edgeN1 >> read_edgeN2 >> read_edgeLab;
							edge_n1.push_back(read_edgeN1);
							edge_n2.push_back(read_edgeN2);
							edge_labs.push_back(read_edgeLab);
						}
						read_ifs >> nextRead;
						if(read_ifs.eof())
							break;
					}
					if(!dbAll_g[noOfdbAll_i].BLoadGraph(curGraph_i,node_ids, node_labs, edge_n1, edge_n2, edge_labs))
						ErrorExit("main()", "Error in load graph");
					noOfdbAll_i++;
					curGraph_i++;
				}
			}

			minSupport_i = floor(global_min_support*noOfdbAll_i/tempcurGraph_i);

			// Running MFS
			for(int i=0; i < noOfdbAll_i; i++)
			{
				dbAll_g[i].FindGraphCode();
				dbAll_g[i].FindNewGraphCode();
			}
			int i=0;
			edge_iterator ei, ei_end, next_e1,next_e;
			times(&tms3);
			StartExpandCut(j);
			times(&tms4);

			double time_load = ((double)(tms3.tms_utime-tms2.tms_utime)/(double)(double)sysconf(_SC_CLK_TCK))+((double)(tms3.tms_stime-tms2.tms_stime)/(double)(double)sysconf(_SC_CLK_TCK));
			double time_expcut =  ((double)(tms4.tms_utime-tms3.tms_utime)/(double)(double)sysconf(_SC_CLK_TCK))+((double)(tms3.tms_stime-tms3.tms_stime)/(double)(double)sysconf(_SC_CLK_TCK));
			double time_total_partition =  ((double)(tms4.tms_utime-tms2.tms_utime)/(double)(double)sysconf(_SC_CLK_TCK))+((double)(tms4.tms_stime-tms2.tms_stime)/(double)(double)sysconf(_SC_CLK_TCK));

			cout << "ExpCut Time Taken for " << j << "th partition        : " << time_expcut << endl;
			cout << "No Of Expand Cuts for " << j << "th partition        : " << noOfExpandCuts << endl;
			cout << "Total Time Taken  for " << j << "th partition        : " << time_total_partition << endl;
			cout << "----------------------------------------------------------------" << endl;

			for(int i=maxFreq.size()-1;i>=0;i--){
				if(!validMaxFreq[i])
					continue;
				delete maxFreq[i];
			}
			maxFreq.erase(maxFreq.begin(), maxFreq.end());
			vector<Graph*>().swap(maxFreq);
			maxFreq_FreqVal.erase(maxFreq_FreqVal.begin(), maxFreq_FreqVal.end());
			vector<int>().swap(maxFreq_FreqVal);
			validMaxFreq.erase(validMaxFreq.begin(), validMaxFreq.end());
			vector<bool>().swap(validMaxFreq);

			delete [] dbAll_g;
		}

		times(&tms5);	
		double time_total_local =  ((double)(tms5.tms_utime-tms1.tms_utime)/(double)(double)sysconf(_SC_CLK_TCK))+((double)(tms5.tms_stime-tms1.tms_stime)/(double)(double)sysconf(_SC_CLK_TCK));
		cout << "Local Computation done" << endl;
		cout << "Total Time Taken for local computation for each partition        : " << time_total_local << endl;
		cout << "----------------------------------------------------------------" << endl;

		map<int, vector<Graph*> >::reverse_iterator it, find_it;
		it = global_maxfreq.rbegin();
		while(it!=global_maxfreq.rend()){
			for(int i=0;i<it->second.size();i++)
			{
				boost::dynamic_bitset<> temp_bitset(total_graphs);
				cand_graph* tempG = new cand_graph();
				tempG->g = it->second[i];
				tempG->validity = validglobal_maxfreq[it->first][i];
				tempG->graphIDsBitset = temp_bitset;
				tempG->localFreq = true;
				globalMaxFreq[it->first].push_back(tempG);
			}
			it++;
		}

		if(isLocalFile){
			map<int, vector<Graph*> >::iterator it3;
			it3 = global_maxfreq.begin();

			int tempCount=0;
			while(it3!=global_maxfreq.end()){
				for(int i=0;i<it3->second.size();i++)
				{
					Graph *g = new Graph();
					g->ExactgraphCopy(it3->second[i]);
					g->FindGraphCode();
					g->FindNewGraphCode();
					ofstream ofs;
					ofs.open(localFile_c,ofstream::app);
					ofs << "t # " << tempCount << endl;
					ofs.close();
					g->PrintFile(localFile_c);
					tempCount++;
				}
				it3++;
			}
			cout << "NO of local MFS: "<< tempCount << endl;
			exit(1);
		}
	}
	else 
	{
		// Reading file that has stored locally found MFS that are termed as candidate graphs 
		ifstream read_ifs1(argv[11]);
		if(!read_ifs1)
		{
			cout << "Unable to open " << argv[11] << endl;
			exit(0);
		}

		curGraph_i = 0;
		int num_of_candidate_graphs = local_graphs;
		string temp_read1;
		read_ifs1 >> temp_read1;
		cout << "Starting to read the file" << endl;
		while(curGraph_i < num_of_candidate_graphs)
		{
			vector<int> node_ids(0), node_labs(0), edge_n1(0), edge_n2(0), edge_labs(0);
			string read1;
			read_ifs1 >> read1;
			if(read1 == "#")
			{
				int read2;
				read_ifs1 >> read2;
				string nextRead;
				read_ifs1 >> nextRead;
				while(nextRead != "t")
				{
					if(nextRead == "v" || nextRead == ":v")
					{
						int read_nodeID, read_nodeLab;
						read_ifs1 >> read_nodeID >> read_nodeLab;
						node_ids.push_back(read_nodeID);
						node_labs.push_back(read_nodeLab);
					}
					else if(nextRead == "u")
					{
						int read_edgeN1, read_edgeN2, read_edgeLab;
						read_ifs1 >> read_edgeN1 >> read_edgeN2 >> read_edgeLab;
						edge_n1.push_back(read_edgeN1);
						edge_n2.push_back(read_edgeN2);
						edge_labs.push_back(read_edgeLab);
					}
					read_ifs1 >> nextRead;
					if(read_ifs1.eof())
						break;
				}
				boost::dynamic_bitset<> temp_bitset(total_graphs);

				cand_graph* tempG = new cand_graph();
				tempG->g = new Graph();
				if(!tempG->g->BLoadGraph(curGraph_i,node_ids, node_labs, edge_n1, edge_n2, edge_labs))
					ErrorExit("main()", "Error in load graph");
				int sizeOfTempGraph = tempG->g->IGetNoOfAllEdges();
				tempG->g->FindGraphCode();
				tempG->g->FindNewGraphCode();
				tempG->validity = Valid;
				tempG->graphIDsBitset = temp_bitset;
				tempG->localFreq = true;
				globalMaxFreq[sizeOfTempGraph].push_back(tempG);
				curGraph_i++;
			}
		}
		cout << "Done reading" << endl;
	}

	times(&tms4);

	// Global Computation
	Graph *dbAll_globalcomp;
	int noOfdbAll_globalcomp;
	minSupport_i = 2;

	map<int, vector<Graph*> >::reverse_iterator it, find_it;
	map<int, vector<cand_graph*> >::reverse_iterator it_cand, find_it_cand;
    
	// Module 2: Global computation

	// Computing Frequency of candidate graphs
	int num_of_InFreq_graph = 0;
	cout << "Computing Frequency for all the candidate graphs" << endl;
	curGraph_i = 0;

	ifstream read_ifs3;
	read_ifs3.rdbuf()->pubsetbuf(0, 0);
	read_ifs3.open(argv[2]);
	if(!read_ifs3)
	{
		cout << "Unable to open " << argv[2] << endl;
		exit(0);
	}

	count_per_partition = total_graphs/num_of_partition;
	for(int j=0;j<num_of_partition;j++)
	{
		noOfdbAll_i = 0;
		count_per_partition = (total_graphs - curGraph_i)/(num_of_partition-j);
		dbAll_g = new Graph[count_per_partition];

		while(noOfdbAll_i < count_per_partition)
		{
			vector<int> node_ids(0), node_labs(0), edge_n1(0), edge_n2(0), edge_labs(0);
			string read1;
			read_ifs3 >> read1;
			if(read1 == "#")
			{
				int read2;
				read_ifs3 >> read2;
				assert(read2 == curGraph_i);

				string nextRead;
				read_ifs3 >> nextRead;
				while(nextRead != "t")
				{
					if(nextRead == "v")
					{
						int read_nodeID, read_nodeLab;
						read_ifs3 >> read_nodeID >> read_nodeLab;
						node_ids.push_back(read_nodeID);
						node_labs.push_back(read_nodeLab);
					}
					else if(nextRead == "u")
					{
						int read_edgeN1, read_edgeN2, read_edgeLab;
						read_ifs3 >> read_edgeN1 >> read_edgeN2 >> read_edgeLab;
						edge_n1.push_back(read_edgeN1);
						edge_n2.push_back(read_edgeN2);
						edge_labs.push_back(read_edgeLab);
					}
					read_ifs3 >> nextRead;
					if(read_ifs3.eof())
						break;
				}
				if(!dbAll_g[noOfdbAll_i].BLoadGraph(curGraph_i,node_ids, node_labs, edge_n1, edge_n2, edge_labs))
					ErrorExit("main()", "Error in load graph");

				noOfdbAll_i++;
				curGraph_i++;
			}
		}
		for(int i=0; i < noOfdbAll_i; i++)
		{
			dbAll_g[i].FindGraphCode();
			dbAll_g[i].FindNewGraphCode();
		}
		num_of_InFreq_graph = 0;
		it_cand = globalMaxFreq.rbegin();
		while(it_cand!=globalMaxFreq.rend())
		{
			if(it_cand->second.empty())
			{
				it_cand++;
				continue;
			}

			for(int i=it_cand->second.size()-1;i>=0;i--)
			{
				int freq_i = 0;
				if(it_cand->second[i]->validity == Frequent)
					continue;

				num_of_InFreq_graph++;

				if(it_cand->second[i]->g->GetGraphType() == GRAPH_EMPTY)
					freq_i = global_min_support;
				else{
					for(int k=0; k<noOfdbAll_i; k++)
					{
						if(it_cand->second[i]->g->BIsSubGraphOf(&(dbAll_g[k])))
						{
							it_cand->second[i]->graphIDsBitset[dbAll_g[k].IGetGraphID()] = 1;
							if(it_cand->second[i]->graphIDsBitset.count() >= global_min_support)
							{
								it_cand->second[i]->validity = Frequent;
								num_of_InFreq_graph--;
								break;
							}
						}
					}
				}
			}
			it_cand++;
		}

		delete [] dbAll_g;

		if(num_of_InFreq_graph == 0)
			break;
	}
	times(&tms7);
	double time_freq_computation = ((double)(tms7.tms_utime-tms4.tms_utime)/(double)(double)sysconf(_SC_CLK_TCK))+((double)(tms7.tms_stime-tms4.tms_stime)/(double)(double)sysconf(_SC_CLK_TCK));

	it_cand = globalMaxFreq.rbegin();
	cout << globalMaxFreq.size() << endl;
	while(it_cand!=globalMaxFreq.rend()){
		cout << "Level: " << it_cand->first << endl;
		for(int i=0;i<it_cand->second.size();i++)
		{
			cout << "Candidate Index " << i << endl;
			cout << "Validity " << it_cand->second[i]->validity << endl;
			cout << "Freq " << it_cand->second[i]->graphIDsBitset.count() << endl;
			it_cand->second[i]->g->Print();
		}
		it_cand++;
	}

	cout << "Number of Infrequent graphs: " << num_of_InFreq_graph << endl;

	map<int, vector<cand_graph*> >::iterator it_cand_3, find_it_cand_3;

	times(&tms5);
	
	// Module 2.1 Computing MCS for each candidate graph from each partition.
	cout << "Start Computing MCS for all the candidate local maximal frequent graphs found" << endl;
	int num_of_MCS_operations = 0;
	int MCS_graphsadded = 0;
	curGraph_i = 0;
	ifstream read_ifs2;
	read_ifs2.rdbuf()->pubsetbuf(0, 0);
	read_ifs2.open(argv[2]);
	if(!read_ifs2)
	{
		cout << "Unable to open " << argv[2] << endl;
		exit(0);
	}

	count_per_partition = total_graphs/num_of_partition;
	double time_mcs = 0;
	for(int j=0;j<num_of_partition;j++)
	{
		if(num_of_InFreq_graph == 0)
		{
			cout << "No. of Infreq graphs = 0" << endl;	
			break;
		}
		noOfdbAll_globalcomp = 0;
		count_per_partition = (total_graphs - curGraph_i)/(num_of_partition-j);
		dbAll_globalcomp = new Graph[count_per_partition];

		cout << "start " <<  j << " partition with " << count_per_partition << " number of graphs" << endl;
		while(noOfdbAll_globalcomp < count_per_partition)
		{
			vector<int> node_ids(0), node_labs(0), edge_n1(0), edge_n2(0), edge_labs(0);
			string read1;
			read_ifs2 >> read1;
			if(read1 == "#")
			{
				int read2;
				read_ifs2 >> read2;
				assert(read2 == curGraph_i);

				string nextRead;
				read_ifs2 >> nextRead;
				while(nextRead != "t")
				{
					if(nextRead == "v")
					{
						int read_nodeID, read_nodeLab;
						read_ifs2 >> read_nodeID >> read_nodeLab;
						node_ids.push_back(read_nodeID);
						node_labs.push_back(read_nodeLab);
					}
					else if(nextRead == "u")
					{
						int read_edgeN1, read_edgeN2, read_edgeLab;
						read_ifs2 >> read_edgeN1 >> read_edgeN2 >> read_edgeLab;
						edge_n1.push_back(read_edgeN1);
						edge_n2.push_back(read_edgeN2);
						edge_labs.push_back(read_edgeLab);
					}
					read_ifs2 >> nextRead;
					if(read_ifs2.eof())
						break;
				}
				if(!dbAll_globalcomp[noOfdbAll_globalcomp].BLoadGraph(curGraph_i,node_ids, node_labs, edge_n1, edge_n2, edge_labs))
					ErrorExit("main()", "Error in load graph");

				noOfdbAll_globalcomp++;
				curGraph_i++;
			}
		}

		for(int i=0; i < noOfdbAll_globalcomp; i++)
		{
			dbAll_globalcomp[i].FindGraphCode();
			dbAll_globalcomp[i].FindNewGraphCode();
		}

		noOfdbAll_i = 2;
		dbAll_g = new Graph[noOfdbAll_i];

		// Compute MCS only if it is one of the candidate of the locally found maximal frequent subgraph and it is Infrequent 
		it_cand = globalMaxFreq.rbegin();
		while(it_cand!=globalMaxFreq.rend())
		{
			cout << "Level: " << it_cand->first << endl;
			for(int k=0;k<noOfdbAll_globalcomp;k++)
			{
				for(int i=0;i<it_cand->second.size();i++)
				{
					if(!it_cand->second[i]->localFreq)
						break;

					if(it_cand->second[i]->validity == Frequent)
						continue; // No need to do MCS

					// If already there in this DB then skip doing MCS.
					if(it_cand->second[i]->graphIDsBitset[dbAll_globalcomp[k].IGetGraphID()] == 1)
						continue;

					dbAll_g[1].ExactgraphCopy(&dbAll_globalcomp[k]);
					vector<Graph*> components =  it_cand->second[i]->g->GetComponents(&dbAll_g[1]);

					vector<Graph*>::iterator git;
					git = components.begin();
					while(git!=components.end())
					{

						if((*git)->GetGraphType() == GRAPH_EMPTY)
						{
							git++;
							continue;
						}

						if((*git)->GetGraphType() == GRAPH_SINGLE_NODE)
						{
							int node_label = (*git)->IGetNodeLabel((*git)->IGetTheSingleNode());
							for(int l=0; l < dbAll_g[1].IGetNoOfAllNodes(); l++)
							{
								if(dbAll_g[1].IGetNodeLabel(l) == node_label)
								{
									Graph* temp = new Graph();
									temp->Copy(*git);
									maxFreq.push_back(temp);
									validMaxFreq.push_back(true);
									break;
								}
							}
						}
						else{

							boost::dynamic_bitset<> tx(total_graphs);
							dbAll_g[0].ExactgraphCopy(*git); 
							dbAll_g[0].SetGraphID(-1);
							dbAll_g[0].FindGraphCode();
							dbAll_g[0].FindNewGraphCode();
							times(&tms2);
							cout << "doing MCS" << endl;
							BGetMCS(true, tx);
							cout << "Done MCS" << endl;
							num_of_MCS_operations++;
							times(&tms3);
							time_mcs += ((double)(tms3.tms_utime-tms2.tms_utime)/(double)(double)sysconf(_SC_CLK_TCK))+((double)(tms3.tms_stime-tms2.tms_stime)/(double)(double)sysconf(_SC_CLK_TCK));
							dbAll_g[0].GraphClear();
						}
						git++;

						// Merging the output
						for(int l=0;l<maxFreq.size();l++)
						{
							if(!validMaxFreq[l])
								continue;

							int sizeOfTempGraph = maxFreq[l]->IGetNoOfAllEdges();

							// Checking for same subgraph if present already
							bool flag_ifalreadypresent = false;

							// If the MCS obtained is a subgraph of any previously visited cand graph then either it would be produced through it
							// or being a subgraph of LMFS it shouldn't be produced
							find_it_cand = globalMaxFreq.rbegin();
							while(find_it_cand!=globalMaxFreq.rend())
							{
								if(find_it_cand->first <= sizeOfTempGraph)
									break;
 
								for(int m=0;m<find_it_cand->second.size();m++){
									if(!find_it_cand->second[m]->localFreq)
										break;

									if(find_it_cand->first > it_cand->first || (find_it_cand->first == it_cand->first && m<i))
									{
										// Subgraph only when maxFreq[l] graphIDs is a subset of find_it_cand->second[m]->g
										if(maxFreq[l]->BIsSubGraphOf(find_it_cand->second[m]->g))
										{
											// No need to add this, this would be just an extra.
											flag_ifalreadypresent = true;
											break;
										}
									}
								}
								if(flag_ifalreadypresent){
									break;
								}
								find_it_cand++;
							}
							
							if(flag_ifalreadypresent){
								continue;
							}

							for(int m=0;m<globalMaxFreq[sizeOfTempGraph].size();m++){
								if(maxFreq[l]->BIsIsomorphicTo(globalMaxFreq[sizeOfTempGraph][m]->g))
								{
									// Update the GraphIDs and add the found graph as a subgraph of the cand graph
									globalMaxFreq[sizeOfTempGraph][m]->graphIDsBitset[dbAll_g[1].IGetGraphID()] = 1;
									globalMaxFreq[sizeOfTempGraph][m]->graphIDsBitset |= it_cand->second[i]->graphIDsBitset;

									if(sizeOfTempGraph < it_cand->first && find(it_cand->second[i]->subgraphs[sizeOfTempGraph].begin(), it_cand->second[i]->subgraphs[sizeOfTempGraph].end(), globalMaxFreq[sizeOfTempGraph][m]) == it_cand->second[i]->subgraphs[sizeOfTempGraph].end())
										it_cand->second[i]->subgraphs[sizeOfTempGraph].push_back(globalMaxFreq[sizeOfTempGraph][m]);

									flag_ifalreadypresent = true;
									break;
								}
							}

							if(flag_ifalreadypresent){
								continue;
							}

							Graph *g = new Graph();
							g->Copy(maxFreq[l]);
							cand_graph* tempG = new cand_graph();
							tempG->g = g;
							tempG->validity = Valid;
							tempG->localFreq = false;
							tempG->parent = it_cand->second[i];
							tempG->graphIDsBitset = it_cand->second[i]->graphIDsBitset;
							tempG->graphIDsBitset[dbAll_g[1].IGetGraphID()] = 1;

							globalMaxFreq[sizeOfTempGraph].push_back(tempG);
							it_cand->second[i]->subgraphs[sizeOfTempGraph].push_back(tempG);
							MCS_graphsadded++;
						}

						// Clear the maxFreq struture which outputs the MCS of the two given graphs that are stored in dbAll_g
						for(int l=maxFreq.size()-1;l>=0;l--){
							if(!validMaxFreq[l])
								continue;
							delete maxFreq[l];
						}
						maxFreq.erase(maxFreq.begin(), maxFreq.end());
						vector<Graph*>().swap(maxFreq);
						maxFreq_FreqVal.erase(maxFreq_FreqVal.begin(), maxFreq_FreqVal.end());
						vector<int>().swap(maxFreq_FreqVal);
						validMaxFreq.erase(validMaxFreq.begin(), validMaxFreq.end());
						vector<bool>().swap(validMaxFreq);
					}
					for(int l=components.size()-1;l>=0;l--)
					{
						if(components[l]->GetGraphType() != GRAPH_EMPTY)
							delete components[l]; 
					}
					components.erase(components.begin(), components.end());
					vector<Graph*>().swap(components);
					dbAll_g[1].GraphClear();
				}

				// free memory

				map<int, map<int, knownGraphNode*> >:: reverse_iterator it1;
				it1 = knownGraphsMap.rbegin();
				while(it1!=knownGraphsMap.rend())
				{
					map<int, knownGraphNode*>:: reverse_iterator it2;
					it2=it1->second.rbegin();
					while(it2!=it1->second.rend())
					{
						int val =  it2->second->knownGraphs.size();
						for(int k=0; k < val; k++)
						{
							delete it2->second->knownGraphs[k];
						}
						it2->second->knownGraphs.erase(it2->second->knownGraphs.begin(),it2->second->knownGraphs.end());
						vector<Graph*>().swap(it2->second->knownGraphs);

						delete it2->second;
						it2++;
					}
					it1->second.clear();
					it1++;
				}
				knownGraphsMap.clear();
			}
			cout << "Done Level: " << it_cand->first << endl;
			it_cand++;
		}

		delete [] dbAll_g;
		delete [] dbAll_globalcomp;
		cout << "Done Levels" << endl;
	}
	times(&tms7);
	double time_mcs_computation_database = ((double)(tms7.tms_utime-tms5.tms_utime)/(double)(double)sysconf(_SC_CLK_TCK))+((double)(tms7.tms_stime-tms5.tms_stime)/(double)(double)sysconf(_SC_CLK_TCK));

	int tempCount1=0;
	
	cout << "After computing MCS wrt db" << endl;
	it_cand = globalMaxFreq.rbegin();
	while(it_cand!=globalMaxFreq.rend())
	{
		for(int i=0;i<it_cand->second.size();i++)
		{
			cout << "t # " << tempCount1 << endl;
			cout << "Freq " << it_cand->second[i]->graphIDsBitset.count() << endl;
			cout << "GraphID size " << it_cand->second[i]->graphIDsBitset.size() << endl;
			cout << "Validity: " << it_cand->second[i]->validity << endl;
			it_cand->second[i]->g->Print();
			cout << "Printing Subgraphs" << endl;
			map<int, vector<cand_graph*> >::iterator subit = it_cand->second[i]->subgraphs.begin();
			while(subit!=it_cand->second[i]->subgraphs.end()){
				cout << "	At level: " << subit->first << endl;
				for(int j=0;j<subit->second.size();j++){
					subit->second[j]->g->Print();
				}	
				subit++;
			}
			cout << " " << endl;
			tempCount1++;
		}
		it_cand++;
	}

	
	cout << "Time taken for freq Computation: " << time_freq_computation << endl;
	cout << "Time taken for BGetMCS: " << time_mcs << endl;
	cout << "Time taken for MCS computation wrt the database: " << time_mcs_computation_database << endl;
	cout << "Number of MCS Operations: " << num_of_MCS_operations << endl;
	cout << "No. of graphs added in GlobalMaxFreq after MCS Computation: " << MCS_graphsadded << endl;
	cout << "----------------------------------------------------------------" << endl;
	
	times(&tms8);

	// Module 2.2 Filter out the global max frequent subgraphs
	cout << "Checking for Maximal Frequent Subgraphs" << endl;
	noOfdbAll_i = 2;
	int MFS_graphsadded = 0;
	dbAll_g = new Graph[noOfdbAll_i];

	it_cand = globalMaxFreq.rbegin();
	while(it_cand!=globalMaxFreq.rend())
	{
		for(int i=it_cand->second.size()-1;i>=0;i--)
		{

			cout << "Level: " << it_cand->first << " " << "Indexes: " << i  << " Validity: " << it_cand->second[i]->validity << endl;
			if(it_cand->second[i]->validity == Valid || it_cand->second[i]->validity == Frequent)
			{
				if(it_cand->second[i]->graphIDsBitset.count() >= global_min_support)
				{
					//Graph is Frequent
					//Mark all its subgraphs as  invalid

					cout << "Graph is frequent with frequency: " << it_cand->second[i]->graphIDsBitset.count() << endl;

					find_it_cand = globalMaxFreq.rbegin();
					bool status = true;
					while(find_it_cand != globalMaxFreq.rend())
					{
						// Have to do subgraph for only supergraphs
						if(find_it_cand->first <= it_cand->first)
							break;
						for(int j=0;j<find_it_cand->second.size();j++)
						{
							if(find_it_cand->second[j]->validity == Frequent)
							{
								if(it_cand->second[i]->g->BIsSubGraphOf(find_it_cand->second[j]->g))
								{
									status = false;
									break;
								}
							}
						}
						if(!status)
							break;
						find_it_cand++;
					}

					if(status)
						it_cand->second[i]->validity = Frequent;
					else
						it_cand->second[i]->validity = InValid;

					map<int, vector<cand_graph*> >::iterator index_it;
					index_it = it_cand->second[i]->subgraphs.begin();
					while(index_it!=it_cand->second[i]->subgraphs.end()){
						for(int j=index_it->second.size()-1;j>=0;j--)
						{
							if(index_it->second[j]->validity!=InValid)
							{
								index_it->second[j]->validity = InValid;
								// index_it->second.erase(index_it->second.begin()+j);
							}
						}
						index_it++;
					}
				}
				else
				{
					// Graph is infrequent
					// Mark it as Infrequent

					cout << "Graph is Infrequent with frequency: " << it_cand->second[i]->graphIDsBitset.count() << endl;
					it_cand->second[i]->validity = InFrequent;
				}
			}
		}

		// Do MCS for the pair of infrequent graphs
		// Add the MCS in the global structure
		for(int i=0;i<it_cand->second.size();i++)
		{
			if(it_cand->second[i]->validity == InFrequent)
			{
				if(it_cand->second[i]->parent == NULL)
					continue;

				cout << "Starting with graph at Level: " << it_cand->first << " index: " << i << endl;
				dbAll_g[0].ExactgraphCopy(it_cand->second[i]->g); 
				dbAll_g[0].SetGraphID(-1);

				find_it_cand = it_cand->second[i]->parent->subgraphs.rbegin();
				bool gotfreq = false;
				while(find_it_cand != it_cand->second[i]->parent->subgraphs.rend())
				{
					// Have to do MCS for only supergraphs
					if(find_it_cand->first < it_cand->first)
						break;
					for(int j=0;j<find_it_cand->second.size();j++)
					{
						if(gotfreq)
							break;

						if(find_it_cand->first == it_cand->first && j==i)
							break;

						if(find_it_cand->second[j]->validity != InFrequent)
							continue;

						boost::dynamic_bitset<> temp_bitset(total_graphs);
						temp_bitset  = it_cand->second[i]->graphIDsBitset;

						temp_bitset  |= find_it_cand->second[j]->graphIDsBitset;
						if(temp_bitset.count() == it_cand->second[i]->graphIDsBitset.count() || temp_bitset.count() == find_it_cand->second[j]->graphIDsBitset.count())
							continue;
						// Compute MCS only when the graph is not a super graph of the current graph irrespective of what validity the graph has.
						// Because if it is frequent then this current graph would also be marked as frequent.
						// If infrequent then we definetely need this
						// If invalid (because its supergraph is frequent) then this graph would also have been marked as invalid.

						dbAll_g[1].ExactgraphCopy(find_it_cand->second[j]->g);
						dbAll_g[1].SetGraphID(0);

						cout << "BGetMCS() function" << endl;
						boost::dynamic_bitset<> x_temp1(total_graphs);
						x_temp1 |= it_cand->second[i]->graphIDsBitset;
						x_temp1 |= find_it_cand->second[j]->graphIDsBitset;

						// All the found graphs should have their frequency updated in the BGetMCS function itself where it is finding all the supergraphs
						BGetMCS(false, x_temp1);
						cout << "Start merging" << endl; 
						for(int l=0;l<maxFreq.size();l++)
						{
							if(!validMaxFreq[l])
								continue;

							int sizeOfTempGraph = maxFreq[l]->IGetNoOfAllEdges();
							// Checking for same subgraph if present already
							bool flag_ifalreadypresent = false;
						
							for(int m=0;m<globalMaxFreq[sizeOfTempGraph].size();m++)
							{
								if(maxFreq[l]->BIsIsomorphicTo(globalMaxFreq[sizeOfTempGraph][m]->g))
								{
									// global_maxfreq_GraphIDs[sizeOfTempGraph][m].insert(GraphIDs_parent.begin(), GraphIDs_parent.end());
									//set bitset to be equal to that of the parent
									globalMaxFreq[sizeOfTempGraph][m]->graphIDsBitset |= x_temp1;
									if(globalMaxFreq[sizeOfTempGraph][m]->validity!= InValid && globalMaxFreq[sizeOfTempGraph][m]->graphIDsBitset.count() >= global_min_support)
									{
										globalMaxFreq[sizeOfTempGraph][m]->validity = Frequent;
										if(sizeOfTempGraph == it_cand->first && m==i){
											gotfreq = true;
										}
									}

									if(sizeOfTempGraph < it_cand->first && find(it_cand->second[i]->subgraphs[sizeOfTempGraph].begin(), it_cand->second[i]->subgraphs[sizeOfTempGraph].end(), globalMaxFreq[sizeOfTempGraph][m]) == it_cand->second[i]->subgraphs[sizeOfTempGraph].end())
										it_cand->second[i]->subgraphs[sizeOfTempGraph].push_back(globalMaxFreq[sizeOfTempGraph][m]);

									if(sizeOfTempGraph < find_it_cand->first && find(find_it_cand->second[j]->subgraphs[sizeOfTempGraph].begin(), find_it_cand->second[j]->subgraphs[sizeOfTempGraph].end(), globalMaxFreq[sizeOfTempGraph][m]) == find_it_cand->second[j]->subgraphs[sizeOfTempGraph].end())
										find_it_cand->second[j]->subgraphs[sizeOfTempGraph].push_back(globalMaxFreq[sizeOfTempGraph][m]);

									flag_ifalreadypresent = true;
									break;
								}
							}

							if(flag_ifalreadypresent){
								continue;
							}

							Graph *g = new Graph();
							g->Copy(maxFreq[l]);
							boost::dynamic_bitset<> temp_bitset(total_graphs);
							cand_graph* tempG = new cand_graph();
							tempG->g = g;
							tempG->validity = Valid;
							tempG->graphIDsBitset = temp_bitset;
							globalMaxFreq[sizeOfTempGraph].push_back(tempG);
							MFS_graphsadded++;
							map<int, vector<cand_graph*> >::reverse_iterator it_cand_1 = it_cand->second[i]->parent->subgraphs.rbegin();
							while(it_cand_1!=it_cand->second[i]->parent->subgraphs.rend())
							{
								if(it_cand_1->first == sizeOfTempGraph)
								{
									it_cand_1++;
									continue;
								}

								if(it_cand_1->first > sizeOfTempGraph)
								{
									for(int m=0;m<it_cand_1->second.size();m++)
									{
										if(maxFreq[l]->BIsSubGraphOf(it_cand_1->second[m]->g))
										{
											// it_cand_1->second[m]->g is the supergraph of maxFreq[i]
											// Update the Subgraphs of it_cand_1->second[m]->g by adding the graph pointer of maxFreq[i] to it.
											tempG->graphIDsBitset |= it_cand_1->second[m]->graphIDsBitset;
											if(tempG->graphIDsBitset.count() >= global_min_support)
												tempG->validity = Frequent;
											it_cand_1->second[m]->subgraphs[sizeOfTempGraph].push_back(tempG);
											if(it_cand_1->second[m]->validity == Frequent)
												tempG->validity = InValid;
										}				
									}
								}
								else
								{
									for(int m=0;m<it_cand_1->second.size();m++)
									{
										if(it_cand_1->second[m]->g->BIsSubGraphOf(maxFreq[l]))
										{
											// Update the Subgraphs of maxFreq[m] by adding the graph pointer of it_cand_1->second[m]->g to it.
											it_cand_1->second[m]->graphIDsBitset |= tempG->graphIDsBitset;
											tempG->subgraphs[it_cand_1->first].push_back(it_cand_1->second[m]);
										}				
									}
								}
								it_cand_1++;
							}
							if(tempG->validity==Frequent){
								it_cand_1 = tempG->subgraphs.rbegin();
								while(it_cand_1!=tempG->subgraphs.rend()){
									for(int m=it_cand_1->second.size()-1;m>=0;m--){
										if(it_cand_1->second[m]->validity != InValid)
										{
											it_cand_1->second[m]->validity = InValid;
										}
									}
									it_cand_1++;
								}
							}

						}
						cout << "Done merging" << endl; 
						cout << find_it_cand->second[0]->graphIDsBitset.size() << endl;
						for(int k=maxFreq.size()-1;k>=0;k--){
							if(!validMaxFreq[k])
								continue;
							delete maxFreq[k];
						}
						maxFreq.erase(maxFreq.begin(), maxFreq.end());
						vector<Graph*>().swap(maxFreq);
						maxFreq_FreqVal.erase(maxFreq_FreqVal.begin(), maxFreq_FreqVal.end());
						vector<int>().swap(maxFreq_FreqVal);
						validMaxFreq.erase(validMaxFreq.begin(), validMaxFreq.end());
						vector<bool>().swap(validMaxFreq);
						
						dbAll_g[1].GraphClear();
					}
					if(gotfreq)
						break;
					find_it_cand++;
				}
				// Found out to be freq
				if(gotfreq)
				{
					find_it_cand = globalMaxFreq.rbegin();
					bool status = true;
					while(find_it_cand != globalMaxFreq.rend())
					{
						// Have to do subgraph for only supergraphs
						if(find_it_cand->first <= it_cand->first)
							break;
						for(int j=0;j<find_it_cand->second.size();j++)
						{
							if(find_it_cand->second[j]->validity == Frequent)
							{
								if(it_cand->second[i]->g->BIsSubGraphOf(find_it_cand->second[j]->g))
								{
									status = false;
									break;
								}
							}
						}
						if(!status)
							break;
						find_it_cand++;
					}

					if(status)
						it_cand->second[i]->validity = Frequent;
					else
						it_cand->second[i]->validity = InValid;

					map<int, vector<cand_graph*> >::iterator index_it;
					index_it = it_cand->second[i]->subgraphs.begin();
					while(index_it!=it_cand->second[i]->subgraphs.end()){
						for(int j=index_it->second.size()-1;j>=0;j--)
						{
							if(index_it->second[j]->validity!=InValid)
							{
								index_it->second[j]->validity = InValid;
							}
						}
						index_it++;
					}
				}
				dbAll_g[0].GraphClear();
			}
		}
		it_cand++;
	}

	delete [] dbAll_g;

	times(&tms9);	
	double time_global_iteration =  ((double)(tms9.tms_utime-tms8.tms_utime)/(double)(double)sysconf(_SC_CLK_TCK))+((double)(tms9.tms_stime-tms8.tms_stime)/(double)(double)sysconf(_SC_CLK_TCK));
	cout << "Time taken by the iteration for filtering out the global max freq subgraphs: " << time_global_iteration << endl;
	cout << "No. of graphs added in GlobalMaxFreq after MFS Computation: " << MFS_graphsadded << endl;
	cout << "Done Global computing" << endl;
	cout << "----------------------------------------------------------------" << endl;
	

	int graphgenerated = 0;
	int FreqGraph = 0;
	if(isOutputFile){
		map<int,vector<cand_graph*> >::iterator it_cand_2;
		it_cand_2 = globalMaxFreq.begin();
		int tempcount = 0;
		while(it_cand_2!=globalMaxFreq.end()){
			graphgenerated += it_cand_2->second.size(); 
			cout << "OutputLevel " << it_cand_2->first << " Size: " << it_cand_2->second.size() << endl;
			for(int i=0;i<it_cand_2->second.size();i++)
			{
				if(it_cand_2->second[i]->validity == Frequent){
					ofstream ofs;
					ofs.open(outputFile_c,ofstream::app);
					ofs << "t # " << tempcount << endl;
					ofs << "Freq : " << it_cand_2->second[i]->graphIDsBitset.count() << endl;
					ofs << "Freq : " << it_cand_2->second[i]->graphIDsBitset << endl;
					it_cand_2->second[i]->g->FindNewGraphCode();
					ofs.close();
					it_cand_2->second[i]->g->PrintFile(outputFile_c);
					tempcount++;
					FreqGraph++;
				}
			}
			it_cand_2++;
		}

	}

	it_cand = globalMaxFreq.rbegin();
	while(it_cand!=globalMaxFreq.rend())
	{
		for(int i=it_cand->second.size()-1;i>=0;i--)
		{
			delete it_cand->second[i];
			it_cand->second.erase(it_cand->second.begin()+i);
		}

		it_cand++;
	}	

	times(&tms6);	
	int num_subgraphiso=return_num_subgraphiso();
	int num_iso=return_num_iso();
	double time_total_global =  ((double)(tms6.tms_utime-tms4.tms_utime)/(double)(double)sysconf(_SC_CLK_TCK))+((double)(tms6.tms_stime-tms4.tms_stime)/(double)(double)sysconf(_SC_CLK_TCK));
	cout << "Global Computation done" << endl;
	cout << "Graph Freq: " << FreqGraph << endl;
	cout << "Total subgraph isomorphism checks                          : " << num_subgraphiso  << endl;
	cout << "Total isomorphism checks                                   : " << num_iso  << endl;
	cout << "Total graphs generated in global computation        		: " << graphgenerated  << endl;
	cout << "Total Time Taken for global computation        			: " << time_total_global << endl;
	cout << "----------------------------------------------------------------" << endl;	
}
