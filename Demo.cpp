#include "index.h"
int main()
{
    int N, M, T, R;
    Server Server_list[MAX_ROW];
    VirtualMachine VM_list[MAX_DAY];
    Request Request_list[MAX_DAY][MAX_NUM];

    scanf("%d", &N);
    for(int i = 0; i < N; i++)
        scanf("%s%d%d%d%d", Server_list[i].Type, &Server_list[i].Core, &Server_list[i].Memory, &Server_list[i].Hardware_cost, &Server_list[i].DailyEnergy_cost);

    scanf("%d", &M);
    for(int i = 0; i < M; i++)
        scanf("%s%d%d%d", VM_list[i].Type, &VM_list[i].Core, &VM_list[i].Memory, &VM_list[i].Doublenode);
/* 
    // unfinished
    scanf("%d", &T);
    for(int i = 0; i < T; i++)
    {
        scanf("%d", &R);
        for(int req_num = 0; req_num < R; req_num++)
        {
            scanf("%s", Request_list[i][req_num].Operation);
            if(strcmp(Request_list[i][req_num].Operation, "add") == 0)
                scanf("%s%d", Request_list[i][req_num].Type, &Request_list[i][req_num].ID);
            else
                scanf("%d", &Request_list[i][req_num].ID);
        }
            
    }
*/

    for(int i = 0; i < N; i++)
        printf("Type:%s Core:%d Memory:%d Hardware_cost:%d DailyEnergy_cost:%d\n", Server_list[i].Type, Server_list[i].Core, Server_list[i].Memory, Server_list[i].Hardware_cost, Server_list[i].DailyEnergy_cost);
    for(int i = 0; i < M; i++)
        printf("Type:%s Core:%d Memory:%d Doublenode:%d\n", VM_list[i].Type, VM_list[i].Core, VM_list[i].Memory, VM_list[i].Doublenode);
    



    return 0;
}

/*
1
(NV603, 92, 324, 53800, 500)

*/
