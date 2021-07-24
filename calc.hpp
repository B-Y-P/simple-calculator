#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define Assert(cond) if(!(cond)){ printf("Assertion fired\n"); (*(int *)0) = 0; }

#define INF INFINITY
#define MIN 6.1232339957367660e-17

#define  PI64 3.14159265358979323846
#define TAU64 6.28318530717958647692
#define   E64 2.71828182845904523536

enum op{
    CON, NEG,
    SIN, COS, TAN, SQRT, LOG, LN,
    ADD, SUB, MUL, DIV, MOD, EXP,
};

struct expr_node{
    op Type;
    double Value;
    expr_node *A, *B;
};

double Evaluate(expr_node *Node){
    switch(Node->Type){
        case(CON):  return Node->Value; break;
        case(NEG):  return       -Evaluate(Node->A);  break;
        case(SIN):  return    sin(Evaluate(Node->A)); break;
        case(COS):  return    cos(Evaluate(Node->A)); break;
        case(TAN):  return    tan(Evaluate(Node->A)); break;
        case(LN):   return    log(Evaluate(Node->A)); break;
        case(SQRT): return   sqrt(Evaluate(Node->A)); break;
        case(LOG):  return  log10(Evaluate(Node->A)); break;
        case(ADD):  return  Evaluate(Node->A) + Evaluate(Node->B); break;
        case(SUB):  return  Evaluate(Node->A) - Evaluate(Node->B); break;
        case(MUL):  return  Evaluate(Node->A) * Evaluate(Node->B); break;
        case(DIV):  return  Evaluate(Node->A) / Evaluate(Node->B); break;
        case(MOD):  return fmod(Evaluate(Node->A), Evaluate(Node->B)); break;
        case(EXP):  return  pow(Evaluate(Node->A), Evaluate(Node->B)); break;
        default: printf("Error Evaluating\n"); Assert(false); return NAN; break;
    }
}

enum token_type{
    Token_Unknown,
    Token_EndOfStream,
    Token_OpenParen, Token_CloseParen,
    Token_Number,
    Token_Plus, Token_Minus,
    Token_Asterisk, Token_ForwardSlash,
    Token_Caret,
    Token_Percent,
    Token_Sin, Token_Cos, Token_Tan,
    Token_Log, Token_Ln,
    Token_Sqrt,
    Token_PI, Token_TAU, Token_E,
    Token_ANS,
};

struct token{
    token_type Type;
    const char *Text;
    int Size;
};

struct tokenizer{
    const char *At;
    token Cur;
};

int OpPrec(token Token){
    switch(Token.Type){
        case Token_Plus: case Token_Minus:              return 1;
        case Token_Asterisk: case Token_ForwardSlash:   return 2;
        case Token_Caret:                               return 3;
        case Token_Percent:                             return 4;
        default: return 0;
    }
}

bool IsRight(token Token){ return Token.Type==Token_Caret; }

// Used for implicit multiplication, so Token_Minus isn't included
bool IsStartAtom(token Token){
    switch(Token.Type){
        case Token_OpenParen:
        case Token_Number:
        case Token_TAU:
        case Token_PI:
        case Token_E:
        case Token_Sin:
        case Token_Cos:
        case Token_Tan:
        case Token_Sqrt:
        case Token_Log:
        case Token_Ln:
            return true;
        default:
            return false;
    }
}

op GetOp(token Token){
    switch(Token.Type){
        case Token_Plus:          return ADD;
        case Token_Minus:         return SUB;
        case Token_Asterisk:      return MUL;
        case Token_ForwardSlash:  return DIV;
        case Token_Percent:       return MOD;
        case Token_Caret:         return EXP;
        case Token_Sin:           return SIN;
        case Token_Cos:           return COS;
        case Token_Tan:           return TAN;
        case Token_Log:           return LOG;
        case Token_Ln:            return LN;
        case Token_Sqrt:          return SQRT;
        default: Assert(false); return CON;
    }
}

