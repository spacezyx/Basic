#include "exp.h"

exp::exp()
{

}

//Expression 部分
Expression::Expression(){}
Expression::~Expression(){}

//constant 部分
ConstantExp::ConstantExp(int val){
    value=val;
}
ConstantExp::~ConstantExp(){}

int ConstantExp::eval(evalstate &Vars){
    return value;
}

ExpressionType ConstantExp::type(){
    return CONSTANT;
}

//identifier 部分
IdentifierExp::IdentifierExp(QString name){
    this->name = name;
}
int IdentifierExp::eval(evalstate & context){
    if(!context.isDefined(name))
        {
            QString error = "";
            error += name;
            error += " is not defined!";
            throw error;
        }
        else {
            return context.get(name);
        }

}
ExpressionType IdentifierExp::type(){
        return IDENTIFIER;
}
QString IdentifierExp::getIdentifierName(){
    return this->name;
}

QString CompoundExp::getIdentifierName(){
    return " ";
}


QString ConstantExp::getIdentifierName(){
    return " ";
}

//compound 部分
CompoundExp::CompoundExp(QString op, Expression *lhs, Expression *rhs){
    this->op=op;
    this->lhs=lhs;
    this->rhs=rhs;
}

CompoundExp::~CompoundExp(){}

int CompoundExp::eval(evalstate & context){
    if(rhs == nullptr){
            QString t = "Illegal operator in expression , an operator must have right value!";
            throw t;
        }
        int right = rhs->eval(context);
           if (op == "="){
              context.set(lhs->getIdentifierName(), right);
              return right;
           }
           if(lhs == nullptr){
               QString t = "Illegal operator in expression , an operator must have left value!";
               throw t;
           }
           int left = lhs->eval(context);
           if (op == "+") return left + right;
           if (op == "-") return left - right;
           if (op == "*") return left * right;
           if (op == "/") {
              if (right == 0){
                  QString error = "Division by 0";
                  throw(error);
              }
              return left / right;
           }
           if(op=="**"){
               return pow(left,right);
           }
           QString err = "Illegal operator in expression";
           throw err;
}

ExpressionType CompoundExp::type(){
    return COMPOUND;
}

QString CompoundExp::getOperator(){
    return op;
}
Expression* CompoundExp::getLHS(){
    return lhs;
}

Expression* CompoundExp::getRHS(){
    return rhs;
}
