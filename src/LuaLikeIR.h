#ifndef LUALIKEIR_H
#define LUALIKEIR_H

#include "/home/codeleaded/System/Static/Library/AlxScope.h"
#include "/home/codeleaded/System/Static/Library/AlxShutingYard.h"
#include "/home/codeleaded/System/Static/Library/AlxCompiler.h"

#include "/home/codeleaded/System/Static/Library/ConstParser.h"
#include "/home/codeleaded/System/Static/Library/Files.h"
#include "/home/codeleaded/System/Static/Library/String_G.h"
#include "/home/codeleaded/System/Static/Library/../Container/CVector.h"

#include "LuaLikeIRDefines.h"
#include "LuaLikeIRASM.h"

FunctionRT ShutingYard_compress(LuaLikeIR* ll,TokenMap* tm);

FunctionRT ShutingYard_compress_staticmethods(LuaLikeIR* ll,TokenMap* tm){
    for(int i = 0;i<tm->size;i++){
        Token* class = (Token*)Vector_Get(tm,i);
        
        if(i<tm->size-2){
            Token* dddop = (Token*)Vector_Get(tm,i+1);
            Token* func = (Token*)Vector_Get(tm,i+2);

            if((class->tt==TOKEN_STRING || class->tt==TOKEN_TYPE) &&
               dddop->tt==TOKEN_LUALIKE_DDDOT &&
               (func->tt==TOKEN_STRING || func->tt==TOKEN_TYPE)){
                
                String builder = String_Make(class->str);
                String_Append(&builder,dddop->str);
                String_Append(&builder,func->str);

                CStr space = String_CStr(&builder);
                String_Free(&builder);

                CStr_Set((CStr*)&func->str,space);
                CStr_Free(&space);

                TokenMap_Remove(tm,i,i+2);
                i--;
                continue;
            }
        }
    }
    return FUNCTIONRT_NONE;
}
FunctionRT ShutingYard_compress_cast(LuaLikeIR* ll,TokenMap* tm){
    for(int i = 0;i<tm->size;i++){
        Token* pretl = (Token*)Vector_Get(tm,i);
        Token* type = (Token*)Vector_Get(tm,i+1);
        Token* pretr = (Token*)Vector_Get(tm,i+2);

        if(pretl->tt==TOKEN_PARENTHESES_L && type->tt==TOKEN_TYPE && pretr->tt==TOKEN_PARENTHESES_R){
            pretl->tt = TOKEN_CAST;
            CStr_Set((char**)&pretl->str,type->str);
            TokenMap_Remove(tm,i+1,i+3);
        }
    }
    return False;
}
FunctionRT ShutingYard_compress_decl(LuaLikeIR* ll,TokenMap* tm){
    for(int i = 0;i + 2<tm->size;i++){
        Token* name = (Token*)Vector_Get(tm,i);
        Token* ddot = (Token*)Vector_Get(tm,i+1);
        Token* type = (Token*)Vector_Get(tm,i+2);

        if(name->tt==TOKEN_STRING && ddot->tt==TOKEN_LUALIKE_DDOT && type->tt==TOKEN_TYPE){
            Token cpy = Token_Cpy(type);
            TokenMap_Remove(tm,i+1,i+3);
            Vector_Add(tm,&cpy,i);
        }
    }
    return False;
}
FunctionRT ShutingYard_compress_objects(LuaLikeIR* ll,TokenMap* tm){
    for(int i = 0;i+1<tm->size;i++){
        Token* prentl = (Token*)Vector_Get(tm,i);

        if(prentl->tt==TOKEN_CURLY_BRACES_L){
            prentl->tt = TOKEN_OBJECT;
            prentl->args = malloc(sizeof(Vector));
            *prentl->args = Vector_New(sizeof(TokenMap));
            
            int Parentheses = 0;
            int Last = i+1;
            Token* prentr = (Token*)Vector_Get(tm,i+1);
            if(prentr->tt!=TOKEN_CURLY_BRACES_R){
                for(int j = Last;j<tm->size;j++){
                    Token* t = (Token*)Vector_Get(tm,j);
            
                    if(t->tt==TOKEN_CURLY_BRACES_L) Parentheses++;
                    if(t->tt==TOKEN_CURLY_BRACES_R) Parentheses--;
                    if(t->tt==TOKEN_COMMA || Parentheses<0){
                        if(Parentheses<=0){
                            TokenMap newtm = TokenMap_SubMove(tm,Last,j);
                            ShutingYard_compress(ll,&newtm);
                            Vector_Push(prentl->args,&newtm);
                            Last = j + 1;
                        }
                    }
                    if(Parentheses<0) break;
                } 
            }else{
                Last++;
            }

            TokenMap_Remove(tm,i+1,Last);
        }
    }
    return FUNCTIONRT_NONE;
}
FunctionRT ShutingYard_compress_subscript(LuaLikeIR* ll,TokenMap* tm){
    for(int i = 0;i<tm->size;i++){
        Token* subs = (Token*)Vector_Get(tm,i);

        if(subs->tt==TOKEN_LUALIKE_SUBS){
            Vector_Add(tm,(Token[]){ Token_By(TOKEN_PARENTHESES_L,"(") },i+1);
        }
        if(subs->tt==TOKEN_SQUARE_BRACKETS_R){
            subs->tt = TOKEN_PARENTHESES_R;
            CStr_Set((CStr*)&subs->str,")");
        }
    }
    return FUNCTIONRT_NONE;
}
FunctionRT ShutingYard_compress_functioncalls(LuaLikeIR* ll,TokenMap* tm){
    for(int i = 0;i+1<tm->size;i++){
        Token* func = (Token*)Vector_Get(tm,i);
        Token* prentl = (Token*)Vector_Get(tm,i+1);

        if(func->tt==TOKEN_STRING && prentl->tt==TOKEN_PARENTHESES_L){
            func->tt = TOKEN_FUNCTION;
            func->args = malloc(sizeof(Vector));
            *func->args = Vector_New(sizeof(TokenMap));
            
            int Parentheses = 0;
            int Last = i+2;
            Token* prentr = (Token*)Vector_Get(tm,i+2);
            if(prentr->tt!=TOKEN_PARENTHESES_R){
                for(int j = Last;j<tm->size;j++){
                    Token* t = (Token*)Vector_Get(tm,j);
            
                    if(t->tt==TOKEN_PARENTHESES_L) Parentheses++;
                    if(t->tt==TOKEN_PARENTHESES_R) Parentheses--;
                    if(t->tt==TOKEN_COMMA || Parentheses<0){
                        if(Parentheses<=0){
                            TokenMap newtm = TokenMap_Sub(tm,Last,j);
                            ShutingYard_compress(ll,&newtm);
                            Vector_Push(func->args,&newtm);
                            Last = j + 1;
                        }
                    }
                    if(Parentheses<0) break;
                } 
            }else{
                Last++;
            }

            TokenMap_Remove(tm,i+1,Last);
        }
    }
    return FUNCTIONRT_NONE;
}
FunctionRT ShutingYard_compress_functiontoFunc(LuaLikeIR* ll,TokenMap* tm){
    for(int i = 0;i<tm->size;i++){
        Token* func = (Token*)Vector_Get(tm,i);

        if(func->tt==TOKEN_STRING){
            TT_Iter it = FunctionMap_Find(&ll->ev.fs,func->str);
            if(it>=0){
                func->tt = TOKEN_FUNCTIONPOINTER;
            }
        }
    }
    return FUNCTIONRT_NONE;
}
FunctionRT ShutingYard_compress(LuaLikeIR* ll,TokenMap* tm){
    //ShutingYard_compress_objects(ll,tm);
    
    ShutingYard_compress_decl(ll,tm);
    ShutingYard_compress_staticmethods(ll,tm);
    ShutingYard_compress_cast(ll,tm);
    ShutingYard_compress_subscript(ll,tm);
    ShutingYard_compress_functioncalls(ll,tm);
    ShutingYard_compress_functiontoFunc(ll,tm);
    return FUNCTIONRT_NONE;
}
FunctionRT ShutingYard_function(LuaLikeIR* ll,TokenMap* tm){
    CStr name = ((Token*)Vector_Get(tm,1))->str;
    Vector params = Vector_New(sizeof(Member));

    TokenMap rest = TokenMap_SubFromToken(tm,TOKEN_PARENTHESES_L);
    TokenMap between = TokenMap_SubToToken(&rest,TOKEN_PARENTHESES_R);
    TokenMap rettype = TokenMap_SubFromToken(&rest,TOKEN_PARENTHESES_R);
    TokenMap_Free(&rest);

    for(;between.size>0;){
        TokenMap first = TokenMap_SubToToken(&between,TOKEN_COMMA);
        TokenMap after = TokenMap_SubFromToken(&between,TOKEN_COMMA);

        TT_Iter name = TokenMap_Find(&first,TOKEN_STRING);
        TT_Iter decl = TokenMap_Find(&first,TOKEN_LUALIKE_DDOT);
        TT_Iter type = TokenMap_Find(&first,TOKEN_TYPE);
        
        TT_Iter ref = TokenMap_Find(&first,TOKEN_LUALIKE_REF);
        TT_Iter constant = TokenMap_Find(&first,TOKEN_LUALIKE_CONST);

        if(name>=0){
            CStr sname = ((Token*)Vector_Get(&first,name))->str;
            CStr stype = NULL;
            
            FunctionRT bref = ref>=0;
            FunctionRT bconstant = constant>=0;
            
            if(decl>=0 && type<first.size)
                stype = ((Token*)Vector_Get(&first,type))->str;
            
            Vector_Push(&params,(Member[]){ Member_Make(bconstant,bref,stype,sname) });
        }

        TokenMap_Free(&first);
        TokenMap_Free(&between);
        between = after;
    }
    TokenMap_Free(&between);

    CStr ftype = NULL;
    //FunctionRT bref = False;
    //FunctionRT bconstant = False;

    if(rettype.size>0){
        TT_Iter decl = TokenMap_Find(&rettype,TOKEN_LUALIKE_DDOT);
        TT_Iter type = decl + 1;
        
        //TT_Iter ref = TokenMap_Find(&rettype,TOKEN_LUALIKE_REF);
        //TT_Iter constant = TokenMap_Find(&rettype,TOKEN_LUALIKE_CONST);
        //bref = ref>=0;
        //bconstant = constant>=0;
        if(decl>=0 && type<tm->size)
            ftype = ((Token*)Vector_Get(&rettype,type))->str;
    }

    Vector_Push(&params,(Member[]){ MEMBER_END });

    String builder = String_New();
    for(int i = 0;i<ll->ev.cs.size;i++){
        CallPosition* cp = (CallPosition*)Vector_Get(&ll->ev.cs,i);
        
        if(cp->type==TOKEN_LUALIKE_CLASS){
            String_Append(&builder,cp->fname);
            String_Append(&builder,"::");
        }
    }
    String_Append(&builder,name);
    CStr realname = String_CStr(&builder);
    CStr_Set(&name,realname);
    String_Free(&builder);

    CallPosition cp = CallPosition_New(TOKEN_FUNCTION,ll->ev.iter);
    Vector_Push(&ll->ev.cs,&cp);

    Function f = Function_Make(ll->ev.iter,realname,ftype,(Member*)params.Memory);
    Vector_Push(&ll->ev.fs,&f);

    TokenMap_Free(&rettype);
    Vector_Free(&params);

    return FUNCTIONRT_NONE;
}

