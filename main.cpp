#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <graphics.h>
#include <sys/types.h>
#include <dirent.h>
#include <math.h>
#include <conio.h>

// ��ʼ����ͼ���ڴ�С
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 640;

// ���廭�ʴ�С
const int PEN_SIZE = 15;

#define MAX_DIGIT 1024
#define MAX_K 10
#define PATH_LENTH 256
#define DEFAULTARGS 0

char path[] = "trainingDigits/";

char defaulttestpath[] = "testDigits/";

typedef struct data
{
    int label;
    int feature[MAX_DIGIT];
    struct data *next = NULL;
} DATA;

/*
   ����һ�����ڱȽ��������ݽṹ��labelֵ��С�ıȽϺ���
   ����a��b��ָ��DATA���͵�ָ�룬��ʾҪ�Ƚϵ��������ݽṹ
   ����ֵΪ���ͣ���ʾa->label��b->label�Ĳ�ֵ
*/
int cmp(const void* a, const void* b)
{
    DATA* da = (DATA*)a;         // ��ָ��aǿ��ת��ΪDATA����ָ��
    DATA* db = (DATA*)b;         // ��ָ��bǿ��ת��ΪDATA����ָ��
    return da->label - db->label; // �����������ݽṹ��labelֵ�Ĳ�
}

/*
������Transform_Pixel
������
    - pMem��ͼ���ڴ�ָ��
    - i��������
    - j��������
���ܣ��������ؿ��ֵ
����ֵ��
    -0����ɫ
    -1����ɫ
*/
int Transform_Pixel(int x, int y)
{
    int sum = 0;
    int color;
    for (int i = 0; i < 20; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            color = getpixel(x + i, y + j);
            if (color == 0xffffff)
            {
                sum += 0;
            }
            else if (color == 0x000000)
            {
                sum += 1;
            }
        }
    }
    double avg = (double)sum / 100;
    if (avg > 0.4)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
������Turn_Picture_to_txt
������
    - filename: png�ļ���
���ܣ���ȡ��ͼ�����ݴ����txt�ļ�
*/
void Turn_Picture_to_txt()
{
    int Tarry[32][32];
    int i = 0;
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            Tarry[i][j] = Transform_Pixel(i * 20, j * 20);
        }
    }
    FILE* fp = fopen("target.txt", "w");
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            fprintf(fp, "%d", Tarry[j][i]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

/*
������Get_FileLable
������-filename[]
���ܣ����ļ����л�ȡ��ǩֵ
����ֵ:int lable
*/
int Get_FileLable(char* filename)
{
    char tamp = filename[0];
    int lable = tamp - '0';
    return lable;
}

/*
������txt2vector
������-filename
���ܣ���txt�ļ�������
���أ�-DATA
*/
void txt2vector(struct dirent *ptr,DATA *pdata)
{
    char filename[PATH_LENTH];
    memset(filename, 0, sizeof(char));
    /*char buffer[1024];
    _getcwd(buffer, 1024);
    printf("%s", buffer);*/
    strcat(filename, path);
    strcat(filename, ptr->d_name);
    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("Can not open target file");
    }
    int ch;
    int cur = 0;
    ch = fgetc(fp);
    while (ch != EOF)
    {
        if (ch != '\n')
        {
            pdata->feature[cur] = ch - '0';
            cur++;
        }
        ch = fgetc(fp);
    }
    fclose(fp);
}

/*
������Train_DataSet
��������
���ܣ�ѵ�����ݲ��ҷ���
����ֵ��
*/

DATA *Train_DataSet(DATA *phead)
{
    printf("Start training\n");
    DATA *pcur = phead;
    DATA *pdata = NULL;
    DIR *dir = opendir("trainingDigits");
    struct dirent *ptr;
    ptr = readdir(dir);
    int count = 1;
    while (ptr->d_name[0] == '.')
    {
        ptr = readdir(dir);
    }
    while (ptr != NULL)
    {
        pdata = (DATA *)malloc(sizeof(DATA));
        pdata->label = Get_FileLable(ptr->d_name);
        txt2vector(ptr, pdata);
        if (phead == NULL)
        {
            phead = pdata;
            pdata->next = NULL;
            pcur = phead;
        }
        else
        {
            pcur->next = pdata;
            pcur = pdata;
            pcur->next = NULL;
        }
        ptr = readdir(dir);
    }
    printf("Training success\n");
    return phead;
}

