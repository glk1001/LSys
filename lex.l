%{
/* lex.l - lexical analyzer for L-systems.
 *
 * Copyright (C) 1990, Jonathan P. Leech
 *
 * This software may be freely copied, modified, and redistributed,
 * provided that this copyright notice is preserved on all copies.
 *
 * There is no warranty or other guarantee of fitness for this software,
 * it is provided solely "as is". Bug reports or fixes may be sent
 * to the author, who may or may not act on them as he desires.
 *
 * You may not include this software in a program or other software product
 * without supplying the source, or without informing the end-user that the
 * source is available for no extra charge.
 *
 * If you modify this software, you should include a notice giving the
 * name of the person performing the modification, the date of modification,
 * and the reason for such modification.
 *
 * $Log:	lex.l,v $
 * Revision 1.6  94/08/26  16:37:12  leech
 * Only declare our own prototypes if not ANSI C.
 *
 * Revision 1.5  1991/10/10  19:57:42  leech
 * Added set_parser_input function to allow reading from multiple files.
 *
 * Revision 1.4  91/03/20  21:34:36  leech
 * Fix malloc declaration problem with flex.
 *
 * Revision 1.3  91/03/20  10:33:56  leech
 * Support for flex and gcc.
 *
 * Revision 1.2  90/10/12  18:48:20  leech
 * First public release.
 *
 */

#include "token.h" /* Make sure this the first include to avoid name clashes */
#include <string>
#include <cstdio>
#include "lexdefs.h"
#include "debug.h"

int lexcol= 1, lexline= 1;

#ifdef __cplusplus
extern "C" int yywrap();
#else
extern int yywrap());
#endif
char *lex_token();
void lex_pushstate(int type);
void lex_popstate();
/**void lex_in_expr(int flag); **/

/* user-defined functions */
static void save_name(const char *s);
static int spaceadd(int curlen, const char *s);
static int countnewlines(const char *s, int *width);
static void switchstate(int state, const char *msg);

%}
U	[A-Z]
L	[a-z_]
A	[A-Za-z_]
D	[0-9]
E	[Ee][-+]?{D}+

%START MODULE EXPRESSION
%%
"/*"(\*[^/]|[^*]+)*"*/"   {
                lexcol += strlen(yytext);
                lexline += countnewlines(yytext, &lexcol);
              }
#ignore			  { lexcol += 7; return LSys::LSYS_IGNORE; }
#include		  { lexcol += 8; return LSys::LSYS_INCLUDE; }
#define			  { lexcol += 7; return LSys::LSYS_DEFINE; }
START		      { lexcol += 5; return LSys::LSYS_START; }
->			      { lexcol += 2; return LSys::LSYS_YIELDS; }
<MODULE>{A}{D}*|Fl|Fr	  {
                lexcol += strlen(yytext);
                save_name(yytext);
                PDebug(PD_LEXER, fprintf(stderr, "lex: module name -> %s\n", yytext));
                return LSys::LSYS_NAME;
              }
<MODULE>@{A}*|~{A}* {
                lexcol += strlen(yytext);
                save_name(yytext);
                PDebug(PD_LEXER, fprintf(stderr, "lex: module name -> %s\n", yytext));
                return LSys::LSYS_NAME;
              }
<MODULE>{D}   { lexcol++;
                save_name(yytext);
                PDebug(PD_LEXER, fprintf(stderr, "lex: module name -> %s\n", yytext));
                return LSys::LSYS_NAME;
              }
<MODULE>[+^/&|%~!-]  {
                lexcol++;
                save_name(yytext);
                PDebug(PD_LEXER, fprintf(stderr, "lex: module name -> %s\n", yytext));
                return LSys::LSYS_NAME;
              }
<EXPRESSION>{A}({A}|{D})*   {
                lexcol += strlen(yytext);
                save_name(yytext);
                PDebug(PD_LEXER, fprintf(stderr, "lex: expression name -> %s\n", yytext));
                return LSys::LSYS_NAME;
              }
<EXPRESSION>{D}+  {
                lexcol += strlen(yytext);
                save_name(yytext);
                PDebug(PD_LEXER, fprintf(stderr, "lex: integer -> %s\n", yytext));
                return LSys::LSYS_INTEGER;
              }
<EXPRESSION>{D}+"."{D}*({E})? |
{D}*"."{D}+({E})? |
{D}+{E}E		  {
                lexcol += strlen(yytext);
                save_name(yytext);
                PDebug(PD_LEXER, fprintf(stderr, "lex: real -> %s\n", yytext));
                return LSys::LSYS_REAL;
              }
