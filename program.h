#ifndef PROGRAM_H
#define PROGRAM_H
#include "statement.h"
#include<QList>
#include "evalstate.h"
#include "parser.h"
#include "tokenizer.h"
#include <map>

//将程序拆分为语句跑起来
//INPUT 指令由于其特殊性放在MainWindow里面实现
class program
{
public:
    program();
    //寻找行号所在位置
    statement* dst(int num) ;
    void insert(int num,QString newtext);
    QString visit(int num);
    void clear();
    void del(int num);
    statement* find(int num);
    statement* findnext(int num);

    int length=0;
    statement *head;
    map<int,statement*> pro;
    parser* Parser;
    tokenizer* Tokenizer;
    bool isError=false;


    void cmdRem(evalstate &var,statement*cur);
    void cmdLet(evalstate &var,statement*cur);
    QString cmdPrint(evalstate &var,statement*cur);
    QString cmdGoto(evalstate &var,statement*cur);
    void cmdIf(evalstate &var,statement*cur);
    void cmdEnd(evalstate &var,statement*cur);
    QString cmdPrintf(evalstate &var,statement*cur);


    //确定行号是否存在，false为已存在，true为新行号
    bool notexist=true;
};

#endif // PROGRAM_H
