#include "ast2llvm.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <cassert>
#include <list>

using namespace std;
using namespace LLVMIR;

static unordered_map<string, FuncType> funcReturnMap;
static unordered_map<string, StructInfo> structInfoMap;
static unordered_map<string, Name_name *> globalVarMap;
static unordered_map<string, Temp_temp *> localVarMap;
static list<L_stm *> emit_irs;

LLVMIR::L_prog *ast2llvm(aA_program p)
{
    auto defs = ast2llvmProg_first(p);
    auto funcs = ast2llvmProg_second(p);
    vector<L_func *> funcs_block;
    for (const auto &f : funcs)
    {
        funcs_block.push_back(ast2llvmFuncBlock(f));
    }
    for (auto &f : funcs_block)
    {
        ast2llvm_moveAlloca(f);
    }
    return new L_prog(defs, funcs_block);
}

int ast2llvmRightVal_first(aA_rightVal r)
{
    if (r == nullptr)
    {
        return 0;
    }
    switch (r->kind)
    {
    case A_arithExprValKind:
    {
        return ast2llvmArithExpr_first(r->u.arithExpr);
        break;
    }
    case A_boolExprValKind:
    {
        return ast2llvmBoolExpr_first(r->u.boolExpr);
        break;
    }
    default:
        break;
    }
    return 0;
}

int ast2llvmBoolExpr_first(aA_boolExpr b)
{
    switch (b->kind)
    {
    case A_boolBiOpExprKind:
    {
        return ast2llvmBoolBiOpExpr_first(b->u.boolBiOpExpr);
        break;
    }
    case A_boolUnitKind:
    {
        return ast2llvmBoolUnit_first(b->u.boolUnit);
        break;
    }
    default:
        break;
    }
    return 0;
}

int ast2llvmBoolBiOpExpr_first(aA_boolBiOpExpr b)
{
    int l = ast2llvmBoolExpr_first(b->left);
    int r = ast2llvmBoolExpr_first(b->right);
    if (b->op == A_and)
    {
        return l && r;
    }
    else
    {
        return l || r;
    }
}

int ast2llvmBoolUOpExpr_first(aA_boolUOpExpr b)
{
    if (b->op == A_not)
    {
        return !ast2llvmBoolUnit_first(b->cond);
    }
    return 0;
}

int ast2llvmBoolUnit_first(aA_boolUnit b)
{
    switch (b->kind)
    {
    case A_comOpExprKind:
    {
        return ast2llvmComOpExpr_first(b->u.comExpr);
        break;
    }
    case A_boolExprKind:
    {
        return ast2llvmBoolExpr_first(b->u.boolExpr);
        break;
    }
    case A_boolUOpExprKind:
    {
        return ast2llvmBoolUOpExpr_first(b->u.boolUOpExpr);
        break;
    }
    default:
        break;
    }
    return 0;
}

int ast2llvmComOpExpr_first(aA_comExpr c)
{
    auto l = ast2llvmExprUnit_first(c->left);
    auto r = ast2llvmExprUnit_first(c->right);
    switch (c->op)
    {
    case A_lt:
    {
        return l < r;
        break;
    }
    case A_le:
    {
        return l <= r;
        break;
    }
    case A_gt:
    {
        return l > r;
        break;
    }
    case A_ge:
    {
        return l >= r;
        break;
    }
    case A_eq:
    {
        return l == r;
        break;
    }
    case A_ne:
    {
        return l != r;
        break;
    }
    default:
        break;
    }
    return 0;
}

int ast2llvmArithBiOpExpr_first(aA_arithBiOpExpr a)
{
    auto l = ast2llvmArithExpr_first(a->left);
    auto r = ast2llvmArithExpr_first(a->right);
    switch (a->op)
    {
    case A_add:
    {
        return l + r;
        break;
    }
    case A_sub:
    {
        return l - r;
        break;
    }
    case A_mul:
    {
        return l * r;
        break;
    }
    case A_div:
    {
        return l / r;
        break;
    }
    default:
        break;
    }
    return 0;
}

int ast2llvmArithUExpr_first(aA_arithUExpr a)
{
    if (a->op == A_neg)
    {
        return -ast2llvmExprUnit_first(a->expr);
    }
    return 0;
}

int ast2llvmArithExpr_first(aA_arithExpr a)
{
    switch (a->kind)
    {
    case A_arithBiOpExprKind:
    {
        return ast2llvmArithBiOpExpr_first(a->u.arithBiOpExpr);
        break;
    }
    case A_exprUnitKind:
    {
        return ast2llvmExprUnit_first(a->u.exprUnit);
        break;
    }
    default:
        assert(0);
        break;
    }
    return 0;
}

int ast2llvmExprUnit_first(aA_exprUnit e)
{
    if (e->kind == A_numExprKind)
    {
        return e->u.num;
    }
    else if (e->kind == A_arithExprKind)
    {
        return ast2llvmArithExpr_first(e->u.arithExpr);
    }
    else if (e->kind == A_arithUExprKind)
    {
        return ast2llvmArithUExpr_first(e->u.arithUExpr);
    }
    else
    {
        assert(0);
    }
    return 0;
}

