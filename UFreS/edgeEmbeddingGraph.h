#include <bits/stdc++.h>
#include "graph.h"
#define XX first
#define YY second
using namespace std;

vector <Graph> database;

bool cmpEmbeddings(Embedding a, Embedding b){
    for(int i=0; i<(int)a.edges.size(); i++){
        if(a.edges[i]<b.edges[i]) return true;
        if(a.edges[i]>b.edges[i]) return false;
    }
    return false;
}

struct EdgeEmbeddingGraph{
    int no_of_edges;
    int no_of_embeddings;
    vector < vector <int> >  edge_to_embedding;
    vector < vector <int> > embedding_to_edge;
    vector <double> edgeP;
    vector <double> inherentP;
    EdgeEmbeddingGraph(){
        no_of_edges=0;
        no_of_embeddings=0;
        edge_to_embedding.push_back(vector <int>());
        embedding_to_edge.push_back(vector <int>());
        edgeP.push_back(0);
        inherentP.push_back(0);
    }


    bool isEqual(Embedding a, Embedding b){
        for(int i=0; i<(int)a.edges.size(); i++){
            if(a.edges[i]!=b.edges[i]) return false;
        }
        return true;
    }

    EdgeEmbeddingGraph(vector <Embedding> embeddings){  /*Assumes all embeddings belong to the same graph*/
        no_of_edges=0;
        no_of_embeddings=0;
        map <int, int> edgeNo;
        edge_to_embedding.push_back(vector <int>());
        embedding_to_edge.push_back(vector <int>());
        edgeP.push_back(0);
        inherentP.push_back(0);
        for(int i=0; i<(int)embeddings.size(); i++){
            sort(embeddings[i].edges.begin(), embeddings[i].edges.end());
        }
        sort(embeddings.begin(), embeddings.end(), cmpEmbeddings);
        for(auto embedding: embeddings){
            for(auto edge: embedding.edges){
                if(edgeNo.find(edge)==edgeNo.end()){
                    no_of_edges++;
                    edge_to_embedding.push_back(vector <int>());
                    edgeP.push_back(database[embedding.gid].edges[edge].p);
                    edgeNo[edge]=no_of_edges;
                }
            }
        }
        for(int i=0; i<(int)embeddings.size(); i++){
            if(i==0 || !isEqual(embeddings[i], embeddings[i-1])){
                no_of_embeddings++;
                embedding_to_edge.push_back(vector <int> ());
                inherentP.push_back(1);
                for(auto edge: embeddings[i].edges){
                    edge_to_embedding[edgeNo[edge]].push_back(no_of_embeddings);
                    embedding_to_edge[no_of_embeddings].push_back(edgeNo[edge]);
                }
            }
        }
    }

    void print(){
        printf("No of Edges=%d\n", no_of_edges);
        printf("No of embeddings=%d\n", no_of_embeddings);
        for(int i=1; i<=no_of_edges; i++){
            printf("Edge %d connected to embeddings: ", i);
            for(auto embedding: edge_to_embedding[i]){
                printf("%d ", embedding);
            }
            printf("\n");
            printf("Probability of edge existing: %f\n", edgeP[i]);
        }
        for(int i=1; i<=no_of_embeddings; i++){
            printf("Embedding %d connected to edges: ", i);
            for(auto edge: embedding_to_edge[i]){
                printf("%d ", edge);
            }
            printf("\n");
            printf("Probability of embedding existing: %f\n", inherentP[i]);
        }
    }
};

EdgeEmbeddingGraph compress(EdgeEmbeddingGraph g){
    EdgeEmbeddingGraph result;
    result.no_of_embeddings=g.no_of_embeddings;
    vector <int> remapping;
    remapping.resize(g.no_of_edges+1, -1);
    result.no_of_embeddings=g.no_of_embeddings;
    result.inherentP.resize(result.no_of_embeddings+1, 1);
    int cnt=0;
    for(auto adj: g.edge_to_embedding){
        if(adj.size()>1){
            result.no_of_edges++;
            remapping[cnt]=result.no_of_edges;
            //printf("Remapping %d to %d\n", cnt, remapping[cnt]);;
            result.edge_to_embedding.push_back(adj);
            //printf("Result's edge to embedding size is now %d\n", (int)result.edge_to_embedding.size());
            result.edgeP.push_back(g.edgeP[cnt]);
        }
        else if(adj.size()==1){
            for(auto em: adj){
                result.inherentP[em]*=g.edgeP[cnt];
            }
        }
        cnt++;
    }
    int v;
    for(int i=1; i<=g.no_of_embeddings; i++){
        result.embedding_to_edge.push_back(vector <int>());
        for(int j=0; j<(int)g.embedding_to_edge[i].size(); j++){
            v=remapping[g.embedding_to_edge[i][j]];
            if(v!=-1){
                result.embedding_to_edge[i].push_back(v);
            }
        }
    }
    return result;
}


