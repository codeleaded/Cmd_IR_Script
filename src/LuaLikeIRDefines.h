#ifndef LUALIKEIRDEFINES_H
#define LUALIKEIRDEFINES_H

#include "/home/codeleaded/System/Static/Library/AlxParser.h"
#include "/home/codeleaded/System/Static/Library/LuaLikeDefines.h"

#define LUALIKEIR_IF                         "IF"
#define LUALIKEIR_ELIF                       "ELIF"
#define LUALIKEIR_ELSE                       "ELSE"
#define LUALIKEIR_WHILE                      "WHILE"
#define LUALIKEIR_FOR                        "FOR"
#define LUALIKEIR_LOG                        "LOG"

#define LUALIKEIR_INDENTATION                "    "
#define LUALIKEIR_INDENTATION_NONE           0
#define LUALIKEIR_INDENTATION_FUNCTION       1

typedef struct LuaLikeIRVariable{
    char destroy;
    char padd1;
    short padd2;
    int padd3;
    void* parent;
} LuaLikeIRVariable;

LuaLikeIRVariable LuaLikeIRVariable_New(char destroy,void* parent){
    LuaLikeIRVariable sv;
    sv.destroy = destroy;
    sv.parent = parent;
    return sv;
}
void LuaLikeIRVariable_Free(LuaLikeIRVariable* sv){
    
}

#endif //!LUALIKEIRDEFINES_H