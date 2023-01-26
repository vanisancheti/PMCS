/*
 * File Info: Graph.cpp
 * Description: This file contains the methods of Graph class
 *
 * */

#include <sstream>
#include "Graph.h" // declarations
#include <algorithm>
#include "global_functions.h" // declarations

using namespace std;
int num_subgraphiso;
int num_iso;

// Constructors
void intialize_num()
{
	num_subgraphiso =0 ;
    num_iso = 0;
	return;
}

int return_num_subgraphiso()
{
	return num_subgraphiso;
}

int return_num_iso()
{
	return num_iso;
}

bool findIfExist(map<int, map<int, knownGraphNode* > > &knownGraphsMap, int r, int c){
	if(knownGraphsMap.find(r)!=knownGraphsMap.end() && knownGraphsMap[r].find(c)!=knownGraphsMap[r].end()){
		return true;
	}
	return false;
}

void InitializeknownGraphNode(knownGraphNode* knownGraphNode, int r, int c){
	knownGraphNode->noOfEdges = r;
	knownGraphNode->noOfNodes = c;

	// tempknownGraphNode->childleft = NULL;
	// tempknownGraphNode->childright = NULL;

	// tempknownGraphNode->parentleft = NULL;
	// tempknownGraphNode->parentright = NULL;
}

Graph::Graph()
{
	graphID_i = -1;
	// noOfValidNodes_i = 0;
	// nodeIDs_nMnid.init(*this);
	// edgeLabel_eM.init(*this);
	edgeAddedOrRemovedsource = edgeAddedOrRemovedtarget = nodeAddedOrRemoved = -1;
	// make_undirected();
}

Graph::~Graph()
{
	// 2
	nodeLabels_Vi.erase(nodeLabels_Vi.begin(), nodeLabels_Vi.end());
	vector<int>().swap(nodeLabels_Vi);
	//1
	allNodes_Vn.erase(allNodes_Vn.begin(), allNodes_Vn.end());
	vector<Node>().swap(allNodes_Vn);
	// 4
	labelFreqs_Vi.erase(labelFreqs_Vi.begin(), labelFreqs_Vi.end());
	vector<int>().swap(labelFreqs_Vi);
	// 5
	freqEdgeLabels_Vi.erase(freqEdgeLabels_Vi.begin(), freqEdgeLabels_Vi.end());
	vector<int>().swap(freqEdgeLabels_Vi);
	// 6
	isValidNode_Vb.erase(isValidNode_Vb.begin(), isValidNode_Vb.end());
	vector<bool>().swap(isValidNode_Vb);
	// 7
	hiddenEdges_Ve.erase(hiddenEdges_Ve.begin(), hiddenEdges_Ve.end());
	vector<HiddenEdge>().swap(hiddenEdges_Ve);
	// 8
	nodeHiddenEdges_Vm.erase(nodeHiddenEdges_Vm.begin(), nodeHiddenEdges_Vm.end());
	vector<map<int, bool> >().swap(nodeHiddenEdges_Vm);
	// 3
	labelNoOfIncidentEdges_Vi.erase(labelNoOfIncidentEdges_Vi.begin(), labelNoOfIncidentEdges_Vi.end());
	vector<int>().swap(labelNoOfIncidentEdges_Vi);

	edge_iterator ei, ei_end, next_e;
	boost::tie(ei, ei_end) = edges(G);
	for(next_e = ei; ei!=ei_end; ei=next_e)
	{
		++next_e;
		remove_edge(*ei, G);
	}
	vertex_iterator vi, vi_end, next;
	boost::tie(vi, vi_end) = vertices(G);
	for(next = vi; vi!=vi_end; vi=next)
	{
		++next;
		remove_vertex(*vi, G);
	}
	graphID_i = -1;
	noOfValidNodes_i = 0;
	graphCode = newGraphCode = "";
	edgeAddedOrRemovedsource = edgeAddedOrRemovedtarget = nodeAddedOrRemoved = -1;
	G.clear();
}

bool Graph::BLoadGraph(int graphID_i, vector<int> node_ids, vector<int> node_labs,
		vector<int> edge_n1, vector<int> edge_n2, vector<int> edge_labs)
{
	this->graphID_i =  graphID_i;
	if(!BLoadNodes(node_ids, node_labs))
		ErrorExit("Graph::Graph", "Unable to load the nodes");
	
	if(!BLoadEdges(edge_n1, edge_n2, edge_labs))
		ErrorExit("Graph::Graph", "Unable to load the edges");

	FindNoOfIncidentEdges();
	return true;
}

bool Graph::BInsertEdge(int node1, int node2, int edge_label)
{
	if(node1 >= allNodes_Vn.size() || node2>= allNodes_Vn.size())
		return false;

	if(BIsThisEdgeExisting(node1, node2, edge_label))
		return false;
	boost::add_edge(NGetNodeWithID(node1), NGetNodeWithID(node2), edge_label, G);
	if(freqEdgeLabels_Vi.size() <= edge_label) // update freqEdgeLabels_Vi
	{
		for(int j=freqEdgeLabels_Vi.size(); j<=edge_label; j++)
			freqEdgeLabels_Vi.push_back(0);
	}
	freqEdgeLabels_Vi[edge_label]++;
	FindNoOfIncidentEdges();
	return true;
}

bool Graph::BDeleteEdge(int node1, int node2, int edge_label)
{
	if(node1 >= allNodes_Vn.size() || node2>= allNodes_Vn.size())
		return false;
	Edge del_edge = EGetEdgeUndirected(node1, node2);
	boost:remove_edge(del_edge, G);
	freqEdgeLabels_Vi[edge_label]--;
	FindNoOfIncidentEdges();
	return true;
}

int Graph::BInsertNode(int label)
{
	noOfValidNodes_i+=1;
	int node = noOfValidNodes_i-1;
	nodeLabels_Vi.push_back(label); // store the node label
	// nodeIDs_nMnid[temp_n] = node; // store the node->nodeID mapping

	Node temp_node = boost::add_vertex(label, G);
	allNodes_Vn.push_back(temp_node);
	isValidNode_Vb.push_back(true); // remember the valid node id
	if(labelFreqs_Vi.size() <= label)
	{
		for(int j=labelFreqs_Vi.size(); j < label+1; j++)
			labelFreqs_Vi.push_back(0);
	}
	labelFreqs_Vi[label]++;
	map<int, bool> emptyMap;
	nodeHiddenEdges_Vm.push_back(emptyMap);
	emptyMap.~map<int, bool>();
	FindNoOfIncidentEdges();
	return node;
}

// Load Nodes
bool Graph::BLoadNodes(vector<int> node_ids, vector<int> node_labs)
{
	int noOfNodes_i;
	noOfNodes_i = node_ids.size();

	noOfValidNodes_i = noOfNodes_i; // set the number of valid nodes
	for(int i = 0; i < noOfNodes_i; i++)
	{
		int nodeID_i, nodeLabel_i;
		nodeID_i = node_ids[i];
		nodeLabel_i = node_labs[i];

		// assert(nodeID_i == i); // make sure that the nodes are numbered continously

		// load the node
		// Node temp_n = new_node(); // create new node
		//
		// allNodes_Vn.push_back(temp_n); // remember the node
		// temp_n.~Node();

		Node temp_node = boost::add_vertex(nodeLabel_i, G);
		allNodes_Vn.push_back(temp_node);
		nodeLabels_Vi.push_back(nodeLabel_i); // store the node label

		// nodeIDs_nMnid[node] = nodeID_i; // store the node->nodeID mapping
		isValidNode_Vb.push_back(true); // remember the valid node id

		// set the label frequencies
		if(labelFreqs_Vi.size() <= nodeLabel_i)
		{
			for(int j=labelFreqs_Vi.size(); j < nodeLabel_i+1; j++)
				labelFreqs_Vi.push_back(0);
		}
		labelFreqs_Vi[nodeLabel_i]++;
	}

	//assert(number_of_nodes() == noOfNodes_i); // make sure that all the node are read and initialized

	// Initialize the nodeHiddenEdges_Vm
	// Its default size should be equal to the number of nodes
	for(int i=0; i < noOfNodes_i; i++)
	{
		map<int, bool> emptyMap;
		nodeHiddenEdges_Vm.push_back(emptyMap);
		emptyMap.~map<int, bool>();
	}
	return true;
}

// Load Edges
bool Graph::BLoadEdges(vector<int> edge_n1, vector<int> edge_n2, vector<int> edge_labs)
{
	int noOfEdges_i;
	noOfEdges_i = edge_n1.size();
	pair<Edge, bool> e1;
	for(int i = 0; i < noOfEdges_i; i++)
	{
		int node1_i, node2_i, edgeLabel_i;
		node1_i = edge_n1[i];
		node2_i = edge_n2[i];
		edgeLabel_i = edge_labs[i];
		//assert(node1_i < number_of_nodes()); // make sure that the index is correct
		//assert(node2_i < number_of_nodes()); // make sure that the index is correct

		// Edge eTemp = new_edge(allNodes_Vn[node1_i], allNodes_Vn[node2_i]); // create the edge
		// edgeLabel_eM[eTemp] = edgeLabel_i;
		e1 = boost::add_edge(NGetNodeWithID(node1_i), NGetNodeWithID(node2_i), edgeLabel_i, G);
		// std::cout << IGetSourceNode(e1.first) << ' ' << IGetTargetNode(e1.first) << ' ' << IGetEdgeLabel(e1.first) << endl;
		if(freqEdgeLabels_Vi.size() <= edgeLabel_i) // update freqEdgeLabels_Vi
		{
			for(int j=freqEdgeLabels_Vi.size(); j<=edgeLabel_i; j++)
				freqEdgeLabels_Vi.push_back(0);
		}
		freqEdgeLabels_Vi[edgeLabel_i]++;
	}
	// boost::remove_edge(e1.first, G);

	//assert(number_of_edges() == noOfEdges_i); // make sure that all the edges are read
	return true;
}

void Graph::FindGraphCode()
{
	/*
	 * To check if you are visiting the same subgraph of same G_i again
	 *
	 * For each node id 'i', all the node ids incident on 'i' and greater
	 * than 'i' is stored in index[i].
	 * For each 'i', index[i] is sorted
	 * graphCode: i.index[i][0].i.index[i][1]....i+1.index[i+1][0].....
	 *                      for all i=0...number_of_nodes()-1
	 * */
	vector< vector<int> > index;
	for(int i=0; i < num_vertices(G); i++)
	{
		vector<int> temp;
		index.push_back(temp);
	}

	edge_iterator ei, ei_end;
	vertex_index v_index = boost::get(boost::vertex_index_t(), G);
	for(boost::tie(ei, ei_end) = boost::edges(G); ei!=ei_end; ++ei)
	{

		int source_vertex = v_index[boost::source(*ei, G)];
		int target_vertex = v_index[boost::target(*ei, G)];
		if(source_vertex < target_vertex)
			index[source_vertex].push_back(target_vertex);
		else
			index[target_vertex].push_back(source_vertex);
	}

	// Sorting
	for(int i=0; i < num_vertices(G); i++)
		sort(index[i].begin(), index[i].end());

	graphCode = "";
	for(int i=0; i < num_vertices(G); i++)
	{
		for(int j=0; j < index[i].size(); j++)
		{
			char *tempstr = (char*)malloc(sizeof(char)*100);
			stringstream sstr1,sstr2;
			sstr1 << i;
			sstr2 << index[i][j];
			string tempStr1 = sstr1.str();
			string tempStr2 = sstr2.str();
			graphCode += tempStr1+"."+tempStr2+".";
			if(tempstr != NULL)
			{
				free(tempstr);
				tempstr = NULL;
			}
		}
	}

}

void Graph::PrintHiddenEdgesCode()
{
	/*
	 * To check if you are visiting the same subgraph of same G_i again
	 *
	 * For each node id 'i', all the node ids incident on 'i' and greater
	 * than 'i' is stored in index[i].
	 * For each 'i', index[i] is sorted
	 * graphCode: i.index[i][0].i.index[i][1]....i+1.index[i+1][0].....
	 *                      for all i=0...number_of_nodes()-1
	 * */
	for(int i=0;i<hiddenEdges_Ve.size();i++)
	{
		std::cout << hiddenEdges_Ve[i].first.first << ' ' << hiddenEdges_Ve[i].first.second << '\n';
	}
	std::cout << "\n" << '\n';
	vector< vector<int> > index;
	for(int i=0; i < num_vertices(G); i++)
	{
		vector<int> temp;
		index.push_back(temp);
	}

	//graph::edge_iterator begin_eIt = edges_begin();
	//graph::edge_iterator end_eIt = edges_end();
	vertex_index v_index = boost::get(boost::vertex_index_t(), G);
	for(int i=0; i<hiddenEdges_Ve.size(); i++)
	{
		int source_vertex = hiddenEdges_Ve[i].first.first;
		int target_vertex = hiddenEdges_Ve[i].first.second;
		if(source_vertex < target_vertex)
			index[source_vertex].push_back(target_vertex);
		else
			index[target_vertex].push_back(source_vertex);
	}

	// Sorting
	for(int i=0; i < num_vertices(G); i++)
		sort(index[i].begin(), index[i].end());

	for(int i=0; i < num_vertices(G); i++)
	{
		for(int j=0; j < index[i].size(); j++)
		{
			char *tempstr = (char*)malloc(sizeof(char)*100);
			stringstream sstr1,sstr2;
			sstr1 << i;
			sstr2 << index[i][j];
			string tempStr1 = sstr1.str();
			string tempStr2 = sstr2.str();
			cout << tempStr1 << "." << tempStr2 << ".";
			if(tempstr != NULL)
			{
				free(tempstr);
				tempstr = NULL;
			}
		}
	}

}

// Print
void Graph::PrintFile(char *fileName)
{
	ofstream outfs;
	outfs.open(fileName, ofstream::app);
	//outfs << "t # " << graphID_i << endl;

	GraphType type_gt = GetGraphType();
	if(type_gt == GRAPH_EMPTY)
	{
		outfs << "EMPTY" << endl;
	}
	else if(type_gt == GRAPH_SINGLE_NODE)
	{
		outfs << "SINGLE NODE :";
		for(int i=0; i<num_vertices(G); i++)
			if(isValidNode_Vb[i])
			{
				outfs << "v " << i << " " << nodeLabels_Vi[i] << endl ;
				break;
			}
		outfs << endl;
	}
	else
	{
		for(int i=0; i < num_vertices(G); i++)
			if(isValidNode_Vb[i])
				outfs << "v " << i << " " << nodeLabels_Vi[i] << endl ;

		// graph::edge_iterator begin_eIt = edges_begin();
		// graph::edge_iterator end_eIt = edges_end();
		edge_iterator ei, ei_end;
		vertex_index v_index = boost::get(boost::vertex_index_t(), G);
		edge_color e_color = boost::get(boost::edge_color_t(), G);
		for(boost::tie(ei, ei_end) = boost::edges(G); ei!=ei_end; ++ei)
		{
			outfs << "u " << v_index[boost::source(*ei, G)] << " " << v_index[boost::target(*ei, G)];
			outfs << " " << e_color[*ei] << endl;
		}
	}
	outfs.close();
}


// Print
void Graph::Print()
{
	//cout << "t # " << graphID_i << endl;

	GraphType type_gt = GetGraphType();
	if(type_gt == GRAPH_EMPTY)
	{
		cout << "EMPTY" << endl;
	}
	else if(type_gt == GRAPH_SINGLE_NODE)
	{
		cout << "SINGLE NODE :";
		for(int i=0; i<num_vertices(G); i++)
			if(isValidNode_Vb[i])
			{
				PrintNode(i);
				break;
			}
		cout << endl;
	}
	else
	{
		for(int i=0; i < num_vertices(G); i++)
			if(isValidNode_Vb[i])
				PrintNode(i);

		edge_iterator ei, ei_end;
		vertex_index v_index = boost::get(boost::vertex_index_t(), G);
		edge_color e_color = boost::get(boost::edge_color_t(), G);
		for(boost::tie(ei, ei_end) = boost::edges(G); ei!=ei_end; ++ei)
		{
			cout << "u " << v_index[boost::source(*ei, G)] << " " << v_index[boost::target(*ei, G)];
			cout << " " << e_color[*ei] << endl;
		}
		cout << "GraphCode: " << graphCode << endl;
		cout << "NewGraphCode: " << newGraphCode << endl;

#if 0 // debug
		PrintHiddenEdgesCode();
		for(int i=0; i < hiddenEdges_Ve.size(); i++)
		{
			cout << "(" << nodeIDs_nMnid[hiddenEdges_Ve[i].source()] << "," << nodeIDs_nMnid[hiddenEdges_Ve[i].target()] << ") ";
		}
		cout << endl;
#endif
	}

}




/*

// Print
void Graph::Print()
{
//cout << "t # " << graphID_i << endl;

GraphType type_gt = GetGraphType();
if(type_gt == GRAPH_EMPTY)
{
cout << "EMPTY" << endl;
}
else if(type_gt == GRAPH_SINGLE_NODE)
{
cout << "SINGLE NODE :";
for(int i=0; i<number_of_nodes(); i++)
if(isValidNode_Vb[i])
{
PrintNode(i);
break;
}
cout << endl;
}
else
{
for(int i=0; i < number_of_nodes(); i++)
if(isValidNode_Vb[i])
PrintNode(i);

graph::edge_iterator begin_eIt = edges_begin();
graph::edge_iterator end_eIt = edges_end();

while(begin_eIt != end_eIt)
{
cout << "u " << nodeIDs_nMnid[(*begin_eIt).source()] << " " << nodeIDs_nMnid[(*begin_eIt).target()];
cout << " " << edgeLabel_eM[(*begin_eIt)] << endl;
begin_eIt++;
}
}

}*/


// Print
void Graph::PrintDebug()
{
	cout << graphID_i << " [" << IGetNoOfValidNodes() << " : " << num_edges(G) << "] ";

	GraphType type_gt = GetGraphType();
	if(type_gt == GRAPH_EMPTY)
	{
		cout << "EMPTY" << endl;
	}
	else if(type_gt == GRAPH_SINGLE_NODE)
	{
		cout << "SINGLE NODE :";
		for(int i=0; i<num_vertices(G); i++)
			if(isValidNode_Vb[i])
			{
				PrintNode(i);
				break;
			}
		cout << endl;
	}
	else
	{

		edge_iterator ei, ei_end;
		vertex_index v_index = boost::get(boost::vertex_index_t(), G);
		edge_color e_color = boost::get(boost::edge_color_t(), G);
		for(boost::tie(ei, ei_end) = boost::edges(G); ei!=ei_end; ++ei)
		{
			cout << "(";
			PrintNode(boost::source(*ei, G));
			cout << ",";
			PrintNode(boost::target(*ei, G));
			cout << "), ";
		}
		cout << endl;
	}

	cout << "Label Freqs: ";
	for(int i=0; i < labelFreqs_Vi.size(); i++)
		cout << "[" << i << " - " << labelFreqs_Vi[i] << "], ";
	cout << endl;

	cout << "Label Incident Edges Freqs: ";
	for(int i=0; i < labelNoOfIncidentEdges_Vi.size(); i++)
		cout << "[" << i << " - " << labelNoOfIncidentEdges_Vi[i] << "], ";
	cout << endl;

	cout << "No Of Valid Nodes: " << noOfValidNodes_i << " -- ";
	for(int i=0; i < num_vertices(G); i++)
		if(isValidNode_Vb[i])
		{
			PrintNode(i);
			cout << ", ";
		}
	cout << endl;

	cout << "No Of Hidden Edges: " << hiddenEdges_Ve.size();
	cout << " :";
	for(int i=0; i < hiddenEdges_Ve.size(); i++)
	{
		PrintEdge(hiddenEdges_Ve[i]);
		cout << "; ";
	}
	cout << endl;

	cout << "Node Hidden Edges: " << endl;
	for(int i=0; i < num_vertices(G); i++)
	{
		cout << "Node: ";
		PrintNode(i);
		cout << " : [" << nodeHiddenEdges_Vm[i].size() << "] - ";
		map<int, bool>::iterator begin_mIt = nodeHiddenEdges_Vm[i].begin();
		map<int, bool>::iterator end_mIt = nodeHiddenEdges_Vm[i].end();
		while(begin_mIt != end_mIt)
		{
			cout << "(" << (*begin_mIt).first << "," << (*begin_mIt).second << "), ";
			begin_mIt++;
		}
		cout << endl;
	}
}

// Print Node
void Graph::PrintNode(Node nd)
{
	vertex_index v_index = boost::get(boost::vertex_index_t(), G);
	vertex_color v_color = boost::get(boost::vertex_color_t(), G);
	cout << "v " << v_index[nd] << " " << v_color[nd] << endl ;
}

// Print Node
void Graph::PrintNode(NodeID ndID)
{
	cout << "v " << ndID << " " << nodeLabels_Vi[ndID] << endl ;
}

void Graph::PrintEdge(Edge ed)
{
	edge_color e_color = boost::get(boost::edge_color_t(), G);
	PrintNode(boost::source(ed, G));
	cout << " -> ";
	PrintNode(boost::target(ed, G));
	cout << "[" << e_color[ed] << "] ";
}

