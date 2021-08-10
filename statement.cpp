#include "statement.h"

statement::statement(){
    next=nullptr;
}

//拆分语句
statement::statement(QString state,statement* n){
    text=state;
    next=n;
    QList<QString> tmp = text.split(' ');
    linenum = tmp[0].toInt();
}

//覆盖语句
void statement::rewrite(QString newtext){
    text = newtext;
    QList<QString> tmp=text.split(' ');
    linenum=tmp[0].toInt();
}