/*
������Len_DataSet
������DATA *phead
���ܣ����ѵ�����ĳ���
����ֵ��int lenth
*/
int Len_DataSet(DATA* phead)
{
    DATA* cur = phead;
    int count=0;
    while (cur != NULL)
    {
        count++;
        cur = cur->next;
    }
    return count;
}

/*
������Euclidean_Distance
������
���ܣ�����ʶ�����ݺͲ�������֮���ŷʽ����
����ֵ��distance
*/

float Eucliden_Distance(DATA *pdata,DATA *ptrain)
{
    int i = 0;
    float distance = -1;
    while (i < MAX_DIGIT)
    {
        distance += pow(pdata->feature[i] - ptrain->feature[i], 2);
        i++;
    }
    distance = sqrt(distance);

    return distance;
}

/*
   ������knn
   ������
        - data���������������ݺͱ�ǩ������ָ��
        - num�����ݼ��е�����������
        - k��ѡ�����ڷ��������ھ���
        - result���洢������������ָ��
   ���ܣ�
        ����k������㷨�Ը��������ݼ����з���
*/
int KNN(DATA* pTrain, int k)
{
    //�Ȱ�Ŀ������������
    DATA* pData = (DATA*)malloc(sizeof(DATA));
    pData->label = -1;
    pData->next = NULL;
    FILE* fp = fopen("target.txt", "r");
    if (fp == NULL)
    {
        perror("Can not open target.txt file");
        exit(0);
    }
    int ch;
    int cur = 0;
    ch = fgetc(fp);
    while (ch != EOF)
    {
        if (ch != '\n')
        {
            pData->feature[cur] = ch - '0';
            cur++;
        }
        ch = fgetc(fp);
    }
    fclose(fp);

    int DataSetLenth = Len_DataSet(pTrain);
    float All_distance[2000];
    memset(All_distance, -1, sizeof(float));
    int all_lable[2000];
    memset(all_lable, -1, sizeof(int));

    cur = 0;
    DATA* pcur = pTrain;
    while (cur < DataSetLenth && pcur != NULL)
    {
        All_distance[cur] = Eucliden_Distance(pData, pcur);
        all_lable[cur] = pcur->label;
        pcur = pcur->next;
        cur++;
        //printf("progressing...... %d/%d\n", cur,DataSetLenth);
    }

    float minDistance[3] = {1024,1024,1024};  //K=3
    int minlable[3] = { -1,-1,-1 };
    cur = 0;
    while (cur < DataSetLenth)
    {
        for (int i = 0; i < 3; i++)
        {
            if (All_distance[cur] < minDistance[i])
            {
                minDistance[i] = All_distance[cur];
                minlable[i] = all_lable[cur];
                break;
            }
        }
        cur++;
    }
    free(pData);
    printf("The 3 nearest neighbour:\n");
    for (int i = 0; i < 3; i++)
    {
        printf("Distance:%.4f Lable:%d\n", minDistance[i], minlable[i]);
    }
    return minlable[0];
}

int KNN_TEST(DATA *pTrain,char *filename)
{
    //�Ȱ�Ŀ������������
    DATA* pData = (DATA*)malloc(sizeof(DATA));
    pData->label = -1;
    pData->next = NULL;
    FILE* fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("Can not open target.txt file");
        exit(0);
    }
    int ch;
    int cur = 0;
    ch = fgetc(fp);
    while (ch != EOF)
    {
        if (ch != '\n')
        {
            pData->feature[cur] = ch - '0';
            cur++;
        }
        ch = fgetc(fp);
    }
    fclose(fp);

    int DataSetLenth = Len_DataSet(pTrain);
    float All_distance[2000];
    memset(All_distance, -1, sizeof(float));
    int all_lable[2000];
    memset(all_lable, -1, sizeof(int));

    cur = 0;
    DATA* pcur = pTrain;
    while (cur < DataSetLenth && pcur != NULL)
    {
        All_distance[cur] = Eucliden_Distance(pData, pcur);
        all_lable[cur] = pcur->label;
        pcur = pcur->next;
        cur++;
        //printf("prograssing...... %d/%d\n", cur,DataSetLenth);
    }

    float minDistance[3] = { 1024,1024,1024 };  //K=3
    int minlable[3] = { -1,-1,-1 };
    cur = 0;
    while (cur < DataSetLenth)
    {
        for (int i = 0; i < 3; i++)
        {
            if (All_distance[cur] < minDistance[i])
            {
                minDistance[i] = All_distance[cur];
                minlable[i] = all_lable[cur];
                break;
            }
        }
        cur++;
    }
    free(pData);
    return minlable[0];
}