void Graph::PrintEdge(HiddenEdge ed)
{
	PrintNode(ed.first.first);
	cout << " -> ";
	PrintNode(ed.first.second);
	cout << "[" << ed.second << "] ";
}

void Graph::PrintGraphType()
{
	GraphType type_gt = GetGraphType();
	if(type_gt == GRAPH_EMPTY)
		cout << "EMPTY";
	else if(type_gt == GRAPH_SINGLE_NODE)
		cout << "SINGLE NODE";
	else if(type_gt == GRAPH_SINGLE_EDGE)
		cout << "SINGLE EDGE";
	else
		cout << "GENERAL";
}

void Graph::ShallowCopy(Graph* g)
{
	graphID_i = g->IGetGraphID();

	// insert the nodes
	noOfValidNodes_i = g->IGetNoOfValidNodes();
	// std::cout << g->IGetNoOfAllNodes() << '\n';
	for(int i=0; i < g->IGetNoOfValidNodes(); i++)
	{
		// Node nd = new_node();
		nodeLabels_Vi.push_back(g->IGetNodeLabel(i));
		Node nd = boost::add_vertex(g->IGetNodeLabel(i), G);
		// nodeIDs_nMnid[nd] = i;
		allNodes_Vn.push_back(nd);
		// nd.~Node();

		// update the valid nodes
		isValidNode_Vb.push_back(true);

		// initialize nodeHiddenEdges_Vm
		map<int, bool> emptyMap;
		nodeHiddenEdges_Vm.push_back(emptyMap);
		//emptyMap.~map<int, bool>();
	}
	for(int i=0; i < g->IGetSizeLabelFreqs(); i++)
		labelFreqs_Vi.push_back(g->IGetLabelFreq(i));

	for(int i=0; i < g->IGetSizeFreqEdgeLabels(); i++)
		freqEdgeLabels_Vi.push_back(g->IGetFreqEdgeLabel(i));

	// insert the edges
	edge_iterator ei, ei_end;
	// std::cout << allNodes_Vn.size() << '\n';
	for(boost::tie(ei, ei_end) = g->PEGetAllEdges(); ei!=ei_end; ++ei)
	{
		int source_i = g->IGetSourceNode(*ei);
		int target_i = g->IGetTargetNode(*ei);
		boost::add_edge(NGetNodeWithID(source_i), NGetNodeWithID(target_i), g->IGetEdgeLabel(*ei), G);
	}

	FindNoOfIncidentEdges();

	graphCode = g->GetGraphCode();
	newGraphCode = g->GetNewGraphCode();

}

void Graph::ExactgraphCopy(Graph* g)
{
	vector<int> node_ids(0), node_labs(0), edge_n1(0), edge_n2(0), edge_labs(0);
	int index = 0;
	map<int, int> map_orgcopy;
	for(int i=0; i < g->IGetNoOfAllNodes(); i++)
	{
		if(g->BIsValidNode(i)){
			node_ids.push_back(index);
			node_labs.push_back(g->IGetNodeLabel(i));
			map_orgcopy.insert(make_pair(i, index));
			index++;
		}
	}
	edge_iterator ei, ei_end;
	for(boost::tie(ei, ei_end) = g->PEGetAllEdges(); ei!=ei_end; ++ei)
	{
		int source_i = map_orgcopy[g->IGetSourceNode(*ei)];
		int target_i = map_orgcopy[g->IGetTargetNode(*ei)];
		edge_n1.push_back(source_i);
		edge_n2.push_back(target_i);
		edge_labs.push_back(g->IGetEdgeLabel(*ei));
	}

	if(!BLoadGraph(g->IGetGraphID(), node_ids, node_labs, edge_n1, edge_n2, edge_labs))
		ErrorExit("BMCS()", "Error in load graph");

	graphCode = g->GetGraphCode();
	newGraphCode = g->GetNewGraphCode();
}

void Graph::Copy(Graph *g)
{
	graphID_i = g->IGetGraphID();

	// insert the nodes
	noOfValidNodes_i = g->IGetNoOfValidNodes();
	// std::cout << g->IGetNoOfAllNodes() << '\n';
	for(int i=0; i < g->IGetNoOfAllNodes(); i++)
	{
		// Node nd = new_node();
		nodeLabels_Vi.push_back(g->IGetNodeLabel(i));
		Node nd = boost::add_vertex(g->IGetNodeLabel(i), G);
		// nodeIDs_nMnid[nd] = i;
		allNodes_Vn.push_back(nd);
		// nd.~Node();

		// update the valid nodes
		isValidNode_Vb.push_back(g->BIsValidNode(i));

		// initialize nodeHiddenEdges_Vm
		map<int, bool> emptyMap;
		nodeHiddenEdges_Vm.push_back(emptyMap);
		//emptyMap.~map<int, bool>();
	}
	for(int i=0; i < g->IGetSizeLabelFreqs(); i++)
		labelFreqs_Vi.push_back(g->IGetLabelFreq(i));

	for(int i=0; i < g->IGetSizeFreqEdgeLabels(); i++)
		freqEdgeLabels_Vi.push_back(g->IGetFreqEdgeLabel(i));

	// insert the edges
	edge_iterator ei, ei_end;
	// std::cout << allNodes_Vn.size() << '\n';
	for(boost::tie(ei, ei_end) = g->PEGetAllEdges(); ei!=ei_end; ++ei)
	{
		int source_i = g->IGetSourceNode(*ei);
		int target_i = g->IGetTargetNode(*ei);
		boost::add_edge(NGetNodeWithID(source_i), NGetNodeWithID(target_i), g->IGetEdgeLabel(*ei), G);
	}
	// Update the hidden edge
	for(int i=0; i < g->IGetNoOfHiddenEdges(); i++)
	{
		HiddenEdge hidden_e = g->EGetHiddenEdge(i);
		int source_i = hidden_e.first.first;
		int target_i = hidden_e.first.second;
		int color = hidden_e.second;

		// update hiddenEdges_Ve
		int indexHiddenEdge_i = hiddenEdges_Ve.size();
		hiddenEdges_Ve.push_back(make_pair(make_pair(source_i, target_i), color));

		// update nodeHiddenEdges_Vm
		nodeHiddenEdges_Vm[source_i].insert(pair<int, bool>(indexHiddenEdge_i, true));
		nodeHiddenEdges_Vm[target_i].insert(pair<int, bool>(indexHiddenEdge_i, false));
	}
	FindNoOfIncidentEdges();

	graphCode = g->GetGraphCode();
	newGraphCode = g->GetNewGraphCode();
	edgeAddedOrRemovedsource = g->GetEdgeAddRemoveSource();
	edgeAddedOrRemovedtarget = g->GetEdgeAddRemoveTarget();
	nodeAddedOrRemoved = g->GetNodeAddRemove();
	// edge_iterator ei, ei_end;
	// for(boost::tie(ei, ei_end) = g->PEGetAllEdges(); ei!=ei_end; ++ei)
	// {
	//     // int source_i = g->IGetSourceNode(*ei);
	//     // int target_i = g->IGetTargetNode(*ei);
	//     // boost::add_edge(NGetNodeWithID(source_i), NGetNodeWithID(target_i), g->IGetEdgeLabel(*ei), G);
	//     cout << g->IGetTargetNode(*ei) << ' ' << g->IGetSourceNode(*ei)  <<  endl;
	// }
}

// Check Functions
bool Graph::BIsValidNode(NodeID ndID)
{
	if(isValidNode_Vb[ndID])
		return true;
	return false;
}

bool Graph::BIsValidNode(Node nd)
{
	vertex_index v_index = boost::get(boost::vertex_index_t(), G);
	if(isValidNode_Vb[v_index[nd]])
		return true;
	return false;
}

// Returns true if 'this' is not G_i. False otherwise.
bool Graph::BItIsNotG_i()
{
	if(hiddenEdges_Ve.size() == 0) // if there are no hidden edges, then its not a subgraph
	{
		// When edges are hidden, when you reach a single edge, the single edge is not hidden.
		// Pending: Figure out why this 'if' condition is needed
		// if(noOfValidNodes_i == 1)
		// 	return true;
		return false;
	}
	return true;
}

GraphType Graph::GetGraphType()
{
	// check the number of valid nodes
	if(noOfValidNodes_i == 0)
	{
		//assert(number_of_edges() == 0);
		return GRAPH_EMPTY;
	}

	if(noOfValidNodes_i == 1)
	{
		//assert(number_of_edges() == 0);
		return GRAPH_SINGLE_NODE;
	}

	if(num_edges(G) == 1)
		return GRAPH_SINGLE_EDGE;

	return GRAPH_GENERAL;
}

int Graph::IGetLabelFreq(int index_i)
{
	if(index_i >= labelFreqs_Vi.size())
		return -1;
	return labelFreqs_Vi[index_i];
}

bool Graph::BCanHideEdge(Edge ed)
{
	// hide the edge and check if its okay
	// std::cout << IGetNoOfAllEdges() << '\n';
	edge_color e_color = boost::get(boost::edge_color_t(), G);
	vertex_index v_index = boost::get(boost::vertex_index_t(), G);
	Node source_node = boost::source(ed, G);
	Node target_node = boost::target(ed, G);
	int color = e_color[ed];
	boost::remove_edge(ed, G);

	pair<Edge, bool> added;
	// one of the node is with only one edge incident on it
	// if( (boost::out_degree(source_node, G) == 0) && (boost::out_degree(target_node, G) == 0))
	// {
	// 	added = boost::add_edge(source_node, target_node, color, G);
	// 	// std::cout << "Added edge: " << added.second << '\n';
	// 	// std::cout << "dingding1" << IGetNoOfAllEdges() << '\n';
	// 	return false;
	// }

	if( (boost::out_degree(source_node, G) == 0) || (boost::out_degree(target_node, G) == 0))
	{
		added = boost::add_edge(source_node, target_node, color, G);
		return true;
	}

	vector<NodeID> visitedNodes_nID;
	if(!BBreadFirstSearch(source_node, target_node, visitedNodes_nID))
	{
		added = boost::add_edge(source_node, target_node, color, G);
		return false;
	}

	added = boost::add_edge(source_node, target_node, color, G);
	return true;
}

bool Graph::BBreadFirstSearch(Node start_n, Node search_n, vector<NodeID> &visitedNodes_nID)
{
	visitedNodes_nID.push_back(IGetNodeID(start_n));

	pair<adjacency_iterator, adjacency_iterator> vp = boost::adjacent_vertices(start_n, G);

	while(vp.first != vp.second)
	{
		if(search_n == (*vp.first)) // found the searching element
			return true;
		if(!BIsPresentInNodeIDVector(visitedNodes_nID, IGetNodeID(*vp.first)))
		{
			if(BBreadFirstSearch((*vp.first), search_n, visitedNodes_nID))
				return true;
		}
		++vp.first;
	}
	return false;
}

void Graph::DFS(Node start_n, vector<int> &visited, int limit, int &cnt)
{
	visited[IGetNodeID(start_n)] = 1;
	pair<adjacency_iterator, adjacency_iterator> vp = boost::adjacent_vertices(start_n, G);
	while(vp.first != vp.second)
	{
		if(!visited[IGetNodeID(*vp.first)])
		{
			DFS((*vp.first), visited, limit, cnt);
		}
		++vp.first;
	}
}

// bool Graph::BBreadFirst(Node start_n, vector<NodeID> visitedNodes_nID)
// {
//     visitedNodes_nID.push_back(nodeIDs_nMnid[start_n]);
//     node::adj_nodes_iterator begin_adjnIt = start_n.adj_nodes_begin();
//     node::adj_nodes_iterator end_adjnIt = start_n.adj_nodes_end();
//     while(begin_adjnIt != end_adjnIt)
//     {
//         if(!BIsPresentInNodeIDVector(visitedNodes_nID, nodeIDs_nMnid[(*begin_adjnIt)]))
//         {
// 	  if(BBreadFirstSearch((*begin_adjnIt), search_n, visitedNodes_nID))
// 	    return true;
// 	}
//         begin_adjnIt++;
//     }
//     if(visitedNodes_nID.size()== IGetNoOfValidNodes())
//       return true;
//     else
//       return false;
// }

void Graph::HideEdge(Edge ed, bool check_b)
{
	//this->Print();
	// BCanHideEdge should be called before calling this function
	// We dont check in this function whether it is valid to hide the edge or not

	// Handle single edge graphs as special case.
	// Should be done before calling this function

	// Make sure that the graph is not empty, not single node
	// and not single edge
	GraphType type_gt = GetGraphType();
	// std::cout << type_gt << '\n';
	// Hide the edge 'ed' and if any of the nodes becomes disconnected from
	// the graph, then make them invalidbuild_hash
	Node source_n = boost::source(ed, G);
	Node target_n = boost::target(ed, G);
	vertex_index v_index = boost::get(boost::vertex_index_t(), G);
	edge_color e_color = boost::get(boost::edge_color_t(), G);
	int color = IGetEdgeLabel(ed);
	// std::cout << color << '\n';
	freqEdgeLabels_Vi[color]--;
	//  - Update the hidden edges
	hiddenEdges_Ve.push_back(make_pair(make_pair(IGetSourceNode(ed), IGetTargetNode(ed)), color));
	// std::cout << "/* message */" << '\n';
	boost::remove_edge(ed, G);
	// std::cout << "message2" << '\n';
	// Make sure that both the nodes are not becoming disconnected from the
	// graph. At most one of it might be disconnected
	//if(check_b)
	// assert ( !( (source_n.degree()==0) && (target_n.degree()==0)) );
	// std::cout << "her3" << '\n';
	// Update the valid nodes
	v_index = boost::get(boost::vertex_index_t(), G);
	e_color = boost::get(boost::edge_color_t(), G);
	if(boost::out_degree(source_n, G) == 0)
	{
		// std::cout << "part1" << '\n';
		isValidNode_Vb[v_index[source_n]] = false;
		noOfValidNodes_i--;
	}
	else if(boost::out_degree(target_n, G) == 0)
	{
		// std::cout << "part2" << '\n';
		isValidNode_Vb[v_index[target_n]] = false;
		noOfValidNodes_i--;
	}
	// std::cout << "/* message */" << '\n';
	// Update the list of hidden edges and the node information related to it
	int indexOfEdge_i = hiddenEdges_Ve.size() - 1; // if we put the edge, its index in the vector will be this


	//  - Update the node info
	nodeHiddenEdges_Vm[v_index[source_n]].insert(pair<int, bool>(indexOfEdge_i, true));
	nodeHiddenEdges_Vm[v_index[target_n]].insert(pair<int, bool>(indexOfEdge_i, false));

	// update edgelabel freqs
	int sourceID = v_index[source_n];
	int targetID = v_index[target_n];

	//assert(labelNoOfIncidentEdges_Vi[nodeLabels_Vi[sourceID]] > 0);
	labelNoOfIncidentEdges_Vi[nodeLabels_Vi[sourceID]]--;
	//assert(labelNoOfIncidentEdges_Vi[nodeLabels_Vi[targetID]] > 0);
	labelNoOfIncidentEdges_Vi[nodeLabels_Vi[targetID]]--;

	stringstream sstr1,sstr2;
	if(sourceID < targetID)
	{
		sstr1 << sourceID;
		sstr2 << targetID;
	}
	else
	{
		sstr1 << targetID;
		sstr2 << sourceID;
	}
	string str1 = sstr1.str();
	string str2 = sstr2.str();
	string searchStr = str1+"."+str2+".";
	string::size_type pos = graphCode.find(searchStr,0);
	string strFirst = graphCode.substr(0,pos);
	string strSec = graphCode.substr(pos+searchStr.size(),graphCode.size()+2);
	graphCode = strFirst + strSec;

	int l1 = nodeLabels_Vi[sourceID];
	int l2 = nodeLabels_Vi[targetID];
	int lc = color;
	stringstream tempStream;
	if(l1 < l2)
		tempStream << l1 << "." << lc << "." << l2 << ".";
	else
		tempStream << l2 << "." << lc << "." << l1 << ".";
	string tempStr = tempStream.str();
	pos = newGraphCode.find(tempStr, 0);
	str1 = newGraphCode.substr(0, pos);
	str2 = newGraphCode.substr(pos+tempStr.size(), newGraphCode.size()-pos-tempStr.size());
	newGraphCode = str1+str2;
}

// HideEdge method: version if the edge is a single edge
//   - for this we need more information as to make which of the nodes as valid
void Graph::HideEdge(Edge ed, Node nd)
{
	// make sure that the graph is single edge
	GraphType type_gt = GetGraphType();
	//assert (type_gt == GRAPH_SINGLE_EDGE);

	// Hide the edge 'ed' and if any of the nodes becomes disconnected from
	// the graph, then make them invalid
	Node source_n = boost::source(ed, G);
	Node target_n = boost::target(ed, G);
	edge_color e_color = boost::get(boost::edge_color_t(), G);
	vertex_index v_index = boost::get(boost::vertex_index_t(), G);
	int color = e_color[ed];

	// make sure that the one of the nodes is 'nd'
	//assert( (source_n == nd) || (target_n == nd) );

	freqEdgeLabels_Vi[color]--;
	//  - Update the hidden edges
	hiddenEdges_Ve.push_back(make_pair(make_pair(v_index[source_n], v_index[target_n]), color));
	boost::remove_edge(ed, G);

	// make the node other than 'nd' as invalid
	e_color = boost::get(boost::edge_color_t(), G);
	v_index = boost::get(boost::vertex_index_t(), G);
	if(source_n == nd)
	{
		isValidNode_Vb[v_index[target_n]] = false;
		noOfValidNodes_i--;
		stringstream sstr;
		sstr << v_index[source_n];
		string tempStr = sstr.str();
		//graphCode = tempStr+".";
		graphCode = tempStr;

		stringstream nstr;
		nstr << nodeLabels_Vi[v_index[source_n]];
		newGraphCode = nstr.str();
	}
	else if(target_n == nd)
	{
		isValidNode_Vb[v_index[source_n]] = false;
		noOfValidNodes_i--;
		stringstream sstr;
		sstr << v_index[target_n];
		string tempStr = sstr.str();
		//graphCode = tempStr+".";
		graphCode = tempStr;

		stringstream nstr;
		nstr << nodeLabels_Vi[v_index[target_n]];
		newGraphCode = nstr.str();
	}

	// Update the list of hidden edges and the node information related to it
	int indexOfEdge_i = hiddenEdges_Ve.size() - 1; // if we put the edge, its index in the vector will be this

	//  - Update the node info
	nodeHiddenEdges_Vm[v_index[source_n]].insert(pair<int, bool>(indexOfEdge_i, true));
	nodeHiddenEdges_Vm[v_index[target_n]].insert(pair<int, bool>(indexOfEdge_i, false));

	// update the edge label freqs
	//assert(labelNoOfIncidentEdges_Vi[nodeLabels_Vi[nodeIDs_nMnid[source_n]]] > 0);
	labelNoOfIncidentEdges_Vi[nodeLabels_Vi[v_index[source_n]]]--;
	//assert(labelNoOfIncidentEdges_Vi[nodeLabels_Vi[nodeIDs_nMnid[target_n]]] > 0);
	labelNoOfIncidentEdges_Vi[nodeLabels_Vi[v_index[target_n]]]--;
}

