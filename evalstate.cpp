#include "evalstate.h"

evalstate::evalstate()
{

}

//evalstate作为一个变量存储空间 主要对程序中的变量进行处理

//设置变量的值
void evalstate::set(QString var, int value){
    if(Variable.find(var)!=Variable.end()){
        Variable.erase(var);
        Variable.insert(pair<QString,int>(var,value));
    }
    else
        Variable.insert(pair<QString,int>(var,value));
}

void evalstate::stringset(QString var, QString value){
    if(stringVariable.find(var)!=stringVariable.end()){
        stringVariable.erase(var);
        stringVariable.insert(pair<QString,QString>(var,value));
    }
    else
        stringVariable.insert(pair<QString,QString>(var,value));
}

//获取变量的值
int evalstate::get(QString var){
    return Variable[var];
}

//清空所有变量
void evalstate::clear(){
    Variable.clear();
    stringVariable.clear();
}

//判断是否为已定义变量
bool evalstate::isDefined(QString var){
    return Variable.count(var);
}

bool evalstate::stringisDefined(QString var){
    return stringVariable.count(var);
}
