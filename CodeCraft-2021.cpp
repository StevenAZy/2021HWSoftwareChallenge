#include <iostream>
#include <cstdio>
#include <cstring>
#include <array>
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <algorithm>
#include "header.hxx"
using namespace std;

//定义
vector<Server> Server_list;                      //服务器列表
unordered_map<string, VirtualMachine> VM_list(MAXM);//虚拟机信息
array<list<Request>, MAX_DAY> Request_list;         //请求序列
unordered_map<int, VirtualMachine> VM_Buyed;        //所有已创建的虚拟机, <虚拟机ID ：虚拟机>
unordered_map<string, vector<Server>> Server_Buyed; //所有购置的服务器，<服务器类型：<节点A - 节点B>>

// int dayBuy = 0;
int Days = 0;                                       //请求序列总天数
int ServerCount = 0;                                //服务器总数
long long cost = 0;                                 //总花费

void assignment(const string& Type, const int id, unordered_map<string, int>& Everyday) {
    auto vmInfo = VM_list[Type];
    int core = vmInfo.Core;
    int memory = vmInfo.Memory;
    int doubleTag = vmInfo.Doublenode;
    VM_Buyed.insert(make_pair(id, VirtualMachine(Type,id,core,memory,doubleTag)));
    // TODO:添加一个记录index，如果下一次创建的虚拟机比这次的大，就从当前位置往下找，而不用从头。
    for (int i = 0; i < 80; ++i) {
        Server s = Server_list[i];
        string serverName = s.Type;
        if (doubleTag == 0 && s.Total_Core/2 >= core && s.Total_Memory/2 >= memory) {
            int serverId = ServerCount;
            int nodeId = 0;             //默认部署节点A
            int indx = Server_Buyed[serverName].size();
            if (indx != 0) {
                auto svec = Server_Buyed[serverName];
                for (auto it : svec) {
                    auto node = it.node;
                    if (node.first.Core >= core && node.first.Memory >= memory) {
                        node.first.Core -= core;
                        node.first.Memory -= memory;
                        serverId = it.ID;
                        nodeId = 0;
                        break;
                    } else if (node.second.Core >= core && node.second.Memory >= memory) {
                        node.second.Core -= core;
                        node.second.Memory -= memory;
                        serverId = it.ID;
                        nodeId = 1;
                        break;
                    }
                }
            } else {
                //购买服务器
                Node n1(s.Total_Core-core, s.Total_Memory-memory);
                Node n2(s.Total_Core/2, s.Total_Memory/2);
                Server tmp(serverName,serverId,s.Total_Core,s.Total_Memory,s.Hardware_cost,s.DailyEnergy_cost,pair<Node,Node>(n1,n2));
                Server_Buyed[serverName].emplace_back(tmp);
                Everyday[serverName]++;
                ++ServerCount;
                // ++dayBuy;
                // cost += s.Hardware_cost;
            }
            VM_Buyed[id].BelongServer.first = serverName;
            VM_Buyed[id].BelongServer.second = indx;
            VM_Buyed[id].NodeID = nodeId;
            VM_Buyed[id].ServerID = serverId;
            break;
        } else if (doubleTag == 1 && s.Total_Core >= core && s.Total_Memory >= memory) {
            int serverId = ServerCount;
            int nodeId = 0;             //默认部署节点A
            int indx = Server_Buyed[serverName].size();
            if (indx != 0) {
                auto svec = Server_Buyed[serverName];
                for (auto it : svec) {
                    auto node = it.node;
                    if (node.first.Core >= core/2 && node.first.Memory >= 2*memory/2 && 
                        node.second.Core >= core/2 && node.second.Memory >= memory/2) {
                            node.first.Core -= core/2;
                            node.first.Memory -= core/2;
                            node.second.Core -= core/2;
                            node.second.Memory -= core/2;
                            serverId = it.ID;
                            nodeId = 2;
                            break;
                    }
                }
            } else {
                //购买服务器
                Node n((s.Total_Core-core)/2, (s.Total_Memory-memory)/2);
                Server tmp(serverName,serverId,s.Total_Core,s.Total_Memory,s.Hardware_cost,s.DailyEnergy_cost,pair<Node,Node>(n,n));
                Server_Buyed[serverName].emplace_back(tmp);
                Everyday[serverName]++;
                ++ServerCount;
                // ++dayBuy;
                // cost += s.Hardware_cost;
            }
            VM_Buyed[id].BelongServer.first = serverName;
            VM_Buyed[id].BelongServer.second = indx;
            VM_Buyed[id].NodeID = nodeId;
            VM_Buyed[id].ServerID = serverId;
            break;
        }
    }
}
// TODO:处理当天请求序列
unordered_map<string, int> handle(const list<Request>& rv) {
    unordered_map<string, int> Everyday;
    for (auto r : rv) {
        if (r.Operation == "del") {
            VirtualMachine vm = VM_Buyed[r.ID];
            auto svec = Server_Buyed[vm.BelongServer.first];
            int i = vm.BelongServer.second;
            if (vm.Doublenode == 1) {
                svec[i].node.first.Core += vm.Core / 2;
                svec[i].node.first.Memory += vm.Memory / 2;
                svec[i].node.second.Core += vm.Core / 2;
                svec[i].node.second.Memory += vm.Memory / 2;
            } else {
                if (vm.NodeID == 0) {
                    svec[i].node.first.Core += vm.Core;
                    svec[i].node.first.Memory += vm.Memory;
                } else if (vm.NodeID == 1) {
                    svec[i].node.second.Core += vm.Core;
                    svec[i].node.second.Memory += vm.Memory;
                }
            }
            VM_Buyed.erase(r.ID);
        }
        else {
            assignment(r.Type,r.ID, Everyday);
        }
    }
    return Everyday;
}

