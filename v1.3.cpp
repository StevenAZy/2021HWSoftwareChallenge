#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "head_1.3.hxx"
using namespace std;

/**********************************结构及变量定义*****************************/

//存储所有服务器的数组
vector<Server> Server_list;

//存储所有虚拟机类型的哈希表，以类型名做键
unordered_map<string, VirtualMachine> VM_list(1000);

//存储已购买的服务器的哈希表，以服务器ID做键
unordered_map<int, Server> All_Buy_Server;

//存储所有已创建的虚拟机，以虚拟机ID做键
unordered_map<int, VirtualMachine> All_Create_VM;

//存储每天购买的服务器，以服务器类型名做键
unordered_map<string, vector<Server>> TodayServer_Buyed;

//存储每天需创建的虚拟机，以虚拟机ID做键
vector<VirtualMachine> TodayVM_Buyed;

//存放当天创建的虚拟机ID，用于按请求顺序打印
vector<int> addList;

int total_nums = 0;  //已购服务器总数
int total_days = 0;  //总请求天数
int cur_day = 0;     //当前天数
double lambda = 0.5;
/****************************************************************************/

/**********************************函数声明***********************************/

//获取所有服务器信息
void getServerInfo(const int row);

//获取所有虚拟机信息
void getVMInfo(const int row);

//获取一条请求信息
Request getOneRequest();

//删除虚拟机
void delVM(VirtualMachine& vm);

//部署虚拟机
bool deployVM(VirtualMachine& vm);

//购买服务器
void buyServer(VirtualMachine& vm);

//打印部署信息
void printDeployInfo(const VirtualMachine& vm);
/*******************************************************************************/

int main()
{
    int num;
    cin >> num;
    Server_list.resize(num);
    //获取服务器信息
    getServerInfo(num);

	//一次性获取所有虚拟机信息
    cin >> num;
    getVMInfo(num);

    //对所有服务器按照性价比排序
    sort(Server_list.begin(), Server_list.end(), [](Server& s1, Server& s2) -> bool {
        double cost1 = s1.Hardware_cost / (s1.A_CoreSize + s1.B_CoreSize) + 
                        s1.Hardware_cost / (s1.A_MemorySize + s1.B_MemorySize) + s1.DailyEnergy_cost;
        double cost2 = s2.Hardware_cost / (s2.A_CoreSize + s2.B_CoreSize) + 
                        s2.Hardware_cost / (s2.A_MemorySize + s2.B_MemorySize) + s2.DailyEnergy_cost;
        return cost1 < cost2;
    });

    cin >> total_days;
    for (int i = 0; i < total_days; ++i) {
        cin >> num;
        cur_day++;
        int cnt = 0;
        
        //按天处理请求
        for (int j = 0; j < num; ++j) {
            //获取一条请求
            Request r = getOneRequest();
            //如果是"del"执行删除操作
            if (r.Operation == "del") {
                if (All_Create_VM.count(r.ID)) {
                    delVM(All_Create_VM[r.ID]);
                }
            }
            //如果是"add"操作，则进行部署 
            else if (r.Operation == "add") {
                addList.push_back(r.ID);
                VirtualMachine TempVM(r.Type,r.ID,VM_list[r.Type].Core,VM_list[r.Type].Memory,VM_list[r.Type].isDouble);
                bool isDone = deployVM(TempVM);
                //如果部署不成功，则购买服务器
                if (!isDone) {
                    buyServer(TempVM);
                }
                TodayVM_Buyed.emplace_back(TempVM);
                cnt += 1;
            }
        }
        //打印购买信息
        int today_buy = TodayServer_Buyed.size();   //今天购买的服务器类型总数
        printf("(purchase, %d)\n",today_buy);
        for (auto it = TodayServer_Buyed.cbegin(); it != TodayServer_Buyed.cend(); ++it) {
            int num = TodayServer_Buyed.size(); //每种类型的服务器数量
            printf("(%s, %d)\n",it->first.c_str(), num);
            //对当天购买的服务器进行编号
            for (auto iter = TodayServer_Buyed[it->first].begin(); iter != TodayServer_Buyed[it->first].end(); ++iter) {
                iter->ID = total_nums++;
                All_Buy_Server.insert( {iter->ID, *iter} );
            }
        }

        //打印迁移信息
        // int mvNum = 0;
        printf("(migration, 0)\n");

        //在部署前，先进行排序，优先安排大需求的；
        sort(TodayVM_Buyed.begin(), TodayVM_Buyed.end(), [](VirtualMachine& vm1, VirtualMachine& vm2) -> bool {
            int cost1, cost2;
            cost1 = vm1.isDouble == 1 ? (vm1.Core + vm1.Memory)/2 : vm1.Core + vm1.Memory;
            cost2 = vm1.isDouble == 1 ? (vm2.Core + vm2.Memory)/2 : vm2.Core + vm2.Memory;
            return cost1 > cost2;
        });

        //对当天未部署的虚拟机进行部署
        for (auto iter = TodayVM_Buyed.begin(); iter != TodayVM_Buyed.end(); ++iter) {
            //如果未部署，则进行部署
            if (iter->ServerID == -1) {
                bool isDone = deployVM(*iter);
                if (!isDone) {
                    printf("第%d天，虚拟机 %d 未能部署成功", cur_day, iter->ID);
                    return 0;
                }
            }
        }

        //打印部署信息
        for (int id : addList) {
            int i = 0;
            while (TodayVM_Buyed[i].ID != id) { ++i; }
            printDeployInfo(TodayVM_Buyed[i]);
        }

        TodayServer_Buyed.clear();
        TodayVM_Buyed.clear();
        addList.clear();
    }
    return 0;
}