vector <EdgeEmbeddingGraph> split(EdgeEmbeddingGraph g){
    vector <EdgeEmbeddingGraph> result;
    vector < pair <int, int> > edgeComponent;
    vector < pair <int, int> > embeddingComponent;
    int componentCount=0;
    int edgesInComponent;
    int embeddingsInComponent;
    edgeComponent.resize(g.no_of_edges+1, {-1, -1});
    embeddingComponent.resize(g.no_of_embeddings+1, {-1, -1});
    stack < pair <int, int> > dfsS;
    for(int i=1; i<=g.no_of_edges; i++){
        if(edgeComponent[i].XX==-1){
            dfsS.push({0, i});
            edgeComponent[i]={componentCount, 1};
            edgesInComponent=1;
            embeddingsInComponent=0;
            int u;
            while(!dfsS.empty()){
                if(dfsS.top().XX==0){
                    u=dfsS.top().YY;
                    dfsS.pop();
                    for(auto v: g.edge_to_embedding[u]){
                        if(embeddingComponent[v].XX!=-1) continue;
                        dfsS.push({1, v});
                        embeddingsInComponent++;
                        embeddingComponent[v]={componentCount, embeddingsInComponent};
                    }
                }
                else{
                    u=dfsS.top().YY;
                    dfsS.pop();
                    for(auto v: g.embedding_to_edge[u]){
                        if(edgeComponent[v].XX!=-1) continue;
                        dfsS.push({0, v});
                        edgesInComponent++;
                        edgeComponent[v]={componentCount, edgesInComponent};
                    }
                }
            }
            componentCount++;
        }
    }
    for(int i=1; i<=g.no_of_embeddings; i++){
        if(embeddingComponent[i].XX==-1){
            dfsS.push({1, i});
            embeddingComponent[i]={componentCount, 1};
            edgesInComponent=0;
            embeddingsInComponent=1;
            int u;
            while(!dfsS.empty()){
                if(dfsS.top().XX==0){
                    u=dfsS.top().YY;
                    dfsS.pop();
                    for(auto v: g.edge_to_embedding[u]){
                        dfsS.push({1, v});
                        embeddingsInComponent++;
                        embeddingComponent[v]={componentCount, embeddingsInComponent};
                    }
                }
                else{
                    u=dfsS.top().YY;
                    dfsS.pop();
                    for(auto v: g.embedding_to_edge[u]){
                        dfsS.push({0, v});
                        edgesInComponent++;
                        edgeComponent[v]={componentCount, edgesInComponent};
                    }
                }
            }
            componentCount++;
        }
    }
    for(int i=0; i<componentCount; i++){
        result.push_back(EdgeEmbeddingGraph());
    }
    for(int i=1; i<=g.no_of_edges; i++){
        result[edgeComponent[i].XX].no_of_edges++;
        result[edgeComponent[i].XX].edge_to_embedding.push_back(vector <int>());
        result[edgeComponent[i].XX].edgeP.push_back(0);
    }
    for(int i=1; i<=g.no_of_embeddings; i++){
        result[embeddingComponent[i].XX].no_of_embeddings++;
        result[embeddingComponent[i].XX].embedding_to_edge.push_back(vector <int>());
        result[embeddingComponent[i].XX].inherentP.push_back(1);
    }
    for(int i=1; i<=g.no_of_edges; i++){
        result[edgeComponent[i].XX].edgeP[edgeComponent[i].YY]=g.edgeP[i];
        for(auto v: g.edge_to_embedding[i]){
            result[edgeComponent[i].XX].edge_to_embedding[edgeComponent[i].YY].push_back(embeddingComponent[v].YY);
        }
    }
    for(int i=1; i<=g.no_of_embeddings; i++){
        result[embeddingComponent[i].XX].inherentP[embeddingComponent[i].YY]=g.inherentP[i];
        for(auto v: g.embedding_to_edge[i]){
            result[embeddingComponent[i].XX].embedding_to_edge[embeddingComponent[i].YY].push_back(edgeComponent[v].YY);
        }
    }
    return result;
}

