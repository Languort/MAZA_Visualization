#include <Windows.h>
#include"easyx/include/easyx.h"

#include <conio.h>

#include<ctime>
#include<stack>
#include<vector>
#include<cmath>
#include<algorithm>

using namespace std;

#define WIDTH 510  //画面宽度
#define HEIGHT 510  //画面高度
#define Block_xy 10 //方格边长

#define ROAD 0
#define WALL 1
#define WAIT 2

#define THEWAY 3
#define PREWAY 4

const int Block_wid = WIDTH / Block_xy; // x轴方格个数
const int Block_hei = HEIGHT / Block_xy; // y轴方格个数

#define air COLORREF RGB(50,50,50) //空气颜色
#define side COLORREF RGB(255,255,255)//墙壁颜色
#define PreSelect COLORREF RGB(0,120,255)  //预选颜色
#define Select COLORREF RGB(255,0,0)   //选中颜色
#define preway COLORREF RGB(0,255,0) //预选路颜色
#define way COLORREF RGB(0,100,0)// 寻路路径颜色


struct block{
public:
    int x;
    int y;
    block(int x, int y) {
        this->x = x;
        this->y = y;
    }
};



int MAZA[Block_wid][Block_hei];

//未学 动态数组
vector<block*> openlist;//待选列表
vector<block*> list;



/*函数声明*/
void clearscreen(COLORREF RGB);//清空地图为RGB颜色(带有网格)
void drawpixel(int x, int y, COLORREF RGB);//在x，y位置画颜色为RGB的方格
void initializeMaza();
//初始化
void join(int x, int y);//将x,y四周是墙壁的路点加入待选列表
void display();//刷新屏幕
void prim_main();//prim主循环
void through(int x, int y);//将x,y与它四周一个随机已经变成路的路点打通

void displayPath();
void displaySearch();
/*********/



//最小生成树 Prim版
//初始化
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

    int BRandomPoint = rand() % list.size();
    block* B = list[BRandomPoint];

    // 将X Y 与B点连通
    MAZA[B->x][B->y] = ROAD;
}

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



typedef struct Node{
    int x;
    int y;
    struct Node *next;
}Node;

//创建一个栈
typedef Node* Stack;

//栈函数：
Stack createEmptyStack(){
    Stack p =(Stack)malloc(sizeof(Node)); //为空栈申请空间;
    if(p){
        p->next = NULL;
        return p;
    }
}

//元素入栈
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

//元素出栈
void pop(Stack s){
    Stack p;
    p = s->next;
    if(p->next){
        s->next = p->next;
        free(p);
    }
    else return;
}

//获取栈顶元素
Node top(Stack s){
    Node t;
    //判断栈是否为空 不为空则返回
    t=*(s->next);
    return t;
}

//检测栈是否为空
int isEmpty(Stack s) {
    //为空则返回1，不为空返回0
    if (s->next == NULL) return 1;
    else return 0;
}


int direction[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};  //定义一个数组存四个方向操作数
int MIN=100;//用于记录最小路径
int flag=0;


//遍历迷宫寻找路径 DFS
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
                    flag++;
                    displayPath();

                    MAZA[newx][newy] = 0;   //将终点设为WALL
                    pop(s);     //未理解
                 //   return;
                }
                else {
                    MazaPathDFS(newx, newy, endx, endy, Block_wid, Block_hei, s); //自我嵌套实现递归
                    if (flag > 0) {
                        return;
                    }
                }

            }
       }

    //displayPath();  //绘制路径
    // 遍历完四个方向之后回溯前将自己赋为空
    MAZA[x][y] = 0;
    pop(s);


}



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

void displayPath(){
    //  FlushBatchDraw();
    //绘制路径
    for(int y = 0; y < Block_hei ; y++){
        for(int x = 0;x<Block_wid;x++)
        {
            if(MAZA[x][y] == THEWAY) {
                drawpixel(x, y, preway);
            }

        }

    }
    FlushBatchDraw();
}

void displaySearch(){
    //  FlushBatchDraw();
    //绘制路径
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




////直接生成绘制地图
//void drawMaza(int Maza[Block_wid][Block_hei]){
//    //先将二维数组全部设置为墙
//    for(int i = 0 ;i <= Block_wid;i++)
//    {
//        for (int j = 0;j <= Block_hei;j++){
//            if (Maza[i][j] = WALL) {
//                setfillcolor(BLACK);    //黑色表示墙壁
//            }
//            else if(Maza[i][j] = ROAD)
//            {
//                setfillcolor(WHITE);    //白色表示路径
//            }
//            //以四个定位点绘制正方形
//            fillrectangle(j * Block_xy, i * Block_xy, (j + 1) * Block_xy, (i + 1) * Block_xy);
//        }
//    }
//}

void drawpixel(int x,int y,COLORREF RGB)
{
    setfillcolor(RGB);
    setlinecolor(COLORREF RGB(0,0,0));
    fillrectangle(x*Block_xy,y*Block_xy,x*Block_xy + Block_xy,y*Block_xy+Block_xy);

}

void initializePath(){ //初始化绘制路径

    int xbegin  = 0;
    int ybegin  = 0;
    int xend = 50;
    int yend = 50;

    Stack s = createEmptyStack();

    MazaPathDFS(xbegin,ybegin,xend,yend,Block_wid,Block_hei,s);

    for(int i = 0; i<Block_wid;i++){
        for(int j =0;j<Block_hei;j++){
            if(MAZA[i][j] == THEWAY) {
                displayPath();
            }

            }
        }



}




int main() {


    //生成迷宫
    srand((unsigned)time(NULL));//随机数种子
    initgraph(WIDTH, HEIGHT);//创建绘图窗口
    BeginBatchDraw();//开始批量绘图
    cleardevice();//清空显示
    initializeMaza();
    //EndBatchDraw();

    //迷宫寻路
    BeginBatchDraw();//开始批量绘图
    initializePath();
    displayPath();
    getch();
//    EndBatchDraw();


//
//    display();
//    FlushBatchDraw();//将缓冲输出到屏幕
//
//    while(1);

}





