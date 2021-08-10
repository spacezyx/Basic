#include "program.h"
#include "parser.h"

program::program()
{
    head=new statement;
    length=0;
    Tokenizer=new tokenizer;
    Parser=new parser;
}

//program用来链接并执行每一行语句
//由于程序中存在诸多跳转 用map+指针形式可能比用list更好维护
//每个语句的next指针在运行过程中会变化 可能导致一些语句在指向过程中的丢失 多次RUN时需要注意这个问题

//清空program
void program::clear(){
    statement *p=head->next;
    statement *q=head;
    while(p!=nullptr)
    {
        q=p;
        delete p;
        p=q;
        p=q->next;
    }
    pro.clear();
}


//删除某行
void program::del(int num){
    bool flag=false;
    if(pro.find(num)!=pro.end()){
        flag=true;
        pro.erase(num);
    }
    return;
}

//找到行号所在位置
statement* program::dst(int num){
    if(pro.find(num)!=pro.end()){
        notexist=false;
        return pro.find(num)->second;
    }
    if(pro.size()==0||pro.begin()->second->linenum>num){
        notexist=true;
        return head;
    }
    else{
        auto p=pro.begin();
        p++;
        auto q=pro.begin();
        while(p->second->linenum<num){
            if(p++!=pro.end())
                q++;
            else
                return p->second;
        }
        notexist=true;
        return q->second;
    }
}

void program::insert(int num,QString newtext){
        dst(num);
        //如果行号已经存在则替换
        if(notexist==false){
            statement *tmp=dst(num);
            tmp->text=newtext;
            QList<QString> tmplist=newtext.split(' ');
            int tmplinenum=tmplist[0].toInt();
            tmp->linenum=tmplinenum;
            pro.erase(num);
            pro.insert(pair<int,statement*>(num,tmp));
        }
        //添加新行号
        else{
            statement *tmp=dst(num);
            statement *tmpq=tmp;
            if(tmp->next==nullptr){
               tmp->next=new statement(newtext,nullptr);
               length++;
            }else{
                tmp->next=new statement(newtext,tmpq->next);
                length++;
            }
            pro.insert(pair<int,statement*>(num,tmp->next));
       }
}

//找到某行
statement* program::find(int num){
    if(pro.find(num)!=pro.end()){
        statement* tmp=pro.find(num)->second;
        return tmp;
    }
    else
        return nullptr;
}

//找到某行的下一行
statement* program::findnext(int num){
    if(pro.find(num)!=pro.end()){
        auto iter=pro.find(num);
        iter++;
        statement* tmp=iter->second;
        return tmp;
    }
    else
        return nullptr;
}

//指令
void program::cmdRem(evalstate &var,statement*cur){
    if(cur->next)
        cur=cur->next;
}

void program::cmdLet(evalstate &var,statement*cur){
    QList<QString> tmp = cur->text.split(' ');
    //int tmpsize=tmp.size();
    QString state = "";

    //获取一个单纯的表达式
    for(int i=2;i <tmp.length();++i)
        state += tmp[i];
    QString variableName="";
    QString valexp="";
    int j=0;
    //判断一下是否存在等号
    if(!cur->text.contains("=")){
        isError=true;
        cur->text=QString::number(cur->linenum,10)+" Error";
        cur->next=nullptr;
        return;
    }
    for(;state[j]!="=";j++)
        variableName +=state[j];
    j++;


    QString stringexp="";
    //字符变量
    if(((state[j]=="\"")&&(state[state.size()-1]=="\""))||((state[j]=="'")&&(state[state.size()-1]=="\'"))){
        int s=state.size();
        j++;
        for(;j<s-1;j++)
            stringexp +=state[j];
        var.stringset(variableName,stringexp);
    }
    else if(((state[j]=="\"")&&(state[state.size()-1]=="'"))||((state[j]=="'")&&(state[state.size()-1]=="\""))){
        cur->text=QString::number(cur->linenum,10)+" Error";
        isError=true;
    }
    //数值变量
    else{
        int s=state.size();
        for(;j<s;j++)
            valexp +=state[j];
        int num=0;
        QList<QString>tokens;
        tokens.clear();
        tokens = Tokenizer->toTokens(valexp);
        //测试后缀表达式
        vector<QString> a=Parser->toPostfix(tokens);
        //int size=a.size();

        //后缀表达式求值
        //是否所有变量均已定义
        if(!Parser->exchange(a,var)){
            //QString err="undefined variable";
            cur->text=QString::number(cur->linenum,10)+" Error";
            isError=true;
        }
        else{
            Parser->exchange(a,var);
            num=Parser->evalRPN(a,var);;
            var.set(variableName,num);
        }
    }
}