EdgeEmbeddingGraph keepEdge(EdgeEmbeddingGraph g, int ind){
    EdgeEmbeddingGraph result;
    result.no_of_edges=g.no_of_edges-1;
    result.no_of_embeddings=g.no_of_embeddings;
    for(int i=1; i<=g.no_of_edges; i++){
        if(i==ind) continue;
        result.edge_to_embedding.push_back(g.edge_to_embedding[i]);
        result.edgeP.push_back(g.edgeP[i]);
    }
    for(int i=1; i<=g.no_of_embeddings; i++){
        result.embedding_to_edge.push_back(vector <int> ());
        result.inherentP.push_back(g.inherentP[i]);
        for(auto v: g.embedding_to_edge[i]){
            if(v<ind){
                result.embedding_to_edge[i].push_back(v);
            }
            else if(v>ind){
                result.embedding_to_edge[i].push_back(v-1);
            }
        }
    }
    return result;
}

EdgeEmbeddingGraph dontKeepEdge(EdgeEmbeddingGraph g, int ind){
    EdgeEmbeddingGraph result;
    result.no_of_edges=g.no_of_edges-1;
    result.no_of_embeddings=g.no_of_embeddings-g.edge_to_embedding[ind].size();
    vector <int> remappedEdges;
    vector <int> remappedEmbeddings;
    remappedEdges.resize(g.no_of_edges+1, -1);
    remappedEmbeddings.resize(g.no_of_embeddings+1, -1);
    remappedEdges[ind]=0;
    for(auto v: g.edge_to_embedding[ind]){
        remappedEmbeddings[v]=0;
    }
    int cnt=0;
    for(int i=1; i<=g.no_of_edges; i++){
        if(remappedEdges[i]==-1){
            cnt++;
            remappedEdges[i]=cnt;
        }
        else remappedEdges[i]=-1;
    }
    cnt=0;
    for(int i=1; i<=g.no_of_embeddings; i++){
        if(remappedEmbeddings[i]==-1){
            cnt++;
            remappedEmbeddings[i]=cnt;
        }
        else remappedEmbeddings[i]=-1;
    }
    for(int i=1; i<=g.no_of_edges; i++){
        if(remappedEdges[i]==-1) continue;
        result.edge_to_embedding.push_back(vector <int> ());
        for(auto v: g.edge_to_embedding[i]){
            if(remappedEmbeddings[v]==-1) continue;
            result.edge_to_embedding[remappedEdges[i]].push_back(remappedEmbeddings[v]);
        }
        result.edgeP.push_back(g.edgeP[i]);
    }
    for(int i=1; i<=g.no_of_embeddings; i++){
        if(remappedEmbeddings[i]==-1) continue;
        result.embedding_to_edge.push_back(vector <int> ());
        for(auto v: g.embedding_to_edge[i]){
            if(remappedEdges[v]==-1) continue;
            result.embedding_to_edge[remappedEmbeddings[i]].push_back(remappedEdges[v]);
        }
        result.inherentP.push_back(g.inherentP[i]);
    }
    return result;
}

map <int, int> keepSize(EdgeEmbeddingGraph e, int ind){
    map <int, int> result;
    vector <bool> visitedEd, visitedEm;
    visitedEd.resize(e.no_of_edges+1, false);
    visitedEm.resize(e.no_of_embeddings+1, false);
    visitedEd[ind]=true;
    stack < pair < int, int> > stk;
    int componentSize;
    pair <int, int> temp;
    for(int i=1; i<=e.no_of_edges; i++){
        if(!visitedEd[i]){
            visitedEd[i]=true;
            componentSize=1;
            stk.push({0, i});
            while(!stk.empty()){
                temp=stk.top();
                stk.pop();
                if(temp.XX==0){
                    for(auto v: e.edge_to_embedding[temp.YY]){
                        if(visitedEm[v]==false){
                            visitedEm[v]=true;
                            stk.push({1, v});
                        }
                    }
                }
                else{
                    for(auto v: e.embedding_to_edge[temp.YY]){
                        if(visitedEd[v]==false){
                            visitedEd[v]=true;
                            stk.push({0, v});
                            componentSize++;
                        }
                    }
                }
            }
            result[componentSize]++;
        }
    }
    return result;
}

