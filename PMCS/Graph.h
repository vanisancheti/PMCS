/*
 * File Info: Graph.h
 * Description: This file contains the declaration of Graph class
 *
 * */

#ifndef _GRAPH_H_
#define _GRAPH_H_

/*
 * Node labels are integers.
 * "convert.pl" maps the string labels into consecutive integers
 * If there are 'n' nodes, then the node ids will be from 0 to n-1.
 *
 * Data Types:
 * Vi -> vector of integers
 * Vn -> vector of nodes
 * nM -> node map
 * nid -> node id
 * */

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <cassert>
#include <limits.h>

#include "decls.h" // declarations
#include "Triplet.h"

using namespace std;

void intialize_num();
int return_num_subgraphiso();
int return_num_iso();

struct knownGraphNode;
enum validity{InFrequent=0, Frequent=1, InValid=2, Valid=3};
void InitializeknownGraphNode(knownGraphNode* knownGraphNode, int r, int c);
bool findIfExist(map<int, map<int, knownGraphNode* > > &knownGraphsMap, int r, int c);

class Graph
{
    private:
        int graphID_i; // id of the graph

        BoostGraph G;
        // node information
        vector<Node> allNodes_Vn;       // array of all the nodes
        // map<Node, NodeID> nodeIDs_nMnid;  // mapping from node -> nodeID
        vector<int> nodeLabels_Vi;    // mapping from nodeIDs -> node label


        // edge_map<int> edgeLabel_eM;  // mapping from edge -> edgelabel

        // for node label 'i', labelNoOfIncidentEdges_Vi[i] gives the number
        // of edges which have a node with label 'i' as its end node
        vector<int> labelNoOfIncidentEdges_Vi; // number of incident edges on each of the node labels

        vector<int> labelFreqs_Vi;    // Frequency of each node label of 'this'
        vector<int> freqEdgeLabels_Vi; // frequency of edges of each edge label of 'this'

        int noOfValidNodes_i; // valid node: on hiding edges, this node is still a
                              //                    part of the connected subgraph
                              // keep a count of number of valid nodes
                              // so that to find whether the graph is empty or not
                              //            it would be a simple lookup
        vector<bool> isValidNode_Vb; // boolean array to represent if the node is valid
                                     // true -> node is valid;
                                     // false -> node is invalid

        vector<HiddenEdge> hiddenEdges_Ve;    // array of all hidden edges

        vector<map<int, bool> > nodeHiddenEdges_Vm;
        /* nodeHiddenEdges_Vm[i] will give a "SET" of pairs(X, Y) {lini: vector of vector of pairs}
         *   - where X is the index of the edge in hiddenEdges_Ve which is hidden
         *   -       Y is the boolean which denotes whether its to or from
         *                  Y=true => edge from node ID i
         *                  Y=false => edge to node ID i
         */
        /* Note: Since the graph is undirected, an edge's entry will be
         * stored twice. If the edge is edge(A,B) then it will be stored in
         * both nodeHiddenEdges_Vm[A] and nodeHiddenEdges_Vm[B]
         */

        // To check if you are visiting the same subgraph of same G_i again
        string graphCode;

        string newGraphCode;

        int graphDegreeCode;

        // Information of the latest edge removed or added when a parent or
        // child of 'this' is found. edgeAddedOrRemovedsource and
        // edgeAddedOrRemovedtarget are the node ids of the source and
        // target of the edge added or removed.
        int edgeAddedOrRemovedsource, edgeAddedOrRemovedtarget;
        // Due to edge addition, an invalid node becomes valid or
        //        edge deletion, a valid node becomes valid,
        //        you remember that node id
        int nodeAddedOrRemoved;

