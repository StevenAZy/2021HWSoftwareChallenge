#include<cstdio>
#include<iostream>
#include<cstdlib>
#include<cstring>
using namespace std;

const int MAX_ROW = 100;
const int MAX_COL = 100;
const int MAX_DAY = 1000;
const int MAX_NUM = 100000;

/* Properties of Server
Type: 型号，长度不超过20，仅由数字和大小字母构成
Core：CPU核数
Memory：内存大小
Hardware_cost：硬件成本
DailyEnergy_cost：每日能耗成本
*/
struct Server {
    char Type[20];
    int Core;
    int Memory;
    int Hardware_cost;
    int DailyEnergy_cost;
};

/* Properties of VirtualMachine
Type: 型号，长度不超过20，仅由数字和大小字母构成
Core：CPU核数
Memory：内存大小
Doublenode：是否双节点
*/
struct VirtualMachine {
    char Type[20];
    int Core;
    int Memory;
    int Doublenode;
};

/* Properties of Request
Operation: 操作 add/del
Type：虚拟机型号
ID：虚拟机ID
*/
struct Request {
    char Operation[4];
    char Type[20];
    int ID;
};