std::vector<LLVMIR::L_def *> ast2llvmProg_first(aA_program p)
{
    vector<L_def *> defs;
    defs.push_back(L_Funcdecl("getch", vector<TempDef>(), FuncType(ReturnType::INT_TYPE)));
    defs.push_back(L_Funcdecl("getint", vector<TempDef>(), FuncType(ReturnType::INT_TYPE)));
    defs.push_back(L_Funcdecl("putch", vector<TempDef>{TempDef(TempType::INT_TEMP)}, FuncType(ReturnType::VOID_TYPE)));
    defs.push_back(L_Funcdecl("putint", vector<TempDef>{TempDef(TempType::INT_TEMP)}, FuncType(ReturnType::VOID_TYPE)));
    defs.push_back(L_Funcdecl("putarray", vector<TempDef>{TempDef(TempType::INT_TEMP), TempDef(TempType::INT_PTR, -1)}, FuncType(ReturnType::VOID_TYPE)));
    defs.push_back(L_Funcdecl("_sysy_starttime", vector<TempDef>{TempDef(TempType::INT_TEMP)}, FuncType(ReturnType::VOID_TYPE)));
    defs.push_back(L_Funcdecl("_sysy_stoptime", vector<TempDef>{TempDef(TempType::INT_TEMP)}, FuncType(ReturnType::VOID_TYPE)));
    for (const auto &v : p->programElements)
    {
        switch (v->kind)
        {
        case A_programNullStmtKind:
        {
            break;
        }
        case A_programVarDeclStmtKind:
        {
            if (v->u.varDeclStmt->kind == A_varDeclKind)
            {
                auto varDecl = v->u.varDeclStmt->u.varDecl;
                if (varDecl->kind == A_varDeclScalarKind)
                {
                    auto declScalar = varDecl->u.declScalar;
                    auto id = *declScalar->id;
                    if (declScalar->type->type == A_structTypeKind)
                    {
                        globalVarMap.emplace(id,
                                             Name_newname_struct(Temp_newlabel_named(id), *declScalar->type->u.structType));
                        TempDef def(TempType::STRUCT_TEMP, 0, *declScalar->type->u.structType);
                        defs.push_back(L_Globaldef(id, def, vector<int>()));
                    }
                    else
                    {
                        globalVarMap.emplace(id,
                                             Name_newname_int(Temp_newlabel_named(id)));
                        TempDef def(TempType::INT_TEMP, 0);
                        defs.push_back(L_Globaldef(id, def, vector<int>()));
                    }
                }
                else if (varDecl->kind == A_varDeclArrayKind)
                {
                    auto declArray = varDecl->u.declArray;
                    auto id = *declArray->id;
                    if (declArray->type->type == A_structTypeKind)
                    {
                        globalVarMap.emplace(id,
                                             Name_newname_struct_ptr(Temp_newlabel_named(id), declArray->len, *declArray->type->u.structType));
                        TempDef def(TempType::STRUCT_PTR, declArray->len, *declArray->type->u.structType);
                        defs.push_back(L_Globaldef(id, def, vector<int>()));
                    }
                    else
                    {
                        globalVarMap.emplace(id,
                                             Name_newname_int_ptr(Temp_newlabel_named(id), declArray->len));
                        TempDef def(TempType::INT_PTR, declArray->len);
                        defs.push_back(L_Globaldef(id, def, vector<int>()));
                    }
                }
                else
                {
                    assert(0);
                }
            }
            else if (v->u.varDeclStmt->kind == A_varDefKind)
            {
                auto varDef = v->u.varDeclStmt->u.varDef;
                if (varDef->kind == A_varDefScalarKind)
                {
                    auto defScalar = varDef->u.defScalar;
                    auto id = *defScalar->id;
                    if (defScalar->type->type == A_structTypeKind)
                    {
                        globalVarMap.emplace(id, Name_newname_struct(Temp_newlabel_named(id), *defScalar->type->u.structType));
                        TempDef def(TempType::STRUCT_TEMP, 0, *defScalar->type->u.structType);
                        defs.push_back(L_Globaldef(id, def, vector<int>()));
                    }
                    else
                    {
                        globalVarMap.emplace(id, Name_newname_int(Temp_newlabel_named(id)));
                        TempDef def(TempType::INT_TEMP, 0);
                        vector<int> init;
                        init.push_back(ast2llvmRightVal_first(defScalar->val));
                        defs.push_back(L_Globaldef(id, def, init));
                    }
                }
                else if (varDef->kind == A_varDefArrayKind)
                {
                    auto defArray = varDef->u.defArray;
                    auto id = *defArray->id;
                    if (defArray->type->type == A_structTypeKind)
                    {
                        globalVarMap.emplace(id, Name_newname_struct_ptr(Temp_newlabel_named(id), defArray->len, *defArray->type->u.structType));
                        TempDef def(TempType::STRUCT_PTR, defArray->len, *defArray->type->u.structType);
                        defs.push_back(L_Globaldef(id, def, vector<int>()));
                    }
                    else
                    {
                        globalVarMap.emplace(id, Name_newname_int_ptr(Temp_newlabel_named(id), defArray->len));
                        TempDef def(TempType::INT_PTR, defArray->len);
                        vector<int> init;
                        for (auto &el : defArray->vals)
                        {
                            init.push_back(ast2llvmRightVal_first(el));
                        }
                        defs.push_back(L_Globaldef(id, def, init));
                    }
                }
                else
                {
                    assert(0);
                }
            }
            else
            {
                assert(0);
            }
            break;
        }
        case A_programStructDefKind:
        {
            StructInfo si;
            int off = 0;
            vector<TempDef> members;
            for (const auto &decl : v->u.structDef->varDecls)
            {
                if (decl->kind == A_varDeclScalarKind)
                {
                    if (decl->u.declScalar->type->type == A_structTypeKind)
                    {
                        TempDef def(TempType::STRUCT_TEMP, 0, *decl->u.declScalar->type->u.structType);
                        MemberInfo info(off++, def);
                        si.memberinfos.emplace(*decl->u.declScalar->id, info);
                        members.push_back(def);
                    }
                    else
                    {
                        TempDef def(TempType::INT_TEMP, 0);
                        MemberInfo info(off++, def);
                        si.memberinfos.emplace(*decl->u.declScalar->id, info);
                        members.push_back(def);
                    }
                }
                else if (decl->kind == A_varDeclArrayKind)
                {
                    if (decl->u.declArray->type->type == A_structTypeKind)
                    {
                        TempDef def(TempType::STRUCT_PTR, decl->u.declArray->len, *decl->u.declArray->type->u.structType);
                        MemberInfo info(off++, def);
                        si.memberinfos.emplace(*decl->u.declArray->id, info);
                        members.push_back(def);
                    }
                    else
                    {
                        TempDef def(TempType::INT_PTR, decl->u.declArray->len);
                        MemberInfo info(off++, def);
                        si.memberinfos.emplace(*decl->u.declArray->id, info);
                        members.push_back(def);
                    }
                }
                else
                {
                    assert(0);
                }
            }
            structInfoMap.emplace(*v->u.structDef->id, std::move(si));
            defs.push_back(L_Structdef(*v->u.structDef->id, members));
            break;
        }
        case A_programFnDeclStmtKind:
        {
            FuncType type;
            if (v->u.fnDeclStmt->fnDecl->type == nullptr)
            {
                type.type = ReturnType::VOID_TYPE;
            }
            else if (v->u.fnDeclStmt->fnDecl->type->type == A_nativeTypeKind)
            {
                type.type = ReturnType::INT_TYPE;
            }
            else if (v->u.fnDeclStmt->fnDecl->type->type == A_structTypeKind)
            {
                type.type = ReturnType::STRUCT_TYPE;
                type.structname = *v->u.fnDeclStmt->fnDecl->type->u.structType;
            }
            else
            {
                assert(0);
            }
            if (funcReturnMap.find(*v->u.fnDeclStmt->fnDecl->id) == funcReturnMap.end())
                funcReturnMap.emplace(*v->u.fnDeclStmt->fnDecl->id, std::move(type));
            vector<TempDef> args;
            for (const auto &decl : v->u.fnDeclStmt->fnDecl->paramDecl->varDecls)
            {
                if (decl->kind == A_varDeclScalarKind)
                {
                    if (decl->u.declScalar->type->type == A_structTypeKind)
                    {
                        TempDef def(TempType::STRUCT_PTR, 0, *decl->u.declScalar->type->u.structType);
                        args.push_back(def);
                    }
                    else
                    {
                        TempDef def(TempType::INT_TEMP, 0);
                        args.push_back(def);
                    }
                }
                else if (decl->kind == A_varDeclArrayKind)
                {
                    if (decl->u.declArray->type->type == A_structTypeKind)
                    {
                        TempDef def(TempType::STRUCT_PTR, -1, *decl->u.declArray->type->u.structType);
                        args.push_back(def);
                    }
                    else
                    {
                        TempDef def(TempType::INT_PTR, -1);
                        args.push_back(def);
                    }
                }
                else
                {
                    assert(0);
                }
            }
            defs.push_back(L_Funcdecl(*v->u.fnDeclStmt->fnDecl->id, args, type));
            break;
        }
        case A_programFnDefKind:
        {
            if (funcReturnMap.find(*v->u.fnDef->fnDecl->id) == funcReturnMap.end())
            {
                FuncType type;
                if (v->u.fnDef->fnDecl->type == nullptr)
                {
                    type.type = ReturnType::VOID_TYPE;
                }
                else if (v->u.fnDef->fnDecl->type->type == A_nativeTypeKind)
                {
                    type.type = ReturnType::INT_TYPE;
                }
                else if (v->u.fnDef->fnDecl->type->type == A_structTypeKind)
                {
                    type.type = ReturnType::STRUCT_TYPE;
                    type.structname = *v->u.fnDef->fnDecl->type->u.structType;
                }
                else
                {
                    assert(0);
                }
                funcReturnMap.emplace(*v->u.fnDef->fnDecl->id, std::move(type));
            }
            break;
        }
        default:
            assert(0);
            break;
        }
    }
    return defs;
}

