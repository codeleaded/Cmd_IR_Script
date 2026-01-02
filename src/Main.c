#include "/home/codeleaded/System/Static/Library/IntraASM.h"

int main(int argc,char** argv){
    //if(argc<=1){
    //    printf("Missing Src File ."INTRAASM_TYPE"!\n");
    //    return 1;
    //}
    //if(argc<=2){
    //    printf("Missing Output File .iralx!\n");
    //    return 1;
    //}

    IntraASM ll = IntraASM_New("./bin","./code/Main.ll","./code/Main.iralx",64);
    //IntraASM_Print(&ll);
    IntraASM_Build(&ll);
    IntraASM_Free(&ll);
    
    return 0;
}