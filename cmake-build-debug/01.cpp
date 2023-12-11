
#include <iostream>
#include <vector>

// 定义块类
class Block {
public:
    int data;  // 块的数据

    // 构造函数
    Block(int value) : data(value) {}
    // 其他块的成员可以在这里添加
};

int main() {
    // 创建存储 Block 指针的 vector
    std::vector<Block*> openlist;

    // 创建块并添加到向量
    Block* b1 = new Block(1);
    openlist.push_back(b1);

    Block* b2 = new Block(2);
    openlist.push_back(b2);

    // 打印向量中的块数据
    for (size_t i = 0; i < openlist.size(); ++i) {
        std::cout << "Block " << i + 1 << ": " << openlist[i]->data << std::endl;
    }

    // 释放向量中的块内存
    for (Block* block : openlist) {
        delete block;
    }

    // 清空向量
    openlist.clear();

    return 0;
}
//
// Created by 86179 on 2023/12/9.
//
