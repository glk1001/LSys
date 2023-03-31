/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_LSYS_TAB_H_INCLUDED
# define YY_YY_LSYS_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    ERROR = 258,                   /* ERROR  */
    VALUE = 259,                   /* VALUE  */
    INTEGER = 260,                 /* INTEGER  */
    REAL = 261,                    /* REAL  */
    FUNCTION = 262,                /* FUNCTION  */
    IGNORE = 263,                  /* IGNORE  */
    INCLUDE = 264,                 /* INCLUDE  */
    DEFINE = 265,                  /* DEFINE  */
    YIELDS = 266,                  /* YIELDS  */
    NAME = 267,                    /* NAME  */
    START = 268,                   /* START  */
    OR = 269,                      /* OR  */
    AND = 270,                     /* AND  */
    EQ = 271,                      /* EQ  */
    NE = 272,                      /* NE  */
    LE = 273,                      /* LE  */
    GE = 274,                      /* GE  */
    UMINUS = 275                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{

  int name;                                /* module or variable name id */
  float probability;                       /* production probability */
  LSYS::Value* value;	                   /* numeric value */
  LSYS::Module* module;
  LSYS::Expression* expression;
  LSYS::List<LSYS::Module>* moduleList;	   /* list of modules as in a production successor */
  LSYS::List<LSYS::Expression>* expressionList;
                                           /* list of expressions within a module */
  LSYS::Predecessor* predecessor;          /* a production predecessor */
  LSYS::Successor* successor;	           /* a production successor */
  LSYS::List<LSYS::Successor>* successors; /* list of production successors with probabilities */


};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_LSYS_TAB_H_INCLUDED  */
