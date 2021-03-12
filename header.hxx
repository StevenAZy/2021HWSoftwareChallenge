#include <cstdio>
#include <cstdlib>
#include <string>
#include <unordered_map>

const int MAX_ROW = 100;
const int MAX_COL = 100;

const int MAXN = 100;                //服务器数量
const int MAXM = 1000;               //虚拟机数量
const int MAX_DAY = 1000;            //请求天数
// const int MAXR = 100000;             //每天请求次数
const int MAX_ID = 20;               //型号
const int MAX_CORE_MEMORY = 1024;    //CPU核数&内存大小
const int MAX_HARDWARE_COST = 500000;//硬件成本
const int MAX_DAILY_COST = 5000;     //每日能耗成本
const int doubleNode[2] = { 0,1 };



class Node {
public:
    int Core = 0;
    int Memory = 0;
    Node() {}
    Node(int c, int m) : Core(c), Memory(m) {}
};
/* Properties of Server
Type: 型号，长度不超过20，仅由数字和大小字母构成
ID：服务器编号
Core：CPU核数
Memory：内存大小
Hardware_cost：硬件成本
DailyEnergy_cost：每日能耗成本
*/
class Server {
public:
    std::string Type;
    int ID;
    int Total_Core;
    int Total_Memory;
    int Hardware_cost;
    int DailyEnergy_cost ;
    std::pair<Node, Node> node;
    Server() {}
    Server(std::string t,int id,int tc,int tm,int hc, int dc,std::pair<Node,Node> p)
        :Type(t),ID(id),Total_Core(tc),Total_Memory(tm),Hardware_cost(hc),DailyEnergy_cost(dc),node(p) {}
};

/* Properties of VirtualMachine
Type: 型号，长度不超过20，仅由数字和大小字母构成
Core：CPU核数
Memory：内存大小
Doublenode：是否双节点，0表示单节点存储，1表示双节点存储
ServerID：所属服务器的ID
Belong 虚拟机所属<服务器-ID>
NodeID，0表示第一个节点，1表示第二个节点
*/
class VirtualMachine {
public:
    std::string Type;
    int Core;
    int Memory;
    int Doublenode;
    int NodeID;
    int ServerID;
    std::pair<std::string, int> BelongServer;
    VirtualMachine() {}
    VirtualMachine(std::string t,int c,int m,int d):Type(t),Core(c),Memory(m),Doublenode(d) {}
};

/* Properties of Request
Operation: 操作 add/del
Type：虚拟机型号
ID：虚拟机ID
*/
class Request {
public:
    std::string Operation;   // add or del
    std::string Type;        // server type
    int ID;             // server id
    Request() {}
    Request(std::string o, std::string t, int id) : Operation(o), Type(""), ID(id) {}
};