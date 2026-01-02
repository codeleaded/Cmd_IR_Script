#include "/home/codeleaded/System/Static/Library/AlxCallStack.h"
#include "/home/codeleaded/System/Static/Library/AlxExternFunctions.h"
#include "/home/codeleaded/System/Static/Library/IntraASM.h"

char Int_All_Const(Vector* args){
    if(args->size <= 0) return 0;

    for(int i = 0;i<args->size;i++){
        Token* t = (Token*)Vector_Get(args,i);
        if(t->tt != TOKEN_NUMBER){
            return 0;
        }
    }
    return 1;
}

Token Int_Int_Handler_Ass(IntraASM* ll,Token* op,Vector* args){
    return IntraASM_ExecuteAss(ll,ASM_OP_ASS,ASM_TYPE_INT,args);
}
Token Int_Int_Handler_Add(IntraASM* ll,Token* op,Vector* args){
    if(Int_All_Const(args)){
        Token* src = (Token*)Vector_Get(args,0);
        Number ret = Number_Parse(src->str);
        for(int i = 1;i<args->size;i++){
            Token* argi = (Token*)Vector_Get(args,i);
            ret += Number_Parse(argi->str);
        }
        return Token_Move(TOKEN_NUMBER,Number_Get(ret));
    }
    return IntraASM_Execute(ll,ASM_OP_ADD,ASM_TYPE_INT,args);
}
Token Int_Int_Handler_Sub(IntraASM* ll,Token* op,Vector* args){
    if(Int_All_Const(args)){
        Token* src = (Token*)Vector_Get(args,0);
        Number ret = Number_Parse(src->str);
        for(int i = 1;i<args->size;i++){
            Token* argi = (Token*)Vector_Get(args,i);
            ret -= Number_Parse(argi->str);
        }
        return Token_Move(TOKEN_NUMBER,Number_Get(ret));
    }
    return IntraASM_Execute(ll,ASM_OP_SUB,ASM_TYPE_INT,args);
}
Token Int_Int_Handler_Mul(IntraASM* ll,Token* op,Vector* args){
    if(Int_All_Const(args)){
        Token* src = (Token*)Vector_Get(args,0);
        Number ret = Number_Parse(src->str);
        for(int i = 1;i<args->size;i++){
            Token* argi = (Token*)Vector_Get(args,i);
            ret *= Number_Parse(argi->str);
        }
        return Token_Move(TOKEN_NUMBER,Number_Get(ret));
    }
    return IntraASM_Execute(ll,ASM_OP_MUL,ASM_TYPE_INT,args);
}
Token Int_Int_Handler_Div(IntraASM* ll,Token* op,Vector* args){
    if(Int_All_Const(args)){
        Token* src = (Token*)Vector_Get(args,0);
        Number ret = Number_Parse(src->str);
        for(int i = 1;i<args->size;i++){
            Token* argi = (Token*)Vector_Get(args,i);
            ret /= Number_Parse(argi->str);
        }
        return Token_Move(TOKEN_NUMBER,Number_Get(ret));
    }
    return IntraASM_Execute(ll,ASM_OP_DIV,ASM_TYPE_INT,args);
}
Token Int_Int_Handler_Neg(IntraASM* ll,Token* op,Vector* args){
    return IntraASM_Execute(ll,ASM_OP_NEG,ASM_TYPE_INT,args);
}

