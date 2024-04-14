#include "TypeCheck.h"
#include "Error.h"
#include "utils.h"
//global tabels
//typeMap func2retType; // function name to return type

// global token ids to type

// local token ids to type, since func param can override global param
vector<typeMap*> local_token2Type;

// private util functions
void error_print(std::ostream& out, A_pos p, string info)
{
    out << "Typecheck error in line " << p->line << ", col " << p->col << ": " << info << std::endl;
    exit(0);
}


void print_token_map(typeMap* map){
    for(auto it = map->begin(); it != map->end(); it++){
        std::cout << it->first << " : ";
        switch (it->second.first->type->type)
        {
        case A_dataType::A_nativeTypeKind:
            switch (it->second.first->type->u.nativeType)
            {
            case A_nativeType::A_intTypeKind:
                std::cout << "int";
                break;
            default:
                break;
            }
            break;
        case A_dataType::A_structTypeKind:
            std::cout << *(it->second.first->type->u.structType);
            break;
        default:
            break;
        }
        switch(it->second.first->isVarArrFunc){
            case 0:
                std::cout << " scalar";
                break;
            case 1:
                std::cout << " array";
                break;
            case 2:
                std::cout << " function";
                break;
        }
        std::cout << std::endl;
    }
}


void print_token_maps(){
    std::cout << "global token2Type:" << std::endl;
    print_token_map(&g_token2Type);
    std::cout << "local token2Type:" << std::endl;
    print_token_map(&funcparam_token2Type);
}


bool comp_aA_type(aA_type target, aA_type t){
    if(!target || !t)
        return false;
    if(target->type != t->type)
        return false;
    if(target->type == A_dataType::A_nativeTypeKind)
        if(target->u.nativeType != t->u.nativeType)
            return false;
    if(target->type == A_dataType::A_structTypeKind)
        if(target->u.structType != t->u.structType)
            return false;
    return true;
}


bool comp_tc_type(tc_type target, tc_type t){
    if(!target || !t)
        return false;
    
    // arr kind first
    if (target->isVarArrFunc && t->isVarArrFunc == 0)
        return false;
    
    // if target type is nullptr, alwayse ok
    return comp_aA_type(target->type, t->type);
}


tc_type tc_Type(aA_type t, uint isVarArrFunc){
    tc_type ret = new tc_type_;
    ret->type = t;
    ret->isVarArrFunc = isVarArrFunc;
    return ret;
}


tc_type tc_Type(aA_varDecl vd){
    if(vd->kind == A_varDeclType::A_varDeclScalarKind)
        return tc_Type(vd->u.declScalar->type, 0);
    else if(vd->kind == A_varDeclType::A_varDeclArrayKind)
        return tc_Type(vd->u.declArray->type, 1);
    return nullptr;
}


// public functions
void check_Prog(std::ostream& out, aA_program p)
{
    for (auto ele : p->programElements)
    {
        if(ele->kind == A_programVarDeclStmtKind){
            check_VarDecl(out, ele->u.varDeclStmt, ArithExprEnv::GLOBAL);
        }else if (ele->kind == A_programStructDefKind){
            check_StructDef(out, ele->u.structDef);
        }
    }
    
    for (auto ele : p->programElements){
        if(ele->kind == A_programFnDeclStmtKind){
            check_FnDeclStmt(out, ele->u.fnDeclStmt);
        }
        else if (ele->kind == A_programFnDefKind){
            check_FnDecl(out, ele->u.fnDef->fnDecl);
        }
    }

    for (auto ele : p->programElements){
        if(ele->kind == A_programFnDefKind){
            check_FnDef(out, ele->u.fnDef);
        }
        else if (ele->kind == A_programNullStmtKind){
            // do nothing
        }
    }

    out << "Typecheck passed!" << std::endl;
    return;
}


