#include "parser.h"

parser::parser()
{

}

//判断类型
bool parser::IsIdentifier(QString exp){
    if((exp[0]>="A" && exp[0]<="Z")||(exp[0]>="a" && exp[0]<="z"))
        return true;
    return false;
}

//取值
int parser::IdenValue(QString exp,evalstate &vars){
    int result;
    result=vars.get(exp);
    return result;
}

//区分优先级
int parser::getLevel(QString exp){
    if(exp=="=")
        return 1;
    else if(exp=="+"||exp=="-")
        return 2;
    else if(exp=="*"||exp=="/")
        return 3;
    else if(exp=="**")
        return 4;
    else
        return 0;//return 0 时发生异常
}

//判断运算符
bool parser::IsMulDiv(QString exp){
    if(exp=="*"||exp=="/")
        return true;
    return false;
}
bool parser::IsAddSub(QString exp){
    if(exp=="+"||exp=="-")
        return true;
    return false;
}
bool parser::IsPow(QString exp){
    if(exp=="**")
        return true;
    return false;
}
bool parser::IsEqu(QString exp){
    if(exp=="=")
        return true;
    return false;
}
bool parser::IsLeft(QString exp){
    if(exp == "(")
        return true;
    return false;
}
bool parser::IsRight(QString exp){
    if(exp == ")")
        return true;
    return false;
}

//将中缀表达式变成后缀表达式
vector<QString> parser::toPostfix( QList<QString>token){
    //QList<QString>token;     //当前读取的字符
    QString s;
    stack<QString> opStack;
    //目标后缀表达式
    vector<QString> exp;
    //QString postfixExp;
    for(int i=0;i<token.length();i++){
        if(IsAddSub(token[i])||IsMulDiv(token[i])||IsPow(token[i])||IsLeft(token[i])||IsRight(token[i])){
            if(IsLeft(token[i]))
                opStack.push(token[i]); //是 ( 则放入栈中
            else if(IsRight(token[i])){
                //将与之匹配的（之前的所有元素出栈
                while(opStack.top()!='('){
                    s=opStack.top();
                    exp.push_back(s);
                    opStack.pop();
                }
                //处理一下负号
                if(s=="-"){
                    exp.pop_back();
                    QString tmpnum="-"+exp.back();
                    exp.pop_back();
                    exp.push_back(tmpnum);
                }
                opStack.pop();
            }
            //若栈为空 或 当前运算符优先级更高 则直接入栈
            else if(opStack.empty()||getLevel(token[i])>getLevel(opStack.top())){
                opStack.push(token[i]);
            }
            //若优先级低于或等于栈顶元素
            else if(getLevel(token[i])<=getLevel(opStack.top())){
                s=opStack.top();
                //postfixExp.append(s);
                exp.push_back(s);
                opStack.pop();
                while(!opStack.empty()&&(getLevel(opStack.top())>=getLevel(token[i]))){
                    s=opStack.top();
                    //postfixExp.append(s);
                    exp.push_back(s);
                    opStack.pop();
                }
                opStack.push(token[i]);
            }
        }
        //若为操作数则直接输出
        else{
            s=token[i];
            //postfixExp.append(s);
            if(s!="")
                exp.push_back(s);
        }
    }
    //将栈中剩余的元素依次输出
    while(!opStack.empty()){
        s=opStack.top();
        exp.push_back(s);
        opStack.pop();
    }
    return exp;
};

//判断有无未定义变量
bool parser::exchange(vector<QString> token,evalstate &vars){
    int size=token.size();
    for(int i=0;i<size;i++){
        if(IsIdentifier(token[i])){
            if(vars.isDefined(token[i])){
                QString t=QString::number(IdenValue(token[i],vars));
                token[i]=t;
                return true;
            }
            else
                return false;
        }
    }
    return true;
}

//计算后缀表达式
int parser::evalRPN(vector<QString>& t,evalstate &vars) {
    //把后缀表达式中的所有变量变成其值
    int wsize=t.size();
    for(int i=0;i<wsize;i++)
        if(IsIdentifier(t[i])){
            if(vars.isDefined(t[i])){
                QString tm=QString::number(IdenValue(t[i],vars));
                t[i]=tm;
            }
        }
    //开始计算后缀表达式
    stack<int> stk;
    int size=t.size();
    vector<string> tokens;
    for(int i=0;i<size;i++)
        if(t[i]!="")
            tokens.push_back(t[i].toStdString());

    int n = tokens.size();
    for (int i = 0; i < n; i++) {
        string& token = tokens[i];
        if (isNumber(token)) {
            stk.push(atoi(token.c_str()));
        } else {
            //解决一下负号问题
            //这个是表达式头部的负号
            if(token=="-"&&stk.size()==1){
                int numtmp=stk.top();
                stk.pop();
                numtmp=0-numtmp;
                stk.push(numtmp);
            }
            //这个是带入数值中的负号
            else if(token[0]=='-'&&token.size()!=1){
                int numtmp=atoi(token.c_str());
                stk.push(numtmp);
            }
            else{
                int num2 = stk.top();
                stk.pop();
                int num1 = stk.top();
                stk.pop();
                switch (token[0]) {
                case '+':
                    stk.push(num1 + num2);
                    break;
                case '-':
                    stk.push(num1 - num2);
                    break;
                case '*':{
                    if(token[0]=='*'&&token[1]=='*'){
                        stk.push(pow(num1,num2));
                        break;
                    }
                    else{
                        stk.push(num1 * num2);
                        break;
                    }
                }

                case '/':
                    stk.push(num1 / num2);
                    break;
                }
            }
        }
    }
    return stk.top();
}

//判断一下是不是数字
bool parser::isNumber(string& token) {
    int size=token.size();
    for(int i=0;i<size;i++){
        if (((token.at(i)>'9') || (token.at(i)<'0'))){
            return false;
        }
    }
    return true;
}

//建树
TreeNode* parser::toTree(QList<QString> tokens){
    stack<TreeNode *> nodeStack;
    QString ch;
    int size=tokens.size();
    for(int i=0;i<size;i++){
        ch=tokens[i];
        TreeNode *pNode = CreateTreeNode(ch);
        if (ch =="+" || ch == "-" || ch == "*" || ch == "/"||ch=="**"){
            TreeNode *pRight = nodeStack.top();
            nodeStack.pop();
            TreeNode *pLeft = nodeStack.top();
            nodeStack.pop();
            pNode->right = pRight;
            pNode->left = pLeft;
            nodeStack.push(pNode);
        }
        else{
            nodeStack.push(pNode);
        }
    }
    return nodeStack.top();
}
