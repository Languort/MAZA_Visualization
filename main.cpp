#include <Windows.h>
#include"easyx/include/easyx.h"
#include <conio.h>
#include<ctime>
#include<vector>
#include<cmath>

using namespace std;

//函数声明

void drawpixel(int x, int y, COLORREF RGB);//在x，y位置画颜色为RGB的方格
void initializeMaza();  //初始化迷宫生成
void display();//刷新屏幕
void prim_main();//prim主循环
void through(int x, int y);//将x,y与它四周一个随机已经变成路的路点打通
void join(int x, int y);//将x,y四周是墙壁的路点加入待选列表
void displaying();//BFS绘制寻路路线
void displayed();//BFS绘制寻路路线
void displayway();//DFS绘制寻路路线


/*
 *
----迷宫 自动生成（Prim）与自动寻路(BFS & DFS）可视化程序----

目录
1. 初始化变量
    1.1 初始化可视化变量
    1.2 初始化迷宫元素变量
2. Prim算法生成迷宫
    2.1 初始化Prim算法主函数
    2.2 Prim算法
3. DFS深度优先算法寻路
    3.1 建立栈Stack
    3.2 初始化DFS变量
    3.3 DFS寻路函数
    3.4 初始化DFS算法主函数
4. BFS广度优先算法寻路
    4.1 建立队列Queue
    4.2 初始化BFS变量
    4.3 BFS寻路函数
    4.4 初始化BFS算法主函数
5. 可视化
    5.1 初始化
    5.2 迷宫生成可视化
    5.3 BFS迷宫寻路可视化
    5.4 DFS迷宫寻路可视化
6. 随机生成可视化迷宫
7. 主函数main

-------------------------------------------------------
 *
*/


//  1. 初始化变量

//  1.1 初始化可视化变量

//标准量定义
#define WIDTH 510  //画面宽度
#define HEIGHT 510  //画面高度
#define Block_xy 10 //方格边长
#define ROAD 0
#define WALL 1
#define WAIT 2

#define WAY 3
#define PREWAY 4
#define THEWAY 5

//颜色定义
#define air COLORREF RGB(50,50,50) //空气颜色
#define side COLORREF RGB(225,225,225)//墙壁颜色
#define PreSelect COLORREF RGB(0,50,255)  //预选颜色
#define Select COLORREF RGB(255,0,0)   //选中颜色
#define preway COLORREF RGB(0,100,0) //预选路颜色
#define theway COLORREF RGB(0,255,0)// 寻路路径颜色

//  1.2 初始化迷宫元素变量

//迷宫大小定义
const int Block_wid = WIDTH / Block_xy; // x轴方格个数
const int Block_hei = HEIGHT / Block_xy; // y轴方格个数

//定义迷宫方格结构
struct block{
public:
    int x;
    int y;
    block(int x, int y) {
        this->x = x;
        this->y = y;
    }
};

//定义二维数组迷宫
int MAZA[Block_wid][Block_hei];

//动态数组
vector<block*> openlist;//待选列表
vector<block*> list;

//  2.  Prim算法生成迷宫

//  2.1 初始化Prim算法主函数
void initializeMaza()
{
    openlist.clear();
    // 初始化地图全部设置为WALL
    for(int y = 0 ;y < Block_hei;y++){
        for (int x = 0;x < Block_wid;x++){
            MAZA[x][y] = WALL;
        }
    }
    /*随机选一个路点，将它变成路*/
    MAZA[0][0] = ROAD;
/*将路点四周不是路的路点加入待选列表*/
    join(0, 0);
    prim_main();
}

//  2.2 Prim算法
void prim_main() {
    while (openlist.size() > 0) { //while (!openlist.empty()) { 存疑
        //从openlist中选一个路点
        int ARandomPoint = rand() % openlist.size();
        block *A = openlist[ARandomPoint];
        //将随机点A与其周围一个随机的路点连接
        through(A->x, A->y);
        //再将A点变成路点
        MAZA[A->x][A->y] = ROAD;
        //再再将A点周围不是路的路点加入待选组 且不要加入重复路点
        join(A->x, A->y);
        //即可从待选组中移除A点
        openlist.erase(openlist.begin() + ARandomPoint);
        drawpixel(A->x, A->y,Select);
        FlushBatchDraw(); //批量绘制 防止闪屏
        display();
    }
    openlist.clear(); //？
    EndBatchDraw(); //关闭批量绘图
}

