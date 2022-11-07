#ifndef _Rand_h
  #define _Rand_h

  #include <stdlib.h>

  namespace LSys {

    inline double drand48()
    {
      return float(rand())/float(RAND_MAX);
    }

    inline void srand48(unsigned int seed)
    {
      srand(seed);
    }

  };

#endif
