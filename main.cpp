#include "calc.hpp"
#include <string.h>
#include "test.h"

int main(int argc, char **argv){
    char line[512];
    if(argc > 1 && strcmp(argv[1], "test") == 0){
        return test_cases(argc > 2 && strcmp(argv[2], "negative") == 0);
    }
    while(true){
        printf(">>> ");
        gets_s(line, sizeof(line));
        if(strcmp(line, "exit") == 0){ break; }
        double Ans = BeginParse(line);
        if(!isnan(Ans)){
            int Casted = Ans;
            (Ans - Casted == 0.0) ? printf(" %d \n", Casted) : printf(" %f \n", Ans);
        }
        else{ printf("Invalid Expression\n"); }
    }
    return 0;
}
