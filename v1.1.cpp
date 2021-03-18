#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <list>
#include <unordered_set>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "header.hxx"
using namespace std;

//定义
vector<Server> Server_list;                      //服务器列表
unordered_map<string, VirtualMachine> VM_list(MAXM);//虚拟机信息
unordered_map<int, VirtualMachine> All_Create_VM;        //所有已创建的虚拟机, <虚拟机ID ：虚拟机>
unordered_map<int, Server> All_Buy_Server; //所有购置的服务器，<服务器类型：<节点A - 节点B>>


int ServerCount = 0;                                //已购服务器总数
long long cost = 0;                                 //总花费
int day = 0;


void buyServer(const string& Type, const int id, unordered_map<string, vector<Server>>& TodayServer_Buyed, list<VirtualMachine>& TodayVM_Buyed);
void delVM(VirtualMachine& vm);
void printServer(){
    for (auto p : All_Buy_Server) {
        printf("编号：%d，节点A核数：%d，节点A内存：%d，节点B核数：%d，节点B内存：%d\n",p.first,p.second.node.first.Core,p.second.node.first.Memory,p.second.node.second.Core,p.second.node.second.Memory);
    }
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
        Server_list[i].node.first.Core = Server_list[i].Total_Core / 2;
        Server_list[i].node.first.Memory = Server_list[i].Total_Memory / 2;
        Server_list[i].node.second.Core = Server_list[i].Total_Core / 2;
        Server_list[i].node.second.Memory = Server_list[i].Total_Memory / 2;
    }

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

    sort(Server_list.begin(), Server_list.end(), [](Server& s1, Server& s2) -> bool {
        double ratio = abs(s1.Hardware_cost - s2.Hardware_cost) / s1.Hardware_cost;
        if (ratio <= 0.1) { return s1.DailyEnergy_cost < s2.DailyEnergy_cost; }
        return s1.Hardware_cost < s2.Hardware_cost;
    });

    int Days = 0;
    cin >> Days;
    int maxCore = 0;
    int maxMemory = 0;
    for (int i = 0; i < Days; ++i) {
        cin >> lineNum;
        list<VirtualMachine> TodayVM_Buyed;
        unordered_map<string, vector<Server>> TodayServer_Buyed;
        day++;
        int cnt = 0;
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
            if (r.Operation == "del") {
                if (!All_Create_VM.count(r.ID)) {
                    VirtualMachine TempVM(r.Type,r.ID, VM_list[r.Type].Core, VM_list[r.Type].Memory, VM_list[r.Type].Doublenode);
                    TempVM.ServerType = "del";
                    TodayVM_Buyed.emplace_back(TempVM);
                } else {
                    delVM(All_Create_VM[r.ID]);
                }
            } else if (r.Operation == "add") {
                maxCore = max(maxCore, VM_list[r.Type].Core);
                maxMemory = max(maxMemory, VM_list[r.Type].Memory);
                TodayVM_Buyed.emplace_back(VirtualMachine(r.Type,r.ID,VM_list[r.Type].Core,VM_list[r.Type].Memory,VM_list[r.Type].Doublenode));
                cnt += 1;
            }
        }

        for (auto iter = Server_list.begin(); iter != Server_list.end(); ++iter) {
            if (iter->Total_Core/2 >= maxCore && iter->Total_Memory/2 >= maxMemory) {
                TodayServer_Buyed.insert(make_pair(iter->Type, vector<Server>(cnt/2,*iter)));
                break;
            }
        }

        int len = TodayServer_Buyed.size();
        printf("(purchase, %d)\n",len);
        for (auto it = TodayServer_Buyed.cbegin(); it != TodayServer_Buyed.cend(); ++it) {
            int num = it->second.size();
            printf("(%s, %d)\n",it->first.c_str(), num);
            auto iter = TodayServer_Buyed[it->first].begin();
            for (int i = 0; i < it->second.size(); ++i) {
                iter->ID = ServerCount++;
                All_Buy_Server[iter->ID] = *iter;
                iter++;
            }
        }

        int mvNum = 0;
        printf("(migration, 0)\n");

        for (auto iter = TodayVM_Buyed.begin(); iter != TodayVM_Buyed.end(); ++iter) {
            if (iter->ServerType == "del") {
                // printServer();
                delVM(*iter);
            }
            for (auto& p : All_Buy_Server) {
                if (iter->ServerType == "del") { delVM(*iter); continue; }
                if (iter->Doublenode == 0) {
                    if (p.second.node.first.Core >= iter->Core && p.second.node.first.Memory >= iter->Memory) {
                        iter->ServerType = p.second.Type;
                        iter->NodeID = 'A';
                        iter->ServerID = p.first;
                        p.second.node.first.Core -= iter->Core;
                        p.second.node.first.Memory -= iter->Memory;
                        All_Create_VM.insert(make_pair(iter->ID, *iter));
                        cnt -= 1;
                        if (day == Days && iter->ID == TodayVM_Buyed.back().ID) {
                            printf("(%d, %c)", p.first, 'A');
                        } else {
                            printf("(%d, %c)\n", p.first, 'A');
                        }
                        break;
                    } else if (p.second.node.second.Core >= iter->Core && p.second.node.second.Memory >= iter->Memory) {
                        iter->ServerType = p.second.Type;
                        iter->NodeID = 'B';
                        iter->ServerID = p.first;
                        p.second.node.second.Core -= iter->Core;
                        p.second.node.second.Memory -= iter->Memory;
                        All_Create_VM.insert(make_pair(iter->ID, *iter));
                        cnt -= 1;
                        if (day == Days && iter->ID == TodayVM_Buyed.back().ID) {
                            printf("(%d, %c)", p.first, 'B');
                        } else {
                            printf("(%d, %c)\n", p.first, 'B');
                        }
                        break;
                    }
                } else if (iter->Doublenode == 1) {
                    if (p.second.node.first.Core >= iter->Core/2 && p.second.node.first.Memory >= iter->Memory/2
                        && p.second.node.second.Core >= iter->Core/2 && p.second.node.second.Memory >= iter->Memory/2) {
                            iter->ServerType = p.second.Type;
                            iter->NodeID = 'D';
                            iter->ServerID = p.first;
                            p.second.node.first.Core -= iter->Core/2;
                            p.second.node.first.Memory -= iter->Memory/2;
                            p.second.node.second.Core -= iter->Core/2;
                            p.second.node.second.Memory -= iter->Memory/2;
                            All_Create_VM.insert(make_pair(iter->ID, *iter));
                            cnt -= 1;
                            if (day == Days && iter->ID == TodayVM_Buyed.back().ID) {
                                printf("(%d)", p.first);
                            } else {
                                printf("(%d)\n", p.first);
                            }
                            break;
                    }
                }
            }
        }
    }
    return 0;
}

