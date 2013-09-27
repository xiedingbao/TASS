%{
	#include <stdio.h>
	#include <stdlib.h>
	#include "Constraint.h"
	using namespace std;
	extern int yylex();
	extern unsigned int line_number;
	void yyerror(const char *msg){	
      		printf("Syntax Error: %s\n", msg);
		exit(1);
	}
	void yyerror(const std::string s){
     		yyerror(s.c_str());
	}

	void yyerror(std::vector<Constraint>& test, const char *msg){yyerror(msg);}
%}

%parse-param {std::vector<Constraint>& constraints_param}
%type<conlist> constraint_list constraint_no
%type<constraint> constraint
%type<pvlist> linearexpr
%type<pv> term
%type<mystring> NUMBER
%union{
	char * mystring;
	std::vector<Constraint>* conlist;
	Constraint* constraint;
	std::vector<Variable>* pvlist;
	Variable* pv;
};

%token <mystring> INT IDENT 
%left OGE OLE OGT OLT OEQ 
%left '+' '-'
%left '(' ')'


%%	

program:
	constraint_list
	{
		constraints_param=*$1;
		delete $1;
	}
	;


constraint_list:
	constraint_list constraint_no
	{ 
		$$=$1;
		for(unsigned i=0;i<$2->size();i++)
			$$->push_back((*$2)[i]);
		delete $2;
	}
	| 
	{
		$$=new vector<Constraint>();
	}
	;

constraint_no:
	constraint
	{
		$$=new vector<Constraint>();
		$$->push_back(*$1);
		delete $1;
	}
	|NUMBER OLE linearexpr OLE NUMBER
	{
		$$=new vector<Constraint>();
		$$->push_back(Constraint(*$3,GE,$1));
		$$->push_back(Constraint(*$3,LE,$5));
		delete $3;
	}
	;	
   
constraint:
	linearexpr OGE NUMBER	
	{
		$$=new Constraint(*$1,GE,$3);
		delete $1;	
	}
	|linearexpr OLE NUMBER	
	{	
		$$=new Constraint(*$1,LE,$3);
		delete $1;
	}
	|linearexpr OGT NUMBER	
	{
		$$=new Constraint(*$1,GT,$3);
		delete $1;	
	}
	| linearexpr OLT NUMBER	
	{
		$$=new Constraint(*$1,LT,$3);
		delete $1;
	}
	| linearexpr OEQ NUMBER	
	{	
		$$=new Constraint(*$1,EQ,$3);
		delete $1;	
	}

	;


linearexpr:
	term 
	{
		$$=new vector<Variable>();
		$$->push_back(*$1);
		delete $1;
	}
	|linearexpr '+' term 
	{
		$$=$1;
		$$->push_back(*$3);
		delete $3;
	}
	|linearexpr '-' term 
	{
		$$=$1;
		$3->reverse_parameter();
		$$->push_back(*$3);
		delete $3;
	}
	| '(' linearexpr ')'
	{
		$$=$2;
	}
	;
term:
	IDENT
	{
		$$=new Variable("1",$1);
		
	}
	|NUMBER IDENT
	{
		$$=new Variable($1,$2);	
	}
	|'-' IDENT
	{
		$$=new Variable("-1",$2);
	}
	;	
NUMBER:
	INT {$$=$1;}
	|'+' INT {$$=$2;}
	|'-' INT
	{
		char p[256];
		sprintf(p,"-%s",$2);
		$$ = p;
	}	

%%
/* you can use it to test your input file
int main(int argc, char *argv[])
{
	static char buffer[] = "ATM-USER=1";
	yyin=fmemopen(buffer, strlen (buffer), "r");
	yyparse();
	return 0;
}
*/