void check_VarDecl(std::ostream& out, aA_varDeclStmt vd, ArithExprEnv env)
{
    if (!vd)
        return;
    string name;

    if (vd->kind == A_varDeclStmtType::A_varDeclKind){
        // decl only
        aA_varDecl vdecl = vd->u.varDecl;
        if(vdecl->kind == A_varDeclType::A_varDeclScalarKind){
            name = *vdecl->u.declScalar->id;
            /* fill code here*/
            if(env == ArithExprEnv::GLOBAL){
                // std::cerr << "global name :" << name << std::endl;
            }
            if (isExist(name)){
                Error::Redefinition(vd->u.varDecl->u.declScalar->pos, name);
            }
            aA_type type = vd->u.varDecl->u.declScalar->type;
            // std::cerr << name << " type= " << type->type << std::endl;
            if (type && !TypeValid(type)){
                Error::UnknowType(type->pos, *type->u.structType);
            }
            if(env == ArithExprEnv::GLOBAL){
                runtime_token.insert({name, std::pair<tc_type, ValueInfo>(tc_Type(type, 0), ValueInfo(Check_Scalar, 0))});
            }
        }else if (vdecl->kind == A_varDeclType::A_varDeclArrayKind){
            name = *vdecl->u.declArray->id;
            /* fill code here*/
            if (isExist(name)){
                Error::Redefinition(vd->pos, name);
            }
            int len = vd->u.varDecl->u.declArray->len;
            if (len <= 0){
                Error::ArrayNegativeSize(vd->pos, name);
            }

            aA_type type = vd->u.varDecl->u.declArray->type;
            if (type && !TypeValid(type)){
                Error::UnknowType(type->pos, *type->u.structType);
            }
            if(env == ArithExprEnv::GLOBAL){
                runtime_token.insert({name, std::pair<tc_type, ValueInfo>(tc_Type(type, 1), ValueInfo(Check_Array, len))});
            }
        }
    }
    else if (vd->kind == A_varDeclStmtType::A_varDefKind){
        // decl and def
        aA_varDef vdef = vd->u.varDef;
        if (vdef->kind == A_varDefType::A_varDefScalarKind){
            name = *vdef->u.defScalar->id;
            /* fill code here, allow omited type */
            if (isExist(name)){
                Error::Redefinition(vd->pos, name);
            }
            // Type Valid
            aA_type type = vdef->u.defScalar->type;
            // std::cerr << name << " type = "<< type->type << " typename: " << *type->u.structType <<  std::endl;
            if (type && !TypeValid(type)){
                Error::UnknowType(type->pos, *type->u.structType);
            }
            // RightValue Invariant
            aA_rightVal rightVal = vd->u.varDef->u.defScalar->val;
            check_RightValue(out, rightVal, type, ArithExprEnv::GLOBAL);
            if(env == ArithExprEnv::GLOBAL){
                runtime_token.insert({name, std::pair<tc_type, ValueInfo>(tc_Type(type, 0), ValueInfo(Check_Scalar, 0))});
            }

        }else if (vdef->kind == A_varDefType::A_varDefArrayKind){
            name = *vdef->u.defArray->id;
            /* fill code here, allow omited type */
            if (isExist(name)){
                Error::Redefinition(vd->pos, name);
            }
            // Type Valid
            aA_type type = vd->u.varDef->u.defArray->type;
            if (type && !TypeValid(type)){
                Error::UnknowType(type->pos, *type->u.structType);
            }
            // Initializer invariant
            int len = vd->u.varDef->u.defArray->len;
            if (len <= 0){
                Error::ArrayNegativeSize(vd->pos, name);
            }
            // RightValue Invariant
            // Excess
            std::vector<aA_rightVal> rightVals = vd->u.varDef->u.defArray->vals;
            if (rightVals.size() > len){
                Error::ArrayExcessElement(rightVals[len-1]->pos);
            }
            check_RightValueList(out, rightVals, type, ArithExprEnv::GLOBAL);
            if(env == ArithExprEnv::GLOBAL){
                runtime_token.insert({name, std::pair<tc_type, ValueInfo>(tc_Type(type, 1), ValueInfo(Check_Array, len))});
            }
        }
    }
    return;
}


void check_StructDef(std::ostream& out, aA_structDef sd)
{
    if (!sd)
        return;
    string name = *sd->id;
    if (struct2Members.find(name) != struct2Members.end())
        error_print(out, sd->pos, "This id is already defined!");
    struct2Members[name] = &(sd->varDecls);
    return;
}

