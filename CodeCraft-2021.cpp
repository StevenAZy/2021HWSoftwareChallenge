#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <assert.h>
#include "head.hxx"
using namespace std;

/**********************************结构及变量定义*****************************/

//存储所有服务器的数组
vector<Server> Server_list;

//存储所有虚拟机类型的哈希表，以类型名做键
unordered_map<string, VirtualMachine> VM_list(1000);

//存储已购买的服务器的哈希表，以总服务器ID做键
unordered_map<int, Server> TotalServer;

//存储所有已创建的虚拟机，以虚拟机本身ID做键
unordered_map<int, VirtualMachine> TotalVM;

//存储每天需创建的虚拟机，以总服务器ID做键
unordered_map<int, vector<int>> createVM_Today;

//存放当天创建的虚拟机ID，用于按请求顺序打印
vector<int> addList;

//当前申请的服务器类型 与 分配的总编号 的关系映射表
unordered_map<string, vector<int>> purchaseToday;

int total_nums = 0;  //已购服务器总数
int total_days = 0;  //总请求天数
int cur_day = 0;     //当前天数
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
void buyServer(VirtualMachine& vm, int& purchaseNum_Today);

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

    //对所有服务器按日均消耗排序
    sort(Server_list.begin(), Server_list.end(), [](const Server& s1, const Server& s2) -> bool {
        return s1.DailyEnergy_cost < s2.DailyEnergy_cost;
    });
    //对所有服务器按照性价比排序
    sort(Server_list.begin(), Server_list.end(), [](const Server& s1, const Server& s2) -> bool {
        double cost1 = (s1.Hardware_cost + s1.DailyEnergy_cost) / (s1.A_CoreSize + s1.B_CoreSize) + 
                        (s1.Hardware_cost + s1.DailyEnergy_cost) / (s1.A_MemorySize + s1.B_MemorySize);
        double cost2 = (s2.Hardware_cost + s2.DailyEnergy_cost) / (s2.A_CoreSize + s2.B_CoreSize) + 
                        (s2.Hardware_cost + s2.DailyEnergy_cost) / (s2.A_MemorySize + s2.B_MemorySize) + s2.DailyEnergy_cost;
        return cost1 < cost2;
    });
    // sort(Server_list.begin(), Server_list.end(), [](const Server& s1, const Server& s2) -> bool {
    //     double cost1 = s1.Hardware_cost / (s1.A_CoreSize + s1.B_CoreSize) + 
    //                     s1.Hardware_cost / (s1.A_MemorySize + s1.B_MemorySize) + s1.DailyEnergy_cost;
    //     double cost2 = s2.Hardware_cost / (s2.A_CoreSize + s2.B_CoreSize) + 
    //                     s2.Hardware_cost / (s2.A_MemorySize + s2.B_MemorySize) + s2.DailyEnergy_cost;
    //     return cost1 < cost2;
    // });

    cin >> total_days;
    for (int i = 0; i < total_days; ++i) {
        cin >> num;
        cur_day++;
        int purchaseNum_Today = 0;  //今天购买的服务器数量
        int noDel = 1;
        vector<int> tempCreateVM_Today;   //存储当天双节点部署的虚拟机
        
        //按天处理请求
        for (int j = 0; j < num; ++j) {
            //获取一条请求
            Request r = getOneRequest();
            //如果是"add"执行部署操作
            if (r.Operation == "add") {
                addList.push_back(r.ID);
                VirtualMachine TempVM(r.Type,r.ID,VM_list[r.Type].Core,VM_list[r.Type].Memory,VM_list[r.Type].isDouble);
                //先不部署，存起来，遇到del再部署
                tempCreateVM_Today.push_back(r.ID);
                TotalVM.insert( {TempVM.ID, TempVM} );
                noDel = 1;
            }
            //如果是"del"执行删除操作
            else if (r.Operation == "del") {
                //先单后双
                for (auto id : tempCreateVM_Today) {
                    if (TotalVM[id].isDouble == 1) {continue;}
                    bool isDone = deployVM(TotalVM[id]);
                    //如果部署不成功，则购买服务器
                    if (!isDone) {
                        buyServer(TotalVM[id], purchaseNum_Today);
                    }
                    createVM_Today[TotalVM[id].ServerID].push_back(id);
                }
                for (auto id : tempCreateVM_Today) {
                    if (TotalVM[id].isDouble == 0) {continue;}
                    bool isDone = deployVM(TotalVM[id]);
                    //如果部署不成功，则购买服务器
                    if (!isDone) {
                        buyServer(TotalVM[id], purchaseNum_Today);
                    }
                    createVM_Today[TotalVM[id].ServerID].push_back(id);
                }
                delVM(TotalVM[r.ID]);
                tempCreateVM_Today.clear();
                noDel = 0;
            }
        }
        if (noDel == 1) {
            //部署虚拟机，先单后双
            for (auto id : tempCreateVM_Today) {
                if (TotalVM[id].isDouble == 1) {continue;}
                bool isDone = deployVM(TotalVM[id]);
                //如果部署不成功，则购买服务器
                if (!isDone) {
                    buyServer(TotalVM[id], purchaseNum_Today);
                }
                createVM_Today[TotalVM[id].ServerID].push_back(id);
            }
            for (auto id : tempCreateVM_Today) {
                if (TotalVM[id].isDouble == 0) {continue;}
                bool isDone = deployVM(TotalVM[id]);
                //如果部署不成功，则购买服务器
                if (!isDone) {
                    buyServer(TotalVM[id], purchaseNum_Today);
                }
                createVM_Today[TotalVM[id].ServerID].push_back(id);
            }
        }


        //对当天购买的服务器ID重排序，并打印购买信息
        int serverNums = purchaseToday.size();
        printf("(purchase, %d)\n",serverNums);
        int beg_indx = TotalServer.size() - purchaseNum_Today;  //重排序的起始索引下标
        unordered_map<int, Server> tmpServerList;
        for (auto tmpName : purchaseToday) {
            int n = tmpName.second.size();
            printf("(%s, %d)\n", tmpName.first.c_str(), n);
            for (auto tmpid : tmpName.second) {
                tmpServerList.insert({beg_indx, TotalServer[tmpid]});
                //更换虚拟机映射ID
                for (auto vmid : createVM_Today[tmpid]) {
                    TotalVM[vmid].ServerID = beg_indx;
                }
                beg_indx++;
            }
        }
        for (auto it = tmpServerList.begin(); it != tmpServerList.end(); ++it) {
            TotalServer[it->first] = it->second;
        }
        tmpServerList.clear();

        //打印迁移信息
        printf("(migration, 0)\n");

        //打印部署信息
        for (int id : addList) {
            printDeployInfo(TotalVM[id]);
        }

        purchaseToday.clear();
        createVM_Today.clear();
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
        TotalServer[vm.ServerID].A_CoreSize += vm.Core / 2;
        TotalServer[vm.ServerID].A_MemorySize += vm.Memory / 2;
        TotalServer[vm.ServerID].B_CoreSize += vm.Core / 2;
        TotalServer[vm.ServerID].B_MemorySize += vm.Memory / 2;
    } else if (vm.NodeID == 'A') {
            TotalServer[vm.ServerID].A_CoreSize += vm.Core;
            TotalServer[vm.ServerID].A_MemorySize += vm.Memory;
    } else if (vm.NodeID == 'B') {
        TotalServer[vm.ServerID].B_CoreSize += vm.Core;
        TotalServer[vm.ServerID].B_MemorySize += vm.Memory;
    }
    TotalVM.erase(vm.ID); 
}

