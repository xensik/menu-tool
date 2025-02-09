// Copyright 2022 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#include "stdafx.hpp"
#include "iw5.hpp"
#include "memory.hpp"
#include "eval.hpp"

namespace xsk::menu::iw5
{

char s_precedence[EVAL_OP_COUNT] { 99, 0, 3, 2, 11, 11, 13, 13, 12, 12, 12, 10, 10, 13, 7, 5, 6, 13, 2, 1, 8, 8, 9, 9, 9, 9 };
bool s_rightToLeft[EVAL_OP_COUNT] { 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
int s_consumedOperandCount[EVAL_OP_COUNT] { 1, -1, 2, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1 };

bool Eval_IsUnaryOp(Eval *eval)
{
    if (eval->valStackPos)
        return eval->pushedOp;

    return true;
}

bool Eval_CanPushValue(Eval *eval)
{
    if (!eval->valStackPos)
        return true;

    if (eval->valStackPos == 1024)
    {
        throw std::runtime_error("evaluation stack overflow - expression is too complex");
    }

    return eval->pushedOp;
}

void Eval_PrepareBinaryOpSameTypes(Eval *eval)
{
    if (eval->valStackPos < 2)
    {
        throw std::runtime_error("missing operand (for example, 'a + ' or ' / b')");
    }

    if (eval->valStack[eval->valStackPos - 2].type == EVAL_VALUE_STRING || eval->valStack[eval->valStackPos - 1].type == EVAL_VALUE_STRING)
    {
        throw std::runtime_error("operation not valid on strings");
    }

    if (eval->valStack[eval->valStackPos - 2].type != eval->valStack[eval->valStackPos - 1].type)
    {
        int val;

        if (eval->valStack[eval->valStackPos - 2].type == EVAL_VALUE_INT)
        {
            eval->valStack[eval->valStackPos - 2].u.d = (double)eval->valStack[eval->valStackPos - 2].u.i;
            val = eval->valStackPos - 2;
        }
        else
        {
            eval->valStack[eval->valStackPos - 1].u.d = (double)eval->valStack[eval->valStackPos - 1].u.i;
            val = eval->valStackPos - 1;
        }

        eval->valStack[val].type = EVAL_VALUE_DOUBLE;
    }
}

void Eval_PrepareBinaryOpIntegers(Eval *eval)
{
    if (eval->valStackPos < 2)
    {
        throw std::runtime_error("missing operand (for example, 'a + ' or ' / b')");
    }

    if (eval->valStack[eval->valStackPos - 2].type == EVAL_VALUE_STRING || eval->valStack[eval->valStackPos - 1].type == EVAL_VALUE_STRING)
    {
        throw std::runtime_error("operation not valid on strings");
    }

    if (eval->valStack[eval->valStackPos - 2].type == EVAL_VALUE_DOUBLE)
    {
        eval->valStack[eval->valStackPos - 2].u.i = (signed int)eval->valStack[eval->valStackPos - 2].u.d;
        eval->valStack[eval->valStackPos - 2].type = EVAL_VALUE_INT;
    }

    if (eval->valStack[eval->valStackPos - 1].type == EVAL_VALUE_DOUBLE)
    {
        eval->valStack[eval->valStackPos - 1].u.i = (signed int)eval->valStack[eval->valStackPos - 1].u.d;
        eval->valStack[eval->valStackPos - 1].type = EVAL_VALUE_INT;
    }
}
void Eval_PrepareBinaryOpBoolean(Eval *eval)
{
    if (eval->valStackPos < 2)
    {
        throw std::runtime_error("missing operand (for example, 'a + ' or ' / b')");
    }

    if (eval->valStack[eval->valStackPos - 2].type == EVAL_VALUE_STRING || eval->valStack[eval->valStackPos - 1].type == EVAL_VALUE_STRING)
    {
        throw std::runtime_error("operation not valid on strings");
    }

    if (eval->valStack[eval->valStackPos - 2].type)
    {
        eval->valStack[eval->valStackPos - 2].u.i = eval->valStack[eval->valStackPos - 2].u.i != 0;
    }
    else
    {
        eval->valStack[eval->valStackPos - 2].u.i = eval->valStack[eval->valStackPos - 2].u.d != 0.0;
        eval->valStack[eval->valStackPos - 2].type = EVAL_VALUE_INT;
    }

    if (eval->valStack[eval->valStackPos - 1].type)
    {
        eval->valStack[eval->valStackPos - 1].u.i = eval->valStack[eval->valStackPos - 1].u.i != 0;
    }
    else
    {
        eval->valStack[eval->valStackPos - 1].u.i = eval->valStack[eval->valStackPos - 1].u.d != 0.0;
        eval->valStack[eval->valStackPos - 1].type = EVAL_VALUE_INT;
    }
}

bool Eval_EvaluationStep(Eval *eval)
{
    if (!eval->opStackPos)
        return false;

    if (eval->opStack[--eval->opStackPos] == EVAL_OP_LPAREN)
        return true;

    if (eval->opStack[eval->opStackPos] == EVAL_OP_QUESTION)
    {
        throw std::runtime_error("found '?' with no following ':' in expression of type 'a ? b : c'");
    }

    if (!eval->valStackPos)
    {
        throw std::runtime_error("missing operand (for example, 'a + ' or ' / b')");
    }

    int opcode = eval->opStack[eval->opStackPos];

    switch(opcode)
    {
        case EVAL_OP_LPAREN:
        case EVAL_OP_RPAREN:
            false;
        case EVAL_OP_COLON:
        {
            if (eval->valStackPos < 3)
            {
                throw std::runtime_error("missing operand (for example, 'a + ' or ' / b')");
            }

            int val;

            if (eval->valStack[eval->valStackPos - 3].type)
            {
                if (eval->valStack[eval->valStackPos - 3].type != EVAL_VALUE_INT)
                {
                    throw std::runtime_error("can only switch on numbers");
                }

                val = -(eval->valStack[eval->valStackPos - 3].u.i != 0) - 1;
            }
            else
            {
                if ( eval->valStack[eval->valStackPos - 3].u.d == 0.0 )
                    val = -1;
                else
                    val = -2;
            }

            if (eval->valStack[eval->valStackPos - 2].type != EVAL_VALUE_STRING || eval->valStack[eval->valStackPos - 1].type != EVAL_VALUE_STRING)
            {
                Eval_PrepareBinaryOpSameTypes(eval);
            }
            else
            {
                std::free(eval->valStack[1 - val + eval->valStackPos].u.s);
            }

            std::memcpy(&eval->valStack[eval->valStackPos - 3], &eval->valStack[val + eval->valStackPos], sizeof(EvalValue));
            eval->valStackPos -= 2;
            --eval->opStackPos;
            return true;
        }
        case EVAL_OP_QUESTION:
            return false;
        case EVAL_OP_PLUS:
        {
            if (eval->valStackPos < 2 || eval->valStack[eval->valStackPos - 2].type != EVAL_VALUE_STRING || eval->valStack[eval->valStackPos - 1].type != EVAL_VALUE_STRING)
            {
                Eval_PrepareBinaryOpSameTypes(eval);
        
                if (eval->valStack[eval->valStackPos - 2].type)
                    eval->valStack[eval->valStackPos - 2].u.i += eval->valStack[eval->valStackPos - 1].u.i;
                else
                    eval->valStack[eval->valStackPos - 2].u.d = eval->valStack[eval->valStackPos - 2].u.d + eval->valStack[eval->valStackPos - 1].u.d;
            }
            else
            {
                auto l1 = strlen(eval->valStack[eval->valStackPos - 2].u.s);
                auto l2 = strlen(eval->valStack[eval->valStackPos - 1].u.s);
                auto str = (char *)std::malloc(l1 + l2 + 1);
                std::memcpy(str, eval->valStack[eval->valStackPos - 2].u.s, l1);
                std::memcpy(&str[l1], eval->valStack[eval->valStackPos - 1].u.s, l2);
                str[l1+l2] = '\0';
                std::free(eval->valStack[eval->valStackPos - 2].u.s);
                std::free(eval->valStack[eval->valStackPos - 1].u.s);
                eval->valStack[eval->valStackPos - 2].u.s = str;
            }
            --eval->valStackPos;
            return true;
        }
        case EVAL_OP_MINUS:
        {
            Eval_PrepareBinaryOpSameTypes(eval);

            if (eval->valStack[eval->valStackPos - 2].type)
                eval->valStack[eval->valStackPos - 2].u.i -= eval->valStack[eval->valStackPos - 1].u.i;
            else
                eval->valStack[eval->valStackPos - 2].u.d = eval->valStack[eval->valStackPos - 2].u.d - eval->valStack[eval->valStackPos - 1].u.d;
            --eval->valStackPos;
            return true;
        }
        case EVAL_OP_UNARY_PLUS:
            return true;
        case EVAL_OP_UNARY_MINUS:
        {
            if (eval->valStack[eval->valStackPos - 1].type == EVAL_VALUE_INT)
            {
                eval->valStack[eval->valStackPos - 1].u.i = -eval->valStack[eval->valStackPos - 1].u.i;
            }
            else
            {
                if (eval->valStack[eval->valStackPos - 1].type)
                {
                    throw std::runtime_error("cannot negate strings");
                }
                eval->valStack[eval->valStackPos - 1].u.d = -eval->valStack[eval->valStackPos - 1].u.d;
            }

            return true;
        }
        case EVAL_OP_MULTIPLY:
        {
            Eval_PrepareBinaryOpSameTypes(eval);
            if (eval->valStack[eval->valStackPos - 2].type)
                eval->valStack[eval->valStackPos - 2].u.i *= eval->valStack[eval->valStackPos - 1].u.i;
            else
                eval->valStack[eval->valStackPos - 2].u.d = eval->valStack[eval->valStackPos - 2].u.d * eval->valStack[eval->valStackPos - 1].u.d;
            --eval->valStackPos;
            return true;
        }
        case EVAL_OP_DIVIDE:
        {
            Eval_PrepareBinaryOpSameTypes(eval);
            if (eval->valStack[eval->valStackPos - 2].type)
            {
                if (!eval->valStack[eval->valStackPos - 1].u.i)
                {
                    throw std::runtime_error("divide by zero");
                }
                eval->valStack[eval->valStackPos - 2].u.i /= eval->valStack[eval->valStackPos - 1].u.i;
            }
            else
            {
                if (eval->valStack[eval->valStackPos - 1].u.d == 0.0)
                {
                    throw std::runtime_error("divide by zero");
                }
                eval->valStack[eval->valStackPos - 2].u.d = eval->valStack[eval->valStackPos - 2].u.d / eval->valStack[eval->valStackPos - 1].u.d;
            }
            --eval->valStackPos;
            return true;
        }
        case EVAL_OP_MODULUS:
        {
            Eval_PrepareBinaryOpSameTypes(eval);
            if (eval->valStack[eval->valStackPos - 2].type)
            {
                if (!eval->valStack[eval->valStackPos - 1].u.i)
                {
                    throw std::runtime_error("divide by zero");
                }
                eval->valStack[eval->valStackPos - 2].u.i %= eval->valStack[eval->valStackPos - 1].u.i;
            }
            else
            {
                if (eval->valStack[eval->valStackPos - 1].u.d == 0.0)
                {
                    throw std::runtime_error("divide by zero");
                }
                auto num = floor(eval->valStack[eval->valStackPos - 2].u.d / eval->valStack[eval->valStackPos - 1].u.d);
                eval->valStack[eval->valStackPos - 2].u.d = eval->valStack[eval->valStackPos - 2].u.d - eval->valStack[eval->valStackPos - 1].u.d * num;
            }
            --eval->valStackPos;
            return true;
        }
        case EVAL_OP_LSHIFT:
        {
            Eval_PrepareBinaryOpSameTypes(eval);
            if (eval->valStack[eval->valStackPos - 2].type)
            {
                eval->valStack[eval->valStackPos - 2].u.i <<= eval->valStack[eval->valStackPos - 1].u.i;
            }
            else
            {
                auto num = pow(2.0, eval->valStack[eval->valStackPos - 1].u.d);
                eval->valStack[eval->valStackPos - 2].u.d = num * eval->valStack[eval->valStackPos - 2].u.d;
            }
            --eval->valStackPos;
            return true;
        }
        case EVAL_OP_RSHIFT:
        {
            Eval_PrepareBinaryOpSameTypes(eval);
            if (eval->valStack[eval->valStackPos - 2].type)
            {
                eval->valStack[eval->valStackPos - 2].u.i >>= eval->valStack[eval->valStackPos - 1].u.i;
            }
            else
            {
                auto num = pow(2.0, -eval->valStack[eval->valStackPos - 1].u.d);
                eval->valStack[eval->valStackPos - 2].u.d = num * eval->valStack[eval->valStackPos - 2].u.d;
            }
            --eval->valStackPos;
            return true;
        }
        case EVAL_OP_BITWISE_NOT:
        {
            if (eval->valStack[eval->valStackPos - 1].type)
            {
                if (eval->valStack[eval->valStackPos - 2].type == EVAL_VALUE_STRING)
                {
                    throw std::runtime_error("cannot bitwise invert strings");
                }
            }
            else
            {
                eval->valStack[eval->valStackPos - 1].u.i = (signed int)eval->valStack[eval->valStackPos - 1].u.d;
                eval->valStack[eval->valStackPos - 1].type = EVAL_VALUE_INT;
            }
            eval->valStack[eval->valStackPos - 1].u.i = ~eval->valStack[eval->valStackPos - 1].u.i;
            return true;
        }
        case EVAL_OP_BITWISE_AND:
        {
            Eval_PrepareBinaryOpIntegers(eval);
            eval->valStack[eval->valStackPos - 2].u.i &= eval->valStack[eval->valStackPos - 1].u.i;
            --eval->valStackPos;
            return true;
        }
        case EVAL_OP_BITWISE_OR:
        {
            Eval_PrepareBinaryOpIntegers(eval);
            eval->valStack[eval->valStackPos - 2].u.i |= eval->valStack[eval->valStackPos - 1].u.i;
            --eval->valStackPos;
            return true;
        }
        case EVAL_OP_BITWISE_XOR:
        {
            Eval_PrepareBinaryOpIntegers(eval);
            eval->valStack[eval->valStackPos - 2].u.i ^= eval->valStack[eval->valStackPos - 1].u.i;
            --eval->valStackPos;
            return true;
        }
        case EVAL_OP_LOGICAL_NOT:
        {
            if (eval->valStack[eval->valStackPos - 1].type)
            {
                if (eval->valStack[eval->valStackPos - 1].type != EVAL_VALUE_INT)
                {
                    throw std::runtime_error("cannot logical invert strings");
                }
                eval->valStack[eval->valStackPos - 1].u.i = eval->valStack[eval->valStackPos - 1].u.i == 0;
            }
            else
            {
                eval->valStack[eval->valStackPos - 1].u.i = eval->valStack[eval->valStackPos - 1].u.d == 0.0;
            }
            eval->valStack[eval->valStackPos - 1].type = EVAL_VALUE_INT;
            return true;
        }
        case EVAL_OP_LOGICAL_AND:
        {
            Eval_PrepareBinaryOpBoolean(eval);
            eval->valStack[eval->valStackPos - 2].u.i &= eval->valStack[eval->valStackPos - 1].u.i;
            --eval->valStackPos;
            return true;
        }
        case EVAL_OP_LOGICAL_OR:
        {
            Eval_PrepareBinaryOpBoolean(eval);
            eval->valStack[eval->valStackPos - 2].u.i |= eval->valStack[eval->valStackPos - 1].u.i;
            --eval->valStackPos;
            return true;
        }
        case EVAL_OP_EQUALS:
        {
            if ( eval->valStackPos < 2 || eval->valStack[eval->valStackPos - 2].type != EVAL_VALUE_STRING || eval->valStack[eval->valStackPos - 1].type != EVAL_VALUE_STRING)
            {
                Eval_PrepareBinaryOpSameTypes(eval);
                if (eval->valStack[eval->valStackPos - 2].type)
                {
                    eval->valStack[eval->valStackPos - 2].u.i = eval->valStack[eval->valStackPos - 2].u.i == eval->valStack[eval->valStackPos - 1].u.i;
                }
                else
                {
                    eval->valStack[eval->valStackPos - 2].u.i = eval->valStack[eval->valStackPos - 2].u.d == eval->valStack[eval->valStackPos - 1].u.d;
                    eval->valStack[eval->valStackPos - 2].type = EVAL_VALUE_INT;
                }
            }
            else
            {
                auto value = stricmp(eval->valStack[eval->valStackPos - 2].u.s, eval->valStack[eval->valStackPos - 1].u.s) == 0;
                std::free(eval->valStack[eval->valStackPos - 2].u.s);
                std::free(eval->valStack[eval->valStackPos - 1].u.s);
                eval->valStack[eval->valStackPos - 2].type = EVAL_VALUE_INT;
                eval->valStack[eval->valStackPos - 2].u.i = value;
            }
            --eval->valStackPos;
            return true;
        }
        case EVAL_OP_NOT_EQUAL:
        {
            if (eval->valStackPos < 2 || eval->valStack[eval->valStackPos - 2].type != EVAL_VALUE_STRING || eval->valStack[eval->valStackPos - 1].type != EVAL_VALUE_STRING)
            {
                Eval_PrepareBinaryOpSameTypes(eval);
                if (eval->valStack[eval->valStackPos - 2].type)
                {
                    eval->valStack[eval->valStackPos - 2].u.i = eval->valStack[eval->valStackPos - 2].u.i != eval->valStack[eval->valStackPos - 1].u.i;
                }
                else
                {
                    eval->valStack[eval->valStackPos - 2].u.i = eval->valStack[eval->valStackPos - 2].u.d != eval->valStack[eval->valStackPos - 1].u.d;
                    eval->valStack[eval->valStackPos - 2].type = EVAL_VALUE_INT;
                }
            }
            else
            {
                auto value = stricmp(eval->valStack[eval->valStackPos - 2].u.s, eval->valStack[eval->valStackPos - 1].u.s) == 0;
                std::free(eval->valStack[eval->valStackPos - 2].u.s);
                std::free(eval->valStack[eval->valStackPos - 1].u.s);
                eval->valStack[eval->valStackPos - 2].type = EVAL_VALUE_INT;
                eval->valStack[eval->valStackPos - 2].u.i = value == 0;
            }
            --eval->valStackPos;
            return true;
        }
        case EVAL_OP_LESS:
        {
            Eval_PrepareBinaryOpSameTypes(eval);
            if (eval->valStack[eval->valStackPos - 2].type)
            {
                eval->valStack[eval->valStackPos - 2].u.i = eval->valStack[eval->valStackPos - 2].u.i < eval->valStack[eval->valStackPos - 1].u.i;
            }
            else
            {
                eval->valStack[eval->valStackPos - 2].u.i = eval->valStack[eval->valStackPos - 1].u.d > eval->valStack[eval->valStackPos - 2].u.d;
                eval->valStack[eval->valStackPos - 2].type = EVAL_VALUE_INT;
            }
            --eval->valStackPos;
            return true;
        }
        case EVAL_OP_LESS_EQUAL:
        {
            Eval_PrepareBinaryOpSameTypes(eval);
            if (eval->valStack[eval->valStackPos - 2].type)
            {
                eval->valStack[eval->valStackPos - 2].u.i = eval->valStack[eval->valStackPos - 2].u.i <= eval->valStack[eval->valStackPos - 1].u.i;
            }
            else
            {
                eval->valStack[eval->valStackPos - 2].u.i = eval->valStack[eval->valStackPos - 1].u.d >= eval->valStack[eval->valStackPos - 2].u.d;
                eval->valStack[eval->valStackPos - 2].type = EVAL_VALUE_INT;
            }
            --eval->valStackPos;
            return true;
        }
        case EVAL_OP_GREATER:
        {
            Eval_PrepareBinaryOpSameTypes(eval);
            if (eval->valStack[eval->valStackPos - 2].type)
            {
                eval->valStack[eval->valStackPos - 2].u.i = eval->valStack[eval->valStackPos - 2].u.i > eval->valStack[eval->valStackPos - 1].u.i;
            }
            else
            {
                eval->valStack[eval->valStackPos - 2].u.i = eval->valStack[eval->valStackPos - 2].u.d > eval->valStack[eval->valStackPos - 1].u.d;
                eval->valStack[eval->valStackPos - 2].type = EVAL_VALUE_INT;
            }
            --eval->valStackPos;
            return true;
        }
        case EVAL_OP_GREATER_EQUAL:
        {
            Eval_PrepareBinaryOpSameTypes(eval);
            if (eval->valStack[eval->valStackPos - 2].type)
            {
                eval->valStack[eval->valStackPos - 2].u.i = eval->valStack[eval->valStackPos - 2].u.i >= eval->valStack[eval->valStackPos - 1].u.i;
            }
            else
            {
                eval->valStack[eval->valStackPos - 2].u.i = eval->valStack[eval->valStackPos - 2].u.d >= eval->valStack[eval->valStackPos - 1].u.d;
                eval->valStack[eval->valStackPos - 2].type = EVAL_VALUE_INT;
            }
            --eval->valStackPos;
        }
        default:
            throw std::runtime_error("unknown operator type");
            return false;
    }
}

auto Eval_Solve(EvalValue *result, Eval *eval) -> EvalValue*
{
    if (eval->parenCount)
    {
        throw std::runtime_error("missing ')'");
    }

    bool end = true;

    while (end)
    {
        end = Eval_EvaluationStep(eval);
    }

    if (eval->valStackPos > 1)
    {
        throw std::runtime_error("extra operand (for example, 'a b +')");
    }

    result->type = eval->valStack[0].type;
    result->u.d = eval->valStack[0].u.d;
    return result;
}

bool Eval_PushInteger(Eval *eval, int value)
{
    if (!Eval_CanPushValue(eval))
        return false;

    eval->valStack[eval->valStackPos].type = EVAL_VALUE_INT;
    eval->valStack[eval->valStackPos++].u.i = value;
    eval->pushedOp = false;
    return true;
}

bool Eval_PushNumber(Eval *eval, long double value)
{
    if (!Eval_CanPushValue(eval))
        return false;

    eval->valStack[eval->valStackPos].type = EVAL_VALUE_DOUBLE;
    eval->valStack[eval->valStackPos++].u.d = value;
    eval->pushedOp = false;
    return true;
}

bool Eval_PushOperator(Eval *eval, EvalOperatorType op)
{
    bool leftToRight;
    char precedence;
    bool higherPrecedence;

    if (s_precedence[op] < 0)
        return false;

    if (op == EVAL_OP_RPAREN && !eval->parenCount)
        return false;

    if (op == EVAL_OP_LPAREN )
    {
        if (eval->valStackPos && !eval->pushedOp)
            return false;

        ++eval->parenCount;
    }

    if (op == EVAL_OP_PLUS)
    {
        if (Eval_IsUnaryOp(eval))
            op = EVAL_OP_UNARY_PLUS;
    }
    else if (op == EVAL_OP_MINUS && Eval_IsUnaryOp(eval))
    {
        op = EVAL_OP_UNARY_MINUS;
    }

    precedence = s_precedence[op];

    while (eval->opStackPos > 0)
    {
        higherPrecedence = s_precedence[eval->opStack[eval->opStackPos - 1]] > precedence;
        leftToRight = s_precedence[eval->opStack[eval->opStackPos - 1]] == precedence && !s_rightToLeft[eval->opStack[eval->opStackPos - 1]];
        
        if (!higherPrecedence && !leftToRight)
            break;

        if (eval->opStack[eval->opStackPos - 1] == EVAL_OP_LPAREN)
        {
            if ( op == EVAL_OP_RPAREN )
            {
                --eval->parenCount;
                --eval->opStackPos;
                eval->pushedOp = false;
                return true;
            }
            break;
        }

        if (!Eval_EvaluationStep(eval))
            return false;
    }

    if (op != EVAL_OP_COLON || eval->opStackPos && eval->opStack[eval->opStackPos - 1] == EVAL_OP_QUESTION)
    {
        if (eval->opStackPos == 1024)
        {
            throw std::runtime_error("evaluation stack overflow - expression is too complex");
        }
    
        eval->opStack[eval->opStackPos++] = op;
        eval->pushedOp = true;
        return true;
    }
    
    if (eval->parenCount)
    {
        throw std::runtime_error("found ':' without preceding '?' in expression of type 'a ? b : c'");
    }

    return false;
}

bool Eval_OperatorForToken(const char *text, EvalOperatorType *op)
{
    switch ( *text )
    {
        case '!':
            *op = (text[1] == '=') ? EVAL_OP_NOT_EQUAL : EVAL_OP_LOGICAL_NOT;
            return true;
        case '%':
            *op = EVAL_OP_MODULUS;
            return true;
        case '&':
            *op = (text[1] == '&') ? EVAL_OP_LOGICAL_AND : EVAL_OP_BITWISE_AND;
            return true;
        case '(':
            *op = EVAL_OP_LPAREN;
            return true;
        case ')':
            *op = EVAL_OP_RPAREN;
            return true;
        case '*':
            *op = EVAL_OP_MULTIPLY;
            return true;
        case '+':
            *op = EVAL_OP_PLUS;
            return true;
        case '-':
            *op = EVAL_OP_MINUS;
            return true;
        case '/':
            *op = EVAL_OP_DIVIDE;
            return true;
        case ':':
            *op = EVAL_OP_COLON;
            return true;
        case '<':
            *op = (text[1] == '<') ? EVAL_OP_LSHIFT : ((text[1] == '=') ? EVAL_OP_LESS_EQUAL : EVAL_OP_LESS);
            return true;
        case '=':
            if ( text[1] != '=' )
                return false;
            *op = EVAL_OP_EQUALS;
            return true;
        case '>':
            *op = (text[1] == '>') ? EVAL_OP_RSHIFT : ((text[1] == '=') ? EVAL_OP_GREATER_EQUAL : EVAL_OP_GREATER);
            return true;
        case '?':
            *op = EVAL_OP_QUESTION;
            return true;
        case '^':
            *op = EVAL_OP_BITWISE_XOR;
            return true;
        case '|':
            *op = (text[1] == ('|')) ? EVAL_OP_LOGICAL_OR : EVAL_OP_BITWISE_OR;
            return true;
        case '~':
            *op = EVAL_OP_BITWISE_NOT;
            return true;
        default:
            return false;
    }
}

bool Eval_AnyMissingOperands(Eval *eval)
{
    int opIndex;
    signed int requiredOperandCount;

    requiredOperandCount = 1;

    for ( opIndex = 0; opIndex < eval->opStackPos; ++opIndex )
        requiredOperandCount += s_consumedOperandCount[eval->opStack[opIndex]];

    return requiredOperandCount != eval->valStackPos;
}

} // namespace xsk::menu::iw5