std::vector<Func_local *> ast2llvmProg_second(aA_program p)
{
    vector<Func_local *> funcs;
    for (const auto &v : p->programElements)
    {
        switch (v->kind)
        {
        case A_programNullStmtKind:
        {
            break;
        }
        case A_programVarDeclStmtKind:
        {
            break;
        }
        case A_programStructDefKind:
        {
            break;
        }
        case A_programFnDeclStmtKind:
        {
            break;
        }
        case A_programFnDefKind:
        {
            funcs.push_back(ast2llvmFunc(v->u.fnDef));
            break;
        }
        default:
            assert(0);
            break;
        }
    }
    return funcs;
}

Func_local *ast2llvmFunc(aA_fnDef f)
{
    auto name = *f->fnDecl->id;
    auto ret = funcReturnMap[name];
    std::vector<Temp_temp *> args;
    localVarMap.clear();
    emit_irs.clear();

    // function start label
    auto start_label = Temp_newlabel();
    emit_irs.push_back(L_Label(start_label));

    // function params
    for (const auto &decl : f->fnDecl->paramDecl->varDecls)
    {
        if (decl->kind == A_varDeclScalarKind)
        {
            auto declScalar = decl->u.declScalar;
            auto id = *declScalar->id;
            auto type = declScalar->type;
            if (type->type == A_structTypeKind)
            {
                // struct param is passed by pointer
                auto temp = Temp_newtemp_struct_ptr(0, *type->u.structType);
                args.push_back(temp);
                localVarMap[id] = temp;
            }
            else
            {
                auto temp = Temp_newtemp_int();
                args.push_back(temp);
                auto stackPtr = Temp_newtemp_int_ptr(0);
                localVarMap[id] = stackPtr;

                // irs: alloc
                emit_irs.push_back(L_Alloca(AS_Operand_Temp(stackPtr)));

                // irs: store
                emit_irs.push_back(L_Store(AS_Operand_Temp(temp), AS_Operand_Temp(stackPtr)));
            }
        }
        else if (decl->kind == A_varDeclArrayKind)
        {
            auto declArray = decl->u.declArray;
            auto id = *declArray->id;
            auto type = declArray->type;
            auto len = declArray->len;
            if (type->type == A_structTypeKind)
            {
                auto temp = Temp_newtemp_struct_ptr(len, *type->u.structType);
                args.push_back(temp);
                localVarMap[id] = temp;
            }
            else
            {
                auto temp = Temp_newtemp_int_ptr(len);
                args.push_back(temp);
                localVarMap[id] = temp;
            }
        }
        else
        {
            assert(0);
        }
    }

    aA_codeBlockStmt currentStmt = nullptr;

    // function body
    for (const auto &s : f->stmts)
    {
        currentStmt = s;
        ast2llvmBlock(s);

        // ignore if meet ret, break, continue
        if (s->kind == A_codeBlockStmtType::A_returnStmtKind ||
            s->kind == A_codeBlockStmtType::A_breakStmtKind ||
            s->kind == A_codeBlockStmtType::A_continueStmtKind)
        {
            break;
        }
    }

    if (currentStmt == nullptr || currentStmt->kind != A_codeBlockStmtType::A_returnStmtKind)
    {
        if (ret.type == ReturnType::INT_TYPE)
        {
            emit_irs.push_back(L_Ret(AS_Operand_Const(0)));
        }
        else if (ret.type == ReturnType::VOID_TYPE)
        {
            emit_irs.push_back(L_Ret(nullptr));
        }
        else
        {
            assert(0);
        }
    }

    return new Func_local(name, ret, args, emit_irs);
}

