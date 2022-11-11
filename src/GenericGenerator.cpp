#include "GenericGenerator.h"

#include "Module.h"
#include "Turtle.h"
#include "Vector.h"

#include <fstream>
#include <iomanip>

using std::fixed;
using std::setprecision;
using std::setw;


namespace LSys
{

const int precision = 5;


void GenericGenerator::SetHeader(const std::string& str)
{
  *m_output << "Start_Comment\n";
  *m_output << '\n';

  *m_output << str << '\n';

  *m_output << "End_Comment\n";
  *m_output << "\n\n";
}


void GenericGenerator::Prelude(const Turtle& t)
{
  IGenerator::Prelude(t);
  *m_output << fixed << setprecision(precision);
  groupNum = 0;
}


void GenericGenerator::Postscript(const Turtle& t)
{
  this->OutputBounds(t);

  *m_output << "\n\n";
  *m_output << "RADEND"
            << "\n";

  IGenerator::Postscript(t);
}


// Called when starting a new stream of graphics
void GenericGenerator::StartGraphics(const Turtle&)
{
}


// Called when ending a stream of graphics, to display it
void GenericGenerator::FlushGraphics(const Turtle&)
{
}


inline void OutputVec(std::ostream& f, const Vector& v)
{
  //  f << v[0] << " " << v[1] << " " << v[2];
  // Revert to right handed coord system
  f << std::setw(10) << -Maths::Round(v[2], precision) << " " << std::setw(10)
    << Maths::Round(v[1], precision) << " " << std::setw(10) << -Maths::Round(v[0], precision);
}


void GenericGenerator::OutputBounds(const Turtle& t)
{
  const Vector bndsMin = t.GetBoundingBox().Min();
  const Vector bndsMax = t.GetBoundingBox().Max();
  const Vector startPoint(0, 0, 0);

  // Output start point
  *m_boundsOutput << "start" << '\n';
  *m_boundsOutput << "  ";
  OutputVec(*m_boundsOutput, startPoint);
  *m_boundsOutput << '\n';
  *m_boundsOutput << "\n";

  // Output bounds
  *m_boundsOutput << "bounds" << '\n';
  *m_boundsOutput << "  min: " << setw(12) << Maths::Round(bndsMin[0], precision) << " " << setw(12)
                  << Maths::Round(bndsMin[1], precision) << " " << setw(12)
                  << Maths::Round(bndsMin[2], precision) << '\n';
  *m_boundsOutput << "  max: " << setw(12) << Maths::Round(bndsMax[0], precision) << " " << setw(12)
                  << Maths::Round(bndsMax[1], precision) << " " << setw(12)
                  << Maths::Round(bndsMax[2], precision) << '\n';
  *m_boundsOutput << "\n\n";
}


void GenericGenerator::Polygon(const Turtle& turtle, const LSys::Polygon& polygon)
{
  // Draw the polygon

  ConstPolygonIterator pi(polygon);
  StartGraphics(turtle);

  //const Vector start     = this->LastPosition();
  //const Vector end       = turtle.GetPosition();
  const int frontColor   = turtle.GetColor().m_color.index;
  const int backColor    = turtle.GetBackColor().m_color.index;
  const int frontTexture = turtle.GetTexture();
  const int backTexture  = turtle.GetTexture();
  //const float width      = turtle.GetWidth();

  groupNum++;
  *m_output << "Start_Object_Group " << groupNum << '\n';
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
  for (const Vector* vec = pi.first(); vec != NULL; vec = pi.next())
    n++;
  *m_output << "  "
            << "vertices: " << n << '\n';
  for (const Vector* vec = pi.first(); vec != NULL; vec = pi.next())
  {
    *m_output << "  "
              << "  ";
    OutputVec(*m_output, *vec);
    *m_output << '\n';
  }
  *m_output << "\n";

  *m_output << "End_Object_Group " << groupNum << '\n';
  *m_output << "\n\n";
}


void GenericGenerator::LineTo(const Turtle& t)
{
  const Vector start      = this->GetLastPosition();
  const Vector end        = t.GetPosition();
  const int frontColor    = t.GetColor().m_color.index;
  const int backColor     = t.GetBackColor().m_color.index;
  const int frontTexture  = t.GetTexture();
  const int backTexture   = t.GetTexture();
  const float lineLength  = Distance(start, end);
  const float startRadius = (0.5F * this->GetLastWidth() * lineLength) / 100.0F;
  const float endRadius   = (0.5F * t.GetWidth() * lineLength) / 100.0F;

  groupNum++;
  *m_output << "Start_Object_Group " << groupNum << '\n';
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
            << "  " << Maths::Round(startRadius, precision) << " "
            << Maths::Round(endRadius, precision) << '\n';
  *m_output << '\n';

  *m_output << "  "
            << "sphere" << '\n';
  *m_output << "  "
            << "  ";
  OutputVec(*m_output, end);
  *m_output << '\n';
  *m_output << "  "
            << "  " << Maths::Round(endRadius, precision) << '\n';
  *m_output << '\n';

  *m_output << "End_Object_Group " << groupNum << '\n';
  *m_output << "\n\n";

  IGenerator::LineTo(t);
}


void GenericGenerator::DrawObject(const Turtle& turtle,
                                  const Module& mod,
                                  const int numArgs,
                                  const ArgsArray& args)
{
  const auto objName      = mod.GetName().str().erase(0, 1); // skip '~'
  const auto contactPoint = this->GetLastPosition();
  const auto width        = turtle.GetWidth();
  const auto distance     = turtle.GetDefaultDistance();
  const auto frontColor   = turtle.GetColor().m_color.index;
  const auto backColor    = turtle.GetBackColor().m_color.index;
  const auto frontTexture = turtle.GetTexture();
  const auto backTexture  = turtle.GetTexture();

  groupNum++;
  *m_output << "Start_Object_Group " << groupNum << '\n';
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
            << "  LineWidth: " << Maths::Round(width, precision) << '\n';
  *m_output << " "
            << "  LineDistance: " << Maths::Round(distance, precision) << '\n';
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
  for (auto i = 0U; i < static_cast<uint32_t>(numArgs); i++)
  {
    *m_output << "  "
              << "    " << args[i] << '\n';
  }
  *m_output << '\n';

  *m_output << "End_Object_Group " << groupNum << '\n';
  *m_output << "\n\n";
}


void GenericGenerator::SetColor(const Turtle&)
{
}


void GenericGenerator::SetBackColor(const Turtle&)
{
}


void GenericGenerator::SetTexture(const Turtle&)
{
}


void GenericGenerator::SetWidth(const Turtle&)
{
}


}; // namespace LSys
