#pragma once

#include "Consts.h"
#include "IGenerator.h"

#include <string>

namespace LSys
{

class GenericGenerator : public IGenerator
{
public:
  GenericGenerator(std::ofstream* output, std::ofstream* bndsOutput)
    : IGenerator(output, bndsOutput), groupNum(0)
  {
  }

  void SetHeader(const std::string& header) override;

  // Functions to provide bracketing information
  void Prelude(const Turtle& turtle) override;
  void Postscript(const Turtle& turtle) override;

  // Functions to start/end a stream of graphics
  void StartGraphics(const Turtle& turtle) override;
  void FlushGraphics(const Turtle& turtle) override;

  // Functions to draw objects in graphics mode
  void Polygon(const Turtle& turtle, const LSys::Polygon& polygon) override;
  void LineTo(const Turtle& turtle) override;
  void flower(const Turtle& turtle, float radius);
  void leaf(const Turtle& turtle, float length);
  void apex(const Turtle& turtle, Vector& start, float length);
  void DrawObject(const Turtle& turtle,
                  const Module& mod,
                  int numArgs,
                  const ArgsArray& args) override;

  // Functions to change rendering parameters
  void SetColor(const Turtle& turtle) override;
  void SetBackColor(const Turtle& turtle) override;
  void SetWidth(const Turtle& turtle) override;
  void SetTexture(const Turtle& turtle) override;

private:
  int groupNum;
  void OutputBounds(const Turtle& turtle);
};

} // namespace LSys
