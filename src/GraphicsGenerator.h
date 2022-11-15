#pragma once

#include "Consts.h"
#include "IGenerator.h"
#include "Vector.h"

#include <functional>
#include <string>

namespace L_SYSTEM
{

class GraphicsGenerator : public IGenerator
{
public:
  using DrawLineFunc =
      std::function<void(const Vector& point1, const Vector& point2, int color, float lineWidth)>;
  using DrawPolygonFunc =
      std::function<void(const std::vector<Vector>& polygon, int color, float lineWidth)>;
  struct DrawFuncs
  {
    DrawLineFunc drawLineFunc;
    DrawPolygonFunc drawPolygonFunc;
  };

  explicit GraphicsGenerator(const DrawFuncs& drawFuncs);

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
  const DrawFuncs& m_drawFuncs;
  int m_groupNum = 0;
};

} // namespace L_SYSTEM