        // Method to be called only by BIsSubGraphOf()
        // Given a graph 'super_g'(some G_i) and the present graph 'this', this method
        // returns true if 'this' is a subgraph of 'super_g'. The starting
        // nodes for the isomorphism are 'subCurrent_n' for 'this' and
        // 'superCurrent_n' for 'super_g'. This is a recursive function
        // which searches among the 'toBeVisited_Se' of 'this'.
        bool BCanBeMatchedWith_all_embeddings(Graph *super_g, Node subCurrent_n, Node superCurrent_n,
                              map<int, int> subToSuper_m, map<int, int> superToSub_m,
                              set<Edge> visited_Se, set<Edge> toBeVisited_Se, vector< pair< map<int,int>, map<int, int> > > &store, Edge edge);

        bool BCanBeMatchedWith(Graph *super_g, Node subCurrent_n, Node superCurrent_n,
                              map<int, int> subToSuper_m, map<int, int> superToSub_m,
                              set<Edge> visited_Se, set<Edge> toBeVisited_Se);


        // Method to populate the vector 'labelNoOfIncidentEdges_Vi'
        void FindNoOfIncidentEdges();

        // Method to prune 'this' not being subgraph of 'super_g' using
        // the vector 'labelNoOfIncidentEdges_Vi' (each entry of
        /*
         * Prunes 'this' not being a subgraph of 'super_g' if
         * i. the entries of labelNoOfIncidentEdges_Vi is greater than that
         * of super_g
         * ii. the entries of freqEdgeLabels_Vi is greater than that of
         * super_g
         * */
        bool BCanMatchLabelsIncidentEdges(Graph *super_g);


    public:

        // constructors
        Graph(); // empty constructors

        // destructor: Free memory
        ~Graph();
        void GraphClear();
        void build_hash(int start, pair< pair<int,int>, pair<int,int> > edge, map<int, vector< pair< map<int, int>, map<int, int> > > > &main_storage,
                              vector<Graph*> maxFreq, vector<bool> validMaxFreq, vector<bool> newMaxFreq);
        void build_hash_delete(int start, map<int, vector< pair< map<int, int>, map<int, int> > > > &main_storage,
                              vector<Graph*> maxFreq, vector<bool> validMaxFreq, vector<bool> newMaxFreq);
        // edge_n1, edge_n2 is freed at the end of the loading
        bool BLoadGraph(int graphID_i, vector<int> node_ids, vector<int> node_labs,
                        vector<int> edge_n1, vector<int> edge_n2, vector<int> edge_labs);
        bool BLoadNodes(vector<int> node_ids, vector<int> node_labs);
        bool BLoadEdges(vector<int> edge_n1, vector<int> edge_n2, vector<int> edge_labs);
        bool BInsertEdge(int node1, int node2, int edge_label);
        bool BDeleteEdge(int node1, int node2, int edge_label);
        int BInsertNode(int label);
        // Make a copy of 'this' into 'g' (to create subgraphs by hiding edges or supergraphs by unhiding edges)
        void Copy(Graph *g);
        void ShallowCopy(Graph* g);
        void ExactgraphCopy(Graph* g);

        // Print the graph class
        void Print();
        void PrintFile_gSpan(char *fileName); // Printing in format for gSpan
        void PrintFile(char *fileName);
        void PrintDebug(); // used only for debug purposes
        void PrintNode(Node nd);
        void PrintNode(NodeID ndID);
        void PrintEdge(Edge ed);
        void PrintEdge(HiddenEdge ed);
        void PrintGraphType();