void getServerInfo(const int row) {
    for (int i = 0; i < row; ++i) {
        string t,c,m,h,d;
        cin >> t >> c >> m >> h >> d;
        Server_list[i].Type = t.substr(1,t.size()-2);
        int total_Core = stoi(c.substr(0, c.size()-1));
        Server_list[i].A_CoreSize = total_Core / 2;
        Server_list[i].B_CoreSize = total_Core / 2;
        int total_Memory = stoi(m.substr(0, m.size()-1));
        Server_list[i].A_MemorySize = total_Memory / 2;
        Server_list[i].B_MemorySize = total_Memory / 2;
        Server_list[i].Hardware_cost = stoi(h.substr(0, h.size()-1));
        Server_list[i].DailyEnergy_cost = stoi(d.substr(0, d.size()-1));
    }
}

void getVMInfo(const int row) {
    for (int i = 0; i < row; ++i) {
        VirtualMachine vm;
        string t,c,m,d;
        cin >> t >> c >> m >> d;
        vm.Type = t.substr(1,t.size()-2);
        vm.Core = stoi(c.substr(0,c.size()-1));
        vm.Memory = stoi(m.substr(0,m.size()-1));
        vm.isDouble = stoi(d.substr(0,d.size()-1));
        VM_list.insert( {vm.Type, vm} );
    }
}

Request getOneRequest() {
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
    return r;
}

void delVM(VirtualMachine& vm) {
    if (vm.NodeID == 'D') {
        All_Buy_Server[vm.ServerID].A_CoreSize += vm.Core / 2;
        All_Buy_Server[vm.ServerID].A_MemorySize += vm.Memory / 2;
        All_Buy_Server[vm.ServerID].B_CoreSize += vm.Core / 2;
        All_Buy_Server[vm.ServerID].B_MemorySize += vm.Memory / 2;
    } else if (vm.NodeID == 'A') {
            All_Buy_Server[vm.ServerID].A_CoreSize += vm.Core;
            All_Buy_Server[vm.ServerID].A_MemorySize += vm.Memory;
    } else if (vm.NodeID == 'B') {
        All_Buy_Server[vm.ServerID].B_CoreSize += vm.Core;
        All_Buy_Server[vm.ServerID].B_MemorySize += vm.Memory;
    }
    All_Create_VM.erase(vm.ID); 
}