Token Int_Int_Handler_Equ(IntraASM* ll,Token* op,Vector* args){
    if(Int_All_Const(args)){
        Token* src = (Token*)Vector_Get(args,0);
        Number ret = Number_Parse(src->str);

        for(int i = 1;i<args->size;i++){
            Token* argi = (Token*)Vector_Get(args,i);
            
            if(!(ret == Number_Parse(argi->str)))
                return Token_Move(TOKEN_BOOL,Boolean_Get(0));
        }
        return Token_Move(TOKEN_BOOL,Boolean_Get(1));
    }
    return IntraASM_Execute(ll,ASM_OP_EQU,ASM_TYPE_INT,args);
}
Token Int_Int_Handler_Les(IntraASM* ll,Token* op,Vector* args){
    if(Int_All_Const(args)){
        Token* src = (Token*)Vector_Get(args,0);
        Number ret = Number_Parse(src->str);

        for(int i = 1;i<args->size;i++){
            Token* argi = (Token*)Vector_Get(args,i);
            
            if(!(ret < Number_Parse(argi->str)))
                return Token_Move(TOKEN_BOOL,Boolean_Get(0));
        }
        return Token_Move(TOKEN_BOOL,Boolean_Get(1));
    }
    return IntraASM_Execute(ll,ASM_OP_LES,ASM_TYPE_INT,args);
}
Token Int_Int_Handler_Grt(IntraASM* ll,Token* op,Vector* args){
    if(Int_All_Const(args)){
        Token* src = (Token*)Vector_Get(args,0);
        Number ret = Number_Parse(src->str);

        for(int i = 1;i<args->size;i++){
            Token* argi = (Token*)Vector_Get(args,i);
            
            if(!(ret > Number_Parse(argi->str)))
                return Token_Move(TOKEN_BOOL,Boolean_Get(0));
        }
        return Token_Move(TOKEN_BOOL,Boolean_Get(1));
    }
    return IntraASM_Execute(ll,ASM_OP_GRT,ASM_TYPE_INT,args);
}
Token Int_Int_Handler_Leq(IntraASM* ll,Token* op,Vector* args){
    if(Int_All_Const(args)){
        Token* src = (Token*)Vector_Get(args,0);
        Number ret = Number_Parse(src->str);

        for(int i = 1;i<args->size;i++){
            Token* argi = (Token*)Vector_Get(args,i);
            
            if(!(ret <= Number_Parse(argi->str)))
                return Token_Move(TOKEN_BOOL,Boolean_Get(0));
        }
        return Token_Move(TOKEN_BOOL,Boolean_Get(1));
    }
    return IntraASM_Execute(ll,ASM_OP_LEQ,ASM_TYPE_INT,args);
}
Token Int_Int_Handler_Grq(IntraASM* ll,Token* op,Vector* args){
    if(Int_All_Const(args)){
        Token* src = (Token*)Vector_Get(args,0);
        Number ret = Number_Parse(src->str);

        for(int i = 1;i<args->size;i++){
            Token* argi = (Token*)Vector_Get(args,i);
            
            if(!(ret >= Number_Parse(argi->str)))
                return Token_Move(TOKEN_BOOL,Boolean_Get(0));
        }
        return Token_Move(TOKEN_BOOL,Boolean_Get(1));
    }
    return IntraASM_Execute(ll,ASM_OP_GRQ,ASM_TYPE_INT,args);
}

Token Int_Handler_Cast(IntraASM* ll,Token* op,Vector* args){
    if(Int_All_Const(args)){
        Token* src = (Token*)Vector_Get(args,0);
        if(CStr_Cmp(op->str,ASM_TYPE_INT))     return Token_By(TOKEN_NUMBER,src->str);
        if(CStr_Cmp(op->str,"float"))   return Token_By(TOKEN_FLOAT,src->str);
        if(CStr_Cmp(op->str,"str"))     return Token_By(TOKEN_CONSTSTRING_DOUBLE,src->str);
    }
    return IntraASM_Execute(ll,ASM_OP_CAST,ASM_TYPE_INT,args);
}

void Ex_Packer(ExternFunctionMap* Extern_Functions,Vector* funcs,Scope* s){//Vector<CStr>
    TypeMap_PushContained(&s->types,funcs,
        Type_New("int",sizeof(VariableASM),OperatorInterationMap_Make((OperatorInterater[]){
            OperatorInterater_Make((CStr[]){ NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_INTRAASM_NEG,  (Token(*)(void*,Token*,Vector*))Int_Int_Handler_Neg),
                OperatorDefiner_New(TOKEN_CAST,         (Token(*)(void*,Token*,Vector*))Int_Handler_Cast),
                //OperatorDefiner_New(TOKEN_INIT,NULL),
                //OperatorDefiner_New(TOKEN_DESTROY,NULL),
                OPERATORDEFINER_END
            })),
            OperatorInterater_Make((CStr[]){ "int",NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_INTRAASM_ASS,  (Token(*)(void*,Token*,Vector*))Int_Int_Handler_Ass),
                OperatorDefiner_New(TOKEN_INTRAASM_ADD,  (Token(*)(void*,Token*,Vector*))Int_Int_Handler_Add),
                OperatorDefiner_New(TOKEN_INTRAASM_SUB,  (Token(*)(void*,Token*,Vector*))Int_Int_Handler_Sub),
                OperatorDefiner_New(TOKEN_INTRAASM_MUL,  (Token(*)(void*,Token*,Vector*))Int_Int_Handler_Mul),
                OperatorDefiner_New(TOKEN_INTRAASM_DIV,  (Token(*)(void*,Token*,Vector*))Int_Int_Handler_Div),
                OperatorDefiner_New(TOKEN_INTRAASM_EQU,  (Token(*)(void*,Token*,Vector*))Int_Int_Handler_Equ),
                OperatorDefiner_New(TOKEN_INTRAASM_LES,  (Token(*)(void*,Token*,Vector*))Int_Int_Handler_Les),
                OperatorDefiner_New(TOKEN_INTRAASM_GRT,  (Token(*)(void*,Token*,Vector*))Int_Int_Handler_Grt),
                OperatorDefiner_New(TOKEN_INTRAASM_LEQ,  (Token(*)(void*,Token*,Vector*))Int_Int_Handler_Leq),
                OperatorDefiner_New(TOKEN_INTRAASM_GRQ,  (Token(*)(void*,Token*,Vector*))Int_Int_Handler_Grq),
                OPERATORDEFINER_END
            })),
            OPERATORINTERATER_END
        }),VariableASM_Destroyer,VariableASM_Cpyer)
    );
}