FunctionRT ShutingYard_Import(LuaLikeIR* ll,TokenMap* tm){
    // TODO with as -> Objects & access using . & :: (Methods and Members)
    return FUNCTIONRT_NONE;
}
FunctionRT ShutingYard_From(LuaLikeIR* ll,TokenMap* tm){
    Token* Package = (Token*)Vector_Get(tm,1);
    Token* Import = (Token*)Vector_Get(tm,2);
    Token* Asterisk = (Token*)Vector_Get(tm,3);

    return FUNCTIONRT_NONE;
}
FunctionRT ShutingYard_Include(LuaLikeIR* ll,TokenMap* tm){
    Token* file = (Token*)Vector_Get(tm,1);

    if(file->tt==TOKEN_CONSTSTRING_DOUBLE){
        char* type = Files_Type(file->str);
        if(CStr_Cmp(type,LUALIKE_TYPE)){
            CStr current = *(CStr*)CVector_Get(&ll->filesstack,ll->filesstack.size-1);
            CStr dir = Files_Path(current);
            CStr realpath = Files_FromPath(dir,file->str);
            CStr_Free(&dir);

            if(!CVector_Contains(&ll->filesinc,(CStr[]){ realpath })){
                CVector_Push(&ll->filesstack,(CStr[]){ CStr_Cpy(realpath) });
                CVector_Push(&ll->filesinc,(CStr[]){ CStr_Cpy(realpath) });
    
                Compiler_AddScript(&ll->ev,ll->ev.iter,realpath);
            
                CVector_PopTop(&ll->filesstack);
                CStr_Free(&realpath);
            }else{
                Enviroment_ErrorHandler(&ll->ev,"Include: module \"%s\" already included!\n",file->str);
            }
        }else{
            Enviroment_ErrorHandler(&ll->ev,"Include: file type of module \"%s\" is wrong: .%s, should be .%s\n",file->str,type,LUALIKE_TYPE);
        }
        free(type);
    }else{
        Enviroment_ErrorHandler(&ll->ev,"Include: module \"%s\" doesn't exist!\n",file->str);
    }

    return FUNCTIONRT_NONE;
}
FunctionRT ShutingYard_Class(LuaLikeIR* ll,TokenMap* tm){
    return FUNCTIONRT_NONE;
}

