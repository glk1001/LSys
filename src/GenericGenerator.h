#pragma once

#include "Consts.h"
#include "IGenerator.h"

#include <fstream>
#include <string>

namespace L_SYSTEM
{

class GenericGenerator : public IGenerator
{
public:
  GenericGenerator(const std::string& outputFilename, const std::string& boundsFilename);

  auto SetHeader(const std::string& header) -> void override;

  // Functions to provide bracketing information
  auto Prelude() -> void override;
  auto Postscript() -> void override;

  // Functions to start/end a stream of graphics
  auto StartGraphics() -> void override;
  auto FlushGraphics() -> void override;

  // Functions to draw objects in graphics mode
  auto Polygon(const L_SYSTEM::Polygon& polygon) -> void override;
  auto LineTo() -> void override;
  auto Flower(float radius) -> void;
  auto Leaf(float length) -> void;
  auto Apex(Vector& start, float length) -> void;
  auto DrawObject(const Module& mod, int numArgs, const ArgsArray& args) -> void override;

  // Functions to change rendering parameters
  auto SetColor() -> void override;
  auto SetBackColor() -> void override;
  auto SetWidth() -> void override;
  auto SetTexture() -> void override;

private:
  std::ofstream m_output;
  std::ofstream m_boundsOutput;
  int m_groupNum = 0;
  auto OutputBounds() -> void;
  auto OutputAttributes(const Turtle::State& turtleState) -> void;
};

} // namespace L_SYSTEM
