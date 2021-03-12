#include<cstdio>
#include<cstdlib>
#include<cstring>

const int MAX_ROW = 100;
const int MAX_COL = 100;

const int MAXN = 100;				//����������
const int MAXM = 1000;				//���������
const int MAXT = 1000;				//��������
const int MAXR = 100000;			//ÿ���������
const int MAXID = 20;				//�ͺ�
const int MAX_CORE_MEMORY = 1024;	//CPU����&�ڴ��С
const int MAX_HARDWARE_COST = 500000;//Ӳ���ɱ�
const int MAX_DAILY_COST = 5000;	//ÿ���ܺĳɱ�
const int doubleNode[2] = { 0,1 };

/* Properties of Server
Type: �ͺţ����Ȳ�����20���������ֺʹ�С��ĸ����
Core��CPU����
Memory���ڴ��С
Hardware_cost��Ӳ���ɱ�
DailyEnergy_cost��ÿ���ܺĳɱ�
*/
struct Server {
    char Type[20];
    int Core;
    int Memory;
    int Hardware_cost;
    int DailyEnergy_cost;
};

/* Properties of VirtualMachine
Type: �ͺţ����Ȳ�����20���������ֺʹ�С��ĸ����
Core��CPU����
Memory���ڴ��С
Doublenode���Ƿ�˫�ڵ�
*/
struct VirtualMachine {
    char Type[20];
    int Core;
    int Memory;
    int Doublenode;
};

/* Properties of Request
Operation: ���� add/del
Type��������ͺ�
ID�������ID
*/
struct Request {
    char Operation[4];
    char Type[20];
    int ID;
};

// Split String
int split(char dst[][20], char str[])
{
    int n = 0, j = 0;
    int length = strlen(str);
    for(int i = 1; i < length - 1; i++)
    {
        if(str[i] != ',')
        {
            dst[n][j] = str[i];
            j++; 
        }
        else
        {
			dst[n][j] = '\0';
            i++;
            n++;
            j = 0;
        }
            
    }
    return n;
}
