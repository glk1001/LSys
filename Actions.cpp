/* actions.c - actions taken in interpretation of an CurrentLeft-system to
 *  produce PostScript.
 *
 * Copyright (C) 1990, Jonathan P. Leech
 *
 * This software may be freely copied, modified, and redistributed,
 * provided that this copyright notice is preserved on all copies.
 *
 * There is no warranty or other guarantee of fitness for this software,
 * it is provided solely "as is". Bug reports or fixes may be sent
 * to the author, who may or may not act on them as he desires.
 *
 * You may not include this software in a program or other software product
 * without supplying the source, or without informing the end-user that the
 * source is available for no extra charge.
 *
 * If you modify this software, you should include a notice giving the
 * name of the person performing the modification, the date of modification,
 * and the reason for such modification.
 *
 * $Log:	actions.c,v $
 * Revision 1.3  91/03/20  10:36:16  leech
 * Better color support.
 *
 * Revision 1.2  90/10/12  18:48:16  leech
 * First public release.
 *
 */
//static char RCSid[]= "$Id: actions.c,v 1.3 91/03/20 10:36:16 leech Exp Locker: leech $";

#include <cstring>
#include "Consts.h"
#include "debug.h"
#include "Value.h"
#include "Expression.h"
#include "Module.h"
#include "Turtle.h"
#include "Polygon.h"
#include "Generator.h"
#include "Actions.h"

using std::cerr;
using std::endl;


namespace LSys {

  const char drawObjectStartChar= '~';
  const char* const drawObjectStart= "~";


  namespace {

    // The stack is used by { } commands to define polygons.
    // The stack can get quite deep in recursive CurrentLeft-system
    // productions, thus we use a depth of 100 (probably should
    // use a dynamically allocated list).
    int polyptr= -1;
    const int max_polys= 100;
    LSys::Polygon* polystack[max_polys];

    enum { START, DRAWING, POLYGON } State= START;

    // Common Move/draw routine
    void MoveTurtle(Turtle&, int nargs, const float args[maxargs]);

    // Set line width only if changed too much
    void SetLineWidth(Turtle&, Generator&);

    // Set color only if changed
    void SetColor(Turtle&, Generator&);

    // Set texture only if changed
    void SetTexture(Turtle&, Generator&);

    // Add an edge to the current polygon while moving
    void AddPolygonEdge(Turtle&, int nargs, const float args[maxargs]);

  };


  // f(l) Move without drawing
  void Move(
    ConstListIterator<Module>&, Turtle& t,
    Generator& db, int nargs, const float args[maxargs])
  {
    PDebug(PD_INTERPRET, cerr << "Move          " << endl);

    if (State == DRAWING || State == START) {
      MoveTurtle(t, nargs, args);
      db.MoveTo(t);
    } else if (State == POLYGON) {
      AddPolygonEdge(t, nargs, args);
    }
  }


  // z Move half standard distance without drawing
  void MoveHalf(
    ConstListIterator<Module>& li, Turtle& t, Generator& db, int, const float[])
  {
    PDebug(PD_INTERPRET, cerr << "MoveHalf      " << endl);

    const float args[1]= {0.5F * t.DefaultDistance()};
    Move(li, t, db, 1, args);
  }


  // F(l) Move while drawing
  // Fr(l), Fl(l) - Right and CurrentLeft edges respectively
  void Draw(
    ConstListIterator<Module>&, Turtle& t,
    Generator& db, int nargs, const float args[maxargs])
  {
    PDebug(PD_INTERPRET, cerr << "Draw          " << endl);

    if (State == START) {
      db.StartGraphics(t);
      State= DRAWING;
    }

    if (State == DRAWING) {
      MoveTurtle(t, nargs, args);
      db.LineTo(t);
    } else if (State == POLYGON) {
      AddPolygonEdge(t, nargs, args);
    }
  }