QString program::cmdPrint(evalstate &var,statement*cur){
    QList<QString> tmp = cur->text.split(' ');
    QString state = "";
    //获取一个单纯的表达式
    for(int i=2;i <tmp.length();++i){
        state += tmp[i];
    }
    int num=0;
    QList<QString>tokens;
    tokens.clear();
    tokens = Tokenizer->toTokens(state);
    //测试后缀表达式
    vector<QString> a=Parser->toPostfix(tokens);
    //int size=a.size();
    //后缀表达式求值
    if(!Parser->exchange(a,var)){
        QString err="undefined variable";
        cur->text=QString::number(cur->linenum,10)+" Error";
        isError=true;
        return err;
    }
    else{
        Parser->exchange(a,var);
        num=Parser->evalRPN(a,var);
        QString out = QString::number(num,10);
        return  out;
    }
}
//void program::cmdInput(evalstate &var,statement*cur){
//    //在mainwindow里面
//}

QString program::cmdGoto(evalstate &var,statement*cur){
    QList<QString> tmp = cur->text.split(' ');
    //QString state = "";
    QString numstate = tmp[2];
    int num=numstate.toInt();
    if(find(num)){
        cur->next=find(num);
        return "";
    }
    else{
        QString err="the linenumber is not exsit";
        cur->text=QString::number(cur->linenum,10)+" Error";
        isError=true;
        return err;
    }
}

void program::cmdIf(evalstate &var,statement*cur){
    QList<QString> tmp = cur->text.split(' ');
    QString exp1 = "";
    QString exp2 = "";
    QString op = "";
    QString dst="";
    QString state="";

    //获取一个单纯的表达式
    int i=2;

    for(;tmp[i]!="THEN";++i)
        state += tmp[i];
    i++;
    dst=tmp[i];


    //拆出exp1,exp2
    int stsize=state.size();
    int j=0;
    for(;state[j]!="<"&&state[j]!=">"&&state[j]!="=";j++)
            exp1 +=state[j];
    op=state[j];
    j++;
    for(;j<stsize;j++)
        exp2 +=state[j];
    int num1=0;
    int num2=0;
    QList<QString>tokens;
    tokens.clear();
    tokens = Tokenizer->toTokens(exp1);
    //测试后缀表达式
    vector<QString> a1=Parser->toPostfix(tokens);
   // int size1=a1.size();
    //后缀表达式求值
    if(!Parser->exchange(a1,var)){
        QString err="undefined variable";
        cur->text=QString::number(cur->linenum,10)+" Error";
        isError=true;
        //return err;
    }
    else{
        Parser->exchange(a1,var);
        num1=Parser->evalRPN(a1,var);
    }
    tokens.clear();
    tokens = Tokenizer->toTokens(exp2);
    //测试后缀表达式
    vector<QString> a2=Parser->toPostfix(tokens);
    //int size2=a2.size();
    //后缀表达式求值
    if(!Parser->exchange(a2,var)){
        QString err="undefined variable";
        cur->text=QString::number(cur->linenum,10)+" Error";
        isError=true;
        //return err;
    }
    else{
        Parser->exchange(a2,var);
        num2=Parser->evalRPN(a2,var);
    }
    bool flag=false;
    if(op=="<")
        flag=(num1<num2)?true:false;
    else if(op==">")
        flag=(num1>num2)?true:false;
    else if(op=="=")
        flag=(num1==num2)?true:false;

    if(flag){
        int num=dst.toInt();
        if(find(num)){
            cur->next=find(num);
            return;
        }
        else{
            cur->text=QString::number(cur->linenum,10)+" Error";
            isError=true;
        }

    }
    else{
        if(findnext(cur->linenum))
            cur->next=findnext(cur->linenum);
        else{
            cur->text=QString::number(cur->linenum,10)+" Error";
            isError=true;
        }
    }
}
void program::cmdEnd(evalstate &var,statement*cur){
    cur->next=nullptr;
}