void ast2llvmBlock(aA_codeBlockStmt b, Temp_label *con_label, Temp_label *bre_label)
{
    switch (b->kind)
    {

    case A_codeBlockStmtType::A_nullStmtKind:
    {
    }
    break;

    case A_codeBlockStmtType::A_varDeclStmtKind:
    {
        auto varDeclStmt = b->u.varDeclStmt;
        if (varDeclStmt->kind == A_varDeclStmtType::A_varDeclKind)
        {
            auto varDecl = varDeclStmt->u.varDecl;

            switch (varDecl->kind)
            {
            case A_varDeclType::A_varDeclScalarKind:
            {
                auto declScalar = varDecl->u.declScalar;
                auto id = *declScalar->id;
                auto type = declScalar->type;
                switch (type->type)
                {
                case A_dataType::A_structTypeKind:
                {
                    // case: let a: MyStruct;
                    auto temp = Temp_newtemp_struct_ptr(0, *type->u.structType);
                    localVarMap[id] = temp;

                    // ir: alloc
                    emit_irs.push_back(L_Alloca(AS_Operand_Temp(temp)));
                }
                break;

                case A_dataType::A_nativeTypeKind:
                {
                    // case: let a: int;
                    auto temp = Temp_newtemp_int_ptr(0);
                    localVarMap[id] = temp;

                    // ir: alloc
                    emit_irs.push_back(L_Alloca(AS_Operand_Temp(temp)));
                }
                break;

                default:
                {
                    assert(0);
                }
                break;
                }
            }
            break;

            case A_varDeclType::A_varDeclArrayKind:
            {
                auto declArray = varDecl->u.declArray;
                auto id = *declArray->id;
                auto type = declArray->type;
                auto len = declArray->len;
                switch (type->type)
                {
                case A_dataType::A_structTypeKind:
                {
                    // case: let a[10]: MyStruct
                    auto temp = Temp_newtemp_struct_ptr(len, *type->u.structType);
                    localVarMap[id] = temp;

                    // ir: alloc
                    emit_irs.push_back(L_Alloca(AS_Operand_Temp(temp)));
                }
                break;

                case A_dataType::A_nativeTypeKind:
                {
                    // case: let a[10]: int
                    auto temp = Temp_newtemp_int_ptr(len);
                    localVarMap[id] = temp;

                    // ir: alloc
                    emit_irs.push_back(L_Alloca(AS_Operand_Temp(temp)));
                }
                break;

                default:
                {
                    assert(0);
                }
                break;
                }
            }
            break;

            default:
            {
                assert(0);
            }
            break;
            }
        }
        else if (varDeclStmt->kind == A_varDeclStmtType::A_varDefKind)
        {
            auto varDef = varDeclStmt->u.varDef;
            switch (varDef->kind)
            {

            case A_varDefType::A_varDefScalarKind:
            {
                auto defScalar = varDef->u.defScalar;
                auto id = *defScalar->id;
                auto type = defScalar->type;
                switch (type->type)
                {
                case A_dataType::A_nativeTypeKind:
                {
                    // case: let a: int = 1;
                    auto stackPtr = Temp_newtemp_int_ptr(0);
                    localVarMap[id] = stackPtr;

                    // ir: alloc
                    emit_irs.push_back(L_Alloca(AS_Operand_Temp(stackPtr)));

                    // ir: store
                    emit_irs.push_back(L_Store(ast2llvmRightVal(defScalar->val), AS_Operand_Temp(stackPtr)));
                }
                break;

                default:
                {
                    assert(0);
                }
                break;
                }
            }
            break;

            case A_varDefType::A_varDefArrayKind:
            {
                auto defArray = varDef->u.defArray;
                auto id = *defArray->id;
                auto type = defArray->type;
                auto len = defArray->len;

                switch (type->type)
                {
                case A_dataType::A_nativeTypeKind:
                {
                    // case: let a[3]: int = {1, 2, 3};
                    auto temp = Temp_newtemp_int_ptr(len);
                    localVarMap[id] = temp;

                    // ir: alloc
                    emit_irs.push_back(L_Alloca(AS_Operand_Temp(temp)));

                    // ir: store
                    for (int i = 0; i < defArray->vals.size(); i++)
                    {
                        // ir: get element ptr
                        auto temp_i = AS_Operand_Temp(Temp_newtemp_int_ptr(0));
                        emit_irs.push_back(L_Gep(temp_i, AS_Operand_Temp(temp), AS_Operand_Const(i)));

                        // ir: store
                        emit_irs.push_back(L_Store(ast2llvmRightVal(defArray->vals[i]), temp_i));
                    }
                }
                break;

                default:
                {
                    assert(0);
                }
                break;
                }
            }
            break;

            default:
            {
                assert(0);
            }
            break;
            }
        }
        else
        {
            assert(0);
        }
    }
    break;

    case A_codeBlockStmtType::A_assignStmtKind:
    {
        auto assignStmt = b->u.assignStmt;
        auto leftPtr = ast2llvmLeftVal(assignStmt->leftVal);
        auto right = ast2llvmRightVal(assignStmt->rightVal);

        emit_irs.push_back(L_Store(right, leftPtr));
    }
    break;

    case A_codeBlockStmtType::A_callStmtKind:
    {
        auto callStmt = b->u.callStmt;
        std::vector<AS_operand *> args;

        for (auto &arg : callStmt->fnCall->vals)
        {
            args.push_back(ast2llvmRightVal(arg));
        }

        emit_irs.push_back(L_Voidcall(*callStmt->fnCall->fn, args));
    }
    break;

    case A_codeBlockStmtType::A_ifStmtKind:
    {
        auto ifStmt = b->u.ifStmt;
        auto has_else = !ifStmt->elseStmts.empty();
        auto ifTrueLabel = Temp_newlabel();
        auto ifFalseLabel = Temp_newlabel();
        auto ifEndLabel = has_else ? Temp_newlabel() : ifFalseLabel;

        // irs: cond
        ast2llvmBoolExpr(ifStmt->boolExpr, ifTrueLabel, ifFalseLabel);

        // ir: if stmts
        emit_irs.push_back(L_Label(ifTrueLabel));
        auto find_jump = false;
        for (auto &s : ifStmt->ifStmts)
        {
            ast2llvmBlock(s, con_label, bre_label);

            // ignore if meet ret, break, continue
            if (s->kind == A_codeBlockStmtType::A_returnStmtKind ||
                s->kind == A_codeBlockStmtType::A_breakStmtKind ||
                s->kind == A_codeBlockStmtType::A_continueStmtKind)
            {
                find_jump = true;
                break;
            }
        }
        if (!find_jump)
        {
            emit_irs.push_back(L_Jump(ifEndLabel));
        }

        if (has_else)
        {
            // ir: else stmts
            emit_irs.push_back(L_Label(ifFalseLabel));
            find_jump = false;
            for (auto &s : ifStmt->elseStmts)
            {
                ast2llvmBlock(s, con_label, bre_label);

                // ignore if meet ret, break, continue
                if (s->kind == A_codeBlockStmtType::A_returnStmtKind ||
                    s->kind == A_codeBlockStmtType::A_breakStmtKind ||
                    s->kind == A_codeBlockStmtType::A_continueStmtKind)
                {
                    find_jump = true;
                    break;
                }
            }
            if (!find_jump)
            {
                emit_irs.push_back(L_Jump(ifEndLabel));
            }
        }

        // ir: end
        emit_irs.push_back(L_Label(ifEndLabel));
    }
    break;

    case A_codeBlockStmtType::A_whileStmtKind:
    {
        auto whileStmt = b->u.whileStmt;
        auto whileTestLabel = Temp_newlabel();
        auto whileTrueLabel = Temp_newlabel();
        auto whileFalseLabel = Temp_newlabel();

        emit_irs.push_back(L_Jump(whileTestLabel));

        // ir: cond
        emit_irs.push_back(L_Label(whileTestLabel));
        ast2llvmBoolExpr(whileStmt->boolExpr, whileTrueLabel, whileFalseLabel);

        // ir: body
        emit_irs.push_back(L_Label(whileTrueLabel));
        auto find_jump = false;
        for (auto &s : whileStmt->whileStmts)
        {
            ast2llvmBlock(s, whileTestLabel, whileFalseLabel);

            // ignore if meet ret, break, continue
            if (s->kind == A_codeBlockStmtType::A_returnStmtKind ||
                s->kind == A_codeBlockStmtType::A_breakStmtKind ||
                s->kind == A_codeBlockStmtType::A_continueStmtKind)
            {
                find_jump = true;
                break;
            }
        }
        if (!find_jump)
        {
            emit_irs.push_back(L_Jump(whileTestLabel));
        }

        // ir: end
        emit_irs.push_back(L_Label(whileFalseLabel));
    }
    break;

    case A_codeBlockStmtType::A_returnStmtKind:
    {
        auto returnStmt = b->u.returnStmt;
        if (returnStmt->retVal != nullptr)
        {
            auto retVal = ast2llvmRightVal(returnStmt->retVal);
            emit_irs.push_back(L_Ret(retVal));
        }
        else
        {
            emit_irs.push_back(L_Ret(nullptr));
        }
    }
    break;

    case A_codeBlockStmtType::A_continueStmtKind:
    {
        assert(con_label != nullptr);

        emit_irs.push_back(L_Jump(con_label));
    }
    break;

    case A_codeBlockStmtType::A_breakStmtKind:
    {
        assert(bre_label != nullptr);

        emit_irs.push_back(L_Jump(bre_label));
    }
    break;

    default:
    {
        assert(0);
    }
    break;
    }
}

