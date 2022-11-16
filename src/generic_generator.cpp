#include "generic_generator.h"

#include "module.h"
#include "turtle.h"
#include "vector.h"

#include <fstream>
#include <iomanip>
#include <stdexcept>

namespace L_SYSTEM
{

static constexpr auto PRECISION = 5;
static constexpr auto* INDENT   = "  ";

GenericGenerator::GenericGenerator(const std::string& outputFilename,
                                   const std::string& boundsFilename)
  : m_output{outputFilename}, m_boundsOutput{boundsFilename}
{
  if (not m_output)
  {
    throw std::runtime_error("RadianceGenerator: Could not open output file.");
  }
  if (not m_boundsOutput)
  {
    throw std::runtime_error("RadianceGenerator: Could not open bounds output file.");
  }
}

auto GenericGenerator::SetHeader(const std::string& header) -> void
{
  IGenerator::SetHeader(header);

  m_output << "Start Header\n" << header << "End Header\n";
  m_output << "\n\n";
}

auto GenericGenerator::Prelude() -> void
{
  IGenerator::Prelude();

  m_output << std::fixed << std::setprecision(PRECISION);
  if (m_output.bad())
  {
    OutputFailed();
  }

  m_groupNum = 0;
}

auto GenericGenerator::Postscript() -> void
{
  OutputBounds();

  m_output << "End File\n";

  if (m_output.bad())
  {
    OutputFailed();
  }
  m_output.close();
}

auto GenericGenerator::StartGraphics() -> void
{
  // Not used.
}

auto GenericGenerator::FlushGraphics() -> void
{
  // Not used.
}

static inline auto OutputVec(std::ostream& out, const Vector& vec) -> void
{
  out << std::setw(10) << MATHS::Round(vec[0], PRECISION) << " " << std::setw(10)
      << MATHS::Round(vec[1], PRECISION) << " " << std::setw(10) << MATHS::Round(vec[2], PRECISION);
  return;

  // Revert to right-handed coord system
  out << std::setw(10) << -MATHS::Round(vec[2], PRECISION) << " " << std::setw(10)
      << MATHS::Round(vec[1], PRECISION) << " " << std::setw(10)
      << -MATHS::Round(vec[0], PRECISION);
}

auto GenericGenerator::OutputBounds() -> void
{
  const auto minBoundingBox = GetTurtle().GetBoundingBox().Min();
  const auto maxBoundingBox = GetTurtle().GetBoundingBox().Max();
  const auto startPoint     = Vector{0.0F, 0.0F, 0.0F};

  // Output start point
  m_boundsOutput << "start"
                 << "\n";
  m_boundsOutput << INDENT;
  OutputVec(m_boundsOutput, startPoint);
  m_boundsOutput << "\n";
  m_boundsOutput << "\n";

  // Output bounds
  m_boundsOutput << "bounds"
                 << "\n";
  m_boundsOutput << INDENT << "min: " << std::setw(12) << MATHS::Round(minBoundingBox[0], PRECISION)
                 << " " << std::setw(12) << MATHS::Round(minBoundingBox[1], PRECISION) << " "
                 << std::setw(12) << MATHS::Round(minBoundingBox[2], PRECISION) << "\n";
  m_boundsOutput << INDENT << "max: " << std::setw(12) << MATHS::Round(maxBoundingBox[0], PRECISION)
                 << " " << std::setw(12) << MATHS::Round(maxBoundingBox[1], PRECISION) << " "
                 << std::setw(12) << MATHS::Round(maxBoundingBox[2], PRECISION) << "\n";
  m_boundsOutput << "\n\n";
}

auto GenericGenerator::OutputAttributes(const Turtle::State& turtleState) -> void
{
  m_output << INDENT << "FrontMaterial: " << turtleState.color.m_color.index << "\n";
  m_output << INDENT << "FrontTexture: " << turtleState.texture << "\n";
  m_output << INDENT << "BackMaterial: " << turtleState.backgroundColor.m_color.index << "\n";
  m_output << INDENT << "BackTexture: " << turtleState.texture << "\n";
  m_output << INDENT << "Width: " << turtleState.width << "\n";
}

auto GenericGenerator::Polygon(const L_SYSTEM::Polygon& polygon) -> void
{
  // Draw the polygon
  StartGraphics();

  ++m_groupNum;
  m_output << "Start Group " << m_groupNum << "\n";
  OutputAttributes(GetTurtle().GetCurrentState());
  m_output << "\n";

  const auto numClosedPolygonVertices = polygon.size() + 1;
  m_output << INDENT << "polygon\n";
  m_output << INDENT << "vertices: " << numClosedPolygonVertices << "\n";
  for (auto polygonIter = cbegin(polygon); polygonIter != cend(polygon); ++polygonIter)
  {
    m_output << INDENT << INDENT;
    OutputVec(m_output, *polygonIter);
    m_output << "\n";
  }
  // Close the polygon.
  m_output << INDENT << INDENT;
  OutputVec(m_output, polygon.front());
  m_output << "\n";
  m_output << "\n";

  m_output << "End Group " << m_groupNum << "\n";
  m_output << "\n\n";
}

auto GenericGenerator::LineTo() -> void
{
  ++m_groupNum;
  m_output << "Start Group " << m_groupNum << "\n";
  OutputAttributes(GetTurtle().GetCurrentState());
  m_output << "\n";

  const auto& start = GetLastPosition();
  const auto& end   = GetTurtle().GetCurrentState().position;

  m_output << INDENT << "line\n";
  m_output << INDENT << INDENT;
  OutputVec(m_output, start);
  m_output << "\n";
  m_output << INDENT << INDENT;
  OutputVec(m_output, end);
  m_output << "\n";

  m_output << "End Group " << m_groupNum << "\n";
  m_output << "\n\n";

  IGenerator::LineTo();
}

auto GenericGenerator::DrawObject(const Module& mod, const int numArgs, const ArgsArray& args)
    -> void
{
  const auto objName      = mod.GetName().str().erase(0, 1); // skip '~'
  const auto contactPoint = GetLastPosition();

  ++m_groupNum;
  m_output << "Start Group " << m_groupNum << "\n";
  OutputAttributes(GetTurtle().GetCurrentState());
  m_output << "\n";

  m_output << INDENT << "object\n";
  m_output << INDENT << "  Name: " << objName << "\n";
  m_output << INDENT
           << "  LineWidth: " << MATHS::Round(GetTurtle().GetCurrentState().width, PRECISION)
           << "\n";
  m_output << INDENT << "  LineDistance: "
           << MATHS::Round(GetTurtle().GetCurrentState().defaultDistance, PRECISION) << "\n";
  m_output << INDENT << "  ContactPoint: ";
  OutputVec(m_output, contactPoint);
  m_output << "\n";
  m_output << INDENT << "  Heading: ";
  OutputVec(m_output, GetTurtle().GetHeading());
  m_output << "\n";
  m_output << INDENT << "  Left: ";
  OutputVec(m_output, GetTurtle().GetLeft());
  m_output << "\n";
  m_output << INDENT << "  Up:";
  OutputVec(m_output, GetTurtle().GetUp());
  m_output << "\n";
  m_output << INDENT << "  nargs: " << numArgs << "\n";
  for (auto i = 0U; i < static_cast<uint32_t>(numArgs); ++i)
  {
    m_output << INDENT << "    " << args[i] << "\n";
  }
  m_output << "\n";

  m_output << "End Group " << m_groupNum << "\n";
  m_output << "\n\n";
}

auto GenericGenerator::SetColor() -> void
{
  // Not needed.
}

auto GenericGenerator::SetBackColor() -> void
{
  // Not needed.
}

auto GenericGenerator::SetTexture() -> void
{
  // Not needed.
}

auto GenericGenerator::SetWidth() -> void
{
  // Not needed.
}

} // namespace L_SYSTEM