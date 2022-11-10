/* lexdefs.h - lexical analyzer states.
 *
 * $Id: lexdefs.h,v 1.2 90/10/12 18:48:21 leech Exp $
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
 * $Log:	lexdefs.h,v $
 * Revision 1.2  90/10/12  18:48:21  leech
 * First public release.
 *
 */

#pragma once

#define LEX_START      0    /* Start state */
#define LEX_EXPRESSION 1    /* Scan expression NAMEs state */
#define LEX_MODULE     2    /* Scan module NAMEs state */