void Graph::RestoreEdge(HiddenEdge hidden_ed)
{
	// Make sure that the edges are not restored on a empty graph
	//          -- first the node should be made valid
	GraphType type_gt = GetGraphType();
	//assert(type_gt != GRAPH_EMPTY);

	// We test in this method that if the edge is restored will it create
	// disconnected parts in the graph
	//      - If the graph is neither single node nor single edge
	//      - then by restoring this edge this should create disconnected parts
	//if( type_gt == GRAPH_GENERAL)
	//assert( !( (ed.source().degree() == 0) && (ed.target().degree()==0) ));

#if 0
	// debug
	cout << "Restoring Edge: ";
	PrintEdge(ed);
	cout << endl;
	cout << "Edge Label: " << edgeLabel_eM[ed] << endl;
#endif

	freqEdgeLabels_Vi[hidden_ed.second]++;
	boost::add_edge(NGetNodeWithID(hidden_ed.first.first), NGetNodeWithID(hidden_ed.first.second), hidden_ed.second, G);
	// remove it from the list of hidden edges
	int indexHiddenEdge_i=0;
	vector<HiddenEdge>::iterator begin_VeIt = hiddenEdges_Ve.begin();
	vector<HiddenEdge>::iterator end_VeIt = hiddenEdges_Ve.end();
	while(begin_VeIt != end_VeIt)
	{
		if((*begin_VeIt).first.first == hidden_ed.first.first && (*begin_VeIt).first.second == hidden_ed.first.second && (*begin_VeIt).second == hidden_ed.second)
			break;
		indexHiddenEdge_i++;
		begin_VeIt++;
	}
	//assert(begin_VeIt != end_VeIt); // 'ed' should be definitely found

	// Remove it from the hiddenEdges_Ve
	hiddenEdges_Ve.erase(begin_VeIt);

	// Remove it from the nodeHiddenEdges_Vm
	int source_nID = hidden_ed.first.first;
	int target_nID = hidden_ed.first.second;
  // std::cout << indexHiddenEdge_i << hiddenEdges_Ve.size() << '\n';
	// Remove for source_n
	map<int, bool>::iterator find_mIt = nodeHiddenEdges_Vm[source_nID].find(indexHiddenEdge_i);
	//assert(find_mIt != nodeHiddenEdges_Vm[nodeIDs_nMnid[source_n]].end()); // make sure that its found
	nodeHiddenEdges_Vm[source_nID].erase(find_mIt);

	// Remove for target_n
	find_mIt = nodeHiddenEdges_Vm[target_nID].find(indexHiddenEdge_i);
	//assert(find_mIt != nodeHiddenEdges_Vm[nodeIDs_nMnid[target_n]].end()); // make sure that its found
	nodeHiddenEdges_Vm[target_nID].erase(find_mIt);

	// Update valid nodes
	// make sure that atleast one of the nodes is valid
	//assert( !( (!isValidNode_Vb[nodeIDs_nMnid[source_n]]) && (!isValidNode_Vb[nodeIDs_nMnid[target_n]]) ) );

	if(!isValidNode_Vb[source_nID])
	{
		isValidNode_Vb[source_nID] = true;
		noOfValidNodes_i++;
	}
	else if(!isValidNode_Vb[target_nID])
	{
		isValidNode_Vb[target_nID] = true;
		noOfValidNodes_i++;
	}

	// update the edge label freqs
	labelNoOfIncidentEdges_Vi[nodeLabels_Vi[source_nID]]++;
	labelNoOfIncidentEdges_Vi[nodeLabels_Vi[target_nID]]++;

	FindGraphCode();
	FindNewGraphCode();
}

NodeID Graph::IGetTheSingleNode()
{
	// make sure that the graph is single node
	//assert(GetGraphType() == GRAPH_SINGLE_NODE);

	for(int i=0; i<num_vertices(G); i++)
		if(isValidNode_Vb[i])
			return i;

	// shouldn't reach here
	return -1;
}

bool Graph::BIsThisEdgeExisting(int source_i, int target_i, int label_i)
{
	edge_iterator ei, ei_end;
	for(boost::tie(ei, ei_end) = boost::edges(G); ei!=ei_end; ++ei)
	{
		if( (IGetSourceNode(*ei) == source_i) &&
				(IGetTargetNode(*ei) == target_i) )
			if(IGetEdgeLabel(*ei) == label_i)
				return true;
			if( (IGetSourceNode(*ei) == target_i) &&
						(IGetTargetNode(*ei) == source_i) )
					if(IGetEdgeLabel(*ei) == label_i)
						return true;
	}
	return false;
}

bool Graph::BIsThisEdgeHidingUndirected(int source_i, int target_i)
{
	vertex_index v_index = boost::get(boost::vertex_index_t(), G);
	edge_color e_color = boost::get(boost::edge_color_t(), G);
	for(int i=0; i < hiddenEdges_Ve.size(); i++)
	{
		if( (hiddenEdges_Ve[i].first.first == source_i)
				&& (hiddenEdges_Ve[i].first.second == target_i) )
			return true;
		else if( (hiddenEdges_Ve[i].first.second == source_i)
				&& (hiddenEdges_Ve[i].first.first == target_i) )
			return true;
	}
	return false;
}
Edge Graph::EGetEdge(int source_i, int target_i)
{
	// optimizing this code
	pair<out_edge_iterator, out_edge_iterator> edges = boost::out_edges(NGetNodeWithID(source_i), G);
	while(edges.first != edges.second)
	{
		if( (IGetSourceNode(*edges.first) == source_i) &&
				(IGetTargetNode(*edges.first) == target_i) )
			return (*edges.first);
		++edges.first;
	}
	// std::cout << "Here problem" << '\n';
	ErrorExit("Graph::EGetEdge", "Unable to find edge");
}

pair<out_edge_iterator, out_edge_iterator> Graph::GetEdges(int node_id)
{
	return boost::out_edges(node_id, G);
}

bool Graph::HasEdge(int source_i, int target_i)
{
	// optimizing this code
	pair<out_edge_iterator, out_edge_iterator> edges = boost::out_edges(NGetNodeWithID(source_i), G);
	while(edges.first != edges.second)
	{
		if( (IGetSourceNode(*edges.first) == source_i) &&
				(IGetTargetNode(*edges.first) == target_i) )
			return true;
		if( (IGetSourceNode(*edges.first) == target_i) &&
					(IGetTargetNode(*edges.first) == source_i) )
			return true;
		++edges.first;
	}
	return false;
}

bool Graph::HasEdgeLabelCheck(int source_label, int target_label, int edge_label)
{
	edge_iterator ei, ei_end;
	for(boost::tie(ei, ei_end) = boost::edges(G); ei!=ei_end; ++ei)
	{
		if( (IGetNodeLabel(IGetSourceNode(*ei)) == source_label) &&
				(IGetNodeLabel(IGetTargetNode(*ei)) == target_label) &&
				IGetEdgeLabel(*ei) == edge_label)
			return true;
		else if((IGetNodeLabel(IGetSourceNode(*ei)) == target_label) &&
				(IGetNodeLabel(IGetTargetNode(*ei)) == source_label) &&
				IGetEdgeLabel(*ei) == edge_label )
			return true;
	}
	return false;
}

Edge Graph::EGetEdgeUndirected(int source_i, int target_i)
{
	edge_iterator ei, ei_end;
	for(boost::tie(ei, ei_end) = boost::edges(G); ei!=ei_end; ++ei)
	{
		if( (IGetSourceNode(*ei) == source_i) &&
				(IGetTargetNode(*ei) == target_i) )
			return (*ei);
		else if( (IGetSourceNode(*ei) == target_i) &&
				(IGetTargetNode(*ei) == source_i) )
			return (*ei);
	}

	ErrorExit("Graph::EGetEdge", "Unable to find edge");
}

HiddenEdge Graph::EGetHiddenEdge(NodeID source_nID, NodeID target_nID)
{
	for(int i=0; i < hiddenEdges_Ve.size(); i++)
	{
		if( (hiddenEdges_Ve[i].first.first == source_nID)
				&& (hiddenEdges_Ve[i].first.second == target_nID) )
			return hiddenEdges_Ve[i];
	}

	ErrorExit("Graph::EGetHiddenEdge", "Error in finding the hidden edge");
}

HiddenEdge Graph::EGetHiddenEdgeUndirected(NodeID source_nID, NodeID target_nID)
{
	for(int i=0; i < hiddenEdges_Ve.size(); i++)
	{
		if( (hiddenEdges_Ve[i].first.first == source_nID)
				&& (hiddenEdges_Ve[i].first.second == target_nID) )
			return hiddenEdges_Ve[i];
		else if( (hiddenEdges_Ve[i].first.second == source_nID)
				&& (hiddenEdges_Ve[i].first.first == target_nID) )
			return hiddenEdges_Ve[i];
	}

	ErrorExit("Graph::EGetHiddenEdge", "Error in finding the hidden edge");
}


// return 1 when the first frequent parent is found. Also return the
//                  infrequent parents found so far
// return -1 if no frequent parents are found along with all the infrequent
//                      parents
int Graph::GetFirstFreqParent(int noOfGraphDB, Graph *graphDB,
		map<int, map<int, knownGraphNode*> > &knownGraphsMap,
		int minSup, vector <Graph*> *ansGraphs, vector<int> *toBeAdded)
{

	// Handle empty and single node graphs as special case
	GraphType type_gt = GetGraphType();

	if(type_gt == GRAPH_EMPTY)
	{
		// std::cout << "Empty graph" << '\n';
		//GetParentsEmpty(noOfParents, parents_Pg);
		return GetFirstFreqParentOfEmptyGraph(noOfGraphDB, graphDB, knownGraphsMap, minSup, ansGraphs, toBeAdded);
	}

	if(type_gt == GRAPH_SINGLE_NODE)
	{
		// std::cout << "Single node graph" << '\n';
		return GetFirstFreqParentOfSingleNodeGraph(noOfGraphDB, graphDB, knownGraphsMap, minSup, ansGraphs, toBeAdded);
	}

	// collect all the edges that can be restored
	//std::cout << "Entered here1" << '\n';
	set<int> edgeSet;
	for(int i=0; i < hiddenEdges_Ve.size(); i++)
	{
		if( BIsValidNode(hiddenEdges_Ve[i].first.first) || BIsValidNode(hiddenEdges_Ve[i].first.second) )
		{
			Graph g;
			g.Copy(this);
			HiddenEdge restore_e = g.EGetHiddenEdge(i);
			g.RestoreEdge(restore_e);
			g.SetEdgeAddedOrRemoved(restore_e.first.first, restore_e.first.second);
			// set label frequency
			if(!BIsValidNode(hiddenEdges_Ve[i].first.first))
				g.IncrementLabelFreq(hiddenEdges_Ve[i].first.first);
			if(!BIsValidNode(hiddenEdges_Ve[i].first.second )) // else if???
				g.IncrementLabelFreq(hiddenEdges_Ve[i].first.second);
			// check for frequency
			int r = g.IGetNoOfAllEdges();
			int c = g.IGetNoOfValidNodes();
			// first find in the knownFreqgraphs
			// std::cout << "/* message */" << '\n';
			// std::cout << knownFreqGraphs[r][c].size() << '\n';
			bool flag=false;

			if(findIfExist(knownGraphsMap, r, c))
			{
				for(int j=0; j < knownGraphsMap[r][c]->knownFreqGraphs.size(); j++)
				{
					//std::cout << j << '\n';
					if(g.BIsExactlyEqualTo(knownGraphsMap[r][c]->knownFreqGraphs[j])) // it is frequent
					{
						// so add it to answer and exit. return 1
						Graph *temp_g = new Graph();
						temp_g->Copy(&g);
						(*ansGraphs).clear();
						(*ansGraphs).push_back(temp_g);
						(*toBeAdded).clear();
						(*toBeAdded).push_back(j);
						//temp_g->~Graph();
						// std::cout << "Exiting here" << '\n';
						return 1;
					}
				}
				//std::cout << "And hereee" << '\n';
				// bool flag=false;
				// if you are here its not there in frequent graphs so find in
				// infrquent ones
				// std::cout << "/* message */" << '\n';
				// std::cout << knownInfreqGraphs[r][c].size() << '\n';
				for(int j=0; j < knownGraphsMap[r][c]->knownInfreqGraphs.size(); j++)
				{
					if(g.BIsExactlyEqualTo(knownGraphsMap[r][c]->knownInfreqGraphs[j])) // it is infrequent
					{
						// so add it to answer
						Graph *temp_g = new Graph();
						temp_g->Copy(&g);
						(*ansGraphs).push_back(temp_g);
						(*toBeAdded).push_back(j);
						flag = true;
						//temp_g->~Graph();
						break;
					}
				}
			}
			//std::cout << "hereee" << flag<< '\n';
			// g.Print();
			if(!flag) // still nothing is found
			{

				int freq_i=0, lostfreq_i=0;
				// find frequency
				for(int k=0; k < noOfGraphDB; k++)
				{
					//std::cout << k << '\n';
					if(g.IGetGraphID() == graphDB[k].IGetGraphID())
						freq_i++;
					else if(g.BIsSubGraphOf(&(graphDB[k])))
					{
						//std::cout << k << '\n';
						freq_i++;
					}
					else
						lostfreq_i++;

					if(freq_i >= minSup)
						break;
					if(lostfreq_i > noOfGraphDB-minSup)
					{
						freq_i = minSup-1;
						break;
					}
				}
				// std::cout << "Hiiiiiii123" << '\n';

				if(freq_i >= minSup) // frequent
				{
					// so add it to answer and exit. return 1
					Graph *temp_g = new Graph();
					temp_g->Copy(&g);
					(*ansGraphs).clear();
					(*ansGraphs).push_back(temp_g);
					(*toBeAdded).clear();
					(*toBeAdded).push_back(-1);
					//temp_g->~Graph();
					// std::cout << "Exiting here" << '\n';
					return 1;
				}
				else // infrequent
				{
					Graph *temp_g = new Graph();
					temp_g->Copy(&g);
					(*ansGraphs).push_back(temp_g);
					(*toBeAdded).push_back(-1);
					//temp_g->~Graph();
				}
			}
		}

	}
	// std::cout << "Exit" << '\n';
	return -1;
}

int Graph::GetFirstFreqParentWithEmbeddings(int noOfGraphDB, Graph *graphDB,
		vector < vector < vector<Graph*> > > &knownFreqGraphs,
		vector < vector < vector<Graph*> > > &knownInfreqGraphs,
		vector < vector < vector<int> > > &knownFreqGraphs_FreqVal,
		vector < vector < vector<Embedding> > > &knownFreqGraphEmbeddings,
		vector < vector < vector<Embedding> > > &knownInFreqGraphEmbeddings,
		int minSup, vector <Graph*> *ansGraphs, vector<int> *toBeAdded, Embedding f_embedding, bool shouldCount)
{

	// Handle empty and single node graphs as special case
	GraphType type_gt = GetGraphType();

	if(type_gt == GRAPH_EMPTY)
	{
		std::cerr << "Empty graph shouldn't come here" << '\n';
		return -100;
		//GetParentsEmpty(noOfParents, parents_Pg);
		//return GetFirstFreqParentOfEmptyGraph(noOfGraphDB, graphDB, knownFreqGraphs, knownInfreqGraphs, minSup, ansGraphs, toBeAdded);
	}

	if(type_gt == GRAPH_SINGLE_NODE)
	{
		// std::cout << "Single node graph" << '\n';
		return GetFirstFreqParentOfSingleNodeGraphWithEmbeddings(noOfGraphDB, graphDB, knownFreqGraphs, knownInfreqGraphs, knownFreqGraphs_FreqVal, knownFreqGraphEmbeddings, knownInFreqGraphEmbeddings, minSup, ansGraphs, toBeAdded, shouldCount);
	}

	// collect all the edges that can be restored
	//std::cout << "Entered here1" << '\n';
	set<int> edgeSet;
	for(int j=0; j < hiddenEdges_Ve.size(); j++)
	{
		//std::cout << i << '\n';
		if( BIsValidNode(hiddenEdges_Ve[j].first.first) || BIsValidNode(hiddenEdges_Ve[j].first.second) )
		{
			//std::cout << "here too" << '\n';
			// form parent with EGetHiddenEdge(i)
			//edgeSet.insert(i);
			Graph g;
			g.Copy(this);
			HiddenEdge restore_e = g.EGetHiddenEdge(j);
			g.RestoreEdge(restore_e);
			g.SetEdgeAddedOrRemoved(restore_e.first.first, restore_e.first.second);
			// set label frequency
			if(!BIsValidNode(hiddenEdges_Ve[j].first.first))
				g.IncrementLabelFreq(hiddenEdges_Ve[j].first.first);
			if(!BIsValidNode(hiddenEdges_Ve[j].first.second )) // else if???
				g.IncrementLabelFreq(hiddenEdges_Ve[j].first.second);
			// check for frequency
			int r = g.IGetNoOfAllEdges();
			int c = g.IGetNoOfValidNodes();
			Triplet trip;
			Embedding g_embedding;
			bool ans,flag;
			flag = false;
			ans = false;
			for(int i=0; i < knownFreqGraphs[r][c].size(); i++)
			{
				//std::cout << "IsThisGraphFrequentAndFound" << g->IGetGraphID() << knownFreqGraphs[r][c][i]->IGetGraphID() << '\n';
				if(g.BIsExactlyEqualTo(knownFreqGraphs[r][c][i]))
				{
					trip.SetValues(r,c,i);
					ans = true;
					flag = true;
					g_embedding = knownFreqGraphEmbeddings[r][c][i];
				}
			}
			//std::cout << "flagg " << flag << '\n';
			if(!flag)
			{
				for(int i=0; i < knownInfreqGraphs[r][c].size(); i++)
				{
					if(g.BIsExactlyEqualTo(knownInfreqGraphs[r][c][i]))
					{
						trip.SetValues(r,c,i);
						ans =false;
						flag = true;
						g_embedding = knownInFreqGraphEmbeddings[r][c][i];
					}
				}
			}
			//std::cout << "flagg2 " << flag << '\n';
			if(!flag)
			{
				int freq_i=0,lostfreq_i=0;
				if(g.GetGraphType() == GRAPH_EMPTY)
					freq_i = minSup;
				else
				{
					int sourceID, targetID;
					g.StoreEdgeAddedOrRemoved(&sourceID, &targetID);
					Edge edge = g.EGetEdgeUndirected(sourceID, targetID);
					int edge_label = g.IGetEdgeLabel(edge);
					map<int, std::pair<map<int, int>, map<int, int> > > m1;
					std::pair<map<int, int>, map<int, int> > m2;
					m1 = f_embedding.second;
					map<int, std::pair<map<int, int>, map<int, int> > >::iterator it;
					map<int, int>::iterator it2;
					vector<int> checked_graphs;
					for(int i=0;i<noOfGraphDB;i++)
						checked_graphs.push_back(0);
					int val1, val2;
					//std::cout << sourceID << targetID << '\n';
					for(it=m1.begin(); it!=m1.end();it++)
					{
						//std::cout << "heeeeeere" << '\n';
						int id = it->first;
						val1 = 0;
						val2 = 0;
						//std::cout << id << '\n';
						if(it->second.first.find(sourceID) != it->second.first.end())
							val1=1;
						if(it->second.first.find(targetID) != it->second.first.end())
							val2=1;
						// std::cout << val1 << val2 << '\n';
						if(val1 == 1 && val2 == 1)
						{
								int super_SID = it->second.first[sourceID];
								int super_TID = it->second.first[targetID];
								//std::cout << super_SID << "   " << super_TID << "   "<< edge_label <<'\n';
								bool isthere = graphDB[id].BIsThisEdgeExisting(super_SID, super_TID, edge_label);

								if(isthere)
								{
									g_embedding.second[id] = it->second;
									g_embedding.first = id;
									checked_graphs[id] = 1;
									freq_i += 1;
								}
						}
						else if(val1 == 1)
						{
							int super_SID = it->second.first[sourceID];
							Node super_SNode = graphDB[id].NGetNodeWithID(super_SID);
							int v_label = g.IGetNodeLabel(targetID);
							vector<Node> possibilities = graphDB[id].VNGetNodesAdjToWithLabel(super_SNode, v_label, edge_label);
							if(possibilities.size() > 0)
							{
								Node super_tNode = possibilities[0];
								int super_tID = graphDB[id].IGetNodeID(super_tNode);
								m2 = it->second;
								m2.first[targetID] = super_tID;
								m2.second[super_tID] = targetID;
								g_embedding.second[id] = m2;
								g_embedding.first = id;
								checked_graphs[id] = 1;
								freq_i +=1 ;
							}
						}
						else if(val2 == 1)
						{
							int super_TID = it->second.first[targetID];
							Node super_TNode = graphDB[id].NGetNodeWithID(super_TID);
							int v_label = g.IGetNodeLabel(sourceID);
							//std::cout << super_TID << v_label << edge_label << '\n';
							vector<Node> possibilities = graphDB[id].VNGetNodesAdjToWithLabel(super_TNode, v_label, edge_label);
							if(possibilities.size() > 0)
							{
								Node super_sNode = possibilities[0];
								int super_sID = graphDB[id].IGetNodeID(super_sNode);
								m2 = it->second;
								m2.first[sourceID] = super_sID;
								m2.second[super_sID] = sourceID;
								g_embedding.second[id] = m2;
								g_embedding.first = id;
								checked_graphs[id] = 1;
								freq_i +=1 ;
							}
						}
						else
						{
							std::cout << "Wrong entry !! Shouldn't come here" << '\n';
						}
						//std::cout << freq_i << '\n';
						if(freq_i >= minSup)
							break;
					}
					if(freq_i >= minSup)
					{
						Graph *temp_g = new Graph();
						temp_g->Copy(&g);
						trip.SetValues(r,c,knownFreqGraphs[r][c].size());
						//std::cout << "/* message33 */" << r << c << '\n';
						knownFreqGraphs[r][c].push_back(temp_g);
						if(shouldCount)
							knownFreqGraphs_FreqVal[r][c].push_back(freq_i);
						knownFreqGraphEmbeddings[r][c].push_back(g_embedding);
						ans = true;
						flag = true;
					}
					if(!flag){
					for(int i=0;i<noOfGraphDB;i++)
					{
						//std::cout << "Came hereeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee" << '\n';
						if(!checked_graphs[i])
						{
							pair<map<int, int>, map<int, int> > embedding;
							embedding = g.get_one_embedding(&(graphDB[i]));
							if(embedding.first.size() != 0)
							{
								freq_i++;
								g_embedding.second[i] = embedding;
								g_embedding.first = i;
							}
							else
								lostfreq_i++;
							if(shouldCount == false)
							{
									if(freq_i >= minSup)
										break;
									if(lostfreq_i > noOfGraphDB-minSup)
									{
										freq_i = minSup-1;
										break;
									}
							}
							checked_graphs[i] = 1;
						}
					}
				if(freq_i >= minSup)
				{
					Graph *temp_g = new Graph();
					temp_g->Copy(&g);
					trip.SetValues(r,c,knownFreqGraphs[r][c].size());
					// std::cout << "/* message33 */" << '\n';
					knownFreqGraphs[r][c].push_back(temp_g);
					if(shouldCount)
						knownFreqGraphs_FreqVal[r][c].push_back(freq_i);
					knownFreqGraphEmbeddings[r][c].push_back(g_embedding);
					ans = true;
					flag = true;
				}
				else
				{
					Graph *temp_g = new Graph();
					temp_g->Copy(&g);
					trip.SetValues( r,c,knownInfreqGraphs[r][c].size());
					knownInfreqGraphs[r][c].push_back(temp_g);
					knownInFreqGraphEmbeddings[r][c].push_back(g_embedding);
					ans = false;
					flag = true;
				}
			}
		}
			}

			//bool ans = FindFrequencyWithEmbeddings(knownFreqGraphs, knownInfreqGraphs, knownFreqGraphs_FreqVal, knownFreqGraphEmbeddings, knownInFreqGraphEmbeddings, this, &g, &trip, f_embedding, g_embedding);
			if(ans)
			{
				//std::cout << "Herrre true" << '\n';
				Graph *temp_g = new Graph();
				temp_g->Copy(&g);
				(*ansGraphs).clear();
				(*ansGraphs).push_back(temp_g);
				(*toBeAdded).clear();
				(*toBeAdded).push_back(trip.GetThird());
				// (*parentsOfFiEmbeddings).clear();
				// (*parentsOfFiEmbeddings).push_back(g_embedding);
				//std::cout << knownFreqGraphs[3][4].size() << '\n';
				return 1;
			}
			else
			{
				Graph *temp_g = new Graph();
				temp_g->Copy(&g);
				(*ansGraphs).push_back(temp_g);
				(*toBeAdded).push_back(trip.GetThird());
				//(*parentsOfFiEmbeddings).push_back(g_embedding);
			}
			// first find in the knownFreqgraphs
		}

	}
	//std::cout << "Exit" << '\n';
	return -1;
}

