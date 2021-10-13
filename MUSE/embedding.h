using namespace std;
struct Embedding{
	int gid; //Number of the transaction to which this embedding belongs
	vector <int> edges; //Indexes of the edges in that transaction which are included in the embedding
	vector <int> nodesMapped; //Nodes in the transaction which are included in the embedding
	
	void print(){
		printf("New Embedding\n");
		printf("Embedding belongs to the graph %d\n", gid);
		printf("Edges in the embedding are: ");
		for(auto edge: edges){
			printf(" %d", edge);
		}
		printf("\nEmbedding description complete\n");
	}
	
	Embedding(){}
	
	Embedding(int _gid, int edge_id, int node0, int node1){
		gid=_gid;
		edges.push_back(edge_id);
		nodesMapped.push_back(node0);
		nodesMapped.push_back(node1);
	}
	
	void addEdge(int edge_id, int newNode=-1){ //Adds an edge to the embedding, if the edge is a back edge, simply call it with the first parameter only 
		edges.push_back(edge_id);
		if(newNode!=-1) nodesMapped.push_back(newNode);
	}
	int getMappedNode(int u){
		if(u>=(int)nodesMapped.size()) return -1;
		return nodesMapped[u];
	}
};

