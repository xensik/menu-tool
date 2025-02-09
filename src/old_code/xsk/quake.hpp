// Copyright 2022 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#pragma once

namespace xsk::menu::iw5
{

// lexer
#define MAX_TOKEN					1024
//script flags
#define SCFL_NOERRORS				0x0001
#define SCFL_NOWARNINGS				0x0002
#define SCFL_NOSTRINGWHITESPACES	0x0004
#define SCFL_NOSTRINGESCAPECHARS	0x0008
#define SCFL_PRIMITIVE				0x0010
#define SCFL_NOBINARYNUMBERS		0x0020
#define SCFL_NONUMBERVALUES		    0x0040
//token types
#define TT_STRING				    1			// string
#define TT_LITERAL					2			// literal
#define TT_NUMBER					3			// number
#define TT_NAME						4			// name
#define TT_PUNCTUATION				5			// punctuation
//number sub type
#define TT_DECIMAL					0x0008	// decimal number
#define TT_HEX						0x0100	// hexadecimal number
#define TT_OCTAL					0x0200	// octal number
#define TT_BINARY					0x0400	// binary number
#define TT_FLOAT					0x0800	// floating point number
#define TT_INTEGER					0x1000	// integer number
#define TT_LONG						0x2000	// long number
#define TT_UNSIGNED					0x4000	// unsigned number
//punctuation sub type
#define P_RSHIFT_ASSIGN				1
#define P_LSHIFT_ASSIGN				2
#define P_PARMS						3
#define P_PRECOMPMERGE				4
#define P_LOGIC_AND					5
#define P_LOGIC_OR					6
#define P_LOGIC_GEQ					7
#define P_LOGIC_LEQ					8
#define P_LOGIC_EQ					9
#define P_LOGIC_UNEQ				10
#define P_MUL_ASSIGN				11
#define P_DIV_ASSIGN				12
#define P_MOD_ASSIGN				13
#define P_ADD_ASSIGN				14
#define P_SUB_ASSIGN				15
#define P_INC						16
#define P_DEC						17
#define P_BIN_AND_ASSIGN			18
#define P_BIN_OR_ASSIGN				19
#define P_BIN_XOR_ASSIGN			20
#define P_RSHIFT					21
#define P_LSHIFT					22
#define P_POINTERREF				23
#define P_CPP1						24
#define P_CPP2						25
#define P_MUL						26
#define P_DIV						27
#define P_MOD						28
#define P_ADD						29
#define P_SUB						30
#define P_ASSIGN					31
#define P_BIN_AND					32
#define P_BIN_OR					33
#define P_BIN_XOR					34
#define P_BIN_NOT					35
#define P_LOGIC_NOT					36
#define P_LOGIC_GREATER				37
#define P_LOGIC_LESS				38
#define P_REF						39
#define P_COMMA						40
#define P_SEMICOLON					41
#define P_COLON						42
#define P_QUESTIONMARK				43
#define P_PARENTHESESOPEN			44
#define P_PARENTHESESCLOSE			45
#define P_BRACEOPEN					46
#define P_BRACECLOSE				47
#define P_SQBRACKETOPEN				48
#define P_SQBRACKETCLOSE			49
#define P_BACKSLASH					50
#define P_PRECOMP					51
#define P_DOLLAR					52

//punctuation
typedef struct punctuation_s
{
	char *p;						//punctuation character(s)
	int n;							//punctuation indication
	struct punctuation_s *next;		//next punctuation
} punctuation_t;

//token
typedef struct token_s
{
	char string[MAX_TOKEN];			//available token
	int type;						//last read token type
	int subtype;					//last read token sub type
	unsigned long int intvalue;	    //integer value
	long double floatvalue;			//floating point value
	char *whitespace_p;				//start of white space before token
	char *endwhitespace_p;			//start of white space before token
	int line;						//line the token was on
	int linescrossed;				//lines crossed in white space
	struct token_s *next;			//next token in chain
} token_t;

//script file
typedef struct script_s
{
	char filename[1024];			//file name of the script
	char *buffer;					//buffer containing the script
	char *script_p;					//current pointer in the script
	char *end_p;					//pointer to the end of the script
	char *lastscript_p;				//script pointer before reading token
	char *whitespace_p;				//begin of the white space
	char *endwhitespace_p;			//end of the white space
	int length;						//length of the script in bytes
	int line;						//current line in script
	int lastline;					//line before reading token
	int tokenavailable;				//set by UnreadLastToken
	int flags;						//several script flags
	punctuation_t *punctuations;	//the punctuations used in the script
	punctuation_t **punctuationtable;
	token_t token;					//available token
	struct script_s *next;			//next script in a chain
} script_t;

//read a token from the script
bool PS_ReadToken(script_t *script, token_t *token);
//expect a certain token
bool PS_ExpectTokenString(script_t *script, char *string);
//expect a certain token type
bool PS_ExpectTokenType(script_t *script, int type, int subtype, token_t *token);
//expect a token
bool PS_ExpectAnyToken(script_t *script, token_t *token);
//returns true when the token is available
bool PS_CheckTokenString(script_t *script, char *string);
//returns true an reads the token when a token with the given type is available
bool PS_CheckTokenType(script_t *script, int type, int subtype, token_t *token);
//skip tokens until the given token string is read
bool PS_SkipUntilString(script_t *script, char *string);
//unread the last token read from the script
void PS_UnreadLastToken(script_t *script);
//unread the given token
void PS_UnreadToken(script_t *script, token_t *token);
//returns the next character of the read white space, returns NULL if none
char PS_NextWhiteSpaceChar(script_t *script);
//remove any leading and trailing double quotes from the token
void StripDoubleQuotes(char *string);
//remove any leading and trailing single quotes from the token
void StripSingleQuotes(char *string);
//read a possible signed integer
signed long int ReadSignedInt(script_t *script);
//read a possible signed floating point number
long double ReadSignedFloat(script_t *script);
//set an array with punctuations, NULL restores default C/C++ set
void SetScriptPunctuations(script_t *script, punctuation_t *p);
//set script flags
void SetScriptFlags(script_t *script, int flags);
//get script flags
int GetScriptFlags(script_t *script);
//reset a script
void ResetScript(script_t *script);
//returns true if at the end of the script
bool EndOfScript(script_t *script);
//returns a pointer to the punctuation with the given number
char *PunctuationFromNum(script_t *script, int num);
//load a script from the given file at the given offset with the given length
script_t *LoadScriptFile(const char *filename);
//load a script from the given memory with the given length
script_t *LoadScriptMemory(char *ptr, int length, char *name);
//free a script
void FreeScript(script_t *script);
//set the base folder to load files from
void PS_SetBaseFolder(char *path);
//print a script error with filename and line number
void ScriptError(script_t *script, char *str, ...);
//print a script warning with filename and line number
void ScriptWarning(script_t *script, char *str, ...);

//////////////////////////////////////////////////////////////////////////////
//                                preprocessor
//////////////////////////////////////////////////////////////////////////////
#define MAX_PATH			    64

#define DEFINE_FIXED			0x0001

#define BUILTIN_LINE			1
#define BUILTIN_FILE			2
#define BUILTIN_DATE			3
#define BUILTIN_TIME			4
#define BUILTIN_STDC			5

#define INDENT_IF				0x0001
#define INDENT_ELSE				0x0002
#define INDENT_ELIF				0x0004
#define INDENT_IFDEF			0x0008
#define INDENT_IFNDEF			0x0010

typedef struct define_s
{
	char *name;                // define name
	int flags;                 // define flags
	int builtin;               // > 0 if builtin define
	int numparms;              // number of define parameters
	token_s *parms;            // define parameters
	token_s *tokens;           // macro tokens (possibly containing parm tokens)
	struct define_s *next;     // next defined macro in a list
	struct define_s *hashnext; // next define in the hash chain
} define_t;

typedef struct indent_s
{
	int type;              // indent type
	int skip;              // true if skipping current indent
	script_t *script;      // script the indent was in
	struct indent_s *next; // next indent on the indent stack
} indent_t;

typedef struct source_s
{
	char filename[MAX_PATH];     // file name of the script
	char includepath[MAX_PATH];  // path to include files
	punctuation_t *punctuations; // punctuations to use
	script_t *scriptstack;       // stack with scripts of the source
	token_t *tokens;             // tokens to read first
	define_t *defines;           // list with macro definitions
	define_t **definehash;       // hash chain with defines
	indent_t *indentstack;       // stack with indents
	int skip;                    // > 0 if skipping conditional code
	token_t token;               // last read token
} source_t;

//read a token from the source
bool PC_ReadToken(source_t *source, token_t *token);
//expect a certain token
bool PC_ExpectTokenString(source_t *source, char *string);
//expect a certain token type
bool PC_ExpectTokenType(source_t *source, int type, int subtype, token_t *token);
//expect a token
bool PC_ExpectAnyToken(source_t *source, token_t *token);
//returns true when the token is available
bool PC_CheckTokenString(source_t *source, char *string);
//returns true an reads the token when a token with the given type is available
bool PC_CheckTokenType(source_t *source, int type, int subtype, token_t *token);
//skip tokens until the given token string is read
bool PC_SkipUntilString(source_t *source, char *string);
//unread the last token read from the script
void PC_UnreadLastToken(source_t *source);
//unread the given token
void PC_UnreadToken(source_t *source, token_t *token);
//read a token only if on the same line, lines are concatenated with a slash
bool PC_ReadLine(source_t *source, token_t *token);
//returns true if there was a white space in front of the token
bool PC_WhiteSpaceBeforeToken(token_t *token);
//add a define to the source
bool PC_AddDefine(source_t *source, char *string);
//add a globals define that will be added to all opened sources
bool PC_AddGlobalDefine(char *string);
//remove the given global define
bool PC_RemoveGlobalDefine(char *name);
//remove all globals defines
void PC_RemoveAllGlobalDefines(void);
//add builtin defines
void PC_AddBuiltinDefines(source_t *source);
//set the source include path
void PC_SetIncludePath(source_t *source, char *path);
//set the punction set
void PC_SetPunctuations(source_t *source, punctuation_t *p);
//set the base folder to load files from
void PC_SetBaseFolder(char *path);
//load a source file
source_t *LoadSourceFile(const char *filename);
//load a source from memory
source_t *LoadSourceMemory(char *ptr, int length, char *name);
//free the given source
void FreeSource(source_t *source);
//print a source error
void SourceError(source_t *source, char *str, ...);
//print a source warning
void SourceWarning(source_t *source, char *str, ...);

#define MAX_TOKENLENGTH		1024

typedef struct pc_token_s
{
	int type;
	int subtype;
	int intvalue;
	float floatvalue;
	char string[MAX_TOKENLENGTH];
} pc_token_t;

int PC_LoadSourceHandle(const char *filename, const char **builtinDefines);
bool PC_FreeSourceHandle(int handle);
bool PC_ReadTokenHandle(int handle, pc_token_t *pc_token);
bool PC_ReadLineHandle(int handle, pc_token_s *pc_token);
bool PC_SourceFileAndLine(int handle, char *filename, int *line);
void PC_UnreadLastTokenHandle(int handle);
void PC_SourceError(int handle, char *format, ...);

} // namespace xsk::menu::iw5
