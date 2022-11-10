/*
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
   * $Log:	debug.h,v $
   * Revision 1.3  91/03/20  10:40:46  leech
   * New debug bit for Names. Support for G++.
   *
   * Revision 1.2  90/10/12  18:48:18  leech
   * First public release.
   *
   */

#pragma once

extern int ParseDebug;

#define PD_EXPRESSION 0x1
#define PD_LEXER 0x2
#define PD_MAIN 0x4
#define PD_MODULE 0x8
#define PD_PARSER 0x10
#define PD_PRODUCTION 0x20
#define PD_INTERPRET 0x80
#define PD_NAME 0x200

#define PDEBUG_ENABLED
#ifdef PDEBUG_ENABLED

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wextra"
#define PDebug(level, code) \
  [&]() { if (ParseDebug) { code; } } ()
#pragma GCC diagnostic pop

#else
#define PDebug(level, code)
#endif
