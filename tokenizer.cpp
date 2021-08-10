#include "tokenizer.h"

tokenizer::tokenizer()
{

}

//将表达式转换成列表方便建树
QList<QString>tokenizer::toTokens(QString expression){
    Tokens.clear();
    //去除表达式中的空格
    expression.remove(QChar(' '), Qt::CaseInsensitive);

    //j记录符号位置
    int j=-1;
    for(int i=0;i<expression.length();++i){
        if(isOp(expression[i])){
            string exp=expression.toStdString();
            string tmp;
            //截取运算符之间的内容
                tmp=exp.substr(unsigned(j+1),unsigned(i-1-j));
            QString qtmp = QString::fromStdString(tmp);
            if(qtmp!="")
                Tokens.append(qtmp);
            //判断是否为特殊运算符**
            if((expression[i]=="*")&&(expression[i+1]=="*")){
                QString tm="**";
                Tokens.append(tm);
                i++;
                j=i;
            }
            else{
                Tokens.append(QString(expression[i]));
                j=i;
            }
        }
        else continue;
    }

    //把最后剩下的部分也填进去
    //若表达式中存在符号
    if(j!=-1){
        string exp=expression.toStdString();
        string tmp;
        tmp=exp.substr(unsigned(j+1),unsigned(expression.length()-1));
        QString Qtmp=QString::fromStdString(tmp);
        Tokens.append(Qtmp);
    }
    //若表达式里没有符号
    else{
        Tokens.append(expression);
    }
    return Tokens;
}

//判断是否为运算符
bool tokenizer::isOp(QChar x){
    if((x=="+")||(x=="-")||(x=="*")||(x=="/")||(x=="=")||(x=="(")||(x==")"))
        return true;
    return false;
}
