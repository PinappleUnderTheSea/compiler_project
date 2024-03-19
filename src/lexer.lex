%{
#include <stdio.h>
#include <string.h>
#include "TeaplAst.h"
#include "y.tab.hpp"
extern int line, col;
int c;
int calc(char *s, int len);
%}

// TODO:
// your lexer

%x COMMENT1
%x COMMENT2

num [1-9][0-9]*|0
letter [a-zA-Z]
id [_a-zA-z][_a-zA-z0-9]*

%%
<INITIAL>"\t" { col+=4; }
<INITIAL>"\n" {line += 1;}
<INITIAL>num {
    yylval.tokenNum = A_TokenNum(A_Pos(line, col), calc(yytext, yyleng));
    col+=yyleng;
    return NUM;
}

<INITIAL>id {
    char* Id = (char*)malloc(yyleng + 1);
    strcpy(Id, yytext);
    yylval.tokenId = A_TokenId(A_Pos(line, col), Id);
    return ID;
}

<INITIAL>"//" {
    BEGIN COMMENT1;
}

<INITIAL>"/*" {
    BEGIN COMMENT2;
}

<COMMENT1>"\n" {
    BEGIN INITIAL;
}

<COMMENT1>. {}

<COMMENT2> "*/" {
    BEGIN INITIAL;
}

<COMMENT2>. {}

"let" {
    return LET;
}

"struct" {
    return STRUCT;
}

"fn" {
    return FN;
}

"continue" {
    return CONTINUE;
}

"break" {
    return BREAK;
}

"while" {
    return WHILE;
}

"if" {
    return IF;
}

"else" {
    return ELSE;
}

"int" {
    return INT;
}

"+" {
    return ADD;
}

"-" {
    return SUB;
}

"*" {
    return MUL;
}

"/" {
    return DIV;
}

"=" {
    return EQUAL;
}

";" {
    return SEMICOLON;
}

":" {
    return COLON;
}

"(" {
    return LEFT_PARENTHESIS;
}

")" {
    return RIGHT_PARENTHESIS;
}

"[" {
    return LEFT_BRACKET;
}

"]" {
    return RIGHT_BRACKET;
}

"{" {
    return LEFT_BRACE;
}

"}" {
    return RIGHT_BRACE;
}

"->" {
    return ARROW;
}

"." {
    return DOT;
}

"," {
    return COMMA;
}

"&&" {
    return AND;
}

"||" {
    return OR;
}

"!" {
    return NOT;
}

"<" {
    return LESS;
}

">" {
    return GREATER;
}

"<=" {
    return LESS_EQUAL;
}

">=" {
    return GREATER_EQUAL;
}

"==" {
    return IS;
}

"!=" {
    return IS_NOT;
}

[ \t\n\r] {}

. {
    yyerror("Unexpected Word");
}

%%

// This function takes a string of digits and its length as input, and returns the integer value of the string.
int calc(char *s, int len) {
    int ret = 0;
    for(int i = 0; i < len; i++)
        ret = ret * 10 + (s[i] - '0');
    return ret;
}