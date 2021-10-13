#include <bits/stdc++.h>
#include "graph.h"
#define XX first
#define YY second
using namespace std;

vector <Graph> database;
set <int> visitedEmbeddings;
queue <int> embeddingQueue;
vector < vector <Embedding> > getJointEmbeddings(vector <Embedding> embeddings){
    vector < vector <Embedding> > result;
    visitedEmbeddings.clear();
    for(int i=0; i<(int)embeddings.size(); i++){
        if(visitedEmbeddings.find(i)==visitedEmbeddings.end()){
            vector <Embedding> newVec;
            newVec.push_back(embeddings[i]);
            embeddingQueue.push(i);
            visitedEmbeddings.insert(i);
            int u;
            while(!embeddingQueue.empty()){
                u=embeddingQueue.front();
                embeddingQueue.pop();
                for(int j=0; j<(int)embeddings.size(); j++){
                    if(visitedEmbeddings.find(j)==visitedEmbeddings.end()){
                        int flag=0;
                        for(auto edge_u: embeddings[u].edges){
                            for(auto edge_j: embeddings[j].edges){
                                if(edge_u==edge_j){
                                    flag=1;
                                    break;
                                }
                            }
                        }
                        if(flag){
                            newVec.push_back(embeddings[j]);
                            embeddingQueue.push(j);
                            visitedEmbeddings.insert(j);
                        }
                    }
                }
            }
            result.push_back(newVec);
        }
    }
    return result;
}

vector < string > generateSubsets(int options, int to_take){
    if(to_take==0){
        string str="";
        for(int i=0; i<options; i++){
            str+="0";
        }
        vector <string> vec;
        vec.push_back(str);
        return vec;
    }
    if(options==to_take){
        string str="";
        for(int i=0; i<options; i++){
            str+="1";
        }
        vector <string> vec;
        vec.push_back(str);
        return vec;
    }
    vector <string> result;
    vector <string> opt1, opt2;
    opt1=generateSubsets(options-1, to_take);
    opt2=generateSubsets(options-1, to_take-1);
    string newStr;
    for(auto str: opt1){
        newStr="0"+str;
        result.push_back(newStr);
    }
    for(auto str: opt2){
        newStr="1"+str;
        result.push_back(newStr);
    }
    return result;
}

string firstSubset(int total, int ones){
    int zeroes=total-ones;
    string res="";
    for(int i=0; i<zeroes; i++){
        res+="0";
    }
    for(int i=0; i<ones; i++){
        res+="1";
    }
    return res;
}

string getNextSubset(string subset, int ones){
    bool foundOne=false;
    for(int i=subset.size()-1; i>=0; i--){
        if(subset[i]=='1'){
            foundOne=true;
        }
        else if(foundOne){
            subset[i]='1';
            subset[i+1]='0';
            int countOne=0;
            for(int j=i+2; j<subset.size(); j++){
                if(subset[j]=='1'){
                    countOne++;
                }
            }
            int k=subset.size()-1;
            while(countOne){
                subset[k]='1';
                countOne--;
                k--;
            }
            while(k>i+1){
                subset[k]='0';
                k--;
            }
            return subset;
        }
    }
    return "";
}

pair <double, double> evaluate_exact(vector <Embedding> embeddings){
    map < string, double > probability;
    double sum=0.0;
    probability[generateSubsets(embeddings.size(), 0)[0]]=1;
    for(int i=1; i<=(int)embeddings.size(); i++){
        //cerr<<"i="<<i<<endl;
        string subset=firstSubset(embeddings.size(), i);
        //cerr<<subsets.size()<<endl;
        double val;
        while(subset.size()>0){
            //cerr<<subset<<endl;
            int flag=0;
            int ind1, ind2;
            for(int j=0; j<(int)subset.size(); j++){
                if(subset[j]=='1'){
                    for(int k=j+1; k<(int)subset.size(); k++){
                        if(subset[k]=='1'){
                            int flag2=0;
                            for(auto edge_j: embeddings[j].edges){
                                for(auto edge_k: embeddings[k].edges){
                                    if(edge_j==edge_k){
                                        flag2=1;
                                    }
                                }
                            }
                            if(flag2==0){
                                flag=1;
                                ind1=j;
                                ind2=k;
                            }
                        }
                    }
                }
            }
            if(flag){
                string sub1=subset;
                sub1[ind1]='0';
                string sub2=subset;
                sub2[ind2]='0';
                string sub3=subset;
                sub3[ind1]='0';
                sub3[ind2]='0';
                val=(probability[sub1]*probability[sub2])/probability[sub3];
            }
            else{
                set <int> edges;
                int gid=embeddings[0].gid;
                for(int j=0; j<(int)subset.size(); j++){
                    if(subset[j]=='1'){
                        //cerr<<"j="<<j<<endl;
                        for(auto edge: embeddings[j].edges){
                            //cerr<<"edge="<<edge<<endl;
                            edges.insert(edge);
                        }
                    }
                }
                val=1;
                for(auto edge: edges){
                    val*=database[gid].edges[edge].p;
                }
            }
            probability[subset]=val;
            if(i%2) sum+=val;
            else sum-=val;
            subset=getNextSubset(subset, i);
        }
    }
    return {sum, sum};
}

