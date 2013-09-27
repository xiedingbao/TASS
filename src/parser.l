%{
  #include <stdlib.h>
  #include "Constraint.h"
  #include "parser.tab.h"
  
  extern void yyerror(const char *);
  extern int yylex();
  char* outchar;
  char* mychars;
  unsigned int line_number=1;

%}
%option nounput 
%option noyywrap
%option noinput
%%


"=" 		{return OEQ;}
">=" 		{return OGE;}
"<=" 		{return OLE;}
">" 		{return OGT;}
"<" 		{return OLT;}

  
([0-9]+)(("."[0-9]+)?)((([eE])([+-]?)([0-9]+))?)  {yylval.mystring = strdup(yytext); return INT;}
    
[-+<>*/&|(){}:;,.=!]|"["|"]" {return *yytext;}

(["$""?"a-zA-Z]|[_])("$"|"?"|[a-zA-Z]|[0-9]|[_]|[~])* { yylval.mystring = strdup(yytext); return IDENT; }

  
"/*"([^*]|("*"+[^*/]))*"*"+"/" /* skip comment */
"//".*                         /* skip comment XS*/
"--".*                         /* skip comment */


[\n] { ++line_number; 

}
[ \t\r]                      

. { //outchar=strcat("invalid character",strdup(yytext));
    outchar=strdup(yytext);
    yyerror(outchar);}
%%