        // Set Functions
        void SetGraphID(int graphID_i) { this->graphID_i = graphID_i;}
        int SetLabelFreq(int index_i, int freq_i) { labelFreqs_Vi[index_i]=freq_i; }
        void SetAsValidNode(NodeID nodeID) { isValidNode_Vb[nodeID]=true;
                                          labelFreqs_Vi[nodeLabels_Vi[nodeID]]++;
                                          noOfValidNodes_i++; }
        void SetAsInvalidNode(NodeID nodeID) { isValidNode_Vb[nodeID]=false;
                                            labelFreqs_Vi[nodeLabels_Vi[nodeID]]--;
                                            noOfValidNodes_i--; }
        void SetAsInvalidNode(Node node_n) { vertex_index v_index = boost::get(boost::vertex_index_t(), G);
                                             SetAsInvalidNode(v_index[node_n]); }
        void SetEdgeAddedOrRemoved(Edge e) { vertex_index v_index = boost::get(boost::vertex_index_t(), G);
                                              edgeAddedOrRemovedsource = v_index[boost::source(e, G)];
                                             edgeAddedOrRemovedtarget = v_index[boost::target(e, G)]; }
        void SetEdgeAddedOrRemoved(int s, int t) { edgeAddedOrRemovedsource = s;
                                                   edgeAddedOrRemovedtarget = t; }
        void SetNodeAddedOrRemoved(Node n) { vertex_index v_index = boost::get(boost::vertex_index_t(), G);
                                              nodeAddedOrRemoved = v_index[n]; }
        void SetNodeAddedOrRemoved(NodeID n) { nodeAddedOrRemoved = n; }
        void StoreEdgeAddedOrRemoved(int *sourceID, int *targetID)
        {
            int GraphType = GetGraphType();
            if( GraphType == GRAPH_EMPTY)
            {
                (*sourceID) = nodeAddedOrRemoved;
                (*targetID) = -1;
            }
            else
            {
                (*sourceID) = edgeAddedOrRemovedsource;
                (*targetID) = edgeAddedOrRemovedtarget;
            }
        }

        void SetGraphCode(string s) { graphCode = s; }




        /**************************************/
        // Get Functions
        // 'I' -> returns integer
        // 'E' -> return edge
        void experiment();
        string GetGraphCode() { return graphCode; }
        string GetNewGraphCode() { return newGraphCode; }
        int GetGraphDegreeCode() { return graphDegreeCode; }
        BoostGraph IGetBoostGraph() {return G; }
        int IGetGraphID() { return graphID_i; }
        int IGetNodeLabel(NodeID ndID) { return nodeLabels_Vi[ndID]; }
        int IGetNodeLabel(Node nd) { vertex_color v_color = boost::get(boost::vertex_color_t(), G);
          return v_color[nd]; }
        int IGetNoOfAllNodes() { return num_vertices(G); } // number of nodes of G_i
        int IGetNoOfAllEdges() { return num_edges(G); } // number of edges in G_i
        NodeID IGetNodeID(Node nd) { vertex_index v_index = boost::get(boost::vertex_index_t(), G);
          return v_index[nd]; }
        int IGetLabelFreq(int index_i);
        int IGetSizeLabelFreqs() { return labelFreqs_Vi.size(); } // number of node labels
        int IGetNoOfValidNodes() { return noOfValidNodes_i; } // number of actual nodes in 'this'
        int IGetNoOfHiddenEdges() { return hiddenEdges_Ve.size(); }
        NodeID IGetTheSingleNode(); // Returns the id of the valid node when 'this' is a singleton graph
        int IGetEdgeLabel(Edge ed) {edge_color e_color = boost::get(boost::edge_color_t(), G);
          return e_color[ed]; }
        bool HasEdge(int source_i, int target_i);
        bool HasEdgeLabelCheck(int source_label, int target_label, int edge_label);
        int IGetSourceNode(Edge e) {vertex_index v_index = boost::get(boost::vertex_index_t(), G);
          return v_index[boost::source(e, G)]; }
        int IGetTargetNode(Edge e) {vertex_index v_index = boost::get(boost::vertex_index_t(), G);
          return v_index[boost::target(e, G)]; }
        int IGetDegree(Node n) { return boost::out_degree(n, G); }
        pair<edge_iterator, edge_iterator> PEGetAllEdges() {return edges(G); }
        pair<vertex_iterator, vertex_iterator> PEGetVertices() {return vertices(G); }
        int PEGetAllVertices() {return num_vertices(G);}
        Edge GetTheOnlyIncidentEdge(Node n);
        Node GetOppositeEdge(Edge e, Node n) {
          if ( n == boost::source(e, G))
          {
            return boost::target(e, G);
          }
          else if(n == boost::target(e, G))
          {
            return boost::source(e, G);
          }
          return n;
        } // opposite is an in-built function of GTL

