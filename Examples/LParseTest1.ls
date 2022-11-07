#define maxgen 10
#define delta  10
#define width 80
#define distance 100
#define x (maxgen-1)

/** #define plant 1  **/


/* p1 : C : * -> ['H]z!/C */
/* p3 : G : * -> F!^+F^+     ;'    [&&&L]G */
/* p4:  L : * -> ['{--z++z++z--|--z++z++z}] */

/*
C
C=[cH]z!>C
H=[^^^G]%[^^^G]>>>H
G=F!^+F^+;'[&&&L]G
L=[c{--z++z++z--|--z++z++z}]
*/


START : C
p1 : C : * -> [@Tx(2)'(3)H]'(1)Z@mw(0.7)/C
p2 : H : * -> [^^^GS(x)]/(180)[^^^GS(x)]///H
p3 : G : * -> F@mw(0.7)^+F^+@ma(1.1111)@md(0.9)[&&&(rand(50)+40)L][^^^(rand(50)+40)L][///(rand(50)+40)L]G
p4 : L : * -> [@Tx(3)'(4,5)~Leaf(1)]
p5 : S(x) : x < (maxgen-1) -> ['(2)@mw(0.3)@md(0.00001)F]
p6 : S(x) : x >= (maxgen-1) -> S(x-1)