FunctionRT ShutingYard_type(LuaLikeIR* ll,TokenMap* tm){
    if(tm->size >= 2){
        Token* type = (Token*)Vector_Get(tm,0);
        Token* name = (Token*)Vector_Get(tm,1);
        Scope_BuildVariable(&ll->ev.sc,name->str,type->str);

        String* funcstr = LuaLikeIR_FunctionText(ll);
        LuaLikeIR_Indentation_Appendf(ll,funcstr,"make\t%s\t%s",type->str,name->str);
        
        TokenMap cpy = TokenMap_Cpy(tm);
        TokenMap_RemoveI(&cpy,0);
        Boolean ret = Compiler_ShutingYard(&ll->ev,&cpy);
        TokenMap_Free(&cpy);
        return ret;
    }
    return FUNCTIONRT_NONE;
}
FunctionRT ShutingYard_Function(LuaLikeIR* ll,TokenMap* tm){
    if(tm->size >= 2){
        //Token* function = (Token*)Vector_Get(tm,0);
        Token* name = (Token*)Vector_Get(tm,1);
        Function* func = FunctionMap_FindF(&ll->ev.fs,name->str);
        
        if(func){
            for(int i = 0;i<func->params.size;i++){
                Member* m = (Member*)Vector_Get(&func->params,i);
                Scope_BuildVariable(&ll->ev.sc,m->name,m->type);
            }
        }else{
            Compiler_ErrorHandler(&ll->ev,"function: %s doesn't exist!\n",name->str);
        }
    }
    return FUNCTIONRT_NONE;
}
FunctionRT ShutingYard_return(LuaLikeIR* ll,TokenMap* tm){
    return FUNCTIONRT_NONE;
}
FunctionRT ShutingYard_end(LuaLikeIR* ll,TokenMap* tm){
    return FUNCTIONRT_NONE;
}
FunctionRT ShutingYard_if(LuaLikeIR* ll,TokenMap* tm){
    return FUNCTIONRT_NONE;
}
FunctionRT ShutingYard_elif(LuaLikeIR* ll,TokenMap* tm){
    return FUNCTIONRT_NONE;
}
FunctionRT ShutingYard_else(LuaLikeIR* ll,TokenMap* tm){
    return FUNCTIONRT_NONE;
}
FunctionRT ShutingYard_while(LuaLikeIR* ll,TokenMap* tm){
    return FUNCTIONRT_NONE;
}
FunctionRT ShutingYard_for(LuaLikeIR* ll,TokenMap* tm){
    return FUNCTIONRT_NONE;
}

