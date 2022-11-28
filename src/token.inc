#pragma once

#ifndef YYSTYPE
typedef union {
  int name;                         /* module or variable name id */
  float probability;                /* production probability */
  L_SYSTEM::Value* value;	                    /* numeric value */
  L_SYSTEM::Module* module;
  L_SYSTEM::Expression* expression;
  L_SYSTEM::List<L_SYSTEM::Module>* moduleList;	  /* list of modules as in a production successor */
  L_SYSTEM::List<L_SYSTEM::Expression>* expressionList;	/* list of expressions within a module */
  L_SYSTEM::Predecessor* predecessor;	        /* a production predecessor */
  L_SYSTEM::Successor* successor;	            /* a production successor */
  L_SYSTEM::List<L_SYSTEM::Successor>* successors;/* list of production successors with probabilities */
} yystype;

# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif

# define	ERROR	257
# define	VALUE	258
# define	INTEGER	259
# define	REAL	260
# define	FUNCTION	261
# define	IGNORE	262
# define	INCLUDE	263
# define	DEFINE	264
# define	YIELDS	265
# define	NAME	266
# define	START	267
# define	OR	268
# define	AND	269
# define	EQ	270
# define	NE	271
# define	LE	272
# define	GE	273
# define	UMINUS	274

extern YYSTYPE yylval;
