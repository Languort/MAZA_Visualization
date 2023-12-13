#include <Windows.h>
    #include"easyx/include/easyx.h"
#include <conio.h>
#include<ctime>
#include<vector>
#include<cmath>

using namespace std;

/*
 *
--------迷宫 自动生成与自动寻路可视化程序--------

 目录
 1. 初始化变量
    1.1 初始化地图变量
    1.2 定义地图元素
    1.3 定义迷宫大小
    1.4 定义颜色
    1.5 定义迷宫格子
    1.6 定义二维数组迷宫
    1.7 定义动态数组
    1.8 函数声明

 2. 迷宫生成
    2.1 Prim算法生成迷宫
 3. 迷宫寻路
    3.1 DFS算法寻路

 4. 可视化
    4.1 初始化
    4.2 迷宫生成可视化
    4.3 迷宫寻路可视化

----------------------------------------------
*/


//1.0 初始化变量

//1.1 画面初始化
#define WIDTH 510  //画面宽度
#define HEIGHT 510  //画面高度
#define Block_xy 10 //方格边长

//1.2 定义地图元素
#define ROAD 0  // 路
#define WALL 1  // 墙壁
#define WAIT 2  // 墙壁临界块
#define THEWAY 3    // 实际路径
#define PREWAY 4    // 遍历路径

//1.3 定义迷宫大小
const int Block_wid = WIDTH / Block_xy; // x轴方格个数
const int Block_hei = HEIGHT / Block_xy; // y轴方格个数


//1.4 定义颜色
#define air COLORREF RGB(50,50,50) //空气颜色
#define side COLORREF RGB(255,255,255)//墙壁颜色
#define PreSelect COLORREF RGB(0,120,255)  //预选颜色
#define Select COLORREF RGB(255,0,0)   //选中颜色
#define preway COLORREF RGB(0,255,0) //遍历路颜色
#define way COLORREF RGB(0,100,0)// 实际寻路路径颜色


//1.5 定义迷宫格子
struct block{
public:
    int x;
    int y;
    block(int x, int y) {
        this->x = x;
        this->y = y;
    }
};


//1.6 定义二维数组迷宫
int MAZA[Block_wid][Block_hei];

//1.7 定义动态数组(vector)
vector<block*> openlist;//待选列表
vector<block*> list;

//1.8 函数声明
void clearscreen(COLORREF RGB);//清空地图为RGB颜色(带有网格)
void drawpixel(int x, int y, COLORREF RGB);//在x，y位置画颜色为RGB的方格
void initializeMaza();  //初始化迷宫生成
void join(int x, int y);//将x,y四周是墙壁的路点加入待选列表
void display();//刷新屏幕
void prim_main();//prim主循环
void through(int x, int y);//将x,y与它四周一个随机已经变成路的路点打通
void displayPath();//绘制寻路路线
void displaySearch();//


//2

//2.1 迷宫生成初始化
void initializeMaza()
{
    openlist.clear();
    // 2.1.1 初始化地图全部设置为WALL
    for(int y = 0 ;y < Block_hei;y++){
        for (int x = 0;x < Block_wid;x++){
            MAZA[x][y] = WALL;
        }
    }
    //2.1.2 将（0，0）点设置为路
    MAZA[0][0] = ROAD;

    //2.1.3 将路点四周不是路的路点加入待选列表
    join(0, 0);
    //2.1.4 执行Prim算法
    prim_main();
}


// 2.2 Prim算法生成迷宫
void prim_main() {
    while (openlist.size() > 0) {   //while (!openlist.empty()) { 存疑
        // 2.2.1 从openlist中随机生成一个点A
        int ARandomPoint = rand() % openlist.size();
        // 2.2.2 将生成的点A存入block中
        block *A = openlist[ARandomPoint];
        //2.2.3 将随机点A与其周围一个随机的路点连接
        through(A->x, A->y);
        //2.2.4 再将A点变成路点
        MAZA[A->x][A->y] = ROAD;
        //2.2.5 再再将A点周围不是路的路点加入待选组 且不要加入重复路点
        join(A->x, A->y);
        //2.2.6 即可从待选组中移除A点
        openlist.erase(openlist.begin() + ARandomPoint);
        //2.2.7 绘制地图选择方格
        drawpixel(A->x, A->y,Select);
        FlushBatchDraw(); //批量绘制 防止闪屏
        //2.2.8 进行绘制
        display();
    }
    openlist.clear(); //清理openlist
    EndBatchDraw(); //关闭批量绘图
}

// 2.3 对随机pick点进行扩展逐渐生成迷宫
void through(int x,int y){
    list.clear();
    //2.3.1 对点格的相间隔点进行判断 如果两点可以连通则在中间形成ROAD
    //上
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

    //2.3.2 生成随机点B
    int BRandomPoint = rand() % list.size();
    block* B = list[BRandomPoint];


    //2.3.3 将X Y 与B点连通
    MAZA[B->x][B->y] = ROAD;
}

//2.4
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


// 3. DFS深度优先算法寻路

