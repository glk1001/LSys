#include "GenericGenerator.h"

#include "Module.h"
#include "Turtle.h"
#include "Vector.h"

#include <fstream>
#include <iomanip>

namespace LSys
{

static constexpr auto PRECISION = 5;

auto GenericGenerator::SetHeader(const std::string& header) -> void
{
  *m_output << "Start_Comment\n";
  *m_output << '\n';

  *m_output << header << '\n';

  *m_output << "End_Comment\n";
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

  *m_output << "\n\n";
  *m_output << "RADEND"
            << "\n";

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
  //  out << vec[0] << " " << vec[1] << " " << vec[2];
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
  *m_boundsOutput << "start" << '\n';
  *m_boundsOutput << "  ";
  OutputVec(*m_boundsOutput, startPoint);
  *m_boundsOutput << '\n';
  *m_boundsOutput << "\n";

  // Output bounds
  *m_boundsOutput << "bounds" << '\n';
  *m_boundsOutput << "  min: " << std::setw(12) << Maths::Round(minBoundingBox[0], PRECISION) << " "
                  << std::setw(12) << Maths::Round(minBoundingBox[1], PRECISION) << " "
                  << std::setw(12) << Maths::Round(minBoundingBox[2], PRECISION) << '\n';
  *m_boundsOutput << "  max: " << std::setw(12) << Maths::Round(maxBoundingBox[0], PRECISION) << " "
                  << std::setw(12) << Maths::Round(maxBoundingBox[1], PRECISION) << " "
                  << std::setw(12) << Maths::Round(maxBoundingBox[2], PRECISION) << '\n';
  *m_boundsOutput << "\n\n";
}

auto GenericGenerator::Polygon(const Turtle& turtle, const LSys::Polygon& polygon) -> void
{
  // Draw the polygon
  auto polygonIter = ConstPolygonIterator{polygon};
  StartGraphics(turtle);

  //const Vector start     = this->LastPosition();
  //const Vector end       = turtle.GetPosition();
  const int frontColor   = turtle.GetColor().m_color.index;
  const int backColor    = turtle.GetBackColor().m_color.index;
  const int frontTexture = turtle.GetTexture();
  const int backTexture  = turtle.GetTexture();
  //const float width      = turtle.GetWidth();

  ++m_groupNum;
  *m_output << "Start_Object_Group " << m_groupNum << '\n';
  *m_output << " "
            << "FrontMaterial: " << frontColor << '\n';
  *m_output << " "
            << "FrontTexture: " << frontTexture << '\n';
  *m_output << " "
            << "BackMaterial: " << backColor << '\n';
  *m_output << " "
            << "BackTexture: " << backTexture << '\n';
  *m_output << '\n';

  *m_output << "  "
            << "polygon" << '\n';
  int n = 0;
  for (const Vector* vec = polygonIter.first(); vec != nullptr; vec = polygonIter.next())
  {
    ++n;
  }
  *m_output << "  "
            << "vertices: " << n << '\n';
  for (const Vector* vec = polygonIter.first(); vec != nullptr; vec = polygonIter.next())
  {
    *m_output << "  "
              << "  ";
    OutputVec(*m_output, *vec);
    *m_output << '\n';
  }
  *m_output << "\n";

  *m_output << "End_Object_Group " << m_groupNum << '\n';
  *m_output << "\n\n";
}

auto GenericGenerator::LineTo(const Turtle& turtle) -> void
{
  const Vector start      = GetLastPosition();
  const Vector end        = turtle.GetPosition();
  const int frontColor    = turtle.GetColor().m_color.index;
  const int backColor     = turtle.GetBackColor().m_color.index;
  const int frontTexture  = turtle.GetTexture();
  const int backTexture   = turtle.GetTexture();
  const float lineLength  = Distance(start, end);
  const float startRadius = (0.5F * GetLastWidth() * lineLength) / 100.0F;
  const float endRadius   = (0.5F * turtle.GetWidth() * lineLength) / 100.0F;

  ++m_groupNum;
  *m_output << "Start_Object_Group " << m_groupNum << '\n';
  *m_output << " "
            << "FrontMaterial: " << frontColor << '\n';
  *m_output << " "
            << "FrontTexture: " << frontTexture << '\n';
  *m_output << " "
            << "BackMaterial: " << backColor << '\n';
  *m_output << " "
            << "BackTexture: " << backTexture << '\n';
  *m_output << '\n';

  *m_output << "  "
            << "cone" << '\n';
  *m_output << "  "
            << "  ";
  OutputVec(*m_output, start);
  *m_output << '\n';
  *m_output << "  "
            << "  ";
  OutputVec(*m_output, end);
  *m_output << '\n';
  *m_output << "  "
            << "  " << Maths::Round(startRadius, PRECISION) << " "
            << Maths::Round(endRadius, PRECISION) << '\n';
  *m_output << '\n';

  *m_output << "  "
            << "sphere" << '\n';
  *m_output << "  "
            << "  ";
  OutputVec(*m_output, end);
  *m_output << '\n';
  *m_output << "  "
            << "  " << Maths::Round(endRadius, PRECISION) << '\n';
  *m_output << '\n';

  *m_output << "End_Object_Group " << m_groupNum << '\n';
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
  *m_output << "Start_Object_Group " << m_groupNum << '\n';
  *m_output << " "
            << "FrontMaterial: " << frontColor << '\n';
  *m_output << " "
            << "FrontTexture: " << frontTexture << '\n';
  *m_output << " "
            << "BackMaterial: " << backColor << '\n';
  *m_output << " "
            << "BackTexture: " << backTexture << '\n';
  *m_output << '\n';

  *m_output << " "
            << "object" << '\n';
  *m_output << " "
            << "  Name: " << objName << '\n';
  *m_output << " "
            << "  LineWidth: " << Maths::Round(width, PRECISION) << '\n';
  *m_output << " "
            << "  LineDistance: " << Maths::Round(distance, PRECISION) << '\n';
  *m_output << " "
            << "  ContactPoint: ";
  OutputVec(*m_output, contactPoint);
  *m_output << '\n';
  *m_output << " "
            << "  Heading: ";
  OutputVec(*m_output, turtle.GetHeading());
  *m_output << '\n';
  *m_output << " "
            << "  Left: ";
  OutputVec(*m_output, turtle.GetLeft());
  *m_output << '\n';
  *m_output << " "
            << "  Up:";
  OutputVec(*m_output, turtle.GetUp());
  *m_output << '\n';
  *m_output << " "
            << "  nargs: " << numArgs << '\n';
  for (auto i = 0U; i < static_cast<uint32_t>(numArgs); ++i)
  {
    *m_output << "  "
              << "    " << args[i] << '\n';
  }
  *m_output << '\n';

  *m_output << "End_Object_Group " << m_groupNum << '\n';
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