Boolean ShutingYard_FunctionCall_Acs(LuaLikeIR* ll,TokenMap* tm,int i,int args,Token* tok){
    Token* accssed = (Token*)Vector_Get(tm,i - args);
    Token* func = (Token*)Vector_Get(tm,i - args + 1);

    if(func->tt==TOKEN_FUNCTION){
        Variable* v = Scope_FindVariable(&ll->ev.sc,accssed->str);

        if(v){
            CStr oldname = CStr_Cpy(func->str);
            CStr newname = CStr_Format("%s::%s",v->typename,func->str);
            CStr_Set((char**)&func->str,newname);

            Function* ff = NULL;
            ExternFunction* xf = NULL;
            
            ff = FunctionMap_FindF(&ll->ev.fs,func->str);
            xf = ExternPackageMap_FindFunc(&ll->ev.epm,func->str);
            if(ff || xf){
                TokenMap acs = TokenMap_Make((Token[]){
                    Token_Cpy(accssed),
                    Token_Null()
                });
                Vector_Add(func->args,&acs,0);
            }else{
                CStr_Set((char**)&func->str,oldname);
            }

            CStr_Free(&newname);
            CStr_Free(&oldname);

            ff = FunctionMap_FindF(&ll->ev.fs,func->str);
            xf = ExternPackageMap_FindFunc(&ll->ev.epm,func->str);
            if(ff){
                if(ff->access || CStr_Cmp(accssed->str,LUALIKE_SELF)){
                    FunctionRT ret = Compiler_FunctionCall(&ll->ev,func);
                    if(ret != FUNCTIONRT_ARG0){
                        CStr retstr = Compiler_Variablename_This(&ll->ev,COMPILER_RETURN,7);
                        *tok = Token_Move(TOKEN_STRING,retstr);
                    }
                    return ret;
                }else{
                    Compiler_ErrorHandler(&ll->ev,"Function: %s isn't pub or non self %s tries to access!",func->str,accssed->str);
                    return FUNCTIONRT_NONE;
                }
            }else if(xf){
                if(xf->access || CStr_Cmp(accssed->str,LUALIKE_SELF)){
                    FunctionRT ret = Compiler_FunctionCall(&ll->ev,func);
                    if(ret != FUNCTIONRT_ARG0){
                        CStr retstr = Compiler_Variablename_This(&ll->ev,COMPILER_RETURN,7);
                        *tok = Token_Move(TOKEN_STRING,retstr);
                    }
                    return ret;
                }else{
                    Compiler_ErrorHandler(&ll->ev,"Function: %s isn't pub or non self %s tries to access!",func->str,accssed->str);
                    return FUNCTIONRT_NONE;
                }
            }else{
                Compiler_ErrorHandler(&ll->ev,"Function: dynamic function %s doesn't exist and %s tries to access!",func->str,accssed->str);
                return FUNCTIONRT_NONE;
            }
        }
    }
        
    return FUNCTIONRT_NONE;
}
void LuaLikeIR_Function_Handler(LuaLikeIR* ll,Token* t,Function* f){
    if(t->tt==TOKEN_FUNCTION){
        LuaLikeIR_Indentation_Appendf(ll,&ll->text,"call %s",f->name);
    }else{
        Compiler_ErrorHandler(&ll->ev,"Function: %s doesn't exist!",t->str);
    }
}

