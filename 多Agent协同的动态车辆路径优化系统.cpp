#include <bits/stdc++.h>
using namespace std;
const int MAXN = 105;
const double INF = 1e18;

// ================= 数据结构 =================
struct Node {
    int id;
    double x, y;
    double demand;
    double ready, due; // 时间窗
};

struct Vehicle {
    int id;
    double capacity;
};

vector<Node> nodes;
vector<Vehicle> fleet;
double dista[MAXN][MAXN];

// ================= 距离 =================
double dist(Node a, Node b) {
    return sqrt(pow(a.x-b.x,2)+pow(a.y-b.y,2));
}

// ================= Demand Agent =================
class DemandAgent {
public:
    void generate(int n) {
        nodes.clear();
        for(int i=0;i<n;i++){
            Node temp;
            temp.id = i;
            temp.x = rand()%100;
            temp.y = rand()%100;
            temp.demand = rand()%20 + 1;
            temp.ready = rand()%50;
            temp.due = rand()%50 + 50;
            nodes.push_back(temp);
        }
    }
};

// ================= Fleet Agent =================
class FleetAgent {
public:
    void init(int k) {
        fleet.clear();
        for(int i=0;i<k;i++){
            fleet.push_back({i, 100});
        }
    }
};

// ================= TimeWindow Agent =================
class TimeWindowAgent {
public:
    bool check(double arrival, Node &n) {
        return arrival <= n.due;
    }
};

// ================= ACO Agent =================
class ACOAgent {
public:
    double pheromone[MAXN][MAXN];

    void init(int n) {
        for(int i=0;i<n;i++)
            for(int j=0;j<n;j++)
                pheromone[i][j]=1.0;
    }

    vector<vector<int>> solve() {
        int n = nodes.size();
        init(n);

        vector<vector<int>> best_routes;
        double best_cost = INF;

        for(int iter=0;iter<50;iter++){
            vector<vector<int>> routes(fleet.size());
            vector<bool> vis(n,false);

            for(int k=0;k<fleet.size();k++){
                double cap = fleet[k].capacity;
                int cur = 0;
                routes[k].push_back(cur);

                while(true){
                    int nxt = -1;
                    double best = INF;

                    for(int j=0;j<n;j++){
                        if(!vis[j] && nodes[j].demand <= cap){
                            double score = dista[cur][j] / pheromone[cur][j];
                            if(score < best){
                                best = score;
                                nxt = j;
                            }
                        }
                    }

                    if(nxt==-1) break;

                    routes[k].push_back(nxt);
                    vis[nxt]=true;
                    cap -= nodes[nxt].demand;
                    cur = nxt;
                }
            }

            // 计算成本
            double cost=0;
            for(auto &r:routes){
                for(int i=1;i<r.size();i++){
                    cost += dista[r[i-1]][r[i]];
                }
            }

            if(cost < best_cost){
                best_cost = cost;
                best_routes = routes;
            }

            // 更新信息素
            for(auto &r:routes){
                for(int i=1;i<r.size();i++){
                    pheromone[r[i-1]][r[i]] += 1.0/(cost+1e-6);
                }
            }
        }

        return best_routes;
    }
};

// ================= Environment Agent =================
class EnvironmentAgent {
public:
    double getFactor() {
        return 0.8 + (rand()%70)/100.0;
    }
};

// ================= Replan Agent =================
class ReplanAgent {
public:
    bool trigger(double factor) {
        return factor > 1.3;
    }
};

// ================= Evaluation Agent =================
class EvaluationAgent {
public:
    double calc(vector<vector<int>>& routes, double factor) {
        double cost=0;
        for(auto &r:routes){
            for(int i=1;i<r.size();i++){
                cost += dista[r[i-1]][r[i]] * factor;
            }
        }
        return cost;
    }

    void print(vector<vector<int>>& routes, double cost) {
        for(int i=0;i<routes.size();i++){
            cout<<"车辆 "<<i<<": ";
            for(auto x:routes[i]) cout<<x<<" -> ";
            cout<<"END\n";
        }
        cout<<"总成本: "<<cost<<"\n";
    }
};

// ================= 主程序 =================
int main(){
    srand(time(0));

    DemandAgent demand;
    FleetAgent fleetAgent;
    ACOAgent aco;
    EnvironmentAgent env;
    ReplanAgent replan;
    EvaluationAgent eval;

    int n=20, k=3;

    demand.generate(n);
    fleetAgent.init(k);

    // 距离矩阵
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
            dista[i][j]=dist(nodes[i],nodes[j]);

    auto routes = aco.solve();

    double factor = env.getFactor();

    if(replan.trigger(factor)){
        cout<<"动态扰动触发重规划...\n";
        routes = aco.solve();
    }

    double cost = eval.calc(routes, factor);
    eval.print(routes, cost);

    return 0;
}