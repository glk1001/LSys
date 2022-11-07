#ifndef _GenericGenerator_h
  #define _GenericGenerator_h

  #include <string>
  #include "Generator.h"

  namespace LSys {

    class GenericGenerator: public Generator {
      public:
        GenericGenerator(std::ofstream* output, std::ofstream* boundsOutput)
        : Generator(output, boundsOutput),
          groupNum(0)
        {}

        void SetHeader(const std::string&);

        // Functions to provide bracketing information
        void Prelude(const Turtle&);
        void Postscript(const Turtle&);

        // Functions to start/end a stream of graphics
        void StartGraphics(const Turtle&);
        void FlushGraphics(const Turtle&);

        // Functions to draw objects in graphics mode
        void Polygon(const Turtle&, const LSys::Polygon&);
        void LineTo(const Turtle&);
        void flower(const Turtle&, float radius);
        void leaf(const Turtle&, float length);
        void apex(const Turtle&, Vector& start, float length);
        void DrawObject(const Turtle&, const Module&, int nargs, const float args[]);

        // Functions to change rendering parameters
        void SetColor(const Turtle&);
        void SetBackColor(const Turtle&);
        void SetWidth(const Turtle&);
        void SetTexture(const Turtle&);
      private:
        int groupNum;
        void OutputBounds(const Turtle&);
    };

  };

#endif