  // Z Draw half standard distance while drawing
  void DrawHalf(
    ConstListIterator<Module>& li, Turtle& t, Generator& db, int, const float[])
  {
    PDebug(PD_INTERPRET, cerr << "DrawHalf      " << endl);

    const float args[1]= {0.5F * t.DefaultDistance()};
    Draw(li, t, db, 1, args);
  }


  // -(t) Turn right: negative rotation about Z
  void TurnRight(
    ConstListIterator<Module>&, Turtle& t,
    Generator&, int nargs, const float args[maxargs])
  {
    PDebug(PD_INTERPRET, cerr << "TurnRight     " << endl);

    if (nargs == 0)
      t.Turn(Turtle::negative);
    else
      t.Turn(-Maths::ToRadians(args[0]));
  }


  // +(t) Turn left; positive rotation about Z
  void TurnLeft(
    ConstListIterator<Module>&, Turtle& t,
    Generator&, int nargs, const float args[maxargs])
  {
    PDebug(PD_INTERPRET, cerr << "TurnLeft      " << endl);

    if (nargs == 0)
      t.Turn(Turtle::positive);
    else
      t.Turn(Maths::ToRadians(args[0]));
  }


  // ^(t) Pitch up; negative rotation about Y
  void PitchUp(
    ConstListIterator<Module>&, Turtle& t,
    Generator&, int nargs, const float args[maxargs])
  {
    PDebug(PD_INTERPRET, cerr << "PitchUp       " << endl);

    if (nargs == 0)
      t.Pitch(Turtle::negative);
    else
      t.Pitch(-Maths::ToRadians(args[0]));
  }


  // &(t) Pitch down; positive rotation about Y
  void PitchDown(
    ConstListIterator<Module>&, Turtle& t,
    Generator&, int nargs, const float args[maxargs])
  {
    PDebug(PD_INTERPRET, cerr << "PitchDown     " << endl);

    if (nargs == 0)
      t.Pitch(Turtle::positive);
    else
      t.Pitch(Maths::ToRadians(args[0]));
  }


  // /(t) Roll right; positive rotation about X
  void RollRight(
    ConstListIterator<Module>&, Turtle& t,
    Generator&, int nargs, const float args[maxargs])
  {
    PDebug(PD_INTERPRET, cerr << "RollRight     " << endl);

    if (nargs == 0)
      t.Roll(Turtle::positive);
    else
      t.Roll(Maths::ToRadians(args[0]));
  }


  // \(t) Roll left; negative rotation about X
  void RollLeft(
    ConstListIterator<Module>&, Turtle& t,
    Generator&, int nargs, const float args[maxargs])
  {
    PDebug(PD_INTERPRET, cerr << "RollLeft      " << endl);

    if (nargs == 0)
      t.Roll(Turtle::negative);
    else
      t.Roll(-Maths::ToRadians(args[0]));
  }


  // |	Turn around
  void Reverse(ConstListIterator<Module>&, Turtle& t, Generator&, int, const float[])
  {
    PDebug(PD_INTERPRET, cerr << "Reverse       " << endl);

    t.Reverse();
  }


  // [	Push turtle state
  void Push(ConstListIterator<Module>&, Turtle& t, Generator&, int, const float[])
  {
    PDebug(PD_INTERPRET, cerr << "Push          " << endl);

    t.Push();
  }


  // ]	Pop turtle state
  void Pop(ConstListIterator<Module>& mi, Turtle& t, Generator& db, int, const float[])
  {
    PDebug(PD_INTERPRET, cerr << "Pop           " << endl);

    t.Pop();

    // Look ahead; if the next module is also a Pop, don't do anything.
    // Otherwise, reset the line width, color, and position
    // This is an optimization to handle deep nesting ([[...][...[...]]]),
    // which happens a lot with trees.

    const Module* obj= mi.next();
    if (obj != 0) {
      if (strcmp(obj->name(), "]")) {
        SetLineWidth(t, db);
        SetColor(t, db);
        db.MoveTo(t);
      }
      // Back off one step so the next module is interpreted properly
      mi.previous();
    }
  }