<EXPRESSION>\<=	  { lexcol += 2; return LSys::LSYS_LE; }
<EXPRESSION>==	  { lexcol += 2; return LSys::LSYS_EQ; }
<EXPRESSION>=		  { lexcol++; return LSys::LSYS_EQ; }
<EXPRESSION>>=	  { lexcol += 2; return LSys::LSYS_GE; }
<EXPRESSION>!=	  { lexcol += 2; return LSys::LSYS_NE; }
<EXPRESSION>&&	  { lexcol += 2; return LSys::LSYS_AND; }
<EXPRESSION>\|\|  { lexcol += 2; return LSys::LSYS_OR; }
<EXPRESSION>[+^/&|~%!-]  { lexcol++; return yytext[0]; }
[*:(),<>]		             { lexcol++; return yytext[0]; }
[[\]{}\\$\.']	{
                lexcol++;
                save_name(yytext);
                PDebug(PD_LEXER, fprintf(stderr, "lex: name -> %s\n", yytext));
                return LSys::LSYS_NAME;
              }
[ \t]+			  { lexcol= spaceadd(lexcol, yytext); }
\\\n			    { lexcol= 1; lexline++; }
\n			      { lexcol= 1; lexline++; return '\n'; }
.			        { lexcol++; return LSys::LSYS_ERROR; }
%%


/* Stack of start states adjusted by the parser in lsys.y */
/* lexstate[lexstack] is current state (lexstack < 0 -> start state) */
#define STACKSIZE 10
static int lexstate[STACKSIZE]= { LEX_START };
static int lexstack= 0;


/* Change the start state to that indicated by 'state' */
static void switchstate(int state, const char *msg)
{
  switch (state) {

    case LEX_EXPRESSION:
	    BEGIN EXPRESSION;
	    PDebug(PD_LEXER, fprintf(stderr, "%s: BEGIN EXPRESSION\n", msg));
	    break;

    case LEX_MODULE:
	    BEGIN MODULE;
	    PDebug(PD_LEXER, fprintf(stderr, "%s: BEGIN MODULE\n", msg));
	    break;

    case LEX_START:
    default:
	    BEGIN 0;
	    PDebug(PD_LEXER, fprintf(stderr, "%s: BEGIN START\n", msg));
	    break;
    }
}


static int spaceadd(int curlen, const char *s)
{
  while (*s != '\0') {
    if (*s++ == '\t')
	    curlen= (curlen + 8) & ~7;
    else
	    curlen++;
  }
  return curlen;
}


/* Count number of newlines in s; keep track of the current column in *col.
 */
static int countnewlines(const char *s, int *col)
{
  int nl= 0;

  while (*s != '\0') {
    if (*s == '\n') {
	    nl++;
	    *col= 1;
    } else if (*s == '\t')
      *col= (*col + 8) & ~7;
    else
      (*col)++;
    s++;
  }

  return nl;
}


static char *lex_token_ptr= NULL;
static int lex_token_length= 0;

/* Save the token string pointed to by s for access by the parser */
static void save_name(const char *s)
{
  int len= strlen(s);

  if (len >= lex_token_length) {
    /* Reallocate the buffer */
    if (lex_token_ptr != NULL)
	    free(lex_token_ptr);
    if ((lex_token_ptr= (char*)malloc(len+1)) == NULL) {
	    fprintf(stderr, "FATAL: can't allocate %d bytes to buffer token in lexer!\n", len+1);
	    exit(1);
    }
    lex_token_length= len+1;
  }
  strcpy(lex_token_ptr, s);
}


/* Set the input stream for the parser & lexical analyzer to the
   specified file (if NULL, use stdin, which is already the default
   lex behavior).
 */
void set_parser_input(const char *ifile)
{
  #ifdef pink
    fprintf(stderr, "Setting yyin, yyout to stdin, stdout\n");
    // This is to get around a broken stdio.h which defines stdin and
    //	stdout such that we cannot initialize globals with them.
    yyin= stdin;
    yyout= stdout;
  #endif /*pink*/
  if (ifile != NULL) {
    fprintf(stderr, "Setting yyin to %s\n", ifile);
    if ((yyin= fopen(ifile, "r")) == NULL) {
	    fprintf(stderr, "Can't open input file %s\n", ifile);
	    exit(1);
    }
  }
}


/* Return the token string (called by parser) */
char *lex_token(void)
{
  return lex_token_ptr;
}


/* Push a start state onto the stack and change lex's start state to match */
void lex_pushstate(int type)
{
  if (lexstack >= STACKSIZE-1) {
    fprintf(stderr, "lex_pushstate: stack of start states full!\n");
    exit(1);
  }

  lexstate[++lexstack]= type;
  switchstate(type, "pushstate");
}


/* Pop a start state from the stack and change lex's start state to match */
void lex_popstate(void)
{
  int type;

  if (lexstack <= 0) {
    fprintf(stderr, "lex_popstate: stack of start states empty!\n");
    exit(1);
  }

  type= lexstate[--lexstack];
  switchstate(type, "popstate");
}


/* Dummy yywrap routine */
int yywrap(void)
{
  return 1;
}

