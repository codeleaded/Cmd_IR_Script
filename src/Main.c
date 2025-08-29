#include "LuaLikeIR.h"

int main(int argc,char** argv){
    if(argc<=1){
        printf("Missing Src File ."LUALIKE_TYPE"!\n");
        return 1;
    }
    if(argc<=2){
        printf("Missing Output File .iralx!\n");
        return 1;
    }

    LuaLikeIR ll = LuaLikeIR_New("./bin",argv[1],argv[2],64);
    LuaLikeIR_Print(&ll);
    LuaLikeIR_Build(&ll);
    //LuaLikeIR_Print(&ll);
    LuaLikeIR_Free(&ll);
    
    return 0;
}