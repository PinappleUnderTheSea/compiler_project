#include "utils.h"


typeMap g_token2Type; // global token ids to type
typeMap funcparam_token2Type; // func params token ids to type

paramMemberMap func2Param;
paramMemberMap struct2Members;

std::unordered_map<string, std::pair<aA_type, Function_Status>> funcStatues;

typeMap current_scope_token;

bool STRICT_DEFINITON = true;
bool STRICT_FUNCTION_DEFINITON = true;

aA_type GetTypeFromId(string id) {
    typeMap::const_iterator iter = current_scope_token.find(id);
    if (iter == current_scope_token.end()){
        iter = g_token2Type.find(id);
        if (iter == g_token2Type.end()) {
            return nullptr;
        }
    }
    
    return iter->second.first->type;
}

void SetTypeFromId(string id, aA_type type) {
    typeMap::const_iterator iter = current_scope_token.find(id);
    if (iter == current_scope_token.end()){
        return ;
    }
    
    iter->second.first->type = type;
}

string GetTypeString(aA_type type) {
    if (!type) return "auto";
    string type_name = "";
    if (type->type == A_dataType::A_nativeTypeKind){
        if (type->u.nativeType == A_nativeType::A_intTypeKind){
            type_name = "int";
        }
    }else{
        type_name = *type->u.structType;
    }
    return type_name;
}

bool isExist(string id) {
    typeMap::const_iterator iter = current_scope_token.find(id);
    if (iter == current_scope_token.end()){
        iter = g_token2Type.find(id);
        if (iter == g_token2Type.end()) {
            return false;
        }
    }
    
    return true;
}

bool isStruct(string id) {
    typeMap::const_iterator iter = current_scope_token.find(id);
    if (iter == current_scope_token.end()){
        iter = g_token2Type.find(id);
        if (iter == g_token2Type.end()) {
            return false;
        }
    }
    aA_type id_type = iter->second.first->type;
    if (!id_type) return false;
    if (id_type->type == A_dataType::A_structTypeKind){
        return true;
    }
    return false;
}

bool isStruct(string id, string name) {
    typeMap::const_iterator iter = current_scope_token.find(id);
    if (iter == current_scope_token.end()){
        iter = g_token2Type.find(id);
        if (iter == g_token2Type.end()) {
            return false;
        }
    }
    aA_type id_type = iter->second.first->type;
    if (id_type->type == A_dataType::A_structTypeKind && *id_type->u.structType == name){
        return true;
    }
    return false;
}

bool isArray(string id) {
    typeMap::const_iterator iter = current_scope_token.find(id);
    if (iter == current_scope_token.end()){
        iter = g_token2Type.find(id);
        if (iter == g_token2Type.end()) {
            return false;
        }
    }
    ValueInfo vi = iter->second.second;
    if (vi.type == Check_ValType::Check_Array){
        return true;
    }
    return false;
}

bool isArray(string structId, string memberId) {
    string type_name = *GetTypeFromId(structId)->u.structType;
    paramMemberMap::const_iterator iter = struct2Members.find(type_name);
    std::vector<aA_varDecl> members = *iter->second;
    for (aA_varDecl member: members) {
        if (member->kind == A_varDeclType::A_varDeclScalarKind){
            if (*member->u.declScalar->id == memberId){
                return false;
            }
        } else {
            if (*member->u.declArray->id == memberId){
                return true;
            }
        }
    }
    return false;
}

aA_type GetNativeTypeInstance(A_nativeType data_type) {
    aA_type p = new aA_type_;
    p->pos = nullptr;
    p->type = A_dataType::A_nativeTypeKind;
    p->u.nativeType = data_type;
    return p;
}

bool isStructToken(string token) {
    paramMemberMap::const_iterator iter = struct2Members.find(token);
    if (iter == struct2Members.end()){
        return false;
    }
    return true;
}

aA_type isInStruct(string memberId, string structName) {
    paramMemberMap::const_iterator iter = struct2Members.find(structName);
    if (iter == struct2Members.end()){
        return nullptr;
    }
    std::vector<aA_varDecl> params = *iter->second;
    for (aA_varDecl param: params) {
        if (param->kind == A_varDeclType::A_varDeclArrayKind){
            Error::t_StructArrayMember(param->u.declArray->pos, *param->u.declArray->id);
        } else {
            string id = *param->u.declScalar->id;
            if (id == memberId) {
                return param->u.declScalar->type;
            }
        }
    }
    return nullptr;
}

bool TypeValid(aA_type type){
    if (type->type == A_dataType::A_nativeTypeKind){
        return true;
    }
    if (type->type == A_dataType::A_structTypeKind && isStructToken(*type->u.structType)){
        return true;
    }
    std::cerr << "detail type: " << type->type << std::endl;
    return false;
}

bool isSameType(aA_type x, aA_type y){
    if (x->type != y->type){
        return false;
    }
    if (x->type == A_dataType::A_nativeTypeKind && x->u.nativeType == y->u.nativeType){
        return true;
    }
    if (x->type == A_dataType::A_structTypeKind && *x->u.structType == *y->u.structType){
        return true;
    }
    return false;
}

int getSizeFromArrayId(string id){
    if (!isExist(id)){
        return -1;
    }
    if (!isArray(id)){
        return -1;
    }
    typeMap::const_iterator iter = current_scope_token.find(id);
    if (iter == current_scope_token.end()){
        iter = g_token2Type.find(id);
    }
    return iter->second.second.size;
}

aA_type getReturnTypeByName(string name) {
    std::unordered_map<string, std::pair<aA_type, Function_Status>>::const_iterator iter = funcStatues.find(name);
    if (iter == funcStatues.end()) {
        return nullptr;
    }
    return iter->second.first;
}

aA_type check_ArrayExpr(std::ostream* out, aA_arrayExpr ae){
    if(!ae)
        return nullptr;

    string id = *ae->arr->u.id;
    // exist
    if (!isExist(id)){
        Error::UseOfUndeclaredId(ae->pos, id);
    }
    // array ?
    if (!isArray(id)){
        Error::ArraySubscriptValue(ae->pos, id);
    }

    int size = getSizeFromArrayId(id);
    int index = 0;
    if (ae->idx->kind == A_indexExprKind::A_numIndexKind){
        index = ae->idx->u.num;
        if (size <= index) {
            Error::ArrayPastEnd(ae->idx->pos, index, size);
        }
    } else {
        string idexId = *ae->idx->u.id;
        if (!isExist(idexId)){
            Error::UseOfUndeclaredId(ae->idx->pos, idexId);
        }
        aA_type idexIdType = GetTypeFromId(idexId);
        if (!isSameType(idexIdType, GetNativeTypeInstance(A_nativeType::A_intTypeKind))){
            Error::NoViableConversion(ae->idx->pos, GetTypeString(idexIdType), "int");
        }
    }
    
    return GetTypeFromId(id);
}

aA_type check_MemberExpr(std::ostream* out, aA_memberExpr me) {
    if(!me)
        return nullptr;
    string memberId = *me->memberId;
    string structID = *me->structId->u.id;

    if (!isExist(structID)) {
        Error::UseOfUndeclaredId(me->pos, structID);
    }
    if (!isStruct(structID)) {
        Error::StructNotStruct(me->pos, structID, GetTypeString(GetTypeFromId(structID)));
    }
    aA_type type = isInStruct(memberId, GetTypeString(GetTypeFromId(structID)));

    if (!type) {
        Error::StructNoMember(me->pos, memberId, GetTypeString(GetTypeFromId(structID)));
    }

    return type;
}