// 将四周墙壁的路点加入待选列表
void through(int x,int y){
    list.clear();
    if (y - 2 >= 0 && MAZA[x][y - 2] == ROAD) {
        block* a = new block(x, y - 1);
        list.push_back(a);
    }
    //下
    if (y + 2 <= Block_hei - 1 && MAZA[x][y + 2] == ROAD) {
        block* b = new block(x, y + 1);
        list.push_back(b);
    }

    //左
    if (x - 2 >= 0 && MAZA[x - 2][y] == ROAD) {
        block* c = new block(x - 1, y);
        list.push_back(c);
    }
    //右
    if (x + 2 <= Block_wid - 1 && MAZA[x + 2][y] == ROAD) {
        block* d = new block(x + 1, y);
        list.push_back(d);
    }
    if (!list.empty()) {
        int BRandomPoint = rand() % list.size();
        block *B = list[BRandomPoint];

        // 将X Y 与B点连通
        MAZA[B->x][B->y] = ROAD;
    }
}

// 将四周是墙壁的路点加入待选列表
void join(int x , int y) {
    // 上面的格子
    if (y - 2 >= 0 && MAZA[x][y - 2] == WALL) {
        block *a = new block(x, y - 2);
        openlist.push_back(a);
        MAZA[x][y - 2] = WAIT;
    }

    //下
    if (y + 2 <= Block_wid - 1 && MAZA[x][y + 2] == WALL) {
        block* b = new block(x, y + 2);
        openlist.push_back(b);
        MAZA[x][y + 2] = WAIT;
    }
    //左
    if (x - 2 >= 0 && MAZA[x - 2][y] == WALL) {
        block* c = new block(x - 2, y);
        openlist.push_back(c);
        MAZA[x - 2][y] = WAIT;
    }
    //右
    if (x + 2 <= Block_wid - 1 && MAZA[x + 2][y] == WALL) {
        block* d = new block(x + 2, y);
        openlist.push_back(d);
        MAZA[x + 2][y] = WAIT;
    }
}

//  3.DFS深度优先算法寻路

//  3.1 建立栈Stack
typedef struct Node{    //定义栈节点
    int x;
    int y;
    struct Node *next;
}Node;

typedef Node* Stack;

Stack createEmptyStack(){   //创建空栈
    Stack p =(Stack)malloc(sizeof(Node)); //为空栈申请空间;
    if(p){
        p->next = NULL;
        return p;
    }
}

void push(int x,int y,Stack s){ //入栈
    Stack p =(Stack)malloc(sizeof(Node));
    if(p){ //if 申请成功则将元素压入栈中
        p->x = x;
        p->y = y;
        if(!s->next)
            p->next = NULL;
        else p->next = s->next; //否则将p插入头节点之后
        s->next = p;
    }
    // else{
    //      cout<<"申请空间失败"<<endl;
}

void pop(Stack s){  //出栈
    Stack p;
    p = s->next;
    if(p->next){
        s->next = p->next;
        free(p);
    }
    else return;
}

Node top(Stack s){  //取栈顶元素
    Node t;
    //判断栈是否为空 不为空则返回
    t=*(s->next);
    return t;
}

int isEmpty(Stack s) {  //判断栈是否为空
    //为空则返回1，不为空返回0
    if (s->next == NULL) return 1;
    else return 0;
}

//  3.2 初始化DFS变量
//  定义方向数组
int direction[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};//定义一个数组存四个方向操作数
int MIN=100;//用于记录最小路径
int flag=0;//用于记录是否寻路到终点


//  3.3 DFS寻路函数
void MazaPathDFS(int x,int y,int endx,int endy,int Block_wid,int Block_hei, Stack s){
    int newx,newy,i;
    //对四个方向进行寻路
    for(i=0;i<4;i++) {
        newx = x + direction[i][0];
        newy = y + direction[i][1];
        if (newx >= 0 && newx < Block_wid && newy >= 0 && newy < Block_hei && MAZA[newx][newy] == 0) {
            //判断下一个点是否在地图外
            push(newx, newy, s);
            MAZA[newx][newy] = WAY;
            displaying();
            Sleep(5);
            if (newx == endx && newy == endy) {
                //判断是否已经走到终点
                flag++; //标志
                displayed();  //绘制路线
                MAZA[newx][newy] = 0;   //将终点设为WALL
                pop(s);     //出栈

            }
            else {
                MazaPathDFS(newx, newy, endx, endy, Block_wid, Block_hei, s); //自我嵌套实现递归
                if (flag > 0) {
                    return;
                }
            }
        }
    }
    MAZA[x][y] = 0;
    pop(s);
}

