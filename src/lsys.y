%{
/* lsys.y - parser for L-systems.
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
 * $Log:	lsys.y,v $
 * Revision 1.3  91/03/20  10:33:14  leech
 * Support for G++.
 *
 * Revision 1.2  90/10/12  18:48:23  leech
 * First public release.
 *
 */

#undef YYSTYPE /* stop compilation errors */

#include <cstdlib>
#include <ctype.h>
#include <malloc.h>
#include "debug.h"
#include "Parser.h"

using LSys::LSysModel;
using LSys::Production;
using LSys::Value;
using LSys::Module;
using LSys::Expression;
using LSys::Predecessor;
using LSys::Successor;
using LSys::Name;

/* default version in liby:yyerror.o as extern "C" void yyerror(char *); */
void yyerror(char *);

/* lexical scanner (in lex.yy.o when automatically produced by lex) */
#include "lexdefs.h"
extern int yylex();

extern void lex_pushstate(int);
extern void lex_popstate();
extern int lexcol;
extern int lexline;

/* Return a pointer to the last string token parsed; in lexer */
extern char *lex_token();


// Dictionary & rule information needed/generated by the parser
LSysModel* parseInfo; // GLK

void set_parser_globals(LSysModel* m)
{
  parseInfo = m;
};


// A separate list of CF productions is maintained until all
//  productions have been read.
static LSys::List<Production> Context_free_rules;

// Should expressions be evaluated when read in?
static bool BindExpression = false;

%}


%start lsystem

%union {
  int name;                         /* module or variable name id */
  float probability;                /* production probability */
  Value* value;	                    /* numeric value */
  Module* module;
  Expression* expression;
  LSys::List<Module>* moduleList;	  /* list of modules as in a production successor */
  LSys::List<Expression>* expressionList;	/* list of expressions within a module */
  Predecessor* predecessor;	        /* a production predecessor */
  Successor* successor;	            /* a production successor */
  LSys::List<Successor>* successors;/* list of production successors with probabilities */
}

%type <moduleList> modules right_list
%type <module> module
%type <expressionList> arguments exprlist
%type <expression> expression conditional optional_conditional
%type <value> value
%type <predecessor>	predecessor center_list
%type <successor>	successor
%type <successors> successors optional_successors
%type <probability>	probability
%type <name> name

%token ERROR VALUE INTEGER REAL FUNCTION
%token IGNORE INCLUDE DEFINE YIELDS
%token <name> NAME START
%left OR
%left AND
%left '|'
%left '&'
%left EQ NE
%left '<' LE GE '>'
%left '+' '-'
%left '*' '/' '%'
%left '^'
%left UMINUS
%left '!'
%left '~'

%%
lsystem     :
        {
		      // Get lex into correct start condition
		      lex_pushstate(LEX_EXPRESSION);
		    }
		lines
		    { // This ensures that CF productions are applied last,
		      //  since they are at the end of the list of productions.
		      parserRules.append(&Context_free_rules);
		      lex_popstate();
		    }
	    ;

lines	    :	lines line
	    |	/* empty */
	    ;

line	    :	INCLUDE modules '\n'
	    ;

line	    :	IGNORE
		    { lex_pushstate(LEX_MODULE); }
		names '\n'
		    { lex_popstate(); }
	    ;

line	    :	DEFINE name expression
		    { Value v;
                      if (!parserSymbolTable.lookup(Name($2), v))
                        parserSymbolTable.enter(Name($2), $3->evaluate(parserSymbolTable));
		      delete $3;
		    }
		'\n'
	    ;

line	    :	START ':'
		    { lex_pushstate(LEX_MODULE);
		      // Bind starting list expressions according to symtab
		      BindExpression = true;
		    }
		modules '\n'
		    { BindExpression = false;
		      lex_popstate();
		      parserStart = $4;
		    }
	    ;

line	    :	'\n'
	    ;

line	    :	':'
		    { lex_pushstate(LEX_EXPRESSION); }
		expression
		    { lex_popstate();
		      std::cerr << "Evaluating expression: " << *$3 << std::endl;
		      std::cerr << $3->evaluate(parserSymbolTable) << std::endl;
		      delete $3;
		    }
	    ;

