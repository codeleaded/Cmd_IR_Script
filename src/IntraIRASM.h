#ifndef INTRAIRASM
#define INTRAIRASM

#include "/home/codeleaded/System/Static/Library/AlxScope.h"
#include "/home/codeleaded/System/Static/Library/AlxShutingYard.h"
#include "/home/codeleaded/System/Static/Library/AlxCompiler.h"
#include "/home/codeleaded/System/Static/Library/ConstParser.h"
#include "/home/codeleaded/System/Static/Library/Files.h"
#include "/home/codeleaded/System/Static/Library/String.h"
#include "/home/codeleaded/System/Static/Library/../Container/CVector.h"

#include "IntraIRDefines.h"


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



typedef struct VariableIR{
    void* parent;
} VariableIR;

VariableIR VariableIR_New(void* parent){
    VariableIR vir;
    vir.parent = parent;
    return vir;
}
void VariableIR_Free(VariableIR* vir){
    
}


typedef struct IntraIR {
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
} IntraIR;

int IntraIR_Indentation(IntraIR* ll){
    return I32_Min(1,ll->indent);
}
void IntraIR_Indentation_To(IntraIR* ll,int indent){
    ll->indent = indent;
}

String* IntraIR_FunctionText(IntraIR* ll){
    //Compiler_FunctionIn
    CallPosition* cp = CallStack_FuncPeek(&ll->ev.cs);
    Function* f = FunctionMap_FindF(&ll->ev.fs,cp->fname);
    if(f) return &f->text;
    return NULL;
}
CStr IntraIR_Indentation_CStr(IntraIR* ll){
    String builder = String_New();

    int Indentation = IntraIR_Indentation(ll);
    for(int i = 0;i<Indentation;i++)
        String_Append(&builder,INTRAIR_INDENTATION);

    CStr cstr = String_CStr(&builder);
    String_Free(&builder);
    return cstr;
}
void IntraIR_Indentation_Do(IntraIR* ll,String* str){
    if(IntraIR_Indentation(ll)>0){
        CStr indent = IntraIR_Indentation_CStr(ll);
        String_Append(str,indent);
        CStr_Free(&indent);
    }
}
void IntraIR_Indentation_Append(IntraIR* ll,String* str,char* cstr){
    IntraIR_Indentation_Do(ll,str);
    String_Append(str,cstr);
}
void IntraIR_Indentation_Appendln(IntraIR* ll,String* str,char* cstr){
    IntraIR_Indentation_Do(ll,str);
    String_Append(str,cstr);
    String_AppendChar(str,'\n');
}
void IntraIR_Indentation_Appendf(IntraIR* ll,String* str,char* FormatCStr,...){
    IntraIR_Indentation_Do(ll,str);
    
    va_list args;
    va_start(args,FormatCStr);
    String app = String_FormatA(FormatCStr,args);
    String_AppendString(str,&app);
    String_Free(&app);
    va_end(args);
}
void IntraIR_Indentation_Appendfln(IntraIR* ll,String* str,char* FormatCStr,...){
    IntraIR_Indentation_Do(ll,str);
    
    va_list args;
    va_start(args,FormatCStr);
    String app = String_FormatA(FormatCStr,args);
    String_AppendString(str,&app);
    String_AppendChar(str,'\n');
    String_Free(&app);
    va_end(args);
}

void IntraIR_BuildVariable(IntraIR* ll,CStr name,CStr type){
    Scope_BuildInitVariable(&ll->ev.sc,name,type,(VariableIR[]){ VariableIR_New(ll) });
}

Token VariableIR_Init(VariableIR* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    //printf("[VariableIR]: INIT: %s\n",a->str);
    // Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
    // if(v){
    //     VariableIRVariable* sv = (VariableIRVariable*)Variable_Data(v);
    //     int size = VariableIR_Size(ll,v->typename);
        
    //     if(v->range>0){
    //         if(size>0){
    //             ll->stack += size;
    //             VariableIR_Indentation_Appendf(ll,&ll->text,"sub rsp,%d",size);
    //         }
    //         *sv = VariableIRVariable_New(ll->stack,0,1,ll);
    //     }else{
    //         *sv = VariableIRVariable_New(ll->stack,0,1,ll);
    //         CStr value = VariableIR_BuildGlobal(ll,v->name,size);
    //         sv->global = value;
    //     }
    // }
    return Token_Cpy(a);
}
void VariableIR_Destroyer(Variable* v){
    //printf("[VariableIR]: Destroyer: %s -> ",v->typename);
    VariableIR* v_sv = (VariableIR*)Variable_Data(v);
    if(v_sv->parent){
        IntraIR* ll = (IntraIR*)v_sv->parent;
        
        String* funcstr = IntraIR_FunctionText(ll);
        IntraIR_Indentation_Appendfln(ll,funcstr,"kill\t%s",v->name);
    }
    VariableIR_Free(v_sv);
}
void VariableIR_Cpyer(Variable* src,Variable* dst){
    //printf("[VariableIR]: Cpyer!\n");
    //VariableIRVariable* src_str = (VariableIRVariable*)Variable_Data(src);
    //VariableIRVariable* dst_str = (VariableIRVariable*)Variable_Data(dst);
    //*dst_str = VariableMap_Cpy(src_str);
}

CStr IntraIR_Variablename_Next(IntraIR* ll,CStr name,int offset){
    CStr build = CStr_Concat(name,"*");

    Number stack_name_count = Scope_FindVariablenameLastLike(&ll->ev.sc,build,'*',offset);
    if(stack_name_count==NUMBER_PARSE_ERROR)    stack_name_count = 0;
    else                                        stack_name_count++;


    CStr_Free(&build);
    CStr stack_name = CStr_Format("%s%d",name,stack_name_count);
    return stack_name;
}

