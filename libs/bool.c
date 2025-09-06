#include "/home/codeleaded/System/Static/Library/AlxCallStack.h"
#include "/home/codeleaded/System/Static/Library/AlxExternFunctions.h"
#include "/home/codeleaded/System/Static/Library/LuaLikeDefines.h"

Boolean Implementation_BooleanOf(Scope* s,Token* a){
    Boolean n = 0;
    
    if(a->tt==TOKEN_STRING){
        Variable* a_var = Scope_FindVariable(s,a->str);
        if(a_var){
            n = *(Boolean*)Variable_Data(a_var);
        }else{
            printf("[Boolean]: Impl -> 1. Arg: Variable %s doesn't exist!\n",a->str);
        }
    }else if(a->tt==TOKEN_BOOL){
        n = Boolean_Parse(a->str);
    }else if(a->tt == TOKEN_NUMBER){
        n = Number_Parse(a->str) != 0;
    }else{
        printf("[Boolean]: Impl -> 1. Arg: %s is not a bool type!\n",a->str);
    }
    return n;
}

Token Boolean_Boolean_Handler_Ass(Scope* s,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);

    //printf("ASS: %s = %s\n",a->str,b->str);

    Boolean n2;
    
    if(b->tt==TOKEN_STRING){
        Variable* b_var = Scope_FindVariable(s,b->str);
        if(b_var){
            n2 = *(Boolean*)b_var->data;
        }else{
            printf("[Boolean_Ass]: 2. Arg: Variable %s doesn't exist!\n",b->str);
        }
    }else if(b->tt==TOKEN_BOOL){
        n2 = Boolean_Parse(b->str);
    }else{
        printf("[Boolean_Ass]: 2. Arg: %s is not a bool type!\n",b->str);
    }
    
    if(a->tt==TOKEN_STRING){
        Variable* a_var = Scope_FindVariable(s,a->str);
        if(a_var){
            Variable_PrepairFor(a_var,sizeof(Boolean),"bool",NULL,NULL);
            Variable_SetTo(a_var,(Boolean[]){ n2 });
        }else{
            Scope_BuildInitVariableRange(s,a->str,"bool",s->range-1,(Boolean[]){ n2 });
        }
    }else{
        printf("[Boolean_Ass]: 1. Arg: %s is not a variable type!\n",a->str);
    }

    Boolean res = n2;

    char* resstr = Boolean_Get(res);
    return Token_Move(TOKEN_BOOL,resstr);
}
Token Boolean_Boolean_Handler_Equ(Scope* s,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);

    //printf("EQU: %s == %s\n",a->str,b->str);

    Boolean n1 = Implementation_BooleanOf(s,a);
    Boolean n2 = Implementation_BooleanOf(s,b);
    Boolean res = n1 == n2;

    char* resstr = Boolean_Get(res);
    return Token_Move(TOKEN_BOOL,resstr);
}
Token Boolean_Boolean_Handler_And(Scope* s,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);

    //printf("AND: %s && %s\n",a->str,b->str);

    Boolean n1 = Implementation_BooleanOf(s,a);
    Boolean n2 = Implementation_BooleanOf(s,b);
    Boolean res = n1 && n2;

    char* resstr = Boolean_Get(res);
    return Token_Move(TOKEN_BOOL,resstr);
}
Token Boolean_Boolean_Handler_Or(Scope* s,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);

    //printf("OR: %s || %s\n",a->str,b->str);

    Boolean n1 = Implementation_BooleanOf(s,a);
    Boolean n2 = Implementation_BooleanOf(s,b);
    Boolean res = n1 || n2;

    char* resstr = Boolean_Get(res);
    return Token_Move(TOKEN_BOOL,resstr);
}
Token Boolean_Handler_Not(Scope* s,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    //printf("NOT: !%s\n",a->str,b->str);

    Boolean n1 = Implementation_BooleanOf(s,a);
    Boolean res = !n1;

    char* resstr = Boolean_Get(res);
    return Token_Move(TOKEN_BOOL,resstr);
}
Token Boolean_Handler_Cast(Scope* s,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    Boolean n1 = Implementation_BooleanOf(s,a);
    
    Boolean res = n1;
    char* resstr = Boolean_Get(res);
    return Token_Move(TOKEN_CONSTSTRING_DOUBLE,resstr);
}

void Ex_Packer(ExternFunctionMap* Extern_Functions,Vector* funcs,Scope* s){//Vector<CStr>
    TypeMap_PushContained(&s->types,funcs,
        Type_New("bool",sizeof(Boolean),OperatorInterationMap_Make((OperatorInterater[]){
            OperatorInterater_Make((CStr[]){ NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_LUALIKE_LOT,  (Token(*)(void*,Token*,Vector*))Boolean_Handler_Not),
                OperatorDefiner_New(TOKEN_CAST,         (Token(*)(void*,Token*,Vector*))Boolean_Handler_Cast),
                OperatorDefiner_New(TOKEN_INIT,NULL),
                OperatorDefiner_New(TOKEN_DESTROY,NULL),
                OPERATORDEFINER_END
            })),
            OperatorInterater_Make((CStr[]){ "bool",NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_LUALIKE_ASS,  (Token(*)(void*,Token*,Vector*))Boolean_Boolean_Handler_Ass),
                OperatorDefiner_New(TOKEN_LUALIKE_EQU,  (Token(*)(void*,Token*,Vector*))Boolean_Boolean_Handler_Equ),
                OperatorDefiner_New(TOKEN_LUALIKE_LND,  (Token(*)(void*,Token*,Vector*))Boolean_Boolean_Handler_And),
                OperatorDefiner_New(TOKEN_LUALIKE_LOR,  (Token(*)(void*,Token*,Vector*))Boolean_Boolean_Handler_Or),
                OPERATORDEFINER_END
            })),
            OPERATORINTERATER_END
        }),NULL,NULL)
    );
}