//  3.4 初始化DFS算法主函数
void initializePathDFS(){ //初始化绘制路径
//  初始化 起点 终点
    int xbegin  = 0;
    int ybegin  = 0;
    int xend = 50;
    int yend = 50;
//  建立栈
    Stack s = createEmptyStack();
//  执行DFS分析
    MazaPathDFS(xbegin,ybegin,xend,yend,Block_wid,Block_hei,s);
}

//  4. BFS广度优先算法寻路

//4.1 建立队列Queue
typedef struct {    //定义节点
    int row;
    int col;
} Point;

typedef struct {    //定义队列节点
    Point point;
    int distance;
} QueueNode;

typedef struct {    //定义队列
    QueueNode* array;
    int front;
    int rear;
} Queue;

Queue* createQueue(int size) {  //创建队列
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->array = (QueueNode*)malloc(size * sizeof(QueueNode));
    queue->front = -1;
    queue->rear = -1;
    return queue;
}

void enqueue(Queue* queue, Point point, int distance) { //入队
    QueueNode newNode;
    newNode.point = point;
    newNode.distance = distance;

    if (queue->front == -1) {   //当队列为空时
        queue->front = 0;    //队首指向第一个元素
    }

    queue->rear++;  //队尾指向下一个元素
    queue->array[queue->rear] = newNode;    //将新元素加入队列
}

QueueNode dequeue(Queue* queue) {   //出队
    QueueNode node = queue->array[queue->front];

    if (queue->front == queue->rear) {  //当队列中只有一个元素时
        queue->front = -1;  //队列头为空
        queue->rear = -1;   //队列尾为空
    } else {
        queue->front++; //队列头指向下一个元素
    }

    return node;    //返回出队元素
}

//  4.2 初始化BFS变量

int isValid(int row, int col, int visited[Block_wid][Block_hei]) {  //判断是否为有效路径
    return (row >= 0 && row < Block_wid && col >= 0 && col < Block_hei && !visited[row][col]);
}

void printShortestPath(Point source, Point destination, Point parent[Block_wid][Block_hei]) {   //打印最短路径
    Point path[Block_wid * Block_hei];  //建立路径数组
    int pathLength = 0;

    Point current = destination;    //将终点设为当前点
    while (!(current.row == source.row && current.col == source.col)) {  //当当前点不为起点时
        path[pathLength++] = current;   //将当前点加入路径数组
        current = parent[current.row][current.col]; //将当前点设为父节点
    }
    for (int i = pathLength - 1; i >= 0; i--) {
        int n;
        int m;
        n=path[i].row;
        m=path[i].col;
        MAZA[m][n] = THEWAY;
        displayway();
    }

}


//  4.3 BFS寻路函数
void MazaPathBFS(int maze[Block_wid][Block_hei], Point source, Point destination) {
    //建立遍历过数组
    int visited[Block_wid][Block_hei];
    //建立父节点便于回溯
    Point parent[Block_wid][Block_hei];
    //将遍历数组和父节点进行初始化 防止出现意外数据导致错误
    for (int i = 0; i < Block_wid; i++) {
        for (int j = 0; j < Block_hei; j++) {
            visited[i][j] = 0;
            parent[i][j] = (Point){-1, -1};
        }
    }
    //建立队列
    Queue* queue = createQueue(Block_wid * Block_hei);
    //将起点加入队列
    enqueue(queue, source, 0);
    //将起点加入遍历数组
    visited[source.row][source.col] = 1;
    //建立横纵遍历数组
    int rowOffsets[] = {-1, 0, 1, 0};
    int colOffsets[] = {0, 1, 0, -1};
    //当队列不为空时
    while (queue->front != -1) {
        //取出队列中的第一个元素
        QueueNode current = dequeue(queue);
        //将队列中的第一个元素出队
        Point currentPoint = current.point;
        //将队列中的第一个元素的距离出队
        int currentDistance = current.distance;
        //当队列中的第一个元素为终点时
        if (currentPoint.row == destination.row && currentPoint.col == destination.col) {
            //打印最短路径
            printShortestPath(source, destination, parent);
            break;
        }
        //当队列中的第一个元素不为终点时
        for (int i = 0; i < 4; i++) {
            int newRow = currentPoint.row + rowOffsets[i];
            int newCol = currentPoint.col + colOffsets[i];
            //当队列中的第一个元素的四周有路时
            if (isValid(newRow, newCol, visited) && maze[newRow][newCol] == 0) {
                //将四周的路加入队列
                enqueue(queue, (Point){newRow, newCol}, currentDistance + 1);
                //将四周的路加入遍历数组
                visited[newRow][newCol] = 1;
                //将四周的路加入父节点
                parent[newRow][newCol] = currentPoint;
                MAZA[newRow][newCol] = PREWAY;
                displayway();
                Sleep(5);
            }
        }
    }
    free(queue->array);
    free(queue);
}

