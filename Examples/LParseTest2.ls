#define maxgen 10
#define delta  10
#define width 80
#define distance 100

#define trunk C

START : trunk
p1 : trunk : * -> [@Tx(2)'(3)H]z@mw(0.7)/trunk
p2 : H : * -> [^^^G]/(180)[^^^G]///H
p3 : G : * -> F@mw(0.7)^+F^+@ma(1.1111)@md(0.9)[&&&L]G
p4:  L : * -> [@Tx(3)'(4,5)~Leaf(1)]
/*p4:  L : * -> [@Tx(3)'(4,5){--z++z++z--|--z++z++z}]*/

