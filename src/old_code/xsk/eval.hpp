// Copyright 2022 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#pragma once

namespace xsk::menu::iw5
{

enum EvalOperatorType
{
    EVAL_OP_LPAREN = 0x0,
    EVAL_OP_RPAREN = 0x1,
    EVAL_OP_COLON = 0x2,
    EVAL_OP_QUESTION = 0x3,
    EVAL_OP_PLUS = 0x4,
    EVAL_OP_MINUS = 0x5,
    EVAL_OP_UNARY_PLUS = 0x6,
    EVAL_OP_UNARY_MINUS = 0x7,
    EVAL_OP_MULTIPLY = 0x8,
    EVAL_OP_DIVIDE = 0x9,
    EVAL_OP_MODULUS = 0xA,
    EVAL_OP_LSHIFT = 0xB,
    EVAL_OP_RSHIFT = 0xC,
    EVAL_OP_BITWISE_NOT = 0xD,
    EVAL_OP_BITWISE_AND = 0xE,
    EVAL_OP_BITWISE_OR = 0xF,
    EVAL_OP_BITWISE_XOR = 0x10,
    EVAL_OP_LOGICAL_NOT = 0x11,
    EVAL_OP_LOGICAL_AND = 0x12,
    EVAL_OP_LOGICAL_OR = 0x13,
    EVAL_OP_EQUALS = 0x14,
    EVAL_OP_NOT_EQUAL = 0x15,
    EVAL_OP_LESS = 0x16,
    EVAL_OP_LESS_EQUAL = 0x17,
    EVAL_OP_GREATER = 0x18,
    EVAL_OP_GREATER_EQUAL = 0x19,
    EVAL_OP_COUNT = 0x1A,
};

enum EvalValueType
{
    EVAL_VALUE_DOUBLE = 0x0,
    EVAL_VALUE_INT = 0x1,
    EVAL_VALUE_STRING = 0x2,
};

struct EvalValue
{
    EvalValueType type;
    union
    {
        long double d;
        int i;
        char *s;
    } u;
};

struct Eval
{
    EvalOperatorType opStack[1024];
    EvalValue valStack[1024];
    int opStackPos;
    int valStackPos;
    int parenCount;
    bool pushedOp;
};

auto Eval_Solve(EvalValue *result, Eval *eval) -> EvalValue*;
bool Eval_PushOperator(Eval *eval, EvalOperatorType op);
bool Eval_PushNumber(Eval *eval, long double value);
bool Eval_PushInteger(Eval *eval, int value);
bool Eval_OperatorForToken(const char *text, EvalOperatorType *op);
bool Eval_AnyMissingOperands(Eval *eval);

} // namespace xsk::menu::iw5
