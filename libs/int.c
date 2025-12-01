#include "/home/codeleaded/System/Static/Library/AlxCallStack.h"
#include "/home/codeleaded/System/Static/Library/AlxExternFunctions.h"
#include "/home/codeleaded/System/Static/Library/IntraIR.h"

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

Token Int_Int_Handler_Ass(IntraIR* ll,Token* op,Vector* args){
    return IntraIR_ExecuteAss(ll,IR_OP_ASS,IR_TYPE_INT,args);
}
Token Int_Int_Handler_Add(IntraIR* ll,Token* op,Vector* args){
    if(Int_All_Const(args)){
        Token* src = (Token*)Vector_Get(args,0);
        Number ret = Number_Parse(src->str);
        for(int i = 1;i<args->size;i++){
            Token* argi = (Token*)Vector_Get(args,i);
            ret += Number_Parse(argi->str);
        }
        return Token_Move(TOKEN_NUMBER,Number_Get(ret));
    }
    return IntraIR_Execute(ll,IR_OP_ADD,IR_TYPE_INT,args);
}
Token Int_Int_Handler_Sub(IntraIR* ll,Token* op,Vector* args){
    if(Int_All_Const(args)){
        Token* src = (Token*)Vector_Get(args,0);
        Number ret = Number_Parse(src->str);
        for(int i = 1;i<args->size;i++){
            Token* argi = (Token*)Vector_Get(args,i);
            ret -= Number_Parse(argi->str);
        }
        return Token_Move(TOKEN_NUMBER,Number_Get(ret));
    }
    return IntraIR_Execute(ll,IR_OP_SUB,IR_TYPE_INT,args);
}
Token Int_Int_Handler_Mul(IntraIR* ll,Token* op,Vector* args){
    if(Int_All_Const(args)){
        Token* src = (Token*)Vector_Get(args,0);
        Number ret = Number_Parse(src->str);
        for(int i = 1;i<args->size;i++){
            Token* argi = (Token*)Vector_Get(args,i);
            ret *= Number_Parse(argi->str);
        }
        return Token_Move(TOKEN_NUMBER,Number_Get(ret));
    }
    return IntraIR_Execute(ll,IR_OP_MUL,IR_TYPE_INT,args);
}
Token Int_Int_Handler_Div(IntraIR* ll,Token* op,Vector* args){
    if(Int_All_Const(args)){
        Token* src = (Token*)Vector_Get(args,0);
        Number ret = Number_Parse(src->str);
        for(int i = 1;i<args->size;i++){
            Token* argi = (Token*)Vector_Get(args,i);
            ret /= Number_Parse(argi->str);
        }
        return Token_Move(TOKEN_NUMBER,Number_Get(ret));
    }
    return IntraIR_Execute(ll,IR_OP_DIV,IR_TYPE_INT,args);
}
Token Int_Int_Handler_Neg(IntraIR* ll,Token* op,Vector* args){
    return IntraIR_Execute(ll,IR_OP_NEG,IR_TYPE_INT,args);
}

Token Int_Int_Handler_Equ(IntraIR* ll,Token* op,Vector* args){
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
    return IntraIR_Execute(ll,IR_OP_EQU,IR_TYPE_INT,args);
}
Token Int_Int_Handler_Les(IntraIR* ll,Token* op,Vector* args){
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
    return IntraIR_Execute(ll,IR_OP_LES,IR_TYPE_INT,args);
}
Token Int_Int_Handler_Grt(IntraIR* ll,Token* op,Vector* args){
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
    return IntraIR_Execute(ll,IR_OP_GRT,IR_TYPE_INT,args);
}
Token Int_Int_Handler_Leq(IntraIR* ll,Token* op,Vector* args){
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
    return IntraIR_Execute(ll,IR_OP_LEQ,IR_TYPE_INT,args);
}
Token Int_Int_Handler_Grq(IntraIR* ll,Token* op,Vector* args){
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
    return IntraIR_Execute(ll,IR_OP_GRQ,IR_TYPE_INT,args);
}

Token Int_Handler_Cast(IntraIR* ll,Token* op,Vector* args){
    if(Int_All_Const(args)){
        Token* src = (Token*)Vector_Get(args,0);
        if(CStr_Cmp(op->str,IR_TYPE_INT))     return Token_By(TOKEN_NUMBER,src->str);
        if(CStr_Cmp(op->str,"float"))   return Token_By(TOKEN_FLOAT,src->str);
        if(CStr_Cmp(op->str,"str"))     return Token_By(TOKEN_CONSTSTRING_DOUBLE,src->str);
    }
    return IntraIR_Execute(ll,IR_OP_CAST,IR_TYPE_INT,args);
}

void Ex_Packer(ExternFunctionMap* Extern_Functions,Vector* funcs,Scope* s){//Vector<CStr>
    TypeMap_PushContained(&s->types,funcs,
        Type_New("int",sizeof(VariableIR),OperatorInterationMap_Make((OperatorInterater[]){
            OperatorInterater_Make((CStr[]){ NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_INTRAIR_NEG,  (Token(*)(void*,Token*,Vector*))Int_Int_Handler_Neg),
                OperatorDefiner_New(TOKEN_CAST,         (Token(*)(void*,Token*,Vector*))Int_Handler_Cast),
                //OperatorDefiner_New(TOKEN_INIT,NULL),
                //OperatorDefiner_New(TOKEN_DESTROY,NULL),
                OPERATORDEFINER_END
            })),
            OperatorInterater_Make((CStr[]){ "int",NULL },OperatorDefineMap_Make((OperatorDefiner[]){
                OperatorDefiner_New(TOKEN_INTRAIR_ASS,  (Token(*)(void*,Token*,Vector*))Int_Int_Handler_Ass),
                OperatorDefiner_New(TOKEN_INTRAIR_ADD,  (Token(*)(void*,Token*,Vector*))Int_Int_Handler_Add),
                OperatorDefiner_New(TOKEN_INTRAIR_SUB,  (Token(*)(void*,Token*,Vector*))Int_Int_Handler_Sub),
                OperatorDefiner_New(TOKEN_INTRAIR_MUL,  (Token(*)(void*,Token*,Vector*))Int_Int_Handler_Mul),
                OperatorDefiner_New(TOKEN_INTRAIR_DIV,  (Token(*)(void*,Token*,Vector*))Int_Int_Handler_Div),
                OperatorDefiner_New(TOKEN_INTRAIR_EQU,  (Token(*)(void*,Token*,Vector*))Int_Int_Handler_Equ),
                OperatorDefiner_New(TOKEN_INTRAIR_LES,  (Token(*)(void*,Token*,Vector*))Int_Int_Handler_Les),
                OperatorDefiner_New(TOKEN_INTRAIR_GRT,  (Token(*)(void*,Token*,Vector*))Int_Int_Handler_Grt),
                OperatorDefiner_New(TOKEN_INTRAIR_LEQ,  (Token(*)(void*,Token*,Vector*))Int_Int_Handler_Leq),
                OperatorDefiner_New(TOKEN_INTRAIR_GRQ,  (Token(*)(void*,Token*,Vector*))Int_Int_Handler_Grq),
                OPERATORDEFINER_END
            })),
            OPERATORINTERATER_END
        }),VariableIR_Destroyer,VariableIR_Cpyer)
    );
}