AS_operand *ast2llvmRightVal(aA_rightVal r)
{
    switch (r->kind)
    {
    case A_rightValType::A_arithExprValKind:
    {
        return ast2llvmArithExpr(r->u.arithExpr);
    }
    break;

    case A_rightValType::A_boolExprValKind:
    {
        return ast2llvmBoolExpr(r->u.boolExpr);
    }
    break;

    default:
    {
        assert(0);
    }
    break;
    }

    return nullptr;
}

AS_operand *ast2llvmLeftVal(aA_leftVal l)
{
    switch (l->kind)
    {
    case A_leftValType::A_varValKind:
    {
        // case: a
        auto id = *l->u.id;
        if (localVarMap.find(id) != localVarMap.end())
        {
            return AS_Operand_Temp(localVarMap[id]);
        }
        else if (globalVarMap.find(id) != globalVarMap.end())
        {
            return AS_Operand_Name(globalVarMap[id]);
        }
        else
        {
            assert(0);
        }
    }
    break;

    case A_leftValType::A_arrValKind:
    {
        // case: a[1], a[i], a.b[1]
        auto arrExpr = l->u.arrExpr;
        auto basePtr = ast2llvmLeftVal(arrExpr->arr);
        auto index = ast2llvmIndexExpr(arrExpr->idx);

        // get element ptr
        Temp_temp *temp = nullptr;
        switch (basePtr->kind)
        {
        case OperandKind::TEMP:
        {
            auto b_temp = basePtr->u.TEMP;
            switch (b_temp->type)
            {

            case TempType::INT_PTR:
            {
                temp = Temp_newtemp_int_ptr(b_temp->len);
            }
            break;

            case TempType::STRUCT_PTR:
            {
                temp = Temp_newtemp_struct_ptr(b_temp->len, b_temp->structname);
            }
            break;

            default:
            {
                assert(0);
            }
            break;
            }
        }
        break;

        case OperandKind::NAME:
        {
            auto name = basePtr->u.NAME;
            switch (name->type)
            {

            case TempType::INT_PTR:
            {
                temp = Temp_newtemp_int_ptr(name->len);
            }
            break;

            case TempType::STRUCT_PTR:
            {
                temp = Temp_newtemp_struct_ptr(name->len, name->structname);
            }
            break;

            default:
            {
                assert(0);
            }
            break;
            }
        }
        break;

        default:
        {
            assert(0);
        }
        break;
        }

        emit_irs.push_back(L_Gep(AS_Operand_Temp(temp), basePtr, index));

        return AS_Operand_Temp(temp);
    }
    break;

    case A_leftValType::A_memberValKind:
    {
        // case: a.b, a[1].b
        auto memberExpr = l->u.memberExpr;
        auto basePtr = ast2llvmLeftVal(memberExpr->structId);
        auto memberId = *memberExpr->memberId;

        // get struct name
        string structName;

        switch (basePtr->kind)
        {
        case OperandKind::TEMP:
        {
            auto b_temp = basePtr->u.TEMP;
            if (b_temp->type != TempType::STRUCT_PTR)
            {
                assert(0);
            }

            structName = b_temp->structname;
        }
        break;

        case OperandKind::NAME:
        {
            auto name = basePtr->u.NAME;
            if (name->type != TempType::STRUCT_TEMP)
            {
                assert(0);
            }

            structName = name->structname;
        }
        break;

        default:
        {
            assert(0);
        }
        break;
        }

        auto memberInfo = structInfoMap[structName].memberinfos[memberId];

        Temp_temp *temp = nullptr;
        switch (memberInfo.def.kind)
        {
        case TempType::INT_TEMP:
        {
            temp = Temp_newtemp_int_ptr(0);
        }
        break;

        case TempType::INT_PTR:
        {
            temp = Temp_newtemp_int_ptr(memberInfo.def.len);
        }
        break;

        case TempType::STRUCT_TEMP:
        {
            temp = Temp_newtemp_struct_ptr(0, memberInfo.def.structname);
        }
        break;

        case TempType::STRUCT_PTR:
        {
            temp = Temp_newtemp_struct_ptr(memberInfo.def.len, memberInfo.def.structname);
        }
        break;

        default:
        {
            assert(0);
        }
        break;
        }

        emit_irs.push_back(L_Gep(AS_Operand_Temp(temp), basePtr, AS_Operand_Const(memberInfo.offset)));

        return AS_Operand_Temp(temp);
    }
    break;

    default:
    {
        assert(0);
    }
    break;
    }

    return nullptr;
}

