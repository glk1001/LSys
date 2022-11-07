.\"% nroff -man lsys.l
.\" lsys.l - man page for L-system generation program.
.\" $Id: lsys.l,v 1.4 91/03/20 22:06:26 leech Exp Locker: leech $
.de LS
.sp
.nf
.in +0.5i
..
.de LE
.sp
.fi
.in -0.5i
..
.TH LSYS 1L "March 21, 1991" " "
.ta 1i 2i
.SH NAME
lsys \- Plant generation program

.SH SYNOPSIS
\fBlsys\fP [options]

.SH DESCRIPTION
\fBlsys\fP is a tool for manipulating production systems known
as \fIL-systems\fP. In addition to specifying the systems and applying
production rules, \fBlsys\fP interprets the results graphically,
producing PostScript output.

\fBlsys\fP is modeled after the description in the book
\fIThe Virtual Laboratory: The Algorithmic Beauty of Plants\fP
by P. Prusinkiewicz and A. Lindenmayer, hereafter called ``the book''.
This document assumes familiarity with the book and with basic
concepts of 3-D computer graphics; you will not be able to use
\fBlsys\fP effectively without the book at hand as a reference.
The following sections describe the input format accepted by
\fBlsys\fP, how to run it, and notes on bugs, extensions,
and future enhancements.

.SH INPUT FORMAT
\fBlsys\fP accepts a grammar describing OL-systems (section 1.2 of
the book, DOL-systems)
augmented by brackets (section 1.6.3),
stochastically applied productions (section 1.7),
context-sensitivity (section 1.8),
and parameters (section 1.10).
All these features may be freely combined.

\fBlsys\fP does not accept timed DOL-systems (chapter 6) or map
L-systems (chapter 7) at present, although these extensions are
planned.

In general, the format used in the book is accepted by \fBlsys\fP.
Differences result from the limitations of ASCII input (many of
the L-systems in the book use subscripts and non-ASCII letters)
and a desire to minimize parsing conflicts in the yacc-based
input parser. Legal input is broken down into several classes:
\fIcomments\fP, \fIassertions\fP, \fIdefinitions\fP, the initial
string, and
\fIproductions\fP. Syntactic structures common to all
classes of input are described first, then each class. Unless
escaped with \\ or as otherwise specified, newline terminates
all classes of input. Blank lines and comments are allowed.

.SH NAMES
Two types of names are accepted: \fImodule\fP names, and \fIvariable\fP
names. Module names are restricted to one of
the two strings ``Fl'' and ``Fr'', a single letter or digit,
or one of the following
special characters:
.LS
\ +\ -\ ^\ &\ \\\ /\ |\ $\ [\ ]\ {\ .\ }\ ~\ !\ '\ %.
.LE
Expression names follow the rules for C identifiers: arbitrarily long strings
chosen from the character set [A-Za-z_0-9], with digits disallowed as
the leading character.

Module and expression names appear in different contexts as described below.
Expression names must be separated by other tokens or whitespace, while
the restrictions on module names allow then to appear with
no separating tokens.

