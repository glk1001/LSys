#ifndef _Actions_h
  #define _Actions_h

/* actions.h - external definition of actions used in interpretation
 *  of an L-system.
 *
 * $Id: actions.h,v 1.2 90/10/12 18:48:16 leech Exp Locker: leech $
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
 * $Log:	actions.h,v $
 * Revision 1.2  90/10/12  18:48:16  leech
 * First public release.
 *
 */

  namespace LSys {

    const int maxargs= 10;
    extern const char drawObjectStartChar;
    extern const char* const drawObjectStart;

    // Default interpretation functions; this header should be included
    // after parser.h and Turtle.h

    #define ACTION(name) \
      void name(ConstListIterator<Module>&, Turtle&, \
      Generator&, int nargs, const float args[maxargs])

    // Pointer to an interpretation function
    typedef ACTION((*Actionfunc));

    // Canned interpretation functions
    extern void Prelude(Turtle&);
    extern void Postscript(Turtle&);

    extern ACTION(Move);
    extern ACTION(MoveHalf);
    extern ACTION(Draw);
    extern ACTION(DrawHalf);

    extern ACTION(DrawObject);
    extern ACTION(GeneralisedCylinderStart);
    extern ACTION(GeneralisedCylinderControlPoint);
    extern ACTION(GeneralisedCylinderEnd);
    extern ACTION(GeneralisedCylinderTangents);
    extern ACTION(GeneralisedCylinderTangentLengths);

    extern ACTION(TurnRight);
    extern ACTION(TurnLeft);
    extern ACTION(PitchUp);
    extern ACTION(PitchDown);
    extern ACTION(RollRight);
    extern ACTION(RollLeft);
    extern ACTION(Reverse);
    extern ACTION(RollHorizontal);

    extern ACTION(Push);
    extern ACTION(Pop);
    extern ACTION(CutBranch);

    extern ACTION(MultiplyDefaultDistance);
    extern ACTION(MultiplyDefaultTurnAngle);
    extern ACTION(MultiplyWidth);
    extern ACTION(ChangeWidth);
    extern ACTION(ChangeColor);
    extern ACTION(ChangeTexture);

    extern ACTION(StartPolygon);
    extern ACTION(PolygonVertex);
    extern ACTION(PolygonMove);
    extern ACTION(EndPolygon);

    extern ACTION(Flower);
    extern ACTION(Leaf);
    extern ACTION(Internode);
    extern ACTION(FloweringApex);

    extern ACTION(Tropism);

  };

#endif

