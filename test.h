#pragma once

#define ArrayCount(a) (sizeof(a)/sizeof(a[0]))

struct test_case{
    char *Expression;
    double Expected;
    char *Comment;
};

bool test(test_case *Cases, int Count){
    bool Passed = true;
    printf("\nBeginning Tests\n================\n");
    for(int i=0; i<Count; i++){
        test_case *Case = Cases + i;

        if(Case->Comment){ printf("<= %s =>\n", Case->Comment); }
        printf(">>> \"%s\" ", Case->Expression);

        bool ExpectNAN = isnan(Case->Expected);
        if(!ExpectNAN){ printf("= %f\n", Case->Expected); }

        int Offset = (!ExpectNAN)*(strlen(Case->Expression) + 5);
        double Ans = BeginParse(Case->Expression);

        bool CasePasses;
        if(!isnan(Ans)){
            CasePasses = ((isinf(Case->Expected) && isinf(Ans)) ||  (fabs(Ans - Case->Expected) < 0.000001));
            printf("Ans:%*c%f", Offset, ' ', Ans);
        }else{
            CasePasses = ExpectNAN;
            printf("\nInvalid Expression");
        }
        printf("  %s\n\n", (CasePasses ? "PASSES" : "FAILS"));
        Passed &= CasePasses;
    }
    printf("Test cases %s\n", (Passed ? "PASSED" : "FAILED"));
    return Passed;
}

int test_cases(bool DoNegativeTests){
    test_case tests[] = {
        // (Note: Unary minus doesn't apply on first atom of expression when there's a valid previous expression)
        {"Invalid",         NAN,        "This will be skipped"},
        {"-.5",             -0.5,       "Reading a constant"},
        {"2.4 + 3",         5.4,        "Basic calculation"},
        {"5 % 2",           1,          "Modulus"},
        {"5.2 % 2",         1.2,        "Floating Modulus"},
        {"(-8 % 3)",        -2,         "Negative Modulus"},
        {"2 ^ 3 ^ 2",       512,        "Right associative Exponents"},
        {"2 + -(6 - 4*2)",  4,          "Negative Atoms, Parentheses"},
        {"PI",              3.141593,   "Constants"},
        {"e",               2.718282,   "Euler's Constant"},
        {"TAU",             6.283185,    0},
        {"2PI",             6.283185,   "Implicit Multiplication"},
        {"(3+2)(5-1)",      20,         0},
        {"cos(PI/2)",       0.0,        "Trig expressions"},
        {"tan(PI/4.0)",     1.0,        0},
        {"2+6 - sin(4*2)",  7.010642,   0},
        {"sqrt(4.0)",       2.0,        "Sqrt expressions"},
        {"sqrt(5.0)",       2.236068,   0},
        {"sqrt(-1)",        NAN,        "NAN"},
        {"log(100)",        2,          "Logarithmic expressions"},
        {"log(2)",          0.301030,   0},
        {"ln(2)",           0.6931471,  0},
        {"ln(e)",           1,          0},
        {"2(4 + 3)",        14,         0},
        {"3 + (4 * (5-3))", 11,         0},
        {"* 2",             22,         "Using previous answer implicitly"},
        {"ANS - 4",         18,         "Using previous answer explicitly"},
        {"ANS",             18,         0},
        {"4 / 0",           INF,        "Infinity"},
        {"ANS",             0.0,        "Answer after infinity is zero"},
        {"(5 / 0)*0",       NAN,        "Infinity * 0 = -NAN 'cuz that's what the FPU says"},
        {"",                0.0,        "Yields 0"},
        {"2 + ",            NAN,        0},
        {"ANS",             0.0,        "ANS after Invalid Expression becomes 0"},
        {"2 + (",           NAN,        0},
        {"-2",              -2,         "Unary minus of first atom of expression becomes negation when previous expression was invalid"},
        {")",               NAN,        0},
        {"3 1 +",           NAN,        "RPN is cringe"},
    };
    int Skip = 1;
    Assert(Skip > 0);
    bool Passed = test(tests+Skip, ArrayCount(tests)-Skip);
    if(DoNegativeTests){
        test_case negative_tests[] = {
            {"-.5",             -1.5,       "Unequal constant"},
            {"",                NAN,        "0.0 Expression != NAN"},
            {") 2",             1,          "Invalid Expression != 1"},
        };
        printf("\n\nThese are meant to fail\n");
        test(negative_tests, ArrayCount(negative_tests));
    }
    return (Passed ? 0 : -1);
}
