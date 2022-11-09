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
 * $Log: actions.h,v $
 * Revision 1.2  90/10/12  18:48:16  leech
 * First public release.
 *
 */

#pragma once

#include "Consts.h"
#include "Generator.h"
#include "Module.h"
#include "Turtle.h"

namespace LSys
{

inline constexpr char DRAW_OBJECT_START_CHAR   = '~';
inline constexpr const char* DRAW_OBJECT_START = "~";

// Default interpretation functions; this header should be included
// after parser.h and Turtle.h

#define ACTION(name) \
  void name(ConstListIterator<Module>& moduleIter, \
            Turtle& turtle, \
            Generator& generator, \
            int numArgs, \
            const ArgsArray& args) noexcept

// Pointer to an interpretation function
// TODO(glk) - How to make this a 'using'?
typedef ACTION((*Actionfunc));

// Canned interpretation functions
auto Prelude(Turtle& turtle) noexcept -> void;
auto Postscript(Turtle& turtle) noexcept -> void;

auto Move(const ConstListIterator<Module>& moduleIter,
          Turtle& turtle,
          Generator& generator,
          int numArgs,
          const ArgsArray& args) noexcept -> void;
auto MoveHalf(const ConstListIterator<Module>& moduleIter,
              Turtle& turtle,
              Generator& generator,
              int numArgs,
              const ArgsArray& args) noexcept -> void;
auto Draw(const ConstListIterator<Module>& moduleIter,
          Turtle& turtle,
          Generator& generator,
          int numArgs,
          const ArgsArray& args) noexcept -> void;
auto DrawHalf(ConstListIterator<Module>& moduleIter,
              Turtle& turtle,
              Generator& generator,
              int numArgs,
              const ArgsArray& args) noexcept -> void;

auto DrawObject(const ConstListIterator<Module>& moduleIter,
                const Turtle& turtle,
                Generator& generator,
                int numArgs,
                const ArgsArray& args) noexcept -> void;
auto GeneralisedCylinderStart(const ConstListIterator<Module>& moduleIter,
                              const Turtle& turtle,
                              const Generator& generator,
                              int numArgs,
                              const ArgsArray& args) noexcept -> void;
auto GeneralisedCylinderControlPoint(const ConstListIterator<Module>& moduleIter,
                                     const Turtle& turtle,
                                     const Generator& generator,
                                     int numArgs,
                                     const ArgsArray& args) noexcept -> void;
auto GeneralisedCylinderEnd(const ConstListIterator<Module>& moduleIter,
                            const Turtle& turtle,
                            const Generator& generator,
                            int numArgs,
                            const ArgsArray& args) noexcept -> void;
auto GeneralisedCylinderTangents(const ConstListIterator<Module>& moduleIter,
                                 const Turtle& turtle,
                                 const Generator& generator,
                                 int numArgs,
                                 const ArgsArray& args) noexcept -> void;
auto GeneralisedCylinderTangentLengths(const ConstListIterator<Module>& moduleIter,
                                       const Turtle& turtle,
                                       const Generator& generator,
                                       int numArgs,
                                       const ArgsArray& args) noexcept -> void;

auto TurnRight(const ConstListIterator<Module>& moduleIter,
               Turtle& turtle,
               const Generator& generator,
               int numArgs,
               const ArgsArray& args) noexcept -> void;
auto TurnLeft(const ConstListIterator<Module>& moduleIter,
              Turtle& turtle,
              const Generator& generator,
              int numArgs,
              const ArgsArray& args) noexcept -> void;
auto PitchUp(const ConstListIterator<Module>& moduleIter,
             Turtle& turtle,
             const Generator& generator,
             int numArgs,
             const ArgsArray& args) noexcept -> void;
auto PitchDown(const ConstListIterator<Module>& moduleIter,
               Turtle& turtle,
               const Generator& generator,
               int numArgs,
               const ArgsArray& args) noexcept -> void;
auto RollRight(const ConstListIterator<Module>& moduleIter,
               Turtle& turtle,
               const Generator& generator,
               int numArgs,
               const ArgsArray& args) noexcept -> void;
auto RollLeft(const ConstListIterator<Module>& moduleIter,
              Turtle& turtle,
              const Generator& generator,
              int numArgs,
              const ArgsArray& args) noexcept -> void;
auto Reverse(const ConstListIterator<Module>& moduleIter,
             Turtle& turtle,
             const Generator& generator,
             int numArgs,
             const ArgsArray& args) noexcept -> void;
auto RollHorizontal(const ConstListIterator<Module>& moduleIter,
                    Turtle& turtle,
                    const Generator& generator,
                    int numArgs,
                    const ArgsArray& args) noexcept -> void;

auto Push(const ConstListIterator<Module>& moduleIter,
          Turtle& turtle,
          const Generator& generator,
          int numArgs,
          const ArgsArray& args) noexcept -> void;
auto Pop(ConstListIterator<Module>& moduleIter,
         Turtle& turtle,
         Generator& generator,
         int numArgs,
         const ArgsArray& args) noexcept -> void;
auto CutBranch(ConstListIterator<Module>& moduleIter,
               const Turtle& turtle,
               const Generator& generator,
               int numArgs,
               const ArgsArray& args) noexcept -> void;

auto MultiplyDefaultDistance(const ConstListIterator<Module>& moduleIter,
                             Turtle& turtle,
                             const Generator& generator,
                             int numArgs,
                             const ArgsArray& args) noexcept -> void;
auto MultiplyDefaultTurnAngle(const ConstListIterator<Module>& moduleIter,
                              Turtle& turtle,
                              const Generator& generator,
                              int numArgs,
                              const ArgsArray& args) noexcept -> void;
auto MultiplyWidth(const ConstListIterator<Module>& moduleIter,
                   Turtle& turtle,
                   Generator& generator,
                   int numArgs,
                   const ArgsArray& args) noexcept -> void;
auto ChangeWidth(const ConstListIterator<Module>& moduleIter,
                 Turtle& turtle,
                 Generator& generator,
                 int numArgs,
                 const ArgsArray& args) noexcept -> void;
auto ChangeColor(const ConstListIterator<Module>& moduleIter,
                 Turtle& turtle,
                 Generator& generator,
                 int numArgs,
                 const ArgsArray& args) noexcept -> void;
auto ChangeTexture(const ConstListIterator<Module>& moduleIter,
                   Turtle& turtle,
                   Generator& generator,
                   int numArgs,
                   const ArgsArray& args) noexcept -> void;

auto StartPolygon(const ConstListIterator<Module>& moduleIter,
                  const Turtle& turtle,
                  Generator& generator,
                  int numArgs,
                  const ArgsArray& args) noexcept -> void;
auto PolygonVertex(const ConstListIterator<Module>& moduleIter,
                   const Turtle& turtle,
                   const Generator& generator,
                   int numArgs,
                   const ArgsArray& args) noexcept -> void;
auto PolygonMove(const ConstListIterator<Module>& moduleIter,
                 Turtle& turtle,
                 const Generator& generator,
                 int numArgs,
                 const ArgsArray& args) noexcept -> void;
auto EndPolygon(const ConstListIterator<Module>& moduleIter,
                const Turtle& turtle,
                Generator& generator,
                int numArgs,
                const ArgsArray& args) noexcept -> void;

auto Flower(const ConstListIterator<Module>& moduleIter,
            const Turtle& turtle,
            const Generator& generator,
            int numArgs,
            const ArgsArray& args) noexcept -> void;
auto Leaf(const ConstListIterator<Module>& moduleIter,
          const Turtle& turtle,
          const Generator& generator,
          int numArgs,
          const ArgsArray& args) noexcept -> void;
auto Internode(const ConstListIterator<Module>& moduleIter,
               const Turtle& turtle,
               const Generator& generator,
               int numArgs,
               const ArgsArray& args) noexcept -> void;
auto FloweringApex(const ConstListIterator<Module>& moduleIter,
                   const Turtle& turtle,
                   const Generator& generator,
                   int numArgs,
                   const ArgsArray& args) noexcept -> void;

auto Tropism(const ConstListIterator<Module>& moduleIter,
             Turtle& turtle,
             const Generator& generator,
             int numArgs,
             const ArgsArray& args) noexcept -> void;

} // namespace LSys
