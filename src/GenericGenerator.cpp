#include "GenericGenerator.h"

#include "Module.h"
#include "Turtle.h"
#include "Vector.h"

#include <fstream>
#include <iomanip>

namespace LSys
{

static constexpr auto PRECISION = 5;
static constexpr auto* INDENT   = "  ";

auto GenericGenerator::SetHeader(const std::string& header) -> void
{
  *m_output << "Start Header\n" << header << "End Header\n";
  *m_output << "\n\n";
}

auto GenericGenerator::Prelude(const Turtle& turtle) -> void
{
  IGenerator::Prelude(turtle);
  *m_output << std::fixed << std::setprecision(PRECISION);
  m_groupNum = 0;
}

auto GenericGenerator::Postscript(const Turtle& turtle) -> void
{
  this->OutputBounds(turtle);

  *m_output << "End File\n";

  IGenerator::Postscript(turtle);
}

auto GenericGenerator::StartGraphics([[maybe_unused]] const Turtle& turtle) -> void
{
  // Not used.
}

auto GenericGenerator::FlushGraphics([[maybe_unused]] const Turtle& turtle) -> void
{
  // Not used.
}

inline auto OutputVec(std::ostream& out, const Vector& vec) -> void
{
  out << std::setw(10) << Maths::Round(vec[0], PRECISION) << " " << std::setw(10)
      << Maths::Round(vec[1], PRECISION) << " " << std::setw(10) << Maths::Round(vec[2], PRECISION);
  return;

  // Revert to right-handed coord system
  out << std::setw(10) << -Maths::Round(vec[2], PRECISION) << " " << std::setw(10)
      << Maths::Round(vec[1], PRECISION) << " " << std::setw(10)
      << -Maths::Round(vec[0], PRECISION);
}

auto GenericGenerator::OutputBounds(const Turtle& turtle) -> void
{
  const auto minBoundingBox = turtle.GetBoundingBox().Min();
  const auto maxBoundingBox = turtle.GetBoundingBox().Max();
  const auto startPoint     = Vector{0.0F, 0.0F, 0.0F};

  // Output start point
  *m_boundsOutput << "start"
                  << "\n";
  *m_boundsOutput << INDENT;
  OutputVec(*m_boundsOutput, startPoint);
  *m_boundsOutput << "\n";
  *m_boundsOutput << "\n";

  // Output bounds
  *m_boundsOutput << "bounds"
                  << "\n";
  *m_boundsOutput << INDENT << "min: " << std::setw(12)
                  << Maths::Round(minBoundingBox[0], PRECISION) << " " << std::setw(12)
                  << Maths::Round(minBoundingBox[1], PRECISION) << " " << std::setw(12)
                  << Maths::Round(minBoundingBox[2], PRECISION) << "\n";
  *m_boundsOutput << INDENT << "max: " << std::setw(12)
                  << Maths::Round(maxBoundingBox[0], PRECISION) << " " << std::setw(12)
                  << Maths::Round(maxBoundingBox[1], PRECISION) << " " << std::setw(12)
                  << Maths::Round(maxBoundingBox[2], PRECISION) << "\n";
  *m_boundsOutput << "\n\n";
}

auto GenericGenerator::Polygon(const Turtle& turtle, const LSys::Polygon& polygon) -> void
{
  // Draw the polygon
  StartGraphics(turtle);

  //const auto& start     = this->LastPosition();
  //const auto& end       = turtle.GetPosition();
  const auto frontColor   = turtle.GetColor().m_color.index;
  const auto backColor    = turtle.GetBackColor().m_color.index;
  const auto frontTexture = turtle.GetTexture();
  const auto backTexture  = turtle.GetTexture();
  const auto width        = turtle.GetWidth();

  ++m_groupNum;
  *m_output << "Start Group " << m_groupNum << "\n";
  *m_output << INDENT << "FrontMaterial: " << frontColor << "\n";
  *m_output << INDENT << "FrontTexture: " << frontTexture << "\n";
  *m_output << INDENT << "BackMaterial: " << backColor << "\n";
  *m_output << INDENT << "BackTexture: " << backTexture << "\n";
  *m_output << INDENT << "Width: " << width << "\n";
  *m_output << "\n";

  *m_output << INDENT << "polygon"
            << "\n";
  *m_output << INDENT << "vertices: " << polygon.size() << "\n";
  for (auto polygonIter = cbegin(polygon); polygonIter != cend(polygon); ++polygonIter)
  {
    *m_output << INDENT << INDENT;
    OutputVec(*m_output, *polygonIter);
    *m_output << "\n";
  }
  *m_output << "\n";

  *m_output << "End Group " << m_groupNum << "\n";
  *m_output << "\n\n";
}

auto GenericGenerator::LineTo(const Turtle& turtle) -> void
{
  const auto& start       = GetLastPosition();
  const auto& end         = turtle.GetPosition();
  const auto frontColor   = turtle.GetColor().m_color.index;
  const auto backColor    = turtle.GetBackColor().m_color.index;
  const auto frontTexture = turtle.GetTexture();
  const auto backTexture  = turtle.GetTexture();
  const auto width        = turtle.GetWidth();

  ++m_groupNum;
  *m_output << "Start Group " << m_groupNum << "\n";
  *m_output << INDENT << "FrontMaterial: " << frontColor << "\n";
  *m_output << INDENT << "FrontTexture: " << frontTexture << "\n";
  *m_output << INDENT << "BackMaterial: " << backColor << "\n";
  *m_output << INDENT << "BackTexture: " << backTexture << "\n";
  *m_output << INDENT << "Width: " << width << "\n";
  *m_output << "\n";

  *m_output << INDENT << "line"
            << "\n";
  *m_output << INDENT << INDENT;
  OutputVec(*m_output, start);
  *m_output << "\n";
  *m_output << INDENT << INDENT;
  OutputVec(*m_output, end);
  *m_output << "\n";

  *m_output << "End Group " << m_groupNum << "\n";
  *m_output << "\n\n";

  IGenerator::LineTo(turtle);
}

auto GenericGenerator::DrawObject(const Turtle& turtle,
                                  const Module& mod,
                                  const int numArgs,
                                  const ArgsArray& args) -> void
{
  const auto objName      = mod.GetName().str().erase(0, 1); // skip '~'
  const auto contactPoint = GetLastPosition();
  const auto width        = turtle.GetWidth();
  const auto distance     = turtle.GetDefaultDistance();
  const auto frontColor   = turtle.GetColor().m_color.index;
  const auto backColor    = turtle.GetBackColor().m_color.index;
  const auto frontTexture = turtle.GetTexture();
  const auto backTexture  = turtle.GetTexture();

  ++m_groupNum;
  *m_output << "Start Group " << m_groupNum << "\n";
  *m_output << INDENT << "FrontMaterial: " << frontColor << "\n";
  *m_output << INDENT << "FrontTexture: " << frontTexture << "\n";
  *m_output << INDENT << "BackMaterial: " << backColor << "\n";
  *m_output << INDENT << "BackTexture: " << backTexture << "\n";
  *m_output << "\n";

  *m_output << INDENT << "object"
            << "\n";
  *m_output << INDENT << "  Name: " << objName << "\n";
  *m_output << INDENT << "  LineWidth: " << Maths::Round(width, PRECISION) << "\n";
  *m_output << INDENT << "  LineDistance: " << Maths::Round(distance, PRECISION) << "\n";
  *m_output << INDENT << "  ContactPoint: ";
  OutputVec(*m_output, contactPoint);
  *m_output << "\n";
  *m_output << INDENT << "  Heading: ";
  OutputVec(*m_output, turtle.GetHeading());
  *m_output << "\n";
  *m_output << INDENT << "  Left: ";
  OutputVec(*m_output, turtle.GetLeft());
  *m_output << "\n";
  *m_output << INDENT << "  Up:";
  OutputVec(*m_output, turtle.GetUp());
  *m_output << "\n";
  *m_output << INDENT << "  nargs: " << numArgs << "\n";
  for (auto i = 0U; i < static_cast<uint32_t>(numArgs); ++i)
  {
    *m_output << INDENT << "    " << args[i] << "\n";
  }
  *m_output << "\n";

  *m_output << "End Group " << m_groupNum << "\n";
  *m_output << "\n\n";
}

auto GenericGenerator::SetColor([[maybe_unused]] const Turtle& turtle) -> void
{
  // Not needed.
}

auto GenericGenerator::SetBackColor([[maybe_unused]] const Turtle& turtle) -> void
{
  // Not needed.
}

auto GenericGenerator::SetTexture([[maybe_unused]] const Turtle& turtle) -> void
{
  // Not needed.
}

auto GenericGenerator::SetWidth([[maybe_unused]] const Turtle& turtle) -> void
{
  // Not needed.
}

} // namespace LSys
