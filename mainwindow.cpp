#include "mainwindow.h"
#include "ui_mainwindow.h"

//为了更方便的在UI里显示内容 mainiwindow的内容比较多
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //各个类的初始化
    Tokenizer=new tokenizer;
    Statement=new statement;
    Program=new program();
    Evalstate=new evalstate;
    connect(ui->lineEdit,SIGNAL(returnPressed()),&loop,SLOT(quit()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


//LOAD
void MainWindow::on_pushButton_clicked()
{
    //已经打开一个文件的情况下 要clear将已存在文件内容清空再打开另一个文件
    if(fileFlag){
        ui->textBrowser->append("WARNING!Are you trying to load a file?Please clear the already exist one first");
        return;
    }
    //读取txt类型文件
    //QString file = QFileDialog::getOpenFileName(NULL,"标题",".","*.txt");

    //读取各类型的文本文档
    QString file = QFileDialog::getOpenFileName(NULL,"标题",".","");
    QFile f(file);
    //打开指定文件
    if (!f.open(QIODevice::ReadOnly|QIODevice::Text)){
        QMessageBox::about(NULL, "文件", "文件打开失败");
    }

    QTextStream txtInput(&f);
    while (!txtInput.atEnd()){
       QString qtmp = txtInput.readLine();  //读取数据；
        QStringList tmp = qtmp.split(' ');
        Program->insert(tmp[0].toInt(),qtmp);
        ui->textBrowser->clear();
        int size=Program->length;
        statement* stmp=Program->head;
        for(int i=0;i<size;i++){
            stmp=stmp->next;
            ui->textBrowser->append(stmp->text);
        }
    }
    f.close();
    fileFlag=true;
}

//二叉树的前序遍历
//用于输出语法树
void MainWindow::preorder(TreeNode *root, int padding){
    if (root == nullptr) {
        return;
    }
    QString tmp="";
    //输出树的时候的前面的空格
    for(int i=0;i<4*padding;i++)
        tmp += " ";
    tmp +=root->data;
    ui->textBrowser_3->append(tmp);
    //前序遍历 左右子树应该在同一层
    preorder(root->left, ++padding);
    preorder(root->right, padding);
}

//输出变量表
void MainWindow::varout(){
    ui->textBrowser_4->clear();
    map<QString,int> inttmp=Var.Variable;
    map<QString,int>::iterator ibegin;
    map<QString,int>::iterator iend;
    ibegin=inttmp.begin();
    iend=inttmp.end();
    for(auto i=ibegin;i!=iend;i++){
        QString tmp=i->first+":INT="+QString::number(i->second,10);
        ui->textBrowser_4->append(tmp);
    }
    map<QString,QString> stringtmp=Var.stringVariable;
    map<QString,QString>::iterator sbegin;
    map<QString,QString>::iterator send;
    sbegin=stringtmp.begin();
    send=stringtmp.end();
    for(auto i=sbegin;i!=send;i++){
        QString tmp=i->first+":STR="+i->second;
        ui->textBrowser_4->append(tmp);
    }
}

//输出语句和语法树
void MainWindow::output(){
    map<int,statement*>prog=Program->pro;
    map<int,statement*>::iterator it;
    map<int,statement*>::iterator itEnd;
        it=prog.begin();
        itEnd=prog.end();
    for(;it!=itEnd;it++){
        QString qtmp=it->second->text;
        //多个空格变成一个 去除指令行里的多空格
        qtmp=qtmp.replace(QRegExp("\\s{1,}")," ");

        QList<QString> tmp = qtmp.split(' ');
        //输出每一行
        if(tmp[1]=="Error"){
            ui->textBrowser_3->append(tmp[0]+" Error");
        }
        else if(tmp[1]=="REM"){
            ui->textBrowser_3->append(tmp[0]+" REM");
            QString state = "";
            //获取一个单纯的表达式
            for(int i=2;i <tmp.length();++i)
                state += tmp[i]+" ";

            ui->textBrowser_3->append("    "+state);
        }
        else if(tmp[1]=="GOTO"){
            ui->textBrowser_3->append(tmp[0]+" GOTO");
            ui->textBrowser_3->append("    "+tmp[2]);
        }
        else if(tmp[1]=="END"){
            ui->textBrowser_3->append(tmp[0]+" END");
        }
        else if(tmp[1]=="IF"){
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
            ui->textBrowser_3->append(tmp[0]+" IF THEN "+exp1);
            ui->textBrowser_3->append("    "+op+" "+exp2+" "+dst);
        }
        else if(tmp[1]=="INPUT"){
            ui->textBrowser_3->append(tmp[0]+" INPUT");
            ui->textBrowser_3->append("    "+tmp[2]);
        }
        else if(tmp[1]=="INPUTS"){
            ui->textBrowser_3->append(tmp[0]+" INPUTS");
            ui->textBrowser_3->append("    "+tmp[2]);
        }
        //这个要建树
        else if(tmp[1]=="LET"){
            ui->textBrowser_3->append(tmp[0]+" LET =");
            QString state = "";
            //获取一个单纯的表达式
            for(int i=2;i <tmp.length();++i)
                state += tmp[i];
            QString variableName="";
            QString valexp="";
            int j=0;
            for(;state[j]!="=";j++)
                variableName +=state[j];
            j++;
            int s=state.size();
            for(;j<s;j++)
                valexp +=state[j];
            ui->textBrowser_3->append("    "+variableName);
            //这里在建树
            QList<QString>tokens;
            tokens.clear();
            tokens = Tokenizer->toTokens(valexp);
            vector<QString> a=Parser->toPostfix(tokens);
            QList<QString> li= QList<QString>::fromVector(QVector<QString>::fromStdVector(a));
            TreeNode* root=Parser->toTree(li);
            //这里一个前序遍历输出
            int padding=1;
            preorder(root,padding);
        }

        else if(tmp[1]=="PRINT"){
            ui->textBrowser_3->append(tmp[0]+" PRINT");
            QString state = "";
            //获取一个单纯的表达式
            for(int i=2;i <tmp.length();++i){
                state += tmp[i];
            }
            //这里在建树
            QList<QString>tokens;
            tokens.clear();
            tokens = Tokenizer->toTokens(state);
            vector<QString> a=Parser->toPostfix(tokens);
            QList<QString> li= QList<QString>::fromVector(QVector<QString>::fromStdVector(a));
            TreeNode* root=Parser->toTree(li);
            //这里一个前序遍历输出
            int padding=1;
            preorder(root,padding);
        }

        else if(tmp[1]=="PRINTF"){
            ui->textBrowser_3->append(tmp[0]+" PRINTF");
            QString state = "";
            for(int i=2;i <tmp.length();++i){
                state += tmp[i];
            }
            ui->textBrowser_3->append("    "+state);
        }

        else{
            ui->textBrowser_3->append(tmp[0]+"    "+tmp[1]);
        }
    }
}

//输出某一行的语法树
void MainWindow::outputline(statement* cur){
    ui->textBrowser_3->clear();
        QString qtmp=cur->text;
        //多个空格变成一个 去除指令行里的多空格
        qtmp=qtmp.replace(QRegExp("\\s{1,}")," ");

        QList<QString> tmp = qtmp.split(' ');
        //输出每一行
        if(tmp[1]=="Error"){
            ui->textBrowser_3->append(tmp[0]+" Error");
        }
        if(tmp[1]=="REM"){
            ui->textBrowser_3->append(tmp[0]+" REM");
            QString state = "";
            //获取一个单纯的表达式
            for(int i=2;i <tmp.length();++i)
                state += tmp[i]+" ";

            ui->textBrowser_3->append("    "+state);
        }
        if(tmp[1]=="GOTO"){
            ui->textBrowser_3->append(tmp[0]+" GOTO");
            ui->textBrowser_3->append("    "+tmp[2]);
        }
        if(tmp[1]=="END"){
            ui->textBrowser_3->append(tmp[0]+" END");
        }
        if(tmp[1]=="IF"){
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
            ui->textBrowser_3->append(tmp[0]+" IF THEN "+exp1);
            ui->textBrowser_3->append("    "+op+" "+exp2+" "+dst);
        }
        if(tmp[1]=="INPUT"){
            ui->textBrowser_3->append(tmp[0]+" INPUT");
            ui->textBrowser_3->append("    "+tmp[2]);
        }
        if(tmp[1]=="INPUTS"){
            ui->textBrowser_3->append(tmp[0]+" INPUTS");
            ui->textBrowser_3->append("    "+tmp[2]);
        }
        //这个要建树
        if(tmp[1]=="LET"){
            ui->textBrowser_3->append(tmp[0]+" LET =");
            QString state = "";
            //获取一个单纯的表达式
            for(int i=2;i <tmp.length();++i)
                state += tmp[i];
            QString variableName="";
            QString valexp="";
            int j=0;
            for(;state[j]!="=";j++)
                variableName +=state[j];
            j++;
            int s=state.size();
            for(;j<s;j++)
                valexp +=state[j];
            ui->textBrowser_3->append("    "+variableName);
            //这里在建树
            QList<QString>tokens;
            tokens.clear();
            tokens = Tokenizer->toTokens(valexp);
            vector<QString> a=Parser->toPostfix(tokens);
            QList<QString> li= QList<QString>::fromVector(QVector<QString>::fromStdVector(a));
            TreeNode* root=Parser->toTree(li);
            //这里一个前序遍历输出
            int padding=1;
            preorder(root,padding);
        }

        if(tmp[1]=="PRINT"){
            ui->textBrowser_3->append(tmp[0]+" PRINT");
            QString state = "";
            //获取一个单纯的表达式
            for(int i=2;i <tmp.length();++i){
                state += tmp[i];
            }
            //这里在建树
            QList<QString>tokens;
            tokens.clear();
            tokens = Tokenizer->toTokens(state);
            vector<QString> a=Parser->toPostfix(tokens);
            QList<QString> li= QList<QString>::fromVector(QVector<QString>::fromStdVector(a));
            TreeNode* root=Parser->toTree(li);
            //这里一个前序遍历输出
            int padding=1;
            preorder(root,padding);
        }

        if(tmp[1]=="PRINTF"){
            ui->textBrowser_3->append(tmp[0]+" PRINTF");
            QString state = "";
            for(int i=2;i <tmp.length();++i){
                state += tmp[i];
            }
            ui->textBrowser_3->append("    "+state);
        }
}

//RUN
void MainWindow::on_pushButton_2_clicked()
{

    Program->del(0);
    //遍历一遍指令
    ParseCmd(Var);
    debugFlag=false;
    ui->pushButton->setDisabled(false);
    ui->pushButton_3->setDisabled(false);
    //重置一下链接关系
    auto iter=Program->pro.begin();
    auto it=iter;
    for(;iter!=Program->pro.end();++iter){

        it++;
        if(it!=Program->pro.end()){
            iter->second->next=it->second;
        }
        else{
            iter->second->next=nullptr;
          }
    }

    Program->head->next=Program->pro.begin()->second;
    statement* p=Program->head;
    while(!errorFlag&&p->next){
        //中途出错立即停止程序的运行
        if(p->next&&!Program->isError&&!errorFlag){
              QList<QString> tmp = p->next->text.split(' ');
              Program->isError=false;
              errorFlag=false;
              QString cmd=tmp[1];

              if(cmd=="INPUT"||cmd=="INPUTS"){
                  if(cmd=="INPUT"){
                      input=tmp[2];
                      if((input[0]>="A" && input[0]<="Z")||(input[0]>="a" && input[0]<="z")){
                          ui->lineEdit->setText("?");
                          inputset=true;
                          ui->lineEdit->setFocus();
                          loop.exec();
                        }
                    }
                  else if(cmd=="INPUTS"){
                      input=tmp[2];
                      if((input[0]>="A" && input[0]<="Z")||(input[0]>="a" && input[0]<="z")){
                          ui->lineEdit->setText("?");
                          inputstringset=true;
                          ui->lineEdit->setFocus();
                          loop.exec();
                        }
                    }
                  if(!inputset&&!inputstringset){
                    Run(Var,p->next);
                    p=p->next;
                  }
                }

              else {
                  Run(Var,p->next);
                  p=p->next;
                }
            }
        //看看程序在哪出的错（mainwindow or program）
        if(Program->isError){
            Program->isError=false;
            p->text=QString::number(p->linenum,10)+" Error";
            output();
            varout();
            return;
        }
        else if(errorFlag){
            //如果出错重新输出
            p->text=QString::number(p->linenum,10)+" Error";
            ui->textBrowser_3->clear();
            ui->textBrowser_4->clear();
            errorFlag=false;
            output();
            varout();
            return;
        }
       else{
            ui->textBrowser_3->clear();
            ui->textBrowser_4->clear();
            output();
            varout();
        }
    }
}

//设置高亮
void MainWindow::highlightfunc(int linenum,bool less,QList<QTextEdit::ExtraSelection> &extras){
    //设置高亮
    bool hasAdd=false;
    auto it=Program->pro.begin();
            int curline=0;
            QTextEdit *code;
            code=ui->textBrowser;
            QTextCursor cursor(code->document());
           // QList<QTextEdit::ExtraSelection> extras;
            QList<QPair<int, QColor>> highlights =
            {{92, QColor(100, 255, 100)},
             {131, QColor(255, 100, 100)},
             {180, QColor(255, 100, 100)}};

            while (it!=Program->pro.end()){
                if (it->first==linenum){
                     int position=code->document()->findBlockByLineNumber(curline).position();
                     cursor.setPosition(position,QTextCursor::MoveAnchor);
                     QTextEdit::ExtraSelection h;
                     h.cursor = cursor;
                     h.cursor.movePosition(QTextCursor::StartOfLine);
                     h.cursor.movePosition(QTextCursor::EndOfLine);
                     h.format.setProperty(QTextFormat::FullWidthSelection, true);
                     if(less)
                         h.format.setBackground(highlights[0].second);//绿色
                     else
                         h.format.setBackground(highlights[1].second);//红色
                     extras.push_back(h);
                     hasAdd=true;
                     if(less&&hasAdd)
                         break;
                }
                curline++;
                it++;
            }

            code->setExtraSelections(extras);

            if(less&&hasAdd)
                extras.pop_back();
}

//Debug/Step
void MainWindow::on_pushButton_4_clicked()
{
    Program->del(0);

    if(!debugFlag){
        //重置一下链接关系
        auto iter=Program->pro.begin();
        auto it=iter;
        for(;iter!=Program->pro.end();++iter){
            it++;
            if(it!=Program->pro.end()){
                iter->second->next=it->second;
            }
            else{
                iter->second->next=nullptr;
              }
        }
        Program->head->next=Program->pro.begin()->second;

        //遍历一遍指令
        ParseCmd(Var);
        debugpoint=Program->head;
        debugFlag=true;
        outputline(debugpoint->next);
        varout();
        highlightfunc(debugpoint->next->linenum,true,extras);
        debugpoint=debugpoint->next;
        //按钮失灵
        ui->pushButton->setDisabled(true);
        ui->pushButton_3->setDisabled(true);
    }
    else if(!Program->isError&&!inputset&&!inputstringset){
        QList<QString> tmp = debugpoint->next->text.split(' ');
        Program->isError=false;
        errorFlag=false;
        QString cmd=tmp[1];

        if(cmd=="INPUT"||cmd=="INPUTS"){
        if(cmd=="INPUT"){
                input=tmp[2];
                if((input[0]>="A" && input[0]<="Z")||(input[0]>="a" && input[0]<="z")){
                    ui->lineEdit->setText("?");
                    inputset=true;
                }
        }
        else if(cmd=="INPUTS"){
                input=tmp[2];
                if((input[0]>="A" && input[0]<="Z")||(input[0]>="a" && input[0]<="z")){
                    ui->lineEdit->setText("?");
                    inputstringset=true;
                }
            }

        outputline(debugpoint->next);
        varout();

        //设置高亮
        highlightfunc(debugpoint->next->linenum,true,extras);
        debugpoint=debugpoint->next;
        if(errorFlag||Program->isError){
            QMessageBox::about(NULL, "文件","该语句有错误" );
            ui->pushButton->setDisabled(false);
            ui->pushButton_3->setDisabled(false);
            debugFlag=false;
          }
        else if(!debugpoint->next){
            QMessageBox::about(NULL, "文件", "被调试的程序正常结束");
            ui->pushButton->setDisabled(false);
            ui->pushButton_3->setDisabled(false);
            debugFlag=false;
          }
      }

        else {
          Run(Var,debugpoint->next);
          outputline(debugpoint->next);
          varout();
          //设置高亮
          highlightfunc(debugpoint->next->linenum,true,extras);
          debugpoint=debugpoint->next;
          if(errorFlag||Program->isError){
              QMessageBox::about(NULL, "文件","该语句有错误" );
              ui->pushButton->setDisabled(false);
              ui->pushButton_3->setDisabled(false);
              debugFlag=false;
            }
          else if(!debugpoint->next){
              QMessageBox::about(NULL, "文件", "被调试的程序正常结束");
              ui->pushButton->setDisabled(false);
              ui->pushButton_3->setDisabled(false);
              debugFlag=false;
            }
        }
    }
}

//遍历一遍指令
void MainWindow::ParseCmd(evalstate &var){
    extras.clear();
    //Program->cmdEnd(var,Program->pro.end()->second);
    for(auto iter=Program->pro.begin();iter!=Program->pro.end();++iter){
        statement* cur=iter->second;
        if(cur->linenum<=0||cur->linenum>=10000){
            ++iter;
            cur=iter->second;
        }

        QList<QString> tmp = cur->text.split(' ');
        Program->isError=false;
        errorFlag=false;
        QString cmd=tmp[1];
        if(cmd=="REM"){
            Program->cmdRem(var,cur);
        }
        else if(cmd=="LET"){
            Program->cmdLet(var,cur);
        }
        else if(cmd=="PRINT"){
            Program->cmdPrint(var,cur);
        }
        else if(cmd=="GOTO"){
            Program->cmdGoto(var,cur);
        }
        else if(cmd=="END"){
            Program->cmdEnd(var,cur);
        }
        else if(cmd=="IF"){
            Program->cmdIf(var,cur);
        }
        else if(cmd=="INPUT"){
            if(tmp.size()==2){
                //出错可能之一INPUT后没有变量名
                QString err="Oops!You haven't name the variable,right?";
                ui->textBrowser_2->append(err);
            }
            else{
                input=tmp[2];
                if((input[0]>="A" && input[0]<="Z")||(input[0]>="a" && input[0]<="z")){
                    Var.set(input,0);
                }
                else{
                    //出错可能:INPUT使用的变量名违法
                    QString err="it is an illegal variable";
                    ui->textBrowser_2->append(err);
                }
            }
          }
        else if(cmd=="PRINTF"){
            Program->cmdPrintf(var,cur);
        }
        else if(cmd=="INPUTS"){
            if(tmp.size()==2){
                //出错可能之一INPUT后没有变量名
                QString err="Oops!You haven't name the variable,right?";
                ui->textBrowser_2->append(err);
            }
            else{
                input=tmp[2];
                if((input[0]>="A" && input[0]<="Z")||(input[0]>="a" && input[0]<="z")){
                    Var.stringset(input,0);
                }
                else{
                    //出错可能:INPUT使用的变量名违法
                    QString err="it is an illegal variable";
                    ui->textBrowser_2->append(err);
                }
            }
          }
        else{
            errorFlag=true;
            cur->text=QString::number(cur->linenum,10)+" Error";
        }
        if(Program->isError||errorFlag){
            Program->isError=false;
            errorFlag=false;
            highlightfunc(iter->second->linenum,false,extras);
        }
    }
}

//CLEAR
//除去清空ui界面中的内容外，还应重置可能被改变的变量
void MainWindow::on_pushButton_3_clicked()
{
    Program->clear();
    delete Program;
    Var.clear();

    inputset=false;
    errorFlag=false;
    fileFlag=false;
    debugFlag=false;
    Program->isError=false;
    extras.clear();

    Program = new program();
    ui->textBrowser->clear();
    ui->textBrowser_2->clear();
    ui->textBrowser_3->clear();
    ui->textBrowser_4->clear();
    return;
}

//输入框
//处理特殊指令:RUN,CLEAR,LOAD,HELP
//处理无行号的指令:LET,PRINT,INPUT
//处理其余带行号的指令
void MainWindow::on_lineEdit_returnPressed()
{
  QString qtmp=ui->lineEdit->text();

  if(debugFlag){
      if(inputset){
           QString q=qtmp;
           q.remove(QChar('?'), Qt::CaseInsensitive);
           int out = q.toInt();
           Var.set(input,out);
           inputset=false;
       }

       //输入字符模式时
      else if(inputstringset){
           QString q=qtmp;
           q.remove(QChar('?'), Qt::CaseSensitive);
           Var.stringset(input,q);
           inputstringset=false;
       }
      else
        debugFlag=false;
    }

    ui->lineEdit->clear();
    //解决多个空格问题
    qtmp=qtmp.replace(QRegExp("\\s{1,}")," ");
    QList<QString> tmp = qtmp.split(' ');
    //特殊输入
    //switch语句更好看 但if else比switch更好维护 这里选择if else
    if(qtmp=="QUIT")
        this->close();
    else if(qtmp=="RUN")
        on_pushButton_2_clicked();
    else if(qtmp=="CLEAR")
        on_pushButton_3_clicked();
    else if(qtmp=="LOAD")
        on_pushButton_clicked();
    else if(qtmp=="HELP"){
        QString t="It is a miniBasic";
        ui->textBrowser->append(t);
    }
    //LIST指令直接忽略
    else if(qtmp=="LIST"){}

    //不带行号的指令
    //LET，也就是添加一个变量
    else if(tmp[0]=="LET"){
        QString state = "";
        //获取一个单纯的表达式
        for(int i=1;i <tmp.length();++i)
            state += tmp[i];
        QString variableName="";
        QString valexp="";
        QString stringexp="";
        int j=0;
        //判断一下是否有等号 （出错的一种可能）
        if(!state.contains("=")){
            errorFlag=true;
            QString err="illegal expression";
            ui->textBrowser_2->append(err);
            return;
        }
        for(;state[j]!="=";j++)
            variableName +=state[j];
        j++;
        //字符变量
        if(((state[j]=="\"")&&(state[state.size()-1]=="\""))||((state[j]=="'")&&(state[state.size()-1]=="\'"))){
            int s=state.size();
            for(;j<s;j++)
                stringexp +=state[j];
            Var.stringset(variableName,stringexp);
        }
        else if(((state[j]=="\"")&&(state[state.size()-1]=="'"))||((state[j]=="'")&&(state[state.size()-1]=="\""))){
            ui->textBrowser_2->append("error input");
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
            //后缀表达式
            vector<QString> a=Parser->toPostfix(tokens);

            //后缀表达式求值
            if(!Parser->exchange(a,Var)){
                QString err="undefined variable";
                ui->textBrowser_2->append(err);
            }
            else{
                Parser->exchange(a,Var);
                num=Parser->evalRPN(a,Var);
                Var.set(variableName,num);
            }
        }
    }

    //只有PRINT,直接计算出结果
    else if(tmp[0]=="PRINT"){
        QString state = "";
        //获取一个单纯的表达式
        for(int i=1;i <tmp.length();++i){
            state += tmp[i];
        }
        //计算结果
        int num=0;
        QList<QString> tokens;
        tokens.clear();
        tokens = Tokenizer->toTokens(state);
        //后缀表达式
        vector<QString> a;
        a.clear();
        a=Parser->toPostfix(tokens);

        //后缀表达式求值
        if(!Parser->exchange(a,Var)){
            QString err="undefined variable";
            ui->textBrowser_2->append(err);
        }
        else{
            Parser->exchange(a,Var);
            num=Parser->evalRPN(a,Var);
            QString out = QString::number(num,10);
            ui->textBrowser_2->append(out);
        }
    }
    //PRINTF指令
    else if(tmp[0]=="PRINTF"){
        int count=0;
        int varcount=0;
        QString varstring=0;
        bool isError=false;
        QString text="";
        int stringsize=tmp.size();
        for(int i=1;i<stringsize-1;i++){
            text=text+tmp[i]+" ";
        }
        text +=tmp[stringsize-1];
        int i=0;
        QString tmpstring="";
        int textsize=text.size();
        if(text[0]!='"'&&text[0]!='\'')
            isError=true;
        //把格式化字符串读入
        while(i<textsize){
            if(isError){
                ui->textBrowser->append("Illegal Input");
                break;
            }
            else if(text[i]=='"'){
                i++;
                if(i==textsize)
                    break;
                while(text[i]!='"'){
                    if(text[i]=='\''){
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
            ui->textBrowser_2->append("error input");
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
                else if(Var.isDefined(varvec[i])){
                    varvec[i]=QString::number(Var.Variable[varvec[i]],10);
                }
                else if(Var.stringisDefined(varvec[i])){
                    varvec[i]=Var.stringVariable[varvec[i]];
                }
            }
            for(int i=0;i<varvec.size();i++){
                //替换
                tmpstring=QString(tmpstring).arg(varvec[i]);
            }

        }
        tmpstring += "\n";
        ui->textBrowser_2->append(tmpstring);
    }

    //只有INPUT,考虑一下变量名是否合法
    else if(tmp[0]=="INPUT"){
        if(tmp.size()==1){
            //出错可能之一INPUT后没有变量名
            QString err="Oops!You haven't name the variable,right?";
            ui->textBrowser_2->append(err);
        }
        else{
            input=tmp[1];
            if((input[0]>="A" && input[0]<="Z")||(input[0]>="a" && input[0]<="z")){
                ui->lineEdit->setText("?");
                inputset=true;
            }
            else{
                //出错可能:INPUT使用的变量名违法
                QString err="it is an illegal variable";
                ui->textBrowser_2->append(err);
            }
        }
    }

    else if(tmp[0]=="INPUTS"){
        if(tmp.size()==1){
            //出错可能之一INPUT后没有变量名
            QString err="Oops!You haven't name the variable,right?";
            ui->textBrowser_2->append(err);
        }
        else{
            input=tmp[1];
            if((input[0]>="A" && input[0]<="Z")||(input[0]>="a" && input[0]<="z")){
                ui->lineEdit->setText("?");
                inputstringset=true;
            }
            else{
                //出错可能:INPUT使用的变量名违法
                QString err="it is an illegal variable";
                ui->textBrowser_2->append(err);
            }
        }
    }

    //输入数值模式时
   else if(inputset){
        QString q=qtmp;
        q.remove(QChar('?'), Qt::CaseInsensitive);
        int out = q.toInt();
        Var.set(input,out);
        inputset=false;
    }

    //输入字符模式时
   else if(inputstringset){
        QString q=qtmp;
        q.remove(QChar('?'), Qt::CaseSensitive);
        Var.stringset(input,q);
        inputstringset=false;
    }

    //处理带行号的指令
    else if(0<tmp[0].toInt()&&tmp[0].toInt()<1000000){
        //删除指令
        if(tmp[0]==qtmp)
            Program->del(tmp[0].toInt());

        //添加指令
        //INPUT/INPUTS指令与UI界面有关先放这
        else{
            if(tmp[1]=="INPUT"){
                if(tmp.size()==2){
                    QString err="Oops!You haven't name the variable,right?";
                    ui->textBrowser_2->append(err);
                }
                else{
                    input=tmp[2];
                    if((input[0]>="A" && input[0]<="Z")||(input[0]>="a" && input[0]<="z")){
                        ui->lineEdit->setText("?");
                        inputset=true;
                    }
                    else{
                        QString err="it is an illegal variable";
                        ui->textBrowser_2->append(err);
                    }
                }
            }
            if(tmp[1]=="INPUTS"){
                if(tmp.size()==2){
                    QString err="Oops!You haven't name the variable,right?";
                    ui->textBrowser_2->append(err);
                }
                else{
                    input=tmp[2];
                    if((input[0]>="A" && input[0]<="Z")||(input[0]>="a" && input[0]<="z")){
                        ui->lineEdit->setText("?");
                        inputstringset=true;
                    }
                    else{
                        QString err="it is an illegal variable";
                        ui->textBrowser_2->append(err);
                    }
                }
            }

            //其他带行号指令都可以直接放进来(INPUT指令也要放进来才能显示在代码框和语法树中）
            //示例中错误指令也可以出现在代码框中
            //if(tmp[1]=="REM"||tmp[1]=="LET"||tmp[1]=="PRINT"||tmp[1]=="GOTO"||tmp[1]=="IF"||tmp[1]=="END"||tmp[1]=="INPUT"||tmp[1]=="PRINTF"||tmp[1]=="INPUTS")
                Program->insert(tmp[0].toInt(),qtmp);
        }
        //更新一遍代码框中的内容
        ui->textBrowser->clear();
        map<int,statement*> tmppro=Program->pro;
        for(auto iter=tmppro.begin();iter!=tmppro.end();++iter){
            ui->textBrowser->append(iter->second->text);
        }
    }
    else{
        //提示用户输入错误
        QString tmp="Illegal Input，Please check your code carefully";
        ui->textBrowser->append(tmp);
    }
}

//处理普通指令 主要内容在program中
void MainWindow::Run(evalstate &var,statement*cur){
    QList<QString> tmp = cur->text.split(' ');
    QString cmd=tmp[1];
    if(cmd=="REM"){
        Program->cmdRem(var,cur);
    }
    else if(cmd=="LET"){
        Program->cmdLet(var,cur);
    }
    else if(cmd=="PRINT"){
        QString out=Program->cmdPrint(var,cur);
        ui->textBrowser_2->append(out);
    }
    else if(cmd=="GOTO"){
        if(Program->cmdGoto(var,cur)!=""){
            QString out=Program->cmdGoto(var,cur);
            ui->textBrowser_2->append(out);
        }
    }
    else if(cmd=="END"){
        Program->cmdEnd(var,cur);
    }
    else if(cmd=="IF"){
        Program->cmdIf(var,cur);
    }
    else if(cmd=="INPUT"){
    }
    else if(cmd=="PRINTF"){
        QString out=Program->cmdPrintf(var,cur);
        if(!Program->isError)
            ui->textBrowser_2->append(out);
    }
    else if(cmd=="INPUTS"){}
    else{
        errorFlag=true;
        cur->text=QString::number(cur->linenum,10)+" Error";
    }
}