void check_paramDecl(std::ostream& out, aA_paramDecl pd){
    std::vector<string> innerNames;
    for (aA_varDecl varDecl: pd->varDecls){
        // struct namespace duplication && type check
        if (varDecl->kind == A_varDeclType::A_varDeclScalarKind) {
            std::vector<string>::const_iterator iter = std::find(innerNames.begin(), innerNames.end(), *varDecl->u.declScalar->id);
            if (iter == innerNames.end()){
                innerNames.push_back(*varDecl->u.declScalar->id);
            }else{
                Error::Duplicate(varDecl->pos, *varDecl->u.declScalar->id);
            }

            aA_type type = varDecl->u.declScalar->type;
            if (type && !TypeValid(type)) {
                Error::UnknowType(varDecl->pos, GetTypeString(type));
            }
        }
        
        if (varDecl->kind == A_varDeclType::A_varDeclArrayKind) {
            std::vector<string>::const_iterator iter = std::find(innerNames.begin(), innerNames.end(), *varDecl->u.declArray->id);
            if (iter == innerNames.end()){
                innerNames.push_back(*varDecl->u.declArray->id);
            }else{
                Error::Duplicate(varDecl->pos, *varDecl->u.declArray->id);
            }

            aA_type type = varDecl->u.declArray->type;
            if (type && !TypeValid(type)) {
                Error::UnknowType(varDecl->pos, GetTypeString(type));
            }
        }
    } 
}



void check_FnDecl(std::ostream& out, aA_fnDecl fd)
{
    if (!fd)
        return;
    string name = *fd->id;
    aA_paramDecl paramDecl = fd->paramDecl;
    std::vector<aA_varDecl> varDecls = paramDecl->varDecls;
    aA_type rtType = fd->type;
    // if already declared, should match
    if (func2Param.find(name) != func2Param.end()){
        // is function ret val matches
        /* fill code here */
        auto oldRet = funcStatues.find(name)->second.first;
        auto newRet = fd->type;
        if(!isSameType(oldRet, newRet)){
            Error::FunctionReturnOverload(fd->pos, name, oldRet->pos);
        }

        // is function params matches decl
        /* fill code here */
        auto oldFuncParams = *func2Param.find(name)->second;
        auto newFuncParams = fd->paramDecl->varDecls;
        if (newFuncParams.size() != oldFuncParams.size()){
            Error::t_OverrideFunc(fd->pos, *fd->id);
        }
        for (int i=0;i<newFuncParams.size();i++){
            if (newFuncParams[i]->kind != oldFuncParams[i]->kind){
                Error::t_OverrideFunc(fd->pos, *fd->id);
            }
            aA_type new_t = nullptr, old_t = nullptr;
            if (newFuncParams[i]->kind == A_varDeclType::A_varDeclArrayKind){
                new_t = newFuncParams[i]->u.declArray->type;
                old_t = oldFuncParams[i]->u.declArray->type;
            } else {
                new_t = newFuncParams[i]->u.declScalar->type;
                old_t = oldFuncParams[i]->u.declScalar->type;
            }
            if (isSameType(new_t, old_t)){
                
            } else {
                Error::t_OverrideFunc(fd->pos, *fd->id);
            }
        }
    }else{
        // if not defined
        /* fill code here */
        check_paramDecl(out, paramDecl);
        std::unordered_map<string, std::pair<aA_type, Function_Status>>::iterator iter = funcStatues.find(*fd->id);
        if (iter != funcStatues.end()){
            iter->second.second = Function_Status::Definition;
        } else {
            func2Param.insert({*fd->id, &fd->paramDecl->varDecls});
            funcStatues.insert({*fd->id, std::pair<aA_type, Function_Status>(fd->type, Function_Status::Declaration)});
        }
    }
    return;
}


void check_FnDeclStmt(std::ostream& out, aA_fnDeclStmt fd)
{
    if (!fd)
        return;
    check_FnDecl(out, fd->fnDecl);
    return;
}

