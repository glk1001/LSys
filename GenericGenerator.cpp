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
  *out << "Start_Comment\n";
  *out << '\n';

  *out << str << '\n';

  *out << "End_Comment\n";
  *out << "\n\n";
}


void GenericGenerator::Prelude(const Turtle& t)
{
  Generator::Prelude(t);
  *out << fixed << setprecision(precision);
  groupNum = 0;
}


void GenericGenerator::Postscript(const Turtle& t)
{
  this->OutputBounds(t);

  *out << "\n\n";
  *out << "RADEND"
       << "\n";

  Generator::Postscript(t);
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
  const Vector bndsMin = t.Bounds().Min();
  const Vector bndsMax = t.Bounds().Max();
  const Vector startPoint(0, 0, 0);

  // Output start point
  *boundsOutput << "start" << '\n';
  *boundsOutput << "  ";
  OutputVec(*boundsOutput, startPoint);
  *boundsOutput << '\n';
  *boundsOutput << "\n";

  // Output bounds
  *boundsOutput << "bounds" << '\n';
  *boundsOutput << "  min: " << setw(12) << Maths::Round(bndsMin[0], precision) << " " << setw(12)
                << Maths::Round(bndsMin[1], precision) << " " << setw(12)
                << Maths::Round(bndsMin[2], precision) << '\n';
  *boundsOutput << "  max: " << setw(12) << Maths::Round(bndsMax[0], precision) << " " << setw(12)
                << Maths::Round(bndsMax[1], precision) << " " << setw(12)
                << Maths::Round(bndsMax[2], precision) << '\n';
  *boundsOutput << "\n\n";
}


void GenericGenerator::Polygon(const Turtle& turtle, const LSys::Polygon& polygon)
{
  // Draw the polygon

  ConstPolygonIterator pi(polygon);
  StartGraphics(turtle);

  //const Vector start     = this->LastPosition();
  //const Vector end       = turtle.Location();
  const int frontColor   = turtle.CurrentColor().m_color.index;
  const int backColor    = turtle.CurrentBackColor().m_color.index;
  const int frontTexture = turtle.CurrentTexture();
  const int backTexture  = turtle.CurrentTexture();
  //const float width      = turtle.CurrentWidth();

  groupNum++;
  *out << "Start_Object_Group " << groupNum << '\n';
  *out << " "
       << "FrontMaterial: " << frontColor << '\n';
  *out << " "
       << "FrontTexture: " << frontTexture << '\n';
  *out << " "
       << "BackMaterial: " << backColor << '\n';
  *out << " "
       << "BackTexture: " << backTexture << '\n';
  *out << '\n';

  *out << "  "
       << "polygon" << '\n';
  int n = 0;
  for (const Vector* vec = pi.first(); vec != NULL; vec = pi.next())
    n++;
  *out << "  "
       << "vertices: " << n << '\n';
  for (const Vector* vec = pi.first(); vec != NULL; vec = pi.next())
  {
    *out << "  "
         << "  ";
    OutputVec(*out, *vec);
    *out << '\n';
  }
  *out << "\n";

  *out << "End_Object_Group " << groupNum << '\n';
  *out << "\n\n";
}


void GenericGenerator::LineTo(const Turtle& t)
{
  const Vector start      = this->LastPosition();
  const Vector end        = t.Location();
  const int frontColor    = t.CurrentColor().m_color.index;
  const int backColor     = t.CurrentBackColor().m_color.index;
  const int frontTexture  = t.CurrentTexture();
  const int backTexture   = t.CurrentTexture();
  const float lineLength  = Distance(start, end);
  const float startRadius = (0.5F * this->LastWidth() * lineLength) / 100.0F;
  const float endRadius   = (0.5F * t.CurrentWidth() * lineLength) / 100.0F;

  groupNum++;
  *out << "Start_Object_Group " << groupNum << '\n';
  *out << " "
       << "FrontMaterial: " << frontColor << '\n';
  *out << " "
       << "FrontTexture: " << frontTexture << '\n';
  *out << " "
       << "BackMaterial: " << backColor << '\n';
  *out << " "
       << "BackTexture: " << backTexture << '\n';
  *out << '\n';

  *out << "  "
       << "cone" << '\n';
  *out << "  "
       << "  ";
  OutputVec(*out, start);
  *out << '\n';
  *out << "  "
       << "  ";
  OutputVec(*out, end);
  *out << '\n';
  *out << "  "
       << "  " << Maths::Round(startRadius, precision) << " " << Maths::Round(endRadius, precision)
       << '\n';
  *out << '\n';

  *out << "  "
       << "sphere" << '\n';
  *out << "  "
       << "  ";
  OutputVec(*out, end);
  *out << '\n';
  *out << "  "
       << "  " << Maths::Round(endRadius, precision) << '\n';
  *out << '\n';

  *out << "End_Object_Group " << groupNum << '\n';
  *out << "\n\n";

  Generator::LineTo(t);
}


void GenericGenerator::DrawObject(const Turtle& turtle,
                                  const Module& mod,
                                  const int numArgs,
                                  const ArgsArray& args)
{
  const auto objName      = mod.GetName().str().erase(0, 1); // skip '~'
  const auto contactPoint = this->LastPosition();
  const auto width        = turtle.CurrentWidth();
  const auto distance     = turtle.DefaultDistance();
  const auto frontColor   = turtle.CurrentColor().m_color.index;
  const auto backColor    = turtle.CurrentBackColor().m_color.index;
  const auto frontTexture = turtle.CurrentTexture();
  const auto backTexture  = turtle.CurrentTexture();

  groupNum++;
  *out << "Start_Object_Group " << groupNum << '\n';
  *out << " "
       << "FrontMaterial: " << frontColor << '\n';
  *out << " "
       << "FrontTexture: " << frontTexture << '\n';
  *out << " "
       << "BackMaterial: " << backColor << '\n';
  *out << " "
       << "BackTexture: " << backTexture << '\n';
  *out << '\n';

  *out << " "
       << "object" << '\n';
  *out << " "
       << "  Name: " << objName << '\n';
  *out << " "
       << "  LineWidth: " << Maths::Round(width, precision) << '\n';
  *out << " "
       << "  LineDistance: " << Maths::Round(distance, precision) << '\n';
  *out << " "
       << "  ContactPoint: ";
  OutputVec(*out, contactPoint);
  *out << '\n';
  *out << " "
       << "  Heading: ";
  OutputVec(*out, turtle.CurrentHeading());
  *out << '\n';
  *out << " "
       << "  Left: ";
  OutputVec(*out, turtle.CurrentLeft());
  *out << '\n';
  *out << " "
       << "  Up:";
  OutputVec(*out, turtle.CurrentUp());
  *out << '\n';
  *out << " "
       << "  nargs: " << numArgs << '\n';
  for (auto i = 0U; i < static_cast<uint32_t>(numArgs); i++)
  {
    *out << "  "
         << "    " << args[i] << '\n';
  }
  *out << '\n';

  *out << "End_Object_Group " << groupNum << '\n';
  *out << "\n\n";
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
