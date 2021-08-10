#ifndef TOKENIZER_H
#define TOKENIZER_H

# include <QString>
# include <QList>
# include <string>

using namespace std;

//将表达式转换成一节一节的
class tokenizer
{
public:
    tokenizer();
    QList<QString>toTokens(QString expression);
    bool isOp(QChar a);
    bool isCom(QString a);
    bool isConstantExp(QString a);
    QList<QString>Tokens;
};

#endif // TOKENIZER_H
