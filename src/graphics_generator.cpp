module;

#include <stdexcept>

module LSys.GraphicsGenerator;

import LSys.Module;
import LSys.Vector;

namespace LSYS
{

GraphicsGenerator::GraphicsGenerator(const std::string& name, const DrawFuncs& drawFuncs)
  : IGenerator{name}, m_drawFuncs{drawFuncs}
{
}

auto GraphicsGenerator::SetHeader(const std::string& header) -> void
{
  IGenerator::SetHeader(header);
}

auto GraphicsGenerator::Prelude() -> void
{
  IGenerator::Prelude();

  m_groupNum = 0;
}

auto GraphicsGenerator::Postscript() -> void
{
  // Not used.
}

auto GraphicsGenerator::StartGraphics() -> void
{
  // Not used.
}

auto GraphicsGenerator::FlushGraphics() -> void
{
  // Not used.
}

auto GraphicsGenerator::Polygon(const LSYS::Polygon& polygon) -> void
{
  ++m_groupNum;

  const auto color   = GetTurtle().GetCurrentState().color.m_color.index;
  const auto width   = GetTurtle().GetCurrentState().width;
  auto closedPolygon = polygon;
  closedPolygon.emplace_back(polygon.front());

  m_drawFuncs.drawPolygonFunc(closedPolygon, color, width);
}

auto GraphicsGenerator::LineTo() -> void
{
  ++m_groupNum;

  const auto& start = GetLastPosition();
  const auto& end   = GetTurtle().GetCurrentState().position;
  const auto color  = GetTurtle().GetCurrentState().color.m_color.index;
  const auto width  = GetTurtle().GetCurrentState().width;

  m_drawFuncs.drawLineFunc(start, end, color, width);

  IGenerator::LineTo();
}

auto GraphicsGenerator::DrawObject([[maybe_unused]] const Module& mod,
                                   [[maybe_unused]] const int numArgs,
                                   [[maybe_unused]] const ArgsArray& args) -> void
{
  throw std::runtime_error("Not implemented.");
}

auto GraphicsGenerator::SetColor() -> void
{
  // Not needed.
}

auto GraphicsGenerator::SetBackColor() -> void
{
  // Not needed.
}

auto GraphicsGenerator::SetTexture() -> void
{
  // Not needed.
}

auto GraphicsGenerator::SetWidth() -> void
{
  // Not needed.
}

} // namespace LSYS