void delVM(VirtualMachine& vm) {
    if (vm.NodeID == 'D') {
        All_Buy_Server[vm.ServerID].node.first.Core += vm.Core / 2;
        All_Buy_Server[vm.ServerID].node.first.Memory += vm.Memory / 2;
        All_Buy_Server[vm.ServerID].node.second.Core += vm.Core / 2;
        All_Buy_Server[vm.ServerID].node.second.Memory += vm.Memory / 2;
    } else if (vm.NodeID == 'A') {
            All_Buy_Server[vm.ServerID].node.first.Core += vm.Core;
            All_Buy_Server[vm.ServerID].node.first.Memory += vm.Memory;
    } else if (vm.NodeID == 'B') {
        All_Buy_Server[vm.ServerID].node.second.Core += vm.Core;
        All_Buy_Server[vm.ServerID].node.second.Memory += vm.Memory;
    }
    All_Create_VM.erase(vm.ID); 
}

void buyServer(const string& Type, const int id, unordered_map<string, vector<Server>>& TodayServer_Buyed, list<VirtualMachine>& TodayVM_Buyed){
    auto vmInfo = VM_list[Type];
    int core = vmInfo.Core;
    int memory = vmInfo.Memory;
    int doubleTag = vmInfo.Doublenode;
    //创建虚拟机
    VirtualMachine TempVM(Type, id, core, memory, doubleTag);
    int len = Server_list.size();
    for (auto iter = Server_list.begin(); iter != Server_list.end(); ++iter) {
        if (doubleTag == 0) {
            if (iter->node.first.Core >= core && iter->node.first.Memory >= memory) {
                TempVM.ServerType = iter->Type;
                TodayServer_Buyed[iter->Type].emplace_back(*iter);
                break;
            } else if (iter->node.second.Core >= core && iter->node.second.Memory >= memory) {
                TempVM.ServerType = iter->Type;
                TodayServer_Buyed[iter->Type].emplace_back(*iter);
                break;
            }
        } else if (doubleTag == 1) {
            if (iter->node.first.Core >= core/2 && iter->node.first.Memory >= memory/2 &&
                iter->node.second.Core >= core/2 && iter->node.second.Memory >= memory/2) {
                    TempVM.ServerType = iter->Type;
                    TodayServer_Buyed[iter->Type].emplace_back(*iter);
                    break;
            }
        }
    }
    TodayVM_Buyed.emplace_back(TempVM);
}