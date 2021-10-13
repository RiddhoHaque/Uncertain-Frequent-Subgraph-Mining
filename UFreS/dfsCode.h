#include <bits/stdc++.h>
#include "dfsTuple.h"
using namespace std;
struct DFSCode{
	vector <DFSTuple> tuples;
	DFSCode(){}
	DFSCode(DFSTuple tuple){
		tuples.push_back(tuple);
	}
	
	void print(){
		printf("New DFS Code\n");
		for(auto tuple: tuples) tuple.print();
		printf("DFS Code ends\n");
	}
	
	int getRightMostChild(){
		int mx=0;
		for(auto tuple: tuples){
			mx=max(mx, tuple.u);
			mx=max(mx, tuple.v);
		}
		return mx;
	}
	vector <int> getRightMostPath(){
		int curr=getRightMostChild();
		vector <int> path;
		path.push_back(curr);
		for(int i=tuples.size()-1; i>=0; i--){
			if(tuples[i].v>tuples[i].u){
				if(tuples[i].v==curr){
					curr=tuples[i].u;
					path.push_back(curr);
				}
			}
		}
		reverse(path.begin(), path.end());
		return path;
	}
	
	void addTuple(DFSTuple tuple){
		tuples.push_back(tuple);
	}
};