QString program::cmdPrintf(evalstate &var,statement*cur){
    QList<QString> tmp = cur->text.split(' ');
    int count=0;
    int varcount=0;
    QString varstring=0;
    QString text="";
    int stringsize=tmp.size();
    for(int i=2;i<stringsize-1;i++){
        text=text+tmp[i]+" ";
    }
    //去除行首空格字符
    text.replace(QRegExp("^ "), "");
    text +=tmp[stringsize-1];
    int i=0;
    QString tmpstring="";
    int textsize=text.size();
    if(text[0]!='"'&&text[0]!='\''){
        isError=true;
        cur->text=QString::number(cur->linenum,10)+" Error";
        return "";
    }
    //把格式化字符串读入
    while(i<textsize){
        if(isError){
            isError=true;
            cur->text=QString::number(cur->linenum,10)+" Error";
            return "";
        }
        else if(text[i]=='"'){
            i++;
            if(i==textsize)
                break;
            while(text[i]!='"'){
                if(text[i]=='\''){
                    isError=true;
                    cur->text=QString::number(cur->linenum,10)+" Error";
                    break;
                }
                else if(text[i]=='{'&&text[i+1]=='}'){
                    //放个占位符
                    tmpstring +="%"+QString::number(count,10);

                    tmpstring += " ";
                    i=i+2;
                    count++;
                }
                else{
                    tmpstring +=text[i];
                    i++;
                }

            }
            if((i!=textsize-1)&&(text[i+1]==',')){
                i=i+2;
                break;
            }
        }
        else if(text[i]=='\''){
            i++;
            if(i==textsize)
                break;
            while(text[i]!='\''){
                if(text[i]=='"'){
                    cur->text=QString::number(cur->linenum,10)+" Error";
                    isError=true;
                    break;
                }
                else if(text[i]=='{'&&text[i+1]=='}'){
                    //放个占位符
                    tmpstring +="%"+QString::number(count,10);
                    tmpstring += " ";
                    i=i+2;
                    count++;
                }
                else{
                    tmpstring +=text[i];
                    i++;
                }
            }
            //开始读入参数
            if((i!=textsize-1)&&(text[i+1]==',')){
                i=i+2;
                break;
            }
        }
        else{
            tmpstring += text[i];
            i++;
        }
    }
    //开始处理参数
    while(i<textsize){
        varstring +=text[i];
        i++;
    }
    varstring.remove(QRegExp("\\s"));
    QList<QString> varvec;
    if(text.contains(","))
        varvec = varstring.split(",");//QString字符串分割函数
    if(varvec.size()!=count){
        isError=true;
        return "";
    }
    else{
        for(int i=0;i<varvec.size();i++){
            QString t=varvec[i];
            if(((t[0]=="\"")&&t[t.size()-1]=="\"")||((t[0]=="'")&&t[t.size()-1]=="'")){
                int s=t.size();
                QString m="";
                for(int j=1;j<s-1;j++)
                    m +=t[j];
                varvec[i]=m;
            }
            else if(var.isDefined(varvec[i])){
                varvec[i]=QString::number(var.Variable[varvec[i]],10);
            }
            else if(var.stringisDefined(varvec[i])){
                varvec[i]=var.stringVariable[varvec[i]];
            }
        }
        for(int i=0;i<varvec.size();i++){
            //替换
            tmpstring=QString(tmpstring).arg(varvec[i]);
        }

    }
    tmpstring += "\n";
    return tmpstring;
}