line	    :	name
		    { lex_pushstate(LEX_MODULE); }
		':' predecessor
		optional_conditional
		successors
		    { lex_popstate();
		      Production *p = new Production(Name($1), $4, $5, $6);
		      PDebug(PD_PARSER, std::cerr << "Parsed production: " << *p << std::endl);
		      if (p->context_free())
			Context_free_rules.append(p);
		      else
			parserRules.append(p);
		    }
	    ;

/* A list of names to be ignored in context matching; just add
 *  them to the table of ignored names.
 */
names	    :	names single_name
	    |	/* empty */
	    ;

single_name :	name
		    {
		      PDebug(PD_PARSER, std::cerr << "Ignoring name " << Name($1) << std::endl);
		      parserIgnoreTable.enter(Name($1), Value(1));
		    }
	    ;

/* The convolutions below ensure that at least one successor appears
 *  for a production. This not only makes sense intuitively, it
 *  avoids parsing ambiguities.
 */
successors  :	successor
		optional_successors
		    { $$ = new LSys::List<Successor>;
		      $$->append($1);
		      $$->append($2);
		      delete $2;
		    }
	    ;

optional_successors :
		optional_successors successor
		    { $$ = $1;
		      $$->append($2);
		    }
	    |	/* empty */
		    { $$ = new LSys::List<Successor>; }
	    ;

successor   :	YIELDS probability modules '\n'
		    { $$ = new Successor($3, $2);
		      PDebug(PD_PARSER, std::cerr << "Parsed successor: " << *$$ << std::endl);
		    }
	    ;

probability :	'(' value ')'
		    { float f;
		      if ($2->value(f))
			$$ = f;
		      else
			$$ = 0.0;
		      delete $2;
		    }
	    |	/* empty */
		    { $$ = 1.0; }
	    ;

predecessor :	modules
		    { PDebug(PD_PARSER, std::cerr << "\ttentative LHS is " << *$1 << std::endl); }
		center_list
		    {
		      PDebug(PD_PARSER,
			     std::cerr << "\tcenter,right are " << *$3 << std::endl);
		      $$ = $3;
		      if ($3->center == NULL) {
			// Two possibilities:
			//  A
			//  A > B
			// In both cases, 'modules' is center module alone;
			//  should verify list has only one module and
			//  delete the list after removing that module.

			LSys::ListIterator<Module> mi($1);
			$$->center = mi.first();
			PDebug(PD_PARSER, std::cerr << "Predecessor (no context) is: " << *$$ << std::endl);
		      } else {
			// Two possibilities:
			//  A < B
			//  A < B > C
			// In both cases, 'modules' is left context

			$$->left = $1;
			PDebug(PD_PARSER, std::cerr << "Predecessor (context) is: " << *$$ << std::endl);
		      }
		    }
	    ;

center_list :	'<' module right_list
		    { $$ = new Predecessor(NULL, $2, $3); }
	    |	right_list
		    { $$ = new Predecessor(NULL, NULL, $1); }
	    ;

right_list  :	'>' modules
		    { $$ = $2; }
	    |	/* empty */
		    { $$ = NULL; }
	    ;

modules     :	modules module
		    { $1->append($2);
		      $$ = $1;
		    }
	    |	/* empty */
		    { $$ = new LSys::List<Module>; }
	    ;

module	    :	name
		arguments
		    { Value v;
		      bool ignore = parserIgnoreTable.lookup(Name($1), v);

		      $$ = new Module(Name($1), $2, ignore);
		    }
	    ;

arguments   :	'('
		    { lex_pushstate(LEX_EXPRESSION); }
		exprlist ')'
		    { lex_popstate();
		      $$ = $3;
		    }
	    |	/* empty */
		    { $$ = NULL; }
	    ;