// NOTE: Newlines are treated as ending an expression (though get_s() can't insert newlines)
bool IsWhiteSpace(char c){ return (c == ' ' || c == '\t'); }
bool IsNum(char c){ return (c >= '0' && c <= '9'); }

#define StrMatch(Tokenizer, Str) StrMatch_(Tokenizer, Str, sizeof(Str)-1)
bool StrMatch_(tokenizer *T, const char *Str, const int StrCount){
    for(int i=0; i<StrCount; i++)
        if(T->At[i] == '\0' || T->At[i] != Str[i]){ return false; }
    T->At += StrCount;
    return true;
}
void ConsumeWhiteSpace(tokenizer *T){ while(IsWhiteSpace(*T->At)){ T->At++; } }

token GetToken(tokenizer *Tokenizer){
    token Token = {Token_Unknown, 0, 1};        // Default token length of 1
    ConsumeWhiteSpace(Tokenizer);
    char c = *(Token.Text = Tokenizer->At++);
    switch(c){
        case '\0':      // Don't allow consuming EOS
        case '\n':      // '=' is for evaluating and outputting test cases
        case EOF:{ Token.Type = Token_EndOfStream; Token.Size=0; --Tokenizer->At;  } break;
        case '(':{ Token.Type = Token_OpenParen;    } break;
        case ')':{ Token.Type = Token_CloseParen;   } break;
        case '+':{ Token.Type = Token_Plus;         } break;
        case '-':{ Token.Type = Token_Minus;        } break;
        case '*':{ Token.Type = Token_Asterisk;     } break;
        case '/':{ Token.Type = Token_ForwardSlash; } break;
        case '^':{ Token.Type = Token_Caret;        } break;
        case '%':{ Token.Type = Token_Percent;      } break;
        case 'e':{ Token.Type = Token_E;            } break;
        default:{
            --Tokenizer->At;
            if(IsNum(c) || c == '.'){
                Token.Type = Token_Number;
                bool D = false;     // Decimal
                while(IsNum(*Tokenizer->At) || !D && (D |= (*Tokenizer->At=='.'))){ ++Tokenizer->At; }
            }
            else if(StrMatch(Tokenizer, "sin("))  { Token.Type = Token_Sin;  }
            else if(StrMatch(Tokenizer, "cos("))  { Token.Type = Token_Cos;  }
            else if(StrMatch(Tokenizer, "tan("))  { Token.Type = Token_Tan;  }
            else if(StrMatch(Tokenizer, "sqrt(")) { Token.Type = Token_Sqrt; }
            else if(StrMatch(Tokenizer, "log("))  { Token.Type = Token_Log;  }
            else if(StrMatch(Tokenizer, "ln("))   { Token.Type = Token_Ln;   }
            else if(StrMatch(Tokenizer, "PI"))    { Token.Type = Token_PI;   }
            else if(StrMatch(Tokenizer, "TAU"))   { Token.Type = Token_TAU;  }
            else if(StrMatch(Tokenizer, "ANS"))   { Token.Type = Token_ANS;  }

            else{ Token.Type = Token_Unknown; }
            Token.Size = Tokenizer->At - Token.Text;    // All valid default cases must correctly update Tokenizer->At
        } break;
    }
    return Tokenizer->Cur = Token;
}
token PeekToken(tokenizer *Tokenizer){
    tokenizer Temp = *Tokenizer;
    return GetToken(&Temp);
}
void ConsumePeeked(tokenizer *Tokenizer, token Peeked){
    Tokenizer->Cur = Peeked;
    Tokenizer->At = Peeked.Text + Peeked.Size;
}

expr_node* AddNode(op Op, expr_node *A=0, expr_node *B=0, double Value=0.0){
    expr_node *Node = (expr_node *)malloc(sizeof(expr_node));
    *Node = {Op, Value, A, B};
    return Node;
}
void FreeExprNode(expr_node *Node){
    if(Node){
        FreeExprNode(Node->A);
        FreeExprNode(Node->B);
        free(Node);
    }
}