int Graph::GetFirstFreqParentOfSingleNodeGraph(int noOfGraphDB, Graph *graphDB,
		map<int, map<int, knownGraphNode*> > &knownGraphsMap,
		int minSup, vector <Graph*> *ansGraphs, vector<int> *toBeAdded)
{
	// make sure that this method is called only on single node graph
	//assert(GetGraphType() == GRAPH_SINGLE_NODE);

	int singleNode_i = IGetTheSingleNode();
	// std::cout << "Single Node ID " << singleNode_i << '\n';
	map<int, bool>::iterator begin_mIt = nodeHiddenEdges_Vm[singleNode_i].begin();
	map<int, bool>::iterator end_mIt = nodeHiddenEdges_Vm[singleNode_i].end();

	while(begin_mIt != end_mIt)
	{
		Graph g;
		g.Copy(this);
		HiddenEdge restore_e = g.EGetHiddenEdge((*begin_mIt).first);
		// std::cout << "Hidden Edge " << restore_e.first.first << restore_e.first.second <<'\n';
		string s="";
		g.SetGraphCode(s);
		g.RestoreEdge(restore_e);

		g.SetEdgeAddedOrRemoved(restore_e.first.first, restore_e.first.second);
		if(restore_e.first.first != singleNode_i)
			g.IncrementLabelFreq(restore_e.first.first); // set the label frequency
		else
			g.IncrementLabelFreq(restore_e.first.second); // set the label frequency

		// check for frequency and do the appropriate thing
		int r = g.IGetNoOfAllEdges();
		int c = g.IGetNoOfValidNodes();
		// first find in the knownFreqgraphs
		bool flag=false;

		if(findIfExist(knownGraphsMap, r, c))
		{
			for(int j=0; j < knownGraphsMap[r][c]->knownFreqGraphs.size(); j++)
				if(g.BIsExactlyEqualTo(knownGraphsMap[r][c]->knownFreqGraphs[j])) // it is frequent
				{
					// so add it to answer and exit. return 1
					Graph *temp_g = new Graph();
					temp_g->Copy(&g);
					(*ansGraphs).clear();
					(*ansGraphs).push_back(temp_g);
					(*toBeAdded).clear();
					(*toBeAdded).push_back(j);
					return 1;
				}

			// bool flag=false;
			// if you are here its not there in frequent graphs so find in
			// infrquent ones
			for(int j=0; j < knownGraphsMap[r][c]->knownInfreqGraphs.size(); j++)
				if(g.BIsExactlyEqualTo(knownGraphsMap[r][c]->knownInfreqGraphs[j])) // it is infrequent
				{
					// so add it to answer
					Graph *temp_g = new Graph();
					temp_g->Copy(&g);
					(*ansGraphs).push_back(temp_g);
					(*toBeAdded).push_back(j);
					flag = true;
					break;
				}
		}

		if(!flag) // still nothing is found
		{
			int freq_i=0, lostfreq_i=0;
			// find frequency
			for(int i=0; i < noOfGraphDB; i++)
			{
				if(g.IGetGraphID() == graphDB[i].IGetGraphID())
					freq_i++;
				else if(g.BIsSubGraphOf(&(graphDB[i])))
					freq_i++;
				else
					lostfreq_i++;

				if(freq_i >= minSup)
					break;
				if(lostfreq_i > noOfGraphDB-minSup)
				{
					freq_i = minSup-1;
					break;
				}
			}

			if(freq_i >= minSup) // frequent
			{
				// so add it to answer and exit. return 1
				Graph *temp_g = new Graph();
				temp_g->Copy(&g);
				(*ansGraphs).clear();
				(*ansGraphs).push_back(temp_g);
				(*toBeAdded).clear();
				(*toBeAdded).push_back(-1);
				return 1;
			}
			else // infrequent
			{
				Graph *temp_g = new Graph();
				temp_g->Copy(&g);
				(*ansGraphs).push_back(temp_g);
				(*toBeAdded).push_back(-1);
			}
		}
		begin_mIt++;
	}

	return -1;
	//assert(count_i == (*noOfParents)); // make sure that we have all the parents
}

int Graph::GetFirstFreqParentOfSingleNodeGraphWithEmbeddings(int noOfGraphDB, Graph *graphDB,
		vector < vector < vector<Graph*> > > &knownFreqGraphs,
		vector < vector < vector<Graph*> > > &knownInfreqGraphs,
		vector < vector < vector<int> > > &knownFreqGraphs_FreqVal,
		vector < vector < vector<Embedding> > > &knownFreqGraphEmbeddings,
		vector < vector < vector<Embedding> > > &knownInFreqGraphEmbeddings,
		int minSup, vector <Graph*> *ansGraphs, vector<int> *toBeAdded, bool shouldCount)
{
	// make sure that this method is called only on single node graph
	//assert(GetGraphType() == GRAPH_SINGLE_NODE);

	int singleNode_i = IGetTheSingleNode();
	// std::cout << "Single Node ID " << singleNode_i << '\n';
	map<int, bool>::iterator begin_mIt = nodeHiddenEdges_Vm[singleNode_i].begin();
	map<int, bool>::iterator end_mIt = nodeHiddenEdges_Vm[singleNode_i].end();

	while(begin_mIt != end_mIt)
	{
		Graph g;
		g.Copy(this);
		HiddenEdge restore_e = g.EGetHiddenEdge((*begin_mIt).first);
		// std::cout << "Hidden Edge " << restore_e.first.first << restore_e.first.second <<'\n';
		string s="";
		g.SetGraphCode(s);
		g.RestoreEdge(restore_e);

		g.SetEdgeAddedOrRemoved(restore_e.first.first, restore_e.first.second);
		if(restore_e.first.first != singleNode_i)
			g.IncrementLabelFreq(restore_e.first.first); // set the label frequency
		else
			g.IncrementLabelFreq(restore_e.first.second); // set the label frequency

		// check for frequency and do the appropriate thing
		int r = g.IGetNoOfAllEdges();
		int c = g.IGetNoOfValidNodes();
		// first find in the knownFreqgraphs
		for(int j=0; j < knownFreqGraphs[r][c].size(); j++)
			if(g.BIsExactlyEqualTo(knownFreqGraphs[r][c][j])) // it is frequent
			{
				// so add it to answer and exit. return 1
				Graph *temp_g = new Graph();
				temp_g->Copy(&g);
				(*ansGraphs).clear();
				(*ansGraphs).push_back(temp_g);
				(*toBeAdded).clear();
				(*toBeAdded).push_back(j);
				// (*parentsOfFiEmbeddings).clear();
				// (*parentsOfFiEmbeddings).push_back(knownFreqGraphEmbeddings[r][c][j]);
				return 1;
			}

		bool flag=false;
		// if you are here its not there in frequent graphs so find in
		// infrquent ones
		for(int j=0; j < knownInfreqGraphs[r][c].size(); j++)
			if(g.BIsExactlyEqualTo(knownInfreqGraphs[r][c][j])) // it is infrequent
			{
				// so add it to answer
				Graph *temp_g = new Graph();
				temp_g->Copy(&g);
				(*ansGraphs).push_back(temp_g);
				(*toBeAdded).push_back(j);
				//(*parentsOfFiEmbeddings).push_back(knownInFreqGraphEmbeddings[r][c][j]);
				flag = true;
				break;
			}

		if(!flag) // still nothing is found
		{
			Embedding g_embedding;
			int freq_i=0, lostfreq_i=0;
			// find frequency
			for(int i=0; i < noOfGraphDB; i++)
			{
				if(g.IGetGraphID() == graphDB[i].IGetGraphID())
					freq_i++;
				else if(g.BIsSubGraphOf(&(graphDB[i])))
				{
					freq_i++;
					g_embedding.first = i;
					pair<map<int, int>, map<int, int> > embedding1;
					embedding1 = g.get_one_embedding(&graphDB[i]);
					if(embedding1.first.size() > 0)
						g_embedding.second[i] = embedding1;
				}
				else
					lostfreq_i++;

				if(freq_i >= minSup)
					break;
				if(lostfreq_i > noOfGraphDB-minSup)
				{
					freq_i = minSup-1;
					break;
				}
			}

			if(freq_i >= minSup) // frequent
			{
				// so add it to answer and exit. return 1
				Graph *temp_g = new Graph();
				temp_g->Copy(&g);
				(*ansGraphs).clear();
				(*ansGraphs).push_back(temp_g);
				(*toBeAdded).clear();
				(*toBeAdded).push_back(knownFreqGraphs[r][c].size());
				knownFreqGraphs[r][c].push_back(temp_g);
				knownFreqGraphEmbeddings[r][c].push_back(g_embedding);
				if(shouldCount)
					knownFreqGraphs_FreqVal[r][c].push_back(freq_i);
				// (*parentsOfFiEmbeddings).clear();
				// (*parentsOfFiEmbeddings).push_back(g_embedding);
				return 1;
			}
			else // infrequent
			{
				Graph *temp_g = new Graph();
				temp_g->Copy(&g);
				(*ansGraphs).push_back(temp_g);
				(*toBeAdded).push_back(knownInfreqGraphs[r][c].size());
				knownInfreqGraphs[r][c].push_back(temp_g);
				knownInFreqGraphEmbeddings[r][c].push_back(g_embedding);
				//(*parentsOfFiEmbeddings).push_back(g_embedding);
			}
		}
		begin_mIt++;
	}

	return -1;
	//assert(count_i == (*noOfParents)); // make sure that we have all the parents
}

int Graph::GetFirstFreqParentOfEmptyGraph(int noOfGraphDB, Graph *graphDB,
		map<int, map<int, knownGraphNode*> > &knownGraphsMap,
		int minSup, vector <Graph*> *ansGraphs, vector<int> *toBeAdded)
{
	// make sure that this method is called only on empty graph
	//assert( GetGraphType() == GRAPH_EMPTY);

	for(int i=0; i < num_vertices(G); i++)
	{
		Graph *g;
		g->Copy(this);
		g->SetAsValidNode(i); // set as valid node; label freq will be taken care by it
		string s = GetAsString(i);
		g->SetGraphCode(s);
		g->SetNodeAddedOrRemoved(g->NGetNodeWithID(i));
		g->SetEdgeAddedOrRemoved(g->NGetNodeWithID(i), -1);
		// check for frequency and do the appropriate thing
		int r = g->IGetNoOfAllNodes();
		int c = g->IGetNoOfValidNodes();
		// first find in the knownFreqgraphs
		bool flag = false;

		if(findIfExist(knownGraphsMap, r, c))
		{
			for(int j=0; j < knownGraphsMap[r][c]->knownFreqGraphs.size(); j++)
				if(g->BIsExactlyEqualTo(knownGraphsMap[r][c]->knownFreqGraphs[j])) // it is frequent
				{
					// so add it to answer and exit. return 1
					(*ansGraphs).clear();
					(*ansGraphs).push_back(g);
					(*toBeAdded).clear();
					(*toBeAdded).push_back(j);
					return 1;
				}

			// bool flag=false;
			// if you are here its not there in frequent graphs so find in
			// infrquent ones
			for(int j=0; j < knownGraphsMap[r][c]->knownInfreqGraphs.size(); j++)
				if(g->BIsExactlyEqualTo(knownGraphsMap[r][c]->knownInfreqGraphs[j])) // it is infrequent
				{
					// so add it to answer
					(*ansGraphs).push_back(g);
					(*toBeAdded).push_back(j);
					flag = true;
					break;
				}
		}

		if(!flag) // still nothing is found
		{
			int freq_i=0, lostfreq_i=0;
			// find frequency
			for(int k=0; k < noOfGraphDB; k++)
			{
				if(g->IGetGraphID() == graphDB[k].IGetGraphID())
					freq_i++;
				else if(g->BIsSubGraphOf(&(graphDB[k])))
					freq_i++;
				else
					lostfreq_i++;

				if(freq_i >= minSup)
					break;
				if(lostfreq_i > noOfGraphDB-minSup)
				{
					freq_i = minSup-1;
					break;
				}
			}

			if(freq_i >= minSup) // frequent
			{
				// so add it to answer and exit. return 1
				(*ansGraphs).clear();
				(*ansGraphs).push_back(g);
				(*toBeAdded).clear();
				(*toBeAdded).push_back(-1);
				return 1;
			}
			else // infrequent
			{
				(*ansGraphs).push_back(g);
				(*toBeAdded).push_back(-1);
			}
		}
	}

	return -1;
}


vector<Graph*> Graph::GetParents(int *noOfParents)
{
	// Handle empty and single node graphs as special case
	GraphType type_gt = GetGraphType();

	if(type_gt == GRAPH_EMPTY)
	{
		return GetParentsOfEmptyGraph(noOfParents);
	}

	if(type_gt == GRAPH_SINGLE_NODE)
	{
		return GetParentsOfSingleNodeGraph(noOfParents);
	}

	// collect all the edges that can be restored
	set<int> edgeSet;
	for(int i=0; i < hiddenEdges_Ve.size(); i++)
	{
		// std::cout << "Hidden edges " << hiddenEdges_Ve[i].first.first << hiddenEdges_Ve[i].first.second << '\n';
		if( BIsValidNode(hiddenEdges_Ve[i].first.first) || BIsValidNode(hiddenEdges_Ve[i].first.second ))
			edgeSet.insert(i);
	}

	(*noOfParents) = edgeSet.size();
	vector<Graph*> parents_Pg;
	for(int i=0;i<(*noOfParents);i++)
	{
		Graph* g = new Graph();
		parents_Pg.push_back(g);
	}
	set<int>::iterator beginIt = edgeSet.begin();
	set<int>::iterator endIt = edgeSet.end();
	int count_i = 0;
	while(beginIt != endIt)
	{
		parents_Pg[count_i]->Copy(this); // copy the current graph
		HiddenEdge restore_e = parents_Pg[count_i]->EGetHiddenEdge((*beginIt));
		parents_Pg[count_i]->RestoreEdge(restore_e);
		parents_Pg[count_i]->SetEdgeAddedOrRemoved(restore_e.first.first, restore_e.first.second);
		// set the label frequency
		if( !BIsValidNode(hiddenEdges_Ve[(*beginIt)].first.first) )
			parents_Pg[count_i]->IncrementLabelFreq(hiddenEdges_Ve[(*beginIt)].first.first);
		if( !BIsValidNode(hiddenEdges_Ve[(*beginIt)]. first.second)) // else if???
			parents_Pg[count_i]->IncrementLabelFreq(hiddenEdges_Ve[(*beginIt)].first.second);

		count_i++;
		beginIt++;
	}
	return parents_Pg;
}

void Graph::GetOnlyOneParent(Graph *parent_Pg)
{
	/*
	 * This method can be called only if the graph is
	 * not the main graph itself
	 * */
	//assert(this->BItIsNotG_i());

	GraphType type_gt = GetGraphType();
	if(type_gt == GRAPH_EMPTY)
	{
		int noOfParents;
		vector<Graph*> parents_Pg;
		parents_Pg = this->GetParentsOfEmptyGraph(&noOfParents);
		//assert(noOfParents >= 1);
		(*parent_Pg).Copy(parents_Pg[0]);
		return;
	}
	if( type_gt == GRAPH_SINGLE_NODE)
	{
		// Single Node Graph
		int noOfParents;
		vector<Graph*> parents_Pg;
		parents_Pg = this->GetParentsOfSingleNodeGraph(&noOfParents);
		//assert(noOfParents >= 1);
		(*parent_Pg).Copy(parents_Pg[0]);
		return;
	}

	// Make sure that there are edges that can be restored
	//      which makes sure that the this method is not called on the main
	//      graph itself
	//assert( (hiddenEdges_Ve.size() > 0));

	// collect all the edges that can be restored
	for(int i=0; i < hiddenEdges_Ve.size(); i++)
	{
		if( BIsValidNode(hiddenEdges_Ve[i].first.first) || BIsValidNode(hiddenEdges_Ve[i].first.second) )
		{
			// if this edge can be restored, form the parent and return
			(*parent_Pg).Copy(this); // copy the current graph
			HiddenEdge restore_e = (*parent_Pg).EGetHiddenEdge(i);
			(*parent_Pg).RestoreEdge(restore_e);
			(*parent_Pg).SetEdgeAddedOrRemoved(restore_e.first.first, restore_e.first.second);
			// set the label frequency
			if( !BIsValidNode(hiddenEdges_Ve[i].first.first) )
				(*parent_Pg).IncrementLabelFreq(hiddenEdges_Ve[i].first.first);
			else if( !BIsValidNode(hiddenEdges_Ve[i].first.second) ) // else if??
				(*parent_Pg).IncrementLabelFreq(hiddenEdges_Ve[i].first.second);

			return;
		}
	}

	ErrorExit("Graph::GetOnlyOneParent", "Unable to form the parent");
}

vector<Graph*> Graph::GetParentsOfSingleNodeGraph(int *noOfParents)
{
	// make sure that this method is called only on single node graph
	//assert(GetGraphType() == GRAPH_SINGLE_NODE);

	int singleNode_i = IGetTheSingleNode();
	(*noOfParents) = nodeHiddenEdges_Vm[singleNode_i].size();
	vector<Graph*> parents_Pg;
	for(int i=0;i<(*noOfParents);i++)
	{
		Graph* g = new Graph();
		parents_Pg.push_back(g);
	}
	map<int, bool>::iterator begin_mIt = nodeHiddenEdges_Vm[singleNode_i].begin();
	map<int, bool>::iterator end_mIt = nodeHiddenEdges_Vm[singleNode_i].end();
	int count_i=0;
	while(begin_mIt != end_mIt)
	{
		parents_Pg[count_i]->Copy(this); // first copy the current graph which is single node
		HiddenEdge restore_e = parents_Pg[count_i]->EGetHiddenEdge((*begin_mIt).first);

		string s="";
		parents_Pg[count_i]->SetGraphCode(s);
		parents_Pg[count_i]->RestoreEdge(restore_e);

		parents_Pg[count_i]->SetEdgeAddedOrRemoved(restore_e.first.first, restore_e.first.second);
		if(restore_e.first.first != singleNode_i)
			parents_Pg[count_i]->IncrementLabelFreq(restore_e.first.first); // set the label frequency
		else
			parents_Pg[count_i]->IncrementLabelFreq(restore_e.first.second); // set the label frequency

		count_i++;
		begin_mIt++;
	}
	return parents_Pg;

	//assert(count_i == (*noOfParents)); // make sure that we have all the parents
}

