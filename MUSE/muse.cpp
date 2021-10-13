#include <bits/stdc++.h>
#include "gspan.h"
#define XX first
#define YY second
using namespace std;
void constructGraphs(){
	FILE* fp=fopen(INPUTFILE, "r");
	char str[5];
	int u, v, label, label_u, label_v;
	double p;
	int cnt=-1;
	int sz;
	bool isFirst;
	int maxNode;
	while(fscanf(fp, "%s", str)!=EOF){
		if(str[0]=='t'){
			fscanf(fp, "%s %d", str, &v);
			if(v==-1) break;
			database.push_back(Graph());
			cnt++;
			isFirst=true;
			maxNode=0;
		}
		else if(str[0]=='v'){
			fscanf(fp, "%d %d", &v, &label);
			label=relabelledNodeLabels[label];
			maxNode=max(maxNode, v);
			if(label==-1) continue;
			database[cnt].nodesWithLabel[label].push_back(v);
			if(v==(int)database[cnt].nodeLabel.size()){
				database[cnt].nodeLabel.push_back(label);
			}
			else if(v>(int)database[cnt].nodeLabel.size()){
				database[cnt].nodeLabel.resize(v+1, -1);
				database[cnt].nodeLabel[v]=label;
			}
			else{
				database[cnt].nodeLabel[v]=label;
			}
		}
		else if(str[0]=='e'){
			if(isFirst){
				database[cnt].adj.resize(maxNode+1, vector <int>());
				if((int)database[cnt].nodeLabel.size()<=maxNode){
					database[cnt].nodeLabel.resize(maxNode+1, -1);
				}
				isFirst=false;
			}
			fscanf(fp, "%d %d %d %lf", &u, &v, &label, &p);
			label=relabelledEdgeLabels[label];
			label_u=database[cnt].nodeLabel[u];
			label_v=database[cnt].nodeLabel[v];
			if(label==-1) continue;
			if(label_u==-1) continue;
			if(label_v==-1) continue;
			if(frequentTuples.find({{nodeLabelOrder[label_u], edgeLabelOrder[label]}, nodeLabelOrder[label_v]})==frequentTuples.end()) continue;
			database[cnt].edges.push_back(Edge(u, v, label, p));
			sz=(int)database[cnt].edges.size();
			database[cnt].adj[u].push_back(sz-1);
			database[cnt].adj[v].push_back(sz-1);
        }
	}
	fclose(fp);
}


void test(clock_t t){
	/*cout<<"No of graphs="<<no_of_graphs<<endl;
	cout<<"Node Labels="<<maxNodeLabel+1<<endl;
	cout<<"Edge Labels="<<maxEdgeLabel+1<<endl;
	cout<<"Min sup="<<min_sup<<endl;
	cout<<"Node Labels sorted: ";
	for(auto label: nodeLabelOrder){
		cout<<label<<" ";
	}
	cout<<endl;
	cout<<"Edge Labels sorted: ";
	for(auto label: edgeLabelOrder){
		cout<<label<<" ";
	}
	cout<<endl;*/
	cerr<<"Time taken: "<<(1.0*t)/(1.0*CLOCKS_PER_SEC)<<endl;
	cerr<<"Frequent Subgraphs mined: "<<fsub<<endl;
	cerr<<"Isomorphisms: "<<isomorphism_count<<endl;
}


int main(){
    freopen(OUTPUTFILE, "w", stdout);
    clock_t st_time=clock();
	find_number_of_graphs_and_labels();
	cerr<<"Scan 1 complete\n";
	find_frequent_labels();
	cerr<<"Scan 2 complete\n";
	constructGraphs();
	cerr<<"Scan 3 complete\n";
	int cnt;
	vector <Embedding> embeddings, tempEmbeddings;
	vector <double> parentExpectedSupport;
	vector <DFSTuple> tuples;
	for(auto tuple: frequentTuples){
		DFSTuple dtuple=DFSTuple(0, 1, relabelledNodeLabels[tuple.XX.XX], relabelledEdgeLabels[tuple.XX.YY], relabelledNodeLabels[tuple.YY]);
		tuples.push_back(dtuple);
	}
	sort(tuples.begin(), tuples.end());
	for(auto tuple: tuples){
        //cerr<<"Loop for root tuple "<<tuple.label_u<<" "<<tuple.label_v<<" "<<tuple.eLabel<<endl;
		embeddings.clear();
		cnt=0;
		for(auto graph: database){
			//cerr<<"graph "<<cnt<<endl;
			tempEmbeddings=graph.extendInitEdge(cnt, tuple.label_u, tuple.eLabel, tuple.label_v);
			//cerr<<"Returned\n";
			cnt++;
			for(auto embedding: tempEmbeddings){
				embeddings.push_back(embedding);
			}
		}
        //cerr<<"New embeddings found"<<endl;
		DFSCode code;
		code.tuples.push_back(tuple);
		if(!isMin(code)) continue;
		gSpan(code, embeddings);
		//cerr<<"Got it"<<endl;
		for(auto graph: database){
			graph.deleteTuple(tuple.label_u, tuple.eLabel, tuple.label_v);
		}
		//cerr<<"Tuple deleted"<<endl;
	}
	clock_t ed_time=clock();
	test(ed_time-st_time);
	return 0;
}