AS_operand *ast2llvmIndexExpr(aA_indexExpr index)
{
    switch (index->kind)
    {
    case A_indexExprKind::A_numIndexKind:
    {
        return AS_Operand_Const(index->u.num);
    }
    break;

    case A_indexExprKind::A_idIndexKind:
    {
        auto id = *index->u.id;
        if (localVarMap.find(id) != localVarMap.end())
        {
            auto id_temp = localVarMap.at(id);
            assert(id_temp->type == TempType::INT_PTR && id_temp->len == 0);

            auto temp = Temp_newtemp_int();
            emit_irs.push_back(L_Load(AS_Operand_Temp(temp), AS_Operand_Temp(localVarMap[id])));
            return AS_Operand_Temp(temp);
        }
        else if (globalVarMap.find(id) != globalVarMap.end())
        {
            // case: a[i]
            // global variables should be loaded

            auto id_name = globalVarMap.at(id);

            assert(id_name->type == TempType::INT_TEMP);

            auto temp = Temp_newtemp_int();
            emit_irs.push_back(L_Load(AS_Operand_Temp(temp), AS_Operand_Name(id_name)));
            return AS_Operand_Temp(temp);
        }
        else
        {
            assert(0);
        }
    }
    break;

    default:
    {
        assert(0);
    }
    break;
    }
}

AS_operand *ast2llvmBoolExpr(aA_boolExpr b, Temp_label *true_label, Temp_label *false_label)
{
    bool should_return = false;
    AS_operand *stackPtr = nullptr;
    if (true_label == nullptr || false_label == nullptr)
    {
        true_label = Temp_newlabel();
        false_label = Temp_newlabel();
        should_return = true;
        stackPtr = AS_Operand_Temp(Temp_newtemp_int_ptr(0));

        // ir: alloca
        emit_irs.push_back(L_Alloca(stackPtr));
    }

    switch (b->kind)
    {
    case A_boolExprType::A_boolBiOpExprKind:
    {
        ast2llvmBoolBiOpExpr(b->u.boolBiOpExpr, true_label, false_label);
    }
    break;

    case A_boolExprType::A_boolUnitKind:
    {
        ast2llvmBoolUnit(b->u.boolUnit, true_label, false_label);
    }
    break;

    default:
    {
        assert(0);
    }
    break;
    }

    if (should_return)
    {
        auto temp = AS_Operand_Temp(Temp_newtemp_int());
        auto end_label = Temp_newlabel();

        // ir: move true
        emit_irs.push_back(L_Label(true_label));
        emit_irs.push_back(L_Store(AS_Operand_Const(1), stackPtr));
        emit_irs.push_back(L_Jump(end_label));

        // ir: move false
        emit_irs.push_back(L_Label(false_label));
        emit_irs.push_back(L_Store(AS_Operand_Const(0), stackPtr));
        emit_irs.push_back(L_Jump(end_label));

        // ir: end
        emit_irs.push_back(L_Label(end_label));
        emit_irs.push_back(L_Load(temp, stackPtr));

        return temp;
    }

    return nullptr;
}

