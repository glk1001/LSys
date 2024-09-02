module;

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <stdexcept>

module LSys.RadianceGenerator;

import LSys.Module;
import LSys.Vector;

namespace LSYS
{

static constexpr auto PRECISION = 5;

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
RadianceGenerator::RadianceGenerator(const std::string& outputFilename,
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

auto RadianceGenerator::SetHeader(const std::string& header) -> void
{
  IGenerator::SetHeader(header);

  m_output << "Start_Comment\n";
  m_output << '\n';

  m_output << header << '\n';

  m_output << "End_Comment\n";
  m_output << "\n\n";
}

auto RadianceGenerator::Prelude() -> void
{
  IGenerator::Prelude();

  m_output << std::fixed << std::setprecision(PRECISION);
  if (m_output.bad())
  {
    OutputFailed();
  }

  m_groupNum = 0;
}

auto RadianceGenerator::Postscript() -> void
{
  OutputBounds();

  m_output << "\n\n";
  m_output << "RADEND\n";

  if (m_output.bad())
  {
    OutputFailed();
  }
  m_output.close();
}

auto RadianceGenerator::StartGraphics() -> void
{
  // Not used.
}

auto RadianceGenerator::FlushGraphics() -> void
{
  // Not used.
}

namespace
{
inline auto OutputVec(std::ostream& out, const Vector& vec) -> void
{
  static constexpr auto WID = 10U;
  //  out << vec[0] << " " << vec[1] << " " << vec[2];
  // Revert to right-handed coord system
  out << std::setw(WID) << -MATHS::Round(vec[2], PRECISION) << " " << std::setw(WID)
      << MATHS::Round(vec[1], PRECISION) << " " << std::setw(WID)
      << -MATHS::Round(vec[0], PRECISION);
}
} // namespace

auto RadianceGenerator::OutputBounds() -> void
{
  const auto minBoundingBox = GetTurtle().GetBoundingBox().Min();
  const auto maxBoundingBox = GetTurtle().GetBoundingBox().Max();
  const auto startPoint     = Vector{0.0F, 0.0F, 0.0F};

  // Output start point
  m_boundsOutput << "start" << '\n';
  m_boundsOutput << "  ";
  OutputVec(m_boundsOutput, startPoint);
  m_boundsOutput << '\n';
  m_boundsOutput << "\n";

  // Output bounds
  static constexpr auto WID = 12U;
  m_boundsOutput << "bounds" << '\n';
  m_boundsOutput << "  min: " << std::setw(WID) << MATHS::Round(minBoundingBox[0], PRECISION) << " "
                 << std::setw(WID) << MATHS::Round(minBoundingBox[1], PRECISION) << " "
                 << std::setw(WID) << MATHS::Round(minBoundingBox[2], PRECISION) << '\n';
  m_boundsOutput << "  max: " << std::setw(WID) << MATHS::Round(maxBoundingBox[0], PRECISION) << " "
                 << std::setw(WID) << MATHS::Round(maxBoundingBox[1], PRECISION) << " "
                 << std::setw(WID) << MATHS::Round(maxBoundingBox[2], PRECISION) << '\n';
  m_boundsOutput << "\n\n";
}

auto RadianceGenerator::Polygon(const LSYS::Polygon& polygon) -> void
{
  // Draw the polygon
  StartGraphics();

  ++m_groupNum;
  m_output << "Start_Object_Group " << m_groupNum << '\n';
  m_output << " "
           // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
           << "FrontMaterial: " << GetTurtle().GetCurrentState().color.m_color.index << "\n";
  m_output << " " << "FrontTexture: " << GetTurtle().GetCurrentState().texture << "\n";
  m_output << " "
           // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
           << "BackMaterial: " << GetTurtle().GetCurrentState().backgroundColor.m_color.index
           << "\n";
  m_output << " " << "BackTexture: " << GetTurtle().GetCurrentState().texture << "\n";
  m_output << '\n';

  m_output << "  " << "polygon" << '\n';
  m_output << "  " << "vertices: " << polygon.size() << '\n';
  for (auto polygonIter : polygon)
  {
    m_output << "  " << "  ";
    OutputVec(m_output, polygonIter);
    m_output << '\n';
  }
  m_output << "\n";

  m_output << "End_Object_Group " << m_groupNum << '\n';
  m_output << "\n\n";
}

auto RadianceGenerator::LineTo() -> void
{
  const Vector start      = GetLastPosition();
  const Vector end        = GetTurtle().GetCurrentState().position;
  const float lineLength  = Distance(start, end);
  const float startRadius = (0.5F * GetLastWidth() * lineLength) / 100.0F;
  const float endRadius   = (0.5F * GetTurtle().GetCurrentState().width * lineLength) / 100.0F;

  ++m_groupNum;
  m_output << "Start_Object_Group " << m_groupNum << '\n';
  m_output << " "
           // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
           << "FrontMaterial: " << GetTurtle().GetCurrentState().color.m_color.index << "\n";
  m_output << " " << "FrontTexture: " << GetTurtle().GetCurrentState().texture << "\n";
  m_output << " "
           // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
           << "BackMaterial: " << GetTurtle().GetCurrentState().backgroundColor.m_color.index
           << "\n";
  m_output << " " << "BackTexture: " << GetTurtle().GetCurrentState().texture << "\n";
  m_output << '\n';

  m_output << "  " << "cone" << '\n';
  m_output << "  " << "  ";
  OutputVec(m_output, start);
  m_output << '\n';
  m_output << "  " << "  ";
  OutputVec(m_output, end);
  m_output << '\n';
  m_output << "  " << "  " << MATHS::Round(startRadius, PRECISION) << " "
           << MATHS::Round(endRadius, PRECISION) << '\n';
  m_output << '\n';

  m_output << "  " << "sphere" << '\n';
  m_output << "  " << "  ";
  OutputVec(m_output, end);
  m_output << '\n';
  m_output << "  " << "  " << MATHS::Round(endRadius, PRECISION) << '\n';
  m_output << '\n';

  m_output << "End_Object_Group " << m_groupNum << '\n';
  m_output << "\n\n";

  IGenerator::LineTo();
}

auto RadianceGenerator::DrawObject(const Module& mod, const int numArgs, const ArgsArray& args)
    -> void
{
  const auto objName      = mod.GetName().str().erase(0, 1); // skip '~'
  const auto contactPoint = GetLastPosition();

  ++m_groupNum;
  m_output << "Start_Object_Group " << m_groupNum << '\n';
  m_output << " "
           // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
           << "FrontMaterial: " << GetTurtle().GetCurrentState().color.m_color.index << "\n";
  m_output << " " << "FrontTexture: " << GetTurtle().GetCurrentState().texture << "\n";
  m_output << " "
           // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
           << "BackMaterial: " << GetTurtle().GetCurrentState().backgroundColor.m_color.index
           << "\n";
  m_output << " " << "BackTexture: " << GetTurtle().GetCurrentState().texture << "\n";
  m_output << '\n';

  m_output << " " << "object" << '\n';
  m_output << " " << "  Name: " << objName << '\n';
  m_output << " " << "  LineWidth: " << MATHS::Round(GetTurtle().GetCurrentState().width, PRECISION)
           << '\n';
  m_output << " " << "  LineDistance: "
           << MATHS::Round(GetTurtle().GetCurrentState().defaultDistance, PRECISION) << '\n';
  m_output << " " << "  ContactPoint: ";
  OutputVec(m_output, contactPoint);
  m_output << '\n';
  m_output << " " << "  Heading: ";
  OutputVec(m_output, GetTurtle().GetHeading());
  m_output << '\n';
  m_output << " " << "  Left: ";
  OutputVec(m_output, GetTurtle().GetLeft());
  m_output << '\n';
  m_output << " " << "  Up:";
  OutputVec(m_output, GetTurtle().GetUp());
  m_output << '\n';
  m_output << " " << "  nargs: " << numArgs << '\n';
  for (auto i = 0U; i < static_cast<uint32_t>(numArgs); ++i)
  {
    m_output << "  " << "    " << args.at(i) << '\n';
  }
  m_output << '\n';

  m_output << "End_Object_Group " << m_groupNum << '\n';
  m_output << "\n\n";
}

auto RadianceGenerator::SetColor() -> void
{
  // Not needed.
}

auto RadianceGenerator::SetBackColor() -> void
{
  // Not needed.
}

auto RadianceGenerator::SetTexture() -> void
{
  // Not needed.
}

auto RadianceGenerator::SetWidth() -> void
{
  // Not needed.
}

} // namespace LSYS
