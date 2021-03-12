// #include <cstdio>
// #include <cstdlib>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <array>
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <algorithm>
#include "header.hxx"
using namespace std;


//定义
array<Server, 80> Server_list;                      //服务器列表
unordered_map<string, VirtualMachine> VM_list(MAXM);//虚拟机信息
array<list<Request>, MAX_DAY> Request_list;         //请求序列
unordered_map<int, VirtualMachine> VM_Buyed;        //所有已创建的虚拟机, <虚拟机ID ：虚拟机>
unordered_map<string, vector<Server>> Server_Buyed; //已购置的服务器，<服务器类型：<节点A - 节点B>>

int Days = 0;                                       //请求序列总天数
int ServerCount = 0;                                //服务器总数
long long cost = 0;                                 //总花费


void LoadData(string filePath) {
    // 打开文件
    ifstream f(filePath);
    assert(f.is_open() == true);
    // 加载服务器
    int lineNum;
    f >> lineNum;
    for (int i = 0; i < lineNum; ++i) {
        f >> Server_list[i].Type >> Server_list[i].Total_Core
          >> Server_list[i].Total_Memory >> Server_list[i].Hardware_cost
          >> Server_list[i].DailyEnergy_cost;
    }
    // 加载虚拟机
    f >> lineNum;
    for (int i = 0; i < lineNum; ++i) {
        VirtualMachine vm;
        f >> vm.Type >> vm.Core >> vm.Memory >> vm.Doublenode;
        VM_list.insert(make_pair(vm.Type, vm));
    }
    // 加载请求
    f >> Days;
    for (int i = 0; i < Days; ++i) {
        f >> lineNum;
        for (int j = 0; j < lineNum; ++j) {
            Request r;
            f >> r.Operation;
            if (r.Operation == "add") {
                f >> r.Type;
            }
            f >> r.ID;
            Request_list[i].emplace_back(r);
        }
    }
}

void printArray(array<Server, 100> arr) {
    for (auto it : arr) {
        cout << it.Type  << " "
            << it.Total_Core << " "
            << it.Total_Memory << " "
            << it.Hardware_cost << " "
            << it.DailyEnergy_cost << endl;
    }
}

void assignment(const string& Type, const int id) {
    auto vmInfo = VM_list[Type];
    int core = vmInfo.Core;
    int memory = vmInfo.Memory;
    int doubleTag = vmInfo.Doublenode;
    VM_Buyed.insert(make_pair(id, VirtualMachine(Type,core,memory,doubleTag)));
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
                ++ServerCount;
                cost += s.Hardware_cost;
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
                ++ServerCount;
                cost += s.Hardware_cost;
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
void handle(const list<Request>& rv) {
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
            assignment(r.Type,r.ID);
        }
    }
}

// TODO:迁移虚拟机
int moveVM() {
    int mvNum = 0;

    return mvNum;
}

int main()
{
    string path = "training.txt";
    LoadData(path);
    cout << "服务器列表长度：" << Server_list.size() << endl;
    cout << "虚拟机列表长度：" << VM_list.size() << endl;
    cout << "请求列表长度：" << Request_list.size() << endl;

    sort(Server_list.begin(), Server_list.end(), [](Server& s1, Server& s2) -> bool {
            if (s1.Total_Core == s2.Total_Core) { return s1.Total_Memory < s2.Total_Memory; }
            return s1.Total_Core < s2.Total_Core;
        });
    // printArray(Server_list);

    // int x = 5;
    for (auto request : Request_list) {
        handle(request);
        cout << "(purchase, " << Server_Buyed.size() << ")"<< endl;
        for (auto it : Server_Buyed) {
            cout << "(" << it.first << ", " << it.second.size() << ")" << endl;
        }
        
        int mvNum = moveVM();
        cout << "(migration, " << mvNum << ")"<< endl;

        for (auto it :VM_Buyed) {
            auto vm = it.second;
            if (vm.Doublenode == 0) {
                string s = vm.NodeID + "A";
                cout << "(" << vm.ServerID << ", " << s << ")" << endl;
            } else {
                cout << "(" << vm.ServerID << ")" << endl;
            }
        }
        // --x;
        // if (x == 0) {
        //     break;
        // }
    }

    return 0;
}