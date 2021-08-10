#ifndef STATEMENT_H
#define STATEMENT_H

# include<QString>
# include<string>
# include<QList>
using namespace std;

//对于语句的一些操作
//主要的语句功能在Program中
class statement
{
public:
    statement();
    statement(QString state,statement* n = nullptr);
    void rewrite(QString text);


    int linenum=0;
    QString text;
    statement *next;
};
#endif // STATEMENT_H