        int IGetLabelIncidentNoOfEdges(int lab_i){ return labelNoOfIncidentEdges_Vi[lab_i]; }
        // IGetSizeLabelIncidentNoOfEdges returns number of node labels in G_i
        int IGetSizeLabelIncidentNoOfEdges() { return labelNoOfIncidentEdges_Vi.size(); }

        Node NGetNodeWithID(NodeID ndID) { return allNodes_Vn[ndID]; }

        // IGetSizeFreqEdgeLabels returns the number of edge labels in G_i
        int IGetSizeFreqEdgeLabels() { return freqEdgeLabels_Vi.size(); }
        int IGetFreqEdgeLabel(int i) { return freqEdgeLabels_Vi[i]; }

        // Checks if the edge is entered as (s,t)
        Edge EGetEdge(int source_i, int target_i);
        // To check if the edge is entered as (s,t) or (t,s)
        Edge EGetEdgeUndirected(int source_i, int target_i);

        HiddenEdge EGetHiddenEdge(int index) { return hiddenEdges_Ve[index]; }
        HiddenEdge EGetHiddenEdge(NodeID source_nID, NodeID target_nID);
        HiddenEdge EGetHiddenEdgeUndirected(NodeID source_nID, NodeID target_nID);

        // Returns all the nodes which have node label 'label_i'
        vector<Node> VNGetNodesWithLabel(int label_i);
        /*
        * Given a node 'cur_n', a node label 'label_i' and an edge label
        * 'thisedgeLabel_i', return all the nodes which have label_i incident
        * on 'cur_n' with edge label 'thisedgeLabel_i'
        * */
        vector<Node> VNGetNodesAdjToWithLabel(Node cur_n, int label_i,int thisedgeLabel_i);
        GraphType GetGraphType();
        int GetNodeAddedOrRemoved() { return nodeAddedOrRemoved; }
        int GetEdgeAddRemoveSource() { return edgeAddedOrRemovedsource; }
        int GetEdgeAddRemoveTarget() { return edgeAddedOrRemovedtarget; }
        int GetNodeAddRemove() { return nodeAddedOrRemoved; }


        // When an edge is unhidden and an invalid node becomes valid, to
        // form 'this' out of the copied previous version, the label freq is
        // updated.
        void IncrementLabelFreq(NodeID nodeID) { labelFreqs_Vi[nodeLabels_Vi[nodeID]]++; }
        void DecrementLabelFreq(NodeID nodeID) { labelFreqs_Vi[nodeLabels_Vi[nodeID]]--; }

        // Check Functions
        bool BIsValidNode(NodeID ndID);
        bool BIsValidNode(Node nd);
        // To check if 'this' is a G_i
        bool BItIsNotG_i();

        bool BIsThisEdgeExisting(int source_i, int target_i,int label_i);
        bool BIsThisEdgeHidingUndirected(int source_i, int target_i);

        // By hiding edge ed, 'this' remains connected, then it returns
        // true. otherwise returns false
        bool BCanHideEdge(Edge ed);

        // On hiding edge 'ed', to check if 'this' remains connected, we
        // start the breadth first search from one of the node incident on
        // ed which is ed.source() and try to find the other incident node ed.target()
        bool BBreadFirstSearch(Node start_nID, Node search_nID, vector<NodeID> &visitedNodes_nID);
	      bool BBreadFirst(Node start_n, vector<NodeID> visitedNodes_nID);

        // If BCanHideEdge returns true, then hides edge and updates the
        // appropriate datastructures
        void HideEdge(Edge ed,bool check_b=true);
        // Method to hide edge when single edge is present in the graph.
        // 'nd' is the node which remains valid after hiding 'ed'
        void HideEdge(Edge ed, Node nd);
        // Method to restore the edge
        void RestoreEdge(HiddenEdge ed);

        // Get Components from this graph by first removing the edges that are not present in both this 
        // as well as g graph.
        // This is done to reduce the MCS computation
        vector<Graph*> GetComponents(Graph *g);

