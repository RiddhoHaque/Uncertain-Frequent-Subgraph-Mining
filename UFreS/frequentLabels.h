#include <bits/stdc++.h>
#define INPUTFILE "AIDS_exp_4.0_Sampled.txt"
#define OUTPUTFILE "out.txt"
#define MIN_SUP 0.35
#define PERFORMANCE_FILE "performance_ufres_aids_exp_4.0.csv"
using namespace std;
FILE* fp;
int min_sup;
int isomorphism_count=0;
int maxEdgeLabel;
int maxNodeLabel;
int no_of_graphs;

void find_number_of_graphs_and_labels(){
	FILE* fp=fopen(INPUTFILE, "r");
	char str[5];
	int u, v, label;
	double p;
	while(fscanf(fp, "%s", str)!=EOF){
		if(str[0]=='t'){
			fscanf(fp, "%s %d", str, &v);
			if(v==-1) break;
			no_of_graphs++;
		}
		else if(str[0]=='v'){
			fscanf(fp, "%d %d", &u, &label);
			maxNodeLabel=max(maxNodeLabel, label);
		}
		else if(str[0]=='e'){
			fscanf(fp, "%d %d %d %lf", &u, &v, &label, &p);
			maxEdgeLabel=max(maxEdgeLabel, label);
		}
	}
	min_sup=ceil(no_of_graphs*MIN_SUP);
	fclose(fp);
}

vector <int> relabelledNodeLabels;
vector <int> relabelledEdgeLabels;
vector <int> nodeLabelFlag;
vector <int> edgeLabelFlag;
vector <int> nodeLabelFrequency;
vector <int> edgeLabelFrequency;
vector <int> nodeLabelOrder;
vector <int> edgeLabelOrder;
vector <int> nodeLabelLast;
vector <int> edgeLabelLast;

map < pair < pair <int, int> , int>, int > tupleLast;
map < pair < pair <int, int> , int>, int > tupleFreq;
set < pair < pair <int, int> , int > > frequentTuples;
vector <int> nodeLabels;

void clear(){
	nodeLabelFlag.clear();
	edgeLabelFlag.clear();
	nodeLabelFrequency.clear();
	edgeLabelFrequency.clear();
	nodeLabelLast.clear();
	edgeLabelLast.clear();
	nodeLabels.clear();
	tupleLast.clear();
	tupleFreq.clear();
}

bool cmpNodeLabels(int a, int b){
	return nodeLabelFrequency[a]>nodeLabelFrequency[b];
}

bool cmpEdgeLabels(int a, int b){
	return edgeLabelFrequency[a]>edgeLabelFrequency[b];
}

void find_frequent_labels(){

	relabelledNodeLabels.resize(maxNodeLabel+1, -1);
	nodeLabelFlag.resize(maxNodeLabel+1, -1);
	nodeLabelFrequency.resize(maxNodeLabel+1, 0);
	nodeLabelLast.resize(maxNodeLabel+1, -1);

	relabelledEdgeLabels.resize(maxEdgeLabel+1, -1);
	edgeLabelFlag.resize(maxEdgeLabel+1, -1);
	edgeLabelFrequency.resize(maxEdgeLabel+1, 0);
	edgeLabelLast.resize(maxEdgeLabel+1, -1);

	FILE* fp=fopen(INPUTFILE, "r");
	char str[5];
	int curGraph;
	int u, v, label;
	double p;
	int label_u, label_v;
	pair < pair <int, int> , int > tuple;
	while(fscanf(fp, "%s", str)!=EOF){
		if(str[0]=='t'){
			fscanf(fp, "%s %d", str, &v);
			if(v==-1) break;
			curGraph=v;
			nodeLabels.clear();
		}
		else if(str[0]=='v'){
			fscanf(fp, "%d %d", &u, &label);
			if(u==(int)nodeLabels.size()){
				nodeLabels.push_back(label);
			}
			else if(u>(int)nodeLabels.size()){
				nodeLabels.resize(u+1, -1);
				nodeLabels[u]=label;
			}
			else nodeLabels[u]=label;
			if(nodeLabelLast[label]!=curGraph){
				nodeLabelLast[label]=curGraph;
				nodeLabelFrequency[label]++;
				if(nodeLabelFrequency[label]==min_sup){
					nodeLabelOrder.push_back(label);
				}
			}
		}
		else if(str[0]=='e'){
			fscanf(fp, "%d %d %d %lf", &u, &v, &label, &p);
			//cout<<"p="<<p<<endl;
			label_u=nodeLabels[u];
			label_v=nodeLabels[v];
			tuple={{label_u, label}, label_v};
			if(edgeLabelLast[label]!=curGraph){
				edgeLabelLast[label]=curGraph;
				edgeLabelFrequency[label]++;
				if(edgeLabelFrequency[label]==min_sup){
					edgeLabelOrder.push_back(label);
				}
			}

			if(tupleLast[tuple]!=curGraph+1){
				tupleLast[tuple]=curGraph+1;
				tupleFreq[tuple]++;
				if(tupleFreq[tuple]==min_sup){
					frequentTuples.insert(tuple);
				}
			}
			tuple={{label_v, label}, label_u};
			if(tupleLast[tuple]!=curGraph+1){
				tupleLast[tuple]=curGraph+1;
				tupleFreq[tuple]++;
				if(tupleFreq[tuple]==min_sup){
					frequentTuples.insert(tuple);
				}
			}
		}
	}
	sort(nodeLabelOrder.begin(), nodeLabelOrder.end(), cmpNodeLabels);
	sort(edgeLabelOrder.begin(), edgeLabelOrder.end(), cmpEdgeLabels);
	for(int i=0; i<(int)nodeLabelOrder.size(); i++){
		relabelledNodeLabels[nodeLabelOrder[i]]=i;
	}
	for(int i=0; i<(int)edgeLabelOrder.size(); i++){
		relabelledEdgeLabels[edgeLabelOrder[i]]=i;
	}
	fclose(fp);
	clear();
}


