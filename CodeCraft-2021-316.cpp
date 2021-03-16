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

int Days = 0;                                       //请求序列总天数
int ServerCount = 0;                                //服务器总数
long long cost = 0;                                 //总花费

// 购买分配
void assignment(const string& Type, const int id, unordered_map<string, vector<Server>>& TodayServer_Buyed, list<pair<int,string>>& VM_daily_create){
    auto vmInfo = VM_list[Type];
    int core = vmInfo.Core;
    int memory = vmInfo.Memory;
    int doubleTag = vmInfo.Doublenode;
    //创建虚拟机
    VM_Buyed.insert(make_pair(id, VirtualMachine(Type, id, core,memory, doubleTag)));

    int listLen = Server_list.size();
    for(int i = 0; i < listLen; i++){
        Server& s = Server_list[i];
        string serverName = s.Type;
        if (doubleTag == 0 && s.Total_Core/2 >= core && s.Total_Core/2 >= memory){
            int len = Server_Buyed[serverName].size();
            int serverId = ServerCount;
            int nodeId = 0;
            int j = 0;
            for (auto& it : Server_Buyed[serverName]){
                j++;
                if (it.node.first.Core >= core && it.node.first.Memory >= memory) {
                    it.node.first.Core -= core;
                    it.node.first.Memory -= memory;
                    // printf("当前节点A剩余核数：%d\t",it.node.first.Core);
                    // printf("当前节点A剩余内存：%d\n",it.node.first.Memory);
                    serverId = it.ID;
                    nodeId = 0;
                    break;
                } else if (it.node.second.Core >= core && it.node.second.Memory >= memory) {
                    it.node.second.Core -= core;
                    it.node.second.Memory -= memory;
                    // printf("当前节点B剩余核数：%d\t",it.node.first.Core);
                    // printf("当前节点B剩余内存：%d\n",it.node.first.Memory);
                    serverId = it.ID;
                    nodeId = 1; ////// 修改成1了
                    break;
                }
            }

            if (j == len) { //如果当前已有服务器无法部署新的虚拟机，则购买
                Node n1(s.Total_Core/2 - core, s.Total_Memory/2 - memory);
                Node n2(s.Total_Core/2, s.Total_Memory/2);
                TodayServer_Buyed[serverName].emplace_back(
                    Server (serverName,-1,s.Total_Core,s.Total_Memory,s.Hardware_cost,s.DailyEnergy_cost,pair<Node,Node>(n1,n2))
                    );//先存放ID为0，每天结束后,更新编号，再给其赋值
            }

            //修改虚拟机的信息
            VM_Buyed[id].BelongServer.first = serverName;
            VM_Buyed[id].BelongServer.second = len;
            VM_Buyed[id].NodeID = nodeId;
            VM_Buyed[id].ServerID = serverId;
            string tmp = nodeId == 1 ? "B" : "A";
            VM_daily_create.push_back(pair<int,string>(serverId,tmp));//此时为0
            break;
        }
        else if (doubleTag == 1 && s.Total_Core >= core && s.Total_Memory >= memory) {
            int serverId = ServerCount;
            int nodeId = 0;             //默认部署节点A
            int len = Server_Buyed[serverName].size();
            int j = 0;
            for (auto& it : Server_Buyed[serverName]) {
                j++;
                if (it.node.first.Core >= core/2 && it.node.first.Memory >= memory/2 && 
                    it.node.second.Core >= core/2 && it.node.second.Memory >= memory/2) {
                    it.node.first.Core -= core/2;
                    it.node.first.Memory -= memory/2;
                    it.node.second.Core -= core/2;
                    it.node.second.Memory -= memory/2;
                    // printf("当前节点A剩余核数：%d\t",it.node.first.Core);
                    // printf("当前节点A剩余内存：%d\t",it.node.first.Memory);
                    // printf("当前节点B剩余核数：%d\t",it.node.second.Core);
                    // printf("当前节点B剩余内存：%d\n",it.node.second.Memory);
                    serverId = it.ID;
                    nodeId = 2;
                    break;
                }
            }
            if (j == len) {
                //购买服务器
                Node n((s.Total_Core-core)/2, (s.Total_Memory-memory)/2);
                TodayServer_Buyed[serverName].emplace_back(
                    Server (serverName,-1,s.Total_Core,s.Total_Memory,s.Hardware_cost,s.DailyEnergy_cost,pair<Node,Node>(n,n))
                );//先存放ID为0，每天结束后,更新编号，再给其赋值
            }
            VM_Buyed[id].BelongServer.first = serverName;
            VM_Buyed[id].BelongServer.second = len;
            VM_Buyed[id].NodeID = nodeId;
            VM_Buyed[id].ServerID = serverId;
            VM_daily_create.push_back(pair<int,string>(serverId, ""));
            break;
        }
    } 
    
}