        // Each edge in the hiddenEdges_Ve is searched to see atleast one of the
        // nodes incident on them is valid
        vector<Graph*> GetParents(int *noOfParents);

        // When 'this' is a single node, method to get parents
        // Using nodeHiddenEdges_Vm, we can find all edges incident on the
        // single node avoiding access on hiddenEdges_Ve
        vector<Graph*> GetParentsOfSingleNodeGraph(int *noOfParents);

        // When 'this' is a empty, method to get parents
        vector<Graph*> GetParentsOfEmptyGraph(int *noOfParents);

        //
        void GetOnlyOneParent(Graph *parent_Pg);

        vector<Graph*> GetChildren(int *noOfChildren);
        vector<Graph*> GetChildrenOfSingleNodeGraph(int *noOfChildren);
        // If the graph has single edge, it has to be hidden using
        // HideEdge(Edge, Node)
        vector<Graph*> GetChildrenOfSingleEdgeGraph(int *noOfChildren);
        void GetOnlyOneChild(Graph *child_Pg);
        // For Bottom-Up representative finding, we need to start with an
        // empty graph
        void GetEmptyChild(Graph *child_Pg);

        bool IsEmpty()
        {
            if(GetGraphType() == GRAPH_EMPTY)
                return true;
            return false;
        }

        Node NGetStartNodeForMatching(Graph *g); // heuristic to choose the first matching node
        Node NGetFirstValidNode(); // no heuristic to choose the first matching node

        bool BIsSubGraphOf(Graph *g);
        bool BIsSuperGraphOf(Graph *g);
        bool BIsIsomorphicTo(Graph *g);

        bool BIsExactlyEqualTo(Graph *g);
        // 'this' is the exact subgraph (embedded subgraph) of 'g
        bool BIsCodeSubGraphOf(Graph *g);
        bool BIsLabelSubGraph(Graph *g);

        void FindGraphDegreeCode();
        void FindGraphCode();

        bool BIsThisEdgeHidden(NodeID source_nID, NodeID target_nID);
        // int IsExtendable(int node1, int node2, int edge_label);
        pair<map<int,int>, map<int,int> > get_one_embedding(Graph *super_g);
        vector<pair<map<int,int>, map<int, int> > > get_all_embeddings(Graph *g);
    
        bool GetUnion(Graph *g);
        bool GetSoftUnion(Graph *g);
        int ICanBeMatchedWith(Graph *g, Node subCurrent_n, Node superCurrent_n,map<int, int> &subToSuper_m, map<int, int> &superToSub_m,set<Edge> &visited_Se, set<Edge> &toBeVisited_Se);
        
        // Haven't called this in expand_cut
        // this function can be called only when g1 and g2 are children of a common
        // super graph and the common subgraph is connected(lower diamond property)
        void GetCommonParent(Graph *g1, Graph *g2);
        bool BGetCommonParentSingleNodes(Graph *g1, Graph *g2);
        bool GetCommonChild(Graph *g1, Graph *g2);

        int GetFirstFreqParent(int noOfGraphDB, Graph *graphDB,
                                        map<int, map<int, knownGraphNode*> > &knownGraphsMap,
                                        int minSup, vector <Graph*> *ansGraphs, vector<int> *toBeAdded);
        int GetFirstFreqParentWithEmbeddings(int noOfGraphDB, Graph *graphDB,
                                        		vector < vector < vector<Graph*> > > &knownFreqGraphs,
                                        		vector < vector < vector<Graph*> > > &knownInfreqGraphs,
                                        		vector < vector < vector<int> > > &knownFreqGraphs_FreqVal,
                                        		vector < vector < vector<Embedding> > > &knownFreqGraphEmbeddings,
                                        		vector < vector < vector<Embedding> > > &knownInFreqGraphEmbeddings,
                                        		int minSup, vector <Graph*> *ansGraphs, vector<int> *toBeAdded, Embedding f_embedding, bool shouldCount);

