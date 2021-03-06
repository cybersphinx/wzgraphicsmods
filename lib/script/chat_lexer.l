/*
	This file is part of Warzone 2100.
	Copyright (C) 2006-2008  Roman
	Copyright (C) 2006-2009  Warzone Resurrection Project

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
 *  Lexical analyser for multiplayer chat messages.
 */

#include "lib/framework/frame.h"

// FIXME: #include from src/
#include "src/scriptfuncs.h"

#include "script.h"
#include "chat_processing.h"

/* Get the Yacc definitions */
#include "chat_parser.tab.h"

/* Maximum length for any TEXT value */
#ifndef YYLMAX
#define YYLMAX	255
#endif

#if defined(YY_FLEX_SUBMINOR_VERSION) && YY_FLEX_SUBMINOR_VERSION == 33
extern int chat_get_lineno(void);
extern FILE *chat_get_in(void);
extern FILE *chat_get_out(void);
extern int chat_get_leng(void);
extern char *chat_get_text(void);
extern void chat_set_lineno(int line_number);
extern void chat_set_in(FILE* in_str);
extern void chat_set_out(FILE* out_str);
extern int chat_get_debug(void);
extern void chat_set_debug(int bdebug);
extern int chat_lex_destroy(void);
#endif

/* Pointer to the input buffer */
static const char *pInputBuffer = NULL;
static const char *pEndBuffer = NULL;

static SDWORD playerIndex;

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
      if (pInputBuffer != pEndBuffer) { \
        buf[0] = *(pInputBuffer++); result = 1; \
      } else { \
        buf[0] = EOF; result = YY_NULL; \
      }

#undef chat_getc
#define chat_getc() (pInputBuffer != pEndBuffer ? *(pInputBuffer++) : EOF)

%}

%option yylineno
%option prefix="chat_"
%option nounput
%option case-insensitive

%%

	/* Match integer numbers */
-?[0-9]+					{ chat_lval.ival = atol(chat_text); return R_INTEGER; }

	/* Keywords - Terminals */
"'s"						{ return R_POSSESSION; }
"?"+						{ return _T_QM; }
"!"+						{ return _T_EM; }
"."+						{ return _T_FULLSTOP; }
":"							{ return _T_COLON; };
";"							{ return _T_SEMICOLON; };
","							{ return _T_COMMA; };
"a"							{ return _T_A; }
"affirmative"				{ return _T_AFFIRMATIVE; }
"after"						{ return _T_AFTER; }
"ally"						{ return _T_ALLY; }
"am"						{ return _T_AM; }
"an"						{ return _T_AN; }
"and"						{ return _T_AND; }
"any"						{ return _T_ANY; }
"attack"					{ return _T_ATTACK; }
"attacking"					{ return _T_ATTACKING; }
"army"						{ return _T_ARMY; }
"beacon"					{ return _T_BEACON; }
"building"					{ return _T_BUILDING; }
"can't"						{ return _T_CANT; }
"center"					{ return _T_CENTER; }
"dead"						{ return _T_DEAD; }
"derrick"					{ return _T_DERRICK; }
"do"						{ return _T_DO; }
"drop"						{ return _T_DROP; }
"fine"						{ return _T_FINE; }
"for"						{ return _T_FOR; }
"force"						{ return _T_FORCE; }
"get"						{ return _T_GET; }
"getting"					{ return _T_GETTING; }
"go"						{ return _T_GO; }
"going"						{ return _T_GOING; }
"gonna"						{ return _T_GONNA; }
"got"						{ return _T_GOT; }
"great"						{ return _T_GREAT; }
"have"						{ return _T_HAVE; }
"has"						{ return _T_HAS; }
"help"						{ return _T_HELP; }
"hold"						{ return _T_HOLD; }
"i"							{ return _T_I; }
"i'm"						{ return _T_IM; }
"is"						{ return _T_IS; }
"lassat"					{ return _T_LASSAT; }
"let's"						{ return _T_LETS; }
"me"						{ return _T_ME; }
"more"						{ return _T_MORE; }
"need"						{ return _T_NEED; }
"no"						{ return _T_NO; }
"now"						{ return _T_NOW; }
"of course"					{ return _T_OFCOURSE; }
"ok"+						{ return _T_OK; }
"on"						{ return _T_ON; }
"place"						{ return _T_PLACE; }
"possession"				{ return _T_POSSESSION; }
"power"						{ return _T_POWER; }
"pumping"					{ return _T_PUMPING; }
"put"						{ return _T_PUT; }
"ready"						{ return _T_READY; }
"require"					{ return _T_REQUIRE; }
"roger"						{ return _T_ROGER; }
"rush"						{ return _T_RUSH; }
"sec"						{ return _T_SEC; }
"see"						{ return _T_SEE; }
"some"						{ return _T_SOME; }
"status"					{ return _T_STATUS; }
"stop"						{ return _T_STOP; }
"sure"						{ return _T_SURE; }
"thank you"					{ return _T_THANK_YOU; }
"thanks"					{ return _T_THANKS; }
"the"						{ return _T_THE; }
"u"							{ return _T_U; }
"units"						{ return _T_UNITS; }
"vtols"						{ return _T_VTOLS; }
"wait"						{ return _T_WAIT; }
"where"						{ return _T_WHERE; }
"yea"						{ return _T_YES; }
"yeah"						{ return _T_YES; }
"yep"						{ return _T_YES; }
"yes"						{ return _T_YES; }
"you"						{ return _T_YOU; }

	/* <<EOF>>					{ return _T_EOF; } */

[0-9_a-zA-Z_]*			{
								playerIndex = getPlayerFromString(chat_text);

								if(playerIndex >= 0)
								{
									//console( "matched 'player'");
									chat_lval.ival = playerIndex;

									return R_PLAYER;
								}

							}

	/* [^ \t\n\<\>\[\]\(\)]+		{ return T_WORD; } */
	/* [^ \t\n]+					{ console( "matched 'T_WORD'"); return T_WORD; } */


	/* Skip white space */
[ \t\n\x0d\x0a]						;


	/* Match anything that's been missed and pass it as a char */
	/* .							{console( "matched 'anything else '%s'", chat_text[0]); return chat_text[0];} */

%%

/* Set the current input buffer for the lexer */
void chatSetInputBuffer(char *pBuffer, UDWORD size)
{
	pInputBuffer = pBuffer;
	pEndBuffer = pBuffer + size;

	/* Reset the lexer in case it's been used before */
	chat__flush_buffer(YY_CURRENT_BUFFER);
}

void chatGetErrorData(int *pLine, char **ppText)
{
	*pLine = chat_lineno;
	*ppText = chat_text;
}

int chat_wrap(void)
{
	return 1;
}


/* Older GNU Flex versions don't define yylex_destroy()
 * (and neither define a subminor version)
 */
#if !defined(YY_FLEX_SUBMINOR_VERSION) || (YY_FLEX_SUBMINOR_VERSION < 9)
int chat_lex_destroy(void)
{
	/* For non-reentrant C scanner only. */
	yy_delete_buffer(YY_CURRENT_BUFFER);
	yy_init = 1;
	return 0;
}
#endif