void IntraIR_LogicAddPath(IntraIR* ll){
    if(ll->ev.sc.range>=ll->logicpath.size){
        while(ll->ev.sc.range>=ll->logicpath.size) Vector_Push(&ll->logicpath,(LogicBlock[]){ 0,0 });
    }else{
        LogicBlock* r = (LogicBlock*)Vector_Get(&ll->logicpath,ll->ev.sc.range);
        r->count++;
    }
}
void IntraIR_LogicAddExtend(IntraIR* ll){
    if(ll->ev.sc.range>=ll->logicpath.size){
        while(ll->ev.sc.range>=ll->logicpath.size) Vector_Push(&ll->logicpath,(LogicBlock[]){ 0,0 });
    }else{
        LogicBlock* r = (LogicBlock*)Vector_Get(&ll->logicpath,ll->ev.sc.range);
        r->extend++;
    }
}
int IntraIR_GetLogicPath(IntraIR* ll,Range r){
    if(r<0){
        return -1;
    }
    if(r>=ll->logicpath.size){
        while(ll->ev.sc.range>=ll->logicpath.size) Vector_Push(&ll->logicpath,(LogicBlock[]){ 0,0 });
        return 0;
    }
    return ((LogicBlock*)Vector_Get(&ll->logicpath,r))->count;
}
int IntraIR_GetLogicExtend(IntraIR* ll,Range r){
    if(r<0){
        return -1;
    }
    if(r>=ll->logicpath.size){
        while(ll->ev.sc.range>=ll->logicpath.size) Vector_Push(&ll->logicpath,(LogicBlock[]){ 0,0 });
        return 0;
    }
    return ((LogicBlock*)Vector_Get(&ll->logicpath,r))->extend;
}
CStr IntraIR_Logic(IntraIR* ll,char se,CStr type,Range r){
    int lp = IntraIR_GetLogicPath(ll,ll->ev.sc.range);
    if(CStr_Cmp(type,"ELIF")){
        int ex = IntraIR_GetLogicExtend(ll,ll->ev.sc.range);
        return CStr_Format("l.%d_%s%d_%d_%d",se,type,r,lp,ex);
    }
    return CStr_Format("l.%d_%s%d_%d",se,type,r,lp);
}
void IntraIR_LogicCorrection(IntraIR* ll,TokenMap* tm){
    Token* t = (Token*)Vector_Get(tm,0);
    
    if(ll->logic==TOKEN_LUALIKE_IF){
        int lp = IntraIR_GetLogicPath(ll,ll->ev.sc.range);
        if(t->tt==TOKEN_LUALIKE_ELIF || t->tt==TOKEN_LUALIKE_ELSE){
            CStr log_label = IntraIR_Logic(ll,1,INTRAIR_LOG,ll->ev.sc.range);
            IntraIR_Indentation_Appendf(ll,&ll->text,"jmp %s",log_label);
            CStr_Free(&log_label);

            CStr if_label = IntraIR_Logic(ll,1,INTRAIR_IF,ll->ev.sc.range);
            IntraIR_Indentation_Appendf(ll,&ll->text,"%s:",if_label);
            CStr_Free(&if_label);
        }else{
            CStr log_label = IntraIR_Logic(ll,1,INTRAIR_LOG,ll->ev.sc.range);
            IntraIR_Indentation_Appendf(ll,&ll->text,"%s:",log_label);
            CStr_Free(&log_label);
        }
    }else if(ll->logic==TOKEN_LUALIKE_ELIF){
        int lp = IntraIR_GetLogicPath(ll,ll->ev.sc.range);
        if(t->tt==TOKEN_LUALIKE_ELIF || t->tt==TOKEN_LUALIKE_ELSE){
            CStr log_label = IntraIR_Logic(ll,1,INTRAIR_LOG,ll->ev.sc.range);
            IntraIR_Indentation_Appendf(ll,&ll->text,"jmp %s",log_label);
            CStr_Free(&log_label);

            CStr if_label = IntraIR_Logic(ll,1,INTRAIR_ELIF,ll->ev.sc.range);
            IntraIR_Indentation_Appendf(ll,&ll->text,"%s:",if_label);
            CStr_Free(&if_label);
        }else{
            CStr log_label = IntraIR_Logic(ll,1,INTRAIR_LOG,ll->ev.sc.range);
            IntraIR_Indentation_Appendf(ll,&ll->text,"%s:",log_label);
            CStr_Free(&log_label);
        }
    }else if(ll->logic==TOKEN_LUALIKE_ELSE){
        int lp = IntraIR_GetLogicPath(ll,ll->ev.sc.range);
        CStr log_label = IntraIR_Logic(ll,1,INTRAIR_LOG,ll->ev.sc.range);
        IntraIR_Indentation_Appendf(ll,&ll->text,"%s:",log_label);
        CStr_Free(&log_label);
    }else if(ll->logic==TOKEN_LUALIKE_WHILE){
        int lp = IntraIR_GetLogicPath(ll,ll->ev.sc.range);
        CStr start_label = IntraIR_Logic(ll,0,INTRAIR_WHILE,ll->ev.sc.range);
        CStr end_label = IntraIR_Logic(ll,1,INTRAIR_WHILE,ll->ev.sc.range);
        IntraIR_Indentation_Appendf(ll,&ll->text,"jmp %s",start_label);
        IntraIR_Indentation_Appendf(ll,&ll->text,"%s:",end_label);
        CStr_Free(&end_label);
        CStr_Free(&start_label);
    }else if(ll->logic==TOKEN_LUALIKE_FOR){
        int lp = IntraIR_GetLogicPath(ll,ll->ev.sc.range);
        CStr start_label = IntraIR_Logic(ll,0,INTRAIR_FOR,ll->ev.sc.range);
        CStr end_label = IntraIR_Logic(ll,1,INTRAIR_FOR,ll->ev.sc.range);
        IntraIR_Indentation_Appendf(ll,&ll->text,"jmp %s",start_label);
        IntraIR_Indentation_Appendf(ll,&ll->text,"%s:",end_label);
        CStr_Free(&end_label);
        CStr_Free(&start_label);
        Scope_Pop(&ll->ev.sc);// first possible decl in for ... , 
    }

    ll->logic = TOKEN_NONE;
}

