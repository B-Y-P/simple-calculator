#include "calc.hpp"
#include <string.h>
#include "test.h"

#define Max(a,b) ((a) < (b) ? (b) : (a))

int main(int argc, char **argv){
    char line[512];
    if(argc > 1 && strcmp(argv[1], "test") == 0){
        return test_cases(argc > 2 && strcmp(argv[2], "negative") == 0);
    }
    while(true){
        printf(">>> ");
        gets_s(line, sizeof(line));
        if(strcmp(line, "exit") == 0){ break; }
        double Ans = ParseString(line);
        !isnan(Ans) ? printf(" %g \n", Ans) : printf("Invalid Expression\n");
    }
    return 0;
}
