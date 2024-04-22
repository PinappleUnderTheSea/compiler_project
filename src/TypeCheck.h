#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include "TeaplAst.h"
#include "TeaplaAst.h"
#include <unordered_map>
#include "utils.h"

// token id to token type, including function name to return type





typedef struct tc_type_* tc_type;
// typedef std::unordered_map<string, tc_type> typeMap; 



// func name to params
typedef std::unordered_map<string, vector<aA_varDecl>*> paramMemberMap; 

void check_Prog(std::ostream& out, aA_program p);
void check_VarDecl(std::ostream& out, aA_varDeclStmt vd, ScopeEnv env);
void check_StructDef(std::ostream& out, aA_structDef sd);
void check_paramDecl(std::ostream& out, aA_paramDecl pd);
void check_FnDecl(std::ostream& out, aA_fnDecl fd);
void check_FnDeclStmt(std::ostream& out, aA_fnDeclStmt fd);
void check_FnDef(std::ostream& out, aA_fnDef fd);
void check_CodeblockStmt(std::ostream& out, aA_codeBlockStmt cs);
void check_AssignStmt(std::ostream& out, aA_assignStmt as);
void check_ArrayExpr(std::ostream& out, aA_arrayExpr ae);
tc_type check_MemberExpr(std::ostream& out, aA_memberExpr me);
void check_IfStmt(std::ostream& out, aA_ifStmt is);
void check_BoolExpr(std::ostream& out, aA_boolExpr be, ScopeEnv env);
void check_BoolUnit(std::ostream& out, aA_boolUnit bu, ScopeEnv env);
tc_type check_ExprUnit(std::ostream& out, aA_exprUnit eu, ScopeEnv env);
tc_type check_ArithExpr(std::ostream& out, aA_arithExpr ae, ScopeEnv env);
void check_FuncCall(std::ostream& out, aA_fnCall fc);
void check_WhileStmt(std::ostream& out, aA_whileStmt ws);
void check_CallStmt(std::ostream& out, aA_callStmt cs);
void check_ReturnStmt(std::ostream& out, aA_returnStmt rs);
aA_type check_RightValue(std::ostream& out, aA_rightVal rightVal, aA_type type, ScopeEnv env);
void check_RightValueList(std::ostream& out,std::vector<aA_rightVal> rightVals, aA_type type, ScopeEnv env);

