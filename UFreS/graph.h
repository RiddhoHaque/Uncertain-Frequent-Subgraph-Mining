#include "dfsCode.h"
#include "embedding.h"
#include "edge.h"
#include "frequentLabels.h"
using namespace std;

struct Graph{
	vector < int > nodeLabel;
	vector < vector < int > > nodesWithLabel;
	vector < Edge > edges;
	vector < vector < int > > adj;

	vector <Embedding> extendInitEdge(int gid, int label_u, int elabel, int label_v){
		int sum_u=0;
		int sum_v=0;
		int v;
		for(auto u: nodesWithLabel[label_u]){
			sum_u+=(int)adj[u].size();
		}
		for(auto v: nodesWithLabel[label_v]){
			sum_v+=(int)adj[v].size();
		}
		bool swapped=false;
		if(sum_u>sum_v){
			swap(label_u, label_v);
			swapped=true;
		}

		Embedding embedding;
		vector <Embedding> embeddings;
		for(auto u: nodesWithLabel[label_u]){
			for(auto eid: adj[u]){
				v=edges[eid].getOtherNode(u);
				if(nodeLabel[v]==label_v && edges[eid].label==elabel){
					if(swapped) embedding=Embedding(gid, eid, v, u);
					else  embedding=Embedding(gid, eid, u, v);
					embeddings.push_back(embedding);
				}
			}
		}
		return embeddings;
	}

	vector <Embedding> extendBackEdge(int gid, Embedding embedding, DFSTuple tuple){
		vector <Embedding> embeddings;
		int u=embedding.getMappedNode(tuple.u);
		int v=embedding.getMappedNode(tuple.v);
		if(adj[u].size()>adj[v].size()) swap(u, v);
		for(auto eid: adj[u]){
			if(edges[eid].getOtherNode(u)==v && edges[eid].label==tuple.eLabel){
				embedding.addEdge(eid);
				embeddings.push_back(embedding);
				return embeddings;
			}
		}
		return embeddings;
	}

	vector <Embedding> extendForwardEdge(int gid, Embedding embedding, DFSTuple tuple){
		//cout<<"Extending forward edge"<<endl;
		vector <Embedding> embeddings;
		int u=embedding.getMappedNode(tuple.u);
		bool flag;
		Embedding newEmbedding;
		for(auto eid: adj[u]){
			if(nodeLabel[edges[eid].getOtherNode(u)]==tuple.label_v && edges[eid].label==tuple.eLabel){
				flag=true;
				for(auto node: embedding.nodesMapped){
					if(node==edges[eid].getOtherNode(u)) flag=false;
				}
				if(!flag) continue;
				newEmbedding=embedding;
				newEmbedding.addEdge(eid, edges[eid].getOtherNode(u));
				embeddings.push_back(newEmbedding);
			}
		}
		return embeddings;
	}

	vector <Embedding> extendEmbeddings(int gid, Embedding embedding, DFSTuple tuple){
		if(tuple.u==0 && tuple.v==1){
			return extendInitEdge(gid, tuple.label_u, tuple.eLabel, tuple.label_v);
		}
		vector <Embedding> resultingEmbeddings;
		if(tuple.u>tuple.v){
			resultingEmbeddings=extendBackEdge(gid, embedding, tuple);
		}
		else resultingEmbeddings=extendForwardEdge(gid, embedding, tuple);
		return resultingEmbeddings;
	}

	vector <Embedding> extendEmbeddings(int gid, vector <Embedding> embeddings, DFSTuple tuple){
		if(tuple.u==0 && tuple.v==1){
			return extendInitEdge(gid, tuple.label_u, tuple.eLabel, tuple.label_v);
		}
		vector <Embedding> resultingEmbeddings, tempEmbeddings;
		for(auto embedding: embeddings){
			if(tuple.u>tuple.v){
				tempEmbeddings=extendBackEdge(gid, embedding, tuple);
			}
			else tempEmbeddings=extendForwardEdge(gid, embedding, tuple);
			for(auto embedding: tempEmbeddings){
				resultingEmbeddings.push_back(embedding);
			}
		}
		return resultingEmbeddings;
	}

	inline void updateMinTuple(DFSTuple tuple, DFSTuple &minTuple, bool &isFirst){
		if(isFirst || tuple<minTuple){
			isFirst=false;
			minTuple=tuple;
		}
	}

	DFSTuple getMinTuple(){
		DFSTuple minTuple, tuple;
		bool isFirst=true;
		for(auto edge: edges){
			//cout<<"Considering edge "<<edge.u<<" "<<edge.label<<" "<<edge.v<<endl;
			tuple=DFSTuple(0, 1, nodeLabel[edge.u], edge.label, nodeLabel[edge.v]);
			updateMinTuple(tuple, minTuple, isFirst);
			tuple=DFSTuple(0, 1, nodeLabel[edge.v], edge.label, nodeLabel[edge.u]);
			updateMinTuple(tuple, minTuple, isFirst);
		}
		//minTuple.print();
		return minTuple;
	}

	bool notIn(int node, int nlabel){
		for(auto v: nodesWithLabel[nlabel]){
			if(v==node) return false;
		}
		return true;
	}

	Graph(DFSCode code){
		int mxNode=0, mxNodeLabel=0;
		for(auto tuple: code.tuples){
			mxNode=max(tuple.u, mxNode);
			mxNode=max(tuple.v, mxNode);
			mxNodeLabel=max(mxNodeLabel, tuple.label_u);
			mxNodeLabel=max(mxNodeLabel, tuple.label_v);
		}
		nodeLabel.resize(mxNode+1);
		nodesWithLabel.resize(mxNodeLabel+1, vector <int> ());
		adj.resize(mxNode+1, vector <int> ());
		for(auto tuple: code.tuples){
			nodeLabel[tuple.u]=tuple.label_u;
			nodeLabel[tuple.v]=tuple.label_v;
			if(notIn(tuple.u, tuple.label_u)) nodesWithLabel[tuple.label_u].push_back(tuple.u);
			if(notIn(tuple.v, tuple.label_v)) nodesWithLabel[tuple.label_v].push_back(tuple.v);
			edges.push_back(Edge(tuple.u, tuple.v, tuple.eLabel, 1));
			adj[tuple.u].push_back((int)edges.size()-1);
			adj[tuple.v].push_back((int)edges.size()-1);
		}
	}

	Graph(){
		nodesWithLabel.resize(nodeLabelOrder.size(), vector <int>());
	}

	void deleteTupleWithNodeLabel(int label_u, int elabel, int label_v){
		for(auto u: nodesWithLabel[label_u]){
			int cnt=0;
			for(auto edge_id: adj[u]){
				if(edges[edge_id].label==elabel && nodeLabel[edges[edge_id].getOtherNode(u)]==label_v){
					continue;
				}
				adj[u][cnt]=edge_id;
				cnt++;
			}
			while((int)adj[u].size()>cnt) adj[u].pop_back();
		}
	}

	void deleteTuple(int label_u, int elabel, int label_v){
		deleteTupleWithNodeLabel(label_u, elabel, label_v);
		deleteTupleWithNodeLabel(label_v, elabel, label_u);
	}
};