vector<Graph*> Graph::GetParentsOfEmptyGraph(int *noOfParents)
{
	// make sure that this method is called only on empty graph
	//assert( GetGraphType() == GRAPH_EMPTY);

	(*noOfParents) = num_vertices(G);
	vector<Graph*> parents_Pg;
	for(int i=0;i<(*noOfParents);i++)
	{
		Graph *g = new Graph();
		parents_Pg.push_back(g);
	}
	for(int i=0; i < (*noOfParents); i++)
	{
		parents_Pg[i]->Copy(this); // first copy the current graph which is empty
		parents_Pg[i]->SetAsValidNode(i); // set as valid node; label freq will be taken care by it
		string s = GetAsString(i);
		parents_Pg[i]->SetGraphCode(s);
		parents_Pg[i]->SetNodeAddedOrRemoved(parents_Pg[i]->NGetNodeWithID(i));
		parents_Pg[i]->SetEdgeAddedOrRemoved(parents_Pg[i]->NGetNodeWithID(i), -1);
	}
	return parents_Pg;
}

// extra thing to do: when you find something is infrequent in this process
// store them in inFreqFound and later add them to knownInfreqGraphs
bool Graph::GetFirstFreqChild(int noOfGraphDB, Graph *graphDB,
		vector < vector < vector<Graph*> > > knownFreqGraphs,
		vector < vector < vector<Graph*> > > knownInfreqGraphs,
		int minSup, Graph **ansGraph, int *toBeAdded, vector<Graph*> *inFreqFound)
{
	GraphType type_gt = GetGraphType();
	if(type_gt == GRAPH_EMPTY)
	{
		return false;
	}
	else if(type_gt == GRAPH_SINGLE_NODE)
	{
		return GetFirstFreqChildOfSingleNodeGraph(noOfGraphDB, graphDB, knownFreqGraphs, knownInfreqGraphs, minSup, ansGraph, toBeAdded, inFreqFound);
	}
	else if(type_gt == GRAPH_SINGLE_EDGE)
	{
		return GetFirstFreqChildOfSingleEdgeGraph(noOfGraphDB, graphDB, knownFreqGraphs, knownInfreqGraphs, minSup, ansGraph, toBeAdded, inFreqFound);
	}

	// change the way you generate children
	vector<int> pendentNodes;
	multimap<int, int> list_e;
	vector<HiddenEdge> hideEdges;

	pair<vertex_iterator, vertex_iterator> v_iterator = boost::vertices(G);
	vertex_index v_index = boost::get(boost::vertex_index_t(), G);
	while(v_iterator.first != v_iterator.second)
	{
		if(boost::out_degree(*v_iterator.first, G) == 1)
			pendentNodes.push_back(v_index[*v_iterator.first]);
		++v_iterator.first;
	}

	Graph *tempGraph = new Graph();
	tempGraph->Copy(this); // we use this to form children from now

	for(int i=0; i < pendentNodes.size(); i++)
	{
		Node curNode = allNodes_Vn[pendentNodes[i]];
		if(boost::out_degree(curNode, G) != 1) // special case
			continue;
		Edge eTemp = GetTheOnlyIncidentEdge(curNode);

		int sID = IGetSourceNode(eTemp);
		int tID = IGetTargetNode(eTemp);

		Graph g;
		g.Copy(tempGraph);

		g.SetEdgeAddedOrRemoved(g.EGetEdge(sID, tID));

		g.HideEdge(g.EGetEdge(sID, tID));

		// set the label frequencies
		if(!g.BIsValidNode(sID))
			g.DecrementLabelFreq(sID);
		if(!g.BIsValidNode(tID)) // else if??
			g.DecrementLabelFreq(tID);

		int r = g.IGetNoOfAllEdges();
		int c = g.IGetNoOfValidNodes();
		// first find in the knownFreqgraphs
		for(int j=0; j < knownFreqGraphs[r][c].size(); j++)
			if(g.BIsExactlyEqualTo(knownFreqGraphs[r][c][j])) // it is frequent
			{
				(*ansGraph) = new Graph();
				(*ansGraph)->Copy(&g);
				(*toBeAdded) = j;
				delete tempGraph;
				//g.~Graph();
				return true;
			}

		bool flag=false;
		// if you are here its not there in frequent graphs so find in
		// infrquent ones
		for(int j=0; j < knownInfreqGraphs[r][c].size(); j++)
			if(g.BIsExactlyEqualTo(knownInfreqGraphs[r][c][j])) // it is infrequent
			{
				// do nothing::
				flag = true;
				break;
			}

		if(!flag) // still nothing is found
		{
			int freq_i=0, lostfreq_i=0;
			// find frequency
			for(int k=0; k < noOfGraphDB; k++)
			{
				if(g.IGetGraphID() == graphDB[k].IGetGraphID())
					freq_i++;
				else if(g.BIsSubGraphOf(&(graphDB[k])))
					freq_i++;
				else
					lostfreq_i++;

				if(freq_i >= minSup)
					break;
				if(lostfreq_i > noOfGraphDB-minSup)
				{
					freq_i = minSup-1;
					break;
				}
			}

			if(freq_i >= minSup) // frequent
			{
				(*ansGraph) = new Graph();
				(*ansGraph)->Copy(&g);
				(*toBeAdded) = -1;
				delete tempGraph;
				//g.~Graph();
				return true;
			}
			else // infrequent
			{
				// do nothing:: NO Change this. add this to inFreqFound
				Graph *temp_g = new Graph();
				temp_g->Copy(&g);
				(*inFreqFound).push_back(temp_g);
			}
		}
	}

	for(int i=0; i < pendentNodes.size(); i++)
	{
		Node curNode = allNodes_Vn[pendentNodes[i]];
		if(boost::out_degree(curNode, G) != 1) // special case
			continue;
		Edge eTemp = GetTheOnlyIncidentEdge(curNode);

		NodeID otherNodeID = IGetNodeID(GetOppositeEdge(eTemp, curNode));
		hideEdges.push_back(make_pair(make_pair(IGetSourceNode(eTemp), IGetTargetNode(eTemp)), IGetEdgeLabel(eTemp)));
		// hide_edge(eTemp);
		boost::remove_edge(eTemp, G);
		bool flag_b = true;
		while(flag_b)
		{
			Node tempNode = allNodes_Vn[otherNodeID];
			int degree = boost::out_degree(tempNode, G);
			if(degree==0) // special case
				flag_b = false;
			else if(degree > 1)
				flag_b = false;
			else // degree == 1
			{
				Edge nextEdge = GetTheOnlyIncidentEdge(tempNode);
				Node thirdNode = GetOppositeEdge(nextEdge, tempNode);
				hideEdges.push_back(make_pair(make_pair(IGetSourceNode(nextEdge), IGetTargetNode(nextEdge)),IGetEdgeLabel(nextEdge)));
				// hide_edge(nextEdge);
				boost::remove_edge(nextEdge, G);
				otherNodeID = IGetNodeID(thirdNode);
				// thirdNode.~Node();
				// nextEdge.~Edge();
			}
		}
		// curNode.~Node();
		// eTemp.~Edge();
	}
	edge_iterator ei, ei_end;
	bool exitFromWhile=false,ansFound=false;
	for (tie(ei, ei_end) = edges(G); ei != ei_end; ++ei)
	{
		int sID = IGetSourceNode(*ei);
		int tID = IGetTargetNode(*ei);

		if(tempGraph->BCanHideEdge(tempGraph->EGetEdge(sID,tID)))
		{
			Graph g;
			g.Copy(tempGraph);

			g.SetEdgeAddedOrRemoved(g.EGetEdge(sID, tID));

			g.HideEdge(g.EGetEdge(sID, tID));

			// set the label frequencies
			if(!g.BIsValidNode(sID))
				g.DecrementLabelFreq(sID);
			if(!g.BIsValidNode(tID)) // else if??
				g.DecrementLabelFreq(tID);

			int r = g.IGetNoOfAllEdges();
			int c = g.IGetNoOfValidNodes();
			// first find in the knownFreqgraphs
			for(int j=0; j < knownFreqGraphs[r][c].size(); j++)
				if(g.BIsExactlyEqualTo(knownFreqGraphs[r][c][j])) // it is frequent
				{
					(*ansGraph) = new Graph();
					(*ansGraph)->Copy(&g);
					(*toBeAdded) = j;
					//return true;
					exitFromWhile = true;
					ansFound = true;
					//g.~Graph();
					break; // come out of for loop
				}

			if(exitFromWhile)
				break; // come out of while loop

			bool flag=false;
			// if you are here its not there in frequent graphs so find in
			// infrquent ones
			for(int j=0; j < knownInfreqGraphs[r][c].size(); j++)
				if(g.BIsExactlyEqualTo(knownInfreqGraphs[r][c][j])) // it is infrequent
				{
					// do nothing
					flag = true;
					//g.~Graph();
					break;
				}

			if(!flag) // still nothing is found
			{
				int freq_i=0, lostfreq_i=0;
				// find frequency
				for(int k=0; k < noOfGraphDB; k++)
				{
					if(g.IGetGraphID() == graphDB[k].IGetGraphID())
						freq_i++;
					else if(g.BIsSubGraphOf(&(graphDB[k])))
						freq_i++;
					else
						lostfreq_i++;

					if(freq_i >= minSup)
						break;
					if(lostfreq_i > noOfGraphDB-minSup)
					{
						freq_i = minSup-1;
						break;
					}
				}

				if(freq_i >= minSup) // frequent
				{
					(*ansGraph) = new Graph();
					(*ansGraph)->Copy(&g);
					(*toBeAdded) = -1;
					exitFromWhile = true;
					ansFound = true;
					//return true;
				}
				else // infrequent
				{
					// do nothing:: NO Change this. add this to inFreqFound
					Graph *temp_g = new Graph();
					temp_g->Copy(&g);
					(*inFreqFound).push_back(temp_g);
				}
			}
			if(exitFromWhile)
			{
				//g.~Graph();
				break; // exit from while loop
			}
			//g.~Graph();
		}
	}

	delete tempGraph;
	for(int i=0; i<hideEdges.size(); i++)
		boost::add_edge(NGetNodeWithID(hideEdges[i].first.first), NGetNodeWithID(hideEdges[i].first.second), hideEdges[i].second, G);

	hideEdges.erase(hideEdges.begin(),hideEdges.end());
	if(ansFound)
		return true;

	return false;
}

void Graph::GetOnlyOneChild(Graph *child_Pg)
{
	// This function cannot be called on a empty graph
	GraphType type_gt = GetGraphType();
	//assert(type_gt != GRAPH_EMPTY); // check for empty graph

	if(type_gt == GRAPH_SINGLE_NODE)
	{
		int noOfChildren;
		vector<Graph*> children_Pg;
		children_Pg = GetChildrenOfSingleNodeGraph(&noOfChildren);
		//assert(noOfChildren==1);
		(*child_Pg).Copy(children_Pg[0]);
		return;
	}
	else if(type_gt == GRAPH_SINGLE_EDGE)
	{
		int noOfChildren;
		vector<Graph*> children_Pg;
		children_Pg = GetChildrenOfSingleEdgeGraph(&noOfChildren);
		//assert(noOfChildren==2);
		(*child_Pg).Copy(children_Pg[0]);
		return;
	}
	edge_iterator ei, ei_end;
	int cnt = 0;
	// std::cout << "After!!" << IGetNoOfAllEdges() << '\n';
	// Print();
	vector<Edge> edges;
	for (tie(ei, ei_end) = boost::edges(G); ei != ei_end; ++ei)
	{
		edges.push_back(*ei);
		cnt++;
	}
	cnt=0;

	for (int i=0;i<edges.size();i++)
	{
		Edge e = edges[i];
		cnt++;
		NodeID target_nID = IGetTargetNode(e);
		NodeID source_nID = IGetSourceNode(e);
		if(BCanHideEdge(e))
		{
			(*child_Pg).Copy(this);
			Edge hide_e = (*child_Pg).EGetEdge(source_nID, target_nID);
			(*child_Pg).SetEdgeAddedOrRemoved(hide_e);
			(*child_Pg).HideEdge(hide_e);
			if(!(*child_Pg).BIsValidNode(source_nID))
				(*child_Pg).DecrementLabelFreq(source_nID);
			if(!(*child_Pg).BIsValidNode(target_nID))
				(*child_Pg).DecrementLabelFreq(target_nID);
			return;
		}
	}
}

void Graph::experiment()
{
	edge_iterator ei, ei_end;
	for (tie(ei, ei_end) = edges(G); ei != ei_end; ++ei)
	{
		if(BCanHideEdge(*ei))
		{
			// Node source_nID = IGetSourceNode(*ei);
			// Node target_nID = IGetTargetNode(*ei);
			// std::cout << source_nID << ' ' << target_nID << '\n';
			// (*child_Pg).Copy(this);
			// std::cout << "/* message */" << '\n';
			Graph* child = new Graph();
			(*child).Copy(this);
			NodeID target_nID = IGetTargetNode(*ei);
			NodeID source_nID = IGetSourceNode(*ei);
			// std::cout << source_nID << ' ' << target_nID << '\n';
			// Edge hide_e = (*child_Pg).EGetEdge(source_nID, target_nID);
			// (*child_Pg).SetEdgeAddedOrRemoved(hide_e);
			// (*child_Pg).HideEdge(hide_e);
			//
			// // set the label frequencies
			// if(!(*child_Pg).BIsValidNode(source_nID))
			//     (*child_Pg).DecrementLabelFreq(source_nID);
			// if(!(*child_Pg).BIsValidNode(target_nID))
			//     (*child_Pg).DecrementLabelFreq(target_nID);
			// return;
		}
	}
}

vector<Graph*> Graph::GetChildren(int *noOfChildren)
{
	GraphType type_gt = GetGraphType();
	if(type_gt == GRAPH_EMPTY)
	{
		(*noOfChildren) = 0;
		vector<Graph*> children_Pg; // no children for empty graph
		return children_Pg;
	}
	else if(type_gt == GRAPH_SINGLE_NODE)
	{
		return GetChildrenOfSingleNodeGraph(noOfChildren);

	}
	else if(type_gt == GRAPH_SINGLE_EDGE)
	{
		return GetChildrenOfSingleEdgeGraph(noOfChildren);
	}


	// change the way you generate children
	vector<int> pendentNodes;
	//multimap<int, int> list_e;
	// Triplet -> first, second -> node ids, third whether it can be hidden
	// or not 1 -> can be hidden, -1 cannot be hidden
	vector<Triplet> list_e;
	vector<HiddenEdge> hideEdges;

	pair<vertex_iterator, vertex_iterator> v_iterator = boost::vertices(G);
	while(v_iterator.first != v_iterator.second)
	{
		if(boost::out_degree(*v_iterator.first, G) == 1)
			pendentNodes.push_back(IGetNodeID(*v_iterator.first));
		++v_iterator.first;
	}
	for(int i=0; i < pendentNodes.size(); i++)
	{
		Node curNode = allNodes_Vn[pendentNodes[i]];
		if(boost::out_degree(curNode, G) != 1) // special case
			continue;
		Edge eTemp = GetTheOnlyIncidentEdge(curNode);
    // PrintEdge(eTemp);
		Triplet tempTrip(IGetSourceNode(eTemp),IGetTargetNode(eTemp), 1);
		list_e.push_back(tempTrip);
		//list_e.insert(pair<int, int>(nodeIDs_nMnid[eTemp.source()],
		//nodeIDs_nMnid[eTemp.target()]));
	}
	for(int i=0; i < pendentNodes.size(); i++)
	{
		Node curNode = allNodes_Vn[pendentNodes[i]];
		if(boost::out_degree(curNode, G) != 1) // special case
			continue;
		Edge eTemp = GetTheOnlyIncidentEdge(curNode);

		NodeID otherNodeID = IGetNodeID(GetOppositeEdge(eTemp, curNode));
    // std::cout << curNode << otherNodeID << '\n';
		hideEdges.push_back(make_pair(make_pair(IGetSourceNode(eTemp), IGetTargetNode(eTemp)), IGetEdgeLabel(eTemp)));
		boost::remove_edge(eTemp, G);

		bool flag_b = true;
		while(flag_b)
		{
			Node tempNode = allNodes_Vn[otherNodeID];
			int degree = boost::out_degree(tempNode, G);
			if(degree==0) // special case
				flag_b = false;
			else if(degree > 1)
				flag_b = false;
			else // degree == 1
			{
				Edge nextEdge = GetTheOnlyIncidentEdge(tempNode);
				Node thirdNode = GetOppositeEdge(nextEdge, tempNode);
				hideEdges.push_back(make_pair(make_pair(IGetSourceNode(nextEdge), IGetTargetNode(nextEdge)), IGetEdgeLabel(nextEdge)));
				boost::remove_edge(nextEdge, G);
				otherNodeID = IGetNodeID(thirdNode);
				// thirdNode.~Node();
				// nextEdge.~Edge();
			}
		}
		// curNode.~Node();
		// eTemp.~Edge();
	}
  	edge_iterator ei, ei_end;
	for(boost::tie(ei, ei_end) = boost::edges(G); ei!=ei_end; ++ei)
	{
		Triplet tempTrip(IGetSourceNode(*ei),IGetTargetNode(*ei),-1);
		//list_e.insert(pair<int, int>(nodeIDs_nMnid[(*begin_eIt).source()],
		//nodeIDs_nMnid[(*begin_eIt).target()]));
		list_e.push_back(tempTrip);
	}

	int numChild=0;
	for(int i=0; i<list_e.size(); i++)
	{
		if(list_e[i].GetThird() == -1)
		{
			if(BCanHideEdge(EGetEdge(list_e[i].GetFirst(), list_e[i].GetSecond())))
			{
				list_e[i].SetThird(1);
				numChild++;
			}
		}
		else
			numChild++;
	}

	for(int i=0; i<hideEdges.size(); i++)
		boost::add_edge(NGetNodeWithID(hideEdges[i].first.first), NGetNodeWithID(hideEdges[i].first.second), hideEdges[i].second, G);

	hideEdges.erase(hideEdges.begin(),hideEdges.end());

	//(*noOfChildren) = list_e.size();
	(*noOfChildren) = numChild;
	vector<Graph*> children_Pg;
	for(int i=0;i<numChild;i++)
	{
		Graph* g = new Graph();
		children_Pg.push_back(g);
	}

	//multimap<int, int>::iterator begin_mIt = list_e.begin();
	//multimap<int, int>::iterator end_mIt = list_e.end();
	int count_i = 0;
	//while(begin_mIt != end_mIt)
	for(int i=0; i<list_e.size(); i++)
	{
		if(list_e[i].GetThird() == 1)
		{
			int first=list_e[i].GetFirst();
			int second=list_e[i].GetSecond();
			children_Pg[count_i]->Copy(this);

			children_Pg[count_i]->SetEdgeAddedOrRemoved(children_Pg[count_i]->EGetEdge(first,second));

			children_Pg[count_i]->HideEdge(children_Pg[count_i]->EGetEdge(first,second));

			// set the label frequencies
			if(!children_Pg[count_i]->BIsValidNode(first))
				children_Pg[count_i]->DecrementLabelFreq(first);
			if(!children_Pg[count_i]->BIsValidNode(second)) // else if??
				children_Pg[count_i]->DecrementLabelFreq(second);

			count_i++;
		}
	}
	return children_Pg;
}


vector<Graph*> Graph::GetChildrenOfSingleNodeGraph(int *noOfChildren)
{
	//assert(GetGraphType() == GRAPH_SINGLE_NODE);

	(*noOfChildren) = 1;
	vector<Graph*> children_Pg;
	Graph* g = new Graph();
	children_Pg.push_back(g);
	children_Pg[0]->Copy(this);
	children_Pg[0]->SetNodeAddedOrRemoved(NGetNodeWithID(IGetTheSingleNode()));
	children_Pg[0]->SetAsInvalidNode(IGetTheSingleNode()); // label freq will be taken care by it
	string s="";
	children_Pg[0]->SetGraphCode(s);
	return children_Pg;
}

vector<Graph*> Graph::GetChildrenOfSingleEdgeGraph(int *noOfChildren)
{
	//assert(GetGraphType() == GRAPH_SINGLE_EDGE);

	(*noOfChildren) = 2;
	vector<Graph*> children_Pg;
	Graph* g = new Graph();
	Graph* g1 = new Graph();
	children_Pg.push_back(g);
	children_Pg.push_back(g1);

	children_Pg[0]->Copy(this);
	pair<edge_iterator, edge_iterator> begin0_eIt = children_Pg[0]->PEGetAllEdges();
	//(*children_Pg)[0].SetEdgeAddedOrRemoved((*begin0_eIt));
	children_Pg[0]->SetEdgeAddedOrRemoved(*begin0_eIt.first);
	children_Pg[0]->SetNodeAddedOrRemoved(IGetSourceNode(*begin0_eIt.first));
	children_Pg[0]->DecrementLabelFreq(IGetTargetNode(*begin0_eIt.first));
	string s1 = GetAsString(IGetSourceNode(*begin0_eIt.first));
	children_Pg[0]->SetGraphCode(s1);
	children_Pg[0]->HideEdge(*begin0_eIt.first, NGetNodeWithID(IGetSourceNode(*begin0_eIt.first)));

	children_Pg[1]->Copy(this);
	pair<edge_iterator, edge_iterator> begin1_eIt = children_Pg[1]->PEGetAllEdges();
	//(*children_Pg)[1].SetEdgeAddedOrRemoved((*begin1_eIt));
	children_Pg[1]->SetEdgeAddedOrRemoved(*begin1_eIt.first);
	children_Pg[1]->SetNodeAddedOrRemoved(IGetTargetNode(*begin1_eIt.first));
	children_Pg[1]->DecrementLabelFreq(IGetSourceNode(*begin1_eIt.first));
	string s2 = GetAsString(IGetTargetNode(*begin1_eIt.first));
	children_Pg[1]->SetGraphCode(s2);
	children_Pg[1]->HideEdge(*begin1_eIt.first, NGetNodeWithID(IGetTargetNode(*begin1_eIt.first)));
	return children_Pg;
}

