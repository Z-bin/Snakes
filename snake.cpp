#pragma execution_character_set("utf-8")

#include "snake.h"
#include <time.h>
#include <QApplication>
#include <QPixmap>
#include <QMainWindow>
#include <QLabel>
#include <Windows.h>
#include <QStyle>
#include <QtDebug>
#include <QMessageBox>
#include <QStyle>
#include <QTime>
#include <QEvent>
#include <QKeyEvent>

Snake::Snake(QWidget *parent) :
    QMainWindow(parent)
{
    score=0;
    level=1;       //初始化分数等级

    this->setWindowTitle("贪吃蛇");
    this->resize((MAX_X+7)*BLOCK_SIZE,MAX_Y*BLOCK_SIZE);

    startGame=new QPushButton("开始游戏\nBY 周斌",this);
    startGame->setStyleSheet("QPushButton{ background-color: rgb(255,0,0); border-radius: 30px; font: bold; color: white; }");

    QFont fontBtn("Consolas",13);  //字体类型
    startGame->setFont(fontBtn);
    startGame->resize(160,80);    //大小位置
    startGame->move(MAX_X * BLOCK_SIZE / 2 - 80,MAX_Y * BLOCK_SIZE / 2 - 40);

    //level score标签
    mLabel=new QLabel(this);
    mLabel->setGeometry(MAX_X * BLOCK_SIZE,BLOCK_SIZE,6 * BLOCK_SIZE,(MAX_Y - 2) * BLOCK_SIZE);
    QFont fontview("Consolas",16);
    mLabel->setFont(fontview);
    mLabel->setAlignment(Qt::AlignCenter);

    viewText=QString::number(score,10);
    mLabel->setText("分数:"+viewText+'\n'+"等级:"+QString::number(level,10));

    init(); //初始化游戏窗口
    drawBorder();//边界
    initSnake();//初始化蛇
    drawViewArea();//分数区域

    QObject::connect(&timer,SIGNAL(timeout()),this,SLOT(snakeMoveSlots()));
    QObject::connect(startGame,SIGNAL(clicked()),this,SLOT(startGameSlots()));
}

//初始化游戏区域
void Snake::init()
{
    foodCount=0;
    moveSpeed=200;
    for(int x=0;x<MAX_X;x++)
        for(int y=0;y<MAX_Y;y++)
        {
            QLabel *label=new QLabel(this);
            label->setGeometry(x*BLOCK_SIZE,y*BLOCK_SIZE,BLOCK_SIZE,BLOCK_SIZE);
            label->hide();

            matrix[x][y]=new snakeNode;
            matrix[x][y]->label=label;
            matrix[x][y]->type=NORMAL_LABEL;
            matrix[x][y]->x=x;
            matrix[x][y]->y=y;
        }
}

//画边界
void Snake::drawBorder()
{
    for(int x=0;x<MAX_X;x++)
        for(int y=0;y<MAX_Y;y++)
        {
            if(x==0||y==0||x==MAX_X-1||y==MAX_Y-1)
            {
                matrix[x][y]->label->setStyleSheet("background:gray");
                matrix[x][y]->label->show();
                matrix[x][y]->type=BORDER_LABEL;
            }
        }
}
//初始化蛇
void Snake::initSnake()
{
    //蛇长 初始化位置
    int snakeLen=3;
    int snakeX=5;
    int snakeY=5;

    directionX=1;
    directionY=0;
    snake.clear();
    for(int x=snakeX;x<snakeX+snakeLen;x++)
    {
        snake.append(matrix[x][snakeY]);
        snake.at(snake.length()-1)->x=x;
        snake.at(snake.length()-1)->y=snakeY;
        matrix[x][snakeY]->type=SNAKE_LABEL;
        matrix[x][snakeY]->label->setStyleSheet("background:pink");
        matrix[x][snakeY]->label->show();
    }
}
//分数区域
void Snake::drawViewArea()
{
      for(int x=MAX_X;x<MAX_X+7;x++)
          for(int y=0;y<MAX_Y;y+=MAX_Y-1)
          {
              QLabel *label=new QLabel(this);
              label->setStyleSheet("background:gray");
              label->setGeometry(x*BLOCK_SIZE,y*BLOCK_SIZE,BLOCK_SIZE,BLOCK_SIZE);
              label->show();
          }
      for(int y=1;y<MAX_Y-1;y++)
      {
          QLabel *label=new QLabel(this);
          label->setStyleSheet("background:gray");;
          label->setGeometry((MAX_X+6)*BLOCK_SIZE,y*BLOCK_SIZE,BLOCK_SIZE,BLOCK_SIZE);
          label->show();
      }
}
//移动操作
void Snake::moveSnake()
{
    getHeadTail();
    // 由键盘事件获得的方向来确定蛇头的位置
    snakeNode *tempNode=matrix[head->x+directionX][head->y+directionY];
    tempNode->label->setStyleSheet("background:pink");
    tempNode->label->show();
    //检测是否撞到自己或边界
    if(tempNode->type==BORDER_LABEL||tempNode->type==SNAKE_LABEL)
    {
        tempNode->label->setStyleSheet("background:gray");
        gameover();
    }
    else
    {
        if(tempNode->type==FOOD_LABEL)
        {
            showViewArea();
            tempNode->type=SNAKE_LABEL;
            snake.append(tempNode);
            createFood();
        }
        else
        {
            tempNode->type=SNAKE_LABEL;
            snake.append(tempNode);
            tail->label->hide();
            tail->type=NORMAL_LABEL;
            snake.removeFirst();
        }
    }
}

