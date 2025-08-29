#include "/home/codeleaded/System/Static/Library/AlxCallStack.h"
#include "/home/codeleaded/System/Static/Library/AlxExternFunctions.h"
#include "/home/codeleaded/System/Static/Library/LuaLikeDefines.h"
//#include "/home/codeleaded/Hecke/C/Cmd_Scripter/src/LuaLike.h"


typedef CStr Functype;

Functype Implementation_FuncOf(Scope* s,Token* a){
    Functype n = NULL;
    
    if(a->tt==TOKEN_STRING){
        Variable* a_var = Scope_FindVariable(s,a->str);
        if(a_var){
            n = *(Functype*)Variable_Data(a_var);
        }else{
            printf("[Func_Of]: 1. Arg: Variable %s doesn't exist!\n",a->str);
        }
    }else if(a->tt==TOKEN_FUNCTIONPOINTER){
        n = a->str;
    }
    return n;
}

void Func_Destroyer(Variable* v){
    printf("Func: Destroyer!\n");
    CStr_Free(Variable_Data(v));
}
void Func_Cpyer(Variable* src,Variable* dst){
    printf("Func: Cpyer!\n");
    Functype* src_str = (Functype*)Variable_Data(src);
    Functype* dst_str = (Functype*)Variable_Data(dst);
    *dst_str = CStr_Cpy(*src_str);
}

Token Func_Func_Handler_Ass(Scope* s,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);

    printf("ASS: %s = %s\n",a->str,b->str);

    Functype ft = Implementation_FuncOf(s,b);
    
    if(a->tt==TOKEN_STRING){
        Variable* a_var = Scope_FindVariable(s,a->str);
        if(a_var){
            Variable_PrepairFor(a_var,sizeof(Functype),"func",Func_Destroyer,Func_Cpyer);
            Variable_SetTo(a_var,(Functype[]){ CStr_Cpy(ft) });
        }else{
            Scope_BuildInitVariableRange(s,a->str,"func",s->range-1,(Functype[]){ CStr_Cpy(ft) });
        }
    }else{
        printf("[Func_Ass]: 1. Arg: %s is not a variable type!\n",a->str);
        return Token_Null();
    }

    return Token_Null();
}
Token Func_Handler_Cast(Scope* s,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    //printf("CAST: %s\n",a->str);

    Variable* a_var;
    if(a->tt==TOKEN_STRING){
        a_var = Scope_FindVariable(s,a->str);
        if(!a_var){
            printf("[Func_Ass]: 1. Arg: Variable %s doesn't exist!\n",a->str);
            return Token_Null();
        }
    }else{
        printf("[Func_Ass]: 1. Arg: %s is not a Func type!\n",a->str);
        return Token_Null();
    }

    Functype* func = (Functype*)Variable_Data(a_var);
    char* resstr = NULL;
    if(func && *func) resstr = *func;
    return Token_Move(TOKEN_CONSTSTRING_DOUBLE,resstr);
}
Token Func_Handler_Destroy(Scope* s,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    printf("DESTROY: %s\n",a->str);

    Variable* a_var = Scope_FindVariable(s,a->str);
    if(a_var){
        a_var->destroy(a_var);
    }
    
    return Token_Null();
}

Variable Func_Make(Scope* sc,CStr name,Variable* args){
    Variable ret = Variable_Make(
        "Functype","func",(VariableMap[]){ VariableMap_New() },
        sizeof(VariableMap),sc->range-1,
        Scope_DestroyerOfType(sc,"func"),
        Scope_CpyerOfType(sc,"func")
    );
    return ret;
}

void Ex_Packer(ExternFunctionMap* Extern_Functions,Vector* funcs,Scope* s){//Vector<CStr>
    TypeMap_PushContained(&s->types,funcs,
        Type_New("func",sizeof(Functype),OperatorInterationMap_Make((OperatorInterater[]){
            OperatorInterater_Make((CStr[]){ NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_CAST,(Token(*)(void*,Token*,Vector*))Func_Handler_Cast),
                OperatorDefiner_New(TOKEN_INIT,NULL),
                OperatorDefiner_New(TOKEN_DESTROY,NULL),
                OPERATORDEFINER_END
            })),
            OperatorInterater_Make((CStr[]){ "func",NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_LUALIKE_ASS,(Token(*)(void*,Token*,Vector*))Func_Func_Handler_Ass),
                OPERATORDEFINER_END
            })),
            OPERATORINTERATER_END
        }),Func_Destroyer,Func_Cpyer)
    );
}