string varDecl_InsertRunTime(aA_varDeclStmt vd){
    if (vd->kind == A_varDeclStmtType::A_varDeclKind){
        if (vd->u.varDecl->kind == A_varDeclType::A_varDeclScalarKind){
            string name = *vd->u.varDecl->u.declScalar->id;
            aA_type type = vd->u.varDecl->u.declScalar->type;
            runtime_token.insert({name, std::pair<tc_type, ValueInfo>(tc_Type(type, 0), ValueInfo(Check_Scalar, 0))});
            return name;
        }

        if (vd->u.varDecl->kind == A_varDeclType::A_varDeclArrayKind){
            string name = *vd->u.varDecl->u.declArray->id;
            aA_type type = vd->u.varDecl->u.declArray->type;
            int len = vd->u.varDecl->u.declArray->len;
            runtime_token.insert({name, std::pair<tc_type, ValueInfo>(tc_Type(type, 1), ValueInfo(Check_Array, len))});
            return name;
        }
    }
    else if (vd->kind == A_varDeclStmtType::A_varDefKind){
        if (vd->u.varDef->kind == A_varDefType::A_varDefScalarKind){
            string name = *vd->u.varDef->u.defScalar->id;
            aA_type type = vd->u.varDef->u.defScalar->type;
            runtime_token.insert({name, std::pair<tc_type, ValueInfo>(tc_Type(type, 0), ValueInfo(Check_Scalar, 0))});
            return name;
        }

        if (vd->u.varDef->kind == A_varDefType::A_varDefArrayKind){
            string name = *vd->u.varDef->u.defArray->id;
            aA_type type = vd->u.varDef->u.defArray->type;
            int len = vd->u.varDef->u.defArray->len;
            runtime_token.insert({name, std::pair<tc_type, ValueInfo>(tc_Type(type, 1), ValueInfo(Check_Array, len))});
            return name;
        }
    }
}

void check_FnDef(std::ostream& out, aA_fnDef fd)
{
    if (!fd)
        return;
    // should match if declared
    check_FnDecl(out, fd->fnDecl);
    // add params to local tokenmap, func params override global ones
    // runtime_token.clear();
    typeMap *scope_token2Type = new typeMap;
    local_token2Type.push_back(scope_token2Type);
    for (aA_varDecl vd : fd->fnDecl->paramDecl->varDecls)
    {
        /* fill code here */
        if (vd->kind == A_varDeclType::A_varDeclScalarKind){
            aA_varDeclScalar scalar = vd->u.declScalar;
            string valName = *vd->u.declScalar->id;
            if(runtime_token.find(valName) != runtime_token.end()){
                // std::cerr << "conflict name :" << valName<<" new type:"<< (scalar->type->type) << " global count = " << (runtime_token.size())<< std::endl;
                scope_token2Type->insert({valName, runtime_token.find(valName)->second});
                runtime_token.erase(runtime_token.find(valName));
            }
            runtime_token.insert({valName, std::pair<tc_type, ValueInfo>(tc_Type(scalar->type, 0), ValueInfo(Check_ValType::Check_Scalar, 0))});
            // std::cerr <<"new type: " <<(runtime_token.find(valName)->second.first->type->type)<< " should be "<< (scalar->type->type) <<std::endl;
            // std::cerr <<"old type: " <<(scope_token2Type->find(valName)->second.first->type->type);
        }
        if (vd->kind == A_varDeclType::A_varDeclArrayKind){
            aA_varDeclArray array = vd->u.declArray;
            string valName = *vd->u.declScalar->id;
            if(runtime_token.find(valName) != runtime_token.end()){
                scope_token2Type->insert({valName, runtime_token.find(valName)->second});
                runtime_token.erase(runtime_token.find(valName));
            }
            runtime_token.insert({*array->id, std::pair<tc_type, ValueInfo>(tc_Type(array->type, 1), ValueInfo(Check_ValType::Check_Array, array->len))});
        }
    }

    /* fill code here */
    std::vector<string> typenames;
    env_return_type = fd->fnDecl->type;
    for (aA_codeBlockStmt stmt : fd->stmts)
    {
        check_CodeblockStmt(out, stmt);
        // return value type should match
        /* fill code here */
        if (stmt->kind == A_codeBlockStmtType::A_varDeclStmtKind){
            string name = varDecl_InsertRunTime(stmt->u.varDeclStmt);
            typenames.push_back(name);
        }
        if (stmt->u.returnStmt->retVal && !env_return_type){
            Error::FunctionVoidReturn(stmt->u.returnStmt->pos);
        }
        if (stmt->kind == A_codeBlockStmtType::A_returnStmtKind){
            check_RightValue(out, stmt->u.returnStmt->retVal, env_return_type, ArithExprEnv::CODEBLOCK);
        }
    }
    for (string name: typenames){
        typeMap::const_iterator iter = runtime_token.find(name);
        if (iter == runtime_token.end()){
        } else {
            string name = iter->first;
            runtime_token.erase(iter);
        }
    }

    for(auto iter = scope_token2Type->begin(); iter != scope_token2Type->end(); iter++){
        // std::cerr << "recover name:" <<(iter->first) << " type :" <<(iter->second.first->type->type) << std::endl;
        if(runtime_token.find(iter->first) != runtime_token.end()){
            runtime_token.erase(iter->first);
        }
        runtime_token.insert({iter->first, iter->second});
        // std::cerr << "recover ans name:" <<(iter->first) << " type :" <<((runtime_token.find(iter->first))->second.first->type->type) << std::endl;

    }
    local_token2Type.erase(local_token2Type.end()-1);
    delete scope_token2Type;
    return;
}


