#include "/home/codeleaded/System/Static/Library/AlxCallStack.h"
#include "/home/codeleaded/System/Static/Library/AlxExternFunctions.h"
#include "/home/codeleaded/System/Static/Library/IRRuntime.h"

void Str_Destroyer(Variable* v){
    CStr* str = (CStr*)Variable_Data(v);
    CStr_Free(str);
}
void Str_Cpyer(Variable* src,Variable* dst){
    CStr* src_str = (CStr*)Variable_Data(src);
    CStr* dst_str = (CStr*)Variable_Data(dst);
    *dst_str = CStr_Cpy(*src_str);
}

Token Str_Str_Handler_Ass(IR* ir,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);

    IR_InfoHandler(ir,"ass str %s %s",a->str,b->str);

    CStr n2 = IR_Impl_Str(ir,b);
    if(a->tt==TOKEN_STRING){
        Variable* a_var = VariableMap_Find(&ir->vars,a->str);
        if(a_var){
            Variable_PrepairFor(a_var,sizeof(CStr),"str",NULL,NULL);
            Variable_SetTo(a_var,(CStr[]){ CStr_Cpy(n2) });
        }else{
            IR_ErrorHandler(ir,"Ass: variable %s doesn't exist!",a->str);
        }
    }else{
        IR_ErrorHandler(ir,"Ass: arg1 should be a variable, but is %s!",a->str);
    }

    CStr res = CStr_Cpy(n2);
    return Token_Move(TOKEN_CONSTSTRING_DOUBLE,res);
}

Token Str_Str_Handler_Add2(IR* ir,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);

    IR_InfoHandler(ir,"add str %s %s",a->str,b->str);

    CStr n1 = IR_Impl_Str(ir,a);
    CStr n2 = IR_Impl_Str(ir,b);
    CStr res = CStr_Concat(n1,n2);
    if(a->tt==TOKEN_STRING){
        Variable* a_var = VariableMap_Find(&ir->vars,a->str);
        if(a_var){
            Variable_PrepairFor(a_var,sizeof(CStr),"str",NULL,NULL);
            Variable_SetTo(a_var,(CStr[]){ CStr_Cpy(res) });
        }else{
            IR_ErrorHandler(ir,"Add: variable %s doesn't exist!",a->str);
        }
    }else{
        IR_ErrorHandler(ir,"Add: arg1 should be a variable, but is %s!",a->str);
    }

    return Token_Move(TOKEN_CONSTSTRING_DOUBLE,res);
}
Token Str_Str_Handler_Add3(IR* ir,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);
    Token* c = (Token*)Vector_Get(args,2);

    IR_InfoHandler(ir,"add str %s %s %s",a->str,b->str,c->str);

    CStr n2 = IR_Impl_Str(ir,b);
    CStr n3 = IR_Impl_Str(ir,c);
    CStr res = CStr_Concat(n2,n3);
    if(a->tt==TOKEN_STRING){
        Variable* a_var = VariableMap_Find(&ir->vars,a->str);
        if(a_var){
            Variable_PrepairFor(a_var,sizeof(CStr),"str",NULL,NULL);
            Variable_SetTo(a_var,(CStr[]){ CStr_Cpy(res) });
        }else{
            IR_ErrorHandler(ir,"Add: variable %s doesn't exist!",a->str);
        }
    }else{
        IR_ErrorHandler(ir,"Add: arg1 should be a variable, but is %s!",a->str);
    }

    return Token_Move(TOKEN_CONSTSTRING_DOUBLE,res);
}
Token Str_Str_Handler_Equ(IR* ir,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);

    CStr n1 = IR_Impl_Str(ir,a);
    CStr n2 = IR_Impl_Str(ir,b);
    Boolean res = CStr_Cmp(n1,n2);

    char* resstr = Boolean_Get(res);
    return Token_Move(TOKEN_BOOL,resstr);
}

Token Str_Handler_Cast(IR* ir,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    CStr n1 = IR_Impl_Str(ir,a);
    
    CStr res = n1;
    char* resstr = CStr_Cpy(res);
    return Token_Move(TOKEN_CONSTSTRING_DOUBLE,resstr);
}
Token Str_Handler_Destroy(IR* ir,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    Variable* a_var = VariableMap_Find(&ir->vars,a->str);
    if(a_var){
        a_var->destroy(a_var);
    }
    
    return Token_Null();
}

void Ex_Packer(ExternFunctionMap* Extern_Functions,Vector* funcs,IR* ir){//Vector<CStr>
    TypeMap_PushContained(&ir->types,funcs,
        Type_New("str",sizeof(CStr),OperatorInterationMap_Make((OperatorInterater[]){
            OperatorInterater_Make((CStr[]){ NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_CAST,     (Token(*)(void*,Token*,Vector*))Str_Handler_Cast),
                OperatorDefiner_New(TOKEN_INIT,     NULL),
                OperatorDefiner_New(TOKEN_DESTROY,  NULL),
                OPERATORDEFINER_END
            })),
            OperatorInterater_Make((CStr[]){ "str",NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_IR_ASS,   (Token(*)(void*,Token*,Vector*))Str_Str_Handler_Ass),
                OperatorDefiner_New(TOKEN_IR_ADD,   (Token(*)(void*,Token*,Vector*))Str_Str_Handler_Add2),
                OperatorDefiner_New(TOKEN_IR_EQU,   (Token(*)(void*,Token*,Vector*))Str_Str_Handler_Equ),
                OPERATORDEFINER_END
            })),
            OperatorInterater_Make((CStr[]){ "str","str",NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_IR_ADD,   (Token(*)(void*,Token*,Vector*))Str_Str_Handler_Add3),
                OPERATORDEFINER_END
            })),
            OPERATORINTERATER_END
        }),Str_Destroyer,Str_Cpyer)
    );
}