.SH EXPRESSIONS
\fIExpressions\fP generally follow the rules of C expressions. Variable
names and numeric constants (either integer or real) are operands,
and operators are treated as in C, with the following exceptions:
.sp
.in +1i
.ti -0.5i
\(bu \ ^ is the exponentiation operator, not bitwise XOR.
.sp
.ti -0.5i
\(bu \ the operators (), [], ->, ., ++, --, (\fItype\fP), dereference
(*), address-of (&), \fBsizeof\fP, <<, >>, ?:, and \fIop\fP= are
not allowed.
.sp
.ti -0.5i
\(bu \ = and == are both equality operators; there is no assignment operator
.sp
.ti -0.5i
\(bu \ commas always serve to seperate expressions, rather than
being part of an expression as is allowed in C.
.sp
.ti -0.5i
\(bu \ functions, e.g. \fIfunc\fP(expression,expression,...),
are allowed. The following functions from the C library are accepted:
.LS
.ft B
sin cos tan asin acos atan atan2
abs ceil floor exp log log10 rand srand
.ft P
.LE
rand() returns a uniformly
distributed number on [0,1); rand(n) returns a uniformly distributed
number on [0,n). srand() reseeds the random number generator to the
current time; srand(n) reseeds with the specified (integer) seed.
The seed is returned.
.in -1i
.sp
Integers and reals combine under the usual C promotion rules, except
that integer / integer is done in floating point, for compatibility
with the book. Since variables are not statically typed,
promotion is done at evaluation time.

.SH MODULES
\fIModules\fP (section 1.10.1) consist of a module name followed by optional
parameters enclosed in parenthesis and separated by comma. Depending
on context, the parameters of a module may either be arbitrary
expressions or be restricted to either numeric constants
or variable names. Thus
.LS
F
.LE
is a module with no parameters,
.LS
+(90)
.LE
is a module with a numeric parameter,
.LS
Fr(l)
.LE
is a module with an variable name parameter, and
.LS
f(l*step,n-1)
.LE
is a module with 2 expression parameters.

The interpretation of specific module names is specified on page 209
of the book and in the discussion of tropism towards the end of
this document.

.SH MODULE LISTS
\fIModule lists\fP are simply strings of modules of arbitrary length.
No white space is required between modules (thus the restriction
on module names). Therefore, the list
.LS
F\|0\|f\|1
.LE
is interpreted as the
four modules `F', `0', `f', and `1' rather than the single
module ``F\|0\|f\|1''. The only ambiguity arises with the module names
``Fr'' and ``Fl'', which may be separated from other modules by
white space if needed.

.SH COMMENTS
C-style comments (delimited by /* and */) are accepted at any point
in the input and may extend over multiple lines. Note that comments
are treated as white space during lexical analysis, so that
de/*comment*/lta is two tokens, ``de'' and ``lta''.

.SH ASSERTIONS
Assertions affect the matching and interpretation of productions.
At present, there are only two assertions. A line of the form
.LS
#ignore name name ...
.LE
tags the specified module names such that they are skipped while searching
for context in context-sensitive productions. A line of the form
.LS
#include module module ...
.LE
causes modules matching those specified to be looked up in an external
library of shapes during graphical interpretation (\fBNOTE\fP: this
feature is not yet implemented; #include is accepted, but has no effect).

.SH DEFINITIONS
Definitions bind values to variables at a global level. The format is
.LS
#define variable expression
.LE
where \fIvariable\fP is a variable name whose value is set by
evaluating \fIexpression\fP. Note that unlike the C #define
statement, the expression must evaluate to a numeric value
based only on prior #defines; arbitrary textual replacement is
not allowed.

.SH STARTING MODULE LIST
The initial list of modules, to which productions are applied, is
specified by a line of the form
.LS
START\ :\ list
.LE
Arguments of modules in \fIlist\fP may be arbitrary expressions
in terms of global variables already defined, and will be evaluated
before applying the first set of productions.

.SH PRODUCTIONS
Productions (section 1.10.1) define replacement rules for modules.
Productions are in the following format:
.LS
prodname : predecessor condition successors
.LE
\fIprodname\fP is a variable name and is unused at present, aside
from error reporting. No restrictions on the choice of names applies.

\fIpredecessor\fP has three parts: an optional \fIleft context\fP,
a \fIstrict predecessor\fP, and an optional \fIright context\fP.
The left context, if present, must be a list of modules
followed by the character `<'. The strict predecessor must be
a single module. The right context, if present, must be the character
`>' followed by a list of modules. Modules appearing in the
predecessor may have an arbitrary number of parameters, but
all parameters must be variable names, which are bound to
the values in the matching module when a production is applied.
The variable names in the predecessor share the same name space
as the global variables set with #define, so collisions should be
avoided.

\fIcondition\fP is optional; if present, it consists of the character
`:' followed by an arbitrary expression which may use globally defined
variables and variables bound in the predecessor. The condition is
evaluated when applying a production; if it returns a nonzero value,
the production is applied.

\fIsuccessors\fP has the format
.sp
.ti +0.5i
`->' probability list `\\n'
.sp
repeated one or more times. The probability is optional and defaults
to 1 if not specified; if present, it must have the format `(' number `)'.
The list itself may be empty.

Stochastic productions are specified by giving more than one successor
to a predecessor and assigning each successor a nonzero probability.
When applying the production, a successor is chosen at random with
the specified probabilitiess. If no successor is chosen, which may happen
if probabilities sum to less than 1, the predecessor is replaced by
the empty string.

.SH PRODUCTION EXAMPLES
A production in a determinisic OL-system with brackets:

.ti +1i
p1 : F -> F[+F][-F]

A production with left context sensitivity, parameters, and a condition:

.ti +1i
p2 : + < F(l) : l < 2 -> F(l+1)

A stochastic production with parameters:

.nf
.in +1i
p3 : F(l)	-> (.33)F(l)[+F(l/3)]
	-> (.33)F(l)[-F(l/2)]
	-> (.34)F(l)[+F(l/2)][-F(l/3)]
.in -1i
.fi

.SH RUNNING THE PROGRAM
\fBlsys\fP accepts a single file in the format described above on
the standard input and applies the appropriate productions
for a specified number of generations. The final module list
is interpreted and an output file created (see \fBOUTPUT FORMATS\fP below).
Various options affecting
generation and interpretation may be set by definitions within
the input or by command-line arguments.
The definitions which apply are:
.LS
#define maxgen n
.LE
\fIn\fP is the number of generations to run the production rules for;
each generation applies the first matching production rule in
parallel to all modules in the current module list. n defaults
to 0, which causes interpretation of the initial module list.
.LS
#define delta angle
.LE
\fIdelta\fP is the default amount to turn, in degrees
(90 degrees if not specified).
.LS
#define width w
.LE
\fIw\fP scales the relative width of line segments. Normally,
a unit line is 1/100 units wide; setting w to 10 makes
a unit line 1/10 unit wide. Setting w to 0 produces a line of minimum
device width (this only applies to PostScript output).
.LS
#define plant value
.LE
Any definition of the variable ``plant'' enables special interpretation
of the modules A, I, K, and L in the output routines as a flowering apex,
internode, flower, and leaf respectively. This definition is needed to
produce most of the images in chapter 3 of the text. If not defined,
the usual interpretation (e.g. none) of these four modules is taken.

.SH COMMAND LINE OPTIONS

\fI-delta angle\fP sets the default turn angle.

\fI-display\fP causes
the module list produced at each generation to be printed on
standard output. A separate interpretation program will soon be able
to accept this output for more flexible viewing.

\fI-maxgen n\fP sets the number of generations.

\fI-width w\fP sets the relative line width.

.SH VIEWING OPTIONS
These options affect the virtual camera from which the final graphical
output is generated. They are:

\fI-eye x y z\fP specifies the eye position, default (0 0 1).

\fI-fov theta\fP specifies the field of view of a perspective
projection; if unspecified, an orthographic projection is used.

\fI-lookat x y z\fP specifies the look-at position, default (0 0 0).

\fI-up x y z\fP specifies the up vector, default (0 1 0).

\fI-noscale\fP suppresses the normal automatic scaling
applied to the output to make it fit within a page. In this case,
the canonical viewing window x = -1..1, y = -1..1 is scaled to
the page size; output outside this range is lost. Automatic
scaling should usually be suppressed when a perspective
projection is used.

See BUGS for why you may not want to use perspective viewing at
present.

.SH OUTPUT FORMATS
The default behavior is to generate a PostScript file named `output.ps'.
This may be printed on a PostScript printer or viewed with previewers
such as dxpsview (DEC).

\fI-generic\fP generates a file (default `output.generic')
in a text format.
This is not usable by any other rendering software; it is intended as
an example of how to develop database generators for other output
formats (see the README.OUTPUT file for more on this).

\fI-pphigs\fP generates a file (default `output.pphigs')
in a text format accepted by the PHIGS-like rendering system at UNC. This
is not expected to be useful at other sites.

\fI-blf\fP generates a file (default `output.blf') in a text format
accepted by some local database modification tools.  The format is
easily human and machine readable, something like NFF.

\fI-o\fP \fBfile\fP generates an output file of the specified name
instead of the default. Output to standard output is not possible
as the output generators may need to perform seeks on the output
stream.

.SH DEBUGGING
\fI-stats\fP enables printing of memory and time statistics.

\fI-D arg\fP controls diagnostic output useful in debugging
\fBlsys\fP. The argument following the option
enable diagnostics for the following parts of the program,
depending on which characters are contained:

.nf
.in +0.5i
.ta 1i
Char	Enables
A	All diagnostics
E	Expressions
G	Parsing
I	Interpretation
L	Lexical analysis
M	Modules
N	Names from lexical analysis
P	Productions
S	Symbols (variables)
l	Memory allocation
m	Main program
.in -0.5i
.fi

These options should not be needed in normal use.

.SH NOTABLE DIFFERENCES FROM THE BOOK
The method of implementing tropism used in the book is not described.
\fBlsys\fP binds the module \fIt\fP to control tropism. \fIt\fP(e) sets
the tropism ``suspectibility parameter'' e, and enables tropism.
\fIt\fP(0) disables tropism. \fIt\fP(x,y,z,e) sets the tropism
direction (x,y,z) and parameter e, and enables tropism.

Initially tropism is disabled,
the tropism direction is (0,-1,0) (down), and the suspectibility
parameter is 0.2. Since my implementation of tropism does not
produce results exactly comparable to the book (see BUGS), these details
will change in the near future.

.SH BUGS
Perspective viewing does not rescale line widths by the Z depth; the
resulting PostScript usually looks horrible (high priority).

The discussion of tropism in the book is not complete enough to
reproduce the images using it; the algorithm used in \fBlsys\fP
needs to capture some additional details.
This will take some effort to figure out.

There is a ``#consider'' assertion used in a few places in the book,
but no indication of what it is supposed to do.

There should be a way to specify values of variables on the
command line, overriding #defines.

The distinction between the 'G' and 'f'
commands is not properly implemented.

There should be a clean and flexible way to modify the interpretation
bindings without recompiling; the method (#define plant value)
used to enable the A, I, K, and L bindings is a temporary kludge.
These bindings are implemented in the PostScript generator,
but need some work. In particular, no correction for perspective
or rotation is made.

.SH UPCOMING FEATURES
Database drivers for other renderers.

\fBlsys\fP will be made faster and less of a memory hog.

Error checking needs to be improved; any input errors simply
produce a syntax error message specifying the offending line and column,
usually followed by fatal errors when applying productions.

.SH AUTHOR
Jon Leech (leech@cs.unc.edu)

.SH COPYRIGHT NOTICE
Copyright (C) 1990, 1991, Jonathan P. Leech.

This software may be freely copied, modified, and redistributed,
provided that this copyright notice is preserved on all copies.

There is no warranty or other guarantee of fitness for this software,
it is provided solely "as is". Bug reports or fixes may be sent
to the author, who may or may not act on them as he desires.

You may not include this software in a program or other software product
without supplying the source, or without informing the end-user that the
source is available for no extra charge.

If you modify this software, you should include a notice giving the
name of the person performing the modification, the date of modification,
and the reason for such modification.

.SH OTHER COMMENTS
Please send bugs (accompanied by L-systems causing them),
interesting L-systems for inclusion in the release, enhancements,
and suggestions to the author via email.
