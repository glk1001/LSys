#pragma once

#include "Consts.h"
#include "IGenerator.h"

#include <fstream>
#include <string>

namespace LSys
{

class GenericGenerator : public IGenerator
{
public:
  GenericGenerator(const std::string& outputFilename, const std::string& boundsFilename);

  auto SetHeader(const std::string& header) -> void override;

  // TODO(glk) - just use turtle ref?
  // Functions to provide bracketing information
  auto Prelude(const Turtle& turtle) -> void override;
  auto Postscript(const Turtle& turtle) -> void override;

  // Functions to start/end a stream of graphics
  auto StartGraphics(const Turtle& turtle) -> void override;
  auto FlushGraphics(const Turtle& turtle) -> void override;

  // Functions to draw objects in graphics mode
  auto Polygon(const Turtle& turtle, const LSys::Polygon& polygon) -> void override;
  auto LineTo(const Turtle& turtle) -> void override;
  auto Flower(const Turtle& turtle, float radius) -> void;
  auto Leaf(const Turtle& turtle, float length) -> void;
  auto Apex(const Turtle& turtle, Vector& start, float length) -> void;
  auto DrawObject(const Turtle& turtle, const Module& mod, int numArgs, const ArgsArray& args)
      -> void override;

  // Functions to change rendering parameters
  auto SetColor(const Turtle& turtle) -> void override;
  auto SetBackColor(const Turtle& turtle) -> void override;
  auto SetWidth(const Turtle& turtle) -> void override;
  auto SetTexture(const Turtle& turtle) -> void override;

private:
  std::ofstream m_output;
  std::ofstream m_boundsOutput;
  int m_groupNum = 0;
  auto OutputBounds(const Turtle& turtle) -> void;
  auto OutputAttributes(const Turtle::State& turtleState) -> void;
};

} // namespace LSys