pair <double, double> evaluate_approx(vector <Embedding> embeddings){
    srand(time(NULL));
    int gid=embeddings[0].gid;
    double m_sup=(min_sup*1.0)/(database.size()*1.0);
    //cerr<<"min_sup="<<min_sup<<endl;
    //cerr<<"m_sup="<<m_sup<<endl;
    double eps_prime=EPS*m_sup/2;
    double rounds=4*embeddings.size()*log(2/LAMBDA)/(eps_prime*eps_prime);
    double z=0, x=0, y=0;
    for(auto embedding: embeddings){
        double prod=1;
        for(auto edge: embedding.edges){
            prod*=database[gid].edges[edge].p;
        }
        z+=prod;
    }
    //cerr<<"z="<<z<<endl;
    set <int> pi;
    set <int> decided;
    //cerr<<"Rounds="<<rounds+1<<endl;
    for(int i=0; i<(rounds+1); i++){
        int ind=rand()%embeddings.size();
        //cerr<<"ind="<<ind<<endl;
        pi.clear();
        decided.clear();
        double prod=1;
        for(auto edge: embeddings[ind].edges){
            pi.insert(edge);
            decided.insert(edge);
            prod*=database[gid].edges[edge].p;
        }
        for(auto embedding: embeddings){
            for(auto edge: embedding.edges){
                if(decided.find(edge)==decided.end()){
                    if(rand()%2){
                        pi.insert(edge);
                        decided.insert(edge);
                        //cerr<<edge<<" is in pi"<<endl;
                        prod*=database[gid].edges[edge].p;
                    }
                    else{
                        decided.insert(edge);
                        //cerr<<edge<<" is not in pi"<<endl;
                        prod*=(1-database[gid].edges[edge].p);
                    }
                }
            }
        }
        y+=prod;
        bool isSatisfied=false;
        bool toAdd=true;
        for(int i=0; i<ind; i++){
            isSatisfied=true;
            for(auto edge: embeddings[i].edges){
                //cerr<<"Checking for "<<edge<<endl;
                if(pi.find(edge)==pi.end()){
                    isSatisfied=false;
                }
            }
            if(isSatisfied){
                toAdd=false;
            }
        }
        if(toAdd){
            x+=prod;
        }
    }
    //cerr<<"x="<<x<<endl;
    //cerr<<"y="<<y<<endl;
    //cerr<<"Returning "<<((x*z)/y)-eps_prime<<" and "<<((x*z)/y)+eps_prime<<endl;
    return {((x*z)/y)-eps_prime, ((x*z)/y)+eps_prime};
}
pair <double, double> evaluate(vector <Embedding> embeddings){
    vector <vector <Embedding> > partitions;
    //cerr<<"Finding partitions"<<endl;
    partitions=getJointEmbeddings(embeddings);
    //cerr<<"Partitions found\n";
    int e_s=embeddings[0].edges.size();
    double m_sup=(min_sup*1.0)/(database.size()*1.0);
    double threshold=(16*log(2.0/LAMBDA))/(EPS*EPS*m_sup*m_sup);
    long long challenger=0, temp_challenger;
    int cnt=0;
    int flag=0;
    for(auto jointEmbedding: partitions){
        int x_i=jointEmbedding.size();
        temp_challenger=1;
        while(cnt<x_i){
            temp_challenger*=2;
            if(challenger>threshold){
                flag=1;
                break;
            }
            cnt++;
        }
        challenger+=temp_challenger;
        if(challenger*e_s>threshold) flag=1;
        if(flag) break;
    }
    if(flag){
        pair <double, double> val=evaluate_approx(embeddings);
        return val;
    }
    double prob_l=1, prob_u=1;
    pair <double, double> val;
    for(auto jointEmbeddings: partitions){
        //cerr<<"Calling evaluate exact\n"<<endl;
        val=evaluate_exact(jointEmbeddings);
        prob_l*=(1-val.XX);
        prob_u*=(1-val.YY);
        //cerr<<"Call returned\n"<<endl;
    }
    return {1-prob_l, 1-prob_u};
}


