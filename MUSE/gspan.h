#include <bits/stdc++.h>
#include "probabilityCalculation.h"
#define XX first
#define YY second
using namespace std;

vector <DFSTuple> getRightMostPathExtensions(DFSCode code, vector <Embedding> embeddings){
	map <DFSTuple, int> extensionFrequency;
	map <DFSTuple, int> extensionLast;
	vector <DFSTuple> result;
	int rmChild, u, gid, v, elabel, ind;
	bool flag;
	vector <int> path;
	DFSTuple extension;
	for(auto embedding: embeddings){
		rmChild=code.getRightMostChild();
		path=code.getRightMostPath();
		gid=embedding.gid;
		u=embedding.getMappedNode(rmChild);
		if(u==-1) continue;

		/*generate backward edges*/
		for(auto edgeID: database[gid].adj[u]){
			v=database[gid].edges[edgeID].getOtherNode(u);
			elabel=database[gid].edges[edgeID].label;
			flag=false;
			for(int i=0; i<(int)path.size()-2; i++){
				if(embedding.getMappedNode(path[i])==v){
					flag=true;
					ind=path[i];
					break;
				}
			}
			if(!flag){
				continue;
			}
			for(auto edge: embedding.edges){
				if(edge==edgeID){
					flag=false;
					break;
				}
			}
			if(!flag){
				continue;
			}
			extension=DFSTuple(rmChild, ind, database[gid].nodeLabel[u], elabel, database[gid].nodeLabel[v]);
			if(extensionLast[extension]<(gid+1)){
				extensionLast[extension]=gid+1;
				extensionFrequency[extension]++;
				if(extensionFrequency[extension]==min_sup){
					result.push_back(extension);
				}
			}
		}
		/*Generate forward extensions*/
		for(auto rmNode: path){
			u=embedding.getMappedNode(rmNode);
			if(u==-1) continue;
			for(auto edgeID: database[gid].adj[u]){
				v=database[gid].edges[edgeID].getOtherNode(u);
				elabel=database[gid].edges[edgeID].label;
				flag=true;
				for(auto node: embedding.nodesMapped){
					if(node==v){
						flag=false;
						break;
					}
				}
				if(!flag){
					continue;
				}
				extension=DFSTuple(rmNode, rmChild+1, database[gid].nodeLabel[u], elabel, database[gid].nodeLabel[v]);
				if(extensionLast[extension]<(gid+1)){
					extensionLast[extension]=gid+1;
					extensionFrequency[extension]++;
					if(extensionFrequency[extension]==min_sup){
						result.push_back(extension);
					}
				}
			}
		}
	}
	sort(result.begin(), result.end());
	return result;
}

DFSTuple getMinimumExtension(DFSCode code, vector <Embedding> embeddings, Graph graph){
	DFSTuple minExtension;
	int rmChild, u, v, elabel, ind;
	bool flag;
	vector <int> path;
	DFSTuple extension;
	bool isFirst=true;
	for(auto embedding: embeddings){
		rmChild=code.getRightMostChild();
		path=code.getRightMostPath();
		u=embedding.getMappedNode(rmChild);
		if(u==-1) continue;
		for(auto edgeID: graph.adj[u]){
			v=graph.edges[edgeID].getOtherNode(u);
			elabel=graph.edges[edgeID].label;
			flag=false;
			for(int i=0; i<(int)path.size()-2; i++){
				if(embedding.getMappedNode(path[i])==v){
					flag=true;
					ind=path[i];
					break;
				}
			}
			if(!flag){
				continue;
			}
			for(auto tuple: code.tuples){
				if(tuple.u>tuple.v){
					if(tuple.u==rmChild){
						if(tuple.v==ind){
							flag=false;
						}
					}
				}
			}
			if(!flag){
				continue;
			}
			extension=DFSTuple(rmChild, ind, graph.nodeLabel[u], elabel, graph.nodeLabel[v]);
			if(isFirst){
				minExtension=extension;
				isFirst=false;
			}
			else if(extension<minExtension){
				minExtension=extension;
			}
		}
		/*Generate forward extensions*/
		for(auto rmNode: path){
			u=embedding.getMappedNode(rmNode);
			if(u==-1) continue;
			for(auto edgeID: graph.adj[u]){
				v=graph.edges[edgeID].getOtherNode(u);
				elabel=graph.edges[edgeID].label;
				flag=true;
				for(auto node: embedding.nodesMapped){
					if(node==v){
						flag=false;
						break;
					}
				}
				if(!flag){
					continue;
				}
				extension=DFSTuple(rmNode, rmChild+1, graph.nodeLabel[u], elabel, graph.nodeLabel[v]);
				if(isFirst){
					minExtension=extension;
					isFirst=false;
				}
				else if(extension<minExtension){
					minExtension=extension;
				}
			}
		}
	}
	return minExtension;
}