//部署函数
bool deployVM(VirtualMachine& vm) {
    int isdone = 0;         //是否部署成功
    double gap = INT32_MAX;
    double lambda = 1;
    int candidateID = -1;
    char candidateNodeID = '-';
    for (auto s = TotalServer.begin(); s !=  TotalServer.end(); ++s) {
        if (vm.isDouble == 1) {
            if (s->second.A_CoreSize >= vm.Core/2 && s->second.A_MemorySize >= vm.Memory/2
                && s->second.B_CoreSize >= vm.Core/2 && s->second.B_MemorySize >= vm.Memory/2) {
                    double val_A = lambda * (s->second.A_CoreSize - vm.Core) + (1 - lambda) * (s->second.A_MemorySize - vm.Memory);
                    double val_B = lambda * (s->second.B_CoreSize - vm.Core) + (1 - lambda) * (s->second.B_MemorySize - vm.Memory);
                    if (min(val_A, val_B) < gap) {
                        gap = min(val_A, val_B);
                        candidateID = s->first;
                        candidateNodeID = 'D';
                    }
            }
        } else {
            if (s->second.A_CoreSize >= vm.Core && s->second.A_MemorySize >= vm.Memory) {
                double val = lambda * (s->second.A_CoreSize - vm.Core) + (1 - lambda) * (s->second.A_MemorySize - vm.Memory);
                if (val < gap) {
                    gap = val;
                    candidateID = s->first;
                    candidateNodeID = 'A';
                }
            } else if (s->second.B_CoreSize >= vm.Core && s->second.B_MemorySize >= vm.Memory) {
                double val = lambda * (s->second.B_CoreSize - vm.Core) + (1 - lambda) * (s->second.B_MemorySize - vm.Memory);
                if (val < gap) {
                    gap = val;
                    candidateID = s->first;
                    candidateNodeID = 'B';
                }
            }
        }
    }
    if (candidateID != -1 && candidateNodeID != '-') {
        if (vm.isDouble == 1) {
            TotalServer[candidateID].A_CoreSize -= vm.Core / 2;
            TotalServer[candidateID].A_MemorySize -= vm.Memory / 2;
            TotalServer[candidateID].B_CoreSize -= vm.Core / 2;
            TotalServer[candidateID].B_MemorySize -= vm.Memory / 2;
        } else {
            if (candidateNodeID == 'A') {
                TotalServer[candidateID].A_CoreSize -= vm.Core;
                TotalServer[candidateID].A_MemorySize -= vm.Memory;
            } else if (candidateNodeID == 'B') {
                TotalServer[candidateID].B_CoreSize -= vm.Core;
                TotalServer[candidateID].B_MemorySize -= vm.Memory;
            }
        }
        vm.NodeID = candidateNodeID;
        vm.ServerID = candidateID;
        isdone = 1;
    }
    return isdone == 1 ? true : false;
}