void check_CodeblockStmt(std::ostream& out, aA_codeBlockStmt cs){
    if(!cs)
        return;
    // variables declared in a code block should not duplicate with outer ones.
    switch (cs->kind)
    {
    case A_codeBlockStmtType::A_varDeclStmtKind:
        check_VarDecl(out, cs->u.varDeclStmt, ArithExprEnv::CODEBLOCK);
        break;
    case A_codeBlockStmtType::A_assignStmtKind:
        check_AssignStmt(out, cs->u.assignStmt);
        break;
    case A_codeBlockStmtType::A_ifStmtKind:
        check_IfStmt(out, cs->u.ifStmt);
        break;
    case A_codeBlockStmtType::A_whileStmtKind:
        check_WhileStmt(out, cs->u.whileStmt);
        break;
    case A_codeBlockStmtType::A_callStmtKind:
        check_CallStmt(out, cs->u.callStmt);
        break;
    case A_codeBlockStmtType::A_returnStmtKind:
        check_ReturnStmt(out, cs->u.returnStmt);
        break;
    default:
        break;
    }
    return;
}


void check_AssignStmt(std::ostream& out, aA_assignStmt as){
    if(!as)
        return;
    string name;
    tc_type deduced_type; // deduced type if type is omitted at decl
    // std::cerr<<name<<" type is " << as->leftVal->kind<<std::endl;
    switch (as->leftVal->kind)
    {
        case A_leftValType::A_varValKind:{
            name = *as->leftVal->u.id;
            /* fill code here */
            if (!isExist(name)){
                Error::UseOfUndeclaredId(as->pos, name);
            }
            
            // right value invariance
            aA_rightVal right = as->rightVal;
            aA_type rightType = nullptr, leftType = GetTypeFromId(name);
            check_RightValue(out, right, leftType, ArithExprEnv::CODEBLOCK);
        }
            break;
        case A_leftValType::A_arrValKind:{
            name = *as->leftVal->u.arrExpr->arr->u.id;
            /* fill code here */
            check_ArrayExpr(out, as->leftVal->u.arrExpr);
            aA_type type = GetTypeFromId(name);
            aA_rightVal right = as->rightVal;
            check_RightValue(out, right, type, ArithExprEnv::CODEBLOCK);
        }
            break;
        case A_leftValType::A_memberValKind:{
            /* fill code here */
            aA_type type = check_MemberExpr(out, as->leftVal->u.memberExpr)->type;
            aA_rightVal right = as->rightVal;
            check_RightValue(out, right, type, ArithExprEnv::CODEBLOCK);
        }
            break;
    }
    return;
}


void check_ArrayExpr(std::ostream& out, aA_arrayExpr ae){
    if(!ae)
        return;
    string name = *ae->arr->u.id;
    // check array name
    /* fill code here */
        // exist
    if (!isExist(name)){
        Error::UseOfUndeclaredId(ae->pos, name);
    }
    // array ?
    if (!isArray(name)){
        Error::ArraySubscriptValue(ae->pos, name);
    }
    // check index
    /* fill code here */
    int size = getSizeFromArrayId(name);
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
    return;
}

 
    // check if the member exists and return the tyep of the member
tc_type check_MemberExpr(std::ostream& out, aA_memberExpr me){
    if(!me)
        return nullptr;
    string name = *me->structId->u.id;
    // check struct name
    /* fill code here */

    if (!isExist(name)) {
        Error::UseOfUndeclaredId(me->pos, name);
    }
    if (!isStruct(name)) {
        Error::StructNotStruct(me->pos, name, GetTypeString(GetTypeFromId(name)));
    }
    // check member name
    /* fill code here */
    string memberName = *me->memberId;
    aA_type type = isInStruct(memberName, GetTypeString(GetTypeFromId(name)));
    if (!type) {
        Error::StructNoMember(me->pos, memberName, GetTypeString(GetTypeFromId(name)));
    }
    
    return tc_Type(type, 0);
}


