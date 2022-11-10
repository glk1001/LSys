/*
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
#include "IGenerator.h"
#include "Module.h"
#include "Turtle.h"

#include <functional>

namespace LSys
{

inline constexpr char DRAW_OBJECT_START_CHAR   = '~';
inline constexpr const char* DRAW_OBJECT_START = "~";

using ActionFunc = std::function<void(ConstListIterator<Module>& moduleIter,
                                      Turtle& turtle,
                                      IGenerator& generator,
                                      int numArgs,
                                      const ArgsArray& args)>;

// Canned interpretation functions
auto Prelude(Turtle& turtle) noexcept -> void;
auto Postscript(Turtle& turtle) noexcept -> void;

auto Move(ConstListIterator<Module>& moduleIter,
          Turtle& turtle,
          IGenerator& generator,
          int numArgs,
          const ArgsArray& args) noexcept -> void;
auto MoveHalf(ConstListIterator<Module>& moduleIter,
              Turtle& turtle,
              IGenerator& generator,
              int numArgs,
              const ArgsArray& args) noexcept -> void;

auto Draw(ConstListIterator<Module>& moduleIter,
          Turtle& turtle,
          IGenerator& generator,
          int numArgs,
          const ArgsArray& args) noexcept -> void;
auto DrawHalf(ConstListIterator<Module>& moduleIter,
              Turtle& turtle,
              IGenerator& generator,
              int numArgs,
              const ArgsArray& args) noexcept -> void;

auto DrawObject(ConstListIterator<Module>& moduleIter,
                const Turtle& turtle,
                IGenerator& generator,
                int numArgs,
                const ArgsArray& args) noexcept -> void;

auto GeneralisedCylinderStart(ConstListIterator<Module>& moduleIter,
                              Turtle& turtle,
                              IGenerator& generator,
                              int numArgs,
                              const ArgsArray& args) noexcept -> void;
auto GeneralisedCylinderControlPoint(ConstListIterator<Module>& moduleIter,
                                     Turtle& turtle,
                                     IGenerator& generator,
                                     int numArgs,
                                     const ArgsArray& args) noexcept -> void;
auto GeneralisedCylinderEnd(ConstListIterator<Module>& moduleIter,
                            Turtle& turtle,
                            IGenerator& generator,
                            int numArgs,
                            const ArgsArray& args) noexcept -> void;
auto GeneralisedCylinderTangents(ConstListIterator<Module>& moduleIter,
                                 Turtle& turtle,
                                 IGenerator& generator,
                                 int numArgs,
                                 const ArgsArray& args) noexcept -> void;
auto GeneralisedCylinderTangentLengths(ConstListIterator<Module>& moduleIter,
                                       Turtle& turtle,
                                       IGenerator& generator,
                                       int numArgs,
                                       const ArgsArray& args) noexcept -> void;

auto TurnRight(ConstListIterator<Module>& moduleIter,
               Turtle& turtle,
               const IGenerator& generator,
               int numArgs,
               const ArgsArray& args) noexcept -> void;
auto TurnLeft(ConstListIterator<Module>& moduleIter,
              Turtle& turtle,
              const IGenerator& generator,
              int numArgs,
              const ArgsArray& args) noexcept -> void;
auto PitchUp(ConstListIterator<Module>& moduleIter,
             Turtle& turtle,
             const IGenerator& generator,
             int numArgs,
             const ArgsArray& args) noexcept -> void;
auto PitchDown(ConstListIterator<Module>& moduleIter,
               Turtle& turtle,
               const IGenerator& generator,
               int numArgs,
               const ArgsArray& args) noexcept -> void;
auto RollRight(ConstListIterator<Module>& moduleIter,
               Turtle& turtle,
               const IGenerator& generator,
               int numArgs,
               const ArgsArray& args) noexcept -> void;
auto RollLeft(ConstListIterator<Module>& moduleIter,
              Turtle& turtle,
              const IGenerator& generator,
              int numArgs,
              const ArgsArray& args) noexcept -> void;
auto Reverse(ConstListIterator<Module>& moduleIter,
             Turtle& turtle,
             const IGenerator& generator,
             int numArgs,
             const ArgsArray& args) noexcept -> void;
auto RollHorizontal(ConstListIterator<Module>& moduleIter,
                    Turtle& turtle,
                    const IGenerator& generator,
                    int numArgs,
                    const ArgsArray& args) noexcept -> void;

auto Push(ConstListIterator<Module>& moduleIter,
          Turtle& turtle,
          const IGenerator& generator,
          int numArgs,
          const ArgsArray& args) noexcept -> void;
auto Pop(ConstListIterator<Module>& moduleIter,
         Turtle& turtle,
         IGenerator& generator,
         int numArgs,
         const ArgsArray& args) noexcept -> void;
auto CutBranch(ConstListIterator<Module>& moduleIter,
               const Turtle& turtle,
               const IGenerator& generator,
               int numArgs,
               const ArgsArray& args) noexcept -> void;

auto MultiplyDefaultDistance(ConstListIterator<Module>& moduleIter,
                             Turtle& turtle,
                             const IGenerator& generator,
                             int numArgs,
                             const ArgsArray& args) noexcept -> void;
auto MultiplyDefaultTurnAngle(ConstListIterator<Module>& moduleIter,
                              Turtle& turtle,
                              const IGenerator& generator,
                              int numArgs,
                              const ArgsArray& args) noexcept -> void;
auto MultiplyWidth(ConstListIterator<Module>& moduleIter,
                   Turtle& turtle,
                   IGenerator& generator,
                   int numArgs,
                   const ArgsArray& args) noexcept -> void;
auto ChangeWidth(ConstListIterator<Module>& moduleIter,
                 Turtle& turtle,
                 IGenerator& generator,
                 int numArgs,
                 const ArgsArray& args) noexcept -> void;

auto ChangeColor(ConstListIterator<Module>& moduleIter,
                 Turtle& turtle,
                 IGenerator& generator,
                 int numArgs,
                 const ArgsArray& args) noexcept -> void;
auto ChangeTexture(ConstListIterator<Module>& moduleIter,
                   Turtle& turtle,
                   IGenerator& generator,
                   int numArgs,
                   const ArgsArray& args) noexcept -> void;

auto StartPolygon(ConstListIterator<Module>& moduleIter,
                  const Turtle& turtle,
                  IGenerator& generator,
                  int numArgs,
                  const ArgsArray& args) noexcept -> void;
auto PolygonVertex(ConstListIterator<Module>& moduleIter,
                   const Turtle& turtle,
                   const IGenerator& generator,
                   int numArgs,
                   const ArgsArray& args) noexcept -> void;
auto PolygonMove(ConstListIterator<Module>& moduleIter,
                 Turtle& turtle,
                 const IGenerator& generator,
                 int numArgs,
                 const ArgsArray& args) noexcept -> void;
auto EndPolygon(ConstListIterator<Module>& moduleIter,
                const Turtle& turtle,
                IGenerator& generator,
                int numArgs,
                const ArgsArray& args) noexcept -> void;

auto Tropism(ConstListIterator<Module>& moduleIter,
             Turtle& turtle,
             const IGenerator& generator,
             int numArgs,
             const ArgsArray& args) noexcept -> void;

auto Flower(ConstListIterator<Module>& moduleIter,
            const Turtle& turtle,
            const IGenerator& generator,
            int numArgs,
            const ArgsArray& args) noexcept -> void;
auto Leaf(ConstListIterator<Module>& moduleIter,
          const Turtle& turtle,
          const IGenerator& generator,
          int numArgs,
          const ArgsArray& args) noexcept -> void;
auto Internode(ConstListIterator<Module>& moduleIter,
               const Turtle& turtle,
               const IGenerator& generator,
               int numArgs,
               const ArgsArray& args) noexcept -> void;
auto FloweringApex(ConstListIterator<Module>& moduleIter,
                   const Turtle& turtle,
                   const IGenerator& generator,
                   int numArgs,
                   const ArgsArray& args) noexcept -> void;

} // namespace LSys
