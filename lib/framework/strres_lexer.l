/*
	This file is part of Warzone 2100.
	Copyright (C) 1999-2004  Eidos Interactive
	Copyright (C) 2005-2009  Warzone Resurrection Project

	Warzone 2100 is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	Warzone 2100 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Warzone 2100; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/
%{
/** @file
 *
 *  Lexical analyser resource files
 */

#include "lib/framework/frame.h"

/* Allow frame header files to be singly included */
#define FRAME_LIB_INCLUDE

#include "lib/framework/strres.h"
#include "lib/framework/strresly.h"

#include <physfs.h>

/* Get the Yacc definitions */
#include "strres_parser.tab.h"

#include "lib/framework/lexer_input.h"

#ifndef yyextra
# define yyextra yyget_extra()
#endif

/* Older GNU Flex versions don't define yyget_extra(), yyset_extra(),
 * yyget_text() and yyget_lineno().
 * (and neither define a subminor version)
 */
#if !defined(YY_FLEX_SUBMINOR_VERSION) || (YY_FLEX_SUBMINOR_VERSION < 9)
# define yyget_extra  strres_get_extra
# define yyset_extra  strres_set_extra
# define yyget_lineno strres_get_lineno
# define yyget_text   strres_get_text
extern void yyset_extra(YY_EXTRA_TYPE user_defined);
extern YY_EXTRA_TYPE yyget_extra(void);
extern int yyget_lineno(void);
int yyget_lineno()
{
	return yylineno;
}

extern char* yyget_text(void);
char* yyget_text()
{
	return yytext;
}
#elif defined(YY_FLEX_SUBMINOR_VERSION) && YY_FLEX_SUBMINOR_VERSION == 33
extern YY_EXTRA_TYPE yyget_extra(void);
extern int strres_get_lineno(void);
extern FILE *strres_get_in(void);
extern FILE *strres_get_out(void);
extern int strres_get_leng(void);
extern char *strres_get_text(void);
extern void strres_set_lineno(int line_number);
extern void strres_set_in(FILE* in_str);
extern void strres_set_out(FILE* out_str);
extern int strres_get_debug(void);
extern void strres_set_debug(int bdebug);
#endif

%}

%option yylineno noyywrap nounput
%option prefix="strres_"

%x COMMENT
%x QUOTE
%x SLCOMMENT

%%

	/* Match text values */
[a-zA-Z][-0-9_a-zA-Z]*		{
								strres_lval.sval = strdup(yytext);
								return TEXT_T;
							}

	/* Match quoted text */
\"							{ BEGIN QUOTE; }
<QUOTE>\"					{ BEGIN 0; }
<QUOTE>\n					{ strres_error("Unexpected end of line in string"); }
<QUOTE>[^\"\n]*				{
								strres_lval.sval = strdup(yytext);
								return QTEXT_T;
							}

	/* Skip white space */
[ \t\n\x0d\x0a]						;

	/* Strip comments */
"/*"						{ BEGIN COMMENT; }
<COMMENT>"*/" |
<COMMENT>"*/"\n				{ BEGIN 0; }
<COMMENT>. |
<COMMENT>\n					;

	/* Strip single line comments */
"//"						{ BEGIN SLCOMMENT; }
<SLCOMMENT>\n				{ BEGIN 0; }
<SLCOMMENT>[^\n]*			;

	/* Match anything that's been missed and pass it as a char */
.							return yytext[0];

%%

static YY_EXTRA_TYPE pBuffer = NULL;

void yyset_extra(YY_EXTRA_TYPE user_defined)
{
	pBuffer = user_defined;
}

YY_EXTRA_TYPE yyget_extra()
{
	return pBuffer;
}

/* Older GNU Flex versions don't define yylex_destroy()
 * (and neither define a subminor version)
 */
#if !defined(YY_FLEX_SUBMINOR_VERSION) || (YY_FLEX_SUBMINOR_VERSION < 9)
int strres_lex_destroy(void)
{
	/* For non-reentrant C scanner only. */
	yy_delete_buffer(YY_CURRENT_BUFFER);
	yy_init = 1;
	return 0;
}
#endif
