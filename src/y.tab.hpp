/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_HPP_INCLUDED
# define YY_YY_Y_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    LET = 258,
    STRUCT = 259,
    FN = 260,
    CONTINUE = 261,
    BREAK = 262,
    RETURN = 263,
    WHILE = 264,
    IF = 265,
    ELSE = 266,
    NUM = 267,
    ID = 268,
    INT = 269,
    ADD = 270,
    SUB = 271,
    MUL = 272,
    DIV = 273,
    SEMICOLON = 274,
    COLON = 275,
    LEFT_SQUARE_BRACKET = 276,
    RIGHT_SQUARE_BRACKET = 277,
    EQUAL = 278,
    OPEN_BRACE = 279,
    CLOSED_BRACE = 280,
    LEFT_PARENTHESIS = 281,
    RIGHT_PARENTHESIS = 282,
    RIGHT_ARROW = 283,
    DOT = 284,
    COMMA = 285,
    AND = 286,
    OR = 287,
    NOT = 288,
    LESS = 289,
    GREATER = 290,
    LESS_EQUAL = 291,
    GREATER_EQUAL = 292,
    IS = 293,
    IS_NOT = 294
  };
#endif
/* Tokens.  */
#define LET 258
#define STRUCT 259
#define FN 260
#define CONTINUE 261
#define BREAK 262
#define RETURN 263
#define WHILE 264
#define IF 265
#define ELSE 266
#define NUM 267
#define ID 268
#define INT 269
#define ADD 270
#define SUB 271
#define MUL 272
#define DIV 273
#define SEMICOLON 274
#define COLON 275
#define LEFT_SQUARE_BRACKET 276
#define RIGHT_SQUARE_BRACKET 277
#define EQUAL 278
#define OPEN_BRACE 279
#define CLOSED_BRACE 280
#define LEFT_PARENTHESIS 281
#define RIGHT_PARENTHESIS 282
#define RIGHT_ARROW 283
#define DOT 284
#define COMMA 285
#define AND 286
#define OR 287
#define NOT 288
#define LESS 289
#define GREATER 290
#define LESS_EQUAL 291
#define GREATER_EQUAL 292
#define IS 293
#define IS_NOT 294

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 19 "parser.yacc" /* yacc.c:1909  */


    A_tokenId tokenId;
    // A_nativeType nativeType;
    A_type type;
    A_tokenNum tokenNum;
    A_rightVal rightVal;
    A_rightValList rightValList;

    A_pos pos;
    A_program program;
    A_programElementList programElementList;
    A_programElement programElement;

    A_arithUExpr arithUExpr;
    A_arithExpr arithExpr;
    A_exprUnit exprUnit;

    A_varDeclList varDeclList;
    A_structDef structDef;


    A_varDecl varDecl;
    A_varDef varDef;
    A_varDeclStmt varDeclStmt;

    A_paramDecl paramDecl;
    A_fnDecl fnDecl;
    A_fnDeclStmt fnDeclStmt;

    A_memberExpr memberExpr;
    A_arrayExpr arrayExpr;
    A_leftVal leftVal;
    A_assignStmt assignStmt;

    A_returnStmt returnStmt;
    A_whileStmt whileStmt;


    // A_boolUOp boolUOp;
    A_comOp comOp;
    A_boolUnit boolUnit;
    A_boolBiOp boolBiOp;
    A_boolExpr boolExpr;
    A_ifStmt ifStmt;

    A_callStmt callStmt;
    A_fnCall fnCall;

    A_codeBlockStmt codeBlockStmt;
    A_codeBlockStmtList codeBlockStmtList;
    A_fnDef fnDef;

#line 186 "y.tab.hpp" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_HPP_INCLUDED  */
