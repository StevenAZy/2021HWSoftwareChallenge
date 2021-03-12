#include "iostream"
#include <string>
#include "index1.h"
#include <stdio.h>
#include <stdlib.h>
using namespace std;

//定义
Server server[MAXN];                //服务器
VirtualMachine virtualMachin[MAXM]; //虚拟机
Request request[MAXT][MAXR];              //请求
int serverNum, virtualMachinNum, daysNum, requestNum;

//输入测试文件
void Input(char testFile[]) {
	FILE* file = fopen(testFile, "r");
    char serverMessage[50], serverTemp[5][20], virtualMachinMessage[30], virtualMachinTemp[4][20], requestMessage[30], requestTemp[3][20];
	while (fscanf(file, "%d\n", &serverNum) != EOF) {
		for (int i = 0; i < serverNum; i++) {
			fscanf(file, "%[^\n]%*c", serverMessage);
			int n = split(serverTemp, serverMessage);
			strcpy(server[i].Type, serverTemp[0]);
			server[i].Core = atoi(serverTemp[1]);
			server[i].Memory = atoi(serverTemp[2]);
			server[i].Hardware_cost = atoi(serverTemp[3]);
			server[i].DailyEnergy_cost = atoi(serverTemp[4]);
		}
        fscanf(file, "%d\n", &virtualMachinNum);
        for (i = 0; i < virtualMachinNum; i++) {
			fscanf(file, "%[^\n]%*c", virtualMachinMessage);
			int n = split(virtualMachinTemp, virtualMachinMessage);
			strcpy(virtualMachin[i].Type, virtualMachinTemp[0]);
			virtualMachin[i].Core = atoi(virtualMachinTemp[1]);
			virtualMachin[i].Memory = atoi(virtualMachinTemp[2]);
			virtualMachin[i].Doublenode = atoi(virtualMachinTemp[3]);
        }
        fscanf(file, "%d\n", &daysNum);
        for (i = 0; i < daysNum; i++) {
            fscanf(file, "%d\n", &requestNum);
            for (int j = 0; j < requestNum; j++) {
                fscanf(file, "%[^\n]%*c", requestMessage);
                int n = split(requestTemp, requestMessage);
                if(n == 1){
                    strcpy(request[i][j].Operation, requestTemp[0]);
                    request[i][j].ID = atoi(requestTemp[1]);
                }
                if (n == 2) {
                    strcpy(request[i][j].Operation, requestTemp[0]);
                    strcpy(request[i][j].Type, requestTemp[1]);
                    request[i][j].ID = atoi(requestTemp[2]);
                }
            }
        }
	}
	fclose(file);
}

//输出测试文件
void Output(Server server[], VirtualMachine virtualMachin[], Request request[][MAXR]) {
	printf("%d\n", serverNum);
    for(int i = 0; i < serverNum; i++){
		printf("(%s, %d, %d, %d, %d)\n", server[i].Type, server[i].Core, server[i].Memory, server[i].Hardware_cost, server[i].DailyEnergy_cost);
	}
	printf("%d\n", virtualMachinNum);
	for(i = 0; i < virtualMachinNum; i++){
		printf("(%s, %d, %d, %d)\n", virtualMachin[i].Type, virtualMachin[i].Core, virtualMachin[i].Memory, virtualMachin[i].Doublenode);
	}
	printf("%d\n", daysNum);
//	for(i = 0; i < daysNum; i++){
	//	printf("%d\n", requestNum);
	//	for(int j = 0; j < requestNum; j++){
		//	if(virtualMachin[i].Doublenode == 0)
		//		printf("(%s, %d, %d, %d)\n", request[i][j].Type, virtualMachin[i].Core, virtualMachin[i].Memory, virtualMachin[i].Doublenode);
		//	if(virtualMachin[i].Doublenode == 1)
			//	printf("(%s, %d, %d, %d)\n", virtualMachin[i].Type, virtualMachin[i].Core, virtualMachin[i].Memory, virtualMachin[i].Doublenode);
		//}
//	}
}

int main()
{
	// TODO:read standard input
    char testFile[] = "training-1.txt";
	//输入测试文件
	Input(testFile);
    Output(server, virtualMachin, request);
	// TODO:process
	// TODO:write standard output
	// TODO:fflush(stdout);

	return 0;
}