void buyServer(VirtualMachine& vm, int& purchaseNum_Today) {
    int isDone = 0;
    for (auto server : Server_list) {
        //部署双节点
        if (vm.isDouble == 1) {
            if (server.A_CoreSize >= vm.Core/2 && server.A_MemorySize >= vm.Memory/2
                && server.B_CoreSize >= vm.Core/2 && server.B_MemorySize >= vm.Memory/2) {
                    //设置服务器信息
                    TotalServer.insert({total_nums, server});
                    TotalServer[total_nums].A_CoreSize -= vm.Core / 2;
                    TotalServer[total_nums].A_MemorySize -= vm.Memory / 2;
                    TotalServer[total_nums].B_CoreSize -= vm.Core / 2;
                    TotalServer[total_nums].B_MemorySize -= vm.Memory / 2;
                    //设置虚拟机信息
                    vm.NodeID = 'D';
                    isDone = 1;
                }
        }
        //部署单节点
        else {
            if (server.A_CoreSize >= vm.Core && server.A_MemorySize >= vm.Memory) {
                //设置服务器信息
                TotalServer.insert({total_nums, server});
                TotalServer[total_nums].A_CoreSize -= vm.Core;
                TotalServer[total_nums].A_MemorySize -= vm.Memory;
                //设置虚拟机信息
                vm.NodeID = 'A';
                isDone = 1;
            }
        }
        if (isDone == 1) {
            vm.ServerID = total_nums;
            //设置重排序所需信息
            purchaseToday[server.Type].push_back(total_nums);
            total_nums++;
            purchaseNum_Today++;
            break;
        }
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