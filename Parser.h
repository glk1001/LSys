/* parser.h - global variables produced by L-system parser.
 *
 * $Id: parser.h,v 1.4 94/08/26 16:39:28 leech Exp $
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
 * $Log:	parser.h,v $
 * Revision 1.4  94/08/26  16:39:28  leech
 * Declare yyparse() for benefit of main().
 * 
 * Revision 1.3  91/10/10  20:08:32  leech
 * Declare new PlantModel global object instead of multiple previous tables
 * and lists.
 *
 * Revision 1.2  90/10/12  18:48:25  leech
 * First public release.
 *
 */

#ifndef _Parser_h
  #define _Parser_h

  #include "Value.h"
  #include "Expression.h"
  #include "Module.h"
  #include "Production.h"
  #include "LSysModel.h"

  #define parserRules (*parseInfo->rules)
  #define parserSymbolTable (*parseInfo->symbolTable)
  #define parserIgnoreTable (*parseInfo->ignoreTable)
  #define parserStart (parseInfo->start)

  // In lex.l, for setting input stream
  extern void set_parser_input(const char* ifile);

  // In lsys.y, for setting parser state
  extern void set_parser_globals(LSys::LSysModel*);
  extern int yyparse(void);

#endif