bool isMin(DFSCode code){
	Graph graph=Graph(code);
	DFSTuple extension=graph.getMinTuple();
	DFSCode tempCode;
	vector <Embedding> embeddings;
	int sz=code.tuples.size();
	int cnt=0;
	for(auto tuple: code.tuples){
		if(extension<tuple){
			return false;
		}
		if(cnt>(sz-2)){
			return true;
		}
		cnt++;
		tempCode.tuples.push_back(extension);
		embeddings=graph.extendEmbeddings(0, embeddings, extension);
		extension=getMinimumExtension(tempCode, embeddings, graph);
	}
	return true;
}
int fsub=0;
void printSubgraphPattern(DFSCode code){
	printf("Subgraph pattern %d\n", fsub);
	for(auto tuple: code.tuples){
		printf("%d %d %d %d %d\n", tuple.u, tuple.v, nodeLabelOrder[tuple.label_u], edgeLabelOrder[tuple.eLabel], nodeLabelOrder[tuple.label_v]);
	}
}

void errorPrint(DFSCode code){
    cerr<<"****************\n";
    for(auto tuple: code.tuples){
        cerr<<tuple.u<<" "<<tuple.v<<" "<<nodeLabelOrder[tuple.label_u]<<" "<<edgeLabelOrder[tuple.eLabel]<<" "<<nodeLabelOrder[tuple.label_v]<<endl;
    }
    cerr<<"***************\n";

}

void gSpan(DFSCode code, vector <Embedding> embeddings){
    //errorPrint(code);
    vector < pair < vector <Embedding>, int > > slices;
    for(int i=0; i<(int)embeddings.size(); i++){
        if(i==0 || embeddings[i].gid!=embeddings[i-1].gid){
            slices.push_back({vector <Embedding> (), embeddings[i].gid});
        }
        slices[(int)slices.size()-1].XX.push_back(embeddings[i]);
    }
    //cerr<<"Slices built"<<endl;
    double sum_u=0, sum_l=0;
    pair <double, double> val;
    for(auto graph: slices){
        try{
            clock_t st_time=clock();
            val=evaluate(graph.XX);
            clock_t ed_time=clock();
            isomorphism_count++;
            if(RECORD_PERFORMANCE){
                fprintf(performance_log, "%d,%d\n", graph.XX.size(), ed_time-st_time);
                fflush(performance_log);
            }
        }
        catch(const std::bad_alloc& e){
            cerr<<"Out of memory exception thrown"<<endl;
            val={0, 0};
        }
        sum_l+=val.XX;
        sum_u+=val.YY;
        //if(sum_u>min_sup && sum_l>((1-EPS)*min_sup)) break;
    }
    //cerr<<"Evaluation done: "<<sum_u<<" "<<sum_l<<" "<<min_sup<<endl;
    if(sum_u<min_sup || sum_l<((1-EPS)*min_sup)) return;
    printSubgraphPattern(code);
    //printf("Expected support: %f %f\n", sum_l, sum_u);
    fsub++;
    vector <DFSTuple> extensions=getRightMostPathExtensions(code, embeddings);
    //cerr<<"Extensions found"<<" "<<extensions.size()<<endl;

	DFSCode newCode;
	for(auto extension: extensions){
		//cerr<<"Adding extension"<<endl;
		newCode=code;
		newCode.tuples.push_back(extension);
		//cerr<<"New code found"<<endl;
		if(!isMin(newCode)) continue;
		vector <Embedding> newEmbeddings, tempEmbeddings;
		for(auto embedding: embeddings){
			tempEmbeddings=database[embedding.gid].extendEmbeddings(embedding.gid, embedding, extension);
			for(auto tembedding: tempEmbeddings){
				newEmbeddings.push_back(tembedding);
			}
		}
		//cerr<<"Embeddings found"<<endl;
		gSpan(newCode, newEmbeddings);
	}
	//cerr<<"Returning\n"<<endl;
}

