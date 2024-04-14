#ifndef GUARD_UTILS
#define GUARD_UTILS

#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <string.h>
#include "Error.h"
#include "TeaplAst.h"
#include "TeaplaAst.h"


typedef enum {
    Check_Scalar,
    Check_Array,
} Check_ValType;

struct ValueInfo {
    Check_ValType type;
    int size;
    ValueInfo(Check_ValType type, int size): type(type), size(size){};
};

typedef enum {
    Definition,
    Declaration,
} Function_Status;

// you can use this type to store the type a token.
typedef enum {
    GLOBAL,
    CODEBLOCK,
    FUNCTION_CALL,
} ArithExprEnv;

struct tc_type_{
    aA_type type;
    uint isVarArrFunc; // 0 for scalar, 1 for array, 2 for function
};
typedef struct tc_type_* tc_type;

typedef std::unordered_map<string, std::pair<tc_type, ValueInfo>> typeMap; 
typedef std::unordered_map<string, vector<aA_varDecl>*> paramMemberMap;

extern typeMap g_token2Type; // global token ids to type
extern typeMap funcparam_token2Type; // func params token ids to type
extern typeMap runtime_token;
// you can use this map to store the members of a struct or params of a function.

extern paramMemberMap func2Param;
extern paramMemberMap struct2Members;

extern std::unordered_map<string, std::pair<aA_type, Function_Status>> funcStatues;


extern bool STRICT_DEFINITON;
extern bool STRICT_FUNCTION_DEFINITON;
extern aA_type env_return_type;


aA_type GetTypeFromId(string id);
string GetTypeString(aA_type type);
bool isExist(string id);
bool isStruct(string id);
bool isStruct(string id, string name);
bool isArray(string id);
bool isArray(string structId, string memberId);
aA_type GetNativeTypeInstance(A_nativeType data_type);
bool isStructToken(string token);
aA_type isInStruct(string memberId, string structName);
bool TypeValid(aA_type type);
bool isSameType(aA_type x, aA_type y);
int getSizeFromArrayId(string id);
aA_type getReturnTypeByName(string name);

aA_type check_ArrayExpr(std::ostream* out, aA_arrayExpr ae);
aA_type check_MemberExpr(std::ostream* out, aA_memberExpr me);
#endif