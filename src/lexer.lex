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
"let"                   {  yylval.pos = A_Pos(line, col);col+=yyleng;return LET; }
"struct"                {  yylval.pos = A_Pos(line, col);col+=yyleng;return STRUCT; }
"fn"                    {  yylval.pos = A_Pos(line, col);col+=yyleng;return FN; }
"continue"              {  yylval.pos = A_Pos(line, col);col+=yyleng;return CONTINUE; }
"break"                 {  yylval.pos = A_Pos(line, col);col+=yyleng;return BREAK; }
"ret"                   {  yylval.pos = A_Pos(line, col);col+=yyleng;return RETURN; }
"while"                 {  yylval.pos = A_Pos(line, col);col+=yyleng;return WHILE; }
"if"                    {  yylval.pos = A_Pos(line, col);col+=yyleng;return IF; }
"else"                  {  yylval.pos = A_Pos(line, col);col+=yyleng;return ELSE; }
"int"                   {  yylval.pos = A_Pos(line, col);col+=yyleng;return INT; }

[1-9][0-9]*|0 {
    yylval.tokenNum = A_TokenNum(A_Pos(line, col), calc(yytext, yyleng));
    col+=yyleng;return NUM;
}

[_a-zA-Z][_a-zA-Z0-9]*    {
    char * val = (char *)malloc(yyleng + 1);
    memset(val, 0, yyleng + 1);
    strcpy(val, yytext);
    yylval.tokenId = A_TokenId(A_Pos(line, col), val);
    col+=yyleng;return ID;
}
"+"	                    {  yylval.pos = A_Pos(line, col);col+=yyleng;return ADD; }
"-"	                    {  yylval.pos = A_Pos(line, col);col+=yyleng;return SUB; }
"*"                     {  yylval.pos = A_Pos(line, col);col+=yyleng;return MUL; }
"/"                     {  yylval.pos = A_Pos(line, col);col+=yyleng;return DIV; }
";"                     {  yylval.pos = A_Pos(line, col);col+=yyleng;return SEMICOLON; }
":"                     {  yylval.pos = A_Pos(line, col);col+=yyleng;return COLON; }
"["                     {  yylval.pos = A_Pos(line, col);col+=yyleng;return LEFT_SQUARE_BRACKET; }
"]"                     {  yylval.pos = A_Pos(line, col);col+=yyleng;return RIGHT_SQUARE_BRACKET; }
"="                     {  yylval.pos = A_Pos(line, col);col+=yyleng;return EQUAL; }
"{"                     {  yylval.pos = A_Pos(line, col);col+=yyleng;return OPEN_BRACE; }
"}"                     {  yylval.pos = A_Pos(line, col);col+=yyleng;return CLOSED_BRACE; }
"("                     {  yylval.pos = A_Pos(line, col);col+=yyleng;return LEFT_PARENTHESIS; }
")"                     {  yylval.pos = A_Pos(line, col);col+=yyleng;return RIGHT_PARENTHESIS; }
"->"	                {  yylval.pos = A_Pos(line, col);col+=yyleng;return RIGHT_ARROW; }
"."                     {  yylval.pos = A_Pos(line, col);col+=yyleng;return DOT; }
","                     {  yylval.pos = A_Pos(line, col);col+=yyleng;return COMMA; }
"&&"                    {  yylval.pos = A_Pos(line, col);col+=yyleng;return AND; }
"||"                    {  yylval.pos = A_Pos(line, col);col+=yyleng;return OR; }
"!"                     {  yylval.pos = A_Pos(line, col);col+=yyleng;return NOT; }
"<"                     {  yylval.pos = A_Pos(line, col);col+=yyleng;return LESS; }
">"                     {  yylval.pos = A_Pos(line, col);col+=yyleng;return GREATER; }
"<="                    {  yylval.pos = A_Pos(line, col);col+=yyleng;return LESS_EQUAL; }
">="                    {  yylval.pos = A_Pos(line, col);col+=yyleng;return GREATER_EQUAL; }
"=="                    {  yylval.pos = A_Pos(line, col);col+=yyleng;return IS; }
"!="                    {  yylval.pos = A_Pos(line, col);col+=yyleng;return IS_NOT; }

\n                 { line++;col=1; }
\t { col+=4;}
\r {col=1; }
[ ] { col++;}
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