// Copyright 2022 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#include "stdafx.hpp"
#include "memory.hpp"
#include "quake.hpp"

namespace xsk::menu::iw5
{

#define PUNCTABLE

//longer punctuations first
punctuation_t default_punctuations[53] =
{
	//binary operators
	{">>=",P_RSHIFT_ASSIGN, NULL},
	{"<<=",P_LSHIFT_ASSIGN, NULL},
	//
	{"...",P_PARMS, NULL},
	//define merge operator
	{"##",P_PRECOMPMERGE, NULL},
	//logic operators
	{"&&",P_LOGIC_AND, NULL},
	{"||",P_LOGIC_OR, NULL},
	{">=",P_LOGIC_GEQ, NULL},
	{"<=",P_LOGIC_LEQ, NULL},
	{"==",P_LOGIC_EQ, NULL},
	{"!=",P_LOGIC_UNEQ, NULL},
	//arithmatic operators
	{"*=",P_MUL_ASSIGN, NULL},
	{"/=",P_DIV_ASSIGN, NULL},
	{"%=",P_MOD_ASSIGN, NULL},
	{"+=",P_ADD_ASSIGN, NULL},
	{"-=",P_SUB_ASSIGN, NULL},
	{"++",P_INC, NULL},
	{"--",P_DEC, NULL},
	//binary operators
	{"&=",P_BIN_AND_ASSIGN, NULL},
	{"|=",P_BIN_OR_ASSIGN, NULL},
	{"^=",P_BIN_XOR_ASSIGN, NULL},
	{">>",P_RSHIFT, NULL},
	{"<<",P_LSHIFT, NULL},
	//reference operators
	{"->",P_POINTERREF, NULL},
	//C++
	{"::",P_CPP1, NULL},
	{".*",P_CPP2, NULL},
	//arithmatic operators
	{"*",P_MUL, NULL},
	{"/",P_DIV, NULL},
	{"%",P_MOD, NULL},
	{"+",P_ADD, NULL},
	{"-",P_SUB, NULL},
	{"=",P_ASSIGN, NULL},
	//binary operators
	{"&",P_BIN_AND, NULL},
	{"|",P_BIN_OR, NULL},
	{"^",P_BIN_XOR, NULL},
	{"~",P_BIN_NOT, NULL},
	//logic operators
	{"!",P_LOGIC_NOT, NULL},
	{">",P_LOGIC_GREATER, NULL},
	{"<",P_LOGIC_LESS, NULL},
	//reference operator
	{".",P_REF, NULL},
	//seperators
	{",",P_COMMA, NULL},
	{";",P_SEMICOLON, NULL},
	//label indication
	{":",P_COLON, NULL},
	//if statement
	{"?",P_QUESTIONMARK, NULL},
	//embracements
	{"(",P_PARENTHESESOPEN, NULL},
	{")",P_PARENTHESESCLOSE, NULL},
	{"{",P_BRACEOPEN, NULL},
	{"}",P_BRACECLOSE, NULL},
	{"[",P_SQBRACKETOPEN, NULL},
	{"]",P_SQBRACKETCLOSE, NULL},
	//
	{"\\",P_BACKSLASH, NULL},
	//precompiler operator
	{"#",P_PRECOMP, NULL},
	{"$",P_DOLLAR, NULL},
	{NULL, 0}
};

void PS_CreatePunctuationTable(script_t *script, punctuation_t *punctuations)
{
	int i;
	punctuation_t *p, *lastp, *newp;

	//get memory for the table
	if (!script->punctuationtable) script->punctuationtable = (punctuation_t **)std::malloc(256 * sizeof(punctuation_t *)); // CHECK MEM LEAK
	std::memset(script->punctuationtable, 0, 256 * sizeof(punctuation_t *));
	//add the punctuations in the list to the punctuation table
	for (i = 0; punctuations[i].p; i++)
	{
		newp = &punctuations[i];
		lastp = NULL;
		//sort the punctuations in this table entry on length (longer punctuations first)
		for (p = script->punctuationtable[(unsigned int) newp->p[0]]; p; p = p->next)
		{
			if (strlen(p->p) < strlen(newp->p))
			{
				newp->next = p;
				if (lastp) lastp->next = newp;
				else script->punctuationtable[(unsigned int) newp->p[0]] = newp;
				break;
			}
			lastp = p;
		}
		if (!p)
		{
			newp->next = NULL;
			if (lastp) lastp->next = newp;
			else script->punctuationtable[(unsigned int) newp->p[0]] = newp;
		}
	}
}

char *PunctuationFromNum(script_t *script, int num)
{
	int i;

	for (i = 0; script->punctuations[i].p; i++)
	{
		if (script->punctuations[i].n == num) return script->punctuations[i].p;
	}

	return "unkown punctuation";
}

void ScriptError(script_t *script, char *str, ...)
{
	char text[1024];
	va_list ap;

	if (script->flags & SCFL_NOERRORS) return;

	va_start(ap, str);
	vsprintf(text, str, ap);
	va_end(ap);

	printf("error: file %s, line %d: %s\n", script->filename, script->line, text);
}

void ScriptWarning(script_t *script, char *str, ...)
{
	char text[1024];
	va_list ap;

	if (script->flags & SCFL_NOWARNINGS) return;

	va_start(ap, str);
	vsprintf(text, str, ap);
	va_end(ap);

	printf("warning: file %s, line %d: %s\n", script->filename, script->line, text);

}

void SetScriptPunctuations(script_t *script, punctuation_t *p)
{
#ifdef PUNCTABLE
	if (p) PS_CreatePunctuationTable(script, p);
	else  PS_CreatePunctuationTable(script, default_punctuations);
#endif //PUNCTABLE
	if (p) script->punctuations = p;
	else script->punctuations = default_punctuations;
}

bool PS_ReadWhiteSpace(script_t *script)
{
	while(1)
	{
		//skip white space
		while(*script->script_p <= ' ')
		{
			if (!*script->script_p) return false;
			if (*script->script_p == '\n') script->line++;
			script->script_p++;
		}
		//skip comments
		if (*script->script_p == '/')
		{
			//comments //
			if (*(script->script_p+1) == '/')
			{
				script->script_p++;
				do
				{
					script->script_p++;
					if (!*script->script_p) return false;
				}
				while(*script->script_p != '\n');
				script->line++;
				script->script_p++;
				if (!*script->script_p) return false;
				continue;
			}
			//comments /* */
			else if (*(script->script_p+1) == '*')
			{
				script->script_p++;
				do
				{
					script->script_p++;
					if (!*script->script_p) return false;
					if (*script->script_p == '\n') script->line++;
				}
				while(!(*script->script_p == '*' && *(script->script_p+1) == '/'));
				script->script_p++;
				if (!*script->script_p) return false;
				script->script_p++;
				if (!*script->script_p) return false;
				continue;
			}
		}
		break;
	}

	return true;
}

bool PS_ReadEscapeCharacter(script_t *script, char *ch)
{
	int c, val, i;

	//step over the leading '\\'
	script->script_p++;
	//determine the escape character
	switch(*script->script_p)
	{
		case '\\': c = '\\'; break;
		case 'n': c = '\n'; break;
		case 'r': c = '\r'; break;
		case 't': c = '\t'; break;
		case 'v': c = '\v'; break;
		case 'b': c = '\b'; break;
		case 'f': c = '\f'; break;
		case 'a': c = '\a'; break;
		case '\'': c = '\''; break;
		case '\"': c = '\"'; break;
		case '\?': c = '\?'; break;
		case 'x':
		{
			script->script_p++;
			for (i = 0, val = 0; ; i++, script->script_p++)
			{
				c = *script->script_p;
				if (c >= '0' && c <= '9') c = c - '0';
				else if (c >= 'A' && c <= 'Z') c = c - 'A' + 10;
				else if (c >= 'a' && c <= 'z') c = c - 'a' + 10;
				else break;
				val = (val << 4) + c;
			}
			script->script_p--;
			if (val > 0xFF)
			{
				ScriptWarning(script, "too large value in escape character");
				val = 0xFF;
			}
			c = val;
			break;
		}
		default: //NOTE: decimal ASCII code, NOT octal
		{
			if (*script->script_p < '0' || *script->script_p > '9') ScriptError(script, "unknown escape char");
			for (i = 0, val = 0; ; i++, script->script_p++)
			{
				c = *script->script_p;
				if (c >= '0' && c <= '9') c = c - '0';
				else break;
				val = val * 10 + c;
			}
			script->script_p--;
			if (val > 0xFF)
			{
				ScriptWarning(script, "too large value in escape character");
				val = 0xFF;
			}
			c = val;
			break;
		}
	} 
	//step over the escape character or the last digit of the number
	script->script_p++;
	//store the escape character
	*ch = c;

	return true;
}

bool PS_ReadString(script_t *script, token_t *token, int quote)
{
	int len, tmpline;
	char *tmpscript_p;

	if (quote == '\"') token->type = TT_STRING;
	else token->type = TT_LITERAL;

	len = 0;
	//leading quote
	token->string[len++] = *script->script_p++;

	while(1)
	{
		//minus 2 because trailing double quote and zero have to be appended
		if (len >= MAX_TOKEN - 2)
		{
			ScriptError(script, "string longer than MAX_TOKEN = %d", MAX_TOKEN);
			return false;
		}
		//if there is an escape character and
		//if escape characters inside a string are allowed
		if (*script->script_p == '\\' && !(script->flags & SCFL_NOSTRINGESCAPECHARS))
		{
			if (!PS_ReadEscapeCharacter(script, &token->string[len]))
			{
				token->string[len] = 0;
				return false;
			} 
			len++;
		}
		//if a trailing quote
		else if (*script->script_p == quote)
		{
			//step over the double quote
			script->script_p++;
			//if white spaces in a string are not allowed
			if (script->flags & SCFL_NOSTRINGWHITESPACES) break;

			tmpscript_p = script->script_p;
			tmpline = script->line;
			//read unusefull stuff between possible two following strings
			if (!PS_ReadWhiteSpace(script))
			{
				script->script_p = tmpscript_p;
				script->line = tmpline;
				break;
			}
			//if there's no leading double qoute
			if (*script->script_p != quote)
			{
				script->script_p = tmpscript_p;
				script->line = tmpline;
				break;
			}
			//step over the new leading double quote
			script->script_p++;
		}
		else
		{
			if (*script->script_p == '\0')
			{
				token->string[len] = 0;
				ScriptError(script, "missing trailing quote");
				return false;
			}
	        if (*script->script_p == '\n')
			{
				token->string[len] = 0;
				ScriptError(script, "newline inside string %s", token->string);
				return false;
			}
			token->string[len++] = *script->script_p++;
		}
	}

	//trailing quote
	token->string[len++] = quote;
	//end string with a zero
	token->string[len] = '\0';
	//the sub type is the length of the string
	token->subtype = len;

	return true;
}

bool PS_ReadName(script_t *script, token_t *token)
{
	int len = 0;
	char c;

	token->type = TT_NAME;
	do
	{
		token->string[len++] = *script->script_p++;
		if (len >= MAX_TOKEN)
		{
			ScriptError(script, "name longer than MAX_TOKEN = %d", MAX_TOKEN);
			return false;
		}
		c = *script->script_p;
    } while ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_');
    
    token->string[len] = '\0';
	//the sub type is the length of the name
	token->subtype = len;

	return true;
}

void NumberValue(char *string, int subtype, unsigned long int *intvalue, long double *floatvalue)
{
	unsigned long int dotfound = 0;

	*intvalue = 0;
	*floatvalue = 0;
	//floating point number
	if (subtype & TT_FLOAT)
	{
		while(*string)
		{
			if (*string == '.')
			{
				if (dotfound) return;
				dotfound = 10;
				string++;
			} //end if
			if (dotfound)
			{
				*floatvalue = *floatvalue + (long double) (*string - '0') /
																	(long double) dotfound;
				dotfound *= 10;
			} //end if
			else
			{
				*floatvalue = *floatvalue * 10.0 + (long double) (*string - '0');
			} //end else
			string++;
		} //end while
		*intvalue = (unsigned long) *floatvalue;
	} //end if
	else if (subtype & TT_DECIMAL)
	{
		while(*string) *intvalue = *intvalue * 10 + (*string++ - '0');
		*floatvalue = *intvalue;
	} //end else if
	else if (subtype & TT_HEX)
	{
		//step over the leading 0x or 0X
		string += 2;
		while(*string)
		{
			*intvalue <<= 4;
			if (*string >= 'a' && *string <= 'f') *intvalue += *string - 'a' + 10;
			else if (*string >= 'A' && *string <= 'F') *intvalue += *string - 'A' + 10;
			else *intvalue += *string - '0';
			string++;
		} //end while
		*floatvalue = *intvalue;
	} //end else if
	else if (subtype & TT_OCTAL)
	{
		//step over the first zero
		string += 1;
		while(*string) *intvalue = (*intvalue << 3) + (*string++ - '0');
		*floatvalue = *intvalue;
	} //end else if
	else if (subtype & TT_BINARY)
	{
		//step over the leading 0b or 0B
		string += 2;
		while(*string) *intvalue = (*intvalue << 1) + (*string++ - '0');
		*floatvalue = *intvalue;
	} //end else if
}

bool PS_ReadNumber(script_t *script, token_t *token)
{
	int len = 0, i;
	bool octal, dot;
	char c;
//	unsigned long int intvalue = 0;
//	long double floatvalue = 0;

	token->type = TT_NUMBER;
	//check for a hexadecimal number
	if (*script->script_p == '0' && (*(script->script_p + 1) == 'x' || *(script->script_p + 1) == 'X'))
	{
		token->string[len++] = *script->script_p++;
		token->string[len++] = *script->script_p++;
		c = *script->script_p;
		//hexadecimal
		while((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'A'))
		{
			token->string[len++] = *script->script_p++;
			if (len >= MAX_TOKEN)
			{
				ScriptError(script, "hexadecimal number longer than MAX_TOKEN = %d", MAX_TOKEN);
				return false;
			}
			c = *script->script_p;
		}
		token->subtype |= TT_HEX;
	}
	//check for a binary number
	else if (*script->script_p == '0' && (*(script->script_p + 1) == 'b' || *(script->script_p + 1) == 'B'))
	{
		token->string[len++] = *script->script_p++;
		token->string[len++] = *script->script_p++;
		c = *script->script_p;
		//binary
		while(c == '0' || c == '1')
		{
			token->string[len++] = *script->script_p++;
			if (len >= MAX_TOKEN)
			{
				ScriptError(script, "binary number longer than MAX_TOKEN = %d", MAX_TOKEN);
				return false;
			}
			c = *script->script_p;
		}
		token->subtype |= TT_BINARY;
	}
	else //decimal or octal integer or floating point number
	{
		octal = false;
		dot = false;
		if (*script->script_p == '0') octal = true;
		while(1)
		{
			c = *script->script_p;
			if (c == '.') dot = true;
			else if (c == '8' || c == '9') octal = false;
			else if (c < '0' || c > '9') break;
			token->string[len++] = *script->script_p++;
			
            if (len >= MAX_TOKEN - 1)
			{
				ScriptError(script, "number longer than MAX_TOKEN = %d", MAX_TOKEN);
				return false;
			}
		}

		if (octal) token->subtype |= TT_OCTAL;
		else token->subtype |= TT_DECIMAL;
		if (dot) token->subtype |= TT_FLOAT;
	}

	for (i = 0; i < 2; i++)
	{
		c = *script->script_p;
		//check for a LONG number
		if ( (c == 'l' || c == 'L') && !(token->subtype & TT_LONG))
		{
			script->script_p++;
			token->subtype |= TT_LONG;
		} 
		//check for an UNSIGNED number
		else if ( (c == 'u' || c == 'U') && !(token->subtype & (TT_UNSIGNED | TT_FLOAT)))
		{
			script->script_p++;
			token->subtype |= TT_UNSIGNED;
		}
	}

	token->string[len] = '\0';

	NumberValue(token->string, token->subtype, &token->intvalue, &token->floatvalue);

	if (!(token->subtype & TT_FLOAT)) token->subtype |= TT_INTEGER;

    return true;
}

bool PS_ReadLiteral(script_t *script, token_t *token)
{
	token->type = TT_LITERAL;
	//first quote
	token->string[0] = *script->script_p++;
	//check for end of file
	if (!*script->script_p)
	{
		ScriptError(script, "end of file before trailing \'");
		return false;
	}

	//if it is an escape character
	if (*script->script_p == '\\')
	{
		if (!PS_ReadEscapeCharacter(script, &token->string[1])) return false;
	}
	else
	{
		token->string[1] = *script->script_p++;
	}

	//check for trailing quote
	if (*script->script_p != '\'')
	{
		ScriptWarning(script, "too many characters in literal, ignored");
		while(*script->script_p && *script->script_p != '\'' && *script->script_p != '\n')
		{
			script->script_p++;
		}

		if (*script->script_p == '\'') script->script_p++;
	}

	//store the trailing quote
	token->string[2] = *script->script_p++;
	//store trailing zero to end the string
	token->string[3] = '\0';
	//the sub type is the integer literal value
	token->subtype = token->string[1];

	return true;
}

bool PS_ReadPunctuation(script_t *script, token_t *token)
{
	int len;
	char *p;
	punctuation_t *punc;

#ifdef PUNCTABLE
	for (punc = script->punctuationtable[(unsigned int)*script->script_p]; punc; punc = punc->next)
	{
#else
	int i;

	for (i = 0; script->punctuations[i].p; i++)
	{
		punc = &script->punctuations[i];
#endif //PUNCTABLE
		p = punc->p;
		len = strlen(p);
		//if the script contains at least as much characters as the punctuation
		if (script->script_p + len <= script->end_p)
		{
			//if the script contains the punctuation
			if (!strncmp(script->script_p, p, len))
			{
				strncpy(token->string, p, MAX_TOKEN);
				script->script_p += len;
				token->type = TT_PUNCTUATION;
				//sub type is the number of the punctuation
				token->subtype = punc->n;
				return true;
			}
		}
	}

	return false;
}

bool PS_ReadPrimitive(script_t *script, token_t *token)
{
	int len = 0;

	while(*script->script_p > ' ' && *script->script_p != ';')
	{
		if (len >= MAX_TOKEN)
		{
			ScriptError(script, "primitive token longer than MAX_TOKEN = %d", MAX_TOKEN);
			return false;
		}
		token->string[len++] = *script->script_p++;
	}

	token->string[len] = 0;
	//copy the token into the script structure
	std::memcpy(&script->token, token, sizeof(token_t));
	//primitive reading successfull
	return true;
}

bool PS_ReadToken(script_t *script, token_t *token)
{
	//if there is a token available (from UnreadToken)
	if (script->tokenavailable)
	{
		script->tokenavailable = 0;
		std::memcpy(token, &script->token, sizeof(token_t));
		return true;
	}

	//save script pointer
	script->lastscript_p = script->script_p;
	//save line counter
	script->lastline = script->line;
	//clear the token stuff
	std::memset(token, 0, sizeof(token_t));
	//start of the white space
	script->whitespace_p = script->script_p;
	token->whitespace_p = script->script_p;
	//read unusefull stuff
	if (!PS_ReadWhiteSpace(script)) return false;
	//end of the white space
	script->endwhitespace_p = script->script_p;
	token->endwhitespace_p = script->script_p;
	//line the token is on
	token->line = script->line;
	//number of lines crossed before token
	token->linescrossed = script->line - script->lastline;
	//if there is a leading double quote
	if (*script->script_p == '\"')
	{
		if (!PS_ReadString(script, token, '\"')) return false;
	}
	//if an literal
	else if (*script->script_p == '\'')
	{
		//if (!PS_ReadLiteral(script, token)) return 0;
		if (!PS_ReadString(script, token, '\'')) return false;
	}
	//if there is a number
	else if ((*script->script_p >= '0' && *script->script_p <= '9') || (*script->script_p == '.' && (*(script->script_p + 1) >= '0' && *(script->script_p + 1) <= '9')))
	{
		if (!PS_ReadNumber(script, token)) return false;
	}
	//if this is a primitive script
	else if (script->flags & SCFL_PRIMITIVE)
	{
		return PS_ReadPrimitive(script, token);
	}
	//if there is a name
	else if ((*script->script_p >= 'a' && *script->script_p <= 'z') || (*script->script_p >= 'A' && *script->script_p <= 'Z') || *script->script_p == '_')
	{
		if (!PS_ReadName(script, token)) return false;
	}
	//check for punctuations
	else if (!PS_ReadPunctuation(script, token))
	{
		ScriptError(script, "can't read token");
		return false;
	}

	//copy the token into the script structure
	std::memcpy(&script->token, token, sizeof(token_t));
	//succesfully read a token
	return true;
}

bool PS_ExpectTokenString(script_t *script, char *string)
{
	token_t token;

	if (!PS_ReadToken(script, &token))
	{
		ScriptError(script, "couldn't find expected %s", string);
		return false;
	}

	if (strcmp(token.string, string))
	{
		ScriptError(script, "expected %s, found %s", string, token.string);
		return false;
	}

	return true;
}

bool PS_ExpectTokenType(script_t *script, int type, int subtype, token_t *token)
{
	char str[MAX_TOKEN];

	if (!PS_ReadToken(script, token))
	{
		ScriptError(script, "couldn't read expected token");
		return false;
	}

	if (token->type != type)
	{
		if (type == TT_STRING) strcpy(str, "string");
		if (type == TT_LITERAL) strcpy(str, "literal");
		if (type == TT_NUMBER) strcpy(str, "number");
		if (type == TT_NAME) strcpy(str, "name");
		if (type == TT_PUNCTUATION) strcpy(str, "punctuation");
		ScriptError(script, "expected a %s, found %s", str, token->string);
		return false;
	}

	if (token->type == TT_NUMBER)
	{
		if ((token->subtype & subtype) != subtype)
		{
			if (subtype & TT_DECIMAL) strcpy(str, "decimal");
			if (subtype & TT_HEX) strcpy(str, "hex");
			if (subtype & TT_OCTAL) strcpy(str, "octal");
			if (subtype & TT_BINARY) strcpy(str, "binary");
			if (subtype & TT_LONG) strcat(str, " long");
			if (subtype & TT_UNSIGNED) strcat(str, " unsigned");
			if (subtype & TT_FLOAT) strcat(str, " float");
			if (subtype & TT_INTEGER) strcat(str, " integer");
			ScriptError(script, "expected %s, found %s", str, token->string);
			return false;
		}
	}
	else if (token->type == TT_PUNCTUATION)
	{
		if (subtype < 0)
		{
			ScriptError(script, "BUG: wrong punctuation subtype");
			return false;
		}

		if (token->subtype != subtype)
		{
			ScriptError(script, "expected %s, found %s", script->punctuations[subtype], token->string);
			return false;
		}
	}

	return true;
}

bool PS_ExpectAnyToken(script_t *script, token_t *token)
{
	if (!PS_ReadToken(script, token))
	{
		ScriptError(script, "couldn't read expected token");
		return false;
	}

	return true;
}

bool PS_CheckTokenString(script_t *script, char *string)
{
	token_t tok;

	if (!PS_ReadToken(script, &tok)) return false;
	//if the token is available
	if (!strcmp(tok.string, string)) return true;
	//token not available
	script->script_p = script->lastscript_p;
	return false;
}

bool PS_CheckTokenType(script_t *script, int type, int subtype, token_t *token)
{
	token_t tok;

	if (!PS_ReadToken(script, &tok)) return false;

	//if the type matches
	if (tok.type == type &&
			(tok.subtype & subtype) == subtype)
	{
		std::memcpy(token, &tok, sizeof(token_t));
		return true;
	}

	//token is not available
	script->script_p = script->lastscript_p;
	return false;
}

bool PS_SkipUntilString(script_t *script, char *string)
{
	token_t token;

	while(PS_ReadToken(script, &token))
	{
		if (!strcmp(token.string, string)) return true;
	}

	return false;
}

void PS_UnreadLastToken(script_t *script)
{
	script->tokenavailable = 1;
}

void PS_UnreadToken(script_t *script, token_t *token)
{
	std::memcpy(&script->token, token, sizeof(token_t));
	script->tokenavailable = 1;
}

char PS_NextWhiteSpaceChar(script_t *script)
{
	if (script->whitespace_p != script->endwhitespace_p)
	{
		return *script->whitespace_p++;
	}
	else
	{
		return 0;
	}
}

void StripDoubleQuotes(char *string)
{
	if (*string == '\"')
	{
		strcpy(string, string+1);
	}
	if (string[strlen(string)-1] == '\"')
	{
		string[strlen(string)-1] = '\0';
	}
}

void StripSingleQuotes(char *string)
{
	if (*string == '\'')
	{
		strcpy(string, string+1);
	}
	if (string[strlen(string)-1] == '\'')
	{
		string[strlen(string)-1] = '\0';
	}
}

long double ReadSignedFloat(script_t *script)
{
	token_t token;
	long double sign = 1;

	PS_ExpectAnyToken(script, &token);

	if (!strcmp(token.string, "-"))
	{
		sign = -1;
		PS_ExpectTokenType(script, TT_NUMBER, 0, &token);
	}
	else if (token.type != TT_NUMBER)
	{
		ScriptError(script, "expected float value, found %s\n", token.string);
	}

	return sign * token.floatvalue;
}

signed long int ReadSignedInt(script_t *script)
{
	token_t token;
	signed long int sign = 1;

	PS_ExpectAnyToken(script, &token);

	if (!strcmp(token.string, "-"))
	{
		sign = -1;
		PS_ExpectTokenType(script, TT_NUMBER, TT_INTEGER, &token);
	}
	else if (token.type != TT_NUMBER || token.subtype == TT_FLOAT)
	{
		ScriptError(script, "expected integer value, found %s\n", token.string);
	}

	return sign * token.intvalue;
}

void SetScriptFlags(script_t *script, int flags)
{
	script->flags = flags;
}

int GetScriptFlags(script_t *script)
{
	return script->flags;
}

void ResetScript(script_t *script)
{
	//pointer in script buffer
	script->script_p = script->buffer;
	//pointer in script buffer before reading token
	script->lastscript_p = script->buffer;
	//begin of white space
	script->whitespace_p = NULL;
	//end of white space
	script->endwhitespace_p = NULL;
	//set if there's a token available in script->token
	script->tokenavailable = 0;
	//
	script->line = 1;
	script->lastline = 1;
	//clear the saved token
	std::memset(&script->token, 0, sizeof(token_t));
}

bool EndOfScript(script_t *script)
{
	return script->script_p >= script->end_p;
}

int NumLinesCrossed(script_t *script)
{
	return script->line - script->lastline;
}

int ScriptSkipTo(script_t *script, char *value)
{
	int len;
	char firstchar;

	firstchar = *value;
	len = strlen(value);
	do
	{
		if (!PS_ReadWhiteSpace(script)) return 0;
		if (*script->script_p == firstchar)
		{
			if (!strncmp(script->script_p, value, len))
			{
				return 1;
			}
		}
		script->script_p++;
	} while(1);
}

int FileLength(FILE *fp)
{
	int pos;
	int end;

	pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	end = ftell(fp);
	fseek(fp, pos, SEEK_SET);

	return end;
}

script_t *LoadScriptFile(const char *filename)
{
	FILE *fp;
	int length;
	void *buffer;
	script_t *script;

	fp = fopen(filename, "rb");
	if (!fp) return NULL;

	length = FileLength(fp);

	buffer = std::malloc(sizeof(script_t) + length + 1); // CHECK MEM LEAK
    std::memset(buffer, 0, sizeof(script_t) + length + 1);
	script = (script_t *) buffer;
	std::memset(script, 0, sizeof(script_t));
	strcpy(script->filename, filename);
	script->buffer = (char *) buffer + sizeof(script_t);
	script->buffer[length] = 0;
	script->length = length;
	//pointer in script buffer
	script->script_p = script->buffer;
	//pointer in script buffer before reading token
	script->lastscript_p = script->buffer;
	//pointer to end of script buffer
	script->end_p = &script->buffer[length];
	//set if there's a token available in script->token
	script->tokenavailable = 0;
	//
	script->line = 1;
	script->lastline = 1;
	//
	SetScriptPunctuations(script, NULL);
	//
	if (fread(script->buffer, length, 1, fp) != 1)
	{
		std::free(buffer);
		script = NULL;
	} //end if
	fclose(fp);

	//
	//script->length = COM_Compress(script->buffer);

	return script;
}

script_t *LoadScriptMemory(const char *ptr, int length, char *name)
{
	void *buffer;
	script_t *script;

	buffer = std::malloc(sizeof(script_t) + length + 1); // CHECK MEM LEAK
    std::memset(buffer, 0, sizeof(script_t) + length + 1);
	script = (script_t *) buffer;
	std::memset(script, 0, sizeof(script_t));
	strcpy(script->filename, name);
	script->buffer = (char *) buffer + sizeof(script_t);
	script->buffer[length] = 0;
	script->length = length;
	//pointer in script buffer
	script->script_p = script->buffer;
	//pointer in script buffer before reading token
	script->lastscript_p = script->buffer;
	//pointer to end of script buffer
	script->end_p = &script->buffer[length];
	//set if there's a token available in script->token
	script->tokenavailable = 0;
	//
	script->line = 1;
	script->lastline = 1;
	//
	SetScriptPunctuations(script, NULL);
	//
	std::memcpy(script->buffer, ptr, length);
	//
	return script;
}

void FreeScript(script_t *script)
{
#ifdef PUNCTABLE
	if (script->punctuationtable) std::free(script->punctuationtable);
#endif //PUNCTABLE
	std::free(script);
}

//////////////////////////////////////////////////////////////////////////////
//                                preprocessor
//////////////////////////////////////////////////////////////////////////////

#define MAX_DEFINEPARMS			128

#define DEFINEHASHING			1

//directive name with parse function
typedef struct directive_s
{
	char *name;
	bool (*func)(source_t *source);
} directive_t;

#define DEFINEHASHSIZE		1024

#define TOKEN_HEAP_SIZE		4096

int numtokens;
/*
int tokenheapinitialized;				//true when the token heap is initialized
token_t token_heap[TOKEN_HEAP_SIZE];	//heap with tokens
token_t *freetokens;					//free tokens from the heap
*/

//list with global defines added to every source loaded
define_t *globaldefines;

void SourceError(source_t *source, char *str, ...)
{
	char text[1024];
	va_list ap;

	va_start(ap, str);
	vsprintf(text, str, ap);
	va_end(ap);

	printf("error: file %s, line %d: %s\n", source->scriptstack->filename, source->scriptstack->line, text);
}

void SourceWarning(source_t *source, char *str, ...)
{
	char text[1024];
	va_list ap;

	va_start(ap, str);
	vsprintf(text, str, ap);
	va_end(ap);

	printf("warning: file %s, line %d: %s\n", source->scriptstack->filename, source->scriptstack->line, text);
}

void PC_PushIndent(source_t *source, int type, int skip)
{
	indent_t *indent;

	indent = (indent_t *) std::malloc(sizeof(indent_t)); // CHECK MEM LEAK
	indent->type = type;
	indent->script = source->scriptstack;
	indent->skip = (skip != 0);
	source->skip += indent->skip;
	indent->next = source->indentstack;
	source->indentstack = indent;
}

void PC_PopIndent(source_t *source, int *type, int *skip)
{
	indent_t *indent;

	*type = 0;
	*skip = 0;

	indent = source->indentstack;
	if (!indent) return;

	//must be an indent from the current script
	if (source->indentstack->script != source->scriptstack) return;

	*type = indent->type;
	*skip = indent->skip;
	source->indentstack = source->indentstack->next;
	source->skip -= indent->skip;
	std::free(indent);
}

void PC_PushScript(source_t *source, script_t *script)
{
	script_t *s;

	for (s = source->scriptstack; s; s = s->next)
	{
		if (!stricmp(s->filename, script->filename))
		{
			SourceError(source, "%s recursively included", script->filename);
			return;
		}
	}

	//push the script on the script stack
	script->next = source->scriptstack;
	source->scriptstack = script;
}

token_t *PC_CopyToken(token_t *token)
{
	token_t *t;

	t = (token_t *) std::malloc(sizeof(token_t)); // CHECK MEM LEAK
//	t = freetokens;
	if (!t)
	{
		throw std::runtime_error("out of token space");
		//Com_Error(0, "out of token space\n");  // THROWWWWWW
		return NULL;
	}

//	freetokens = freetokens->next;
	std::memcpy(t, token, sizeof(token_t));
	t->next = NULL;
	numtokens++;
	return t;
}

void PC_FreeToken(token_t *token)
{
	std::free(token);
//	token->next = freetokens;
//	freetokens = token;
	numtokens--;
}

bool PC_ReadSourceToken(source_t *source, token_t *token)
{
	token_t *t;
	script_t *script;
	int type, skip;

	//if there's no token already available
	while(!source->tokens)
	{
		//if there's a token to read from the script
		if (PS_ReadToken(source->scriptstack, token)) return true;
    
		//if at the end of the script
		if (EndOfScript(source->scriptstack))
		{
			//remove all indents of the script
			while(source->indentstack && source->indentstack->script == source->scriptstack)
			{
				SourceWarning(source, "missing #endif");
				PC_PopIndent(source, &type, &skip);
			}
		}
    
		//if this was the initial script
		if (!source->scriptstack->next) return false;

		//remove the script and return to the last one
		script = source->scriptstack;
		source->scriptstack = source->scriptstack->next;
		FreeScript(script);
	}

	//copy the already available token
	std::memcpy(token, source->tokens, sizeof(token_t));
	//free the read token
	t = source->tokens;
	source->tokens = source->tokens->next;
	PC_FreeToken(t);
	return true;
}

bool PC_UnreadSourceToken(source_t *source, token_t *token)
{
	token_t *t;

	t = PC_CopyToken(token);
	t->next = source->tokens;
	source->tokens = t;
	return true;
}

bool PC_ReadDefineParms(source_t *source, define_t *define, token_t **parms, int maxparms)
{
	token_t token, *t, *last;
	int i, done, lastcomma, numparms, indent;

	if (!PC_ReadSourceToken(source, &token))
	{
		SourceError(source, "define %s missing parms", define->name);
		return false;
	}

	if (define->numparms > maxparms)
	{
		SourceError(source, "define with more than %d parameters", maxparms);
		return false;
	}

	for (i = 0; i < define->numparms; i++) parms[i] = NULL;
	//if no leading "("
	if (strcmp(token.string, "("))
	{
		PC_UnreadSourceToken(source, &token);
		SourceError(source, "define %s missing parms", define->name);
		return false;
	}

	//read the define parameters
	for (done = 0, numparms = 0, indent = 0; !done;)
	{
		if (numparms >= maxparms)
		{
			SourceError(source, "define %s with too many parms", define->name);
			return false;
		}
    
		if (numparms >= define->numparms)
		{
			SourceWarning(source, "define %s has too many parms", define->name);
			return false;
		}

		parms[numparms] = NULL;
		lastcomma = 1;
		last = NULL;
		while(!done)
		{
			if (!PC_ReadSourceToken(source, &token))
			{
				SourceError(source, "define %s incomplete", define->name);
				return false;
			}

			if (!strcmp(token.string, ","))
			{
				if (indent <= 0)
				{
					if (lastcomma) SourceWarning(source, "too many comma's");
					lastcomma = 1;
					break;
				}
			}
			lastcomma = 0;

			if (!strcmp(token.string, "("))
			{
				indent++;
				continue;
			}
			else if (!strcmp(token.string, ")"))
			{
				if (--indent <= 0)
				{
					if (!parms[define->numparms-1])
					{
						SourceWarning(source, "too few define parms");
					}
					done = 1;
					break;
				} 
			}

			if (numparms < define->numparms)
			{
				t = PC_CopyToken(&token);
				t->next = NULL;
				if (last) last->next = t;
				else parms[numparms] = t;
				last = t;
			}
		}
		numparms++;
	}
	return true;
}

bool PC_StringizeTokens(token_t *tokens, token_t *token)
{
	token_t *t;

	token->type = TT_STRING;
	token->whitespace_p = NULL;
	token->endwhitespace_p = NULL;
	token->string[0] = '\0';
	strcat(token->string, "\"");

	for (t = tokens; t; t = t->next)
	{
		strncat(token->string, t->string, MAX_TOKEN - strlen(token->string));
	}

	strncat(token->string, "\"", MAX_TOKEN - strlen(token->string));
	return true;
}

bool PC_MergeTokens(token_t *t1, token_t *t2)
{
	//merging of a name with a name or number
	if (t1->type == TT_NAME && (t2->type == TT_NAME || t2->type == TT_NUMBER))
	{
		strcat(t1->string, t2->string);
		return true;
	}

	//merging of two strings
	if (t1->type == TT_STRING && t2->type == TT_STRING)
	{
		//remove trailing double quote
		t1->string[strlen(t1->string)-1] = '\0';
		//concat without leading double quote
		strcat(t1->string, &t2->string[1]);
		return true;
	}

	//FIXME: merging of two number of the same sub type
	return false;
}

#if DEFINEHASHING
/*
void PC_PrintDefineHashTable(define_t **definehash)
{
	int i;
	define_t *d;

	for (i = 0; i < DEFINEHASHSIZE; i++)
	{
		Log_Write("%4d:", i);
		for (d = definehash[i]; d; d = d->hashnext)
		{
			Log_Write(" %s", d->name);
		}
		Log_Write("\n");
	}
}
*/
int PC_NameHash(char *name)
{
	int hash, i;

	hash = 0;
	for (i = 0; name[i] != '\0'; i++)
	{
		hash += name[i] * (119 + i);
		//hash += (name[i] << 7) + i;
		//hash += (name[i] << (i&15));
	} //end while
	hash = (hash ^ (hash >> 10) ^ (hash >> 20)) & (DEFINEHASHSIZE-1);
	return hash;
}

void PC_AddDefineToHash(define_t *define, define_t **definehash)
{
	int hash;

	hash = PC_NameHash(define->name);
	define->hashnext = definehash[hash];
	definehash[hash] = define;
}

define_t *PC_FindHashedDefine(define_t **definehash, char *name)
{
	define_t *d;
	int hash;

	hash = PC_NameHash(name);
	for (d = definehash[hash]; d; d = d->hashnext)
	{
		if (!strcmp(d->name, name)) return d;
	} //end for
	return NULL;
}

#endif //DEFINEHASHING

define_t *PC_FindDefine(define_t *defines, char *name)
{
	define_t *d;

	for (d = defines; d; d = d->next)
	{
		if (!strcmp(d->name, name)) return d;
	}

	return NULL;
}

int PC_FindDefineParm(define_t *define, char *name)
{
	token_t *p;
	int i;

	i = 0;
	for (p = define->parms; p; p = p->next)
	{
		if (!strcmp(p->string, name)) return i;
		i++;
	}

	return -1;
}

void PC_FreeDefine(define_t *define)
{
	token_t *t, *next;

	//free the define parameters
	for (t = define->parms; t; t = next)
	{
		next = t->next;
		PC_FreeToken(t);
	}

	//free the define tokens
	for (t = define->tokens; t; t = next)
	{
		next = t->next;
		PC_FreeToken(t);
	}

	//free the define
	std::free(define);
}

void PC_AddBuiltinDefines(source_t *source)
{
	int i;
	define_t *define;
	struct builtin
	{
		char *string;
		int builtin;
	} builtin[] = {
		{ "__LINE__",	BUILTIN_LINE },
		{ "__FILE__",	BUILTIN_FILE },
		{ "__DATE__",	BUILTIN_DATE },
		{ "__TIME__",	BUILTIN_TIME },
//		{ "__STDC__", BUILTIN_STDC },
		{ NULL, 0 }
	};

	for (i = 0; builtin[i].string; i++)
	{
		define = (define_t *) std::malloc(sizeof(define_t) + strlen(builtin[i].string) + 1); // CHECK MEM LEAK
		std::memset(define, 0, sizeof(define_t));
		define->name = (char *) define + sizeof(define_t);
		strcpy(define->name, builtin[i].string);
		define->flags |= DEFINE_FIXED;
		define->builtin = builtin[i].builtin;
		//add the define to the source
#if DEFINEHASHING
		PC_AddDefineToHash(define, source->definehash);
#else
		define->next = source->defines;
		source->defines = define;
#endif //DEFINEHASHING
	}
}

bool PC_ExpandBuiltinDefine(source_t *source, token_t *deftoken, define_t *define, token_t **firsttoken, token_t **lasttoken)
{
	token_t *token;
	time_t t;
	char *curtime;

	token = PC_CopyToken(deftoken);
	switch(define->builtin)
	{
		case BUILTIN_LINE:
		{
			sprintf(token->string, "%d", deftoken->line);
			token->intvalue = deftoken->line;
			token->floatvalue = deftoken->line;
			token->type = TT_NUMBER;
			token->subtype = TT_DECIMAL | TT_INTEGER;
			*firsttoken = token;
			*lasttoken = token;
			break;
		}
		case BUILTIN_FILE:
		{
			strcpy(token->string, source->scriptstack->filename);
			token->type = TT_NAME;
			token->subtype = strlen(token->string);
			*firsttoken = token;
			*lasttoken = token;
			break;
		}
		case BUILTIN_DATE:
		{
			t = time(NULL);
			curtime = ctime(&t);
			strcpy(token->string, "\"");
			strncat(token->string, curtime+4, 7);
			strncat(token->string+7, curtime+20, 4);
			strcat(token->string, "\"");
			free(curtime);
			token->type = TT_NAME;
			token->subtype = strlen(token->string);
			*firsttoken = token;
			*lasttoken = token;
			break;
		}
		case BUILTIN_TIME:
		{
			t = time(NULL);
			curtime = ctime(&t);
			strcpy(token->string, "\"");
			strncat(token->string, curtime+11, 8);
			strcat(token->string, "\"");
			free(curtime);
			token->type = TT_NAME;
			token->subtype = strlen(token->string);
			*firsttoken = token;
			*lasttoken = token;
			break;
		}
		case BUILTIN_STDC:
		default:
		{
			*firsttoken = NULL;
			*lasttoken = NULL;
			break;
		}
	}
	return true;
}

bool PC_ExpandDefine(source_t *source, token_t *deftoken, define_t *define, token_t **firsttoken, token_t **lasttoken)
{
	token_t *parms[MAX_DEFINEPARMS], *dt, *pt, *t;
	token_t *t1, *t2, *first, *last, *nextpt, token;
	int parmnum, i;

	//if it is a builtin define
	if (define->builtin)
	{
		return PC_ExpandBuiltinDefine(source, deftoken, define, firsttoken, lasttoken);
	} //end if
	//if the define has parameters
	if (define->numparms)
	{
		if (!PC_ReadDefineParms(source, define, parms, MAX_DEFINEPARMS)) return false;
#ifdef DEBUG_EVAL
		for (i = 0; i < define->numparms; i++)
		{
			Log_Write("define parms %d:", i);
			for (pt = parms[i]; pt; pt = pt->next)
			{
				Log_Write("%s", pt->string);
			} //end for
		} //end for
#endif //DEBUG_EVAL
	} //end if
	//empty list at first
	first = NULL;
	last = NULL;
	//create a list with tokens of the expanded define
	for (dt = define->tokens; dt; dt = dt->next)
	{
		parmnum = -1;
		//if the token is a name, it could be a define parameter
		if (dt->type == TT_NAME)
		{
			parmnum = PC_FindDefineParm(define, dt->string);
		} //end if
		//if it is a define parameter
		if (parmnum >= 0)
		{
			for (pt = parms[parmnum]; pt; pt = pt->next)
			{
				t = PC_CopyToken(pt);
				//add the token to the list
				t->next = NULL;
				if (last) last->next = t;
				else first = t;
				last = t;
			} //end for
		} //end if
		else
		{
			//if stringizing operator
			if (dt->string[0] == '#' && dt->string[1] == '\0')
			{
				//the stringizing operator must be followed by a define parameter
				if (dt->next) parmnum = PC_FindDefineParm(define, dt->next->string);
				else parmnum = -1;
				//
				if (parmnum >= 0)
				{
					//step over the stringizing operator
					dt = dt->next;
					//stringize the define parameter tokens
					if (!PC_StringizeTokens(parms[parmnum], &token))
					{
						SourceError(source, "can't stringize tokens");
						return false;
					} //end if
					t = PC_CopyToken(&token);
				} //end if
				else
				{
					SourceWarning(source, "stringizing operator without define parameter");
					continue;
				} //end if
			} //end if
			else
			{
				t = PC_CopyToken(dt);
			} //end else
			//add the token to the list
			t->next = NULL;
			if (last) last->next = t;
			else first = t;
			last = t;
		} //end else
	} //end for
	//check for the merging operator
	for (t = first; t; )
	{
		if (t->next)
		{
			//if the merging operator
			if (t->next->string[0] == '#' && t->next->string[1] == '#')
			{
				t1 = t;
				t2 = t->next->next;
				if (t2)
				{
					if (!PC_MergeTokens(t1, t2))
					{
						SourceError(source, "can't merge %s with %s", t1->string, t2->string);
						return false;
					} //end if
					PC_FreeToken(t1->next);
					t1->next = t2->next;
					if (t2 == last) last = t1;
					PC_FreeToken(t2);
					continue;
				} //end if
			} //end if
		} //end if
		t = t->next;
	}

	//store the first and last token of the list
	*firsttoken = first;
	*lasttoken = last;

	//free all the parameter tokens
	for (i = 0; i < define->numparms; i++)
	{
		for (pt = parms[i]; pt; pt = nextpt)
		{
			nextpt = pt->next;
			PC_FreeToken(pt);
		}
	}

	return true;
}

bool PC_ExpandDefineIntoSource(source_t *source, token_t *deftoken, define_t *define)
{
	token_t *firsttoken, *lasttoken;

	if (!PC_ExpandDefine(source, deftoken, define, &firsttoken, &lasttoken)) return false;

	if (firsttoken && lasttoken)
	{
		lasttoken->next = source->tokens;
		source->tokens = firsttoken;
		return true;
	}

	return false;
}

void PC_ConvertPath(char *path)
{
	char *ptr;

	//remove double path seperators
	for (ptr = path; *ptr;)
	{
		if ((*ptr == '\\' || *ptr == '/') &&
				(*(ptr+1) == '\\' || *(ptr+1) == '/'))
		{
			strcpy(ptr, ptr+1);
		} //end if
		else
		{
			ptr++;
		} //end else
	} //end while
	//set OS dependent path seperators
	for (ptr = path; *ptr;)
	{
		if (*ptr == '/' || *ptr == '\\') *ptr = '/';
		ptr++;
	} //end while
}

bool PC_Directive_include(source_t *source)
{
	script_t *script;
	token_t token;
	char path[MAX_PATH];

	if (source->skip > 0) return true;

	if (!PC_ReadSourceToken(source, &token))
	{
		SourceError(source, "#include without file name");
		return false;
	}

	if (token.linescrossed > 0)
	{
		SourceError(source, "#include without file name");
		return false;
	}

	if (token.type == TT_STRING)
	{
		StripDoubleQuotes(token.string);
		PC_ConvertPath(token.string);
		script = LoadScriptFile(token.string);
		if (!script)
		{
			strcpy(path, source->includepath);
			strcat(path, token.string);
			script = LoadScriptFile(path);
		}
	}
	else if (token.type == TT_PUNCTUATION && *token.string == '<')
	{
		strcpy(path, source->includepath);
		while(PC_ReadSourceToken(source, &token))
		{
			if (token.linescrossed > 0)
			{
				PC_UnreadSourceToken(source, &token);
				break;
			}

			if (token.type == TT_PUNCTUATION && *token.string == '>') break;
			strncat(path, token.string, MAX_PATH);
		}

		if (*token.string != '>')
		{
			SourceWarning(source, "#include missing trailing >");
		}

		if (!strlen(path))
		{
			SourceError(source, "#include without file name between < >");
			return false;
		}
		PC_ConvertPath(path);
		script = LoadScriptFile(path);
	}
	else
	{
		SourceError(source, "#include without file name");
		return false;
	}

	if (!script)
	{

		SourceError(source, "file %s not found", path);
		return false;
	}

	PC_PushScript(source, script);
	return true;
}

bool PC_ReadLine(source_t *source, token_t *token, bool expandDefines)
{
	int crossline;
	define_s* define;

	for (crossline = 0; ; crossline = 1)
	{
		while (true)
		{
			if (!PC_ReadSourceToken(source, token))
				return false;
			
			if (token->linescrossed > crossline)
			{
				PC_UnreadSourceToken(source, token);
				return false;
			}

			if (token->type != TT_NAME)
				break;

			if (!expandDefines)
				break;

			define = PC_FindHashedDefine(source->definehash, token->string);

			if (!define)
				break;

			if (!PC_ExpandDefineIntoSource(source, token, define))
				return false;
		}

		if (strcmp(token->string, "\\"))
			break;
	}

	return true;
}

bool PC_WhiteSpaceBeforeToken(token_t *token)
{
	return token->endwhitespace_p - token->whitespace_p > 0;
}

void PC_ClearTokenWhiteSpace(token_t *token)
{
	token->whitespace_p = NULL;
	token->endwhitespace_p = NULL;
	token->linescrossed = 0;
}

bool PC_Directive_undef(source_t *source)
{
	token_t token;
	define_t *define, *lastdefine;
	int hash;

	if (source->skip > 0) return true;

	if (!PC_ReadLine(source, &token, false))
	{
		SourceError(source, "undef without name");
		return false;
	}

	if (token.type != TT_NAME)
	{
		PC_UnreadSourceToken(source, &token);
		SourceError(source, "expected name, found %s", token.string);
		return false;
	}

#if DEFINEHASHING

	hash = PC_NameHash(token.string);
	for (lastdefine = NULL, define = source->definehash[hash]; define; define = define->hashnext)
	{
		if (!strcmp(define->name, token.string))
		{
			if (define->flags & DEFINE_FIXED)
			{
				SourceWarning(source, "can't undef %s", token.string);
			}
			else
			{
				if (lastdefine) lastdefine->hashnext = define->hashnext;
				else source->definehash[hash] = define->hashnext;
				PC_FreeDefine(define);
			}
			break;
		}
		lastdefine = define;
	}
#else //DEFINEHASHING
	for (lastdefine = NULL, define = source->defines; define; define = define->next)
	{
		if (!strcmp(define->name, token.string))
		{
			if (define->flags & DEFINE_FIXED)
			{
				SourceWarning(source, "can't undef %s", token.string);
			}
			else
			{
				if (lastdefine) lastdefine->next = define->next;
				else source->defines = define->next;
				PC_FreeDefine(define);
			}
			break;
		}
		lastdefine = define;
	}
#endif //DEFINEHASHING
	return true;
}

bool PC_Directive_define(source_t *source)
{
	token_t token, *t, *last;
	define_t *define;

	if (source->skip > 0) return true;

	if (!PC_ReadLine(source, &token, false))
	{
		SourceError(source, "#define without name");
		return false;
	}

	if (token.type != TT_NAME)
	{
		PC_UnreadSourceToken(source, &token);
		SourceError(source, "expected name after #define, found %s", token.string);
		return false;
	}

	//check if the define already exists
#if DEFINEHASHING
	define = PC_FindHashedDefine(source->definehash, token.string);
#else
	define = PC_FindDefine(source->defines, token.string);
#endif //DEFINEHASHING

	if (define)
	{
		if (define->flags & DEFINE_FIXED)
		{
			SourceError(source, "can't redefine %s", token.string);
			return false;
		}

		SourceWarning(source, "redefinition of %s", token.string);
		//unread the define name before executing the #undef directive
		PC_UnreadSourceToken(source, &token);

		if (!PC_Directive_undef(source)) return false;
		//if the define was not removed (define->flags & DEFINE_FIXED)

#if DEFINEHASHING
		define = PC_FindHashedDefine(source->definehash, token.string);
#else
		define = PC_FindDefine(source->defines, token.string);
#endif //DEFINEHASHING
	}

	//allocate define
	define = (define_t *) std::malloc(sizeof(define_t) + strlen(token.string) + 1); // CHECK MEM LEAK
	std::memset(define, 0, sizeof(define_t));
	define->name = (char *) define + sizeof(define_t);
	strcpy(define->name, token.string);
	//add the define to the source
#if DEFINEHASHING
	PC_AddDefineToHash(define, source->definehash);
#else //DEFINEHASHING
	define->next = source->defines;
	source->defines = define;
#endif //DEFINEHASHING

	//if nothing is defined, just return
	if (!PC_ReadLine(source, &token, false)) return true;

	//if it is a define with parameters
	if (!PC_WhiteSpaceBeforeToken(&token) && !strcmp(token.string, "("))
	{
		//read the define parameters
		last = NULL;
		if (!PC_CheckTokenString(source, ")"))
		{
			while(1)
			{
				if (!PC_ReadLine(source, &token, false))
				{
					SourceError(source, "expected define parameter");
					return false;
				}

				//if it isn't a name
				if (token.type != TT_NAME)
				{
					SourceError(source, "invalid define parameter");
					return false;
				}

				if (PC_FindDefineParm(define, token.string) >= 0)
				{
					SourceError(source, "two the same define parameters");
					return false;
				}
				//add the define parm
				t = PC_CopyToken(&token);
				PC_ClearTokenWhiteSpace(t);
				t->next = NULL;
				if (last) last->next = t;
				else define->parms = t;
				last = t;
				define->numparms++;
				//read next token
				if (!PC_ReadLine(source, &token, false))
				{
					SourceError(source, "define parameters not terminated");
					return false;
				} 

				if (!strcmp(token.string, ")")) break;
				//then it must be a comma
				if (strcmp(token.string, ","))
				{
					SourceError(source, "define not terminated");
					return false;
				}
			}
		}
		if (!PC_ReadLine(source, &token ,false)) return true;
	}
	//read the defined stuff
	last = NULL;
	do
	{
		t = PC_CopyToken(&token);
		if (t->type == TT_NAME && !strcmp(t->string, define->name))
		{
			SourceError(source, "recursive define (removed recursion)");
			continue;
		} //end if
		PC_ClearTokenWhiteSpace(t);
		t->next = NULL;
		if (last) last->next = t;
		else define->tokens = t;
		last = t;
	} while(PC_ReadLine(source, &token, false));

	if (last)
	{
		//check for merge operators at the beginning or end
		if (!strcmp(define->tokens->string, "##") ||
				!strcmp(last->string, "##"))
		{
			SourceError(source, "define with misplaced ##");
			return false;
		}
	}
	return true;
}

define_t *PC_DefineFromString(const char *string)
{
	script_t *script;
	source_t src;
	token_t *t;
	int res, i;
	define_t *def;

	//PC_InitTokenHeap();

	script = LoadScriptMemory(string, strlen(string), "*extern");
	//create a new source
	std::memset(&src, 0, sizeof(source_t));
	strncpy(src.filename, "*extern", MAX_PATH);
	src.scriptstack = script;
#if DEFINEHASHING
	src.definehash = (define_t**)std::malloc(DEFINEHASHSIZE * sizeof(define_t *)); // CHECK MEM LEAK
    std::memset(src.definehash, 0, DEFINEHASHSIZE * sizeof(define_t *));
#endif //DEFINEHASHING

	//create a define from the source
	res = PC_Directive_define(&src);
	//free any tokens if left
	for (t = src.tokens; t; t = src.tokens)
	{
		src.tokens = src.tokens->next;
		PC_FreeToken(t);
	}

#ifdef DEFINEHASHING
	def = NULL;
	for (i = 0; i < DEFINEHASHSIZE; i++)
	{
		if (src.definehash[i])
		{
			def = src.definehash[i];
			break;
		} //end if
	} //end for
#else
	def = src.defines;
#endif //DEFINEHASHING

#if DEFINEHASHING
	std::free(src.definehash);
#endif //DEFINEHASHING

	FreeScript(script);
	//if the define was created succesfully
	if (res > 0) return def;

	//free the define is created
	if (src.defines) PC_FreeDefine(def);

	return NULL;
}

bool PC_AddDefine(source_t *source, const char *string)
{
	define_t *define;

	define = PC_DefineFromString(string);

	if (!define) return false;

#if DEFINEHASHING
	PC_AddDefineToHash(define, source->definehash);
#else //DEFINEHASHING
	define->next = source->defines;
	source->defines = define;
#endif //DEFINEHASHING

	return true;
}

bool PC_AddGlobalDefine(char *string)
{
	define_t *define;

	define = PC_DefineFromString(string);

	if (!define) return false;

	define->next = globaldefines;
	globaldefines = define;
	return true;
}

bool PC_RemoveGlobalDefine(char *name)
{
	define_t *define;

	define = PC_FindDefine(globaldefines, name);

	if (define)
	{
		PC_FreeDefine(define);
		return true;
	}

	return false;
}

void PC_RemoveAllGlobalDefines(void)
{
	define_t *define;

	for (define = globaldefines; define; define = globaldefines)
	{
		globaldefines = globaldefines->next;
		PC_FreeDefine(define);
	}
}

define_t *PC_CopyDefine(source_t *source, define_t *define)
{
	define_t *newdefine;
	token_t *token, *newtoken, *lasttoken;

	newdefine = (define_t *) std::malloc(sizeof(define_t) + strlen(define->name) + 1); // CHECK MEM LEAK
	//copy the define name
	newdefine->name = (char *) newdefine + sizeof(define_t);
	strcpy(newdefine->name, define->name);
	newdefine->flags = define->flags;
	newdefine->builtin = define->builtin;
	newdefine->numparms = define->numparms;
	//the define is not linked
	newdefine->next = NULL;
	newdefine->hashnext = NULL;
	//copy the define tokens
	newdefine->tokens = NULL;

	for (lasttoken = NULL, token = define->tokens; token; token = token->next)
	{
		newtoken = PC_CopyToken(token);
		newtoken->next = NULL;
		if (lasttoken) lasttoken->next = newtoken;
		else newdefine->tokens = newtoken;
		lasttoken = newtoken;
	}

	//copy the define parameters
	newdefine->parms = NULL;
	for (lasttoken = NULL, token = define->parms; token; token = token->next)
	{
		newtoken = PC_CopyToken(token);
		newtoken->next = NULL;
		if (lasttoken) lasttoken->next = newtoken;
		else newdefine->parms = newtoken;
		lasttoken = newtoken;
	}
	return newdefine;
}

void PC_AddGlobalDefinesToSource(source_t *source)
{
	define_t *define, *newdefine;

	for (define = globaldefines; define; define = define->next)
	{
		newdefine = PC_CopyDefine(source, define);
#if DEFINEHASHING
		PC_AddDefineToHash(newdefine, source->definehash);
#else //DEFINEHASHING
		newdefine->next = source->defines;
		source->defines = newdefine;
#endif //DEFINEHASHING
	}
}

bool PC_Directive_if_def(source_t *source, int type)
{
	token_t token;
	define_t *d;
	int skip;

	if (!PC_ReadLine(source, &token, false))
	{
		SourceError(source, "#ifdef without name");
		return false;
	}

	if (token.type != TT_NAME)
	{
		PC_UnreadSourceToken(source, &token);
		SourceError(source, "expected name after #ifdef, found %s", token.string);
		return false;
	}

#if DEFINEHASHING
	d = PC_FindHashedDefine(source->definehash, token.string);
#else
	d = PC_FindDefine(source->defines, token.string);
#endif //DEFINEHASHING

	skip = (type == INDENT_IFDEF) == (d == NULL);
	PC_PushIndent(source, type, skip);
	return true;
}

bool PC_Directive_ifdef(source_t *source)
{
	return PC_Directive_if_def(source, INDENT_IFDEF);
}

bool PC_Directive_ifndef(source_t *source)
{
	return PC_Directive_if_def(source, INDENT_IFNDEF);
}

bool PC_Directive_else(source_t *source)
{
	int type, skip;

	PC_PopIndent(source, &type, &skip);

	if (!type)
	{
		SourceError(source, "misplaced #else");
		return false;
	}

	if (type == INDENT_ELSE)
	{
		SourceError(source, "#else after #else");
		return false;
	}

	PC_PushIndent(source, INDENT_ELSE, !skip);
	return true;
}

bool PC_Directive_endif(source_t *source)
{
	int type, skip;

	PC_PopIndent(source, &type, &skip);

	if (!type)
	{
		SourceError(source, "misplaced #endif");
		return false;
	}

	return true;
}

typedef struct operator_s
{
	int operator_n;
	int priority;
	int parentheses;
	struct operator_s *prev, *next;
} operator_t;

typedef struct value_s
{
	signed long int intvalue;
	double floatvalue;
	int parentheses;
	struct value_s *prev, *next;
} value_t;

int PC_OperatorPriority(int op)
{
	switch(op)
	{
		case P_MUL: return 15;
		case P_DIV: return 15;
		case P_MOD: return 15;
		case P_ADD: return 14;
		case P_SUB: return 14;

		case P_LOGIC_AND: return 7;
		case P_LOGIC_OR: return 6;
		case P_LOGIC_GEQ: return 12;
		case P_LOGIC_LEQ: return 12;
		case P_LOGIC_EQ: return 11;
		case P_LOGIC_UNEQ: return 11;

		case P_LOGIC_NOT: return 16;
		case P_LOGIC_GREATER: return 12;
		case P_LOGIC_LESS: return 12;

		case P_RSHIFT: return 13;
		case P_LSHIFT: return 13;

		case P_BIN_AND: return 10;
		case P_BIN_OR: return 8;
		case P_BIN_XOR: return 9;
		case P_BIN_NOT: return 16;

		case P_COLON: return 5;
		case P_QUESTIONMARK: return 5;
	}

	return 0;
}

//#define AllocValue()			GetClearedMemory(sizeof(value_t));
//#define FreeValue(val)		FreeMemory(val)
//#define AllocOperator(op)		op = (operator_t *) GetClearedMemory(sizeof(operator_t));
//#define FreeOperator(op)		FreeMemory(op);

#define MAX_VALUES		64
#define MAX_OPERATORS	64
#define AllocValue(val)									\
	if (numvalues >= MAX_VALUES) {						\
		SourceError(source, "out of value space\n");		\
		error = 1;										\
		break;											\
	}													\
	else												\
		val = &value_heap[numvalues++];
#define FreeValue(val)
//
#define AllocOperator(op)								\
	if (numoperators >= MAX_OPERATORS) {				\
		SourceError(source, "out of operator space\n");	\
		error = 1;										\
		break;											\
	}													\
	else												\
		op = &operator_heap[numoperators++];
#define FreeOperator(op)

bool PC_EvaluateTokens(source_t *source, token_t *tokens, signed long int *intvalue, double *floatvalue, int integer)
{
	operator_t *o, *firstoperator, *lastoperator;
	value_t *v, *firstvalue, *lastvalue, *v1, *v2;
	token_t *t;
	int brace = 0;
	int parentheses = 0;
	int error = 0;
	int lastwasvalue = 0;
	int negativevalue = 0;
	int questmarkintvalue = 0;
	double questmarkfloatvalue = 0;
	bool gotquestmarkvalue = false;
	int lastoperatortype = 0;
	//
	operator_t operator_heap[MAX_OPERATORS];
	int numoperators = 0;
	value_t value_heap[MAX_VALUES];
	int numvalues = 0;

	firstoperator = lastoperator = NULL;
	firstvalue = lastvalue = NULL;
	if (intvalue) *intvalue = 0;
	if (floatvalue) *floatvalue = 0;
	for (t = tokens; t; t = t->next)
	{
		switch(t->type)
		{
			case TT_NAME:
			{
				if (lastwasvalue || negativevalue)
				{
					SourceError(source, "syntax error in #if/#elif");
					error = 1;
					break;
				} //end if
				if (strcmp(t->string, "defined"))
				{
					SourceError(source, "undefined name %s in #if/#elif", t->string);
					error = 1;
					break;
				} //end if
				t = t->next;
				if (!strcmp(t->string, "("))
				{
					brace = true;
					t = t->next;
				} //end if
				if (!t || t->type != TT_NAME)
				{
					SourceError(source, "defined without name in #if/#elif");
					error = 1;
					break;
				} //end if
				//v = (value_t *) GetClearedMemory(sizeof(value_t));
				AllocValue(v);
#if DEFINEHASHING
				if (PC_FindHashedDefine(source->definehash, t->string))
#else			
				if (PC_FindDefine(source->defines, t->string))
#endif //DEFINEHASHING
				{
					v->intvalue = 1;
					v->floatvalue = 1;
				} //end if
				else
				{
					v->intvalue = 0;
					v->floatvalue = 0;
				} //end else
				v->parentheses = parentheses;
				v->next = NULL;
				v->prev = lastvalue;
				if (lastvalue) lastvalue->next = v;
				else firstvalue = v;
				lastvalue = v;
				if (brace)
				{
					t = t->next;
					if (!t || strcmp(t->string, ")"))
					{
						SourceError(source, "defined without ) in #if/#elif");
						error = 1;
						break;
					} //end if
				} //end if
				brace = false;
				// defined() creates a value
				lastwasvalue = 1;
				break;
			} //end case
			case TT_NUMBER:
			{
				if (lastwasvalue)
				{
					SourceError(source, "syntax error in #if/#elif");
					error = 1;
					break;
				} //end if
				//v = (value_t *) GetClearedMemory(sizeof(value_t));
				AllocValue(v);
				if (negativevalue)
				{
					v->intvalue = - (signed int) t->intvalue;
					v->floatvalue = - t->floatvalue;
				} //end if
				else
				{
					v->intvalue = t->intvalue;
					v->floatvalue = t->floatvalue;
				} //end else
				v->parentheses = parentheses;
				v->next = NULL;
				v->prev = lastvalue;
				if (lastvalue) lastvalue->next = v;
				else firstvalue = v;
				lastvalue = v;
				//last token was a value
				lastwasvalue = 1;
				//
				negativevalue = 0;
				break;
			} //end case
			case TT_PUNCTUATION:
			{
				if (negativevalue)
				{
					SourceError(source, "misplaced minus sign in #if/#elif");
					error = 1;
					break;
				} //end if
				if (t->subtype == P_PARENTHESESOPEN)
				{
					parentheses++;
					break;
				} //end if
				else if (t->subtype == P_PARENTHESESCLOSE)
				{
					parentheses--;
					if (parentheses < 0)
					{
						SourceError(source, "too many ) in #if/#elsif");
						error = 1;
					} //end if
					break;
				} //end else if
				//check for invalid operators on floating point values
				if (!integer)
				{
					if (t->subtype == P_BIN_NOT || t->subtype == P_MOD ||
						t->subtype == P_RSHIFT || t->subtype == P_LSHIFT ||
						t->subtype == P_BIN_AND || t->subtype == P_BIN_OR ||
						t->subtype == P_BIN_XOR)
					{
						SourceError(source, "illigal operator %s on floating point operands\n", t->string);
						error = 1;
						break;
					} //end if
				} //end if
				switch(t->subtype)
				{
					case P_LOGIC_NOT:
					case P_BIN_NOT:
					{
						if (lastwasvalue)
						{
							SourceError(source, "! or ~ after value in #if/#elif");
							error = 1;
							break;
						} //end if
						break;
					} //end case
					case P_INC:
					case P_DEC:
					{
						SourceError(source, "++ or -- used in #if/#elif");
						break;
					} //end case
					case P_SUB:
					{
						if (!lastwasvalue)
						{
							negativevalue = 1;
							break;
						} //end if
					} //end case
					
					case P_MUL:
					case P_DIV:
					case P_MOD:
					case P_ADD:

					case P_LOGIC_AND:
					case P_LOGIC_OR:
					case P_LOGIC_GEQ:
					case P_LOGIC_LEQ:
					case P_LOGIC_EQ:
					case P_LOGIC_UNEQ:

					case P_LOGIC_GREATER:
					case P_LOGIC_LESS:

					case P_RSHIFT:
					case P_LSHIFT:

					case P_BIN_AND:
					case P_BIN_OR:
					case P_BIN_XOR:

					case P_COLON:
					case P_QUESTIONMARK:
					{
						if (!lastwasvalue)
						{
							SourceError(source, "operator %s after operator in #if/#elif", t->string);
							error = 1;
							break;
						} //end if
						break;
					} //end case
					default:
					{
						SourceError(source, "invalid operator %s in #if/#elif", t->string);
						error = 1;
						break;
					} //end default
				} //end switch
				if (!error && !negativevalue)
				{
					//o = (operator_t *) GetClearedMemory(sizeof(operator_t));
					AllocOperator(o);
					o->operator_n = t->subtype;
					o->priority = PC_OperatorPriority(t->subtype);
					o->parentheses = parentheses;
					o->next = NULL;
					o->prev = lastoperator;
					if (lastoperator) lastoperator->next = o;
					else firstoperator = o;
					lastoperator = o;
					lastwasvalue = 0;
				} //end if
				break;
			} //end case
			default:
			{
				SourceError(source, "unknown %s in #if/#elif", t->string);
				error = 1;
				break;
			} //end default
		} //end switch
		if (error) break;
	} //end for
	if (!error)
	{
		if (!lastwasvalue)
		{
			SourceError(source, "trailing operator in #if/#elif");
			error = 1;
		} //end if
		else if (parentheses)
		{
			SourceError(source, "too many ( in #if/#elif");
			error = 1;
		} //end else if
	} //end if
	//
	gotquestmarkvalue = false;
	questmarkintvalue = 0;
	questmarkfloatvalue = 0;
	//while there are operators
	while(!error && firstoperator)
	{
		v = firstvalue;
		for (o = firstoperator; o->next; o = o->next)
		{
			//if the current operator is nested deeper in parentheses
			//than the next operator
			if (o->parentheses > o->next->parentheses) break;
			//if the current and next operator are nested equally deep in parentheses
			if (o->parentheses == o->next->parentheses)
			{
				//if the priority of the current operator is equal or higher
				//than the priority of the next operator
				if (o->priority >= o->next->priority) break;
			} //end if
			//if the arity of the operator isn't equal to 1
			if (o->operator_n != P_LOGIC_NOT
					&& o->operator_n != P_BIN_NOT) v = v->next;
			//if there's no value or no next value
			if (!v)
			{
				SourceError(source, "mising values in #if/#elif");
				error = 1;
				break;
			} //end if
		} //end for
		if (error) break;
		v1 = v;
		v2 = v->next;
#ifdef DEBUG_EVAL
		if (integer)
		{
			Log_Write("operator %s, value1 = %d", PunctuationFromNum(source->scriptstack, o->operator), v1->intvalue);
			if (v2) Log_Write("value2 = %d", v2->intvalue);
		} //end if
		else
		{
			Log_Write("operator %s, value1 = %f", PunctuationFromNum(source->scriptstack, o->operator), v1->floatvalue);
			if (v2) Log_Write("value2 = %f", v2->floatvalue);
		} //end else
#endif //DEBUG_EVAL
		switch(o->operator_n)
		{
			case P_LOGIC_NOT:		v1->intvalue = !v1->intvalue;
									v1->floatvalue = !v1->floatvalue; break;
			case P_BIN_NOT:			v1->intvalue = ~v1->intvalue;
									break;
			case P_MUL:				v1->intvalue *= v2->intvalue;
									v1->floatvalue *= v2->floatvalue; break;
			case P_DIV:				if (!v2->intvalue || !v2->floatvalue)
									{
										SourceError(source, "divide by zero in #if/#elif\n");
										error = 1;
										break;
									}
									v1->intvalue /= v2->intvalue;
									v1->floatvalue /= v2->floatvalue; break;
			case P_MOD:				if (!v2->intvalue)
									{
										SourceError(source, "divide by zero in #if/#elif\n");
										error = 1;
										break;
									}
									v1->intvalue %= v2->intvalue; break;
			case P_ADD:				v1->intvalue += v2->intvalue;
									v1->floatvalue += v2->floatvalue; break;
			case P_SUB:				v1->intvalue -= v2->intvalue;
									v1->floatvalue -= v2->floatvalue; break;
			case P_LOGIC_AND:		v1->intvalue = v1->intvalue && v2->intvalue;
									v1->floatvalue = v1->floatvalue && v2->floatvalue; break;
			case P_LOGIC_OR:		v1->intvalue = v1->intvalue || v2->intvalue;
									v1->floatvalue = v1->floatvalue || v2->floatvalue; break;
			case P_LOGIC_GEQ:		v1->intvalue = v1->intvalue >= v2->intvalue;
									v1->floatvalue = v1->floatvalue >= v2->floatvalue; break;
			case P_LOGIC_LEQ:		v1->intvalue = v1->intvalue <= v2->intvalue;
									v1->floatvalue = v1->floatvalue <= v2->floatvalue; break;
			case P_LOGIC_EQ:		v1->intvalue = v1->intvalue == v2->intvalue;
									v1->floatvalue = v1->floatvalue == v2->floatvalue; break;
			case P_LOGIC_UNEQ:		v1->intvalue = v1->intvalue != v2->intvalue;
									v1->floatvalue = v1->floatvalue != v2->floatvalue; break;
			case P_LOGIC_GREATER:	v1->intvalue = v1->intvalue > v2->intvalue;
									v1->floatvalue = v1->floatvalue > v2->floatvalue; break;
			case P_LOGIC_LESS:		v1->intvalue = v1->intvalue < v2->intvalue;
									v1->floatvalue = v1->floatvalue < v2->floatvalue; break;
			case P_RSHIFT:			v1->intvalue >>= v2->intvalue;
									break;
			case P_LSHIFT:			v1->intvalue <<= v2->intvalue;
									break;
			case P_BIN_AND:			v1->intvalue &= v2->intvalue;
									break;
			case P_BIN_OR:			v1->intvalue |= v2->intvalue;
									break;
			case P_BIN_XOR:			v1->intvalue ^= v2->intvalue;
									break;
			case P_COLON:
			{
				if (!gotquestmarkvalue)
				{
					SourceError(source, ": without ? in #if/#elif");
					error = 1;
					break;
				} //end if
				if (integer)
				{
					if (!questmarkintvalue) v1->intvalue = v2->intvalue;
				} //end if
				else
				{
					if (!questmarkfloatvalue) v1->floatvalue = v2->floatvalue;
				} //end else
				gotquestmarkvalue = false;
				break;
			} //end case
			case P_QUESTIONMARK:
			{
				if (gotquestmarkvalue)
				{
					SourceError(source, "? after ? in #if/#elif");
					error = 1;
					break;
				} //end if
				questmarkintvalue = v1->intvalue;
				questmarkfloatvalue = v1->floatvalue;
				gotquestmarkvalue = true;
				break;
			} //end if
		} //end switch
#ifdef DEBUG_EVAL
		if (integer) Log_Write("result value = %d", v1->intvalue);
		else Log_Write("result value = %f", v1->floatvalue);
#endif //DEBUG_EVAL
		if (error) break;
		lastoperatortype = o->operator_n;
		//if not an operator with arity 1
		if (o->operator_n != P_LOGIC_NOT
				&& o->operator_n != P_BIN_NOT)
		{
			//remove the second value if not question mark operator
			if (o->operator_n != P_QUESTIONMARK) v = v->next;
			//
			if (v->prev) v->prev->next = v->next;
			else firstvalue = v->next;
			if (v->next) v->next->prev = v->prev;
			else lastvalue = v->prev;
			//FreeMemory(v);
			FreeValue(v);
		} //end if
		//remove the operator
		if (o->prev) o->prev->next = o->next;
		else firstoperator = o->next;
		if (o->next) o->next->prev = o->prev;
		else lastoperator = o->prev;
		//FreeMemory(o);
		FreeOperator(o);
	} //end while
	if (firstvalue)
	{
		if (intvalue) *intvalue = firstvalue->intvalue;
		if (floatvalue) *floatvalue = firstvalue->floatvalue;
	} //end if
	for (o = firstoperator; o; o = lastoperator)
	{
		lastoperator = o->next;
		//FreeMemory(o);
		FreeOperator(o);
	} //end for
	for (v = firstvalue; v; v = lastvalue)
	{
		lastvalue = v->next;
		//FreeMemory(v);
		FreeValue(v);
	} //end for
	if (!error) return true;
	if (intvalue) *intvalue = 0;
	if (floatvalue) *floatvalue = 0;
	return false;
}

bool PC_Evaluate(source_t *source, signed long int *intvalue, double *floatvalue, int integer)
{
	token_t token, *firsttoken, *lasttoken;
	token_t *t, *nexttoken;
	define_t *define;
	bool defined = false;

	if (intvalue) *intvalue = 0;
	if (floatvalue) *floatvalue = 0;

	if (!PC_ReadLine(source, &token, true))
	{
		SourceError(source, "no value after #if/#elif");
		return false;
	} //end if
	firsttoken = NULL;
	lasttoken = NULL;
	do
	{
		//if the token is a name
		if (token.type == TT_NAME)
		{
			if (defined)
			{
				defined = false;
				t = PC_CopyToken(&token);
				t->next = NULL;
				if (lasttoken) lasttoken->next = t;
				else firsttoken = t;
				lasttoken = t;
			} //end if
			else if (!strcmp(token.string, "defined"))
			{
				defined = true;
				t = PC_CopyToken(&token);
				t->next = NULL;
				if (lasttoken) lasttoken->next = t;
				else firsttoken = t;
				lasttoken = t;
			} //end if
			else
			{
				//then it must be a define
#if DEFINEHASHING
				define = PC_FindHashedDefine(source->definehash, token.string);
#else
				define = PC_FindDefine(source->defines, token.string);
#endif //DEFINEHASHING
				if (!define)
				{
					SourceError(source, "can't evaluate %s, not defined", token.string);
					return false;
				} //end if
				if (!PC_ExpandDefineIntoSource(source, &token, define)) return false;
			} //end else
		} //end if
		//if the token is a number or a punctuation
		else if (token.type == TT_NUMBER || token.type == TT_PUNCTUATION)
		{
			t = PC_CopyToken(&token);
			t->next = NULL;
			if (lasttoken) lasttoken->next = t;
			else firsttoken = t;
			lasttoken = t;
		} //end else
		else //can't evaluate the token
		{
			SourceError(source, "can't evaluate %s", token.string);
			return false;
		} //end else
	} while(PC_ReadLine(source, &token, defined == false));
	//
	if (!PC_EvaluateTokens(source, firsttoken, intvalue, floatvalue, integer)) return false;
	//
#ifdef DEBUG_EVAL
	Log_Write("eval:");
#endif //DEBUG_EVAL
	for (t = firsttoken; t; t = nexttoken)
	{
#ifdef DEBUG_EVAL
		Log_Write(" %s", t->string);
#endif //DEBUG_EVAL
		nexttoken = t->next;
		PC_FreeToken(t);
	} //end for
#ifdef DEBUG_EVAL
	if (integer) Log_Write("eval result: %d", *intvalue);
	else Log_Write("eval result: %f", *floatvalue);
#endif //DEBUG_EVAL
	//
	return true;
}

bool PC_DollarEvaluate(source_t *source, signed long int *intvalue, double *floatvalue, int integer)
{
	int indent, defined = false;
	token_t token, *firsttoken, *lasttoken;
	token_t *t, *nexttoken;
	define_t *define;

	if (intvalue) *intvalue = 0;
	if (floatvalue) *floatvalue = 0;
	//
	if (!PC_ReadSourceToken(source, &token))
	{
		SourceError(source, "no leading ( after $evalint/$evalfloat");
		return false;
	} //end if
	if (!PC_ReadSourceToken(source, &token))
	{
		SourceError(source, "nothing to evaluate");
		return false;
	} //end if
	indent = 1;
	firsttoken = NULL;
	lasttoken = NULL;
	do
	{
		//if the token is a name
		if (token.type == TT_NAME)
		{
			if (defined)
			{
				defined = false;
				t = PC_CopyToken(&token);
				t->next = NULL;
				if (lasttoken) lasttoken->next = t;
				else firsttoken = t;
				lasttoken = t;
			} //end if
			else if (!strcmp(token.string, "defined"))
			{
				defined = true;
				t = PC_CopyToken(&token);
				t->next = NULL;
				if (lasttoken) lasttoken->next = t;
				else firsttoken = t;
				lasttoken = t;
			} //end if
			else
			{
				//then it must be a define
#if DEFINEHASHING
				define = PC_FindHashedDefine(source->definehash, token.string);
#else
				define = PC_FindDefine(source->defines, token.string);
#endif //DEFINEHASHING
				if (!define)
				{
					SourceError(source, "can't evaluate %s, not defined", token.string);
					return false;
				} //end if
				if (!PC_ExpandDefineIntoSource(source, &token, define)) return false;
			} //end else
		} //end if
		//if the token is a number or a punctuation
		else if (token.type == TT_NUMBER || token.type == TT_PUNCTUATION)
		{
			if (*token.string == '(') indent++;
			else if (*token.string == ')') indent--;
			if (indent <= 0) break;
			t = PC_CopyToken(&token);
			t->next = NULL;
			if (lasttoken) lasttoken->next = t;
			else firsttoken = t;
			lasttoken = t;
		} //end else
		else //can't evaluate the token
		{
			SourceError(source, "can't evaluate %s", token.string);
			return false;
		} //end else
	} while(PC_ReadSourceToken(source, &token));
	//
	if (!PC_EvaluateTokens(source, firsttoken, intvalue, floatvalue, integer)) return false;
	//
#ifdef DEBUG_EVAL
	Log_Write("$eval:");
#endif //DEBUG_EVAL
	for (t = firsttoken; t; t = nexttoken)
	{
#ifdef DEBUG_EVAL
		Log_Write(" %s", t->string);
#endif //DEBUG_EVAL
		nexttoken = t->next;
		PC_FreeToken(t);
	} //end for
#ifdef DEBUG_EVAL
	if (integer) Log_Write("$eval result: %d", *intvalue);
	else Log_Write("$eval result: %f", *floatvalue);
#endif //DEBUG_EVAL
	//
	return true;
}

bool PC_Directive_elif(source_t *source)
{
	signed long int value;
	int type, skip;

	PC_PopIndent(source, &type, &skip);

	if (!type || type == INDENT_ELSE)
	{
		SourceError(source, "misplaced #elif");
		return false;
	}

	if (!PC_Evaluate(source, &value, NULL, true))
        return false;

	skip = (value == 0);
	PC_PushIndent(source, INDENT_ELIF, skip);
	return true;
}

bool PC_Directive_if(source_t *source)
{
	signed long int value;
	int skip;

	if (!PC_Evaluate(source, &value, NULL, true))
        return false;

	skip = (value == 0);
	PC_PushIndent(source, INDENT_IF, skip);
	return true;
}

bool PC_Directive_line(source_t *source)
{
	SourceError(source, "#line directive not supported");
	return false;
}

bool PC_Directive_error(source_t *source)
{
	token_t token;

	strcpy(token.string, "");
	PC_ReadSourceToken(source, &token);
	SourceError(source, "#error directive: %s", token.string);
	return false;
}

bool PC_Directive_pragma(source_t *source)
{
	token_t token;

	SourceWarning(source, "#pragma directive not supported");

	while(PC_ReadLine(source, &token, false));

	return true;
}

void UnreadSignToken(source_t *source)
{
	token_t token;

	token.line = source->scriptstack->line;
	token.whitespace_p = source->scriptstack->script_p;
	token.endwhitespace_p = source->scriptstack->script_p;
	token.linescrossed = 0;
	strcpy(token.string, "-");
	token.type = TT_PUNCTUATION;
	token.subtype = P_SUB;
	PC_UnreadSourceToken(source, &token);
}

bool PC_Directive_eval(source_t *source)
{
	signed long int value;
	token_t token;

	if (!PC_Evaluate(source, &value, NULL, true))
        return false;

	token.line = source->scriptstack->line;
	token.whitespace_p = source->scriptstack->script_p;
	token.endwhitespace_p = source->scriptstack->script_p;
	token.linescrossed = 0;
	sprintf(token.string, "%d", abs(value));
	token.type = TT_NUMBER;
	token.subtype = TT_INTEGER|TT_LONG|TT_DECIMAL;
	PC_UnreadSourceToken(source, &token);
	if (value < 0) UnreadSignToken(source);
	return true;
}

bool PC_Directive_evalfloat(source_t *source)
{
	double value;
	token_t token;

	if (!PC_Evaluate(source, NULL, &value, false))
        return false;

	token.line = source->scriptstack->line;
	token.whitespace_p = source->scriptstack->script_p;
	token.endwhitespace_p = source->scriptstack->script_p;
	token.linescrossed = 0;
	sprintf(token.string, "%1.2f", fabs(value));
	token.type = TT_NUMBER;
	token.subtype = TT_FLOAT|TT_LONG|TT_DECIMAL;
	PC_UnreadSourceToken(source, &token);
	if (value < 0) UnreadSignToken(source);
	return true;
}

directive_t directives[20] =
{
	{ "if", PC_Directive_if },
	{ "ifdef", PC_Directive_ifdef },
	{ "ifndef", PC_Directive_ifndef },
	{ "elif", PC_Directive_elif },
	{ "else", PC_Directive_else },
	{ "endif", PC_Directive_endif },
	{ "include", PC_Directive_include },
	{ "define", PC_Directive_define },
	{ "undef", PC_Directive_undef },
	{ "line", PC_Directive_line },
	{ "error", PC_Directive_error },
	{ "pragma", PC_Directive_pragma },
	{ "eval", PC_Directive_eval },
	{ "evalfloat", PC_Directive_evalfloat },
	{ NULL, NULL }
};

bool PC_ReadDirective(source_t *source)
{
	token_t token;
	int i;

	//read the directive name
	if (!PC_ReadSourceToken(source, &token))
	{
		SourceError(source, "found # without name");
		return false;
	}

	//directive name must be on the same line
	if (token.linescrossed > 0)
	{
		PC_UnreadSourceToken(source, &token);
		SourceError(source, "found # at end of line");
		return false;
	}

	//if if is a name
	if (token.type == TT_NAME)
	{
		//find the precompiler directive
		for (i = 0; directives[i].name; i++)
		{
			if (!strcmp(directives[i].name, token.string))
			{
				return directives[i].func(source);
			}
		}
	}

	SourceError(source, "unknown precompiler directive %s", token.string);
	return false;
}

bool PC_DollarDirective_evalint(source_t *source)
{
	signed long int value;
	token_t token;

	if (!PC_DollarEvaluate(source, &value, NULL, true))
        return false;

	token.line = source->scriptstack->line;
	token.whitespace_p = source->scriptstack->script_p;
	token.endwhitespace_p = source->scriptstack->script_p;
	token.linescrossed = 0;
	sprintf(token.string, "%d", abs(value));
	token.type = TT_NUMBER;
	token.subtype = TT_INTEGER|TT_LONG|TT_DECIMAL;
	token.intvalue = value;
	token.floatvalue = value;
	PC_UnreadSourceToken(source, &token);
	if (value < 0) UnreadSignToken(source);
	return true;
}

bool PC_DollarDirective_evalfloat(source_t *source)
{
	double value;
	token_t token;

	if (!PC_DollarEvaluate(source, NULL, &value, false)) return false;
	token.line = source->scriptstack->line;
	token.whitespace_p = source->scriptstack->script_p;
	token.endwhitespace_p = source->scriptstack->script_p;
	token.linescrossed = 0;
	sprintf(token.string, "%1.2f", fabs(value));
	token.type = TT_NUMBER;
	token.subtype = TT_FLOAT|TT_LONG|TT_DECIMAL;
	token.intvalue = (unsigned long) value;
	token.floatvalue = value;
	PC_UnreadSourceToken(source, &token);
	if (value < 0) UnreadSignToken(source);
	return true;
}

directive_t dollardirectives[20] =
{
	{ "evalint", PC_DollarDirective_evalint },
	{ "evalfloat", PC_DollarDirective_evalfloat },
	{ NULL, NULL }
};

bool PC_ReadDollarDirective(source_t *source)
{
	token_t token;
	int i;

	//read the directive name
	if (!PC_ReadSourceToken(source, &token))
	{
		SourceError(source, "found $ without name");
		return false;
	}

	//directive name must be on the same line
	if (token.linescrossed > 0)
	{
		PC_UnreadSourceToken(source, &token);
		SourceError(source, "found $ at end of line");
		return false;
	}

	//if if is a name
	if (token.type == TT_NAME)
	{
		//find the precompiler directive
		for (i = 0; dollardirectives[i].name; i++)
		{
			if (!strcmp(dollardirectives[i].name, token.string))
			{
				return dollardirectives[i].func(source);
			}
		} 
	}

	PC_UnreadSourceToken(source, &token);
	SourceError(source, "unknown precompiler directive %s", token.string);
	return false;
}

bool PC_ReadToken(source_t *source, token_t *token)
{
	define_t *define;

	while(1)
	{
		if (!PC_ReadSourceToken(source, token)) return false;
		//check for precompiler directives
		if (token->type == TT_PUNCTUATION && *token->string == '#')
		{
#ifdef QUAKEC
			if (!BuiltinFunction(source))
#endif //QUAKC
			{
				//read the precompiler directive
				if (!PC_ReadDirective(source)) return false;
				continue;
			} //end if
		} //end if
		if (token->type == TT_PUNCTUATION && *token->string == '$')
		{
#ifdef QUAKEC
			if (!QuakeCMacro(source))
#endif //QUAKEC
			{
				//read the precompiler directive
				if (!PC_ReadDollarDirective(source)) return false;
				continue;
			} //end if
		} //end if
		// recursively concatenate strings that are behind each other still resolving defines
		if (token->type == TT_STRING)
		{
			token_t newtoken;
			if (PC_ReadToken(source, &newtoken))
			{
				if (newtoken.type == TT_STRING)
				{
					token->string[strlen(token->string)-1] = '\0';
					if (strlen(token->string) + strlen(newtoken.string+1) + 1 >= MAX_TOKEN)
					{
						SourceError(source, "string longer than MAX_TOKEN %d\n", MAX_TOKEN);
						return false;
					}
					strcat(token->string, newtoken.string+1);
				}
				else
				{
					PC_UnreadToken(source, &newtoken);
				}
			}
		} //end if
		//if skipping source because of conditional compilation
		if (source->skip) continue;
		//if the token is a name
		if (token->type == TT_NAME)
		{
			//check if the name is a define macro
#if DEFINEHASHING
			define = PC_FindHashedDefine(source->definehash, token->string);
#else
			define = PC_FindDefine(source->defines, token->string);
#endif //DEFINEHASHING
			//if it is a define macro
			if (define)
			{
				//expand the defined macro
				if (!PC_ExpandDefineIntoSource(source, token, define)) return false;
				continue;
			} 
		}

		//copy token for unreading
		std::memcpy(&source->token, token, sizeof(token_t));
		return true;
	}
}

bool PC_ExpectTokenString(source_t *source, char *string)
{
	token_t token;

	if (!PC_ReadToken(source, &token))
	{
		SourceError(source, "couldn't find expected %s", string);
		return false;
	}

	if (strcmp(token.string, string))
	{
		SourceError(source, "expected %s, found %s", string, token.string);
		return false;
	}

	return true;
}

bool PC_ExpectTokenType(source_t *source, int type, int subtype, token_t *token)
{
	char str[MAX_TOKEN];

	if (!PC_ReadToken(source, token))
	{
		SourceError(source, "couldn't read expected token");
		return false;
	}

	if (token->type != type)
	{
		strcpy(str, "");
		if (type == TT_STRING) strcpy(str, "string");
		if (type == TT_LITERAL) strcpy(str, "literal");
		if (type == TT_NUMBER) strcpy(str, "number");
		if (type == TT_NAME) strcpy(str, "name");
		if (type == TT_PUNCTUATION) strcpy(str, "punctuation");
		SourceError(source, "expected a %s, found %s", str, token->string);
		return false;
	}

	if (token->type == TT_NUMBER)
	{
		if ((token->subtype & subtype) != subtype)
		{
			if (subtype & TT_DECIMAL) strcpy(str, "decimal");
			if (subtype & TT_HEX) strcpy(str, "hex");
			if (subtype & TT_OCTAL) strcpy(str, "octal");
			if (subtype & TT_BINARY) strcpy(str, "binary");
			if (subtype & TT_LONG) strcat(str, " long");
			if (subtype & TT_UNSIGNED) strcat(str, " unsigned");
			if (subtype & TT_FLOAT) strcat(str, " float");
			if (subtype & TT_INTEGER) strcat(str, " integer");
			SourceError(source, "expected %s, found %s", str, token->string);
			return false;
		} 
	}
	else if (token->type == TT_PUNCTUATION)
	{
		if (token->subtype != subtype)
		{
			SourceError(source, "found %s", token->string);
			return false;
		} 
	}

	return true;
}

bool PC_ExpectAnyToken(source_t *source, token_t *token)
{
	if (!PC_ReadToken(source, token))
	{
		SourceError(source, "couldn't read expected token");
		return false;
	}

	return true;
}

bool PC_CheckTokenString(source_t *source, char *string)
{
	token_t tok;

	if (!PC_ReadToken(source, &tok))
        return false;

	if (!strcmp(tok.string, string))
        return true;

	PC_UnreadSourceToken(source, &tok);
	return false;
}

bool PC_CheckTokenType(source_t *source, int type, int subtype, token_t *token)
{
	token_t tok;

	if (!PC_ReadToken(source, &tok))
        return false;
	if (tok.type == type && (tok.subtype & subtype) == subtype)
	{
		std::memcpy(token, &tok, sizeof(token_t));
		return true;
	}

	PC_UnreadSourceToken(source, &tok);
	return false;
}

bool PC_SkipUntilString(source_t *source, char *string)
{
	token_t token;

	while (PC_ReadToken(source, &token))
	{
		if (!strcmp(token.string, string)) return true;
	}

	return false;
}

void PC_UnreadLastToken(source_t *source)
{
	PC_UnreadSourceToken(source, &source->token);
}

void PC_UnreadToken(source_t *source, token_t *token)
{
	PC_UnreadSourceToken(source, token);
}

void PC_SetIncludePath(source_t *source, char *path)
{
	strncpy(source->includepath, path, MAX_PATH);
	//add trailing path seperator
	if (source->includepath[strlen(source->includepath)-1] != '\\' &&
		source->includepath[strlen(source->includepath)-1] != '/')
	{
		strcat(source->includepath, "/");
	}
}

void PC_SetPunctuations(source_t *source, punctuation_t *p)
{
	source->punctuations = p;
}

source_t *LoadSourceFile(const char *filename)
{
	source_t *source;
	script_t *script;

	// PC_InitTokenHeap();

	script = LoadScriptFile(filename);
	if (!script) return NULL;

	script->next = NULL;

	source = (source_t *) std::malloc(sizeof(source_t)); // CHECK MEM LEAK
	std::memset(source, 0, sizeof(source_t));

	strncpy(source->filename, filename, MAX_PATH);
	source->scriptstack = script;
	source->tokens = NULL;
	source->defines = NULL;
	source->indentstack = NULL;
	source->skip = 0;

#if DEFINEHASHING
	source->definehash = (define_t**)std::malloc(DEFINEHASHSIZE * sizeof(define_t *)); // CHECK MEM LEAK
    std::memset(source->definehash, 0, DEFINEHASHSIZE * sizeof(define_t *));
#endif //DEFINEHASHING

	PC_AddGlobalDefinesToSource(source);
	return source;
} 

source_t *LoadSourceMemory(char *ptr, int length, char *name)
{
	source_t *source;
	script_t *script;

	// PC_InitTokenHeap();

	script = LoadScriptMemory(ptr, length, name);
	if (!script) return NULL;
	script->next = NULL;

	source = (source_t *) std::malloc(sizeof(source_t)); // CHECK MEM LEAK
	std::memset(source, 0, sizeof(source_t));

	strncpy(source->filename, name, MAX_PATH);
	source->scriptstack = script;
	source->tokens = NULL;
	source->defines = NULL;
	source->indentstack = NULL;
	source->skip = 0;

#if DEFINEHASHING
	source->definehash = (define_t**)std::malloc(DEFINEHASHSIZE * sizeof(define_t *));  // CHECK MEM LEAK
    std::memset(source->definehash, 0, DEFINEHASHSIZE * sizeof(define_t *));
#endif //DEFINEHASHING
	PC_AddGlobalDefinesToSource(source);
	return source;
} 

void FreeSource(source_t *source)
{
	script_t *script;
	token_t *token;
	define_t *define;
	indent_t *indent;
	int i;

	//free all the scripts
	while(source->scriptstack)
	{
		script = source->scriptstack;
		source->scriptstack = source->scriptstack->next;
		FreeScript(script);
	}

	//free all the tokens
	while(source->tokens)
	{
		token = source->tokens;
		source->tokens = source->tokens->next;
		PC_FreeToken(token);
	}

#if DEFINEHASHING
	for (i = 0; i < DEFINEHASHSIZE; i++)
	{
		while(source->definehash[i])
		{
			define = source->definehash[i];
			source->definehash[i] = source->definehash[i]->hashnext;
			PC_FreeDefine(define);
		}
	}

#else //DEFINEHASHING
	//free all defines
	while(source->defines)
	{
		define = source->defines;
		source->defines = source->defines->next;
		PC_FreeDefine(define);
	} //end for
#endif //DEFINEHASHING

	//free all indents
	while(source->indentstack)
	{
		indent = source->indentstack;
		source->indentstack = source->indentstack->next;
		std::free(indent);
	}

#if DEFINEHASHING
	//
	if (source->definehash) std::free(source->definehash);
#endif //DEFINEHASHING

	//free the source itself
	std::free(source);
}


#define MAX_SOURCEFILES		64

source_t *sourceFiles[MAX_SOURCEFILES];

int PC_LoadSourceHandle(const char *filename)
{
	source_t *source;
	int i;

	for (i = 1; i < MAX_SOURCEFILES; i++)
	{
		if (!sourceFiles[i]) break;
	}

	if (i >= MAX_SOURCEFILES) return 0;

	source = LoadSourceFile(filename);

	if (!source) return 0;

	sourceFiles[i] = source;
	return i;
}

bool PC_FreeSourceHandle(int handle)
{
	if (handle < 1 || handle >= MAX_SOURCEFILES)
		return false;
	if (!sourceFiles[handle])
		return false;

	FreeSource(sourceFiles[handle]);
	sourceFiles[handle] = NULL;
	return true;
}

bool PC_ReadTokenHandle(int handle, pc_token_t *pc_token)
{
	token_t token;
	bool ret;

	if (handle < 1 || handle >= MAX_SOURCEFILES)
		return false;

	if (!sourceFiles[handle])
		return false;

	ret = PC_ReadToken(sourceFiles[handle], &token);
	strcpy(pc_token->string, token.string);
	pc_token->type = token.type;
	pc_token->subtype = token.subtype;
	pc_token->intvalue = token.intvalue;
	pc_token->floatvalue = token.floatvalue;
	if (pc_token->type == TT_STRING)
		StripDoubleQuotes(pc_token->string);
	return ret;
}

bool PC_ReadLineHandle(int handle, pc_token_s *pc_token)
{
	token_s token;
	bool ret;

	if (handle < 1 || handle >= MAX_SOURCEFILES)
		return false;

	if (!sourceFiles[handle])
		return false;

	ret = PC_ReadLine(sourceFiles[handle], &token, true);
	strcpy(pc_token->string, token.string);
	pc_token->type = token.type;
	pc_token->subtype = token.subtype;
	pc_token->intvalue = token.intvalue;
	pc_token->floatvalue = token.floatvalue;
	if ( pc_token->type == 1 )
		StripDoubleQuotes(pc_token->string);

	return ret;
}

bool PC_SourceFileAndLine(int handle, char *filename, int *line)
{
	if (handle < 1 || handle >= MAX_SOURCEFILES)
		return false;

	if (!sourceFiles[handle])
		return false;

	strcpy(filename, sourceFiles[handle]->filename);
	if (sourceFiles[handle]->scriptstack)
		*line = sourceFiles[handle]->scriptstack->line;
	else
		*line = 0;
	return true;
}

void PC_UnreadLastTokenHandle(int handle)
{
	if ( handle >= 1 && handle < MAX_SOURCEFILES )
	{
		if ( sourceFiles[handle] )
			PC_UnreadLastToken(sourceFiles[handle]);
	}
}

char Buffer[0x1000];

void PC_SourceError(int handle, char *format, ...)
{
  int line;
  char filename[128]; 
  va_list ArgList;

  va_start(ArgList, format);
  vsnprintf(Buffer, 0x1000u, format, ArgList);
  filename[0] = 0;
  line = 0;
  PC_SourceFileAndLine(handle, filename, &line);
  
  //Com_PrintError(13, "Menu load error: %s, line %d: %s\n", v3, v2, Buffer);
  printf("Menu load error: %s, line %d: %s\n", filename, line, Buffer);
}

} //namespace xsk::menu::iw5