void ast2llvmBoolBiOpExpr(aA_boolBiOpExpr b, Temp_label *true_label, Temp_label *false_label)
{
    assert(true_label != nullptr);
    assert(false_label != nullptr);

    switch (b->op)
    {
    case A_boolBiOp::A_and:
    {
        auto andTrueLabel = Temp_newlabel();
        ast2llvmBoolExpr(b->left, andTrueLabel, false_label);
        emit_irs.push_back(L_Label(andTrueLabel));
        ast2llvmBoolExpr(b->right, true_label, false_label);
    }
    break;

    case A_boolBiOp::A_or:
    {
        auto orFalseLabel = Temp_newlabel();
        ast2llvmBoolExpr(b->left, true_label, orFalseLabel);
        emit_irs.push_back(L_Label(orFalseLabel));
        ast2llvmBoolExpr(b->right, true_label, false_label);
    }
    break;

    default:
    {
        assert(0);
    }
    break;
    }
}

void ast2llvmBoolUnit(aA_boolUnit b, Temp_label *true_label, Temp_label *false_label)
{

    assert(true_label != nullptr);
    assert(false_label != nullptr);

    if (b->kind == A_boolUnitType::A_boolUOpExprKind){
        return ast2llvmBoolUnit(b->u.boolUOpExpr->cond, true_label, false_label);
    }
    if (b->kind == A_boolUnitType::A_boolExprKind){
        ast2llvmBoolExpr(b->u.boolExpr, true_label, false_label);
        return;
    }
    if (b->kind == A_boolUnitType::A_comOpExprKind){
        return ast2llvmComOpExpr(b->u.comExpr, true_label, false_label);
    }
}

void ast2llvmComOpExpr(aA_comExpr c, Temp_label *true_label, Temp_label *false_label)
{
    assert(true_label != nullptr);
    assert(false_label != nullptr);

    aA_exprUnit left = c->left, right = c->right;
    AS_operand* left_operand = ast2llvmExprUnit(left);
    AS_operand* right_operand = ast2llvmExprUnit(right);
    AS_operand* ans_operand = AS_Operand_Temp(Temp_newtemp_int());
    switch (c->op)
    {
    case A_comOp::A_eq:
        emit_irs.push_back(L_Cmp(L_relopKind::T_eq, left_operand, right_operand, ans_operand));
        break;
    
    case A_comOp::A_ge:
        emit_irs.push_back(L_Cmp(L_relopKind::T_ge, left_operand, right_operand, ans_operand));
        break;

    case A_comOp::A_gt:
        emit_irs.push_back(L_Cmp(L_relopKind::T_gt, left_operand, right_operand, ans_operand));
        break;

    case A_comOp::A_le:
        emit_irs.push_back(L_Cmp(L_relopKind::T_le, left_operand, right_operand, ans_operand));
        break;

    case A_comOp::A_lt:
        emit_irs.push_back(L_Cmp(L_relopKind::T_lt, left_operand, right_operand, ans_operand));
        break;

    case A_comOp::A_ne:
        emit_irs.push_back(L_Cmp(L_relopKind::T_ne, left_operand, right_operand, ans_operand));
        break;

    default:
        break;
    }

    emit_irs.push_back(L_Cjump(ans_operand, true_label, false_label));
}

AS_operand *ast2llvmArithBiOpExpr(aA_arithBiOpExpr a)
{
    aA_arithExpr left = a->left, right = a->right;
    
    AS_operand *left_operand = ast2llvmArithExpr(left), *right_operand = ast2llvmArithExpr(right);
    AS_operand* new_temp_operand = AS_Operand_Temp(Temp_newtemp_int());
    
    L_binopKind op;
    switch (a->op)
    {
    case A_arithBiOp::A_add:
        op = L_binopKind::T_plus;
        break;
    case A_arithBiOp::A_sub:
        op = L_binopKind::T_minus;
        break;
    case A_arithBiOp::A_mul:
        op = L_binopKind::T_mul;
        break;
    case A_arithBiOp::A_div:
        op = L_binopKind::T_div;
        break;
    default:
        assert(0);
        break;
    }
    emit_irs.push_back(L_Binop(op, left_operand, right_operand, new_temp_operand));
    return new_temp_operand;
}

AS_operand *ast2llvmArithUExpr(aA_arithUExpr a)
{
    auto expr = ast2llvmExprUnit(a->expr);
    auto dst = AS_Operand_Temp(Temp_newtemp_int());
    emit_irs.push_back(L_Binop(L_binopKind::T_minus, AS_Operand_Const(0), expr, dst));
    return dst;
}

AS_operand *ast2llvmArithExpr(aA_arithExpr a)
{
    if (a->kind == A_arithExprType::A_arithBiOpExprKind){
        return ast2llvmArithBiOpExpr(a->u.arithBiOpExpr);
    }
    else if (a->kind == A_arithExprType::A_exprUnitKind){
        return ast2llvmExprUnit(a->u.exprUnit);
    }
    assert(0);
}

