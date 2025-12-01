#include "IntraIR.h"

int main(int argc,char** argv){
    if(argc<=1){
        printf("Missing Src File ."LUALIKE_TYPE"!\n");
        return 1;
    }
    if(argc<=2){
        printf("Missing Output File .iralx!\n");
        return 1;
    }

    IntraIR ll = IntraIR_New("./bin","./code/Main.ll","./code/Main.iralx",64);
    IntraIR_Print(&ll);
    IntraIR_Build(&ll);
    //IntraIR_Print(&ll);
    IntraIR_Free(&ll);
    
    return 0;
}