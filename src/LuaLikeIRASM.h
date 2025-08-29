#ifndef LUALIKEIRASM
#define LUALIKEIRASM

#include "/home/codeleaded/System/Static/Library/AlxScope.h"
#include "/home/codeleaded/System/Static/Library/AlxShutingYard.h"
#include "/home/codeleaded/System/Static/Library/AlxCompiler.h"
#include "/home/codeleaded/System/Static/Library/ConstParser.h"
#include "/home/codeleaded/System/Static/Library/Files.h"
#include "/home/codeleaded/System/Static/Library/String_G.h"
#include "/home/codeleaded/System/Static/Library/../Container/CVector.h"

#include "LuaLikeIRDefines.h"


typedef struct LogicBlock {
    int count;
    int extend;
} LogicBlock;

typedef struct Define {
    CStr name;
    TokenMap content;
} Define;

Define Define_New(CStr name,TokenMap tm){
    Define d;
    d.name = CStr_Cpy(name);
    d.content = tm;
    return d;
}
void Define_Free(Define* d){
    CStr_Free(&d->name);
    TokenMap_Free(&d->content);
}
void Define_Print(Define* d){
    printf("%s: ",d->name);
    TokenMap_Print_S(&d->content);
}


typedef struct LuaLikeIR {
    Compiler ev; // like inheritance
    CVector filesstack; // call stack of files -> path
    CVector filesinc;
    String text;
    Vector logicpath; // Vector<LogicBlock>
    char* dllpath;
    char* src;
    char* output;
    unsigned int indent;
    TT_Type logic;
} LuaLikeIR;

int LuaLikeIR_Indentation(LuaLikeIR* ll){
    return I32_Min(1,ll->indent);
}
void LuaLikeIR_Indentation_To(LuaLikeIR* ll,int indent){
    ll->indent = indent;
}

CStr LuaLikeIR_Indentation_CStr(LuaLikeIR* ll){
    String builder = String_New();

    int Indentation = LuaLikeIR_Indentation(ll);
    for(int i = 0;i<Indentation;i++) String_Append(&builder,LUALIKEIR_INDENTATION);

    CStr cstr = String_CStr(&builder);
    String_Free(&builder);
    return cstr;
}
void LuaLikeIR_Indentation_Do(LuaLikeIR* ll,String* str){
    if(LuaLikeIR_Indentation(ll)>0){
        CStr indent = LuaLikeIR_Indentation_CStr(ll);
        String_Append(&ll->text,indent);
        CStr_Free(&indent);
    }
}
void LuaLikeIR_Indentation_Append(LuaLikeIR* ll,String* str,char* cstr){
    LuaLikeIR_Indentation_Do(ll,str);
    String_Append(str,cstr);
}
void LuaLikeIR_Indentation_Appendf(LuaLikeIR* ll,String* str,char* FormatCStr,...){
    LuaLikeIR_Indentation_Do(ll,str);
    
    va_list args;
    va_start(args,FormatCStr);
    String app = String_FormatA(FormatCStr,args);
    String_AppendString(str,&app);
    String_AppendChar(str,'\n');
    String_Free(&app);
    va_end(args);
}

CStr LuaLikeIR_Variablename_Next(LuaLikeIR* ll,CStr name,int offset){
    CStr build = CStr_Concat(name,"*");

    Number stack_name_count = Scope_FindVariablenameLastLike(&ll->ev.sc,build,'*',offset);
    if(stack_name_count==NUMBER_PARSE_ERROR)    stack_name_count = 0;
    else                                        stack_name_count++;


    CStr_Free(&build);
    CStr stack_name = CStr_Format("%s%d",name,stack_name_count);
    return stack_name;
}

