#include <bits/stdc++.h>
struct DFSTuple{
	int u;
	int v;
	int label_u;
	int label_v;
	int eLabel;
	
	DFSTuple(){}
	DFSTuple(int _u, int _v, int lu, int el, int lv){
		u=_u;
		v=_v;
		label_u=lu;
		eLabel=el;
		label_v=lv;
	}
	
	void print(){
		printf("%d %d %d %d %d\n", u, v, label_u, eLabel, label_v);
	}
	
	bool operator <(const DFSTuple& a) const{
		int i=u;
		int j=v;
		int x=a.u;
		int y=a.v;
		
		if(i==x && j==y){
			if(label_u==a.label_u){
				if(eLabel==a.eLabel){
					return label_v<a.label_v;
				}
				return eLabel<a.eLabel;
			}
			else{
				return label_u<a.label_u;
			}
		}
		
		if(i<j && x<y){
			if(j==y){
				return i>x;
			}
			return j<y; 
		}
		if(i>j && x>y){
			if(i==x){
				return j<y;
			}
			return i<x;
		}
		if(i<j && x>y){
			return j<=x;
		}
		if(i>j && x<y){
			return i<y;
		}
		return false;
	}
	
};

