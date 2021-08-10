#ifndef EVALSTATE_H
#define EVALSTATE_H

#include<map>
#include<QString>

using namespace std;

//存储所有变量的空间
class evalstate
{
public:
    map<QString,int> Variable;
    map<QString,QString> stringVariable;
    QString contaning;
    evalstate();
    //设置变量的值
    void set(QString var,int value);
    void stringset(QString var,QString value);
    //获取变量的值
    int get (QString var);
    bool isDefined (QString var);
    bool stringisDefined(QString var);
    void clear();
    //QString traverse();
};

#endif // EVALSTATE_H
