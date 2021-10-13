struct Edge{
	int u, v;
	int label;
	double p;
	Edge(){}
	Edge(int _u, int _v, int _label, double _p){
		u=_u;
		v=_v;
		label=_label;
		p=_p;
    }
	int getOtherNode(int n){
		return (n==u)? v: u;
	}
};

