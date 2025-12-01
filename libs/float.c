#include "/home/codeleaded/System/Static/Library/AlxCallStack.h"
#include "/home/codeleaded/System/Static/Library/AlxExternFunctions.h"
#include "/home/codeleaded/System/Static/Library/IntraIR.h"

Double Implementation_FloatOf(Scope* s,Token* a){
    Double n = DOUBLE_PARSE_ERROR;
    
    if(a->tt==TOKEN_STRING){
        Variable* a_var = Scope_FindVariable(s,a->str);
        if(a_var){
            n = *(Double*)Variable_Data(a_var);
        }else{
            printf("[Float]: Impl -> 1. Arg: Variable %s doesn't exist!\n",a->str);
        }
    }else if(a->tt==TOKEN_FLOAT){
        n = Double_Parse(a->str,1);
    }else if(a->tt==TOKEN_NUMBER){
        n = (Double)Number_Parse(a->str);
    }else{
        printf("[Float]: Impl -> 1. Arg: %s is not a float type!\n",a->str);
    }
    return n;
}

Token Float_Float_Handler_Ass(Scope* s,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);

    //printf("ASS: %s = %s\n",a->str,b->str);

    Double n2 = Implementation_FloatOf(s,b);
    
    if(a->tt==TOKEN_STRING){
        Variable* a_var = Scope_FindVariable(s,a->str);
        if(a_var){
            Variable_PrepairFor(a_var,sizeof(Double),"float",NULL,NULL);
            Variable_SetTo(a_var,(Double[]){ n2 });
        }else{
            Scope_BuildInitVariableRange(s,a->str,"float",s->range-1,(Double[]){ n2 });
            //printf("[Float_Ass]: 1. Arg: Variable %s doesn't exist!\n",a->str);
        }
    }else{
        printf("[Float_Ass]: 1. Arg: %s is not a variable type!\n",a->str);
    }

    Double res = n2;

    char* resstr = Double_Get(res,6);
    return Token_Move(TOKEN_FLOAT,resstr);
}
Token Float_Float_Handler_Add(Scope* s,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);

    //printf("ADD: %s + %s\n",a->str,b->str);

    Double n1 = Implementation_FloatOf(s,a);
    Double n2 = Implementation_FloatOf(s,b);
    Double res = n1 + n2;

    char* resstr = Double_Get(res,6);
    return Token_Move(TOKEN_FLOAT,resstr);
}
Token Float_Float_Handler_Sub(Scope* s,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);

    //printf("SUB: %s - %s\n",a->str,b->str);

    Double n1 = Implementation_FloatOf(s,a);
    Double n2 = Implementation_FloatOf(s,b);
    Double res = n1 - n2;

    char* resstr = Double_Get(res,6);
    return Token_Move(TOKEN_FLOAT,resstr);
}
Token Float_Float_Handler_Mul(Scope* s,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);

    //printf("MUL: %s * %s\n",a->str,b->str);

    Double n1 = Implementation_FloatOf(s,a);
    Double n2 = Implementation_FloatOf(s,b);
    Double res = n1 * n2;

    char* resstr = Double_Get(res,6);
    return Token_Move(TOKEN_FLOAT,resstr);
}
Token Float_Float_Handler_Div(Scope* s,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);

    //printf("DIV: %s / %s\n",a->str,b->str);

    Double n1 = Implementation_FloatOf(s,a);
    Double n2 = Implementation_FloatOf(s,b);

    Double res = 0.0;
    if(n2!=0.0) res = n1 / n2;
    else{
        printf("[Enviroment]: Error: DIV by Zero\n");
    }

    char* resstr = Double_Get(res,6);
    return Token_Move(TOKEN_FLOAT,resstr);
}
Token Float_Float_Handler_Neg(Scope* s,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    //printf("NEG: -%s\n",a->str);

    Double n1 = Implementation_FloatOf(s,a);
    Double res = -n1;

    char* resstr = Double_Get(res,6);
    return Token_Move(TOKEN_FLOAT,resstr);
}
Token Float_Float_Handler_Equ(Scope* s,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);

    //printf("EQU: %s == %s\n",a->str,b->str);

    Double n1 = Implementation_FloatOf(s,a);
    Double n2 = Implementation_FloatOf(s,b);
    Boolean res = n1 == n2;

    char* resstr = Boolean_Get(res);
    return Token_Move(TOKEN_BOOL,resstr);
}
Token Float_Float_Handler_Les(Scope* s,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);

    //printf("LES: %s < %s\n",a->str,b->str);

    Double n1 = Implementation_FloatOf(s,a);
    Double n2 = Implementation_FloatOf(s,b);
    Boolean res = n1 < n2;

    char* resstr = Boolean_Get(res);
    return Token_Move(TOKEN_BOOL,resstr);
}
Token Float_Float_Handler_Grt(Scope* s,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);

    //printf("GRT: %s > %s\n",a->str,b->str);

    Double n1 = Implementation_FloatOf(s,a);
    Double n2 = Implementation_FloatOf(s,b);
    Boolean res = n1 > n2;

    char* resstr = Boolean_Get(res);
    return Token_Move(TOKEN_BOOL,resstr);
}
Token Float_Float_Handler_Leq(Scope* s,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);

    //printf("LEQ: %s <= %s\n",a->str,b->str);

    Double n1 = Implementation_FloatOf(s,a);
    Double n2 = Implementation_FloatOf(s,b);
    Boolean res = n1 <= n2;

    char* resstr = Boolean_Get(res);
    return Token_Move(TOKEN_BOOL,resstr);
}
Token Float_Float_Handler_Grq(Scope* s,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);
    Token* b = (Token*)Vector_Get(args,1);

    //printf("GRQ: %s >= %s\n",a->str,b->str);

    Double n1 = Implementation_FloatOf(s,a);
    Double n2 = Implementation_FloatOf(s,b);
    Boolean res = n1 >= n2;

    char* resstr = Boolean_Get(res);
    return Token_Move(TOKEN_BOOL,resstr);
}
Token Float_Handler_Cast(Scope* s,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    //printf("CAST: %s\n",a->str);

    Double n1 = Implementation_FloatOf(s,a);
    
    Double res = n1;
    char* resstr = Double_Get(res,6);
    return Token_Move(TOKEN_CONSTSTRING_DOUBLE,resstr);
}

