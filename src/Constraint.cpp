#include "Constraint.h"
#include "parser.tab.h"
//extern void yyparse();
extern FILE *yyin;
using namespace std;

void parse_constraints(vector<Constraint>& constraints, const char* text){
	void *p=malloc(strlen(text));
	strcpy((char *)p,text);
	yyin = fmemopen(p, strlen(text), "r");
	if(VERBOSE_LEVEL>1)
		printf("parsing :%s\n",text);
	yyparse(constraints);
	free(p);
}

Constraint::Constraint(vector<Variable> pvlist, Operator op, const char* value){
	this->pvlist=pvlist;
	this->op=op;
	this->value=string(value);
}
Constraint::Constraint(const Constraint &con){
	this->pvlist=con.pvlist;
	this->op=con.op;
	this->value=con.value;
}

void Constraint::print_info(){
	printf("constraint: ");
	for(unsigned int i=0;i<pvlist.size();i++){
		printf("%s*%s ",pvlist[i].getCoefficient().c_str(),pvlist[i].getName().c_str());
	}
	switch(op){
		case LT:printf(" < ");break;
		case LE:printf(" <= ");break;
		case GT:printf(" > ");break;
		case GE:printf(" >= ");break;
		case EQ:printf(" == ");break;

	}
	printf("%s\n",this->value.c_str());
}

Variable::Variable(const char *coefficient,const char *name){
	this->name=name;
	this->coefficient=coefficient;
}


