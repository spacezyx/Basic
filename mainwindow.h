#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <fstream>
#include <QString>
#include <queue>
#include <QTextBlock>
#include <QTextEdit>
#include <QEventLoop>
#include "evalstate.h"
#include "exp.h"
#include "parser.h"
#include "program.h"
#include "statement.h"
#include "tokenizer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QEventLoop loop;

    //声明要用到的各个类
    tokenizer* Tokenizer;
    statement* Statement;
    program* Program;
    evalstate* Evalstate;
    parser* Parser;

    //变量表
    evalstate Var;

    bool inputset=false;
    bool inputstringset=false;
    QString input;
    bool errorFlag=false;
    bool fileFlag=false;
    bool debugFlag=false;
    statement* debugpoint=NULL;
    QList<QTextEdit::ExtraSelection> extras;


    void Run(evalstate &var,statement* cur);
    void ParseCmd(evalstate &var);
    void Step(evalstate &var,statement* cur);
    void RunWithoutPre(evalstate &var,statement* cur);
    void output();
    void outputline(statement* cur);
    void preorder(TreeNode *root, int padding);
    void varout();
    void highlightfunc(int linenum,bool less,QList<QTextEdit::ExtraSelection> &extras);


private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_lineEdit_returnPressed();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
