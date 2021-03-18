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


void buyServer(VirtualMachine& vm, unordered_map<string, vector<Server>>& TodayServer_Buyed, list<VirtualMachine>& TodayVM_Buyed);
void delVM(VirtualMachine& vm);

int main()
{
    // freopen("training-2.txt", "r", stdin);
    // freopen("output.txt", "w", stdout);
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
                VirtualMachine TempVM(r.Type,r.ID,VM_list[r.Type].Core,VM_list[r.Type].Memory,VM_list[r.Type].Doublenode);
                buyServer(TempVM, TodayServer_Buyed, TodayVM_Buyed);
                TodayVM_Buyed.emplace_back(TempVM);
                cnt += 1;
            }
        }
        // printf("第%d天 add 请求数量为：%d\n",day,cnt);
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
            if (iter->ServerType == "del") { delVM(*iter); }
            if (iter->ServerID != -1) {
                if (iter->Doublenode == 0) {
                    if (day == Days && iter->ID == TodayVM_Buyed.back().ID) {
                        printf("(%d, %c)", iter->ServerID, iter->NodeID);
                    } else {
                        printf("(%d, %c)\n", iter->ServerID, iter->NodeID);
                    }
                } else {
                    if (day == Days && iter->ID == TodayVM_Buyed.back().ID) {
                        printf("(%d)", iter->ServerID);
                    } else {
                        printf("(%d)\n", iter->ServerID);
                    }
                }
                continue;
            }
            for (auto s = TodayServer_Buyed[iter->ServerType].begin(); s != TodayServer_Buyed[iter->ServerType].end(); ++s) {
                if (iter->Doublenode == 0) {
                    if (s->node.first.Core >= iter->Core && s->node.first.Memory >= iter->Memory) {
                        iter->ServerType = s->Type;
                        iter->NodeID = 'A';
                        iter->ServerID = s->ID;
                        s->node.first.Core -= iter->Core;
                        s->node.first.Memory -= iter->Memory;
                        All_Create_VM.insert(make_pair(iter->ID, *iter));
                        cnt -= 1;
                        if (day == Days && iter->ID == TodayVM_Buyed.back().ID) {
                            printf("(%d, %c)", s->ID, 'A');
                        } else {
                            printf("(%d, %c)\n", s->ID, 'A');
                        }
                        All_Buy_Server[s->ID] = *s;
                        break;
                    } else if (s->node.second.Core >= iter->Core && s->node.second.Memory >= iter->Memory) {
                        iter->ServerType = s->Type;
                        iter->NodeID = 'B';
                        iter->ServerID = s->ID;
                        s->node.second.Core -= iter->Core;
                        s->node.second.Memory -= iter->Memory;
                        All_Create_VM.insert(make_pair(iter->ID, *iter));
                        cnt -= 1;
                        if (day == Days && iter->ID == TodayVM_Buyed.back().ID) {
                            printf("(%d, %c)", s->ID, 'B');
                        } else {
                            printf("(%d, %c)\n", s->ID, 'B');
                        }
                        All_Buy_Server[s->ID] = *s;
                        break;
                    }
                } else if (iter->Doublenode == 1) {
                    if (s->node.first.Core >= iter->Core/2 && s->node.first.Memory >= iter->Memory/2
                        && s->node.second.Core >= iter->Core/2 && s->node.second.Memory >= iter->Memory/2) {
                            iter->ServerType = s->Type;
                            iter->NodeID = 'D';
                            iter->ServerID = s->ID;
                            s->node.first.Core -= iter->Core/2;
                            s->node.first.Memory -= iter->Memory/2;
                            s->node.second.Core -= iter->Core/2;
                            s->node.second.Memory -= iter->Memory/2;
                            All_Create_VM.insert(make_pair(iter->ID, *iter));
                            cnt -= 1;
                            if (day == Days && iter->ID == TodayVM_Buyed.back().ID) {
                                printf("(%d)", s->ID);
                            } else {
                                printf("(%d)\n", s->ID);
                            }
                            All_Buy_Server[s->ID] = *s;
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

void buyServer(VirtualMachine& vm, unordered_map<string, vector<Server>>& TodayServer_Buyed, list<VirtualMachine>& TodayVM_Buyed){
    int core = vm.Core;
    int memory = vm.Memory;
    int doubleTag = vm.Doublenode;
    int len = All_Buy_Server.size();
    auto iter = All_Buy_Server.begin();
    for (; iter != All_Buy_Server.end(); ++iter) {
        if (doubleTag == 0) {
            if (iter->second.node.first.Core >= core && iter->second.node.first.Memory >= memory) {
                vm.ServerType = iter->second.Type;
                vm.ServerID = iter->first;
                vm.NodeID = 'A';
                iter->second.node.first.Core -= core;
                iter->second.node.first.Memory -= memory;
                break;
            } else if (iter->second.node.second.Core >= core && iter->second.node.second.Memory >= memory) {
                vm.ServerType = iter->second.Type;
                vm.ServerID = iter->first;
                vm.NodeID = 'B';
                iter->second.node.second.Core -= core;
                iter->second.node.second.Memory -=memory;
                break;
            }
        } else if (doubleTag == 1) {
            if (iter->second.node.first.Core >= core/2 && iter->second.node.first.Memory >= memory/2 &&
                iter->second.node.second.Core >= core/2 && iter->second.node.second.Memory >= memory/2) {
                    vm.ServerType = iter->second.Type;
                    vm.ServerID = iter->first;
                    vm.NodeID = 'D';
                    iter->second.node.first.Core -= core/2;
                    iter->second.node.first.Memory -= memory/2;
                    iter->second.node.second.Core -= core/2;
                    iter->second.node.second.Memory -= memory/2;
                    break;
            }
        }
    }
    if (iter == All_Buy_Server.end()) {
        for (auto s = Server_list.begin(); s != Server_list.end(); ++s) {
            if (doubleTag == 0) {
                if (s->node.first.Core >= core && s->node.first.Memory >= memory) {
                    vm.ServerType = s->Type;
                    TodayServer_Buyed[s->Type].emplace_back(*s);
                    break;
                } else if (s->node.second.Core >= core && s->node.second.Memory >= memory) {
                    vm.ServerType = s->Type;
                    TodayServer_Buyed[s->Type].emplace_back(*s);
                    break;
                }
            } else if (doubleTag == 1) {
                if (s->node.first.Core >= core/2 && s->node.first.Memory >= memory/2 &&
                    s->node.second.Core >= core/2 && s->node.second.Memory >= memory/2) {
                        vm.ServerType = s->Type;
                        TodayServer_Buyed[s->Type].emplace_back(*s);
                        break;
                }
            }
        }
    }
}