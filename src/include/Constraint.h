#ifndef _CONSTRAINT_H
#define  _CONSTRAINT_H
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <string>
//#include "parser.tab.h"
//extern void yyparse();
//extern FILE *yyin;

extern int VERBOSE_LEVEL;
enum Operator{LT,LE,GT,GE,EQ};
class Variable{
	std::string name;
	std::string coefficient;
public:
	Variable(const char *coefficient,const char *name);
	std::string getName(){return name;}
	std::string getCoefficient(){return coefficient;}
	void reverse_parameter(){coefficient="-"+coefficient;}
};

class Constraint{
public:
	std::vector<Variable> pvlist;
	Operator op;
	std::string value;
	Constraint(std::vector<Variable> pvlist, Operator op, const char* value);
	Constraint(const Constraint &con);
	void print_info();
};

void parse_constraints(std::vector<Constraint> &constraints, const char *text);
#endif