  // $	Roll to horizontal plane (pg. 57)
  void RollHorizontal(
    ConstListIterator<Module>&, Turtle& t,
    Generator&, int, const float[])
  {
    PDebug(PD_INTERPRET, cerr << "RollHorizontal" << endl);

    t.RollHorizontal();
  }


  // {	Start a new polygon
  void StartPolygon(ConstListIterator<Module>&, Turtle& t, Generator& db, int, const float[])
  {
    PDebug(PD_INTERPRET, cerr << "StartPolygon  " << endl);

    if (State == DRAWING)
      db.FlushGraphics(t);

    State= POLYGON;
    polyptr++;
    if (polyptr >= max_polys) {
      cerr << "StartPolygon: polygon stack filled" << endl;
      return;
    }

    polystack[polyptr]= new LSys::Polygon;
  }


  // .	Add a vertex to the current polygon
  void PolygonVertex(ConstListIterator<Module>&, Turtle& t, Generator&, int, const float[])
  {
    PDebug(PD_INTERPRET, cerr << "PolygonVertex " << endl);

    if (State != POLYGON) {
      cerr << "Illegal: Polygon vertex while not in polygon mode\n";
      return;
    }

    if (polyptr < 0) {
      cerr << "PolygonVertex: no polygon being defined" << endl;
      return;
    } else if (polyptr >= max_polys)
      return;     // Already got an error from StartPolygon

    Vector* v= new Vector(t.Location());
    polystack[polyptr]->append(v);
  }


  // G	Move without creating a polygon edge
  // This seems like it should be illegal outside a polygon, but
  // the rose leaf example in the text uses G in this context.
  // Until the behavior is specified, just Move the turtle without
  // other effects.
  void PolygonMove(
    ConstListIterator<Module>&, Turtle& t,
    Generator&, int nargs, const float args[maxargs])
  {
    PDebug(PD_INTERPRET, cerr << "PolygonMove   " << endl);

    //if (State != POLYGON) {
    //	  cerr << "Illegal: Polygon Move-without-draw while not in polygon mode\n";
    //	  return;
    //}

    MoveTurtle(t, nargs, args);
  }


  // }	Close the current polygon
  void EndPolygon(ConstListIterator<Module>&, Turtle& t, Generator& db, int, const float[])
  {
    PDebug(PD_INTERPRET, cerr << "EndPolygon    " << endl);

    if (State != POLYGON || polyptr < 0) {
      cerr << "EndPolygon: no polygon being defined" << endl;
      return;
    }

    if (polyptr >= max_polys) {
      cerr << "EndPolygon: polygon stack too deep, polygon lost" << endl;
      polyptr--;
    } else {
      db.Polygon(t, *polystack[polyptr]);
      delete polystack[polyptr];
      polyptr--;
      // Return to start state if no more polys on stack
      if (polyptr < 0)
        State= START;
    }
  }


  // @md(f) Multiply DefaultDistance by f
  void MultiplyDefaultDistance(
    ConstListIterator<Module>&, Turtle& t,
    Generator&, int nargs, const float args[maxargs])
  {
    PDebug(PD_INTERPRET, cerr << "MultiplyDefaultDist  " << endl);

    if (nargs == 0)
      t.SetDefaultDistance(1.1*t.DefaultDistance());
    else
      t.SetDefaultDistance(args[0]*t.DefaultDistance());
  }


  // @ma(f) Multiply DefaultTurnAngle by f
  void MultiplyDefaultTurnAngle(
    ConstListIterator<Module>&, Turtle& t,
    Generator&, int nargs, const float args[maxargs])
  {
    PDebug(PD_INTERPRET, cerr << "MultiplyDefaultTurnAngle  " << endl);

    if (nargs == 0)
      t.SetDefaultTurnAngle(1.1*t.DefaultTurnAngle());
    else
      t.SetDefaultTurnAngle(args[0]*t.DefaultTurnAngle());
  }


