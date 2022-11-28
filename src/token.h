#pragma once

/* This must be the first include file in a file. Otherwise name clashes
     may occur. */

#include "module.h"
#include "production.h"
#include "token.inc"
#include "value.h"

namespace L_SYSTEM
{
const int LSYS_ERROR    = ERROR;
const int LSYS_VALUE    = VALUE;
const int LSYS_INTEGER  = INTEGER;
const int LSYS_REAL     = REAL;
const int LSYS_FUNCTION = FUNCTION;
const int LSYS_IGNORE   = IGNORE;
const int LSYS_INCLUDE  = INCLUDE;
const int LSYS_DEFINE   = DEFINE;
const int LSYS_YIELDS   = YIELDS;
const int LSYS_NAME     = NAME;
const int LSYS_START    = START;
const int LSYS_OR       = OR;
const int LSYS_AND      = AND;
const int LSYS_EQ       = EQ;
const int LSYS_NE       = NE;
const int LSYS_LE       = LE;
const int LSYS_GE       = GE;
const int LSYS_UMINUS   = UMINUS;

#undef ERROR
#undef VALUE
#undef INTEGER
#undef REAL
#undef FUNCTION
#undef IGNORE
#undef INCLUDE
#undef DEFINE
#undef YIELDS
#undef NAME
#undef START
#undef OR
#undef AND
#undef EQ
#undef NE
#undef LE
#undef GE
#undef UMINUS

} // namespace L_SYSTEM