        int GetFirstFreqParentOfSingleNodeGraph(int noOfGraphDB, Graph *graphDB,
                                        map<int, map<int, knownGraphNode*> > &knownGraphsMap,
                                        int minSup, vector <Graph*> *ansGraphs, vector<int> *toBeAdded);
        int GetFirstFreqParentOfSingleNodeGraphWithEmbeddings(int noOfGraphDB, Graph *graphDB,
                                        		vector < vector < vector<Graph*> > > &knownFreqGraphs,
                                        		vector < vector < vector<Graph*> > > &knownInfreqGraphs,
                                            vector < vector < vector<int> > > &knownFreqGraphs_FreqVal,
                                        		vector < vector < vector<Embedding> > > &knownFreqGraphEmbeddings,
                                        		vector < vector < vector<Embedding> > > &knownInFreqGraphEmbeddings,
                                        		int minSup, vector <Graph*> *ansGraphs, vector<int> *toBeAdded,bool shouldCount);
        int GetFirstFreqParentOfEmptyGraph(int noOfGraphDB, Graph *graphDB,
                                        map<int, map<int, knownGraphNode*> > &knownGraphsMap,
                                        int minSup, vector <Graph*> *ansGraphs, vector<int> *toBeAdded);

        bool GetFirstFreqChild(int noOfGraphDB, Graph *graphDB,
                                   vector < vector < vector<Graph*> > > knownFreqGraphs,
                                   vector < vector < vector<Graph*> > > knownInfreqGraphs,
                                   int minSup, Graph **ansGraph, int *toBeAdded, vector<Graph*> *inFreqFound);
        bool GetFirstFreqChildOfSingleNodeGraph(int noOfGraphDB, Graph *graphDB,
                                   vector < vector < vector<Graph*> > > knownFreqGraphs,
                                   vector < vector < vector<Graph*> > > knownInfreqGraphs,
                                   int minSup, Graph **ansGraph, int *toBeAdded, vector<Graph*> *inFreqFound);
        bool GetFirstFreqChildOfSingleEdgeGraph(int noOfGraphDB, Graph *graphDB,
                                   vector < vector < vector<Graph*> > > knownFreqGraphs,
                                   vector < vector < vector<Graph*> > > knownInfreqGraphs,
                                   int minSup, Graph **ansGraph, int *toBeAdded, vector<Graph*> *inFreqFound);
        void PrintHiddenEdgesCode();
        void FindNewGraphCode();
        pair<out_edge_iterator, out_edge_iterator> GetEdges(int node_id);
        void DFS(int ori_source_id, vector<int> &visited, vector<int> &nodeIds, Graph *g);
        // void MaxExtend(int node1, int node2, int edge_label, int flag);
        // pair<pair<pair<int, int>, int>, int > check_if_MFS_present(pair<pair<int,int>, pair<int,int> > edge, vector< map< pair< pair<int, int>, int> , vector< int > > > all_hashes);

};

struct knownGraphNode{
  ~knownGraphNode() {};

	int noOfEdges;
	int noOfNodes;

	vector<Graph*> knownFreqGraphs;
	vector<int> knownFreqGraphs_FreqVal;
	vector<Graph*> knownInfreqGraphs;
  vector<Graph*> knownGraphs;
	vector<Embedding> knownFreqGraphEmbeddings;
	vector<Embedding> knownInFreqGraphEmbeddings;

	int knownFreqGraphs_startindex;
	int knownFreqGraphs_FreqVal_startindex;
	int knownInfreqGraphs_startindex;
	int knownFreqGraphEmbeddings_startindex;
	int knownInFreqGraphEmbeddings_startindex;
	// Child. having one more edge then the current 
	// knownGraphNode* childleft; // n number of nodes, m+1 edge 
    // knownGraphNode* childright; // n+1 number of nodes, m+1 edge

	// // Parent. having one less edge then the current
	// knownGraphNode* parentleft; // n-1 number of nodes, m-1 edge
    // knownGraphNode* parentright; // n+1 number of nodes, m-1 edge
};

#endif
