/*
 * File Info: main.cpp
 * Description: This file contains the 'main()' function. The starting point of the code.
 * */
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <cassert>
#include <string.h>
#include <algorithm>
#include <time.h>
#include <sys/times.h>
#include "Graph.h"


int noOfdbAll_i;
int num_of_partition;
Graph *dbAll_g;
int minSupport_i;
int total_graphs;
int local_graphs;
bool isOutputFile;
char *outputFile_c;
bool isLocalFile;
char *localFile_c;
bool isLocalComputation;
bool shouldCount;

map<int, vector<Graph*> > global_local_maxfreq; 
map<int, vector<vector<int> > > global_local_maxFreq_partition_wise_count;
map<int, vector<int> > global_local_maxfreq_count;
map<int, vector<bool> > global_local_maxfreq_count_done;


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
	cout << argc << endl;
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
	// tms5: time at the end of local commputation run (i.e at the end of local computation of maximal frequent graphs)
		// tms5 - tms1: total time take for loading each partition and running local MFS on it
	// tms6: time at the end of the global computation.
		// tms6-tms5: total time taken for global computation
		// tms6-tms1: total time taken

	intialize_num();

	map<int, map<int, knownGraphNode*> >:: iterator knownGraph_it;
	knownGraph_it = knownGraphsMap.begin();
	while(knownGraph_it != knownGraphsMap.end())
	{
		map<int, knownGraphNode*>:: iterator it2;
		it2 = knownGraph_it->second.begin();
		while(it2 != knownGraph_it->second.end())
		{
			it2->second->knownFreqGraphs_startindex = 0;
			it2->second->knownFreqGraphs_FreqVal_startindex = 0;
			it2->second->knownInfreqGraphs_startindex = 0;
			it2->second->knownFreqGraphEmbeddings_startindex = 0;
			it2->second->knownInFreqGraphEmbeddings_startindex = 0;
			it2++;
		}
		knownGraph_it++;
	}


	map<pair<pair<int, int>, int>, int> edge_embeddings;
    times(&tms1);
    int curGraph_i = 0;
    string temp_read;
    read_ifs >> temp_read;
	int edge_cnt = 0;
	int global_min_support = minSupport_i;
	
	int total_graphs = tempcurGraph_i;
	int count_per_partition = total_graphs/num_of_partition;
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
			for(int i=0; i < noOfdbAll_i; i++)
			{
				dbAll_g[i].FindGraphCode();
				dbAll_g[i].FindNewGraphCode();
			}

			minSupport_i = floor(global_min_support*noOfdbAll_i/tempcurGraph_i);

			int i=0;
			edge_iterator ei, ei_end, next_e1,next_e;
			times(&tms3);
			StartExpandCut(j);
			times(&tms4);

			double time_load = ((double)(tms3.tms_utime-tms2.tms_utime)/(double)(double)sysconf(_SC_CLK_TCK))+((double)(tms3.tms_stime-tms2.tms_stime)/(double)(double)sysconf(_SC_CLK_TCK));
			double time_expcut =  ((double)(tms4.tms_utime-tms3.tms_utime)/(double)(double)sysconf(_SC_CLK_TCK))+((double)(tms3.tms_stime-tms3.tms_stime)/(double)(double)sysconf(_SC_CLK_TCK));
			double time_total_partition =  ((double)(tms4.tms_utime-tms2.tms_utime)/(double)(double)sysconf(_SC_CLK_TCK))+((double)(tms4.tms_stime-tms2.tms_stime)/(double)(double)sysconf(_SC_CLK_TCK));

			cout << "ExpCut Time Taken for " << j << "th partition        : " << time_expcut << " Sec" << endl;
			cout << "No Of Expand Cuts for " << j << "th partition        : " << noOfExpandCuts << endl;
			cout << "Total Time Taken  for " << j << "th partition        : " << time_total_partition << " Sec" << endl;
			cout << "----------------------------------------------------------------" << endl;
		

			for(int i=0;i<maxFreq.size();i++){
				if(!validMaxFreq[i])
					continue;
				maxFreq[i]->~Graph();
			}
			maxFreq.erase(maxFreq.begin(), maxFreq.end());
			vector<Graph*>().swap(maxFreq);
			maxFreq_FreqVal.erase(maxFreq_FreqVal.begin(), maxFreq_FreqVal.end());
			vector<int>().swap(maxFreq_FreqVal);
			validMaxFreq.erase(validMaxFreq.begin(), validMaxFreq.end());
			vector<bool>().swap(validMaxFreq);

			for(int i=0;i<noOfdbAll_i;i++)
				dbAll_g[i].~Graph();

			// delete [] dbAll_g;
		}

		times(&tms5);	
		double time_total_local =  ((double)(tms5.tms_utime-tms1.tms_utime)/(double)(double)sysconf(_SC_CLK_TCK))+((double)(tms5.tms_stime-tms1.tms_stime)/(double)(double)sysconf(_SC_CLK_TCK));
		cout << "Local Computation done" << endl;
		cout << "Total Time Taken for local computation for each partition        : " << time_total_local << " Sec" << endl;
		cout << "----------------------------------------------------------------" << endl;
	
		if(isLocalFile)
		{
			cout << "HERE" << endl;
			map<int, vector<Graph*> >::iterator it3;
			it3 = global_local_maxfreq.begin();

			int tempCount=0;
			while(it3!=global_local_maxfreq.end()){
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
		// // For debugging 
		// Reading file that has stored locally found maximum frequent graphs that are termed as candidate graphs 
		// For checking purpose Loading the maxFreq candidate graphs
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
		// cout << temp_read1 << endl;
		dbAll_g = new Graph[num_of_candidate_graphs];
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
					if(nextRead == "v"  || nextRead == ":v")
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
				cout << "Loading graph: " << curGraph_i << " Graph" << endl;
				if(!dbAll_g[curGraph_i].BLoadGraph(curGraph_i,node_ids, node_labs, edge_n1, edge_n2, edge_labs))
					ErrorExit("main()", "Error in load graph");

				Graph* g = new Graph();
				g->Copy(&dbAll_g[curGraph_i]);
				g->FindGraphCode();
				g->FindNewGraphCode();
				int sizeOfTempGraph = dbAll_g[curGraph_i].IGetNoOfAllEdges();
				global_local_maxfreq[sizeOfTempGraph].push_back(g);
				vector<int> temppartitioncount(num_of_partition, -1);
				global_local_maxFreq_partition_wise_count[sizeOfTempGraph].push_back(temppartitioncount);
				numOfValidCandidateGraphs++;
				curGraph_i++;
				cout << "done reading " << curGraph_i << " Graph" << endl; 
			}

		}
		for(int i=0;i<noOfdbAll_i;i++)
				dbAll_g[i].~Graph();

		cout << "Done storing the Global structure" << endl;
	}

	times(&tms5);
	// Global Computation
	map<int, vector<Graph*> >::reverse_iterator it;
	map<int, vector<int> >::reverse_iterator it1;
	
	int graphgenerated = 0;
	it = global_local_maxfreq.rbegin();
	while(it!=global_local_maxfreq.rend()){
		graphgenerated += it->second.size(); 
		for(int i=0;i<it->second.size();i++){
			global_local_maxfreq_count[it->first].push_back(0);
			global_local_maxfreq_count_done[it->first].push_back(false);
		}
		it++;
	}

	int round = 0;
	double time_freq_computation=0;
	while (round < global_local_maxfreq.size())
	{
		cout << "Round number " << round << endl;

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
		times(&tms4);
		for(int j=0;j<num_of_partition;j++)
		{
			noOfdbAll_i = 0;
			count_per_partition = (total_graphs - curGraph_i)/(num_of_partition-j);
			dbAll_g = new Graph[count_per_partition];

			cout << "start new partition with " << count_per_partition << " number of graphs" << endl;
			while(noOfdbAll_i < count_per_partition)
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

			cout << "Computed Frequency for all the candidate graphs" << endl;
			it = global_local_maxfreq.rbegin();
			advance(it, round);
			while(it!=global_local_maxfreq.rend())
			{
				cout << "Level: " << it->first << " Size: " << it->second.size() << endl;
				if(it->second.empty())
				{
					it++;
					continue;
				}

				for(int i=it->second.size()-1;i>=0;i--)
				{
					int freq_i = 0;
					if(global_local_maxfreq_count_done[it->first][i])
						break;

					if(global_local_maxFreq_partition_wise_count[it->first][i][j] == -1)
					{
						if(it->second[i]->GetGraphType() == GRAPH_EMPTY)
							freq_i = minSupport_i;
						else{
							for(int k=0; k<noOfdbAll_i; k++)
							{
								if(it->second[i]->BIsSubGraphOf(&(dbAll_g[k]))){
									freq_i++;
									if(global_local_maxfreq_count[it->first][i] + freq_i >= global_min_support)
										break;
								}
							}
						}
						global_local_maxFreq_partition_wise_count[it->first][i][j] = freq_i;
					}
					cout << "Freq found in " << j << "th partition: " << global_local_maxFreq_partition_wise_count[it->first][i][j] << endl; 
					global_local_maxfreq_count[it->first][i] += global_local_maxFreq_partition_wise_count[it->first][i][j];
				}

				it++;
			}

			for(int i=0;i<noOfdbAll_i;i++)
				dbAll_g[i].~Graph();
		}
		times(&tms7);
		double time_freq_computation1 = ((double)(tms7.tms_utime-tms4.tms_utime)/(double)(double)sysconf(_SC_CLK_TCK))+((double)(tms7.tms_stime-tms4.tms_stime)/(double)(double)sysconf(_SC_CLK_TCK));
		time_freq_computation+=time_freq_computation1;
		cout << "Time taken for freq Computation in this partition: " << time_freq_computation1 << endl;
		

		it = global_local_maxfreq.rbegin();
		advance(it, round);

		int completed_level = 0;
		bool childGraphAdded = false;

		cout << "Checking for globally frequent graphs and globally infrequent graphs Size: " << global_local_maxfreq.size() << endl;
		while(it!=global_local_maxfreq.rend())
		{
			cout << "Level: " << it->first << " Size: " << it->second.size() << endl;
			for(int i=it->second.size()-1;i>=0;i--)
			{
				// Checking whether it has been visited earlier or not 
				if(global_local_maxfreq_count_done[it->first][i])
					break;

				// If a graph has been found that has been visited earlier then all the graphs after it in the iteration has already been visited.
				// As the new graphs are added at the end and iteration is done in reverse order.
				// Not checking for newly added graphs as there frequency haven't been computed yet
				if(global_local_maxfreq_count[it->first][i] == 0)
					continue;

				global_local_maxfreq_count_done[it->first][i] = true;
				if(global_local_maxfreq_count[it->first][i] < global_min_support)
				{
					vector<Graph*> TempChildren;
					int noOfChild;
					TempChildren = global_local_maxfreq[it->first][i]->GetChildren(&noOfChild);

					//Remove the current graph from the candidate set of Global Frequent Graph
					global_local_maxfreq[it->first].erase(global_local_maxfreq[it->first].begin()+i);
					global_local_maxfreq_count[it->first].erase(global_local_maxfreq_count[it->first].begin()+i);
					global_local_maxFreq_partition_wise_count[it->first].erase(global_local_maxFreq_partition_wise_count[it->first].begin()+i);
					global_local_maxfreq_count_done[it->first].erase(global_local_maxfreq_count_done[it->first].begin()+i);

					//Add immediate children to the global_local_max_Freq;
					for(int j=0;j<noOfChild;j++)
					{
						bool flag = true;
						int tempSize = TempChildren[j]->IGetNoOfAllEdges();
						map<int, vector<Graph*> >::reverse_iterator it2 = global_local_maxfreq.rbegin();

						//Checking whether the children graphs are subgraph of already found graphs or not.
						//Checking this only for graphs that have size greater than the child graph size.
						while(it2!=global_local_maxfreq.rend())
						{
							if(it2->first >= tempSize)
							{
								for(int k=0;k<it2->second.size();k++)
								{
									TempChildren[j]->SetGraphID(it2->second[k]->IGetGraphID());
									if(TempChildren[j]->BIsLabelSubGraph(it2->second[k]))
									{
										if(TempChildren[j]->BIsSubGraphOf(it2->second[k]))
										{
											flag = false;
											break;
										}
									}			
								}
							}
							
							if(!flag)
								break;
							it2++;
						}

						if(flag)
						{
							childGraphAdded = true;
							graphgenerated+=1;
							global_local_maxfreq[tempSize].push_back(TempChildren[j]);
							vector<int> temppartitioncount(num_of_partition, -1);
							global_local_maxFreq_partition_wise_count[tempSize].push_back(temppartitioncount);
							global_local_maxfreq_count[tempSize].push_back(0);
							global_local_maxfreq_count_done[tempSize].push_back(false);
						}
					}
				}
			}
			if(!childGraphAdded)
				round++;

			it++;
		}
		round++;
	}

	
	if(isOutputFile){
		map<int, vector<Graph*> >::iterator it3;
		map<int, vector<vector<int> > >::iterator it4;

		it3 = global_local_maxfreq.begin();
		it4 = global_local_maxFreq_partition_wise_count.begin();

		int tempCount=0;
		while(it3!=global_local_maxfreq.end()){
			cout << "OutputLevel " << it3->first << " Size: " << it3->second.size() << endl;
			for(int i=0;i<it3->second.size();i++)
			{
				ofstream ofs;
				ofs.open(outputFile_c,ofstream::app);
				ofs << "t # " << tempCount << endl;
				ofs << "Freq " << global_local_maxfreq_count[it3->first][i] << endl;
				it3->second[i]->FindNewGraphCode();
				ofs.close();
				it3->second[i]->PrintFile(outputFile_c);
				tempCount++;
			}
			it3++;
			it4++;
		}
	}

	times(&tms6);	

	int num_subgraphiso=return_num_subgraphiso();
	int num_iso=return_num_iso();

	double time_total_global =  ((double)(tms6.tms_utime-tms5.tms_utime)/(double)(double)sysconf(_SC_CLK_TCK))+((double)(tms6.tms_stime-tms5.tms_stime)/(double)(double)sysconf(_SC_CLK_TCK));
	double time_total =  ((double)(tms6.tms_utime-tms1.tms_utime)/(double)(double)sysconf(_SC_CLK_TCK))+((double)(tms6.tms_stime-tms1.tms_stime)/(double)(double)sysconf(_SC_CLK_TCK));
	cout << "Global Computation done" << endl;
	cout << "Total number of loadings									: " << round+1 << endl;
	cout << "Total graphs generated in global computation        		: " << graphgenerated  << endl;
	cout << "Total subgraph isomorphism checks                          : " << num_subgraphiso  << endl;
	cout << "Total isomorphism checks                                   : " << num_iso  << endl;
	cout << "Total graphs generated in global computation        		: " << graphgenerated  << endl;
	cout << "Total Time Taken for global computation        			: " << time_total_global << " Sec" << endl;
	cout << "Total time taken for freq Computation: " << time_freq_computation << endl;
	cout << "Total Time Taken for the overall naive algorithm        	: " <<  time_total << " Sec" << endl;
	cout << "----------------------------------------------------------------" << endl;
}
