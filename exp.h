#ifndef EXP_H
#define EXP_H

//声明实现表达式

#include <QString>
#include <string>
#include <map>
#include <cmath>
#include "evalstate.h"

using namespace std;

class exp
{
public:
    exp();
};


class EvaluationContext;
enum ExpressionType { CONSTANT, IDENTIFIER, COMPOUND };

//一个大的expression类
class Expression {
public:
   Expression();
   virtual ~Expression();
   virtual int eval(evalstate & context) = 0;
   virtual ExpressionType type() = 0;
   virtual QString getIdentifierName()=0;
};

//constant expresion
class ConstantExp: public Expression {
public:
   ConstantExp(int val);
   ~ConstantExp();

   int eval(evalstate &Vars);
   ExpressionType type();
   int value;
   QString getIdentifierName();
};

//identifier expression
class IdentifierExp: public Expression {
public:
   IdentifierExp(QString name);
   int eval(evalstate & context);
   ExpressionType type();
   QString getIdentifierName();
   QString name;
};

//compound expression
class CompoundExp: public Expression {
public:
   CompoundExp(QString op, Expression *lhs, Expression *rhs);
   ~CompoundExp();

   int eval(evalstate & context);
   ExpressionType type();

   QString getOperator();
   Expression *getLHS();
   Expression *getRHS();
   QString getIdentifierName();
   //Expression *lhs, *rhs;
private:
   Expression* lhs;
   Expression* rhs;
   QString op;
};




#endif // EXP_H
