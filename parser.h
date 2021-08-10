#ifndef PARSER_H
#define PARSER_H

#include <stack>
#include <vector>
#include <string>
#include <sstream>
#include "tokenizer.h"
#include "exp.h"

using namespace std;

typedef struct BinaryNode
{
    QString data;
    BinaryNode *left;
    BinaryNode *right;
}TreeNode;



//解析给定表达式
//实现计算等功能
class parser
{
public:
    parser();
    TreeNode* toTree(QList<QString> tokens);
    stack<Expression*> exp;
    stack<QString> op;

    TreeNode* CreateTreeNode(QString ch){
        TreeNode *pNode = new TreeNode;
        pNode->data = ch;
        pNode->left = NULL;
        pNode->right = NULL;
        return pNode;
    }

    //判断表达式类型
    bool IsCompound(QString);
    bool IsConstant(QString);
    bool IsIdentifier(QString);

    //取值
    int constValue(QString);
    int IdenValue(QString exp,evalstate &vars);

    //求值函数
    vector<QString> toPostfix(QList<QString>token);
    int evalRPN(vector<QString>& t,evalstate &vars);
    bool isNumber(string& token);
    bool exchange(vector<QString> token,evalstate &vars);
    bool isNumberorminus(string& token);

    //区分优先级
    int getLevel(QString);

    //判断运算符
    bool IsMulDiv(QString);
    bool IsAddSub(QString);
    bool IsPow(QString);
    bool IsEqu(QString);
    bool IsLeft(QString);
    bool IsRight(QString);



};

#endif // PARSER_H