void check_IfStmt(std::ostream& out, aA_ifStmt is){
    if(!is)
        return;
    check_BoolExpr(out, is->boolExpr, ArithExprEnv::CODEBLOCK);
    /* fill code here, take care of variable scope */
    std::vector<string> typenames;
    for(aA_codeBlockStmt s : is->ifStmts){
        check_CodeblockStmt(out, s);
        if (s->kind == A_codeBlockStmtType::A_varDeclStmtKind){
            string name = varDecl_InsertRunTime(s->u.varDeclStmt);
            typenames.push_back(name);
        }
    }
    for (string name: typenames){
        typeMap::const_iterator iter = runtime_token.find(name);
        if (iter == runtime_token.end()){
        } else {
            string name = iter->first;
            runtime_token.erase(iter);
        }
    }
    
    /* fill code here */
    std::vector<string> typenames2;  
    for(aA_codeBlockStmt s : is->elseStmts){
        check_CodeblockStmt(out, s);
        if (s->kind == A_codeBlockStmtType::A_varDeclStmtKind){
            string name = varDecl_InsertRunTime(s->u.varDeclStmt);
            typenames2.push_back(name);
        }
    }
    /* fill code here */
    for (string name: typenames2){
        typeMap::const_iterator iter = runtime_token.find(name);
        if (iter == runtime_token.end()){
        } else {
            string name = iter->first;
            runtime_token.erase(iter);
        }
    }
    return;
}


void check_BoolExpr(std::ostream& out, aA_boolExpr be, ArithExprEnv env){
    if(!be)
        return;
    switch (be->kind)
    {
    case A_boolExprType::A_boolBiOpExprKind:
        check_BoolExpr(out, be->u.boolBiOpExpr->left, env);
        check_BoolExpr(out, be->u.boolBiOpExpr->right, env);
        break;
    case A_boolExprType::A_boolUnitKind:
        check_BoolUnit(out, be->u.boolUnit, env);
        break;
    default:
        break;
    }
    return;
}


void check_BoolUnit(std::ostream& out, aA_boolUnit bu, ArithExprEnv env){
    if(!bu)
        return;
    switch (bu->kind)
    {
        case A_boolUnitType::A_comOpExprKind:{
            /* fill code here */
            aA_exprUnit left = bu->u.comExpr->left;
            aA_type left_type = check_ExprUnit(out, left, env)->type;
            aA_exprUnit right = bu->u.comExpr->right;
            aA_type right_type = check_ExprUnit(out, right, env)->type;
            if (!isSameType(left_type, right_type)) {
                Error::BinaryExpressionInvalidOperand(bu->pos, GetTypeString(left_type), GetTypeString(right_type));
            }
            if (!isSameType(left_type, GetNativeTypeInstance(A_nativeType::A_intTypeKind))){
                Error::t_BoolExprNotAllowed(bu->pos);
            }
        }
            break;
        case A_boolUnitType::A_boolExprKind:
            check_BoolExpr(out, bu->u.boolExpr, env);
            break;
        case A_boolUnitType::A_boolUOpExprKind:
            check_BoolUnit(out, bu->u.boolUOpExpr->cond, env);
            break;
        default:
            break;
    }
    return;
}