void Graph::GetEmptyChild(Graph *child_Pg)
{
	// make sure that the graph is not empty
	//assert(GetGraphType() != GRAPH_EMPTY);

	// Copy the main graph
	(*child_Pg).Copy(this);
	// hide all the edges
	edge_iterator ei, ei_end,next_e;
	pair<edge_iterator, edge_iterator> e_iterator = (*child_Pg).PEGetAllEdges();
	ei = e_iterator.first;

	while(ei != e_iterator.second)
	{
		ei_end = ei;
		next_e = ++ei;
		if((*child_Pg).GetGraphType() != GRAPH_SINGLE_EDGE) // not the single ede
		{
			(*child_Pg).SetEdgeAddedOrRemoved((*ei_end));
			(*child_Pg).HideEdge((*ei_end),false); // dont check the disconnectivity
		}
		else // single edge
		{
			(*child_Pg).SetEdgeAddedOrRemoved(*ei_end);
			(*child_Pg).HideEdge((*ei_end),NGetNodeWithID(IGetSourceNode(*ei_end)));
		}
		ei = next_e;
	}
	// std::cout << "REached here_bottom_graph!!" << '\n';
	// Set all the nodes as invalid
	// std::cout << "/* message */" << '\n';
	for(int i=0; i < num_vertices(G); i++)
		if((*child_Pg).BIsValidNode(i))
			(*child_Pg).SetAsInvalidNode(i);

	string temp="";
	(*child_Pg).SetGraphCode(temp);
}

bool Graph::GetFirstFreqChildOfSingleNodeGraph(int noOfGraphDB, Graph *graphDB,
		vector < vector < vector<Graph*> > > knownFreqGraphs,
		vector < vector < vector<Graph*> > > knownInfreqGraphs,
		int minSup, Graph **ansGraph, int *toBeAdded, vector<Graph*> *inFreqFound)
{
	//assert(GetGraphType() == GRAPH_SINGLE_NODE);

	Graph g;
	g.Copy(this);
	g.SetNodeAddedOrRemoved(NGetNodeWithID(IGetTheSingleNode()));
	g.SetAsInvalidNode(IGetTheSingleNode()); // label freq will be taken care by it
	string s="";
	g.SetGraphCode(s);

	int r = g.IGetNoOfAllEdges();
	int c = g.IGetNoOfValidNodes();
	// first find in the knownFreqgraphs
	for(int j=0; j < knownFreqGraphs[r][c].size(); j++)
		if(g.BIsExactlyEqualTo(knownFreqGraphs[r][c][j])) // it is frequent
		{
			(*ansGraph) = new Graph();
			(*ansGraph)->Copy(&g);
			(*toBeAdded) = j;
			return true;
		}

	bool flag=false;
	// if you are here its not there in frequent graphs so find in
	// infrquent ones
	for(int j=0; j < knownInfreqGraphs[r][c].size(); j++)
		if(g.BIsExactlyEqualTo(knownInfreqGraphs[r][c][j])) // it is infrequent
		{
			// do nothing
			flag = true;
			break;
		}

	if(!flag) // still nothing is found
	{
		int freq_i=0, lostfreq_i=0;
		// find frequency
		for(int k=0; k < noOfGraphDB; k++)
		{
			if(g.IGetGraphID() == graphDB[k].IGetGraphID())
				freq_i++;
			else if(g.BIsSubGraphOf(&(graphDB[k])))
				freq_i++;
			else
				lostfreq_i++;

			if(freq_i >= minSup)
				break;
			if(lostfreq_i > noOfGraphDB-minSup)
			{
				freq_i = minSup-1;
				break;
			}
		}

		if(freq_i >= minSup) // frequent
		{
			(*ansGraph) = new Graph();
			(*ansGraph)->Copy(&g);
			(*toBeAdded) = -1;
			return true;
		}
		else // infrequent
		{
			// do nothing:: NO Change this. add this to inFreqFound
			Graph *temp_g = new Graph();
			temp_g->Copy(&g);
			(*inFreqFound).push_back(temp_g);
		}
	}

	return false;
}

bool Graph::GetFirstFreqChildOfSingleEdgeGraph(int noOfGraphDB, Graph *graphDB,
		vector < vector < vector<Graph*> > > knownFreqGraphs,
		vector < vector < vector<Graph*> > > knownInfreqGraphs,
		int minSup, Graph **ansGraph, int *toBeAdded, vector<Graph*> *inFreqFound)
{
	//assert(GetGraphType() == GRAPH_SINGLE_EDGE);
	Graph g1,g2;

	g1.Copy(this);
	pair<edge_iterator, edge_iterator> begin0_eIt = g1.PEGetAllEdges();
	g1.SetEdgeAddedOrRemoved(*begin0_eIt.first);
	g1.HideEdge(*begin0_eIt.first, NGetNodeWithID(IGetSourceNode(*begin0_eIt.first)));
	g1.DecrementLabelFreq(IGetTargetNode(*begin0_eIt.first));
	string s1 = GetAsString(IGetSourceNode(*begin0_eIt.first));
	g1.SetGraphCode(s1);
	//(*children_Pg)[0].FindNoOfIncidentEdges();

	int r = g1.IGetNoOfAllEdges();
	int c = g1.IGetNoOfValidNodes();
	// first find in the knownFreqgraphs
	for(int j=0; j < knownFreqGraphs[r][c].size(); j++)
		if(g1.BIsExactlyEqualTo(knownFreqGraphs[r][c][j])) // it is frequent
		{
			(*ansGraph) = new Graph();
			(*ansGraph)->Copy(&g1);
			(*toBeAdded) = j;
			return true;
		}

	bool flag=false;
	// if you are here its not there in frequent graphs so find in
	// infrquent ones
	for(int j=0; j < knownInfreqGraphs[r][c].size(); j++)
		if(g1.BIsExactlyEqualTo(knownInfreqGraphs[r][c][j])) // it is infrequent
		{
			// do nothing
			flag = true;
			break;
		}

	if(!flag) // still nothing is found
	{
		int freq_i=0, lostfreq_i=0;
		// find frequency
		for(int k=0; k < noOfGraphDB; k++)
		{
			if(g1.IGetGraphID() == graphDB[k].IGetGraphID())
				freq_i++;
			else if(g1.BIsSubGraphOf(&(graphDB[k])))
				freq_i++;
			else
				lostfreq_i++;

			if(freq_i >= minSup)
				break;
			if(lostfreq_i > noOfGraphDB-minSup)
			{
				freq_i = minSup-1;
				break;
			}
		}

		if(freq_i >= minSup) // frequent
		{
			(*ansGraph) = new Graph();
			(*ansGraph)->Copy(&g1);
			(*toBeAdded) = -1;
			return true;
		}
		else // infrequent
		{
			// do nothing:: NO Change this. add this to inFreqFound
			Graph *temp_g = new Graph();
			temp_g->Copy(&g1);
			(*inFreqFound).push_back(temp_g);
		}
	}

	g2.Copy(this);
	pair<edge_iterator, edge_iterator> begin1_eIt = g2.PEGetAllEdges();
	g2.SetEdgeAddedOrRemoved(*begin1_eIt.first);
	g2.HideEdge(*begin1_eIt.first, NGetNodeWithID(IGetTargetNode(*begin1_eIt.first)));
	g2.DecrementLabelFreq(IGetSourceNode(*begin1_eIt.first));
	string s2 = GetAsString(IGetTargetNode(*begin1_eIt.first));
	g2.SetGraphCode(s2);

	r = g2.IGetNoOfAllEdges();
	c = g2.IGetNoOfValidNodes();
	// first find in the knownFreqgraphs
	for(int j=0; j < knownFreqGraphs[r][c].size(); j++)
		if(g2.BIsExactlyEqualTo(knownFreqGraphs[r][c][j])) // it is frequent
		{
			(*ansGraph) = new Graph();
			(*ansGraph)->Copy(&g2);
			(*toBeAdded) = j;
			return true;
		}

	flag=false;
	// if you are here its not there in frequent graphs so find in
	// infrquent ones
	for(int j=0; j < knownInfreqGraphs[r][c].size(); j++)
		if(g2.BIsExactlyEqualTo(knownInfreqGraphs[r][c][j])) // it is infrequent
		{
			// do nothing
			flag = true;
			break;
		}

	if(!flag) // still nothing is found
	{
		int freq_i=0, lostfreq_i=0;
		// find frequency
		for(int k=0; k < noOfGraphDB; k++)
		{
			if(g2.IGetGraphID() == graphDB[k].IGetGraphID())
				freq_i++;
			else if(g2.BIsSubGraphOf(&(graphDB[k])))
				freq_i++;
			else
				lostfreq_i++;

			if(freq_i >= minSup)
				break;
			if(lostfreq_i > noOfGraphDB-minSup)
			{
				freq_i = minSup-1;
				break;
			}
		}

		if(freq_i >= minSup) // frequent
		{
			(*ansGraph) = new Graph();
			(*ansGraph)->Copy(&g2);
			(*toBeAdded) = -1;
			return true;
		}
		else // infrequent
		{
			// do nothing:: NO Change this. add this to inFreqFound
			Graph *temp_g = new Graph();
			temp_g->Copy(&g2);
			(*inFreqFound).push_back(temp_g);
		}
	}

	return false;
}

// To find start node 'n' in 'this' such that the label of 'n' has the least
// frequency in 'g' out of all other labels in 'this'
Node Graph::NGetStartNodeForMatching(Graph *g)
{
	int minFreq_i=INT_MAX;
	NodeID start_nID=-1;

	for(int i=0; i< num_vertices(G); i++)
	{
		if(isValidNode_Vb[i])
		{
			int gFreq_i = g->IGetLabelFreq(nodeLabels_Vi[i]);
			if(gFreq_i < minFreq_i)
			{
				minFreq_i = gFreq_i;
				start_nID = i;
			}
			// this case wont arise. just for assertion
			if(gFreq_i == -1)
				continue;
		}
	}

	// Pending? Check why this condition is needed
	if(start_nID == -1)
		return NGetFirstValidNode();

	return allNodes_Vn[start_nID];
}

// To find first valid start node 'n' in 'this'
Node Graph::NGetFirstValidNode()
{
	for(int i=0; i < num_vertices(G); i++)
		if(isValidNode_Vb[i])
			return allNodes_Vn[i];

	ErrorExit("Graph::NIDGetFirstValidNode","Error in finding the start node");
}

vector<Node> Graph::VNGetNodesWithLabel(int label_i)
{
	vector<Node> matchingNodes_Vn;
	for(int i=0; i < num_vertices(G); i++)
		if(isValidNode_Vb[i])
		{
			if(nodeLabels_Vi[i] == label_i)
				matchingNodes_Vn.push_back(allNodes_Vn[i]);
		}

	return matchingNodes_Vn;
}

vector<Node> Graph::VNGetNodesAdjToWithLabel(Node cur_n, int label_i,int thisedgeLabel_i)
{
	/*
	 * Given a node 'cur_n', a node label 'label_i' and an edge label
	 * 'thisedgeLabel_i', return all the nodes which have label_i incident
	 * on 'cur_n' with edge label 'thisedgeLabel_i'
	 * */
	vector<Node> adjNodes_Vn;

	// node::inout_edges_iterator begin_inoutIt = cur_n.inout_edges_begin();
	// node::inout_edges_iterator end_inoutIt = cur_n.inout_edges_end();
	pair<out_edge_iterator, out_edge_iterator> e_iterator = boost::out_edges(cur_n, G);
	while(e_iterator.first != e_iterator.second)
	{
		Edge edge1 = *e_iterator.first;
		if(IGetEdgeLabel(edge1) == thisedgeLabel_i)
		{
			if(NGetNodeWithID(IGetSourceNode(edge1)) == cur_n)
			{
				if(nodeLabels_Vi[IGetTargetNode(edge1)] == label_i)
					adjNodes_Vn.push_back(NGetNodeWithID(IGetTargetNode(edge1)));
			}
			else
			{
				if(nodeLabels_Vi[IGetSourceNode(edge1)] == label_i)
					adjNodes_Vn.push_back(NGetNodeWithID(IGetSourceNode(edge1)));
			}
		}
		++e_iterator.first;
	}

	return adjNodes_Vn;
}

void PrintEdgeSet(set<Edge> elements_Se)
{
	set<Edge>::iterator begin_SeIt = elements_Se.begin();
	set<Edge>::iterator end_SeIt = elements_Se.end();
	while(begin_SeIt != end_SeIt)
	{
		cout << (*begin_SeIt) << ", ";
		begin_SeIt++;
	}
	cout << endl;
}
bool Graph::BCanBeMatchedWith(Graph *super_g, Node subCurrent_n, Node superCurrent_n,
		map<int, int> subToSuper_m, map<int, int> superToSub_m,
		set<Edge> visited_Se, set<Edge> toBeVisited_Se)
{
	// this->Print();
	// super_g->Print();
	// std::cout << IGetNodeID(subCurrent_n) << "  " << super_g->IGetNodeID(superCurrent_n) << '\n';
	// std::cout << "Reached BCanBeMatchedWith" << '\n';
	// do the pruning: if the degree of subCurrent_n is more than
	// superCurrent_n then subCurrent_n cannot be mapped to superCurrent_n
	if(IGetDegree(subCurrent_n) > super_g->IGetDegree(superCurrent_n))
		return false; // definitely this cannot be mapped
	// do the mapping

	int v1 = IGetNodeID(subCurrent_n);
	int v2 = super_g->IGetNodeID(superCurrent_n);
	// std::cout << "v1 v2 values" << v1 << " " << v2 << '\n';
	// std::cout << "hereeeee" << subToSuper_m.size() << subToSuper_m.max_size()<<'\n';
	// if(subToSuper_m.find(v1) == subToSuper_m.end())
	// {
	// 	std::cout << "here1" << '\n';
	// 	subToSuper_m[v1] = v2;
	// }
	// if(superToSub_m.find(v2) == superToSub_m.end())
	// {
	// 	std::cout << "here2" << '\n';
	// 	superToSub_m[v2] = v1;
	// }
	subToSuper_m.insert({v1,v2});
	superToSub_m.insert({v2,v1});

	//std::cout << "sravya2" << '\n';
	// add all the edges from the 'subCurrent_n' to the 'toBeVisited_Se'
	pair<out_edge_iterator, out_edge_iterator> e_iterator = boost::out_edges(subCurrent_n, G);
	//std::cout << "ok3" << '\n';
	int cnt=0;
	while(e_iterator.first != e_iterator.second)
	{
		// check if this edge has been visited already
		Edge edge1 = *e_iterator.first;
		set<Edge>::iterator find_SeIt = visited_Se.find(edge1);
		if(find_SeIt == visited_Se.end())//if its not present in visited edges
			toBeVisited_Se.insert(edge1);
		++e_iterator.first;
		cnt++;
	}
	// std::cout << "cnount of edges " << cnt << "   " << toBeVisited_Se.size() << '\n';
	// cout << "ok4" << endl;
	// check if all the edges have been visited and all the nodes have been mapped
	if(toBeVisited_Se.size() == 0)
	{
		//std::cout << "ok5" << '\n';
		if(visited_Se.size() != num_edges(G))
			return false;

		//assert(visited_Se.size() == number_of_edges()); // make sure that all the edges are visited
		if(subToSuper_m.size() != noOfValidNodes_i) // make sure that all the valid nodes are
			return false;
		//assert(subToSuper_m.size() == noOfValidNodes_i); // make sure that all the valid nodes are
		// mapped

		return true;
	}
	set<Edge>::iterator next_SeIt = toBeVisited_Se.begin();
	Edge next_e = (*next_SeIt);
	// std::cout << "Edge end nodes " << IGetSourceNode(next_e) << IGetTargetNode(next_e) << '\n';

	visited_Se.insert(next_e); // add it to the list of visited edges
	toBeVisited_Se.erase(next_e); // remove it from the set
	set<Edge>::iterator it;

	// one of the ends of the edge will be mapped
	map<int, int>::iterator findNext_mIt = subToSuper_m.find(IGetSourceNode(next_e));
	Node subFrom_n, superFrom_n;
	if(findNext_mIt != subToSuper_m.end()) // found it
	{
		subFrom_n = NGetNodeWithID(IGetSourceNode(next_e));
		superFrom_n = super_g->NGetNodeWithID((*findNext_mIt).second);
	}
	else
	{
		findNext_mIt = subToSuper_m.find(IGetTargetNode(next_e));
		// if(findNext_mIt != subToSuper_m.end())
		// {
		// 	this->Print();
		// 	super_g->Print();
		// }
		assert(findNext_mIt != subToSuper_m.end()); // make sure that target is found
		subFrom_n = NGetNodeWithID(IGetTargetNode(next_e));
		superFrom_n = super_g->NGetNodeWithID((*findNext_mIt).second);
	}
	//std::cout << "Sub and super Nodes";
	// Get the other end of the edge
	Node subTo_n = GetOppositeEdge(next_e, subFrom_n);
	// std::cout << IGetNodeID(subFrom_n) << IGetNodeID(subTo_n) <<"Egdge vlues" << IGetTargetNode(next_e) << IGetSourceNode(next_e) << '\n';
	if (subTo_n == subFrom_n)
	{
		std::cout << "Something wrong here!!!" << '\n';
	}

	//cout << "ok4" << endl;
	/*
	 * Get all the nodes in supergraph which are adjacent to superCurrent_n
	 * and which have the same label as 'subOpposite_n'
	 */
	vector<Node> superTo_Vn = super_g->VNGetNodesAdjToWithLabel(superFrom_n,
			nodeLabels_Vi[IGetNodeID(subTo_n)],IGetEdgeLabel(next_e));
	/*
	 * for each element of superOpposite_Vn: s
	 * if s is not mapped or if its mapped to subOpposite_n
	 * then map s to subOpposite_n
	 */
	// cout << "ok6" << endl;
	//  std::cout << "Finalllllll values" << '\n';
 	// std::cout << " SuperVn size  " <<superTo_Vn.size() << '\n';
	for(int i=0; i < superTo_Vn.size(); i++)
	{

		bool shouldBeMapped_b = false;
		bool present = false;
		//std::cout << "pk1" << '\n';
		// find superOpposite_Vn[i] in subToSuper_m
		//std::cout << IGetNodeID(subTo_n) << " " <<super_g->IGetNodeID(superTo_Vn[i]) << '\n';
		map<int, int>::iterator findSub_mIt = subToSuper_m.find(IGetNodeID(subTo_n));
		if(findSub_mIt == subToSuper_m.end()) // not mapped
		{
			map<int, int>::iterator findSuper_mIt = superToSub_m.find( super_g->IGetNodeID(superTo_Vn[i]) );
			if(findSuper_mIt == superToSub_m.end()) // both are not mapped
				shouldBeMapped_b = true;
		}
		// subTo_n is already mapped
		// and its should be mapped to superTo_Vn[i]
		else if( (*findSub_mIt).second == super_g->IGetNodeID(superTo_Vn[i]))
		{
			shouldBeMapped_b = true;
			present = true;
		}
		// std::cout << "pk2" << '\n';
		if(shouldBeMapped_b)
		{
			// std::cout << "pk3" << '\n';
			// std::cout << "Inside shouldbemapped" << IGetNodeID(subTo_n) << " " << super_g->IGetNodeID(superTo_Vn[i]) <<'\n';
			bool val123 = this->BCanBeMatchedWith(super_g, subTo_n, superTo_Vn[i], subToSuper_m, superToSub_m, visited_Se, toBeVisited_Se);
			//std::cout << "returns" << val123 << '\n';
			if(val123)
				return true;
			// if(!present)
			// {
			// 	subToSuper_m.erase(IGetNodeID(subTo_n));
			// 	superToSub_m.erase(super_g->IGetNodeID(superTo_Vn[i]));
			// }
		}
	}
	return false;
}


// main_storage for every MFS
void Graph::build_hash(int start, pair< pair<int,int>, pair<int,int> > edge, map< int, vector< pair< map<int, int>, map<int, int> > > > &main_storage, vector<Graph*> maxFreq, vector<bool> validMaxFreq, vector<bool> newMaxFreq)
{
	int node1_label = IGetNodeLabel(edge.first.first);
	int node2_label = IGetNodeLabel(edge.first.second);

    for(int i=0;i<maxFreq.size();i++)
    {
      if(!validMaxFreq[i])
          continue;
			if(start!=0 && !maxFreq[i]->HasEdgeLabelCheck(node1_label, node2_label, edge.second.first))
				continue;
      vector< pair< map<int, int>, map<int, int> > > store = maxFreq[i]->get_all_embeddings(this);
      main_storage[i] = store;
    }
		//std::cout << main_storage[8].size() << '\n';
    return;
}

