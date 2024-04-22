#include "Error.h"

void error_print(std::ostream& out, A_pos p,string name,  string info)
{
    out << "Typecheck error in line " << p->line << ", col " << p->col << ": " << name << info << std::endl;
    exit(0);
}

void Error::Redefinition(A_pos p, string name) {
    error_print(std::cout, p, name, " is already defined");
}

void Error::Duplicate(A_pos p, string name) {
    error_print(std::cout, p, name, " is duplicate member ");
}

void Error::UnknowType(A_pos p, string type) {
    error_print(std::cout, p, type, "is unknown type name ");
}

void Error::NoViableConversion(A_pos p, string from, string to) {
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "no viable conversion from '%s' to '%s'", from.c_str(), to.c_str());
    error_print(std::cout, p, "", msg);
}

void Error::UseOfUndeclaredId(A_pos p, string name) {
    error_print(std::cout, p, name, " is undefined var, cannot be assigned to");
}

void Error::DeducedTypeInitializer(A_pos p, string name) {
    error_print(std::cout, p, name, " is declared with uncertain type");
}

void Error::BinaryExpressionInvalidOperand(A_pos p, string x, string y) {
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "'%s' is not comparable with '%s')", x.c_str(), y.c_str());
    error_print(std::cout, p, "", msg);
}

void Error::ArrayExcessElement(A_pos p) {
    error_print(std::cout, p, "", " elements not match with array ");
}

void Error::ArrayNegativeSize(A_pos p, string name){
    error_print(std::cout, p, name, " declare negative size");
}

void Error::ArraySubscriptValue(A_pos p, string name){
    error_print(std::cout, p, name, " is not  array/pointer/vector");
}

void Error::ArrayPastEnd(A_pos p, int index, int size){
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "%d index of array is past the end of the array (which contains 1%d element)", index, size);
    error_print(std::cout, p, "", msg);
}

void Error::StructNoMember(A_pos p, string member, string structName){
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "no member named '%s' in '%s'", member.c_str(), structName.c_str());
    error_print(std::cout, p, "", msg);
}

void Error::StructNotStruct(A_pos p, string id, string type){
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "'%s'(type: '%s') is not a struct", id.c_str(), type.c_str());
    error_print(std::cout, p, "", msg);
}

void Error::FunctionArgsSize(A_pos p, int require, int provided){
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "the function requires %d arguments, but %d was provided", require, provided);
    error_print(std::cout, p, "", msg);
}

void Error::FunctionReturnOverload(A_pos p, string name, A_pos old){
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, " function definition on line %d doesn't match the declaration on line %d", p->line, old->line);
    error_print(std::cout, p, "", msg);
}

void Error::FunctionReturnType(A_pos p, string should, string but){
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, " return type'%s' cannot be created an left value of type '%s'", should.c_str(), but.c_str());
    error_print(std::cout, p, "", msg);
}

void Error::FunctionVoidReturn(A_pos p){
    error_print(std::cout, p, "", "void function should not return a value");
}

void Error::t_ArrayIdInArithExpr(A_pos p){
    error_print(std::cout, p, "", "array id not available in arth expression");
}

void Error::t_OverrideFunc(A_pos p, string name){
    error_print(std::cout, p, name, " can not be redefined");
}

void Error::t_StructArrayMember(A_pos p, string name){
    error_print(std::cout, p, name, " array member  expression not available");
}

void Error::t_BoolExprNotAllowed(A_pos p){
    error_print(std::cout, p, "", "bool expression only available for int");
}