tc_type check_ExprUnit(std::ostream& out, aA_exprUnit eu, ArithExprEnv env){
    // return the aA_type of expr eu
    if(!eu)
        return nullptr;
    tc_type ret;
    switch (eu->kind)
    {
        case A_exprUnitType::A_idExprKind:{
            /* fill code here */
            const string id = *eu->u.id;
            if (!isExist(id)) {
                Error::UseOfUndeclaredId(eu->pos, id);
            }
            if (isArray(id) && env != ArithExprEnv::FUNCTION_CALL) {
                // array id is not allowed
                Error::t_ArrayIdInArithExpr(eu->pos);
            }
            return tc_Type(GetTypeFromId(id), 0);
        }
            break;
        case A_exprUnitType::A_numExprKind:{
            aA_type numt = new aA_type_;
            numt->pos = eu->pos;
            numt->type = A_dataType::A_nativeTypeKind;
            numt->u.nativeType = A_nativeType::A_intTypeKind;
            ret = tc_Type(numt, 0);
        }
            break;
        case A_exprUnitType::A_fnCallKind:{
            check_FuncCall(out, eu->u.callExpr);
            // check_FuncCall will check if the function is defined
            /* fill code here */
            return tc_Type(getReturnTypeByName(*(eu->u.callExpr->fn)), 2);
        }
            break;
        case A_exprUnitType::A_arrayExprKind:{
            check_ArrayExpr(out, eu->u.arrayExpr);
            /* fill code here */
            return tc_Type(getReturnTypeByName(*(eu->u.arrayExpr->arr->u.id)), 2);
        }
            break;
        case A_exprUnitType::A_memberExprKind:{
            ret = check_MemberExpr(out, eu->u.memberExpr);
        }
            break;
        case A_exprUnitType::A_arithExprKind:{
            ret = check_ArithExpr(out, eu->u.arithExpr, env);
        }
            break;
        case A_exprUnitType::A_arithUExprKind:{
            ret = check_ExprUnit(out, eu->u.arithUExpr->expr, env);
        }
            break;
    }
    return ret;
}


tc_type check_ArithExpr(std::ostream& out, aA_arithExpr ae, ArithExprEnv env){
    if(!ae)
        return nullptr;
    tc_type ret;
    switch (ae->kind)
    {
        case A_arithExprType::A_arithBiOpExprKind:{
            ret = check_ArithExpr(out, ae->u.arithBiOpExpr->left, env);
            tc_type rightTyep = check_ArithExpr(out, ae->u.arithBiOpExpr->right, env);
            if(ret->type->type > 0 || ret->type->type != A_dataType::A_nativeTypeKind || ret->type->u.nativeType != A_nativeType::A_intTypeKind ||
            rightTyep->type->type > 0 || rightTyep->type->type != A_dataType::A_nativeTypeKind || rightTyep->type->u.nativeType != A_nativeType::A_intTypeKind)
                error_print(out, ae->pos, "Only int can be arithmetic expression operation values!");
        }
            break;
        case A_arithExprType::A_exprUnitKind:
            ret = check_ExprUnit(out, ae->u.exprUnit, env);
            break;
    }
    return ret;
}