bool deployVM(VirtualMachine& vm) {
    // int isdone = 0;         //是否部署成功
    double gap = INT32_MAX; 
    int serverID = -1;      //保存最小gap的服务器ID
    char nodeID = '-';      //保存最小gap的服务器对应的节点
    for (auto s = All_Buy_Server.begin(); s !=  All_Buy_Server.end(); ++s) {
        if (vm.isDouble == 1 && s->second.A_CoreSize >= vm.Core/2 && s->second.A_MemorySize >= vm.Memory/2 &&
            s->second.B_CoreSize >= vm.Core/2 && s->second.B_MemorySize >= vm.Memory/2) {
                double val = lambda * (s->second.A_CoreSize + s->second.B_CoreSize - vm.Core)
                            + (1 - lambda) * (s->second.A_MemorySize + s->second.B_MemorySize - vm.Memory);
                if (val < gap) {
                    serverID = s->first;
                    gap = val;
                    nodeID = 'D';
                }
        } else {
            if (s->second.A_CoreSize >= vm.Core && s->second.A_MemorySize >= vm.Memory) {
                double val = lambda * (s->second.A_CoreSize - vm.Core) + (1 - lambda) * (s->second.A_MemorySize - vm.Memory);
                if (val < gap) {
                    serverID = s->first;
                    gap = val;
                    nodeID = 'A';
                }
            } else if (s->second.B_CoreSize >= vm.Core && s->second.B_MemorySize >= vm.Memory) {
                double val = lambda * (s->second.B_CoreSize - vm.Core) + (1 - lambda) * (s->second.B_MemorySize - vm.Memory);
                if (val < gap) {
                    serverID = s->first;
                    gap = val;
                    nodeID = 'B';
                }
            }
        }
    }
    if (nodeID == '-' || serverID == -1) { return false; }
    if (nodeID == 'D') {
        All_Buy_Server[serverID].A_CoreSize -= vm.Core / 2;
        All_Buy_Server[serverID].A_MemorySize -= vm.Memory / 2;
        All_Buy_Server[serverID].B_CoreSize -= vm.Core / 2;
        All_Buy_Server[serverID].B_MemorySize -= vm.Memory / 2;
    } else if (nodeID == 'A') {
        All_Buy_Server[serverID].A_CoreSize -= vm.Core;
        All_Buy_Server[serverID].A_MemorySize -= vm.Memory;
    } else if (nodeID == 'B') {
        All_Buy_Server[serverID].B_CoreSize -= vm.Core;
        All_Buy_Server[serverID].B_MemorySize -= vm.Memory;
    }
    vm.NodeID = nodeID;
    vm.ServerID = serverID;
    All_Create_VM.insert( {vm.ID, vm} );
    return true;
}

//原始版本的部署函数
// bool deployVM(VirtualMachine& vm) {
//     int isdone = 0;         //是否部署成功
//     for (auto s = All_Buy_Server.begin(); s !=  All_Buy_Server.end(); ++s) {
//         if (vm.isDouble == 1 && s->second.A_CoreSize >= vm.Core/2 && s->second.A_MemorySize >= vm.Memory/2 &&
//             s->second.B_CoreSize >= vm.Core/2 && s->second.B_MemorySize >= vm.Memory/2) {
//                 s->second.A_CoreSize -= vm.Core / 2;
//                 s->second.A_MemorySize -= vm.Memory / 2;
//                 s->second.B_CoreSize -= vm.Core / 2;
//                 s->second.B_MemorySize -= vm.Memory / 2;
//                 vm.NodeID = 'D';
//                 vm.ServerID = s->first;
//                 All_Create_VM.insert( {vm.ID, vm} );
//                 isdone = 1;
//         } else {
//             if (s->second.A_CoreSize >= vm.Core && s->second.A_MemorySize >= vm.Memory) {
//                 s->second.A_CoreSize -= vm.Core;
//                 s->second.A_MemorySize -= vm.Memory;
//                 vm.NodeID = 'A';
//                 vm.ServerID = s->first;
//                 All_Create_VM.insert( {vm.ID, vm} );
//                 isdone = 1;
//             } else if (s->second.B_CoreSize >= vm.Core && s->second.B_MemorySize >= vm.Memory) {
//                 s->second.B_CoreSize -= vm.Core;
//                 s->second.B_MemorySize -= vm.Memory;
//                 vm.NodeID = 'B';
//                 vm.ServerID = s->first;
//                 All_Create_VM.insert( {vm.ID, vm} );
//                 isdone = 1;
//             }
//         }
//     }
//     return isdone == 1 ? true : false;
// }


void buyServer(VirtualMachine& vm){
    int isDone = 0;
    for (auto it = Server_list.cbegin(); it != Server_list.cend(); ++it) {
        if (vm.isDouble == 1 && it->A_CoreSize >= vm.Core/2 && it->A_MemorySize >= vm.Memory/2 &&
            it->B_CoreSize >= vm.Core/2 && it->B_MemorySize >= vm.Memory/2) {
                TodayServer_Buyed[it->Type].emplace_back(*it);
                isDone = 1;
        } else {
            if (it->A_CoreSize >= vm.Core && it->A_MemorySize >= vm.Memory) {
                TodayServer_Buyed[it->Type].emplace_back(*it);
                isDone = 1;
            }
        }
        if (isDone == 1) { break; }
    }
}

void printDeployInfo(const VirtualMachine& vm) {
    if (vm.isDouble == 1) {
        if (cur_day == total_days && vm.ID == addList.back()) {
            printf("(%d)", vm.ServerID);
        } else {
            printf("(%d)\n", vm.ServerID);
        }
    } else {
        if (cur_day == total_days && vm.ID == addList.back()) {
            printf("(%d, %c)", vm.ServerID, vm.NodeID);
        } else {
            printf("(%d, %c)\n", vm.ServerID, vm.NodeID);
        }
    }
}