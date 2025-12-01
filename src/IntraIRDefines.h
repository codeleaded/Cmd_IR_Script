#ifndef INTRAIRDEFINES_H
#define INTRAIRDEFINES_H

#include "/home/codeleaded/System/Static/Library/AlxParser.h"
#include "/home/codeleaded/System/Static/Library/LuaLikeDefines.h"

#define INTRAIR_IF                         "IF"
#define INTRAIR_ELIF                       "ELIF"
#define INTRAIR_ELSE                       "ELSE"
#define INTRAIR_WHILE                      "WHILE"
#define INTRAIR_FOR                        "FOR"
#define INTRAIR_LOG                        "LOG"

#define INTRAIR_INDENTATION                "    "
#define INTRAIR_INDENTATION_NONE           0
#define INTRAIR_INDENTATION_FUNCTION       1

typedef struct IntraIRVariable{
    char destroy;
    char padd1;
    short padd2;
    int padd3;
    void* parent;
} IntraIRVariable;

IntraIRVariable IntraIRVariable_New(char destroy,void* parent){
    IntraIRVariable sv;
    sv.destroy = destroy;
    sv.parent = parent;
    return sv;
}
void IntraIRVariable_Free(IntraIRVariable* sv){
    
}

#endif //!INTRAIRDEFINES_H