#ifndef GUARD_TYPE_ERROR
#define GUARD_TYPE_ERROR

#include <iostream>
#include "TeaplAst.h"
#include "TeaplaAst.h"
#define BUFFER_SIZE 200

void error_print(std::ostream* out, A_pos p, string info);
class Error{
public:
    static void Redefinition(A_pos p, string name);
    static void Duplicate(A_pos p, string name);
    static void UnknowType(A_pos p, string type);
    static void NoViableConversion(A_pos p, string from, string to);

    static void UseOfUndeclaredId(A_pos p, string name);
    static void DeducedTypeInitializer(A_pos p, string name);

    static void BinaryExpressionInvalidOperand(A_pos p, string x, string y);
    
    static void ArrayExcessElement(A_pos p);
    static void ArrayNegativeSize(A_pos p, string name);
    static void ArraySubscriptValue(A_pos p, string name);
    static void ArrayPastEnd(A_pos p, int index, int size);
    
    static void StructNoMember(A_pos p, string member, string structName);
    static void StructNotStruct(A_pos p, string id, string type);

    static void FunctionArgsSize(A_pos p, int require, int provided);
    static void FunctionReturnOverload(A_pos p, string name, A_pos old);
    static void FunctionReturnType(A_pos p, string should, string but);
    static void FunctionVoidReturn(A_pos p);
    
    static void t_ArrayIdInArithExpr(A_pos p);
    static void t_OtherTypeInArithExpr(A_pos p);
    static void t_OverrideFunc(A_pos p, string name);
    static void t_StructArrayMember(A_pos p, string name);
    static void t_BoolExprNotAllowed(A_pos p);

    static void strict_FunctionParamsName(A_pos p, string new_name, string old_name);
};

#endif
