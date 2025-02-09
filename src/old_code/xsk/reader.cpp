// Copyright 2022 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#include "stdafx.hpp"
#include "iw5.hpp"
#include "memory.hpp"
#include "eval.hpp"
#include "quake.hpp"
#include "reader.hpp"

namespace xsk::menu::iw5
{

int handle;

auto reader::init(const char* filename) -> bool
{
	const char *builtinDefines[2];
	builtinDefines[0] = "PC";
	builtinDefines[1] = 0;

	handle = PC_LoadSourceHandle(filename, builtinDefines);
	return (handle != 0) ? true : false;
}

void reader::end()
{
	PC_FreeSourceHandle(handle);
}

auto reader::read_token(token* out) -> bool
{
	return PC_ReadTokenHandle(handle, reinterpret_cast<pc_token_t*>(out));
}

auto reader::read_flag_optional(int* out) -> bool
{
  	pc_token_s token;

	if (!PC_ReadTokenHandle(handle, &token))
		return false;

	if (token.type == TT_NUMBER)
	{
		*out = token.intvalue;
		return true;
	}
	
	PC_UnreadLastTokenHandle(handle);
	return false;
}

auto reader::read_float(float* out) -> bool
{
	pc_token_s token;

	if (!PC_ReadTokenHandle(handle, &token))
		return false;

	if (token.string[0] == '(' )
		return reader::read_float_expr(out);
	
	bool negative = false;

	if (token.string[0] == '-')
	{
		if (!PC_ReadTokenHandle(handle, &token))
			return false;
		negative = true;
	}

	if (token.type == TT_NUMBER)
	{
		if (negative)
			*out = -token.floatvalue;
		else
			*out = token.floatvalue;
		return true;
	}

	reader::source_error("expected float but found %s\n");
	return false;
}

auto reader::read_float_expr(float* out) -> bool
{
	EvalValue result;
	EvalValue* result2;
	EvalOperatorType op;
	Eval eval;
	int parseLevel;
	pc_token_s token;

	parseLevel = 0;
	eval.opStackPos = 0;
  	eval.valStackPos = 0;
  	eval.parenCount = 0;
  	eval.pushedOp = 0;
	while (true)
	{
		while (true)
		{
			if (!PC_ReadTokenHandle(handle, &token))
				return false;

			if ( token.type != TT_NUMBER )
				break;

			if (!Eval_PushNumber(&eval, token.floatvalue))
			{
				reader::source_error("error evaluating expression\n");
				return false;
			}
		}

		if (!Eval_OperatorForToken(token.string, &op))
		{
			reader::source_error("expected operator but found %s\n");
			return false;
		}

		if (op == EVAL_OP_RPAREN)
			break;

		if (op == EVAL_OP_LPAREN && ++parseLevel > 16)
		{
			reader::source_error("too much recursive macro expansion\n");
			return false;
		}

LABEL_X:
		Eval_PushOperator(&eval, op);
	}

	if (parseLevel)
	{
		--parseLevel;
		goto LABEL_X;
	}

	if (Eval_AnyMissingOperands(&eval))
	{
		reader::source_error("error evaluating expression\n");
		return false;
	}

	result2 = Eval_Solve(&result, &eval);
	*out = result2->u.d;
	return true;
}

auto reader::read_int(int* out) -> bool
{
	pc_token_s token;

	if (!PC_ReadTokenHandle(handle, &token))
		return false;
	
	if (token.string[0] == '(')
		return read_int_expr(out);

	bool negative = false;

	if (token.string[0] == '-')
	{
		if (!PC_ReadTokenHandle(handle, &token))
			return false;
		negative = true;
	}

	if (token.type == TT_NUMBER)
	{
		*out = token.intvalue;
		if (negative) *out = -*out;
		return true;
	}

	reader::source_error("expected integer but found %s\n");
	return false;
}

auto reader::read_int_expr(int* out) -> bool
{
	EvalValue result;
	EvalValue* result2;
	EvalOperatorType op;
	Eval eval;
	int parseLevel;
	pc_token_s token;

	parseLevel = 0;
	eval.opStackPos = 0;
  	eval.valStackPos = 0;
  	eval.parenCount = 0;
  	eval.pushedOp = 0;
	while (true)
	{
		while (true)
		{
			if (!PC_ReadTokenHandle(handle, &token))
				return false;

			if ( token.type != TT_NUMBER )
				break;

			if (!Eval_PushInteger(&eval, token.floatvalue))
			{
				reader::source_error("error evaluating expression\n");
				return false;
			}
		}

		if (!Eval_OperatorForToken(token.string, &op))
		{
			reader::source_error("expected operator but found %s\n");
			return false;
		}

		if (op == EVAL_OP_RPAREN)
			break;

		if (op == EVAL_OP_LPAREN && ++parseLevel > 16)
		{
			reader::source_error("too much recursive macro expansion\n");
			return false;
		}

LABEL_X:
		Eval_PushOperator(&eval, op);
	}

	if (parseLevel)
	{
		--parseLevel;
		goto LABEL_X;
	}

	if (Eval_AnyMissingOperands(&eval))
	{
		reader::source_error("error evaluating expression\n");
		return false;
	}

	result2 = Eval_Solve(&result, &eval);
	*out = result2->u.i;
	return true;
}

auto reader::read_int_line(int* out) -> bool
{
  	pc_token_s token;

	if (!PC_ReadLineHandle(handle, &token))
		return false;

	bool negative = 0;

	if ( token.string[0] == '-' )
	{
		if (!PC_ReadLineHandle(handle, &token))
			return false;
		negative = true;
	}

	if (token.type == TT_NUMBER)
	{
		*out = token.intvalue;
		if (negative)
			*out = -*out;
		return true;;
	}

	reader::source_error("expected integer but found %s\n");
	return false;
}

auto reader::read_char(char* out) -> bool
{
	pc_token_s token;

	if (!PC_ReadTokenHandle(handle, &token))
		return false;

	*out = token.string[0];
	return true;
}

auto reader::read_byte(char* out) -> bool
{
	int i;

  	if (!reader::read_int(&i))
    	return false;

	if (i >= 0 && i <= 255)
	{
		*out = i;
		return true;
	}
	
	reader::source_error("expect int value between 0 and 255, but got '%d'\n");
	return false;
}

auto reader::read_string(const char** out) -> bool
{
	pc_token_s token;

	if (!PC_ReadTokenHandle(handle, &token))
		return false;

	*out = strdup(token.string); // CHECK MEM LEAK
	return true;
}

auto reader::read_color(vec4_t* out) -> bool
{
	float f;

	for (int i = 0; i < 4; ++i)
	{
		if (!reader::read_float(&f))
			return false;

		(*out)[i] = f;
	}

	return true;
}

auto reader::read_rect(rectDef_s* out) -> bool
{
	int refPoint;
	int flag;

	if (!reader::read_float(&out->x) || !reader::read_float(&out->y) || !reader::read_float(&out->w) || !reader::read_float(&out->h))
		return false;

	if (!reader::read_flag_optional(&flag))
		out->horzAlign = 0;
	else
		out->horzAlign = flag;

	if (!reader::read_flag_optional(&flag))
		out->vertAlign = 0;
	else
		out->vertAlign = flag;

	if (reader::read_flag_optional(&refPoint) && refPoint)
	{
		if (refPoint == 1)
		{
			out->x = out->x - (float)(out->w * 0.5);
		}
		else if (refPoint == 2)
		{
			out->x = out->x - out->w;
		}
		else
		{
			reader::source_error("invalid horizontal reference point\n");
		}
	}

	if (reader::read_flag_optional(&refPoint) && refPoint)
	{
		if (refPoint == 1)
		{
			out->y = out->y - (float)(out->h * 0.5);
		}
		else if (refPoint == 2)
		{
			out->y = out->y - out->h;
		}
		else
		{
			reader::source_error("invalid vertical reference point\n");
		}
	}

	return true;
}

static auto read_rect(rectDef_s* out, int sizeMode, int virtualResolution, ParseContext *parseContext) -> bool
{
	if (!reader::read_float(&out->x) || !reader::read_float(&out->y))
		return false;

	if (virtualResolution == 1)
	{
		out->x = out->x * 0.6666666865348816;
		out->y = out->y * 0.6666666865348816;
	}

	if (!sizeMode)
	{
		if (!reader::read_float(&out->w) || !reader::read_float(&out->h))
			return false;
	
		if (virtualResolution == 1)
		{
			out->w = out->w * 0.6666666865348816;
			out->h = out->h * 0.6666666865348816;
		}
	}

	out->horzAlign = 0;
	out->vertAlign = 0;

	if (!reader::read_rect_optionalbyteline((unsigned char *)&out->horzAlign)
		&& !reader::read_rect_optionalbyteline((unsigned char *)&out->vertAlign)
		&& !reader::read_rect_optionalbyteline((unsigned char *)&parseContext->horzRefPoint))
	{
		reader::read_rect_optionalbyteline((unsigned char *)&parseContext->vertRefPoint);
	}

	return true;
}

auto reader::read_rect_optionalbyteline(unsigned char* out) -> bool
{
	int value;

	if (reader::read_rect_checksemicolon() || !reader::read_int_line(&value))
		return true;

	if ((unsigned int)value > 10)
		PC_SourceError(handle, "Value %d outside of valid range (0,%d).\n", value, 10);
	else
		*out = value;

	return false;
}

auto reader::read_rect_checksemicolon() -> bool
{
	pc_token_s pc_token;

	if (PC_ReadTokenHandle(handle, &pc_token))
	{
		if (stricmp(pc_token.string, ";"))
		{
			PC_UnreadLastTokenHandle(handle);
			return false;
		}
	}
	else
	{
		PC_SourceError(handle, "Unexpected end of file while checking for semicolon alignment.\n");
		return false;
	}

	return true;
}

auto reader::read_script(const char** out) -> bool
{
	char script[0x1400];
	pc_token_s token;

	std::memset(script, 0, 0x1400);

	if (!PC_ReadTokenHandle(handle, &token))
		return false;

	if (stricmp(token.string, "{"))
		return false;

	do
	{
		if (!PC_ReadTokenHandle(handle, &token))
			return false;

		if (!stricmp(token.string, "}"))
		{
			*out = strdup(script); // CHECK MEM LEAK
			return true;
		}

		if (strlen(token.string) + &script[strlen(script) + 1] - &script[1] > 0x1400 )
		{
			break;
		}

		if ( token.type <= 0 )
			goto LABEL_24;

		if ( token.type <= 2 )
		{
			int len = strlen(script);
			snprintf(script + len, 0x1400 - len, "\"%s\" ", token.string);
			continue;
		}

		if ( token.type != 6 )
		{
LABEL_24:
			strncat(script, token.string, 0x1400);

			if ( token.type != TT_PUNCTUATION || token.subtype != P_SUB )
				strncat(script, " ", 0x1400);
		}
	}
	while ((unsigned int)(&script[strlen(script) + 1] - &script[1] + 1) <= 0x1400);

	reader::source_error("action block too long that starts with: %s\n");
	return false;
}

auto reader::read_event(MenuEventHandlerSet** out) -> bool
{
	pc_token_s token;
	auto handler_count = 0;
	MenuEventHandler* handlers[200];
	char dst[0x1400] = { 0 };

	if (!PC_ReadTokenHandle(handle, &token))
    	return false;

	if (stricmp(token.string, "{"))
	{
		reader::source_error("Expected '{'.\n");
		return false;
	}

	while (true)
	{
		if (!PC_ReadTokenHandle(handle, &token))
		{
			reader::source_error("Unexpected end of file.");
			return false;
		}

		if (handler_count == 200)
		{
			// free all handlers!!!!!!!
			reader::source_error("Exceeded MAX_EVENT_HANDLERS_PER_EVENT(200)");
			return false;
		}

		if (token.string[0] == '}')
			break;
		
		EventType eventType = EVENT_UNCONDITIONAL;

		if (!stricmp(token.string, "if"))
			eventType = EVENT_IF;
		else if (!stricmp(token.string, "else"))
			eventType = EVENT_ELSE;
		else if (!stricmp(token.string, "setLocalVarBool"))
			eventType = EVENT_SET_LOCAL_VAR_BOOL;
		else if (!stricmp(token.string, "setLocalVarInt"))
			eventType = EVENT_SET_LOCAL_VAR_INT;
		else if (!stricmp(token.string, "setLocalVarFloat"))
			eventType = EVENT_SET_LOCAL_VAR_FLOAT;
		else if (!stricmp(token.string, "setLocalVarString"))
			eventType = EVENT_SET_LOCAL_VAR_STRING;
		
		if (eventType == EVENT_IF)
		{
			auto eventHandler = (MenuEventHandler*)std::malloc(sizeof(MenuEventHandler));
			eventHandler->eventType = eventType;
			eventHandler->eventData.conditionalScript = (ConditionalScript*)std::malloc(sizeof(ConditionalScript));

			if (!reader::read_expression(&eventHandler->eventData.conditionalScript->eventExpression))
			{
				reader::source_error("Could not read expression.");
				return false;
			}

			if (!reader::read_event(&eventHandler->eventData.conditionalScript->eventHandlerSet))
			{
				reader::source_error("Could not read event handler set.");
				return false;
			}

			handlers[handler_count++] = eventHandler;
		}
		else if (eventType == EVENT_ELSE)
		{
			if (!handler_count || handlers[handler_count - 1]->eventType != EVENT_IF)
			{
				reader::source_error("else statement must follow an if block.");
				return false;
			}

			auto eventHandler = (MenuEventHandler*)std::malloc(sizeof(MenuEventHandler));
			eventHandler->eventType = eventType;

			if (!reader::read_event(&eventHandler->eventData.elseScript))
			{
				reader::source_error("Could not read event handler set.");
				return false;
			}

			handlers[handler_count++] = eventHandler;
		}
		else if (eventType == EVENT_SET_LOCAL_VAR_BOOL || eventType == EVENT_SET_LOCAL_VAR_INT || eventType == EVENT_SET_LOCAL_VAR_FLOAT || eventType == EVENT_SET_LOCAL_VAR_STRING)
		{
			if (!PC_ReadTokenHandle(handle, &token))
			{
				reader::source_error("Expected local var name.");
				return false;
			}

			auto eventHandler = (MenuEventHandler*)std::malloc(sizeof(MenuEventHandler));
			eventHandler->eventType = eventType;
			eventHandler->eventData.setLocalVarData = (SetLocalVarData*)std::malloc(sizeof(SetLocalVarData));
			eventHandler->eventData.setLocalVarData->localVarName = strdup(token.string);
		
			if (!reader::read_expression(&eventHandler->eventData.setLocalVarData->expression))
			{
				reader::source_error("Could not read expression.");
				return false;
			}

			handlers[handler_count++] = eventHandler;
		}
		else
		{
			std::memset(dst, 0, sizeof(dst));
			PC_UnreadLastTokenHandle(handle);

			do
			{
				if (!PC_ReadTokenHandle(handle, &token))
					return false;

				if (!stricmp(token.string, "}"))
				{
					PC_UnreadLastTokenHandle(handle);
					break;
				}
				else if (!stricmp(token.string, "{"))
				{
					reader::source_error("Unexpected '{' in an event handler.");
					return 0;
				}

				if (strlen(token.string) + strlen(dst) + 1 >= sizeof(dst))
				{
					reader::source_error("action block too long that starts with: %s", dst);
					return false;
				}

				snprintf(dst + strlen(dst), sizeof(dst), "\"%s\" ", token.string);
			} while (strlen(dst) + 1 < sizeof(dst));

			auto eventHandler = (MenuEventHandler*)std::malloc(sizeof(MenuEventHandler));
			eventHandler->eventType = eventType;
			eventHandler->eventData.unconditionalScript = strdup(dst);

			handlers[handler_count++] = eventHandler;
		}
	}

	*out = (MenuEventHandlerSet*)std::malloc(sizeof(MenuEventHandlerSet));
	auto new_handlers = (MenuEventHandler**)std::malloc(handler_count * sizeof(MenuEventHandler*));
	std::memcpy(new_handlers, handlers, sizeof(MenuEventHandler*) * handler_count);
	(*out)->eventHandlers = new_handlers;
	(*out)->eventHandlerCount = handler_count;

	return true;
}

auto read_expr_internal(Statement_s* out, int entries) -> bool
{

}

auto reader::read_expression(Statement_s** out) -> bool
{
	int maxTokens = 200;

	*out = memory::alloc<Statement_s>();
	(*out)->entries = memory::alloc<expressionEntry>(maxTokens);

	if (!read_expr_internal(*out, maxTokens))
		return false;

	return true;
}

void reader::source_error(const char* fmt, ...)
{
	//PC_SourceError(handle, fmt);
}

} // namespace xsk::menu::iw5