/*
void IntraIR_Variable_BuildXX(IntraIR* ll,CStr name,CStr type,int sizeonstack,int stack,char destroy){
    Scope_BuildInitVariable(&ll->ev.sc,name,type,(IntraIRVariable[]){ IntraIRVariable_New(stack,sizeonstack,destroy,ll) });
}
void IntraIR_Variable_BuildX(IntraIR* ll,CStr name,CStr type,int sizeonstack,char destroy){
    Scope_BuildInitVariable(&ll->ev.sc,name,type,(IntraIRVariable[]){ IntraIRVariable_New(ll->stack,sizeonstack,destroy,ll) });
}
void IntraIR_Variable_Build(IntraIR* ll,CStr name,CStr type){
    IntraIR_Variable_BuildX(ll,name,type,0,1);
}
void IntraIR_Variable_BuildRange(IntraIR* ll,CStr name,CStr type,Range r,int sizeonstack,char destroy){
    Scope_BuildInitVariableRange(&ll->ev.sc,name,type,r,(IntraIRVariable[]){ IntraIRVariable_New(ll->stack,sizeonstack,destroy,ll) });
}
void IntraIR_Variable_Build_Decl(IntraIR* ll,CStr name,CStr type){
    IntraIR_Variable_Build(ll,name,type);
    
    if(ll->ev.sc.range==0){
        CStr g_value = IntraIR_BuildGlobal(ll,name,IntraIR_Size(ll,type));
        Variable* v = Scope_FindVariable(&ll->ev.sc,name);
        IntraIRVariable* sv = (IntraIRVariable*)Variable_Data(v);
        sv->global = g_value;
    }else{
        IntraIR_Indentation_Appendf(ll,&ll->text,"sub rsp,%d",IntraIR_Size(ll,type));
    }
}
void IntraIR_Variable_Build_Ref(IntraIR* ll,CStr name,CStr type){
    IntraIR_Variable_BuildX(ll,name,type,0,0);
}
void IntraIR_Variable_BuildRange_Decl(IntraIR* ll,CStr name,CStr type,Range r){
    IntraIR_Variable_BuildRange(ll,name,type,r,0,1);
    
    if(ll->ev.sc.range==0){
        CStr g_value = IntraIR_BuildGlobal(ll,name,IntraIR_Size(ll,type));
        Variable* v = Scope_FindVariable(&ll->ev.sc,name);
        IntraIRVariable* sv = (IntraIRVariable*)Variable_Data(v);
        sv->global = g_value;
    }else{
        IntraIR_Indentation_Appendf(ll,&ll->text,"sub rsp,%d",IntraIR_Size(ll,type));
    }
}
void IntraIR_Variable_Build_Ref_Decl(IntraIR* ll,CStr name,CStr type){
    IntraIR_Variable_Build_Ref(ll,name,type);

    if(ll->ev.sc.range==0){
        CStr g_value = IntraIR_BuildGlobal(ll,name,IntraIR_Size(ll,type));
        Variable* v = Scope_FindVariable(&ll->ev.sc,name);
        IntraIRVariable* sv = (IntraIRVariable*)Variable_Data(v);
        sv->global = g_value;
    }else{
        IntraIR_Indentation_Appendf(ll,&ll->text,"sub rsp,%d",IntraIR_Size(ll,type));
    }
}
void IntraIR_Variable_Build_Use(IntraIR* ll,CStr name,CStr type,int stack){
    IntraIR_Variable_BuildXX(ll,name,type,0,stack,2);
}
void IntraIR_Variable_Destroy_Decl(IntraIR* ll,CStr name){
    Variable* v = Scope_FindVariable(&ll->ev.sc,name);
    if(v->range>0){
        int size = IntraIR_Size(ll,v->typename);
        if(size>0) IntraIR_Indentation_Appendf(ll,&ll->text,"add rsp,%d",size);
        ll->stack -= size;
    }
}
void IntraIR_Variable_Destroy_Ref_Decl(IntraIR* ll,CStr name){
    Variable* v = Scope_FindVariable(&ll->ev.sc,name);
    if(v->range>0){
        //IntraIR_Indentation_Appendf(ll,&ll->text,"add rsp,%d",IntraIR_Size(ll,v->typename));
        ll->stack -= IntraIR_Size(ll,v->typename);
    }
}
void IntraIR_Variable_Destroy_Use(IntraIR* ll,CStr name){
    //Variable* v = Scope_FindVariable(&ll->ev.sc,name);
}

CStr IntraIR_VariableType(IntraIR* ll,Token* a){
    if(a->tt==TOKEN_STRING){
        Variable* v_a = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v_a) return CStr_Cpy(v_a->typename);
    }
    if(a->tt==TOKEN_INTRAIR_BOOLEAN)    return CStr_Cpy(BOOL_TYPE);
    if(a->tt==TOKEN_NUMBER)              return CStr_Cpy(I64_TYPE);
    if(a->tt==TOKEN_CONSTSTRING_DOUBLE)  return CStr_Cpy(STR_TYPE);
    if(a->tt==TOKEN_FLOAT)               return CStr_Cpy(F64_TYPE);
    if(a->tt==TOKEN_FUNCTIONPOINTER)     return CStr_Cpy(NULL_TYPE);
    return NULL;
}
CStr IntraIR_VariablesType(IntraIR* ll,Token* a,Token* b){
    CStr ret = IntraIR_VariableType(ll,a);
    if(ret) return ret;
    return IntraIR_VariableType(ll,b);
}

int IntraIR_TypeRealSize(IntraIR* ll,Token* a){
    CStr type = IntraIR_VariableType(ll,a);
    
    if(IntraIR_DrefType(ll,type)){
        CStr ntype = IntraIR_TypeOfDref(ll,type);
        CStr_Set(&type,ntype);
        CStr_Free(&ntype);
    }

    int size = IntraIR_Size(ll,type);
    CStr_Free(&type);
    return size;
}
char** IntraIR_SelectRT(IntraIR* ll,int size){
    if(size==1) return IntraIR_RT_8;
    if(size==2) return IntraIR_RT_16;
    if(size==4) return IntraIR_RT_32;
    if(size==8) return IntraIR_RT_64;
    return NULL;
}
CStr IntraIR_FunctionName(IntraIR* ll,CStr name){
    String builder = String_Format("f.%s",name);
    String_Replace(&builder,"::",".");
    
    CStr functionname = String_CStr(&builder);
    String_Free(&builder);
    return functionname;
}
CStr IntraIR_SpaceName(IntraIR* ll,CStr name){
    String builder = String_New();

    for(int i = 0;i<ll->ev.cs.size;i++){
        CallPosition* cp = (CallPosition*)Vector_Get(&ll->ev.cs,i);
        if(cp->type==TOKEN_INTRAIR_NAMESPACE){
            String_Append(&builder,cp->fname);
            String_Append(&builder,"::");
        }
    }
    String_Append(&builder,name);

    CStr cstr = String_CStr(&builder);
    String_Free(&builder);
    return cstr;
}
CStr IntraIR_FuncSpaceName(IntraIR* ll,CStr name){
    String builder = String_New();

    for(int i = 0;i<ll->ev.cs.size;i++){
        CallPosition* cp = (CallPosition*)Vector_Get(&ll->ev.cs,i);
        if(cp->type==TOKEN_INTRAIR_IMPL){
            String_Append(&builder,cp->fname);
            String_Append(&builder,"::");
            break;
        }
    }

    if(builder.size == 0){
        for(int i = 0;i<ll->ev.cs.size;i++){
            CallPosition* cp = (CallPosition*)Vector_Get(&ll->ev.cs,i);
            if(cp->type==TOKEN_INTRAIR_NAMESPACE){
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
Function* IntraIR_FunctionIn(IntraIR* ll) {
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

Boolean IntraIR_TypeInt(CStr typename){
    return
    CStr_Cmp(typename,I8_TYPE) || CStr_Cmp(typename,I16_TYPE) ||
    CStr_Cmp(typename,I32_TYPE) || CStr_Cmp(typename,I64_TYPE) ||
    CStr_Cmp(typename,U8_TYPE) || CStr_Cmp(typename,U16_TYPE) ||
    CStr_Cmp(typename,U32_TYPE) || CStr_Cmp(typename,U64_TYPE);
}
Boolean IntraIR_TypeFloat(CStr typename){
    return CStr_Cmp(typename,F32_TYPE) || CStr_Cmp(typename,F64_TYPE);
}

Number IntraIR_Function_Add(Number a,Number b){
    return a + b;
}
Number IntraIR_Function_Sub(Number a,Number b){
    return a - b;
}
Number IntraIR_Function_Mul(Number a,Number b){
    return a * b;
}
Number IntraIR_Function_Div(Number a,Number b){
    return a / b;
}
Number IntraIR_Function_Mod(Number a,Number b){
    return a % b;
}
Number IntraIR_Function_And(Number a,Number b){
    return a & b;
}
Number IntraIR_Function_Or(Number a,Number b){
    return a | b;
}
Number IntraIR_Function_Xor(Number a,Number b){
    return a ^ b;
}
Number IntraIR_Function_Not(Number a){
    return ~a;
}
Number IntraIR_Function_Neg(Number a){
    return -a;
}
Boolean IntraIR_Function_Equ(Number a,Number b){
    return a == b;
}
Boolean IntraIR_Function_Neq(Number a,Number b){
    return a != b;
}
Boolean IntraIR_Function_Les(Number a,Number b){
    return a < b;
}
Boolean IntraIR_Function_Grt(Number a,Number b){
    return a > b;
}
Boolean IntraIR_Function_Leq(Number a,Number b){
    return a <= b;
}
Boolean IntraIR_Function_Grq(Number a,Number b){
    return a >= b;
}
Boolean IntraIR_Function_Lnd(Boolean a,Boolean b){
    return a && b;
}
Boolean IntraIR_Function_Lor(Boolean a,Boolean b){
    return a || b;
}
Boolean IntraIR_Function_Lot(Boolean a){
    return !a;
}

void IntraIR_Destroyer(Variable* v){
    //printf("[IntraIR]: Destroyer: %s -> ",v->typename);
    IntraIRVariable* v_sv = (IntraIRVariable*)Variable_Data(v);
    if(v_sv->destroy==0){
        IntraIR_Variable_Destroy_Ref_Decl(v_sv->parent,v->name);
        //printf("Ref -> %s -> %d\n",v->name,((IntraIR*)v_sv->parent)->stack);
    }else if(v_sv->destroy==1){
        IntraIR_Variable_Destroy_Decl(v_sv->parent,v->name);
        //printf("Real -> %s -> %d\n",v->name,((IntraIR*)v_sv->parent)->stack);
    }else if(v_sv->destroy==2){
        IntraIR_Variable_Destroy_Use(v_sv->parent,v->name);
        //printf("Use -> %s -> %d -> %d\n",v->name,v_sv->stack,((IntraIR*)v_sv->parent)->stack);
    }
    IntraIRVariable_Free(v_sv);
}
void IntraIR_Cpyer(Variable* src,Variable* dst){
    //printf("[IntraIR]: Cpyer!\n");
    //IntraIRVariable* src_str = (IntraIRVariable*)Variable_Data(src);
    //IntraIRVariable* dst_str = (IntraIRVariable*)Variable_Data(dst);
    //*dst_str = VariableMap_Cpy(src_str);
}
Token IntraIR_Init(IntraIR* ll,Token* op,Vector* args){
    Token* a = (Token*)Vector_Get(args,0);

    //printf("[IntraIR]: INIT: %s\n",a->str);
    
    Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
    if(v){
        IntraIRVariable* sv = (IntraIRVariable*)Variable_Data(v);
        int size = IntraIR_Size(ll,v->typename);
        
        if(size>0){
            IntraIR_Indentation_Appendf(ll,&ll->text,"sub rsp,%d",size);
        }
        *sv = IntraIRVariable_New(1,ll);
    }
    return Token_Cpy(a);
}

Double IntraIR_GetFloat(IntraIR* ll,CStr fstr){
    return Double_Parse(fstr,0);
}
CStr IntraIR_GetFloatStr(IntraIR* ll,CStr fstr){
    Double d = IntraIR_GetFloat(ll,fstr);
    CStr cstr = Number_Get(*((Number*)&d));
    return cstr;
}
Token IntraIR_GetFloatToken(IntraIR* ll,CStr fstr){
    return Token_Move(TOKEN_FLOAT,IntraIR_GetFloatStr(ll,fstr));
}

CStr IntraIR_FInstOf(IntraIR* ll,Token* a,CStr inst){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            if(IntraIR_TypeFloat(v->typename)){
                return CStr_Format("f%s",inst);
            }else if(IntraIR_TypeInt(v->typename)){
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

Boolean IntraIR_Extract(IntraIR* ll,Token* a,Number* n){
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
Boolean IntraIR_ExtractBool(IntraIR* ll,Token* a,Boolean* b){
    if(a->tt==TOKEN_STRING){
        return False;
    }else if(a->tt==TOKEN_INTRAIR_BOOLEAN){
        *b = Boolean_Parse(a->str);
        return True;
    }else{
        Compiler_ErrorHandler(&ll->ev,"1. Arg: %s is not a bool type!",a->str);
        return False;
    }
}

Boolean IntraIR_ErrorsInArg(IntraIR* ll,Token* a){
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
    if(a->tt==TOKEN_INTRAIR_BOOLEAN){
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

void IntraIR_DrefIntoReg(IntraIR* ll,Token* a,CStr reg){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            if(IntraIR_DrefType(ll,v->typename)){
                CStr location = IntraIR_Location(ll,a->str);
                IntraIR_Indentation_Appendf(ll,&ll->text,"mov %s,%s",reg,location);
                CStr typename = CStr_Cpy(v->typename);
                int drefs = IntraIR_DrefTypeCount(ll,typename);
                for(int i = 0;i<drefs-1;i++){
                    CStr typeselector = NULL;
                    if(typename[CStr_Size(typename) - 1]=='&')  typeselector = CStr_Cpy(INTRAIR_DREF_64);
                    else                                        typeselector = IntraIR_TypeSelector_T(ll,typename);

                    IntraIR_Indentation_Appendf(ll,&ll->text,"mov %s,%s[%s]",reg,typeselector,reg);
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
void IntraIR_IntoReg(IntraIR* ll,Token* a,CStr reg){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            CStr location = IntraIR_Location(ll,a->str);
        
            if(IntraIR_DrefType(ll,v->typename)){
                IntraIR_DrefIntoReg(ll,a,INTRAIR_REG_D_64);
                
                CStr typename = IntraIR_TypeOfDref(ll,v->typename);
                CStr typeselector = IntraIR_TypeSelector_T(ll,typename);
                IntraIR_Indentation_Appendf(ll,&ll->text,"mov %s,%s[%s]",reg,typeselector,INTRAIR_REG_D_64);
                CStr_Free(&typename);
                CStr_Free(&typeselector);
            }else{
                IntraIR_Indentation_Appendf(ll,&ll->text,"mov %s,%s",reg,location);
            }
            CStr_Free(&location);
        }else{
            Compiler_ErrorHandler(&ll->ev,"IntoReg -> Error: %s is not a var!",a->str);
        }
    }else{
        IntraIR_Indentation_Appendf(ll,&ll->text,"mov %s,%s",reg,a->str);
    }
}
void IntraIR_IntoSet(IntraIR* ll,Token* a,CStr reg){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            CStr location = IntraIR_Location(ll,a->str);
        
            if(IntraIR_DrefType(ll,v->typename)){
                IntraIR_DrefIntoReg(ll,a,INTRAIR_REG_D_64);
                
                CStr typename = IntraIR_TypeOfDref(ll,v->typename);
                CStr typeselector = IntraIR_TypeSelector_T(ll,typename);
                IntraIR_Indentation_Appendf(ll,&ll->text,"mov %s[%s],%s",typeselector,INTRAIR_REG_D_64,reg);
                CStr_Free(&typename);
                CStr_Free(&typeselector);
            }else{
                IntraIR_Indentation_Appendf(ll,&ll->text,"mov %s,%s",location,reg);
            }
            CStr_Free(&location);
        }else{
            Compiler_ErrorHandler(&ll->ev,"IntoSet -> Error: %s is not a var!",a->str);
        }
    }else{
        Compiler_ErrorHandler(&ll->ev,"IntoSet -> Error: %s is not a var but assigned!",a->str);
    }
}
void IntraIR_AtReg(IntraIR* ll,Token* a,CStr reg,CStr inst){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            CStr location = IntraIR_Location(ll,a->str);
            
            if(IntraIR_DrefType(ll,v->typename)){
                IntraIR_DrefIntoReg(ll,a,INTRAIR_REG_D_64);
                
                CStr typename = IntraIR_TypeOfDref(ll,v->typename);
                CStr typeselector = IntraIR_TypeSelector_T(ll,typename);
                IntraIR_Indentation_Appendf(ll,&ll->text,"%s %s[%s],%s",inst,typeselector,INTRAIR_REG_D_64,reg);
                CStr_Free(&typename);
                CStr_Free(&typeselector);
            }else{
                IntraIR_Indentation_Appendf(ll,&ll->text,"%s %s,%s",inst,reg,location);
            }
            CStr_Free(&location);
        }else{
            Compiler_ErrorHandler(&ll->ev,"AtReg -> Error: %s is not a var!",a->str);
        }
    }else{
        IntraIR_Indentation_Appendf(ll,&ll->text,"%s %s,%s",inst,reg,a->str);
    }
}
void IntraIR_AtRegSingle(IntraIR* ll,Token* a,CStr inst){//Always A ex: mul,div etc
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            CStr location = IntraIR_Location(ll,a->str);
            
            if(IntraIR_DrefType(ll,v->typename)){
                IntraIR_DrefIntoReg(ll,a,INTRAIR_REG_D_64);
                
                CStr typename = IntraIR_TypeOfDref(ll,v->typename);
                CStr typeselector = IntraIR_TypeSelector_T(ll,typename);
                IntraIR_Indentation_Appendf(ll,&ll->text,"%s %s[%s]",inst,typeselector,INTRAIR_REG_D_64);
                CStr_Free(&typename);
                CStr_Free(&typeselector);
            }else{
                IntraIR_Indentation_Appendf(ll,&ll->text,"%s %s",inst,location);
            }
            CStr_Free(&location);
        }else{
            Compiler_ErrorHandler(&ll->ev,"AtRegSingle -> Error: %s is not a var!",a->str);
        }
    }else{
        IntraIR_Indentation_Appendf(ll,&ll->text,"%s %s",inst,a->str);
    }
}
void IntraIR_AtSingle(IntraIR* ll,CStr reg,CStr inst){//Always A ex: mul,div etc
    IntraIR_Indentation_Appendf(ll,&ll->text,"%s %s",inst,reg);
}
void IntraIR_AtSet(IntraIR* ll,Token* a,CStr reg,CStr inst){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        if(v){
            CStr location = IntraIR_Location(ll,a->str);
            
            if(IntraIR_DrefType(ll,v->typename)){
                IntraIR_DrefIntoReg(ll,a,INTRAIR_REG_D_64);
                
                CStr typename = IntraIR_TypeOfDref(ll,v->typename);
                CStr typeselector = IntraIR_TypeSelector_T(ll,typename);
                IntraIR_Indentation_Appendf(ll,&ll->text,"%s %s[%s],%s",inst,typeselector,INTRAIR_REG_D_64,reg);
                CStr_Free(&typename);
                CStr_Free(&typeselector);
            }else{
                IntraIR_Indentation_Appendf(ll,&ll->text,"%s %s,%s",inst,location,reg);
            }
            CStr_Free(&location);
        }else{
            Compiler_ErrorHandler(&ll->ev,"AtSet -> Error: %s is not a var!",a->str);
        }
    }else{
        Compiler_ErrorHandler(&ll->ev,"Set -> Error: %s is not a var but assigned!",a->str);
    }
}
void IntraIR_CmpAtReg(IntraIR* ll,Token* a,CStr reg){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        CStr location = IntraIR_Location(ll,a->str);
        
        if(IntraIR_DrefType(ll,v->typename)){
            IntraIR_DrefIntoReg(ll,a,INTRAIR_REG_D_64);
                
            CStr typename = IntraIR_TypeOfDref(ll,v->typename);
            CStr typeselector = IntraIR_TypeSelector_T(ll,typename);
            IntraIR_Indentation_Appendf(ll,&ll->text,"cmp %s[%s],%s",typeselector,INTRAIR_REG_D_64,reg);
            CStr_Free(&typename);
            CStr_Free(&typeselector);
        }else{
            IntraIR_Indentation_Appendf(ll,&ll->text,"cmp %s,%s",reg,location);
        }
        CStr_Free(&location);
    }else{
        IntraIR_Indentation_Appendf(ll,&ll->text,"cmp %s,%s",reg,a->str);
    }
}
void IntraIR_CmpAtSet(IntraIR* ll,Token* a,CStr inst){
    if(a->tt==TOKEN_STRING){
        Variable* v = Scope_FindVariable(&ll->ev.sc,a->str);
        CStr location = IntraIR_Location(ll,a->str);
        
        if(IntraIR_DrefType(ll,v->typename)){
            IntraIR_DrefIntoReg(ll,a,INTRAIR_REG_D_64);
                
            CStr typename = IntraIR_TypeOfDref(ll,v->typename);
            CStr typeselector = IntraIR_TypeSelector_T(ll,typename);
            IntraIR_Indentation_Appendf(ll,&ll->text,"%s %s[%s]",inst,typeselector,INTRAIR_REG_D_64);
            CStr_Free(&typename);
            CStr_Free(&typeselector);
        }else{
            IntraIR_Indentation_Appendf(ll,&ll->text,"%s %s",inst,location);
        }
        CStr_Free(&location);
    }else{
        Compiler_ErrorHandler(&ll->ev,"Set -> Error: %s is not a var but assigned!",a->str);
    }
}

Token IntraIR_ExecuteAss(IntraIR* ll,Token* a,Token* b,Token* op,CStr instname,CStr instnameupper){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(IntraIR_ErrorsInArg(ll,a)) return Token_Null();
    if(IntraIR_ErrorsInArg(ll,b)) return Token_Null();

    if(b->tt==TOKEN_NUMBER){
        IntraIR_AtSet(ll,a,b->str,instname);
        return Token_Cpy(a);
    }else if(b->tt==TOKEN_FLOAT){
        CStr fstr = IntraIR_GetFloatStr(ll,b->str);
        IntraIR_Indentation_Appendf(ll,&ll->text,"mov %s,%s",INTRAIR_REG_A_64,fstr);
        IntraIR_AtSet(ll,a,INTRAIR_REG_A_64,instname);
        CStr_Free(&fstr);
        return Token_Cpy(a);
    }else{
        int realsize_a = IntraIR_TypeRealSize(ll,a);
        int realsize_b = IntraIR_TypeRealSize(ll,b);
        
        if(realsize_b>realsize_a)
            IntraIR_Indentation_Appendf(ll,&ll->text,"mov %s,0",INTRAIR_REG_A_64);

        IntraIR_IntoReg(ll,b,IntraIR_SelectRT(ll,realsize_b)[INTRAIR_REG_A]);
        IntraIR_AtSet(ll,a,IntraIR_SelectRT(ll,realsize_a)[INTRAIR_REG_A],instname);
        return Token_Cpy(a);
    }
}
Token IntraIR_ExecuteAssA(IntraIR* ll,Token* a,Token* b,Token* op,CStr instname,CStr instnameupper){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(IntraIR_ErrorsInArg(ll,a)) return Token_Null();
    if(IntraIR_ErrorsInArg(ll,b)) return Token_Null();

    int realsize_a = IntraIR_TypeRealSize(ll,a);
    int realsize_b = IntraIR_TypeRealSize(ll,b);
        
    if(realsize_b>realsize_a)
        IntraIR_Indentation_Appendf(ll,&ll->text,"mov %s,0",INTRAIR_REG_A_64);

    IntraIR_IntoReg(ll,a,IntraIR_SelectRT(ll,realsize_a)[INTRAIR_REG_A]);
    IntraIR_IntoReg(ll,b,IntraIR_SelectRT(ll,realsize_b)[INTRAIR_REG_B]);
    IntraIR_AtSingle(ll,IntraIR_SelectRT(ll,realsize_b)[INTRAIR_REG_B],instname);
    IntraIR_IntoSet(ll,a,IntraIR_SelectRT(ll,realsize_a)[INTRAIR_REG_A]);
    return Token_Cpy(a);
}
Token IntraIR_ExecuteAssSingle(IntraIR* ll,Token* a,Token* op,CStr instname,CStr instnameupper){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(IntraIR_ErrorsInArg(ll,a)) return Token_Null();

    IntraIR_AtRegSingle(ll,a,instname);
    return Token_Cpy(a);
}

Token IntraIR_Execute(IntraIR* ll,Token* a,Token* b,Token* op,CStr instname,CStr instnameupper,Number (*inst)(Number,Number)){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(IntraIR_ErrorsInArg(ll,a)) return Token_Null();
    if(IntraIR_ErrorsInArg(ll,b)) return Token_Null();

    if(a->tt==TOKEN_NUMBER && b->tt==TOKEN_NUMBER){
        char* resstr = Number_Get(inst(Number_Parse(a->str),Number_Parse(b->str)));
        return Token_Move(TOKEN_NUMBER,resstr);
    }else{
        CStr typename_a = IntraIR_VariableType(ll,a);
        int realsize_a = IntraIR_TypeRealSize(ll,a);
        int realsize_b = IntraIR_TypeRealSize(ll,b);
        
        CStr stack_name = IntraIR_Variablename_Next(ll,".STACK",6);
        IntraIR_Variable_Build_Decl(ll,stack_name,typename_a);
        CStr_Free(&typename_a);
        Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        
        if(realsize_b>realsize_a)
            IntraIR_Indentation_Appendf(ll,&ll->text,"mov %s,0",INTRAIR_REG_A_64);

        IntraIR_IntoReg(ll,a,IntraIR_SelectRT(ll,realsize_a)[INTRAIR_REG_A]);
        IntraIR_AtReg(ll,b,IntraIR_SelectRT(ll,realsize_b)[INTRAIR_REG_A],instname);
        IntraIR_IntoSet(ll,&stack_t,IntraIR_SelectRT(ll,realsize_a)[INTRAIR_REG_A]);
        return stack_t;
    }
}
Token IntraIR_ExecuteA(IntraIR* ll,Token* a,Token* b,Token* op,CStr instname,CStr instnameupper,Number (*inst)(Number,Number)){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(IntraIR_ErrorsInArg(ll,a)) return Token_Null();
    if(IntraIR_ErrorsInArg(ll,b)) return Token_Null();

    if(a->tt==TOKEN_NUMBER && b->tt==TOKEN_NUMBER){
        char* resstr = Number_Get(inst(Number_Parse(a->str),Number_Parse(b->str)));
        return Token_Move(TOKEN_NUMBER,resstr);
    }else{
        if(b->tt==TOKEN_NUMBER){
            CStr typename_a = IntraIR_VariableType(ll,a);
            int realsize_a = IntraIR_TypeRealSize(ll,a);
        
            CStr stack_name = IntraIR_Variablename_Next(ll,".STACK",6);
            IntraIR_Variable_Build_Decl(ll,stack_name,typename_a);
            CStr_Free(&typename_a);
            Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        
            IntraIR_IntoReg(ll,a,IntraIR_SelectRT(ll,realsize_a)[INTRAIR_REG_A]);
            IntraIR_IntoReg(ll,b,INTRAIR_REG_B_64);
            IntraIR_AtSingle(ll,INTRAIR_REG_B_64,instname);
            IntraIR_IntoSet(ll,&stack_t,IntraIR_SelectRT(ll,realsize_a)[INTRAIR_REG_A]);
            return stack_t;
        }else{
            CStr typename_a = IntraIR_VariableType(ll,a);
            int realsize_a = IntraIR_TypeRealSize(ll,a);
        
            CStr stack_name = IntraIR_Variablename_Next(ll,".STACK",6);
            IntraIR_Variable_Build_Decl(ll,stack_name,typename_a);
            CStr_Free(&typename_a);
            Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        
            IntraIR_IntoReg(ll,a,IntraIR_SelectRT(ll,realsize_a)[INTRAIR_REG_A]);
            IntraIR_AtRegSingle(ll,b,instname);
            IntraIR_IntoSet(ll,&stack_t,IntraIR_SelectRT(ll,realsize_a)[INTRAIR_REG_A]);
            return stack_t;
        }
    }
}
Token IntraIR_ExecuteAR(IntraIR* ll,Token* a,Token* b,Token* op,CStr instname,CStr instnameupper,Number (*inst)(Number,Number)){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(IntraIR_ErrorsInArg(ll,a)) return Token_Null();
    if(IntraIR_ErrorsInArg(ll,b)) return Token_Null();

    if(a->tt==TOKEN_NUMBER && b->tt==TOKEN_NUMBER){
        char* resstr = Number_Get(inst(Number_Parse(a->str),Number_Parse(b->str)));
        return Token_Move(TOKEN_NUMBER,resstr);
    }else{
        if(b->tt==TOKEN_NUMBER){
            CStr typename_a = IntraIR_VariableType(ll,a);
            int realsize_a = IntraIR_TypeRealSize(ll,a);
        
            CStr stack_name = IntraIR_Variablename_Next(ll,".STACK",6);
            IntraIR_Variable_Build_Decl(ll,stack_name,typename_a);
            CStr_Free(&typename_a);
            Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        
            IntraIR_IntoReg(ll,a,IntraIR_SelectRT(ll,realsize_a)[INTRAIR_REG_A]);
            IntraIR_IntoReg(ll,b,INTRAIR_REG_B_64);
            IntraIR_AtSingle(ll,INTRAIR_REG_B_64,instname);
            IntraIR_IntoSet(ll,&stack_t,IntraIR_SelectRT(ll,realsize_a)[INTRAIR_REG_D]);
            return stack_t;
        }else{
            CStr typename_a = IntraIR_VariableType(ll,a);
            int realsize_a = IntraIR_TypeRealSize(ll,a);
        
            CStr stack_name = IntraIR_Variablename_Next(ll,".STACK",6);
            IntraIR_Variable_Build_Decl(ll,stack_name,typename_a);
            CStr_Free(&typename_a);
            Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        
            IntraIR_IntoReg(ll,a,IntraIR_SelectRT(ll,realsize_a)[INTRAIR_REG_A]);
            IntraIR_AtRegSingle(ll,b,instname);
            IntraIR_IntoSet(ll,&stack_t,IntraIR_SelectRT(ll,realsize_a)[INTRAIR_REG_D]);
            return stack_t;
        }
    }
}
Token IntraIR_ExecuteSingle(IntraIR* ll,Token* a,Token* op,CStr instname,CStr instnameupper,Number (*inst)(Number)){
    //Compiler_InfoHandler(&ll->ev,"%s: %s%s",instnameupper,op->str,a->str);
    
    if(IntraIR_ErrorsInArg(ll,a)) return Token_Null();

    if(a->tt==TOKEN_NUMBER){
        char* resstr = Number_Get(inst(Number_Parse(a->str)));
        return Token_Move(TOKEN_NUMBER,resstr);
    }else{
        CStr typename_a = IntraIR_VariableType(ll,a);
        int realsize_a = IntraIR_TypeRealSize(ll,a);
        
        CStr stack_name = IntraIR_Variablename_Next(ll,".STACK",6);
        IntraIR_Variable_Build_Decl(ll,stack_name,typename_a);
        CStr_Free(&typename_a);
        Token stack_t = Token_Move(TOKEN_STRING,stack_name);

        IntraIR_IntoReg(ll,a,IntraIR_SelectRT(ll,realsize_a)[INTRAIR_REG_A]);
        IntraIR_AtSingle(ll,IntraIR_SelectRT(ll,realsize_a)[INTRAIR_REG_A],instname);
        IntraIR_IntoSet(ll,&stack_t,IntraIR_SelectRT(ll,realsize_a)[INTRAIR_REG_A]);
        
        return stack_t;
    }
}
Token IntraIR_ExecuteCmp(IntraIR* ll,Token* a,Token* b,Token* op,CStr instname,CStr instnameupper,Boolean (*inst)(Number,Number)){
    //Compiler_InfoHandler(&ll->ev,"%s: %s %s %s",instnameupper,a->str,op->str,b->str);
    
    if(IntraIR_ErrorsInArg(ll,a)) return Token_Null();
    if(IntraIR_ErrorsInArg(ll,b)) return Token_Null();

    if(a->tt==TOKEN_NUMBER && b->tt==TOKEN_NUMBER){
        char* resstr = Boolean_Get(inst(Number_Parse(a->str),Number_Parse(b->str)));
        return Token_Move(TOKEN_INTRAIR_BOOLEAN,resstr);
    }else{
        int realsize_a = IntraIR_TypeRealSize(ll,a);
        int realsize_b = IntraIR_TypeRealSize(ll,b);
        
        CStr stack_name = IntraIR_Variablename_Next(ll,".STACK",6);
        IntraIR_Variable_Build_Decl(ll,stack_name,BOOL_TYPE);
        Token stack_t = Token_Move(TOKEN_STRING,stack_name);
        
        if(realsize_b>realsize_a)
            IntraIR_Indentation_Appendf(ll,&ll->text,"mov %s,0",INTRAIR_REG_A_64);

        IntraIR_IntoReg(ll,a,IntraIR_SelectRT(ll,realsize_a)[INTRAIR_REG_A]);
        IntraIR_CmpAtReg(ll,b,IntraIR_SelectRT(ll,realsize_b)[INTRAIR_REG_A]);
        IntraIR_CmpAtSet(ll,&stack_t,instname);
        return stack_t;
    }
}
*/
#endif //!INTRAIRASM