//获取蛇的头尾指针
void Snake::getHeadTail()
{
    head=snake.at(snake.length()-1);
    tail=snake.at(0);
}

//创建食物
void Snake::createFood()
{
    int foodx;
    int foody;
    srand((unsigned)time(0));
    do
    {
        foodx=rand()%MAX_X;
        foody=rand()%MAX_Y;
    }while(matrix[foodx][foody]->type!=NORMAL_LABEL);
    matrix[foodx][foody]->type=FOOD_LABEL;
    matrix[foodx][foody]->label->setStyleSheet("background:red");
    matrix[foodx][foody]->label->show();
}

//游戏结束
void Snake::gameover()
{
    score=0;
    level=1;
    foodCount=0;
    moveSpeed=200;
    timer.stop();//计时器停止
    QMessageBox::information(this,"Message:","游戏结束!",QMessageBox::Ok);
    //刷新布局
    for(int x=1;x<MAX_X-1;x++)
        for(int y=1;y<MAX_Y-1;y++)
        {
            matrix[x][y]->type=NORMAL_LABEL;
            matrix[x][y]->label->hide();
        }
    drawBorder();
    //重新开始
    startGame->setEnabled(true);
    startGame->show();
}

void Snake::showViewArea()
{
    foodCount++;
    level=(foodCount/10)+1;
    if(level>=8)
        level=8;
    moveSpeed=100*(9-level);
    score=level*foodCount;
    viewText=QString::number(score,10);
    mLabel->setText("分数:"+viewText+'\n'+"等级:"+QString::number(level,10));
    mLabel->setAlignment(Qt::AlignCenter);
}

bool Snake::up()
{
    if(directionX==0)
        return false;
    return true;
}

bool Snake::down()
{
    if(directionX==0)
        return false;
    return true;
}

bool Snake::left()
{
    if(directionY==0)
        return false;
    return true;
}

bool Snake::right()
{
    if(directionY==0)
        return false;
    return true;
}

Snake::~Snake()
{

}

void Snake::snakeMoveSlots()
{
    moveSnake();
}

void Snake::startGameSlots()
{
    drawBorder();
    initSnake();
    moveSnake();
    createFood();

    viewText=QString::number(score,10);
    mLabel->setText("分数:"+viewText+'\n'+"等级:"+QString::number(level,10));
    //开始移动
    timer.start(moveSpeed);

    //设置按钮不可用
    startGame->setEnabled(false);
    startGame->hide();
}

//键盘事件
void Snake::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:
        if(up())
        {
            directionX=0;
            directionY=-1;
            moveSnake();
        }
        break;
    case Qt::Key_Down:
        if(down())
        {
            directionX=0;
            directionY=1;
            moveSnake();
        }
        break;
    case Qt::Key_Right:
        if(right())
        {
            directionX=1;
            directionY=0;
            moveSnake();
        }
        break;
    case Qt::Key_Left:
        if(left())
        {
            directionX=-1;
            directionY=0;
            moveSnake();
        }
        break;
    case Qt::Key_Space:
        if(timer.isActive())
            timer.stop();
        else
            timer.start();
        break;
    default:
        break;
    }
}
