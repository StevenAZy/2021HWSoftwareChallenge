#include <string>
#include <unordered_map>

/* Properties of Server
Type: 型号，长度不超过20，仅由数字和大小字母构成
ID：服务器编号
Core：CPU核数
Memory：内存大小
Hardware_cost：硬件成本
DailyEnergy_cost：每日能耗成本
*/
struct Server{
    std::string Type;
    int ID;
    int A_CoreSize;
    int A_MemorySize;
    int B_CoreSize;
    int B_MemorySize;
    int Hardware_cost;
    int DailyEnergy_cost;
    Server () {}
    Server(std::string t,int id,int acs,int ams,int bcs,int bms,int hc,int dc) : 
        Type(t),ID(id),A_CoreSize(acs),A_MemorySize(ams),B_CoreSize(bcs),B_MemorySize(bms),
        Hardware_cost(hc),DailyEnergy_cost(dc) {}
};

/* Properties of VirtualMachine
Type: 型号，长度不超过20，仅由数字和大小字母构成
ID：虚拟机ID
Core：CPU核数
Memory：内存大小
isDouble：是否双节点，0表示单节点存储，1表示双节点存储
ServerID：所属服务器的ID
Belong 虚拟机所属<服务器-ID>
NodeID，0表示第一个节点，1表示第二个节点
*/
struct VirtualMachine {
    std::string Type;
    int ID;
    int Core;
    int Memory;
    int isDouble;
    char NodeID;
    int ServerID;
    VirtualMachine() {}
    VirtualMachine(std::string t,int id,int c,int m,int d) : 
        Type(t),ID(id),Core(c),Memory(m),isDouble(d),NodeID('-'),ServerID(-1) {}
};

/* Properties of Request
Operation: 操作 add/del
Type：虚拟机型号
ID：虚拟机ID
*/
struct Request {
    std::string Operation;   // add or del
    std::string Type;        // server type
    int ID;             // server id
    Request() {}
    Request(std::string o, std::string t, int id) : Operation(o), Type(""), ID(id) {}
};