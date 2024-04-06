#include "Error.h"

void error_print(std::ostream* out, A_pos p, string info)
{
    *out << "Typecheck error in line " << p->line << ", col " << p->col << ": " << info << std::endl;
    exit(0);
}

void Error::Redefinition(A_pos p, string name) {
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "var '%s' is already defined", name.c_str());
    error_print(&std::cout, p, msg);
}

void Error::Duplicate(A_pos p, string name) {
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "duplicate member '%s'", name.c_str());
    error_print(&std::cout, p, msg);
}

void Error::UnknowType(A_pos p, string type) {
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "unknown type name '%s'", type.c_str());
    error_print(&std::cout, p, msg);
}

void Error::NoViableConversion(A_pos p, string from, string to) {
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "no viable conversion from '%s' to '%s'", from.c_str(), to.c_str());
    error_print(&std::cout, p, msg);
}

void Error::UseOfUndeclaredId(A_pos p, string name) {
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "var %s on line %d, col %d is not defined", name.c_str(), p->line, p->col);
    error_print(&std::cout, p, msg);
}

void Error::DeducedTypeInitializer(A_pos p, string name) {
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "declaration of variable '%s' with deduced type 'auto' requires an initializer", name.c_str());
    error_print(&std::cout, p, msg);
}

void Error::BinaryExpressionInvalidOperand(A_pos p, string x, string y) {
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "'%s' is not comparable with '%s')", x.c_str(), y.c_str());
    error_print(&std::cout, p, msg);
}

void Error::ArrayExcessElement(A_pos p) {
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "excess elements in array initializer");
    error_print(&std::cout, p, msg);
}

void Error::ArrayNegativeSize(A_pos p, string name){
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "'%s' declared as an array with a negative size", name.c_str());
    error_print(&std::cout, p, msg);
}

void Error::ArraySubscriptValue(A_pos p, string name){
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "subscripted value '%s' is not an array, pointer, or vector", name.c_str());
    error_print(&std::cout, p, msg);
}

void Error::ArrayPastEnd(A_pos p, int index, int size){
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "array index %d is past the end of the array (which contains 1%d element)", index, size);
    error_print(&std::cout, p, msg);
}

void Error::StructNoMember(A_pos p, string member, string structName){
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "no member named '%s' in '%s'", member.c_str(), structName.c_str());
    error_print(&std::cout, p, msg);
}

void Error::StructNotStruct(A_pos p, string id, string type){
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "'%s'(type: '%s') is not a struct", id.c_str(), type.c_str());
    error_print(&std::cout, p, msg);
}

void Error::FunctionArgsSize(A_pos p, int require, int provided){
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "candidate function not viable: requires %d arguments, but %d was provided", require, provided);
    error_print(&std::cout, p, msg);
}

void Error::FunctionReturnOverload(A_pos p, string name, A_pos old){
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, " function definition on line %d doesn't match the declaration on line %d", p->line, old->line);
    error_print(&std::cout, p, msg);
}

void Error::FunctionReturnType(A_pos p, string should, string but){
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, " cannot initialize return object of type '%s' with an left value of type '%s'", should.c_str(), but.c_str());
    error_print(&std::cout, p, msg);
}

void Error::FunctionVoidReturn(A_pos p){
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "void function should not return a value");
    error_print(&std::cout, p, msg);
}

void Error::t_ArrayIdInArithExpr(A_pos p){
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "array id not available in arithmetic expression");
    error_print(&std::cout, p, msg);
}

void Error::t_OverrideFunc(A_pos p, string name){
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "'%s' override or redefinition not available", name.c_str());
    error_print(&std::cout, p, msg);
}

void Error::t_StructArrayMember(A_pos p, string name){
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "array member '%s' expression not available", name.c_str());
    error_print(&std::cout, p, msg);
}

void Error::t_BoolExprNotAllowed(A_pos p){
    char msg[BUFFER_SIZE] = { 0 };
    snprintf(msg, sizeof(msg)-1, "bool expression only available for int");
    error_print(&std::cout, p, msg);
}