/*
������Draw
���ܣ��򿪻�ͼ�棬��������
*/
void Draw()
{
    bool drawing = false;
    int x = 0, y = 0;
    while (true)
    {
        // ��ȡ���״̬
        MOUSEMSG msg = GetMouseMsg();

        switch (msg.uMsg)
        {
        case WM_LBUTTONDOWN:
            drawing = true;
            x = msg.x;
            y = msg.y;
            break;
        case WM_MOUSEMOVE:
            if (drawing)
            {
                // ����СԲ��
                for (int i = -PEN_SIZE; i <= PEN_SIZE; ++i)
                {
                    for (int j = -PEN_SIZE; j <= PEN_SIZE; ++j)
                    {
                        putpixel(x + i, y + j, BLACK);
                    }
                }
                x = msg.x;
                y = msg.y;
            }
            break;
        case WM_LBUTTONUP:
            drawing = false;
            break;
        case WM_RBUTTONDOWN:
            goto Enddraw;
        }
    }
    Enddraw:BeginBatchDraw();
    EndBatchDraw();
}

int main()
{
    int args = DEFAULTARGS;

    printf("��ѡ���Ƿ���ҪGUI(1/0,Default:0):");
    scanf("%d",&args);
    if (args == DEFAULTARGS)
    {
        DATA *pTrain = NULL;
        pTrain = Train_DataSet(pTrain);

        char filename[PATH_LENTH];
        memset(filename, 0, sizeof(char));

        int lable[2000];
        int afterlable[2000];
        memset(lable, -1, sizeof(int));
        memset(afterlable, -1, sizeof(int));

        DIR* dir = opendir("testDigits");
        struct dirent* ptr;
        ptr = readdir(dir);
        while (ptr->d_name[0] == '.')
        {
            ptr = readdir(dir);
        }
        printf("\n");
        int cur = 0;
        while (ptr != NULL)
        {
            strcat(filename, defaulttestpath);
            strcat(filename, ptr->d_name);
            lable[cur] = Get_FileLable(ptr->d_name);
            afterlable[cur] = KNN_TEST(pTrain, filename);
            printf("loading testDigits.. %d/945\n", cur);
            memset(filename, 0, sizeof(char));
            ptr = readdir(dir);
            cur++;
        }
        printf("\n");
        cur = 0;
        float wrong = 0;
        while (cur < 946)
        {
            printf("test digits is:%d  recognize digits is:%d\n", lable[cur], afterlable[cur]);
            if (lable[cur] != afterlable[cur])
            {
                wrong++;
            }
            cur++;
        }
        printf("accuracy %.2f ", 1 - wrong / cur);
        _getch();
    }
    else
    {
        // ��ʼ��ͼ�δ���
        initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);

        // ���ñ�����ɫΪ��ɫ
        setbkcolor(WHITE);
        cleardevice();

        // ���û�����ɫΪ��ɫ
        setlinecolor(BLACK);
        setfillcolor(BLACK);

        DATA *pTrain = NULL;
        pTrain = Train_DataSet(pTrain);
        
        ExMessage mskey;

        while (true)
        {
            // ����Ƿ��м�������
            mskey = getmessage(EX_KEY);
            switch (mskey.vkcode)
            {
            case 0x1B: // ����ESC���˳�����
                printf("Program exited.");
                exit(0);
                break;
            case 0x43: // ����C����ջ���
                cleardevice();
                break;
            case 0x53: //����S����ʼ��ͼ
                Draw();
                break;
            case 0x52: // ����R��ʶ������
                FlushBatchDraw();
                Turn_Picture_to_txt();
                int classificatory = KNN(pTrain, 3);
                printf("The Digits is %d\n", classificatory);
                break;
            }
            flushmessage(EX_KEY);
            Sleep(30);
        }

        // �ر�ͼ�δ���
        closegraph();
        return 0;
    }
}
