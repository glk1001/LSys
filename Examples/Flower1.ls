#define maxgen 5
#define delta 25.7

START : P
p1 : P : * -> I + [P + A] --//[-- L] I [++ L]-[P A] ++ P A
p2 : I : * -> F S [//&&L][//^^L]F S
p3 : S : * -> S F S
p4:  L : * -> ['(4,5)~Leaf(1)]
p5 : A : * -> [&&& B ' /W //// W //// W //// W //// W]
p6 : B : * -> FF
p7 : W : * -> ['(13)^F][{&&&&-f+f/(180)-f+f}]