// TODO:迁移虚拟机
int moveVM() {
    int mvNum = 0;

    return mvNum;
}

int main()
{
    int lineNum;
    cin >> lineNum;
    Server_list.resize(lineNum);
    for (int i = 0; i < lineNum; ++i) {
        string t,c,m,h,d;
        cin >> t >> c >> m >> h >> d;
        Server_list[i].Type = t.substr(1,t.size()-2);
        Server_list[i].Total_Core = stoi(c.substr(0, c.size()-1));
        Server_list[i].Total_Memory = stoi(m.substr(0, m.size()-1));
        Server_list[i].Hardware_cost = stoi(h.substr(0, h.size()-1));
        Server_list[i].DailyEnergy_cost = stoi(d.substr(0, d.size()-1));
    }
    // 加载虚拟机
    cin >> lineNum;
    for (int i = 0; i < lineNum; ++i) {
        VirtualMachine vm;
        string t,c,m,d;
        cin >> t >> c >> m >> d;
        vm.Type = t.substr(1,t.size()-2);
        vm.Core = stoi(c.substr(0,c.size()-1));
        vm.Memory = stoi(m.substr(0,m.size()-1));
        vm.Doublenode = stoi(d.substr(0,d.size()-1));
        VM_list.insert(make_pair(vm.Type, vm));
    }
    // 加载请求
    cin >> Days;
    for (int i = 0; i < Days; ++i) {
        cin >> lineNum;
        for (int j = 0; j < lineNum; ++j) {
            Request r;
            cin >> r.Operation;
            r.Operation = r.Operation.substr(1, r.Operation.size()-2);
            if (r.Operation == "add") {
                cin >> r.Type;
                r.Type = r.Type.substr(0, r.Type.size()-1);
            }
            string id;
            cin >> id;
            r.ID = stoi(id.substr(0, id.size()-1));
            Request_list[i].emplace_back(r);
        }
    }

    sort(Server_list.begin(), Server_list.end(), [](Server& s1, Server& s2) -> bool {
            if (s1.Total_Core == s2.Total_Core) { return s1.Total_Memory < s2.Total_Memory; }
            return s1.Total_Core < s2.Total_Core;
        });
    
    // 处理请求
    for (int i = 0; i < Days; ++i) {
        unordered_map<string, int> Everyday = handle(Request_list[i]);
        //打印 (purchase, 当天购买服务器类型总数)
        // cout << "(purchase, " << Everyday.size() << ")"<< endl;
        int len = Everyday.size();
        printf("(purchase, %d)\n",len);
        //打印 （服务器型号，购买数量）
        for (auto it : Everyday) {
            printf("(%s, %d)\n",it.first.c_str(),it.second);
            // cout << "(" << it.first << ", " << it.second << ")" << endl;
        }

        // int mvNum = 0;
        // cout << "(migration, 0)" << endl;
        printf("(migration, 0)\n");

        for (auto it :VM_Buyed) {
            auto vm = it.second;
            if (vm.Doublenode == 0) {
                string s = vm.NodeID + "A";
                // cout << "(" << vm.ID << ", " << vm.ServerID << ", " << s << ")" << endl;
                // cout << "(" << vm.ServerID << ", " << s << ")" << endl;
                printf("(%d, %s)\n",vm.ServerID, s.c_str());
            } else {
                // cout << "(" << vm.ServerID << ")" << endl;
                printf("(%d)\n",vm.ServerID);
            }
        }
    }
    return 0;
}