  // @mw(f) Multiply width by f
  void MultiplyWidth(
    ConstListIterator<Module>&, Turtle& t,
    Generator& db, int nargs, const float args[maxargs])
  {
    PDebug(PD_INTERPRET, cerr << "MultiplyWidth  " << endl);

    if (nargs == 0)
      t.SetWidth(1.4*t.CurrentWidth());
    else
      t.SetWidth(args[0]*t.CurrentWidth());

    SetLineWidth(t, db);
  }


  // !(d) Set line width
  void ChangeWidth(
    ConstListIterator<Module>&, Turtle& t,
    Generator& db, int nargs, const float args[maxargs])
  {
    PDebug(PD_INTERPRET, cerr << "ChangeWidth     " << endl);

    if (nargs == 0)
      t.SetWidth();
    else
      t.SetWidth(args[0]);

    SetLineWidth(t, db);
  }


  // '	Increment color index
  // '(n) Set color index
  // '(r,g,b) Set RGB color
  void ChangeColor(
    ConstListIterator<Module>&, Turtle& t,
    Generator& db, int nargs, const float args[maxargs])
  {
    PDebug(PD_INTERPRET, cerr << "ChangeColor   " << endl);

    if (nargs >= 3)
      t.SetColor(Vector(args[0], args[1], args[2]));
    if (nargs >= 2)
      t.SetColor(args[0], args[1]);
    else if (nargs > 0)
      t.SetColor((int)args[0]);
    else
      t.IncrementColor();

    SetColor(t, db);
  }


  // @Tx(n)	Change texture index
  void ChangeTexture(
    ConstListIterator<Module>&, Turtle& t,
    Generator& db, int, const float args[maxargs])
  {
    PDebug(PD_INTERPRET, cerr << "ChangeTexture   " << endl);

    t.SetTexture((int)args[0]);

    SetTexture(t, db);
  }


  // ~	Draw the following object at the turtle's position and frame
  // Needs a standardized object file format for this.
  // Should leave graphics mode before drawing object.
  void DrawObject(
    ConstListIterator<Module>& mi, Turtle& t,
    Generator& db, int nargs, const float args[])
  {
    PDebug(PD_INTERPRET, cerr << "DrawObject    " << endl);

    //  Module* obj= mi.next();
    const Module* obj= mi.current();
    if (obj != 0) db.DrawObject(t, *obj, nargs, args);
  }


  void GeneralisedCylinderStart(
    ConstListIterator<Module>& mi, Turtle& t, Generator& db, int, const float[])
  {
  }


  void GeneralisedCylinderControlPoint(
    ConstListIterator<Module>& mi, Turtle& t, Generator& db, int, const float[])
  {
  }


  void GeneralisedCylinderEnd(
    ConstListIterator<Module>& mi, Turtle& t, Generator& db, int, const float[])
  {
  }


  void GeneralisedCylinderTangents(
    ConstListIterator<Module>& mi, Turtle& t, Generator& db, int, const float[])
  {
  }


  void GeneralisedCylinderTangentLengths(
    ConstListIterator<Module>& mi, Turtle& t, Generator& db, int, const float[])
  {
  }


  // %	Truncate a branch
  // E.g., ignore all modules up to the next ]
  // Note that this code is cloned from right-context
  // matching in Production::matches()
  void CutBranch(ConstListIterator<Module>& mi, Turtle&, Generator&, int, const float[])
  {
    PDebug(PD_INTERPRET, cerr << "CutBranch     " << endl);

    // Must find a matching ]; skip anything else including
    //	bracketed substrings.
    const Module *obj;
    int brackets;
    for (brackets= 0, obj= mi.next(); obj; obj= mi.next()) {
      if (obj->name() == RBRACKET) {
        if (brackets-- == 0)
          break;
      } else if (obj->name() == LBRACKET)
        brackets++;
    }

    // Back off one step so the pop itself is handled by interpret()
    if (obj != 0)
      mi.previous();
  }