static double PrevAns;
expr_node* ParseExpression(tokenizer *T, token_type EndToken=Token_EndOfStream, expr_node *A=nullptr);

expr_node* ParseAtom(tokenizer *Tokenizer){
    expr_node *Node = 0;
    token Token = GetToken(Tokenizer);
    switch(Token.Type){
        case Token_OpenParen:{ Node = ParseExpression(Tokenizer, Token_CloseParen);  } break;
        case Token_Number:{ Node = AddNode(CON, 0, 0, atof(Token.Text)); } break;
        case Token_ANS:   { Node = AddNode(CON, 0, 0, PrevAns); } break;
        case Token_TAU:   { Node = AddNode(CON, 0, 0,   TAU64); } break;
        case Token_PI:    { Node = AddNode(CON, 0, 0,    PI64); } break;
        case Token_E:     { Node = AddNode(CON, 0, 0,     E64); } break;

        case Token_Minus: { Node = AddNode(NEG, ParseAtom(Tokenizer)); } break;

        case Token_Sin:
        case Token_Cos:
        case Token_Tan:
        case Token_Log:
        case Token_Ln:
        case Token_Sqrt:{
            Node = AddNode(GetOp(Token), ParseExpression(Tokenizer, Token_CloseParen));
        } break;

        default:{ return 0; }
    }

    if(!IsWhiteSpace(Tokenizer->At[0]) && IsStartAtom(PeekToken(Tokenizer))){
        Node = AddNode(MUL, Node, ParseAtom(Tokenizer));   // Implicit multiplication
    }
    if(Node && Node->Type != CON){
        double Val = Evaluate(Node);
        FreeExprNode(Node);
        Node = AddNode(CON, 0, 0, Val);
    }
    return Node;
}

expr_node* ParseExpression(tokenizer *Tokenizer, token_type EndTokenType, expr_node *PreCalc){
    expr_node *A = (PreCalc ? PreCalc : ParseAtom(Tokenizer));
    if(A == nullptr){ return 0; }
    while(true){
        token Token = GetToken(Tokenizer);
        if(Token.Type == EndTokenType){ break; }

        int Precedence1 = OpPrec(Token);
        if(!Precedence1){ FreeExprNode(A); return 0; }

        expr_node *B = ParseAtom(Tokenizer);
        if(B == nullptr){ FreeExprNode(A); return 0; }

        token NextToken = PeekToken(Tokenizer);
        if(NextToken.Type == EndTokenType){
            ConsumePeeked(Tokenizer, NextToken);
            return AddNode(GetOp(Token), A, B);
        }

        int Precendence2 = OpPrec(NextToken);
        if(!Precendence2){ FreeExprNode(A); FreeExprNode(B); return 0; }

        if(Precedence1 < Precendence2 || (Token.Type == NextToken.Type && IsRight(Token))){
            B = ParseExpression(Tokenizer, EndTokenType, B);
            if(Tokenizer->Cur.Type == EndTokenType){ Tokenizer->At = Tokenizer->Cur.Text; }
        }

        A = AddNode(GetOp(Token), A, B);
    }
    return A;
}

double ParseString(const char *Expression){
    tokenizer Tokenizer = {Expression};
    token Token = PeekToken(&Tokenizer);
    if(Token.Type == Token_EndOfStream){ return 0.0; }

    expr_node *A = (OpPrec(Token) > 0 ? AddNode(CON,0,0,PrevAns) : nullptr);
    expr_node *Node = ParseExpression(&Tokenizer, Token_EndOfStream, A);
    if(Node){
        PrevAns = Evaluate(Node);
        FreeExprNode(Node);
        if(isnan(PrevAns)){ PrevAns = 0.0; return NAN; }
        if(isinf(PrevAns)){ PrevAns = 0.0; return INF; }
        if(fabs(PrevAns) <= MIN){ PrevAns = 0.0; }
        return PrevAns;
    }
    PrevAns = 0.0;
    return NAN;
}