void Graph::build_hash_delete(int start, map< int, vector< pair< map<int, int>, map<int, int> > > > &main_storage, vector<Graph*> maxFreq, vector<bool> validMaxFreq, vector<bool> newMaxFreq)
{
	for(int i=0;i<maxFreq.size();i++)
    {
      if(!validMaxFreq[i])
          continue;
      vector< pair< map<int, int>, map<int, int> > > store = maxFreq[i]->get_all_embeddings(this);
      main_storage[i] = store;
    }
    return;
}

bool Graph::BCanBeMatchedWith_all_embeddings(Graph *super_g, Node subCurrent_n, Node superCurrent_n,
                              map<int, int> subToSuper_m, map<int, int> superToSub_m,
                              set<Edge> visited_Se, set<Edge> toBeVisited_Se, vector< pair< map<int, int>, map<int, int> > > &store, Edge edge)
{
		// this->Print();
		// super_g->Print();
    // cout << IGetDegree(subCurrent_n) << " " << super_g->IGetDegree(superCurrent_n) << endl;
		std::cout << IGetNodeID(subCurrent_n) << "  " << super_g->IGetNodeID(superCurrent_n) << '\n';
    // cout << edge << endl;
		if(IGetDegree(subCurrent_n) > super_g->IGetDegree(superCurrent_n))
			return false; // definitely this cannot be mapped

		// std::cout << "hereee" << '\n';
    // do the mapping
		subToSuper_m.insert( make_pair(IGetNodeID(subCurrent_n),super_g->IGetNodeID(superCurrent_n)) );
		superToSub_m.insert( make_pair(super_g->IGetNodeID(superCurrent_n),IGetNodeID(subCurrent_n)) );

    pair<out_edge_iterator, out_edge_iterator> e_iterator = boost::out_edges(subCurrent_n, G);
		// cout << "ok3" << endl;
		int cnt=0;
		while(e_iterator.first != e_iterator.second)
		{
			// check if this edge has been visited already
			Edge edge1 = *e_iterator.first;
			set<Edge>::iterator find_SeIt = visited_Se.find(edge1);
			if(find_SeIt == visited_Se.end())//if its not present in visited edges
				toBeVisited_Se.insert(edge1);
			++e_iterator.first;
			cnt++;
		}
		// std::cout << "cnount of edges " << cnt << "   " << toBeVisited_Se.size() << '\n';

    // check if all the edges have been visited and all the nodes have been mapped
    if(toBeVisited_Se.size() == 0)
    {
			// std::cout << "Will it ever reach heree" << '\n';
        if(visited_Se.size() != num_edges(G))
            return false;
        //assert(visited_Se.size() == number_of_edges()); // make sure that all the edges are visited
        if(subToSuper_m.size() != noOfValidNodes_i) // make sure that all the valid nodes are
            return false;
        //assert(subToSuper_m.size() == noOfValidNodes_i); // make sure that all the valid nodes are
        bool insert=true;
        // for(int i=0;i<store.size();i++)
        // {
        //   if(store[i].first==subToSuper_m)
        //   {
        //     insert=false;
        //     break;
        //   }
        // }
        // if(insert)
        (store).push_back(make_pair(subToSuper_m, superToSub_m));
        return true;
    }
    set<Edge>::iterator next_SeIt = toBeVisited_Se.begin();

    Edge next_e = (*next_SeIt);
		// std::cout << "Edge end nodes " << IGetSourceNode(next_e) << IGetTargetNode(next_e) << '\n';
    // cout << next_e << endl;
		visited_Se.insert(next_e); // add it to the list of visited edges
		toBeVisited_Se.erase(next_e); // remove it from the set
		set<Edge>::iterator it;

		map<int, int>::iterator findNext_mIt = subToSuper_m.find(IGetSourceNode(next_e));
		Node subFrom_n, superFrom_n;
		if(findNext_mIt != subToSuper_m.end()) // found it
		{
			subFrom_n = NGetNodeWithID(IGetSourceNode(next_e));
			superFrom_n = super_g->NGetNodeWithID((*findNext_mIt).second);
		}
		else
		{
			findNext_mIt = subToSuper_m.find(IGetTargetNode(next_e));
			assert(findNext_mIt != subToSuper_m.end()); // make sure that target is found
			subFrom_n = NGetNodeWithID(IGetTargetNode(next_e));
			superFrom_n = super_g->NGetNodeWithID((*findNext_mIt).second);
		}

    // Get the other end of the edge
    Node subTo_n = GetOppositeEdge(next_e, subFrom_n);
		// std::cout << IGetNodeID(subFrom_n) << IGetNodeID(subTo_n) <<"Super to and fro nodes " << super_g->IGetNodeID(superFrom_n) << '\n';
		if (subTo_n == subFrom_n)
		{
			std::cout << "Something wrong here!!!" << '\n';
		}
    /*
     * Get all the nodes in supergraph which are adjacent to superCurrent_n
     * and which have the same label as 'subOpposite_n'
     */
		 vector<Node> superTo_Vn = super_g->VNGetNodesAdjToWithLabel(superFrom_n,
 				nodeLabels_Vi[IGetNodeID(subTo_n)],IGetEdgeLabel(next_e));

		std::cout << " SuperVn size  " << superTo_Vn.size() << '\n';

    bool flag = false;
    for(int i=0; i < superTo_Vn.size(); i++)
    {
			// std::cout << " Super node value" << i << "  " <<super_g->IGetNodeID(superTo_Vn[i]) << '\n';
        bool shouldBeMapped_b = false;
        bool present = false;
        // find superOpposite_Vn[i] in subToSuper_m
				map<int, int>::iterator findSub_mIt = subToSuper_m.find(IGetNodeID(subTo_n));
				if(findSub_mIt == subToSuper_m.end()) // not mapped
				{
					map<int, int>::iterator findSuper_mIt = superToSub_m.find( super_g->IGetNodeID(superTo_Vn[i]) );
					if(findSuper_mIt == superToSub_m.end()) // both are not mapped
						shouldBeMapped_b = true;
				}
        // subTo_n is already mapped
        // and its should be mapped to superTo_Vn[i]
        else if( (*findSub_mIt).second == super_g->IGetNodeID(superTo_Vn[i]))
        {
              shouldBeMapped_b = true;
              present = true;
        }
				// std::cout << "should be mpped" << shouldBeMapped_b << '\n';
        if(shouldBeMapped_b)
        {
            bool val = this->BCanBeMatchedWith_all_embeddings(super_g, subTo_n, superTo_Vn[i],
                                 subToSuper_m, superToSub_m, visited_Se, toBeVisited_Se, store, next_e);
            if(val)
            {
							  flag = true;
								std::cout << "Flag is true finlly" << '\n';
						}
            // if(!present && !val)
            // {
            //     subToSuper_m.erase(IGetNodeID(subTo_n));
            //     superToSub_m.erase(super_g->IGetNodeID(superTo_Vn[i]));
            // }
						// subToSuper_m.erase(IGetNodeID(subTo_n));
            // superToSub_m.erase(super_g->IGetNodeID(superTo_Vn[i]));
            // toBeVisited_Se.insert(next_e);
            // visited_Se.erase(next_e);
        }
    }
    if(flag)
      return true;
		// if(superNodes_Vn.size() == 0)
		// {
		// 	toBeVisited_Se.insert(next_e);
		// 	visited_Se.erase(next_e);
		// }
    return false;
}

bool Graph::BCanMatchLabelsIncidentEdges(Graph *super_g)
{
	/*
	 * Prunes 'this' not being a subgraph of 'super_g' if
	 *  i. the entries of labelNoOfIncidentEdges_Vi is greater than that of
	 *     super_g
	 *  ii. the entries of freqEdgeLabels_Vi is greater than that of super_g
	 * */

	int sizeSuper_i = super_g->IGetSizeLabelIncidentNoOfEdges();

	for(int i=0; i < labelNoOfIncidentEdges_Vi.size(); i++)
	{
		if(i >= sizeSuper_i)
			if(labelNoOfIncidentEdges_Vi[i] != 0)
				return false;
			else
				continue;

		if(labelNoOfIncidentEdges_Vi[i] > super_g->IGetLabelIncidentNoOfEdges(i))
			return false;
	}
	// std::cout << "here came" << '\n';
	int num_superG = super_g->IGetSizeFreqEdgeLabels();
	for(int i=0; i < freqEdgeLabels_Vi.size(); i++)
	{
		if(i >= num_superG)
		{
			if(freqEdgeLabels_Vi[i] != 0)
				return false;
		}
		else
		{
			if(freqEdgeLabels_Vi[i] > super_g->IGetFreqEdgeLabel(i))
				return false;
		}
	}
	//return true
	return true;
}

pair<map<int,int>, map<int,int> > Graph::get_one_embedding(Graph *super_g)
{
    // if 'this' is empty then check the graph id and return true if super_g
    // is also not empty
    pair< map<int, int>, map<int, int> > store;
    GraphType type_gt = GetGraphType();
    // if(type_gt == GRAPH_EMPTY)
    // {
    //     if(IGetGraphID() == super_g->IGetGraphID())
    //         return true;
    //     else
    //         return false;
    // }

    if(num_edges(G) > super_g->IGetNoOfAllEdges())
        return store;

    if(IGetNoOfValidNodes() > super_g->IGetNoOfValidNodes())
        return store;


    // do the extra checking of edge labels frequencies
    if(!BCanMatchLabelsIncidentEdges(super_g))
        return store;

    // make sure that 'this' is not empty
    //assert( GetGraphType() != GRAPH_EMPTY);

    /*
     * Heuristic: decide the starting node of matching intelligently
     * for each node in the subgraph, find the frequency of the label of it
     * in the super graph and decide that one whose frequency is the least
     * Comment the next line if you dont want to use the heuristic
     */
    Node start_n = NGetStartNodeForMatching(super_g);

    /*
     * Without heuristics you can choose any node in the subgraph
     * XXX: Uncomment the next line if you dont want to use the heuristic
     * Node start_n = NGetFirstValidNode();
     */

    // mapping for subgraph nodes to supergraph nodes, super graph nodes to subgraph nodes

    // Get all the nodes in the super graph which can be mapped to 'start_n'
    vector<Node> superNodes_Vn = super_g->VNGetNodesWithLabel(IGetNodeLabel(start_n));
    if(type_gt == GRAPH_SINGLE_NODE) // only single node to match
    {
      map<int, int> subToSuper_m;
      map<int, int> superToSub_m;
      // remember the list of visited edges and to be visited edges
      // set<Edge> visited_Se, toBeVisited_Se;

        if(superNodes_Vn.size() > 0) // found matches
        {
            for(int i=0;i<superNodes_Vn.size();i++)
            {
              subToSuper_m.insert( pair<int,int>(IGetNodeID(start_n),super_g->IGetNodeID(superNodes_Vn[i])) );
              superToSub_m.insert( pair<int, int>(super_g->IGetNodeID(superNodes_Vn[i]), IGetNodeID(start_n)));
            }
            store = make_pair(subToSuper_m, superToSub_m);
            if(superNodes_Vn.size() != 0)
                superNodes_Vn.clear();
            if(subToSuper_m.size() != 0)
                subToSuper_m.erase(subToSuper_m.begin(), subToSuper_m.end());
                superToSub_m.erase(superToSub_m.begin(), superToSub_m.end());
            // start_n.~Node();

            return store;
        }
        else // no matching nodes
        {
            if(superNodes_Vn.size() != 0)
                superNodes_Vn.clear();
            if(subToSuper_m.size() != 0)
                subToSuper_m.erase(subToSuper_m.begin(), subToSuper_m.end());
                superToSub_m.erase(superToSub_m.begin(), superToSub_m.end());
            // start_n.~Node();

            return store;
        }
    }

    bool flag = false;
    for(int i=0; i < superNodes_Vn.size(); i++)
    {
      map<int, int> subToSuper_m, superToSub_m;
      set<Edge> visited_Se, toBeVisited_Se;
      set<Edge>::iterator next_SeIt = toBeVisited_Se.begin();
      Edge edge = (*next_SeIt);
        if(BCanBeMatchedWith(super_g, start_n, superNodes_Vn[i], subToSuper_m, superToSub_m, visited_Se, toBeVisited_Se))
        {
					store = make_pair(subToSuper_m, superToSub_m);
          return store;
        }
    }

    return store;
}

vector<pair<map<int,int>, map<int,int> > > Graph::get_all_embeddings(Graph *super_g)
{

    // if 'this' is empty then check the graph id and return true if super_g
    // is also not empty
		std::cout << "/* message */" << '\n';
    vector< pair< map<int, int>, map<int, int> > > store;
    GraphType type_gt = GetGraphType();
    // if(type_gt == GRAPH_EMPTY)
    // {
    //     if(IGetGraphID() == super_g->IGetGraphID())
    //         return true;
    //     else
    //         return false;
    // }

    if(num_edges(G) > super_g->IGetNoOfAllEdges())
        return store;

    if(IGetNoOfValidNodes() > super_g->IGetNoOfValidNodes())
        return store;

				std::cout << "Should COme hereeee" << '\n';
    // do the extra checking of edge labels frequencies
    if(!BCanMatchLabelsIncidentEdges(super_g))
        return store;

				std::cout << "Atleats hereeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeees" << '\n';
    // make sure that 'this' is not empty
    //assert( GetGraphType() != GRAPH_EMPTY);

    /*
     * Heuristic: decide the starting node of matching intelligently
     * for each node in the subgraph, find the frequency of the label of it
     * in the super graph and decide that one whose frequency is the least
     * Comment the next line if you dont want to use the heuristic
     */
    Node start_n = NGetStartNodeForMatching(super_g);

    /*
     * Without heuristics you can choose any node in the subgraph
     * XXX: Uncomment the next line if you dont want to use the heuristic
     * Node start_n = NGetFirstValidNode();
     */

    // mapping for subgraph nodes to supergraph nodes, super graph nodes to subgraph nodes

    // Get all the nodes in the super graph which can be mapped to 'start_n'
    vector<Node> superNodes_Vn = super_g->VNGetNodesWithLabel(IGetNodeLabel(start_n));
    if(type_gt == GRAPH_SINGLE_NODE) // only single node to match
    {
      map<int, int> subToSuper_m;
      map<int, int> superToSub_m;
      // remember the list of visited edges and to be visited edges
      // set<Edge> visited_Se, toBeVisited_Se;

        if(superNodes_Vn.size() > 0) // found matches
        {
            for(int i=0;i<superNodes_Vn.size();i++)
            {
              subToSuper_m.insert( pair<int,int>(IGetNodeID(start_n),super_g->IGetNodeID(superNodes_Vn[i])) );
              superToSub_m.insert( pair<int, int>(super_g->IGetNodeID(superNodes_Vn[i]), IGetNodeID(start_n)));
            }
            store.push_back(make_pair(subToSuper_m, superToSub_m));
            if(superNodes_Vn.size() != 0)
                superNodes_Vn.clear();
            if(subToSuper_m.size() != 0)
                subToSuper_m.erase(subToSuper_m.begin(), subToSuper_m.end());
                superToSub_m.erase(superToSub_m.begin(), superToSub_m.end());
            // start_n.~Node();

            return store;
        }
        else // no matching nodes
        {
            if(superNodes_Vn.size() != 0)
                superNodes_Vn.clear();
            if(subToSuper_m.size() != 0)
                subToSuper_m.erase(subToSuper_m.begin(), subToSuper_m.end());
                superToSub_m.erase(superToSub_m.begin(), superToSub_m.end());
            // start_n.~Node();

            return store;
        }
    }

    bool flag = false;
		// if(super_g->IGetGraphID() == 0)
			std::cout << superNodes_Vn.size() << "cgfghh"<< '\n';
    for(int i=0; i < superNodes_Vn.size(); i++)
    {
      map<int, int> subToSuper_m, superToSub_m;
      set<Edge> visited_Se, toBeVisited_Se;
      set<Edge>::iterator next_SeIt = toBeVisited_Se.begin();
      Edge edge = (*next_SeIt);
			if(BCanBeMatchedWith_all_embeddings(super_g, start_n, superNodes_Vn[i], subToSuper_m, superToSub_m, visited_Se, toBeVisited_Se, store, edge))
        {
					std::cout << "/* message */ finally true" << '\n';
          flag = true;
        }
    }

    return store;
}

bool Graph::BIsSubGraphOf(Graph *super_g)
{
	num_subgraphiso++;
	// if 'this' is empty then check the graph id and return true if super_g
	// is also not empty
	// super_g->Print();
	// this->Print();
	GraphType type_gt = GetGraphType();
	if(type_gt == GRAPH_EMPTY)
	{
		if(IGetGraphID() == super_g->IGetGraphID())
			return true;
		else
			return false;
	}

	if(IGetNoOfAllEdges() > super_g->IGetNoOfAllEdges())
		return false;

	if(IGetNoOfValidNodes() > super_g->IGetNoOfValidNodes())
		return false;


	// do the extra checking of edge labels frequencies
	if(!BCanMatchLabelsIncidentEdges(super_g))
		return false;
	//std::cout << "Hereeeeee12367" << '\n';
	// make sure that 'this' is not empty
	//assert( GetGraphType() != GRAPH_EMPTY);

	/*
	 * Heuristic: decide the starting node of matching intelligently
	 * for each node in the subgraph, find the frequency of the label of it
	 * in the super graph and decide that one whose frequency is the least
	 * Comment the next line if you dont want to use the heuristic
	 */
	 // std::cout << "Here finally" << '\n';
	Node start_n = NGetStartNodeForMatching(super_g);
	//std::cout << IGetNodeID(start_n) << '\n';
	/*
	 * Without heuristics you can choose any node in the subgraph
	 * XXX: Uncomment the next line if you dont want to use the heuristic
	 * Node start_n = NGetFirstValidNode();
	 */

	// mapping for subgraph nodes to supergraph nodes, super graph nodes to subgraph nodes

	// Get all the nodes in the super graph which can be mapped to 'start_n'
	vector<Node> superNodes_Vn = super_g->VNGetNodesWithLabel(nodeLabels_Vi[IGetNodeID(start_n)]);
	if(type_gt == GRAPH_SINGLE_NODE) // only single node to match
	{
		if(superNodes_Vn.size() > 0) // found matches
		{
			return true;
		}
		else // no matching nodes
		{
			return false;
		}
	}

	// else if 'this' is not a single node
	// For each element in the supergraph which can be mapped to 'start_n'
	//std::cout << "Super nodes" << superNodes_Vn.size()<< '\n';
	map<int, int> subToSuper_m, superToSub_m;
	set<Edge> visited_Se, toBeVisited_Se;
	for(int i=0; i < superNodes_Vn.size(); i++)
	{
		// this is the recursive function for matching 'this' as a subgraph
		// of 'super_g' starting at the start_n

		//std::cout << "/* 4message */" << '\n';
		bool val12 = BCanBeMatchedWith(super_g, start_n, superNodes_Vn[i], subToSuper_m, superToSub_m, visited_Se, toBeVisited_Se);
		//std::cout << "val12 */" << val12 << '\n';
		if(val12)
		{
			return true;
		}
		subToSuper_m.clear();
		superToSub_m.clear();
		visited_Se.clear();
		toBeVisited_Se.clear();
	}
	//std::cout << "Did it reach hereeee" << '\n';
	return false;
}


bool Graph::BIsSuperGraphOf(Graph *sub_g)
{
	return sub_g->BIsSubGraphOf(this);
}

bool Graph::BIsIsomorphicTo(Graph *g)
{
	num_iso++;
	if(this->IGetNoOfValidNodes() != g->IGetNoOfValidNodes())
		return false;
	if(num_edges(G) != g->IGetNoOfAllEdges())
		return false;
	// /std::cout << "/* message */" << '\n';
	if(this->BIsSubGraphOf(g))
		return true;

	return false;
}


bool Graph::BIsExactlyEqualTo(Graph *g)
{

	if(IGetGraphID() != g->IGetGraphID())
		return false;
	if(graphCode == g->GetGraphCode())
		return true;

	return false;
}

bool Graph::BIsThisEdgeHidden(NodeID source_nID, NodeID target_nID)
{
	for(int i=0; i < hiddenEdges_Ve.size(); i++)
	{
		if( (hiddenEdges_Ve[i].first.first == source_nID)
				&& (hiddenEdges_Ve[i].first.second == target_nID) )
			return true;
	}
	return false;
}