void Ex_Packer(ExternFunctionMap* Extern_Functions,Vector* funcs,Scope* s){//Vector<CStr>
    TypeMap_PushContained(&s->types,funcs,
        Type_New("float",8,OperatorInterationMap_Make((OperatorInterater[]){
            OperatorInterater_Make((CStr[]){ NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_INTRAIR_NEG,  (Token(*)(void*,Token*,Vector*))Float_Float_Handler_Neg),
                OperatorDefiner_New(TOKEN_CAST,         (Token(*)(void*,Token*,Vector*))Float_Handler_Cast),
                OperatorDefiner_New(TOKEN_INIT,NULL),
                OperatorDefiner_New(TOKEN_DESTROY,NULL),
                OPERATORDEFINER_END
            })),
            OperatorInterater_Make((CStr[]){ "float",NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_INTRAIR_ASS,(Token(*)(void*,Token*,Vector*))Float_Float_Handler_Ass),
                OperatorDefiner_New(TOKEN_INTRAIR_ADD,(Token(*)(void*,Token*,Vector*))Float_Float_Handler_Add),
                OperatorDefiner_New(TOKEN_INTRAIR_SUB,(Token(*)(void*,Token*,Vector*))Float_Float_Handler_Sub),
                OperatorDefiner_New(TOKEN_INTRAIR_MUL,(Token(*)(void*,Token*,Vector*))Float_Float_Handler_Mul),
                OperatorDefiner_New(TOKEN_INTRAIR_DIV,(Token(*)(void*,Token*,Vector*))Float_Float_Handler_Div),
                OperatorDefiner_New(TOKEN_INTRAIR_EQU,(Token(*)(void*,Token*,Vector*))Float_Float_Handler_Equ),
                OperatorDefiner_New(TOKEN_INTRAIR_LES,(Token(*)(void*,Token*,Vector*))Float_Float_Handler_Les),
                OperatorDefiner_New(TOKEN_INTRAIR_GRT,(Token(*)(void*,Token*,Vector*))Float_Float_Handler_Grt),
                OperatorDefiner_New(TOKEN_INTRAIR_LEQ,(Token(*)(void*,Token*,Vector*))Float_Float_Handler_Leq),
                OperatorDefiner_New(TOKEN_INTRAIR_GRQ,(Token(*)(void*,Token*,Vector*))Float_Float_Handler_Grq),
                OPERATORDEFINER_END
            })),
            OPERATORINTERATER_END
        }),NULL,NULL)
    );
}