//  4.4 初始化BFS算法主函数
void initializePathBFS(){ //初始化绘制路径

    int xbegin  = 0;
    int ybegin  = 0;
    int xend = 50;
    int yend = 50;

    Point start = {0, 0};
    Point end = {50, 50};
//    Queue q = createEmptyQueue();

    MazaPathBFS(MAZA, start, end);
}


//  5. 可视化

//  5.1 初始化

//像素格绘制
void drawpixel(int x,int y,COLORREF RGB)
{
    setfillcolor(RGB);
    setlinecolor(COLORREF RGB(0,0,0));
    fillrectangle(x*Block_xy,y*Block_xy,x*Block_xy + Block_xy,y*Block_xy+Block_xy);
}


//  5.2 迷宫生成可视化
void display(){ //绘制

    //绘制迷宫
    for(int y = 0 ; y < Block_hei ; y++){
        for(int x = 0;x<Block_wid;x++)
        {
            if(MAZA[x][y] ==  WALL){
                drawpixel(x,y,side);
            }
            else if(MAZA[x][y] == ROAD) {
                drawpixel(x, y, air);
            }
            else if(MAZA[x][y] == WAIT){
                drawpixel(x,y,PreSelect);
            }

        }

    }
    FlushBatchDraw();
}


//  5.3 BFS迷宫寻路可视化
void displaying(){
    //绘制路径
    for(int y = 0; y < Block_hei ; y++){
        for(int x = 0;x<Block_wid;x++)
        {
            if(MAZA[x][y] == WAY) {  //绘制路线
                drawpixel(x, y, preway);
            }
        }
    }
    FlushBatchDraw();
}

void displayed(){
    //绘制路径
    for(int y = 0; y < Block_hei ; y++){
        for(int x = 0;x<Block_wid;x++)
        {
            if(MAZA[x][y] == WAY) {  //绘制路线
                Sleep(5);
                drawpixel(x, y, theway);
            }
        }
    }
    FlushBatchDraw();
}

//  5.4 DFS迷宫寻路可视化
void displayway(){
    //绘制路径
    for(int y = 0; y < Block_hei ; y++){
        for(int x = 0;x<Block_wid;x++)
        {
            if(MAZA[x][y] == PREWAY) {
                drawpixel(x, y, preway);
            }
            else if(MAZA[x][y] == THEWAY) {
                drawpixel(x, y, theway);
            }
        }
    }
    FlushBatchDraw();
}


//  6. 随机生成可视化迷宫

//建立随机迷宫主函数
void createRandomMAZA()
{
    srand((unsigned)time(NULL));//随机数种子
    initgraph(WIDTH, HEIGHT);//创建绘图窗口
    BeginBatchDraw();//开始批量绘图
    cleardevice();//清空显示
    //绘制迷宫主函数
    initializeMaza();
}


int main() {

    //随机生成第一个迷宫
    createRandomMAZA();

    //DFS寻找路径主函数
    initializePathDFS();
    getch();
    EndBatchDraw();

    Sleep(500);

    //随机生成第二个迷宫
    createRandomMAZA();

    //BFS寻找路径主函数
    initializePathBFS();
    getch();
    EndBatchDraw();

    //关闭绘图窗口
    closegraph();
    return 0;

}