// int Graph::IsExtendable(int node1, int node2, int edge_label)
// {
//   int flag=0;
//   for(int i=0;i<allNodes_Vn.size();i++)
//   {
//     if(isValidNode_Vb[IGetNodeID(allNodes_Vn[i])])
//     {
//       if(nodeLabels_Vi[IGetNodeID(allNodes_Vn[i])] == nodeLabels_Vi[node1])
//         flag+=1;
//       if(nodeLabels_Vi[IGetNodeID(allNodes_Vn[i])] == nodeLabels_Vi[node2])
//         flag+=2;
//     }
//   }
//   graph::edge_iterator begin_eIt = edges_begin();
//   graph::edge_iterator end_eIt = edges_end();
//   int cnt=0;
//   while(begin_eIt != end_eIt)
//   {
//       cnt++;
//       if(nodeIDs_nMnid[(*begin_eIt).source()] == node1 && nodeIDs_nMnid[(*begin_eIt).target()]==node2 && edgeLabel_eM[*begin_eIt]==edge_label)
//           return 4;
//       if(nodeIDs_nMnid[(*begin_eIt).source()] == node2 && nodeIDs_nMnid[(*begin_eIt).target()]==node1 && edgeLabel_eM[*begin_eIt]==edge_label)
//         return 4;
//       begin_eIt++;
//   }
//
//   // cout << cnt << endl;
//   return flag;
// }

// void Graph::MaxExtend(int node1, int node2, int edge_label, int flag)
// {
//   if(flag == 1)
//   {
//     this->BInsertNode(nodeLabels_Vi[node2]);
//     this->BInsertEdge(node1, node2, edge_label);
//   }
//   else if(flag == 2)
//   {
//     this->BInsertNode(nodeLabels_Vi[node1]);
//     this->BInsertEdge(node1, node2, edge_label);
//   }
//   else
//   {
//     this->BInsertEdge(node1, node2, edge_label);
//   }
//   return;
// }

void Graph::GetCommonParent(Graph *g1, Graph *g2)
{
	// this function can only be called betweeh 'g1' and 'g2'
	// only if both of them differ only by one edge
	// it cannot be called if both are single node graphs

	/* A more rigorous check might be needed but skipping it for efficiency
	 * reasons: more rigorously we have to go through the set of edges in
	 * 'g1' and 'g2' and make sure that only one edge is the difference
	 * */
	//assert ( g1.number_of_edges() == g2.number_of_edges()); // both contain the same number of edges
	//GraphType g1Type_gt = g1->GetGraphType();
	//assert ( (g1Type_gt != GRAPH_EMPTY) && (g1Type_gt != GRAPH_SINGLE_NODE));
	//GraphType g2Type_gt = g2.GetGraphType();
	//assert ( (g2Type_gt != GRAPH_EMPTY) && (g2Type_gt != GRAPH_SINGLE_NODE));

	// if both the graphs are single edge graphs then make sure that both of
	// them have a common node

	// take each edge in 'g2' and find if its hidden in 'g1'
	// if so insert it into g1 and return the parent
	// std::cout << "G1 graph:" << '\n';
	// g1->Print();
	// std::cout << "G2 graph:" << '\n';
	// g2->Print();
	edge_iterator ei, ei_end;
	for (tie(ei, ei_end) = g2->PEGetAllEdges(); ei != ei_end; ++ei)
	{
		NodeID g2Source_nID = g2->IGetSourceNode(*ei);
		NodeID g2Target_nID = g2->IGetTargetNode(*ei);
		// std::cout << g2Source_nID << ' ' << g2Target_nID <<'\n';
		// std::cout << "hidden edges" << '\n';
		// for(int i=0;i<hiddenEdges_Ve.size();i++)
		// {
		//   std::cout << hiddenEdges_Ve[i].first.first << ' ' << hiddenEdges_Ve[i].first.second << '\n';
		// }
		// g1->PrintHiddenEdgesCode();
		if(g1->BIsThisEdgeHidden(g2Source_nID, g2Target_nID))
		{
			Copy(g1);
			// std::cout << "Hello Inside" << '\n';
			HiddenEdge restore_e = EGetHiddenEdge(g2Source_nID, g2Target_nID);
			RestoreEdge(restore_e);
			SetEdgeAddedOrRemoved(restore_e.first.first, restore_e.first.second);
			// set the label frequency
			if(!BIsValidNode(restore_e.first.first))
				IncrementLabelFreq(g2Source_nID);
			if(!BIsValidNode(restore_e.first.second)) // else if??
				IncrementLabelFreq(g2Target_nID);
			return;

		}
	}

	ErrorExit("Graph::GetCommonParentWith","Unable to find the common parent");
}

bool Graph::BGetCommonParentSingleNodes(Graph *g1, Graph *g2)
{
	//assert(g1.GetGraphType() == GRAPH_SINGLE_NODE);
	//assert(g2.GetGraphType() == GRAPH_SINGLE_NODE);

	NodeID g1_nID = g1->IGetTheSingleNode();
	NodeID g2_nID = g2->IGetTheSingleNode();
	// check if there exists an edge in g1 with node ids g1_nID and g2_nID
	if(g1->BIsThisEdgeHidingUndirected(g1_nID, g2_nID))
	{
		Copy(g1);
		HiddenEdge ed = EGetHiddenEdgeUndirected(g1_nID, g2_nID);
		RestoreEdge(ed);
		SetEdgeAddedOrRemoved(ed.first.first, ed.first.second);
		// check the valid nodes
		if(!BIsValidNode(g1_nID))
			SetAsValidNode(g1_nID);
		if(!BIsValidNode(g2_nID))
			SetAsValidNode(g2_nID);
		return true;
	}
	return false;
}

// Haven't called this in expand_cut
// this function can be called only when g1 and g2 are children of a common
// super graph and the common subgraph is connected(lower diamond property)
bool Graph::GetCommonChild(Graph *g1, Graph *g2)
{
    // make sure that both of them have equal number of edges
    //assert (g1.number_of_edges() == g2.number_of_edges());

    GraphType g1_gt = g1->GetGraphType();
    GraphType g2_gt = g2->GetGraphType();
    // make sure that both are not empty graphs
    //assert (g1_gt != GRAPH_EMPTY);
    //assert (g2_gt != GRAPH_EMPTY);

    if(g1_gt == GRAPH_SINGLE_NODE)
    {
        //assert(g2_gt == GRAPH_SINGLE_NODE);
        int noOfChildren;
        vector<Graph*> children_Pg;
        children_Pg = g1->GetChildren(&noOfChildren);
        //assert(noOfChildren == 1);
        Copy(children_Pg[0]); // empty graph
        return true;
    }

    if(g1_gt == GRAPH_SINGLE_EDGE)
    {
        //assert(g2_gt == GRAPH_SINGLE_EDGE);
        int noOfChildren;
        vector<Graph*> children_Pg;
        children_Pg = g1->GetChildren(&noOfChildren);
        //assert(noOfChildren == 2); // both single node graphs

				edge_iterator ei, ei_end;
				tie(ei, ei_end) = g2->PEGetAllEdges();

        NodeID child0_nID = children_Pg[0]->IGetTheSingleNode();
        NodeID child1_nID = children_Pg[1]->IGetTheSingleNode();

        NodeID g2Source_nID = g2->IGetSourceNode(*ei);
        NodeID g2Target_nID = g2->IGetTargetNode(*ei);
        if( (g2Source_nID == child0_nID) || (g2Target_nID == child0_nID) )
        {
            Copy(children_Pg[0]);
            return true;
        }
        if( (g2Source_nID == child1_nID) || (g2Target_nID == child1_nID) )
        {
            Copy(children_Pg[1]);
            return true;
        }
        // ErrorExit("Graph::GetCommonChildWith", "Error in single edge case");
        return false;
    }
		map<pair< pair<int, int>, int >, int> mp1,mp2;
		int cnt1,cnt2;
		cnt1=0;cnt2=0;
		pair< pair<int, int>, int > pi;
		edge_iterator ei, ei_end;
		for (tie(ei, ei_end) = g1->PEGetAllEdges(); ei != ei_end; ++ei)
		{
			pi = make_pair(make_pair(g1->IGetSourceNode(*ei), g1->IGetTargetNode(*ei)), g1->IGetEdgeLabel(*ei));
			mp1.insert({pi,1});
		}
		for (tie(ei, ei_end) = g2->PEGetAllEdges(); ei != ei_end; ++ei)
		{
			pi = make_pair(make_pair(g2->IGetSourceNode(*ei), g2->IGetTargetNode(*ei)), g2->IGetEdgeLabel(*ei));
			if(mp1.find(pi) == mp1.end())
				cnt1++;
			mp2.insert({pi,1});
		}
		for (tie(ei, ei_end) = g1->PEGetAllEdges(); ei != ei_end; ++ei)
		{
			pi = make_pair(make_pair(g1->IGetSourceNode(*ei), g1->IGetTargetNode(*ei)), g1->IGetEdgeLabel(*ei));
			if(mp2.find(pi) == mp2.end())
				cnt2++;
		}
		if(cnt1 != 1 || cnt2 != 1)
			return false;
		for (tie(ei, ei_end) = g1->PEGetAllEdges(); ei != ei_end; ++ei)
		{
        NodeID g1Source_nID = g1->IGetSourceNode(*ei);
        NodeID g1Target_nID = g1->IGetTargetNode(*ei);
        if(g2->BIsThisEdgeHidden(g1Source_nID, g1Target_nID))
        {
					Graph* temp = new Graph();
					temp->Copy(g1);
					Edge hide_e = temp->EGetEdgeUndirected(g1Source_nID, g1Target_nID);
          if(temp->BCanHideEdge(hide_e))
					{
							Copy(g1);
							// std::cout << "/* message */" << '\n';
							// std::cout << "Edges : " <<  << "  " << IGetTargetNode(hide_e) << '\n';
							HideEdge(EGetEdgeUndirected(g1Source_nID, g1Target_nID));
							// std::cout << "Degress  " << IGetDegree(g1Source_nID) << "  " << IGetDegree(g1Target_nID) << '\n';
							SetEdgeAddedOrRemoved(hide_e);
	            // set the label frequencies
	            if(!BIsValidNode(g1Source_nID))
	                DecrementLabelFreq(g1Source_nID);
	            if(!BIsValidNode(g1Target_nID))
	                DecrementLabelFreq(g1Target_nID);
							temp->~Graph();
	            return true;
					}
					temp->~Graph();
        }
    }

		// std::cout << "/* message */" << '\n';
    // ErrorExit("Graph::GetCommonChildWith", "Error in general case");
    return false;
}

void Graph::FindNoOfIncidentEdges()
{
	// allocate memory if its not there already
	int curSize = labelFreqs_Vi.size() - labelNoOfIncidentEdges_Vi.size();
	for(int i=0; i < curSize; i++)
		labelNoOfIncidentEdges_Vi.push_back(0);

	// initialize
	for(int i=0; i < labelFreqs_Vi.size(); i++)
		labelNoOfIncidentEdges_Vi[i] = 0;

	// set the no of incident edges count
	edge_iterator ei, ei_end;
	for (tie(ei, ei_end) = edges(G); ei != ei_end; ++ei)
	{
		int sourceLab = nodeLabels_Vi[IGetSourceNode(*ei)];
		int targetLab = nodeLabels_Vi[IGetTargetNode(*ei)];
		//assert(sourceLab < labelNoOfIncidentEdges_Vi.size());
		//assert(targetLab < labelNoOfIncidentEdges_Vi.size());
		labelNoOfIncidentEdges_Vi[sourceLab]++;
		labelNoOfIncidentEdges_Vi[targetLab]++;
	}
}


Edge Graph::GetTheOnlyIncidentEdge(Node n)
{
	// before calling this function you have to check that the degree of n
	// is 1
  edge_iterator ei, ei_end;
  for (tie(ei, ei_end) = edges(G); ei != ei_end; ++ei)
	{
    if(boost::source(*ei, G) == n || boost::target(*ei, G) == n)
      return *ei;
  }
	// pair<out_edge_iterator, out_edge_iterator> edges = boost::out_edges(n, G);
	// return *edges.first;
}

// 'this' is the exact subgraph (embedded subgraph) of 'g'
bool Graph::BIsCodeSubGraphOf(Graph *g)
{
	int thisGtype = GetGraphType();
	int gGtype = g->GetGraphType();
	//if(gGtype == GRAPH_EMPTY)
	//return false;
	if(thisGtype == GRAPH_EMPTY)
	{
		if(IGetGraphID() == g->IGetGraphID())
			return true;
		else
			return false;
	}
	else if(thisGtype == GRAPH_SINGLE_NODE)
	{
		if(gGtype == GRAPH_SINGLE_NODE)
		{
			if(graphCode == g->GetGraphCode())
				return true;
			else
				return false;
		}
		else
		{
			vector<int> arr;
			int nodeVal = atoi((char*)graphCode.c_str());
			SplitString(g->GetGraphCode(), ".", arr);
			for(int i=0; i < arr.size(); i++)
				if(arr[i] == nodeVal)
					return true;
			return false;
		}
	}

	if(gGtype == GRAPH_SINGLE_NODE)
		return false;

	vector<int> thisParts, gParts;
	thisParts.clear();
	gParts.clear();

	SplitString(graphCode, ".", thisParts);
	SplitString(g->GetGraphCode(), ".", gParts);

	if(thisParts.size() > gParts.size())
		return false;

	int counterThis=0;
	int gCounter=0;
	while(1)
	{
		if(gParts[gCounter] < thisParts[counterThis])
			gCounter += 2;
		else if(gParts[gCounter] > thisParts[counterThis])
		{
			return false;
		}
		else if(gParts[gCounter] == thisParts[counterThis])
		{
			if(gParts[gCounter+1] < thisParts[counterThis+1])
				gCounter += 2;
			else if(gParts[gCounter+1] > thisParts[counterThis+1])
			{
				return false;
			}
			else
				counterThis += 2;
		}
		if(counterThis >= thisParts.size())
			break;
		if(gCounter >= gParts.size())
			break;
	}
	if(counterThis < thisParts.size())
	{
		// std::cout << "Hi4" << '\n';
		return false;
	}

	// std::cout << "Hi4" << '\n';
	return true;
}

bool Graph::BIsLabelSubGraph(Graph *g)
{
	// cout << "WHAt" << endl;
	int thisGtype = GetGraphType();
	int gGtype = g->GetGraphType();
	
	if(thisGtype == GRAPH_EMPTY)
	{
		// cout << "Here -1" << endl;
		if(IGetGraphID() == g->IGetGraphID())
			return true;
		else
			return false;
	}
	else if(thisGtype == GRAPH_SINGLE_NODE)
	{
		// cout << "Here 0" << endl;
		int l1 = nodeLabels_Vi[atoi((char*)graphCode.c_str())];
		if(gGtype == GRAPH_SINGLE_NODE)
		{
			int l2 = g->nodeLabels_Vi[atoi((char*)g->GetGraphCode().c_str())];
			if(l1 == l2)
				return true;
			else
				return false;
		}
		else
		{
			vector<int> arr, labels;
			SplitString(g->GetGraphCode(), ".", arr);
			for(int i=0; i < arr.size(); i++)
				labels.push_back(g->nodeLabels_Vi[arr[i]]);

			for(int i=0; i < labels.size(); i++)
				if(labels[i] == l1)
					return true;
			return false;
		}
	}

	if(gGtype == GRAPH_SINGLE_NODE)
		return false;

	vector<int> thisParts, gParts;

	FindNewGraphCode();
	g->FindNewGraphCode();

	SplitString(newGraphCode, ".", thisParts);
	SplitString(g->GetNewGraphCode(), ".", gParts);

	if(thisParts.size() > gParts.size())
		return false;

	// vector<int>::iterator thisit, git;
	// thisit = thisParts.begin();
	// git = gParts.begin();

	// while(thisit!=thisParts.end()){
	// 	cout << *thisit << " ";
	// 	thisit++;
	// }
	// cout << endl;

	// while(git!=gParts.end()){
	// 	cout << *git << " ";
	// 	git++;
	// }
	// cout << endl;

	int counterThis=0, gCounter=0;
	while(1)
	{
		if(gParts[gCounter] < thisParts[counterThis]){
			gCounter += 3;
		}
		else if(gParts[gCounter] > thisParts[counterThis])
		{
			return false;
		}
		else if(gParts[gCounter] == thisParts[counterThis])
		{
			if(gParts[gCounter+1] < thisParts[counterThis+1])
				gCounter += 3;
			else if(gParts[gCounter+1] > thisParts[counterThis+1])
			{
				return false;
			}
			else if(gParts[gCounter+2] < thisParts[counterThis+2])
				gCounter += 3;
			else if(gParts[gCounter+2] > thisParts[counterThis+2])
			{
				return false;
			}
			else
			{
				counterThis += 3;
				gCounter += 3;
			}
		}
		if(counterThis >= thisParts.size())
			break;
		if(gCounter >= gParts.size())
			break;
	}
	if(counterThis < thisParts.size())
	{
		return false;
	}
	return true;
}

void Graph::FindNewGraphCode()
{
	vector<string> codes;
	edge_iterator ei, ei_end;
	map< int, vector<pair<int, int> > > int_codes;

	for (tie(ei, ei_end) = edges(G); ei != ei_end; ++ei)
	{

		int l1 = nodeLabels_Vi[IGetSourceNode(*ei)];
		int l2 = nodeLabels_Vi[IGetTargetNode(*ei)];
		int el = IGetEdgeLabel(*ei);
		if(l1 < l2){
			if(int_codes.find(l1) == int_codes.end())
			{
				vector<pair<int, int> >temp;
				int_codes[l1] = temp;
			}
			int_codes[l1].push_back(make_pair(el, l2));
		}
		else
		{
			if(int_codes.find(l2) == int_codes.end())
			{
				vector<pair<int, int> >temp;
				int_codes[l2] = temp;
			}
			int_codes[l2].push_back(make_pair(el, l1));
		}
	}

	map<int, vector<pair<int, int> > >:: iterator mit;
	mit = int_codes.begin();
	while(mit!=int_codes.end())
	{
		sort(mit->second.begin(), mit->second.end());
		mit++;
	}

	newGraphCode = "";	
	mit = int_codes.begin();
	while(mit!=int_codes.end())
	{
		for(int i=0;i<mit->second.size();i++){
			stringstream sstr;
			sstr << mit->first << "." << mit->second[i].first << "." << mit->second[i].second << ".";
			codes.push_back(sstr.str());
		}
		mit++;
	}

	for(int i=0; i<codes.size(); i++)
	{
		// cout << codes[i] << endl;
		newGraphCode += codes[i];
	}
}

// pair<pair<pair<int, int>, int>, int > Graph::check_if_MFS_present(pair<pair<int,int>, pair<int,int> > edge, vector< map< pair< pair<int, int>, int> , vector< int > > > all_hashes)
// {
//   node::adj_edges_iterator begin_aeIt;
//   node::adj_edges_iterator end_aeIt;
//   map< pair< pair<int, int>, int> , vector< int > >::iterator it;
//   pair<pair<int, int>, int> edge1;
//   pair<pair<pair<int, int>, int>, int > ans;
//   ans = make_pair(make_pair(make_pair(-1,-1), -1), -1);
//   int node1 = edge.first.first;
//   int node2 = edge.first.second;
//   int edge_label = edge.second.first;
//   int graph_id = edge.second.second;
//   Node nodea = allNodes_Vn[node1];
//   begin_aeIt = nodea.adj_edges_begin();
//   end_aeIt = nodea.adj_edges_end();
//   while(begin_aeIt != end_aeIt)
//   {
//       edge1 = make_pair(make_pair(nodeIDs_nMnid[(*begin_aeIt).source()], nodeIDs_nMnid[(*begin_aeIt).target()]), edgeLabel_eM[*begin_aeIt]);
//       it = all_hashes[graph_id].find(edge1);
//       if(it != all_hashes[graph_id].end())
//       {
//         for(int i=0; i<all_hashes[graph_id][edge1].size(); i++)
//         {
//             if(all_hashes[graph_id][edge1][i] > 0)
//             {
//               cout << "Found an MFS in this graph" << endl;
//               // cout << edge1.first.first << " " << edge1.first.second << " " << edge1.second << endl;
//               // cout << i << endl;
//               ans = make_pair(edge1, i);
//               return ans;
//             }
//         }
//       }
//       begin_aeIt ++;
//   }
//
//   Node nodeb = allNodes_Vn[node2];
//   begin_aeIt = nodeb.adj_edges_begin();
//   end_aeIt = nodeb.adj_edges_end();
//   while(begin_aeIt != end_aeIt)
//   {
//       edge1 = make_pair(make_pair(nodeIDs_nMnid[(*begin_aeIt).source()], nodeIDs_nMnid[(*begin_aeIt).target()]), edgeLabel_eM[*begin_aeIt]);
//       it = all_hashes[graph_id].find(edge1);
//       if(it != all_hashes[graph_id].end())
//       {
//         for(int i=0; i<all_hashes[graph_id][edge1].size(); i++)
//         {
//             if(all_hashes[graph_id][edge1][i] > 0)
//             {
//               cout << "Found an MFS in this graph" << endl;
//               // cout << edge1.first.first << " " << edge1.first.second << " " << edge1.second << endl;
//               // cout << i << endl;
//               ans = make_pair(edge1, i);
//               return ans;
//             }
//         }
//       }
//       begin_aeIt ++;
//   }
//   return ans;
// }