void check_FuncCall(std::ostream& out, aA_fnCall fc){
    if(!fc)
        return;
    // check if function defined
    string func_name = *fc->fn;
    /* fill code here */
    paramMemberMap::const_iterator iter = func2Param.find(func_name);
    if (iter == func2Param.end()){
        Error::UseOfUndeclaredId(fc->pos, func_name);
    }

    std::vector<aA_varDecl> params = *iter->second;
    std::vector<aA_rightVal> params_call = fc->vals;
    
    // arguments size
    if (params.size() != params_call.size()) {
        Error::FunctionArgsSize(fc->pos, params.size(), params_call.size());
    }

    // type check
    aA_type rtType = getReturnTypeByName(func_name);
    // check if parameter list matches
    for(int i = 0; i < fc->vals.size(); i++){
        /* fill code here */
        aA_varDecl param = params[i];
        aA_rightVal param_call = params_call[i];
        if (param->kind == A_varDeclType::A_varDeclArrayKind) {
            if (param_call->kind == A_rightValType::A_boolExprValKind) {
                Error::NoViableConversion(param_call->pos, "bool", "array");
            } else {
                aA_type right_type = check_ArithExpr(out, param_call->u.arithExpr, ArithExprEnv::FUNCTION_CALL)->type;
                if (param_call->u.arithExpr->kind != A_arithExprType::A_exprUnitKind) {
                    Error::NoViableConversion(param_call->pos, GetTypeString(right_type), "array");
                }
                if (param_call->u.arithExpr->u.exprUnit->kind == A_exprUnitType::A_idExprKind) {
                    if (!isArray(*param_call->u.arithExpr->u.exprUnit->u.id)){
                        Error::NoViableConversion(param_call->pos, GetTypeString(right_type), "array");
                    } else {
                        if (isSameType(rtType, right_type)) {
                            continue;
                        } else {
                            Error::NoViableConversion(param_call->pos, GetTypeString(right_type), GetTypeString(rtType));
                        }
                    }
                }
                if (param_call->u.arithExpr->u.exprUnit->kind == A_exprUnitType::A_memberExprKind) {
                    aA_memberExpr memberExpr = param_call->u.arithExpr->u.exprUnit->u.memberExpr;
                    if (!isArray(*memberExpr->structId->u.id, *memberExpr->memberId)){
                        Error::NoViableConversion(param_call->pos, GetTypeString(right_type), "array");
                    } else {
                        if (isSameType(rtType, right_type)) {
                            continue;
                        } else {
                            Error::NoViableConversion(param_call->pos, GetTypeString(right_type), GetTypeString(rtType));
                        }
                    }
                }
                // others will have scalar
                Error::NoViableConversion(param_call->pos, GetTypeString(right_type), "array");
            }
        } else {
            if (param_call->kind == A_rightValType::A_boolExprValKind) {
                Error::NoViableConversion(param_call->pos, "bool", GetTypeString(rtType));
            } else {
                aA_type right_type = check_ArithExpr(out, param_call->u.arithExpr, ArithExprEnv::FUNCTION_CALL)->type;
                if (!isSameType(rtType, right_type)){
                    Error::NoViableConversion(param_call->pos, GetTypeString(right_type), GetTypeString(rtType));
                }
                if (param_call->u.arithExpr->u.exprUnit->kind == A_exprUnitType::A_idExprKind) {
                    if (!isArray(*param_call->u.arithExpr->u.exprUnit->u.id)){
                        continue;
                    } else {
                        Error::NoViableConversion(param_call->pos, "array", GetTypeString(right_type));
                    }
                }
                if (param_call->u.arithExpr->u.exprUnit->kind == A_exprUnitType::A_memberExprKind) {
                    aA_memberExpr memberExpr = param_call->u.arithExpr->u.exprUnit->u.memberExpr;
                    if (!isArray(*memberExpr->structId->u.id, *memberExpr->memberId)){
                        continue;
                    } else {
                        Error::NoViableConversion(param_call->pos, "array", GetTypeString(right_type));
                    }
                }
            }
        }
    }
    return ;
}


void check_WhileStmt(std::ostream& out, aA_whileStmt ws){
    if(!ws)
        return;
    check_BoolExpr(out, ws->boolExpr, ArithExprEnv::CODEBLOCK);
    /* fill code here, take care of variable scope */
    std::vector<string> typenames;

    for(aA_codeBlockStmt s : ws->whileStmts){
        check_CodeblockStmt(out, s);
        if (s->kind == A_codeBlockStmtType::A_varDeclStmtKind){
            string name = varDecl_InsertRunTime(s->u.varDeclStmt);
            typenames.push_back(name);
        }
    }
    /* fill code here */
    for (string name: typenames){
        typeMap::const_iterator iter = runtime_token.find(name);
        if (iter == runtime_token.end()){
        } else {
            string name = iter->first;
            runtime_token.erase(iter);        
        }
    }
    return;
}


void check_CallStmt(std::ostream& out, aA_callStmt cs){
    if(!cs)
        return;
    check_FuncCall(out, cs->fnCall);
    return;
}


void check_ReturnStmt(std::ostream& out, aA_returnStmt rs){
    if(!rs)
        return;
    return;
}

aA_type check_RightValue(std::ostream& out, aA_rightVal rightVal, aA_type type, ArithExprEnv env){
    // RightValue Invariant
    aA_type rightValType = nullptr;
    if (rightVal->kind == A_rightValType::A_boolExprValKind){
        // do not support bool value
        Error::NoViableConversion(rightVal->pos, "bool", GetTypeString(type));
    } else {
        // arith expression
        aA_arithExpr arithExpr = rightVal->u.arithExpr;
        rightValType = check_ArithExpr(out, arithExpr, env)->type;
    }
    // RightValue Type Valid
    if (type && isSameType(type, rightValType)){
        return rightValType;
    }else{
        Error::NoViableConversion(rightVal->pos, GetTypeString(rightValType), GetTypeString(type));
    }
}

void check_RightValueList(std::ostream& out,std::vector<aA_rightVal> rightVals, aA_type type, ArithExprEnv env){
    // Type Valid
    for (aA_rightVal rightVal: rightVals) {
        check_RightValue(out, rightVal, type, env);
    }
}