exprlist    :	exprlist ',' expression
		    { if (BindExpression == true) {
			Value v = $3->evaluate(parserSymbolTable);
			$1->append(new Expression(v));
			delete $3;
		      } else {
			$1->append($3);
		      }
		      PDebug(PD_PARSER, std::cerr << "Parsed additional expression: " << *$3 << std::endl);
		      $$ = $1;
		    }
	    |	expression
		    { $$ = new LSys::List<Expression>;
		      PDebug(PD_PARSER, std::cerr << "Parsed expression: " << *$1 << std::endl);
		      if (BindExpression == true) {
			Value v = $1->evaluate(parserSymbolTable);
			$$->append(new Expression(v));
			delete $1;
		      } else {
			$$->append($1);
		      }
		    }
	    |	/* empty */
		    { $$ = new LSys::List<Expression>; }
	    ;

optional_conditional :
		':'
		    { lex_pushstate(LEX_EXPRESSION); }
		conditional
		    { lex_popstate();
		      $$ = $3;
		    }
	    | /* empty */
		    { $$ = NULL; }
	    ;

conditional :	'*'
		    { $$ = NULL; }
	    |	expression
		    { PDebug(PD_PARSER, std::cerr << "Parsed conditional expression: " << *$1 << std::endl);
		      $$ = $1; }
	    ;

expression  :	expression OR  expression
		    { $$ = new Expression(OR ,$1,$3); }
	    |	expression AND expression
		    { $$ = new Expression(AND,$1,$3); }
	    |	expression '|' expression
		    { $$ = new Expression('|',$1,$3); }
	    |	expression '&' expression
		    { $$ = new Expression('&',$1,$3); }
	    |	expression EQ  expression
		    { $$ = new Expression(EQ ,$1,$3); }
	    |	expression NE  expression
		    { $$ = new Expression(NE ,$1,$3); }
	    |	expression '<' expression
		    { $$ = new Expression('<',$1,$3); }
	    |	expression LE  expression
		    { $$ = new Expression(LE ,$1,$3); }
	    |	expression GE  expression
		    { $$ = new Expression(GE ,$1,$3); }
	    |	expression '>' expression
		    { $$ = new Expression('>',$1,$3); }
	    |	expression '+' expression
		    { $$ = new Expression('+',$1,$3); }
	    |	expression '-' expression
		    { $$ = new Expression('-',$1,$3); }
	    |	expression '*' expression
		    { $$ = new Expression('*',$1,$3); }
	    |	expression '/' expression
		    { $$ = new Expression('/',$1,$3); }
	    |	expression '%' expression
		    { $$ = new Expression('%',$1,$3); }
	    |	expression '^' expression
		    { $$ = new Expression('^',$1,$3); }
	    |	'-' expression		    %prec UMINUS
		    { $$ = new Expression(UMINUS,$2,(Expression *)NULL); }
	    |	'!' expression
		    { $$ = new Expression('!'	,$2,(Expression *)NULL); }
	    |	'~' expression
		    { $$ = new Expression('~'	,$2,(Expression *)NULL); }
	    |	'(' expression ')'
		    { $$ = $2; }
	    |	name
		arguments
		    { $$ = new Expression(Name($1), $2); }
	    |	value
		    { $$ = new Expression(*$1); delete $1; }
	    ;

value	    :	INTEGER
		    { $$ = new Value(std::atoi(lex_token())); }
	    |	REAL
		    { $$ = new Value(std::atof(lex_token())); }
	    ;

name	    :	NAME
		    { PDebug(PD_NAME, std::cerr << "Calling Name::Name(token @" << ((void *)lex_token()) << " = " << lex_token() << ')' << std::endl);
		      $$ = Name(lex_token());
		    }
	    ;
%%

void yyerror(char *msg) {
    std::cout << std::flush;
    std::cerr << msg << " at line " << lexline << ", column "
	 << lexcol << "\n" << std::flush;
}


#ifdef sun
// Probably should be #ifndef _USES_BISON_ or some such
#ifndef __GNUG__
// /usr/lib/yaccpar is old-C and calls to free() should cast to (int *).
// Since there's no way to do that, we overload free() instead.
// This probably applies to other System V yaccs, too.
void free(int *ip) {
    free((char *)ip);
}

void free(YYSTYPE *yp) {
    free((char *)yp);
}
#endif /*!__GNUG__*/
#endif /*sun*/