// TODO:迁移虚拟机
int moveVM();
int main()
{
    freopen("training-1.txt", "r", stdin);
    freopen("out.txt", "w", stdout);
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

    //排序
    sort(Server_list.begin(), Server_list.end(), [](Server& s1, Server& s2) -> bool {
        if (s1.Total_Memory == s2.Total_Memory) { return s1.Total_Core > s2.Total_Core; }
        return s1.Total_Memory < s2.Total_Memory;
    });

    // 加载请求
    cin >> Days;
    for (int i = 0; i < Days; ++i) {
        cin >> lineNum;
        unordered_map<string, int> Everyday;
        list<pair<int, string>> VM_daily_create;
        unordered_map<string, vector<Server>> TodayServer_Buyed;

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

            /*-----处理请求-----*/
            if (r.Operation == "del") {
                VirtualMachine vm = VM_Buyed[r.ID];
                int i = vm.BelongServer.second;
                auto& server = Server_Buyed[vm.BelongServer.first][i];
                if (vm.Doublenode == 1) {
                    server.node.first.Core += vm.Core / 2;
                    server.node.first.Memory += vm.Memory / 2;
                    server.node.second.Core += vm.Core / 2;
                    server.node.second.Memory += vm.Memory / 2;
                } else {
                    if (vm.NodeID == 0) {
                        server.node.first.Core += vm.Core;
                        server.node.first.Memory += vm.Memory;
                    } else if (vm.NodeID == 1) {
                        server.node.second.Core += vm.Core;
                        server.node.second.Memory += vm.Memory;
                    }
                }
                VM_Buyed.erase(r.ID);
            } else if (r.Operation == "add") {
                // TODO：新建虚拟机 vm 并加入到容器 VM_daily_create里，同时判断是否需要购买服务器
                assignment(r.Type, r.ID, TodayServer_Buyed, VM_daily_create);
            }
            
        }
        
        // TODO：统一处理当天的所有 add 操作；
        for(auto it = TodayServer_Buyed.begin(); it != TodayServer_Buyed.end(); it++){
            string TempServerName = it->first;
            Everyday[TempServerName]++;
            if(Everyday[TempServerName] == 1){
                for(auto& it_ : TodayServer_Buyed[TempServerName]){
                Server_Buyed[TempServerName].emplace_back(
                    Server (it_.Type, ServerCount, it_.Total_Core, it_.Total_Memory, it_.Hardware_cost, it_.DailyEnergy_cost, it_.node)
                );
                ServerCount++;
                }
            }
        }

        /*-----打印当天的输出-----*/
        //打印 (purchase, 当天购买服务器类型总数)
        int len = Everyday.size();
        printf("(purchase, %d)\n",len);
        //打印 （服务器型号，购买数量）
        for (auto it : Everyday) {
            printf("(%s, %d)\n",it.first.c_str(),it.second);
        }

        // TODO：打印迁移服务器的信息
        int mvNum = 0;
        printf("(migration, 0)\n");

        for (auto vm :VM_daily_create) {
            if (vm.second != "") {
                printf("(%d, %s)\n",vm.first, vm.second.c_str());
            } else {
                printf("(%d)\n",vm.first);
            }
        }
    }
    return 0;
}