AS_operand *ast2llvmExprUnit(aA_exprUnit e)
{
    switch (e->kind)
    {
    case A_exprUnitType::A_numExprKind:
    {
        return AS_Operand_Const(e->u.num);
    }
    break;

    case A_exprUnitType::A_idExprKind:
    {
        auto id = *e->u.id;
        if (localVarMap.find(id) != localVarMap.end())
        {
            auto id_temp = localVarMap.at(id);

            switch (id_temp->type)
            {
            case TempType::INT_PTR:
            {
                if (id_temp->len == 0)
                {
                    auto temp = Temp_newtemp_int();
                    emit_irs.push_back(L_Load(AS_Operand_Temp(temp), AS_Operand_Temp(id_temp)));
                    return AS_Operand_Temp(temp);
                }
                else
                {
                    return AS_Operand_Temp(id_temp);
                }
            }
            break;

            case TempType::STRUCT_PTR:
            {
                return AS_Operand_Temp(id_temp);
            }
            break;

            default:
            {
                assert(0);
            }
            break;
            }
        }
        else if (globalVarMap.find(id) != globalVarMap.end())
        {
            auto id_name = globalVarMap.at(id);

            switch (id_name->type)
            {
            case TempType::INT_TEMP:
            {
                auto temp = Temp_newtemp_int();
                emit_irs.push_back(L_Load(AS_Operand_Temp(temp), AS_Operand_Name(id_name)));
                return AS_Operand_Temp(temp);
            }
            break;

            case TempType::INT_PTR:
            case TempType::STRUCT_PTR:
            case TempType::STRUCT_TEMP:
            {
                return AS_Operand_Name(id_name);
            }
            break;

            default:
            {
                assert(0);
            }
            break;
            }
        }
        else
        {
            assert(0);
        }
    }
    break;

    case A_exprUnitType::A_arithExprKind:
    {
        return ast2llvmArithExpr(e->u.arithExpr);
    }
    break;

    case A_exprUnitType::A_fnCallKind:
    {
        auto callExpr = e->u.callExpr;
        std::vector<AS_operand *> args;
        for (auto &arg : callExpr->vals)
        {
            args.push_back(ast2llvmRightVal(arg));
        }
        auto res = Temp_newtemp_int();
        emit_irs.push_back(L_Call(*callExpr->fn, AS_Operand_Temp(res), args));
        return AS_Operand_Temp(res);
    }
    break;

    case A_exprUnitType::A_arrayExprKind:
    {
        auto arrExpr = e->u.arrayExpr;
        auto basePtr = ast2llvmLeftVal(arrExpr->arr);
        auto index = ast2llvmIndexExpr(arrExpr->idx);

        // check: right val array expr base must be int ptr
        switch (basePtr->kind)
        {
        case OperandKind::TEMP:
        {
            auto b_temp = basePtr->u.TEMP;
            if (b_temp->type != TempType::INT_PTR)
            {
                assert(0);
            }
        }
        break;

        case OperandKind::NAME:
        {
            auto name = basePtr->u.NAME;
            if (name->type != TempType::INT_PTR)
            {
                assert(0);
            }
        }
        break;

        default:
        {
            assert(0);
        }
        break;
        }

        // ir: get element ptr
        auto resPtr = AS_Operand_Temp(Temp_newtemp_int_ptr(0));
        emit_irs.push_back(L_Gep(resPtr, basePtr, index));

        // ir: load
        auto res = AS_Operand_Temp(Temp_newtemp_int());
        emit_irs.push_back(L_Load(res, resPtr));

        return res;
    }
    break;

    case A_exprUnitType::A_memberExprKind:
    {
        auto memberExpr = e->u.memberExpr;
        auto basePtr = ast2llvmLeftVal(memberExpr->structId);
        auto memberId = *memberExpr->memberId;
        string structname;

        // check:
        // basePtr is temp -> struct ptr
        // basePtr is name -> struct temp
        switch (basePtr->kind)
        {
        case OperandKind::TEMP:
        {
            auto b_temp = basePtr->u.TEMP;

            if (b_temp->type != TempType::STRUCT_PTR)
            {
                assert(0);
            }

            structname = b_temp->structname;
        }
        break;

        case OperandKind::NAME:
        {
            auto name = basePtr->u.NAME;
            if (name->type != TempType::STRUCT_TEMP)
            {
                assert(0);
            }

            structname = name->structname;
        }
        break;

        default:
        {
            assert(0);
        }
        break;
        }

        auto memberInfo = structInfoMap[structname].memberinfos[memberId];

        // check: member must be int temp
        assert(memberInfo.def.kind == TempType::INT_TEMP);

        // ir: get element ptr
        auto resPtr = AS_Operand_Temp(Temp_newtemp_int_ptr(0));
        emit_irs.push_back(L_Gep(resPtr, basePtr, AS_Operand_Const(memberInfo.offset)));

        // ir: load
        auto res = AS_Operand_Temp(Temp_newtemp_int());
        emit_irs.push_back(L_Load(res, resPtr));

        return res;
    }
    break;

    case A_exprUnitType::A_arithUExprKind:
    {
        return ast2llvmArithUExpr(e->u.arithUExpr);
    }

    default:
    {
        assert(0);
    }
    break;
    }

    return nullptr;
}

LLVMIR::L_func *ast2llvmFuncBlock(Func_local *f)
{
    std::list<L_stm *> instrs;
    std::list<L_block *> blocks;

    for (const auto &i : f->irs)
    {
        if (i->type == L_StmKind::T_LABEL)
        {
            if (!instrs.empty())
            {
                blocks.push_back(L_Block(instrs));
                instrs.clear();
            }
        }

        instrs.push_back(i);
    }

    if (!instrs.empty())
    {
        blocks.push_back(L_Block(instrs));
    }

    return new LLVMIR::L_func(f->name, f->ret, f->args, blocks);
}

void ast2llvm_moveAlloca(LLVMIR::L_func *f)
{
    auto first_block = f->blocks.front();
    for (auto i = ++f->blocks.begin(); i != f->blocks.end(); ++i)
    {
        for (auto it = (*i)->instrs.begin(); it != (*i)->instrs.end();)
        {
            if ((*it)->type == L_StmKind::T_ALLOCA)
            {
                first_block->instrs.insert(++first_block->instrs.begin(), *it);
                it = (*i)->instrs.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}