map <int, int> dontkeepSize(EdgeEmbeddingGraph e, int ind){
    map <int, int> result;
    vector <bool> visitedEd, visitedEm, deletedEm;
    visitedEd.resize(e.no_of_edges+1, false);
    visitedEm.resize(e.no_of_embeddings+1, false);
    deletedEm.resize(e.no_of_embeddings+1, false);
    visitedEd[ind]=true;
    for(auto v: e.edge_to_embedding[ind]){
        visitedEm[v]=true;
        deletedEm[v]=true;
    }
    stack < pair < int, int> > stk;
    int componentSize, cnt;
    pair <int, int> temp;
    for(int i=1; i<=e.no_of_edges; i++){
        if(!visitedEd[i]){
            visitedEd[i]=true;
            componentSize=0;
            stk.push({0, i});
            while(!stk.empty()){
                temp=stk.top();
                stk.pop();
                cnt=0;
                if(temp.XX==0){
                    for(auto v: e.edge_to_embedding[temp.YY]){
                        if(visitedEm[v]==false){
                            visitedEm[v]=true;
                            stk.push({1, v});
                        }
                        if(deletedEm[v]==false){
                            cnt++;
                        }
                    }
                    if(cnt>=2){
                        componentSize++;
                    }
                }
                else{
                    for(auto v: e.embedding_to_edge[temp.YY]){
                        if(visitedEd[v]==false){
                            visitedEd[v]=true;
                            stk.push({0, v});
                        }
                    }
                }
            }
            if(componentSize) result[componentSize]++;
        }
    }
    return result;
}

map <int, int> generateCombinedMap(map <int, int> mp1, map <int, int> mp2){
    map<int, int> result;
    map<int, int>::iterator itr;
    for(itr=mp1.begin(); itr!=mp1.end(); itr++){
        result[itr->first]+=(itr->second);
    }
    for(itr=mp2.begin(); itr!=mp2.end(); itr++){
        result[itr->first]+=(itr->second);
    }
    return result;
}

map <int, int> getEstimate(EdgeEmbeddingGraph e, int ind){
    map <int, int> mp=generateCombinedMap(keepSize(e, ind), dontkeepSize(e, ind));
    map <int, int>::iterator it;
    int carry=0;
    int last=0;
    int need;
    int cnt=0;
    for(it=mp.begin(); it!=mp.end(); it++){
        if(last!=0){
            need=1;
            cnt=0;
            while(cnt<((it->first)-last)){
                need<<=1;
                cnt++;
                if(need>carry) break;
            }
            if(need<=carry){
                mp[it->first]+=(carry/need);
                carry%=need;
                mp[last]=carry;
            }
        }
        carry=mp[it->first];
        last=it->first;
    }
    need=1;
    int offset=0;
    while((need<<1)<=carry){
        need<<=1;
        offset++;
    }
    if(offset){
        mp[last+offset]=1;
        mp[last]=(carry-need);
    }
    return mp;
}


bool better(map <int, int> mp1, map <int, int> mp2){
    map <int, int>::iterator it1, it2;
    for(it1=mp1.begin(), it2=mp2.begin(); it1!=mp1.end(), it2!=mp2.end(); it1++, it2++){
        if((it1->first)<(it2->first)){
            return true;
        }
        else if((it1->first)>(it2->first)){
            return false;
        }
        else{
            if((it1->second)<(it2->second)){
                return true;
            }
            else if((it1->second)>(it2->second)){
                return false;
            }
        }
    }
    return false;
}

double evaluate(EdgeEmbeddingGraph g){
    if(g.no_of_edges==0){
        return 1-g.inherentP[1];
    }
    int bestInd=1;
    map <int, int> bestMap=getEstimate(g, 1);
    for(int i=2; i<=g.no_of_edges; i++){
        if(better(getEstimate(g, i), bestMap)){
            bestMap=getEstimate(g, i);
            bestInd=i;
        }
    }
    vector <EdgeEmbeddingGraph> g1, g2;
    g1=split(compress(keepEdge(g, bestInd)));
    g2=split(compress(dontKeepEdge(g, bestInd)));
    double prod1=g.edgeP[bestInd];
    double prod2=1-g.edgeP[bestInd];
    for(auto t: g1){
        prod1*=evaluate(t);
    }
    for(auto t: g2){
        prod2*=evaluate(t);
    }
    return prod1+prod2;
}