/*
void LuaLikeIR_Variable_BuildXX(LuaLikeIR* ll,CStr name,CStr type,int sizeonstack,int stack,char destroy){
    Scope_BuildInitVariable(&ll->ev.sc,name,type,(LuaLikeIRVariable[]){ LuaLikeIRVariable_New(stack,sizeonstack,destroy,ll) });
}
void LuaLikeIR_Variable_BuildX(LuaLikeIR* ll,CStr name,CStr type,int sizeonstack,char destroy){
    Scope_BuildInitVariable(&ll->ev.sc,name,type,(LuaLikeIRVariable[]){ LuaLikeIRVariable_New(ll->stack,sizeonstack,destroy,ll) });
}
void LuaLikeIR_Variable_Build(LuaLikeIR* ll,CStr name,CStr type){
    LuaLikeIR_Variable_BuildX(ll,name,type,0,1);
}
void LuaLikeIR_Variable_BuildRange(LuaLikeIR* ll,CStr name,CStr type,Range r,int sizeonstack,char destroy){
    Scope_BuildInitVariableRange(&ll->ev.sc,name,type,r,(LuaLikeIRVariable[]){ LuaLikeIRVariable_New(ll->stack,sizeonstack,destroy,ll) });
}
void LuaLikeIR_Variable_Build_Decl(LuaLikeIR* ll,CStr name,CStr type){
    LuaLikeIR_Variable_Build(ll,name,type);
    
    if(ll->ev.sc.range==0){
        CStr g_value = LuaLikeIR_BuildGlobal(ll,name,LuaLikeIR_Size(ll,type));
        Variable* v = Scope_FindVariable(&ll->ev.sc,name);
        LuaLikeIRVariable* sv = (LuaLikeIRVariable*)Variable_Data(v);
        sv->global = g_value;
    }else{
        LuaLikeIR_Indentation_Appendf(ll,&ll->text,"sub rsp,%d",LuaLikeIR_Size(ll,type));
    }
}
void LuaLikeIR_Variable_Build_Ref(LuaLikeIR* ll,CStr name,CStr type){
    LuaLikeIR_Variable_BuildX(ll,name,type,0,0);
}
void LuaLikeIR_Variable_BuildRange_Decl(LuaLikeIR* ll,CStr name,CStr type,Range r){
    LuaLikeIR_Variable_BuildRange(ll,name,type,r,0,1);
    
    if(ll->ev.sc.range==0){
        CStr g_value = LuaLikeIR_BuildGlobal(ll,name,LuaLikeIR_Size(ll,type));
        Variable* v = Scope_FindVariable(&ll->ev.sc,name);
        LuaLikeIRVariable* sv = (LuaLikeIRVariable*)Variable_Data(v);
        sv->global = g_value;
    }else{
        LuaLikeIR_Indentation_Appendf(ll,&ll->text,"sub rsp,%d",LuaLikeIR_Size(ll,type));
    }
}
void LuaLikeIR_Variable_Build_Ref_Decl(LuaLikeIR* ll,CStr name,CStr type){
    LuaLikeIR_Variable_Build_Ref(ll,name,type);

    if(ll->ev.sc.range==0){
        CStr g_value = LuaLikeIR_BuildGlobal(ll,name,LuaLikeIR_Size(ll,type));
        Variable* v = Scope_FindVariable(&ll->ev.sc,name);
        LuaLikeIRVariable* sv = (LuaLikeIRVariable*)Variable_Data(v);
        sv->global = g_value;
    }else{
        LuaLikeIR_Indentation_Appendf(ll,&ll->text,"sub rsp,%d",LuaLikeIR_Size(ll,type));
    }
}
void LuaLikeIR_Variable_Build_Use(LuaLikeIR* ll,CStr name,CStr type,int stack){
    LuaLikeIR_Variable_BuildXX(ll,name,type,0,stack,2);
}
void LuaLikeIR_Variable_Destroy_Decl(LuaLikeIR* ll,CStr name){
    Variable* v = Scope_FindVariable(&ll->ev.sc,name);
    if(v->range>0){
        int size = LuaLikeIR_Size(ll,v->typename);
        if(size>0) LuaLikeIR_Indentation_Appendf(ll,&ll->text,"add rsp,%d",size);
        ll->stack -= size;
    }
}
void LuaLikeIR_Variable_Destroy_Ref_Decl(LuaLikeIR* ll,CStr name){
    Variable* v = Scope_FindVariable(&ll->ev.sc,name);
    if(v->range>0){
        //LuaLikeIR_Indentation_Appendf(ll,&ll->text,"add rsp,%d",LuaLikeIR_Size(ll,v->typename));
        ll->stack -= LuaLikeIR_Size(ll,v->typename);
    }
}
void LuaLikeIR_Variable_Destroy_Use(LuaLikeIR* ll,CStr name){
    //Variable* v = Scope_FindVariable(&ll->ev.sc,name);
}

CStr LuaLikeIR_VariableType(LuaLikeIR* ll,Token* a){
    if(a->tt==TOKEN_STRING){
        Variable* v_a = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v_a) return CStr_Cpy(v_a->typename);
    }
    if(a->tt==TOKEN_LUALIKEIR_BOOLEAN)    return CStr_Cpy(BOOL_TYPE);
    if(a->tt==TOKEN_NUMBER)              return CStr_Cpy(I64_TYPE);
    if(a->tt==TOKEN_CONSTSTRING_DOUBLE)  return CStr_Cpy(STR_TYPE);
    if(a->tt==TOKEN_FLOAT)               return CStr_Cpy(F64_TYPE);
    if(a->tt==TOKEN_FUNCTIONPOINTER)     return CStr_Cpy(NULL_TYPE);
    return NULL;
}
CStr LuaLikeIR_VariablesType(LuaLikeIR* ll,Token* a,Token* b){
    CStr ret = LuaLikeIR_VariableType(ll,a);
    if(ret) return ret;
    return LuaLikeIR_VariableType(ll,b);
}

int LuaLikeIR_TypeRealSize(LuaLikeIR* ll,Token* a){
    CStr type = LuaLikeIR_VariableType(ll,a);
    
    if(LuaLikeIR_DrefType(ll,type)){
        CStr ntype = LuaLikeIR_TypeOfDref(ll,type);
        CStr_Set(&type,ntype);
        CStr_Free(&ntype);
    }

    int size = LuaLikeIR_Size(ll,type);
    CStr_Free(&type);
    return size;
}
char** LuaLikeIR_SelectRT(LuaLikeIR* ll,int size){
    if(size==1) return LuaLikeIR_RT_8;
    if(size==2) return LuaLikeIR_RT_16;
    if(size==4) return LuaLikeIR_RT_32;
    if(size==8) return LuaLikeIR_RT_64;
    return NULL;
}
CStr LuaLikeIR_FunctionName(LuaLikeIR* ll,CStr name){
    String builder = String_Format("f.%s",name);
    String_Replace(&builder,"::",".");
    
    CStr functionname = String_CStr(&builder);
    String_Free(&builder);
    return functionname;
}
CStr LuaLikeIR_SpaceName(LuaLikeIR* ll,CStr name){
    String builder = String_New();

    for(int i = 0;i<ll->ev.cs.size;i++){
        CallPosition* cp = (CallPosition*)Vector_Get(&ll->ev.cs,i);
        if(cp->type==TOKEN_LUALIKEIR_NAMESPACE){
            String_Append(&builder,cp->fname);
            String_Append(&builder,"::");
        }
    }
    String_Append(&builder,name);

    CStr cstr = String_CStr(&builder);
    String_Free(&builder);
    return cstr;
}
CStr LuaLikeIR_FuncSpaceName(LuaLikeIR* ll,CStr name){
    String builder = String_New();

    for(int i = 0;i<ll->ev.cs.size;i++){
        CallPosition* cp = (CallPosition*)Vector_Get(&ll->ev.cs,i);
        if(cp->type==TOKEN_LUALIKEIR_IMPL){
            String_Append(&builder,cp->fname);
            String_Append(&builder,"::");
            break;
        }
    }

    if(builder.size == 0){
        for(int i = 0;i<ll->ev.cs.size;i++){
            CallPosition* cp = (CallPosition*)Vector_Get(&ll->ev.cs,i);
            if(cp->type==TOKEN_LUALIKEIR_NAMESPACE){
                String_Append(&builder,cp->fname);
                String_Append(&builder,"::");
            }
        }
    }

    String_Append(&builder,name);

    CStr cstr = String_CStr(&builder);
    String_Free(&builder);
    return cstr;
}
Function* LuaLikeIR_FunctionIn(LuaLikeIR* ll) {
    for(int i = ll->ev.cs.size - 1;i>=0;i--){
        CallPosition* cp = Vector_Get(&ll->ev.cs,i);
        
        if(cp && cp->type==TOKEN_FUNCTIONDECL){
            TT_Iter it = FunctionMap_Find(&ll->ev.fs,cp->fname);
            Function* f = (Function*)Vector_Get(&ll->ev.fs,it);
            return f;
        }
    }
    return NULL;
}

void LuaLikeIR_LogicAddPath(LuaLikeIR* ll){
    if(ll->ev.sc.range>=ll->logicpath.size){
        while(ll->ev.sc.range>=ll->logicpath.size) Vector_Push(&ll->logicpath,(LogicBlock[]){ 0,0 });
    }else{
        LogicBlock* r = (LogicBlock*)Vector_Get(&ll->logicpath,ll->ev.sc.range);
        r->count++;
    }
}
void LuaLikeIR_LogicAddExtend(LuaLikeIR* ll){
    if(ll->ev.sc.range>=ll->logicpath.size){
        while(ll->ev.sc.range>=ll->logicpath.size) Vector_Push(&ll->logicpath,(LogicBlock[]){ 0,0 });
    }else{
        LogicBlock* r = (LogicBlock*)Vector_Get(&ll->logicpath,ll->ev.sc.range);
        r->extend++;
    }
}
int LuaLikeIR_GetLogicPath(LuaLikeIR* ll,Range r){
    if(r<0){
        return -1;
    }
    if(r>=ll->logicpath.size){
        while(ll->ev.sc.range>=ll->logicpath.size) Vector_Push(&ll->logicpath,(LogicBlock[]){ 0,0 });
        return 0;
    }
    return ((LogicBlock*)Vector_Get(&ll->logicpath,r))->count;
}
int LuaLikeIR_GetLogicExtend(LuaLikeIR* ll,Range r){
    if(r<0){
        return -1;
    }
    if(r>=ll->logicpath.size){
        while(ll->ev.sc.range>=ll->logicpath.size) Vector_Push(&ll->logicpath,(LogicBlock[]){ 0,0 });
        return 0;
    }
    return ((LogicBlock*)Vector_Get(&ll->logicpath,r))->extend;
}
CStr LuaLikeIR_Logic(LuaLikeIR* ll,char se,CStr type,Range r){
    int lp = LuaLikeIR_GetLogicPath(ll,ll->ev.sc.range);
    if(CStr_Cmp(type,"ELIF")){
        int ex = LuaLikeIR_GetLogicExtend(ll,ll->ev.sc.range);
        return CStr_Format("l.%d_%s%d_%d_%d",se,type,r,lp,ex);
    }
    return CStr_Format("l.%d_%s%d_%d",se,type,r,lp);
}
void LuaLikeIR_LogicCorrection(LuaLikeIR* ll,TokenMap* tm){
    Token* t = (Token*)Vector_Get(tm,0);
    
    if(ll->logic==TOKEN_LUALIKEIR_IF){
        int lp = LuaLikeIR_GetLogicPath(ll,ll->ev.sc.range);
        if(t->tt==TOKEN_LUALIKEIR_ELIF || t->tt==TOKEN_LUALIKEIR_ELSE){
            CStr log_label = LuaLikeIR_Logic(ll,1,LUALIKEIR_LOG,ll->ev.sc.range);
            LuaLikeIR_Indentation_Appendf(ll,&ll->text,"jmp %s",log_label);
            CStr_Free(&log_label);

            CStr if_label = LuaLikeIR_Logic(ll,1,LUALIKEIR_IF,ll->ev.sc.range);
            LuaLikeIR_Indentation_Appendf(ll,&ll->text,"%s:",if_label);
            CStr_Free(&if_label);
        }else{
            CStr log_label = LuaLikeIR_Logic(ll,1,LUALIKEIR_LOG,ll->ev.sc.range);
            LuaLikeIR_Indentation_Appendf(ll,&ll->text,"%s:",log_label);
            CStr_Free(&log_label);
        }
    }else if(ll->logic==TOKEN_LUALIKEIR_ELIF){
        int lp = LuaLikeIR_GetLogicPath(ll,ll->ev.sc.range);
        if(t->tt==TOKEN_LUALIKEIR_ELIF || t->tt==TOKEN_LUALIKEIR_ELSE){
            CStr log_label = LuaLikeIR_Logic(ll,1,LUALIKEIR_LOG,ll->ev.sc.range);
            LuaLikeIR_Indentation_Appendf(ll,&ll->text,"jmp %s",log_label);
            CStr_Free(&log_label);

            CStr if_label = LuaLikeIR_Logic(ll,1,LUALIKEIR_ELIF,ll->ev.sc.range);
            LuaLikeIR_Indentation_Appendf(ll,&ll->text,"%s:",if_label);
            CStr_Free(&if_label);
        }else{
            CStr log_label = LuaLikeIR_Logic(ll,1,LUALIKEIR_LOG,ll->ev.sc.range);
            LuaLikeIR_Indentation_Appendf(ll,&ll->text,"%s:",log_label);
            CStr_Free(&log_label);
        }
    }else if(ll->logic==TOKEN_LUALIKEIR_ELSE){
        int lp = LuaLikeIR_GetLogicPath(ll,ll->ev.sc.range);
        CStr log_label = LuaLikeIR_Logic(ll,1,LUALIKEIR_LOG,ll->ev.sc.range);
        LuaLikeIR_Indentation_Appendf(ll,&ll->text,"%s:",log_label);
        CStr_Free(&log_label);
    }else if(ll->logic==TOKEN_LUALIKEIR_WHILE){
        int lp = LuaLikeIR_GetLogicPath(ll,ll->ev.sc.range);
        CStr start_label = LuaLikeIR_Logic(ll,0,LUALIKEIR_WHILE,ll->ev.sc.range);
        CStr end_label = LuaLikeIR_Logic(ll,1,LUALIKEIR_WHILE,ll->ev.sc.range);
        LuaLikeIR_Indentation_Appendf(ll,&ll->text,"jmp %s",start_label);
        LuaLikeIR_Indentation_Appendf(ll,&ll->text,"%s:",end_label);
        CStr_Free(&end_label);
        CStr_Free(&start_label);
    }else if(ll->logic==TOKEN_LUALIKEIR_FOR){
        int lp = LuaLikeIR_GetLogicPath(ll,ll->ev.sc.range);
        CStr start_label = LuaLikeIR_Logic(ll,0,LUALIKEIR_FOR,ll->ev.sc.range);
        CStr end_label = LuaLikeIR_Logic(ll,1,LUALIKEIR_FOR,ll->ev.sc.range);
        LuaLikeIR_Indentation_Appendf(ll,&ll->text,"jmp %s",start_label);
        LuaLikeIR_Indentation_Appendf(ll,&ll->text,"%s:",end_label);
        CStr_Free(&end_label);
        CStr_Free(&start_label);
        Scope_Pop(&ll->ev.sc);// first possible decl in for ... , 
    }

    ll->logic = TOKEN_NONE;
}

Boolean LuaLikeIR_TypeInt(CStr typename){
    return
    CStr_Cmp(typename,I8_TYPE) || CStr_Cmp(typename,I16_TYPE) ||
    CStr_Cmp(typename,I32_TYPE) || CStr_Cmp(typename,I64_TYPE) ||
    CStr_Cmp(typename,U8_TYPE) || CStr_Cmp(typename,U16_TYPE) ||
    CStr_Cmp(typename,U32_TYPE) || CStr_Cmp(typename,U64_TYPE);
}
Boolean LuaLikeIR_TypeFloat(CStr typename){
    return CStr_Cmp(typename,F32_TYPE) || CStr_Cmp(typename,F64_TYPE);
}

Number LuaLikeIR_Function_Add(Number a,Number b){
    return a + b;
}
Number LuaLikeIR_Function_Sub(Number a,Number b){
    return a - b;
}
Number LuaLikeIR_Function_Mul(Number a,Number b){
    return a * b;
}
Number LuaLikeIR_Function_Div(Number a,Number b){
    return a / b;
}
Number LuaLikeIR_Function_Mod(Number a,Number b){
    return a % b;
}
Number LuaLikeIR_Function_And(Number a,Number b){
    return a & b;
}
Number LuaLikeIR_Function_Or(Number a,Number b){
    return a | b;
}
Number LuaLikeIR_Function_Xor(Number a,Number b){
    return a ^ b;
}
Number LuaLikeIR_Function_Not(Number a){
    return ~a;
}
Number LuaLikeIR_Function_Neg(Number a){
    return -a;
}
Boolean LuaLikeIR_Function_Equ(Number a,Number b){
    return a == b;
}
Boolean LuaLikeIR_Function_Neq(Number a,Number b){
    return a != b;
}
Boolean LuaLikeIR_Function_Les(Number a,Number b){
    return a < b;
}
Boolean LuaLikeIR_Function_Grt(Number a,Number b){
    return a > b;
}
Boolean LuaLikeIR_Function_Leq(Number a,Number b){
    return a <= b;
}
Boolean LuaLikeIR_Function_Grq(Number a,Number b){
    return a >= b;
}
Boolean LuaLikeIR_Function_Lnd(Boolean a,Boolean b){
    return a && b;
}
Boolean LuaLikeIR_Function_Lor(Boolean a,Boolean b){
    return a || b;
}
Boolean LuaLikeIR_Function_Lot(Boolean a){
    return !a;
}

void LuaLikeIR_Destroyer(Variable* v){
    //printf("[LuaLikeIR]: Destroyer: %s -> ",v->typename);
    LuaLikeIRVariable* v_sv = (LuaLikeIRVariable*)Variable_Data(v);
    if(v_sv->destroy==0){
        LuaLikeIR_Variable_Destroy_Ref_Decl(v_sv->parent,v->name);
        //printf("Ref -> %s -> %d\n",v->name,((LuaLikeIR*)v_sv->parent)->stack);
    }else if(v_sv->destroy==1){
        LuaLikeIR_Variable_Destroy_Decl(v_sv->parent,v->name);
        //printf("Real -> %s -> %d\n",v->name,((LuaLikeIR*)v_sv->parent)->stack);
    }else if(v_sv->destroy==2){
        LuaLikeIR_Variable_Destroy_Use(v_sv->parent,v->name);
        //printf("Use -> %s -> %d -> %d\n",v->name,v_sv->stack,((LuaLikeIR*)v_sv->parent)->stack);
    }
    LuaLikeIRVariable_Free(v_sv);
}
void LuaLikeIR_Cpyer(Variable* src,Variable* dst){
    //printf("[LuaLikeIR]: Cpyer!\n");
    //LuaLikeIRVariable* src_str = (LuaLikeIRVariable*)Variable_Data(src);
    //LuaLikeIRVariable* dst_str = (LuaLikeIRVariable*)Variable_Data(dst);
    //*dst_str = VariableMap_Cpy(src_str);
}
Token LuaLikeIR_Init(LuaLikeIR* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    //printf("[LuaLikeIR]: INIT: %s\n",a->str);
    
    Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
    if(v){
        LuaLikeIRVariable* sv = (LuaLikeIRVariable*)Variable_Data(v);
        int size = LuaLikeIR_Size(ll,v->typename);
        
        if(size>0){
            LuaLikeIR_Indentation_Appendf(ll,&ll->text,"sub rsp,%d",size);
        }
        *sv = LuaLikeIRVariable_New(1,ll);
    }
    return Token_Cpy(a);
}

Double LuaLikeIR_GetFloat(LuaLikeIR* ll,CStr fstr){
    return Double_Parse(fstr,0);
}
CStr LuaLikeIR_GetFloatStr(LuaLikeIR* ll,CStr fstr){
    Double d = LuaLikeIR_GetFloat(ll,fstr);
    CStr cstr = Number_Get(*((Number*)&d));
    return cstr;
}
Token LuaLikeIR_GetFloatToken(LuaLikeIR* ll,CStr fstr){
    return Token_Move(TOKEN_FLOAT,LuaLikeIR_GetFloatStr(ll,fstr));
}

CStr LuaLikeIR_FInstOf(LuaLikeIR* ll,Token* a,CStr inst){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            if(LuaLikeIR_TypeFloat(v->typename)){
                return CStr_Format("f%s",inst);
            }else if(LuaLikeIR_TypeInt(v->typename)){
                return CStr_Format("fi%s",inst);
            }
        }
    }else if(a->tt==TOKEN_FLOAT){
        return CStr_Format("f%s",inst);
    }else if(a->tt==TOKEN_NUMBER){
        return CStr_Format("fi%s",inst);
    }
    return NULL;
}

Boolean LuaLikeIR_Extract(LuaLikeIR* ll,Token* a,Number* n){
    if(a->tt==TOKEN_STRING){
        return False;
    }else if(a->tt==TOKEN_NUMBER){
        *n = Number_Parse(a->str);
        return True;
    }else{
        Compiler_ErrorHandler(&ll->ev,"Number -> 1. Arg: %s is not a int type!",a->str);
        return False;
    }
}
Boolean LuaLikeIR_ExtractBool(LuaLikeIR* ll,Token* a,Boolean* b){
    if(a->tt==TOKEN_STRING){
        return False;
    }else if(a->tt==TOKEN_LUALIKEIR_BOOLEAN){
        *b = Boolean_Parse(a->str);
        return True;
    }else{
        Compiler_ErrorHandler(&ll->ev,"1. Arg: %s is not a bool type!",a->str);
        return False;
    }
}

Boolean LuaLikeIR_ErrorsInArg(LuaLikeIR* ll,Token* a){
    if(a->tt==TOKEN_NONE){
        Compiler_ErrorHandler(&ll->ev,"Errors -> Tokentype of %s is NONE!",a->str);
        return 1;
    }
    if(a->tt==TOKEN_CONSTSTRING_DOUBLE){
        if(a->str == NULL){
            Compiler_ErrorHandler(&ll->ev,"Errors -> const str %s is null!",a->str);
            return 1; 
        }
    }
    if(a->tt==TOKEN_NUMBER){
        if(Number_Parse(a->str) == NUMBER_PARSE_ERROR){
            Compiler_ErrorHandler(&ll->ev,"Errors -> number %s is invalid!",a->str);
            return 1;
        }
    }
    if(a->tt==TOKEN_FLOAT){
        if(Double_Parse(a->str,0) == DOUBLE_PARSE_ERROR){
            Compiler_ErrorHandler(&ll->ev,"Errors -> float %s is invalid!",a->str);
            return 1;
        }
    }
    if(a->tt==TOKEN_LUALIKEIR_BOOLEAN){
        if(Boolean_Parse(a->str) == BOOL_PARSE_ERROR){
            Compiler_ErrorHandler(&ll->ev,"Errors -> bool %s is invalid!",a->str);
            return 1;
        }
    }
    if(a->tt==TOKEN_STRING){
        if(Scope_FindVariable(&ll->ev.sc,a->str) == NULL){
            Compiler_ErrorHandler(&ll->ev,"Errors -> variable %s doesn't exist!",a->str);
            return 1;
        }
    }
    if(a->tt==TOKEN_FUNCTIONPOINTER){
        if(Scope_FindVariable(&ll->ev.sc,a->str) == NULL){
            Compiler_ErrorHandler(&ll->ev,"Errors -> function %s is null!",a->str);
            return 1;
        }
    }
    return 0;
}

void LuaLikeIR_DrefIntoReg(LuaLikeIR* ll,Token* a,CStr reg){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            if(LuaLikeIR_DrefType(ll,v->typename)){
                CStr location = LuaLikeIR_Location(ll,a->str);
                LuaLikeIR_Indentation_Appendf(ll,&ll->text,"mov %s,%s",reg,location);
                CStr typename = CStr_Cpy(v->typename);
                int drefs = LuaLikeIR_DrefTypeCount(ll,typename);
                for(int i = 0;i<drefs-1;i++){
                    CStr typeselector = NULL;
                    if(typename[CStr_Size(typename) - 1]=='&')  typeselector = CStr_Cpy(LUALIKEIR_DREF_64);
                    else                                        typeselector = LuaLikeIR_TypeSelector_T(ll,typename);

                    LuaLikeIR_Indentation_Appendf(ll,&ll->text,"mov %s,%s[%s]",reg,typeselector,reg);
                    CStr_Free(&typeselector);

                    CStr newtypename = CStr_PopOff(typename);
                    CStr_Set(&typename,newtypename);
                    CStr_Free(&newtypename);
                }
                CStr_Free(&location);
            }else{
                Compiler_ErrorHandler(&ll->ev,"IntoReg -> Error: %s is not a var!",a->str);
            }
        }else{
            Compiler_ErrorHandler(&ll->ev,"DrefIntoReg -> Error: %s is not a dref var!",a->str);
        }
    }
}
void LuaLikeIR_IntoReg(LuaLikeIR* ll,Token* a,CStr reg){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            CStr location = LuaLikeIR_Location(ll,a->str);
        
            if(LuaLikeIR_DrefType(ll,v->typename)){
                LuaLikeIR_DrefIntoReg(ll,a,LUALIKEIR_REG_D_64);
                
                CStr typename = LuaLikeIR_TypeOfDref(ll,v->typename);
                CStr typeselector = LuaLikeIR_TypeSelector_T(ll,typename);
                LuaLikeIR_Indentation_Appendf(ll,&ll->text,"mov %s,%s[%s]",reg,typeselector,LUALIKEIR_REG_D_64);
                CStr_Free(&typename);
                CStr_Free(&typeselector);
            }else{
                LuaLikeIR_Indentation_Appendf(ll,&ll->text,"mov %s,%s",reg,location);
            }
            CStr_Free(&location);
        }else{
            Compiler_ErrorHandler(&ll->ev,"IntoReg -> Error: %s is not a var!",a->str);
        }
    }else{
        LuaLikeIR_Indentation_Appendf(ll,&ll->text,"mov %s,%s",reg,a->str);
    }
}
void LuaLikeIR_IntoSet(LuaLikeIR* ll,Token* a,CStr reg){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            CStr location = LuaLikeIR_Location(ll,a->str);
        
            if(LuaLikeIR_DrefType(ll,v->typename)){
                LuaLikeIR_DrefIntoReg(ll,a,LUALIKEIR_REG_D_64);
                
                CStr typename = LuaLikeIR_TypeOfDref(ll,v->typename);
                CStr typeselector = LuaLikeIR_TypeSelector_T(ll,typename);
                LuaLikeIR_Indentation_Appendf(ll,&ll->text,"mov %s[%s],%s",typeselector,LUALIKEIR_REG_D_64,reg);
                CStr_Free(&typename);
                CStr_Free(&typeselector);
            }else{
                LuaLikeIR_Indentation_Appendf(ll,&ll->text,"mov %s,%s",location,reg);
            }
            CStr_Free(&location);
        }else{
            Compiler_ErrorHandler(&ll->ev,"IntoSet -> Error: %s is not a var!",a->str);
        }
    }else{
        Compiler_ErrorHandler(&ll->ev,"IntoSet -> Error: %s is not a var but assigned!",a->str);
    }
}
void LuaLikeIR_AtReg(LuaLikeIR* ll,Token* a,CStr reg,CStr inst){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            CStr location = LuaLikeIR_Location(ll,a->str);
            
            if(LuaLikeIR_DrefType(ll,v->typename)){
                LuaLikeIR_DrefIntoReg(ll,a,LUALIKEIR_REG_D_64);
                
                CStr typename = LuaLikeIR_TypeOfDref(ll,v->typename);
                CStr typeselector = LuaLikeIR_TypeSelector_T(ll,typename);
                LuaLikeIR_Indentation_Appendf(ll,&ll->text,"%s %s[%s],%s",inst,typeselector,LUALIKEIR_REG_D_64,reg);
                CStr_Free(&typename);
                CStr_Free(&typeselector);
            }else{
                LuaLikeIR_Indentation_Appendf(ll,&ll->text,"%s %s,%s",inst,reg,location);
            }
            CStr_Free(&location);
        }else{
            Compiler_ErrorHandler(&ll->ev,"AtReg -> Error: %s is not a var!",a->str);
        }
    }else{
        LuaLikeIR_Indentation_Appendf(ll,&ll->text,"%s %s,%s",inst,reg,a->str);
    }
}
void LuaLikeIR_AtRegSingle(LuaLikeIR* ll,Token* a,CStr inst){//Always A ex: mul,div etc
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            CStr location = LuaLikeIR_Location(ll,a->str);
            
            if(LuaLikeIR_DrefType(ll,v->typename)){
                LuaLikeIR_DrefIntoReg(ll,a,LUALIKEIR_REG_D_64);
                
                CStr typename = LuaLikeIR_TypeOfDref(ll,v->typename);
                CStr typeselector = LuaLikeIR_TypeSelector_T(ll,typename);
                LuaLikeIR_Indentation_Appendf(ll,&ll->text,"%s %s[%s]",inst,typeselector,LUALIKEIR_REG_D_64);
                CStr_Free(&typename);
                CStr_Free(&typeselector);
            }else{
                LuaLikeIR_Indentation_Appendf(ll,&ll->text,"%s %s",inst,location);
            }
            CStr_Free(&location);
        }else{
            Compiler_ErrorHandler(&ll->ev,"AtRegSingle -> Error: %s is not a var!",a->str);
        }
    }else{
        LuaLikeIR_Indentation_Appendf(ll,&ll->text,"%s %s",inst,a->str);
    }
}
void LuaLikeIR_AtSingle(LuaLikeIR* ll,CStr reg,CStr inst){//Always A ex: mul,div etc
    LuaLikeIR_Indentation_Appendf(ll,&ll->text,"%s %s",inst,reg);
}
void LuaLikeIR_AtSet(LuaLikeIR* ll,Token* a,CStr reg,CStr inst){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            CStr location = LuaLikeIR_Location(ll,a->str);
            
            if(LuaLikeIR_DrefType(ll,v->typename)){
                LuaLikeIR_DrefIntoReg(ll,a,LUALIKEIR_REG_D_64);
                
                CStr typename = LuaLikeIR_TypeOfDref(ll,v->typename);
                CStr typeselector = LuaLikeIR_TypeSelector_T(ll,typename);
                LuaLikeIR_Indentation_Appendf(ll,&ll->text,"%s %s[%s],%s",inst,typeselector,LUALIKEIR_REG_D_64,reg);
                CStr_Free(&typename);
                CStr_Free(&typeselector);
            }else{
                LuaLikeIR_Indentation_Appendf(ll,&ll->text,"%s %s,%s",inst,location,reg);
            }
            CStr_Free(&location);
        }else{
            Compiler_ErrorHandler(&ll->ev,"AtSet -> Error: %s is not a var!",a->str);
        }
    }else{
        Compiler_ErrorHandler(&ll->ev,"Set -> Error: %s is not a var but assigned!",a->str);
    }
}
void LuaLikeIR_CmpAtReg(LuaLikeIR* ll,Token* a,CStr reg){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        CStr location = LuaLikeIR_Location(ll,a->str);
        
        if(LuaLikeIR_DrefType(ll,v->typename)){
            LuaLikeIR_DrefIntoReg(ll,a,LUALIKEIR_REG_D_64);
                
            CStr typename = LuaLikeIR_TypeOfDref(ll,v->typename);
            CStr typeselector = LuaLikeIR_TypeSelector_T(ll,typename);
            LuaLikeIR_Indentation_Appendf(ll,&ll->text,"cmp %s[%s],%s",typeselector,LUALIKEIR_REG_D_64,reg);
            CStr_Free(&typename);
            CStr_Free(&typeselector);
        }else{
            LuaLikeIR_Indentation_Appendf(ll,&ll->text,"cmp %s,%s",reg,location);
        }
        CStr_Free(&location);
    }else{
        LuaLikeIR_Indentation_Appendf(ll,&ll->text,"cmp %s,%s",reg,a->str);
    }
}
void LuaLikeIR_CmpAtSet(LuaLikeIR* ll,Token* a,CStr inst){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        CStr location = LuaLikeIR_Location(ll,a->str);
        
        if(LuaLikeIR_DrefType(ll,v->typename)){
            LuaLikeIR_DrefIntoReg(ll,a,LUALIKEIR_REG_D_64);
                
            CStr typename = LuaLikeIR_TypeOfDref(ll,v->typename);
            CStr typeselector = LuaLikeIR_TypeSelector_T(ll,typename);
            LuaLikeIR_Indentation_Appendf(ll,&ll->text,"%s %s[%s]",inst,typeselector,LUALIKEIR_REG_D_64);
            CStr_Free(&typename);
            CStr_Free(&typeselector);
        }else{
            LuaLikeIR_Indentation_Appendf(ll,&ll->text,"%s %s",inst,location);
        }
        CStr_Free(&location);
    }else{
        Compiler_ErrorHandler(&ll->ev,"Set -> Error: %s is not a var but assigned!",a->str);
    }
}

Token LuaLikeIR_ExecuteAss(LuaLikeIR* ll,Token* a,Token* b,Token* op,CStr instname,CStr instnameupper){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(LuaLikeIR_ErrorsInArg(ll,a)) return Token_Null();
    if(LuaLikeIR_ErrorsInArg(ll,b)) return Token_Null();

    if(b->tt==TOKEN_NUMBER){
        LuaLikeIR_AtSet(ll,a,b->str,instname);
        return Token_Cpy(a);
    }else if(b->tt==TOKEN_FLOAT){
        CStr fstr = LuaLikeIR_GetFloatStr(ll,b->str);
        LuaLikeIR_Indentation_Appendf(ll,&ll->text,"mov %s,%s",LUALIKEIR_REG_A_64,fstr);
        LuaLikeIR_AtSet(ll,a,LUALIKEIR_REG_A_64,instname);
        CStr_Free(&fstr);
        return Token_Cpy(a);
    }else{
        int realsize_a = LuaLikeIR_TypeRealSize(ll,a);
        int realsize_b = LuaLikeIR_TypeRealSize(ll,b);
        
        if(realsize_b>realsize_a)
            LuaLikeIR_Indentation_Appendf(ll,&ll->text,"mov %s,0",LUALIKEIR_REG_A_64);

        LuaLikeIR_IntoReg(ll,b,LuaLikeIR_SelectRT(ll,realsize_b)[LUALIKEIR_REG_A]);
        LuaLikeIR_AtSet(ll,a,LuaLikeIR_SelectRT(ll,realsize_a)[LUALIKEIR_REG_A],instname);
        return Token_Cpy(a);
    }
}
Token LuaLikeIR_ExecuteAssA(LuaLikeIR* ll,Token* a,Token* b,Token* op,CStr instname,CStr instnameupper){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(LuaLikeIR_ErrorsInArg(ll,a)) return Token_Null();
    if(LuaLikeIR_ErrorsInArg(ll,b)) return Token_Null();

    int realsize_a = LuaLikeIR_TypeRealSize(ll,a);
    int realsize_b = LuaLikeIR_TypeRealSize(ll,b);
        
    if(realsize_b>realsize_a)
        LuaLikeIR_Indentation_Appendf(ll,&ll->text,"mov %s,0",LUALIKEIR_REG_A_64);

    LuaLikeIR_IntoReg(ll,a,LuaLikeIR_SelectRT(ll,realsize_a)[LUALIKEIR_REG_A]);
    LuaLikeIR_IntoReg(ll,b,LuaLikeIR_SelectRT(ll,realsize_b)[LUALIKEIR_REG_B]);
    LuaLikeIR_AtSingle(ll,LuaLikeIR_SelectRT(ll,realsize_b)[LUALIKEIR_REG_B],instname);
    LuaLikeIR_IntoSet(ll,a,LuaLikeIR_SelectRT(ll,realsize_a)[LUALIKEIR_REG_A]);
    return Token_Cpy(a);
}
Token LuaLikeIR_ExecuteAssSingle(LuaLikeIR* ll,Token* a,Token* op,CStr instname,CStr instnameupper){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(LuaLikeIR_ErrorsInArg(ll,a)) return Token_Null();

    LuaLikeIR_AtRegSingle(ll,a,instname);
    return Token_Cpy(a);
}

Token LuaLikeIR_Execute(LuaLikeIR* ll,Token* a,Token* b,Token* op,CStr instname,CStr instnameupper,Number (*inst)(Number,Number)){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(LuaLikeIR_ErrorsInArg(ll,a)) return Token_Null();
    if(LuaLikeIR_ErrorsInArg(ll,b)) return Token_Null();

    if(a->tt==TOKEN_NUMBER && b->tt==TOKEN_NUMBER){
        char* resstr = Number_Get(inst(Number_Parse(a->str),Number_Parse(b->str)));
        return Token_Move(TOKEN_NUMBER,resstr);
    }else{
        CStr typename_a = LuaLikeIR_VariableType(ll,a);
        int realsize_a = LuaLikeIR_TypeRealSize(ll,a);
        int realsize_b = LuaLikeIR_TypeRealSize(ll,b);
        
        CStr stack_name = LuaLikeIR_Variablename_Next(ll,".STACK",6);
        LuaLikeIR_Variable_Build_Decl(ll,stack_name,typename_a);
        CStr_Free(&typename_a);
        Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        
        if(realsize_b>realsize_a)
            LuaLikeIR_Indentation_Appendf(ll,&ll->text,"mov %s,0",LUALIKEIR_REG_A_64);

        LuaLikeIR_IntoReg(ll,a,LuaLikeIR_SelectRT(ll,realsize_a)[LUALIKEIR_REG_A]);
        LuaLikeIR_AtReg(ll,b,LuaLikeIR_SelectRT(ll,realsize_b)[LUALIKEIR_REG_A],instname);
        LuaLikeIR_IntoSet(ll,&stack_t,LuaLikeIR_SelectRT(ll,realsize_a)[LUALIKEIR_REG_A]);
        return stack_t;
    }
}
Token LuaLikeIR_ExecuteA(LuaLikeIR* ll,Token* a,Token* b,Token* op,CStr instname,CStr instnameupper,Number (*inst)(Number,Number)){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(LuaLikeIR_ErrorsInArg(ll,a)) return Token_Null();
    if(LuaLikeIR_ErrorsInArg(ll,b)) return Token_Null();

    if(a->tt==TOKEN_NUMBER && b->tt==TOKEN_NUMBER){
        char* resstr = Number_Get(inst(Number_Parse(a->str),Number_Parse(b->str)));
        return Token_Move(TOKEN_NUMBER,resstr);
    }else{
        if(b->tt==TOKEN_NUMBER){
            CStr typename_a = LuaLikeIR_VariableType(ll,a);
            int realsize_a = LuaLikeIR_TypeRealSize(ll,a);
        
            CStr stack_name = LuaLikeIR_Variablename_Next(ll,".STACK",6);
            LuaLikeIR_Variable_Build_Decl(ll,stack_name,typename_a);
            CStr_Free(&typename_a);
            Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        
            LuaLikeIR_IntoReg(ll,a,LuaLikeIR_SelectRT(ll,realsize_a)[LUALIKEIR_REG_A]);
            LuaLikeIR_IntoReg(ll,b,LUALIKEIR_REG_B_64);
            LuaLikeIR_AtSingle(ll,LUALIKEIR_REG_B_64,instname);
            LuaLikeIR_IntoSet(ll,&stack_t,LuaLikeIR_SelectRT(ll,realsize_a)[LUALIKEIR_REG_A]);
            return stack_t;
        }else{
            CStr typename_a = LuaLikeIR_VariableType(ll,a);
            int realsize_a = LuaLikeIR_TypeRealSize(ll,a);
        
            CStr stack_name = LuaLikeIR_Variablename_Next(ll,".STACK",6);
            LuaLikeIR_Variable_Build_Decl(ll,stack_name,typename_a);
            CStr_Free(&typename_a);
            Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        
            LuaLikeIR_IntoReg(ll,a,LuaLikeIR_SelectRT(ll,realsize_a)[LUALIKEIR_REG_A]);
            LuaLikeIR_AtRegSingle(ll,b,instname);
            LuaLikeIR_IntoSet(ll,&stack_t,LuaLikeIR_SelectRT(ll,realsize_a)[LUALIKEIR_REG_A]);
            return stack_t;
        }
    }
}
Token LuaLikeIR_ExecuteAR(LuaLikeIR* ll,Token* a,Token* b,Token* op,CStr instname,CStr instnameupper,Number (*inst)(Number,Number)){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(LuaLikeIR_ErrorsInArg(ll,a)) return Token_Null();
    if(LuaLikeIR_ErrorsInArg(ll,b)) return Token_Null();

    if(a->tt==TOKEN_NUMBER && b->tt==TOKEN_NUMBER){
        char* resstr = Number_Get(inst(Number_Parse(a->str),Number_Parse(b->str)));
        return Token_Move(TOKEN_NUMBER,resstr);
    }else{
        if(b->tt==TOKEN_NUMBER){
            CStr typename_a = LuaLikeIR_VariableType(ll,a);
            int realsize_a = LuaLikeIR_TypeRealSize(ll,a);
        
            CStr stack_name = LuaLikeIR_Variablename_Next(ll,".STACK",6);
            LuaLikeIR_Variable_Build_Decl(ll,stack_name,typename_a);
            CStr_Free(&typename_a);
            Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        
            LuaLikeIR_IntoReg(ll,a,LuaLikeIR_SelectRT(ll,realsize_a)[LUALIKEIR_REG_A]);
            LuaLikeIR_IntoReg(ll,b,LUALIKEIR_REG_B_64);
            LuaLikeIR_AtSingle(ll,LUALIKEIR_REG_B_64,instname);
            LuaLikeIR_IntoSet(ll,&stack_t,LuaLikeIR_SelectRT(ll,realsize_a)[LUALIKEIR_REG_D]);
            return stack_t;
        }else{
            CStr typename_a = LuaLikeIR_VariableType(ll,a);
            int realsize_a = LuaLikeIR_TypeRealSize(ll,a);
        
            CStr stack_name = LuaLikeIR_Variablename_Next(ll,".STACK",6);
            LuaLikeIR_Variable_Build_Decl(ll,stack_name,typename_a);
            CStr_Free(&typename_a);
            Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        
            LuaLikeIR_IntoReg(ll,a,LuaLikeIR_SelectRT(ll,realsize_a)[LUALIKEIR_REG_A]);
            LuaLikeIR_AtRegSingle(ll,b,instname);
            LuaLikeIR_IntoSet(ll,&stack_t,LuaLikeIR_SelectRT(ll,realsize_a)[LUALIKEIR_REG_D]);
            return stack_t;
        }
    }
}
Token LuaLikeIR_ExecuteSingle(LuaLikeIR* ll,Token* a,Token* op,CStr instname,CStr instnameupper,Number (*inst)(Number)){
    //Compiler_InfoHandler(&ll->ev,"%s: %s%s",instnameupper,op->str,a->str);
    
    if(LuaLikeIR_ErrorsInArg(ll,a)) return Token_Null();

    if(a->tt==TOKEN_NUMBER){
        char* resstr = Number_Get(inst(Number_Parse(a->str)));
        return Token_Move(TOKEN_NUMBER,resstr);
    }else{
        CStr typename_a = LuaLikeIR_VariableType(ll,a);
        int realsize_a = LuaLikeIR_TypeRealSize(ll,a);
        
        CStr stack_name = LuaLikeIR_Variablename_Next(ll,".STACK",6);
        LuaLikeIR_Variable_Build_Decl(ll,stack_name,typename_a);
        CStr_Free(&typename_a);
        Token stack_t = Token_Move(TOKEN_STRING,stack_name);

        LuaLikeIR_IntoReg(ll,a,LuaLikeIR_SelectRT(ll,realsize_a)[LUALIKEIR_REG_A]);
        LuaLikeIR_AtSingle(ll,LuaLikeIR_SelectRT(ll,realsize_a)[LUALIKEIR_REG_A],instname);
        LuaLikeIR_IntoSet(ll,&stack_t,LuaLikeIR_SelectRT(ll,realsize_a)[LUALIKEIR_REG_A]);
        
        return stack_t;
    }
}
Token LuaLikeIR_ExecuteCmp(LuaLikeIR* ll,Token* a,Token* b,Token* op,CStr instname,CStr instnameupper,Boolean (*inst)(Number,Number)){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(LuaLikeIR_ErrorsInArg(ll,a)) return Token_Null();
    if(LuaLikeIR_ErrorsInArg(ll,b)) return Token_Null();

    if(a->tt==TOKEN_NUMBER && b->tt==TOKEN_NUMBER){
        char* resstr = Boolean_Get(inst(Number_Parse(a->str),Number_Parse(b->str)));
        return Token_Move(TOKEN_LUALIKEIR_BOOLEAN,resstr);
    }else{
        int realsize_a = LuaLikeIR_TypeRealSize(ll,a);
        int realsize_b = LuaLikeIR_TypeRealSize(ll,b);
        
        CStr stack_name = LuaLikeIR_Variablename_Next(ll,".STACK",6);
        LuaLikeIR_Variable_Build_Decl(ll,stack_name,BOOL_TYPE);
        Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        
        if(realsize_b>realsize_a)
            LuaLikeIR_Indentation_Appendf(ll,&ll->text,"mov %s,0",LUALIKEIR_REG_A_64);

        LuaLikeIR_IntoReg(ll,a,LuaLikeIR_SelectRT(ll,realsize_a)[LUALIKEIR_REG_A]);
        LuaLikeIR_CmpAtReg(ll,b,LuaLikeIR_SelectRT(ll,realsize_b)[LUALIKEIR_REG_A]);
        LuaLikeIR_CmpAtSet(ll,&stack_t,instname);
        return stack_t;
    }
}
*/
#endif //!LUALIKEIRASM