%{
#include <stdio.h>
#include <string.h>
#include "TeaplAst.h"
#include "y.tab.hpp"
extern int line, col;
int c;
int calc(char *s, int len);
char* err_pos(int line, int col);
extern "C" { void yyerror(char*); }
%}

%x COMMENT1
%x COMMENT2

%%

<INITIAL>"//"           { //printf("comment1\n"); 
                        BEGIN COMMENT1;  }
<COMMENT1>"\n"          { //printf("end comment1\n"); 
                        BEGIN INITIAL;  }
<INITIAL>"/*"           {  //printf("comment2\n"); 
                        BEGIN COMMENT2;  }
<COMMENT2>"*/"          {  //printf("end comment2\n");
                        BEGIN INITIAL;  }
<COMMENT1>.             {}
<COMMENT2>.             {}
"let"                   { return LET; } 
"struct"                { return STRUCT; }
"fn"                    { return FN; }
"continue"              { return CONTINUE; }
"break"                 { return BREAK; }
"ret"                   { return RETURN; }
"while"                 { return WHILE; }
"if"                    { return IF; }
"else"                  { return ELSE; }

[1-9][0-9]*|0 { 
    yylval.tokenNum = A_TokenNum(A_Pos(line, col), calc(yytext, yyleng));
    return NUM; 
}

[_a-zA-Z][_a-zA-Z0-9]*    { 
    char * temp = (char *)malloc(strlen(yytext) + 1); 
    strcpy(temp, yytext); 
    yylval.tokenId = A_TokenId(A_Pos(line, col), temp); 
    return ID; 
}
"int"                   { return INT; }
"+"	                    { return ADD; }
"-"	                    { return SUB; }
"*"                     { return MUL; }
"/"                     { return DIV; }
";"                     { return SEMICOLON; }
":"                     { return COLON; }
"["                     { return LEFT_SQUARE_BRACKET; }
"]"                     { return RIGHT_SQUARE_BRACKET; }
"="                     { return EQUAL; }
"{"                     { return OPEN_BRACE; }
"}"                     { return CLOSED_BRACE; }
"("                     { return LEFT_PARENTHESIS; }
")"                     { return RIGHT_PARENTHESIS; }
"->"	                { return RIGHT_ARROW; }
"."                     { return DOT; }
","                     { return COMMA; }
"&&"                    { return AND; }
"||"                    { return OR; }
"!"                     { return NOT; }
"<"                     { return LESS; }
">"                     { return GREATER; }
"<="                    { return LESS_EQUAL; }
">="                    { return GREATER_EQUAL; }
"=="                    { return IS; }
"!="                    { return IS_NOT; }

[ \t\n\r]                 { line++; }
.	                    {
                            yyerror("Unexpected token\n"); 
                        }
%%
int calc(char *s, int len) {
    int ret = 0;
    for(int i = 0; i < len; i++)
        ret = ret * 10 + (s[i] - '0');
    return ret;
}

char* err_pos(int line, int col) {
    char* err = (char*)malloc(20);
    memset(err, 0, 20);
    sprintf(err, "%d", line);
    sprintf(err + strlen(err), "%d", col );
    printf("error pos : %s", err);
    return err;
}