LuaLikeIR LuaLikeIR_New(char* dllpath,char* src,char* output,char bits) {
    LuaLikeIR ll;
    ll.ev = Compiler_New(
        KeywordMap_Make((KeywordRP[]){
            KeywordRP_New("function",TOKEN_LUALIKE_FUNCTION),
            KeywordRP_New("return",TOKEN_LUALIKE_RETURN),
            KeywordRP_New("end",TOKEN_LUALIKE_END),
            KeywordRP_New("if",TOKEN_LUALIKE_IF),
            KeywordRP_New("elif",TOKEN_LUALIKE_ELIF),
            KeywordRP_New("else",TOKEN_LUALIKE_ELSE),
            KeywordRP_New("while",TOKEN_LUALIKE_WHILE),
            KeywordRP_New("for",TOKEN_LUALIKE_FOR),
            KeywordRP_New("local",TOKEN_LUALIKE_LOCAL),
    
            KeywordRP_New("import",TOKEN_LUALIKE_IMPORT),
            KeywordRP_New("from",TOKEN_LUALIKE_FROM),
            KeywordRP_New("include",TOKEN_LUALIKE_INCLUDE),
            KeywordRP_New("ref",TOKEN_LUALIKE_REF),
            KeywordRP_New("const",TOKEN_LUALIKE_CONST),
            KeywordRP_New("class",TOKEN_LUALIKE_CLASS),
    
            KeywordRP_New("false",TOKEN_BOOL),
            KeywordRP_New("true",TOKEN_BOOL),
            KEYWORD_END
        }),
        OperatorMap_Make((OperatorRP[]){
            OperatorRP_Make((TT_Type[]){ TOKEN_EQUAL_SIGN,TOKEN_END },                                      TOKEN_LUALIKE_ASS,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_PLUS_SIGN,TOKEN_END },                                       TOKEN_LUALIKE_ADD,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_MINUS_SIGN,TOKEN_END },                                      TOKEN_LUALIKE_SUB,1),
            OperatorRP_Make((TT_Type[]){ TOKEN_ASTERISK,TOKEN_END },                                        TOKEN_LUALIKE_MUL,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_SLASH,TOKEN_END },                                           TOKEN_LUALIKE_DIV,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_MINUS_SIGN,TOKEN_END },                                      TOKEN_LUALIKE_NEG,0),
            OperatorRP_Make((TT_Type[]){ TOKEN_EQUAL_SIGN,TOKEN_EQUAL_SIGN,TOKEN_END },                     TOKEN_LUALIKE_EQU,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_ANGLE_BRACKETS_L,TOKEN_END },                                TOKEN_LUALIKE_LES,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_ANGLE_BRACKETS_R,TOKEN_END },                                TOKEN_LUALIKE_GRT,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_ANGLE_BRACKETS_L,TOKEN_EQUAL_SIGN,TOKEN_END },               TOKEN_LUALIKE_LEQ,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_ANGLE_BRACKETS_R,TOKEN_EQUAL_SIGN,TOKEN_END },               TOKEN_LUALIKE_GRQ,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_PERIOD,TOKEN_END },                                          TOKEN_LUALIKE_ACS,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_COLON,TOKEN_END },                                           TOKEN_LUALIKE_DDOT,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_COLON,TOKEN_COLON,TOKEN_END },                               TOKEN_LUALIKE_DDDOT,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_SQUARE_BRACKETS_L,TOKEN_END },                               TOKEN_LUALIKE_SUBS,ARGS_IGNORE),

            OperatorRP_Make((TT_Type[]){ TOKEN_PLUS_SIGN,TOKEN_EQUAL_SIGN,TOKEN_END },                      TOKEN_LUALIKE_ASD,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_MINUS_SIGN,TOKEN_EQUAL_SIGN,TOKEN_END },                     TOKEN_LUALIKE_ASU,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_ASTERISK,TOKEN_EQUAL_SIGN,TOKEN_END },                       TOKEN_LUALIKE_ASM,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_SLASH,TOKEN_EQUAL_SIGN,TOKEN_END },                          TOKEN_LUALIKE_ASV,ARGS_IGNORE),

            OperatorRP_Make((TT_Type[]){ TOKEN_AMPERSAND,TOKEN_END },                                       TOKEN_LUALIKE_AND,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_PIPE,TOKEN_END },                                            TOKEN_LUALIKE_OR,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_TILDE,TOKEN_END },                                           TOKEN_LUALIKE_NOT,ARGS_IGNORE),

            OperatorRP_Make((TT_Type[]){ TOKEN_AMPERSAND,TOKEN_AMPERSAND,TOKEN_END },                       TOKEN_LUALIKE_LND,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_PIPE,TOKEN_PIPE,TOKEN_END },                                 TOKEN_LUALIKE_LOR,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_EXCLAMATION_MARK,TOKEN_END },                                TOKEN_LUALIKE_LOT,ARGS_IGNORE),
    
            OperatorRP_Make((TT_Type[]){ TOKEN_MINUS_SIGN,TOKEN_MINUS_SIGN,TOKEN_END },                     TOKEN_LUALIKE_LINECOMMENT,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_SLASH,TOKEN_ASTERISK,TOKEN_END },                            TOKEN_LUALIKE_BLOCKCOMMENTSTART,ARGS_IGNORE),
            OperatorRP_Make((TT_Type[]){ TOKEN_ASTERISK,TOKEN_SLASH,TOKEN_END },                            TOKEN_LUALIKE_BLOCKCOMMENTEND,ARGS_IGNORE),
            OperatorRP_End()
        }),
        DirectiveMap_Make((Directive[]){
            Directive_End()
        }),
        DirectiveMap_Make((Directive[]){
            Directive_Comment_Line(TOKEN_LUALIKE_LINECOMMENT),
            Directive_Comment_Block(TOKEN_LUALIKE_BLOCKCOMMENTSTART,TOKEN_LUALIKE_BLOCKCOMMENTEND,True),
            Directive_End()
        }),
        ReseterMap_Make((Reseter[]){ 
            // operators who might have two versions: 1 arg vs. 2 args are not listed: -
            Reseter_New(TOKEN_NONE,RESETER_NONE),
            Reseter_New(TOKEN_NUMBER,RESETER_ADD1),
            Reseter_New(TOKEN_FLOAT,RESETER_ADD1),
            Reseter_New(TOKEN_STRING,RESETER_ADD1),
            Reseter_New(TOKEN_SPACE,RESETER_NONE),
            Reseter_New(TOKEN_NEWLINE,RESETER_RST),// without semicolon like python
            Reseter_New(TOKEN_CARTURN,RESETER_NONE),
            Reseter_New(TOKEN_HTAB,RESETER_NONE),
            Reseter_New(TOKEN_VTAB,RESETER_NONE),
            Reseter_New(TOKEN_COMMA,RESETER_RST),
            Reseter_New(TOKEN_PARENTHESES_R,RESETER_TO1),
            Reseter_New(TOKEN_SQUARE_BRACKETS_R,RESETER_NONE),
            Reseter_New(TOKEN_CURLY_BRACES_R,RESETER_NONE),
            Reseter_New(TOKEN_DOUBLE_ANGLE_BRACKETS_R,RESETER_NONE),
            Reseter_New(TOKEN_WHITE_SQUARE_BRACKETS_R,RESETER_NONE),
            Reseter_New(TOKEN_PARENTHESES_L,RESETER_RST),
            Reseter_New(TOKEN_SQUARE_BRACKETS_L,RESETER_RST),
            Reseter_New(TOKEN_CURLY_BRACES_L,RESETER_RST),
            Reseter_New(TOKEN_DOUBLE_ANGLE_BRACKETS_L,RESETER_RST),
            Reseter_New(TOKEN_WHITE_SQUARE_BRACKETS_L,RESETER_RST),
            Reseter_New(TOKEN_SEMICOLON,RESETER_RST),
            Reseter_New(TOKEN_COLON,RESETER_RST),
            Reseter_New(TOKEN_EXCLAMATION_MARK,RESETER_RST),
            Reseter_New(TOKEN_QUESTION_MARK,RESETER_RST),
            Reseter_New(TOKEN_ELLIPSIS,RESETER_RST),
            Reseter_New(TOKEN_PLUS_SIGN,RESETER_RST),
            Reseter_New(TOKEN_MULTIPLICATION_SIGN,RESETER_RST),
            Reseter_New(TOKEN_DIVISION_SIGN,RESETER_RST),
            Reseter_New(TOKEN_EQUAL_SIGN,RESETER_RST),
            Reseter_New(TOKEN_NOT_EQUAL_SIGN,RESETER_RST),
            Reseter_New(TOKEN_LESS_THAN,RESETER_RST),
            Reseter_New(TOKEN_GREATER_THAN,RESETER_RST),
            Reseter_New(TOKEN_LESS_THAN_OR_EQUAL_TO,RESETER_RST),
            Reseter_New(TOKEN_GREATER_THAN_OR_EQUAL_TO,RESETER_RST),
            Reseter_New(TOKEN_APPROXIMATION_SIGN,RESETER_RST),
            Reseter_New(TOKEN_PLUS_MINUS_SIGN,RESETER_RST),
            Reseter_New(TOKEN_AMPERSAND,RESETER_RST),
            Reseter_New(TOKEN_AT_SIGN,RESETER_RST),
            Reseter_New(TOKEN_ASTERISK,RESETER_RST),
            Reseter_New(TOKEN_CARET,RESETER_RST),
            Reseter_New(TOKEN_TILDE,RESETER_RST),
            Reseter_New(TOKEN_SLASH,RESETER_RST),
            Reseter_New(TOKEN_BACKSLASH,RESETER_RST),
            Reseter_New(TOKEN_UNDERSCORE,RESETER_RST),
            Reseter_New(TOKEN_PIPE,RESETER_RST),
            Reseter_New(TOKEN_PERCENT,RESETER_RST),
            Reseter_New(TOKEN_HASH_POUND_SIGN,RESETER_RST),
            Reseter_New(TOKEN_SINGLE_QUOTE,RESETER_RST),
            Reseter_New(TOKEN_DOUBLE_QUOTE,RESETER_RST),
            Reseter_New(TOKEN_BACKTICK,RESETER_RST),
            Reseter_New(TOKEN_PRIME,RESETER_RST),
            Reseter_New(TOKEN_DOUBLE_PRIME,RESETER_RST),
            Reseter_New(TOKEN_SECTION_SYMBOL,RESETER_RST),
            Reseter_New(TOKEN_DEGREE_SYMBOL,RESETER_RST),
            Reseter_New(TOKEN_FRACTION_SLASH,RESETER_RST),
            Reseter_New(TOKEN_INTERROBANG,RESETER_RST),
            Reseter_New(TOKEN_BULLET_POINT,RESETER_RST),
            Reseter_New(TOKEN_CAST,RESETER_ADD1),
            Reseter_New(TOKEN_FUNCTION,RESETER_ADD1),
            Reseter_New(TOKEN_CONSTSTRING_SINGLE,RESETER_ADD1),
            Reseter_New(TOKEN_CONSTSTRING_DOUBLE,RESETER_ADD1),
            Reseter_New(TOKEN_LUALIKE_RETURN,RESETER_RST),
            Reseter_New(TOKEN_END,RESETER_RST)
        }),
        ShutingYard_New(
            PrecedenceMap_Make((Precedencer[]){
                Precedencer_New(TOKEN_LUALIKE_ASS,1),
                Precedencer_New(TOKEN_LUALIKE_ADD,5),
                Precedencer_New(TOKEN_LUALIKE_SUB,6),
                Precedencer_New(TOKEN_LUALIKE_MUL,7),
                Precedencer_New(TOKEN_LUALIKE_DIV,8),
                Precedencer_New(TOKEN_LUALIKE_NEG,9),
                Precedencer_New(TOKEN_LUALIKE_EQU,3),
                Precedencer_New(TOKEN_LUALIKE_LES,3),
                Precedencer_New(TOKEN_LUALIKE_GRT,3),
                Precedencer_New(TOKEN_LUALIKE_LEQ,3),
                Precedencer_New(TOKEN_LUALIKE_GRQ,3),
                Precedencer_New(TOKEN_LUALIKE_ACS,11),
                Precedencer_New(TOKEN_LUALIKE_DDOT,PRECEDENCE_DONTCARE),
                Precedencer_New(TOKEN_LUALIKE_DDDOT,PRECEDENCE_DONTCARE),
                Precedencer_New(TOKEN_LUALIKE_SUBS,10),
                Precedencer_New(TOKEN_LUALIKE_ASD,1),
                Precedencer_New(TOKEN_LUALIKE_ASU,1),
                Precedencer_New(TOKEN_LUALIKE_ASM,1),
                Precedencer_New(TOKEN_LUALIKE_ASV,1),
                Precedencer_New(TOKEN_LUALIKE_AND,4),
                Precedencer_New(TOKEN_LUALIKE_OR,4),
                Precedencer_New(TOKEN_LUALIKE_NOT,9),
                Precedencer_New(TOKEN_LUALIKE_LND,2),
                Precedencer_New(TOKEN_LUALIKE_LOR,2),
                Precedencer_New(TOKEN_LUALIKE_LOT,3),
                Precedencer_New(TOKEN_PARENTHESES_L,PRECEDENCE_BRACKL),
                Precedencer_New(TOKEN_PARENTHESES_R,PRECEDENCE_BRACKR),
                Precedencer_New(TOKEN_LUALIKE_LINECOMMENT,PRECEDENCE_DONTCARE),
                Precedencer_New(TOKEN_LUALIKE_BLOCKCOMMENTSTART,PRECEDENCE_DONTCARE),
                Precedencer_New(TOKEN_LUALIKE_BLOCKCOMMENTEND,PRECEDENCE_DONTCARE),
                Precedencer_End
            }),
            ExecuteMap_Make((Executer[]){
                Executer_New(TOKEN_LUALIKE_ASS,2,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_ADD,2,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_SUB,2,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_MUL,2,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_DIV,2,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_NEG,1,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_EQU,2,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_LES,2,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_GRT,2,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_LEQ,2,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_GRQ,2,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_ACS,2,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_DDOT,2,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_DDDOT,2,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_SUBS,2,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_ASD,2,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_ASU,2,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_ASM,2,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_ASV,2,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_AND,2,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_OR,2,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_NOT,1,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_LND,2,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_LOR,2,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_LOT,1,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_LINECOMMENT,ARGS_IGNORE,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_BLOCKCOMMENTSTART,ARGS_IGNORE,Scope_StdHandler),
                Executer_New(TOKEN_LUALIKE_BLOCKCOMMENTEND,ARGS_IGNORE,Scope_StdHandler),
                Executer_End
            }),
            PreexecuteMap_Make((Preexecuter[]){
                Preexecuter_New(TOKEN_LUALIKE_ACS,(void*)ShutingYard_FunctionCall_Acs),
                Preexecuter_End
            })
        ),
        Scope_Make(
            (StdConstType[]){
                StdConstType_New(TOKEN_NUMBER,"int"),
                StdConstType_New(TOKEN_FLOAT,"float"),
                StdConstType_New(TOKEN_BOOL,"bool"),
                StdConstType_New(TOKEN_CONSTSTRING_DOUBLE,"str"),
                STDCONSTTYPE_END
            },
            VariableMap_Make((Variable[]){
                VARIABLE_END
            }),
            TypeMap_Make((Type[]){
                TYPE_END
            }),
        (void*)Enviroment_ErrorHandler),
        RangeChangerMap_Make((RangeChanger[]){
            RangeChanger_New(TOKEN_LUALIKE_IF,1),
            RangeChanger_New(TOKEN_LUALIKE_WHILE,1),
            RangeChanger_New(TOKEN_LUALIKE_FOR,1),
            RangeChanger_New(TOKEN_LUALIKE_END,-1),
            RANGECHANGER_END
        }),
        ExternPackageMap_Make((ExternPackage[]){
            EXTERNPACKAGE_END
        }),
        KeywordExecuterMap_Make((KeywordExecuter[]){
            KeywordExecuter_New(TOKEN_STRING,           (void*)ShutingYard_compress),
            KeywordExecuter_New(TOKEN_PARENTHESES_L,    (void*)ShutingYard_compress),
            KeywordExecuter_New(TOKEN_LUALIKE_FUNCTION, (void*)ShutingYard_function),
            KeywordExecuter_New(TOKEN_LUALIKE_RETURN,   (void*)ShutingYard_compress),
            KeywordExecuter_New(TOKEN_LUALIKE_IF,       (void*)ShutingYard_compress),
            KeywordExecuter_New(TOKEN_LUALIKE_ELIF,     (void*)ShutingYard_compress),
            KeywordExecuter_New(TOKEN_LUALIKE_ELSE,     (void*)ShutingYard_compress),
            KeywordExecuter_New(TOKEN_LUALIKE_WHILE,    (void*)ShutingYard_compress),
            KeywordExecuter_New(TOKEN_LUALIKE_FOR,      (void*)ShutingYard_compress),
            KEYWORDEXECUTER_END
        }),
        KeywordExecuterMap_Make((KeywordExecuter[]){
            KeywordExecuter_New(TOKEN_LUALIKE_IMPORT,   (void*)ShutingYard_Import),
            KeywordExecuter_New(TOKEN_LUALIKE_FROM,     (void*)ShutingYard_From),
            KeywordExecuter_New(TOKEN_LUALIKE_INCLUDE,  (void*)ShutingYard_Include),
            KeywordExecuter_New(TOKEN_LUALIKE_CLASS,    (void*)ShutingYard_Class),
            KeywordExecuter_New(TOKEN_LUALIKE_FUNCTION, (void*)ShutingYard_Function),
            KeywordExecuter_New(TOKEN_LUALIKE_IF,       (void*)ShutingYard_if),
            KeywordExecuter_New(TOKEN_LUALIKE_ELIF,     (void*)ShutingYard_elif),
            KeywordExecuter_New(TOKEN_LUALIKE_ELSE,     (void*)ShutingYard_else),
            KeywordExecuter_New(TOKEN_LUALIKE_WHILE,    (void*)ShutingYard_while),
            KeywordExecuter_New(TOKEN_LUALIKE_FOR,      (void*)ShutingYard_for),
            KeywordExecuter_New(TOKEN_LUALIKE_RETURN,   (void*)ShutingYard_return),
            KeywordExecuter_New(TOKEN_LUALIKE_END,      (void*)ShutingYard_end),
            KeywordExecuter_New(TOKEN_TYPE,             (void*)ShutingYard_type),
            KEYWORDEXECUTER_END
        }),
        DTT_TypeMap_Make((DTT_Type[]){
            DTT_Type_New(TOKEN_NEWLINE,Token_Null()),
            DTT_TYPE_END
        }),
        Token_By(TOKEN_LUALIKE_ASS,"="),
        (void*)LuaLikeIR_LogicCorrection,
        (void*)LuaLikeIR_Function_Handler
    );
    ll.logic = TOKEN_NONE;
    ll.indent = 0U;
    ll.src = CStr_Cpy(src);
    ll.output = CStr_Cpy(output);
    ll.dllpath = CStr_Cpy(dllpath);

    ll.text = String_New();

    ll.filesstack = CVector_New(sizeof(CStr),(void*)CStr_Free,NULL,(void*)CStr_PCmp,(void*)CStr_print);
    ll.filesinc = CVector_New(sizeof(CStr),(void*)CStr_Free,NULL,(void*)CStr_PCmp,(void*)CStr_print);
    ll.logicpath = Vector_New(sizeof(LogicBlock));

    Vector_Push(&ll.ev.epm,(ExternPackage[]){ ExternPackage_Make(ll.dllpath,"int","Ex_Packer",  (CStr[]){ "int",NULL },     &ll.ev.sc) });
    Vector_Push(&ll.ev.epm,(ExternPackage[]){ ExternPackage_Make(ll.dllpath,"bool","Ex_Packer", (CStr[]){ "bool",NULL },    &ll.ev.sc) });
    Vector_Push(&ll.ev.epm,(ExternPackage[]){ ExternPackage_Make(ll.dllpath,"float","Ex_Packer",(CStr[]){ "float",NULL },   &ll.ev.sc) });
    Vector_Push(&ll.ev.epm,(ExternPackage[]){ ExternPackage_Make(ll.dllpath,"str","Ex_Packer",  (CStr[]){ "str",NULL },     &ll.ev.sc) });
    
    CVector_Push(&ll.filesstack,(CStr[]){ CStr_Cpy(src) });
    CVector_Push(&ll.filesinc,(CStr[]){ CStr_Cpy(src) });
    Compiler_Script(&ll.ev,src);
    CVector_PopTop(&ll.filesstack);
    return ll;
}
void LuaLikeIR_PrintVariable(LuaLikeIR* ll,Variable* v) {
    Scope_PrintVariableDirect(&ll->ev.sc,v);
}
void LuaLikeIR_Build(LuaLikeIR* ll) {
    Compiler_Begin(&ll->ev);
    
    if(!ll->ev.error){
        String output = String_Format(";|\n;| LuaLikeIR by codeleaded\n;|\n\n");
        String_AppendString(&output,&ll->text);
        String_Append(&output,"\n");

        for(int i = 0;i<ll->ev.fs.size;i++){
            Function* func = (Function*)Vector_Get(&ll->ev.fs,i);
            if(func->text.size == 0) continue;

            String_Appendf(&output,"fn %s ",func->name);
            if(func->rettype){
               String_Appendf(&output,"%s %s0",func->rettype,COMPILER_RETURN);

                if(func->params.size>0)
                    String_Append(&output,",");
            }
            for(int i = 0;i<func->params.size;i++){
                Member* m = (Member*)Vector_Get(&func->params,i);
                String_Appendf(&output,"%s %s",m->type,m->name);

                if(i<func->params.size - 1)
                    String_Append(&output,",");
            }
            String_Appendf(&output,"\n");
            String_AppendString(&output,&func->text);
            String_Appendf(&output,"\n");
        }

        Files_WriteT(ll->output,output.Memory,output.size);
        String_Free(&output);
    }else{
        printf("%s-> %sbuild aborted because of Errors!%s\n",ANSI_FG_GRAY,ANSI_FG_RED,ANSI_FG_WHITE);
    }
}
void LuaLikeIR_Free(LuaLikeIR* ll) {
    String_Free(&ll->text);
    
    CStr_Free(&ll->dllpath);
    CStr_Free(&ll->src);
    CStr_Free(&ll->output);

    Compiler_Free(&ll->ev);

    CVector_Free(&ll->filesstack);
    CVector_Free(&ll->filesinc);

    Vector_Free(&ll->logicpath);
}
void LuaLikeIR_Print(LuaLikeIR* ll) {
    printf("--- LuaLikeIR ---\n");
    printf("dlls: %s, src: %s, output: %s\n",ll->dllpath,ll->src,ll->output);
    CVector_Print(&ll->filesinc);

    for(int i = 0;i<ll->ev.fs.size;i++){
        Function* func = (Function*)Vector_Get(&ll->ev.fs,i);
        if(func->text.size == 0) continue;
        
        CStr cstr = String_CStr(&func->text);
        printf("\n%s\n",cstr);
        CStr_Free(&cstr);
    }

    Compiler_Print(&ll->ev);
    printf("--------------------------\n");
}

#endif