//3.1 建立栈
//3.1.1 定义Node
typedef struct Node{
    int x;
    int y;
    struct Node *next;
}Node;

//3.1.2 定义Stack
typedef Node* Stack;

//3.1.3 建立栈函数
Stack createEmptyStack(){
    Stack p =(Stack)malloc(sizeof(Node)); //为空栈申请空间;
    if(p){
        p->next = NULL;
        return p;
    }
}

//3.1.4 元素入栈函数
void push(int x,int y,Stack s){
    Stack p =(Stack)malloc(sizeof(Node));
    if(p){ //if 申请成功则将元素压入栈中
        p->x = x;
        p->y = y;
        if(!s->next)
            p->next = NULL;     //？
        else p->next = s->next; //否则将p插入头节点之后
        s->next = p;
    }
   // else{
  //      cout<<"申请空间失败"<<endl;
}

//3.1.5 元素出栈函数
void pop(Stack s){
    Stack p;
    p = s->next;
    if(p->next){
        s->next = p->next;
        free(p);
    }
    else return;
}

//3.1.6 获取栈顶元素函数
Node top(Stack s){
    Node t;
    //判断栈是否为空 不为空则返回
    t=*(s->next);
    return t;
}

//3.1.7 检测栈是否为空（未使用）
int isEmpty(Stack s) {
    //为空则返回1，不为空返回0
    if (s->next == NULL) return 1;
    else return 0;
}

//3.2 DFS算法
//3.2.1 DFS初始化
int direction[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};//定义一个数组存四个方向操作数
int MIN=100;//用于记录最小路径
int flag=0;//用于记录是否寻路到终点


//3.2.2 DFS寻路函数
void MazaPathDFS(int x,int y,int endx,int endy,int Block_wid,int Block_hei, Stack s){
    int newx,newy,i;
    //对四个方向进行寻路
        for(i=0;i<4;i++) {
            newx = x + direction[i][0];
            newy = y + direction[i][1];
            if (newx >= 0 && newx < Block_wid && newy >= 0 && newy < Block_hei && MAZA[newx][newy] == 0) {
                //判断下一个点是否在地图外
                push(newx, newy, s);
                MAZA[newx][newy] = THEWAY;
                displaySearch();
                Sleep(10);
                if (newx == endx && newy == endy) {
                    //判断是否已经走到终点
                    flag++; //标志
                    displayPath();  //绘制路线
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


//4.可视化

//4.1 绘制像素格 基本定义
void drawpixel(int x,int y,COLORREF RGB)
{
    setfillcolor(RGB);
    setlinecolor(COLORREF RGB(0,0,0));
    fillrectangle(x*Block_xy,y*Block_xy,x*Block_xy + Block_xy,y*Block_xy+Block_xy);

}

//4.2 绘制函数 对二维数组进行遍历 绘制迷宫地图
void display(){
    for(int y = 0 ; y < Block_hei ; y++){
        for(int x = 0;x<Block_wid;x++)
        {

            if(MAZA[x][y] ==  WALL){    //绘制墙壁
                drawpixel(x,y,side);
            }
            else if(MAZA[x][y] == ROAD) {   //绘制路径
                drawpixel(x, y, air);
            }
            else if(MAZA[x][y] == WAIT){    //绘制路径预选格子
                drawpixel(x,y,PreSelect);
            }
        }

    }
    FlushBatchDraw();
}

//4.3 绘制路径 对分析过的二维数组再次进行遍历 绘制遍历的最终路线
void displayPath(){
    for(int y = 0; y < Block_hei ; y++){
        for(int x = 0;x<Block_wid;x++)
        {
            if(MAZA[x][y] == THEWAY) {  //绘制路线
                drawpixel(x, y, preway);
            }
        }
    }
    FlushBatchDraw();
}

//4.4 绘制遍历过程 绘制遍历时经过的路线
void displaySearch(){
    for(int y = 0; y < Block_hei ; y++){
        for(int x = 0;x<Block_wid;x++)
        {
            if(MAZA[x][y] == THEWAY) {
                drawpixel(x, y, way);
            }
            else if(MAZA[x][y] == PREWAY){
                drawpixel(x,y,preway);
            }
        }
    }
    FlushBatchDraw();
}

//5. DFS部分主函数
void initializePath(){ //初始化绘制路径
//5.1 初始化 起点 终点
    int xbegin  = 0;
    int ybegin  = 0;
    int xend = 50;
    int yend = 50;
//5.2 建立栈
    Stack s = createEmptyStack();
//5.3 执行DFS分析
    MazaPathDFS(xbegin,ybegin,xend,yend,Block_wid,Block_hei,s);
}




int main() {

    //1.生成迷宫
    srand((unsigned)time(NULL));//随机数种子
    initgraph(WIDTH, HEIGHT);//创建绘图窗口
    BeginBatchDraw();//开始批量绘图
    cleardevice();//清空显示
    initializeMaza(); //生成地图主函数


    //2.DFS迷宫寻路
    BeginBatchDraw();//开始批量绘图
    initializePath();//DFS寻找路径主函数
    getch();

    //3.BFS迷宫寻路


}