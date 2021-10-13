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

double cost(vector <Embedding> embeddings){
    vector <vector <Embedding> > partitions;
    partitions=getJointEmbeddings(embeddings);
    double sum=0;
    for(auto jointEmbeddings: partitions){
        int x_i=jointEmbeddings.size();
        int e_s=jointEmbeddings[0].edges.size();
        double m_sup=(min_sup*1.0)/(database.size()*1.0);
        double threshold=(16*log(2.0/LAMBDA))/(EPS*EPS*m_sup*m_sup);
        long long challenger=e_s;
        int cnt=0;
        int flag=0;
        while(cnt<x_i){
            challenger*=2;
            if(challenger>threshold){
                flag=1;
                break;
            }
            cnt++;
        }
        if(flag) sum+=threshold;
        else sum+=challenger;
    }
    return sum;
}

double benefit(vector <Embedding> vec, double mn){
    double sum=0, prod;
    for(auto embedding: vec){
        prod=1;
        for(auto edge: embedding.edges){
            prod*=database[embedding.gid].edges[edge].p;
        }
        sum+=prod;
    }
    return min(mn, sum);
}

bool cmpCBR(pair < pair < vector <Embedding>, int >, double > a, pair < pair < vector <Embedding>, int >, double > b){
    return a.YY>b.YY;
}

void gSpan(DFSCode code, vector <Embedding> embeddings, vector <double> parentExpectedSupport){
    vector < pair < pair < vector <Embedding>, int >, double > > slices;
    vector < bool > inEmbeddings;
    inEmbeddings.resize(database.size(), false);
    for(int i=0; i<(int)embeddings.size(); i++){
        if(i==0 || embeddings[i].gid!=embeddings[i-1].gid){
            inEmbeddings[embeddings[i].gid]=true;
            slices.push_back({{vector <Embedding> (), embeddings[i].gid}, 0});
        }
        slices[(int)slices.size()-1].XX.XX.push_back(embeddings[i]);
    }
    if((int)slices.size()<min_sup){
        return;
    }
    for(auto slice: slices){
        slice.YY=benefit(slice.XX.XX, parentExpectedSupport[slice.XX.YY])/cost(slice.XX.XX);
    }
    sort(slices.begin(), slices.end(), cmpCBR);
    //cerr<<"Slices sorted"<<endl;
    vector <double> newExpectedSupport;
    double remaining_sum=0;
    int c=0;
    for(auto _upper: parentExpectedSupport){
        remaining_sum+=_upper;
        if(inEmbeddings[c]) newExpectedSupport.push_back(_upper);
        else newExpectedSupport.push_back(0);
        c++;
    }
    //cerr<<"Upper bounds pushed\n";
    double sum_u=0, sum_l=0;
    //try{
        pair <double, double> val={0, 0};
        int x=0;
        for(auto graph: slices){
            try{
                clock_t st_clock=clock();
                val=evaluate(graph.XX.XX);
                clock_t ed_clock=clock();
                isomorphism_count++;
                //fprintf(fp, "%d,%d\n", graph.XX.XX.size(), ((ed_clock-st_clock)));
            }
            catch(const std::bad_alloc& e){
                cerr<<"Out of memory exception thrown"<<endl;
                val={0, 0};
            }
            sum_l+=val.XX;
            sum_u+=val.YY;
            if(val.YY>0) newExpectedSupport[graph.XX.YY]=val.YY;
            else newExpectedSupport[graph.XX.YY]=1.0;
            if(graph.XX.YY<((int)database.size()-1) && remaining_sum+sum_u<min_sup) {
                return;
            }
            x++;
        //cerr<<x<<endl;

            remaining_sum-=parentExpectedSupport[graph.XX.YY];
        }
        if(sum_u<min_sup || sum_l<((1-EPS)*min_sup)) return;
    //}
    //catch(const std::bad_alloc& e){
    //    cerr<<"Out of memory exception thrown"<<endl;
    //}
    printSubgraphPattern(code);
    //printf("Expected support: %f %f\n", sum_l, sum_u);
    fsub++;
    vector <DFSTuple> extensions=getRightMostPathExtensions(code, embeddings);


	DFSCode newCode;
	for(auto extension: extensions){
		newCode=code;
		newCode.tuples.push_back(extension);
		if(!isMin(newCode)) continue;
		vector <Embedding> newEmbeddings, tempEmbeddings;
		for(auto embedding: embeddings){
			tempEmbeddings=database[embedding.gid].extendEmbeddings(embedding.gid, embedding, extension);
			for(auto tembedding: tempEmbeddings){
				newEmbeddings.push_back(tembedding);
			}
		}
		gSpan(newCode, newEmbeddings, newExpectedSupport);
	}
}