  // t	Enable/disable tropism corrections after each Move
  // t(x,y,z,e)	- enable tropism; tropism vector is T= (x,y,z),
  // e is `suspectibility parameter', preferably between 0 and 1.
  // t(0)		- disable tropism
  // t(1)		- reenable tropism with last (T,e) parameters

  static void tropism_error()
  {
    cerr << "Tropism: expect arguments (x,y,z,e) or (e)." << endl;
  }


  void Tropism(
    ConstListIterator<Module>&, Turtle& t,
    Generator&, int nargs, const float args[maxargs])
  {
    if (nargs == 0) {
      tropism_error();
      return;
    }

    if (nargs == 1) {
      // Only one parameter; disable tropism if it equals 0,
      // otherwise enable tropism with the specified parameter.

      float e= args[0];
      if (e == 0)
        t.DisableTropism();
      else if (e == 1) {
        t.SetTropismVector(e);
        t.EnableTropism();
      }
      return;
    }

    if (nargs < 4) {
      tropism_error();
      return;
    }

    // Construct the tropism vector
    t.SetTropismVector(Vector(args[0], args[1], args[2]));
    t.SetTropismVector(args[3]);
    t.EnableTropism();
  }


  namespace {

    void MoveTurtle(Turtle& t, int nargs, const float args[maxargs])
    {
      if (nargs == 0)
        t.Move();
      else
        t.Move(args[0]);
    }


    void AddPolygonEdge(Turtle& t, int nargs, const float args[maxargs])
    {
      // Add an edge to the current polygon
      ConstPolygonIterator pi(*polystack[polyptr]);
      const Vector* v= pi.last();

      // See if the starting point needs to be added (only if
      // it's different from the last point defined in
      // the polygon, or there are no points defined in the polygon
      // yet).
      Vector* p= new Vector(t.Location());
      if ((v == 0) || (*v != *p)) {
        PDebug(PD_INTERPRET, cerr << "AddPolygonEdge: adding first vertex " << *p << endl);
        polystack[polyptr]->append(p);
      }

      // Move and add the ending point to the polygon.
      MoveTurtle(t, nargs, args);
      PDebug(PD_INTERPRET, cerr << "AddPolygonEdge: adding last vertex  " << t.Location() << endl);
      polystack[polyptr]->append(new Vector(t.Location()));
    }


    void SetLineWidth(Turtle& t, Generator& db)
    {
      const float epsilon= 1e-6;
      static float lastlinewidth= -1;

      // Don't bother changing line width if 'small enough'.
      // This is an optimization to handle e.g. !(w)[!(w/2)F][!(w/2)F]
      //sort of cases, which happen a lot with trees.
      if (std::fabs(t.CurrentWidth() - lastlinewidth) < epsilon)
        return;

      if (State == DRAWING) {
        db.FlushGraphics(t);
        State= START;
      }

      db.SetWidth(t);
      lastlinewidth= t.CurrentWidth();
    }


    void SetColor(Turtle& t, Generator& db)
    {
      static Color lastcolor(-1);

      // Don't change color if not needed, again an optimization
      if (t.CurrentColor() == lastcolor)
        return;

      if (State == DRAWING) {
        db.FlushGraphics(t);
        State= START;
      }

      db.SetColor(t);
      lastcolor= t.CurrentColor();
    }


    void SetTexture(Turtle& t, Generator& db)
    {
      static int lastTexture(-1);

      // Don't change txture if not needed, again an optimization
      if (t.CurrentTexture() == lastTexture)
        return;

      if (State == DRAWING) {
        db.FlushGraphics(t);
        State= START;
      }

      db.SetTexture(t);
      lastTexture= t.CurrentTexture();
    }


  };


};
