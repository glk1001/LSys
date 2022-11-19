#pragma once

#include "l_sys_model.h"

#include <memory>
#include <string>

namespace L_SYSTEM
{

struct Properties
{
  std::string inputFilename = "";
  int maxGen                = -1;
  float turnAngle           = -1.0F;
  float lineWidth           = -1.0F;
  float lineDistance        = -1.0F;
  int64_t seed              = -1L; // Seed for random numbers: -1 means use time
};

[[nodiscard]] auto GetParsedModel(const Properties& properties) -> std::unique_ptr<LSysModel>;

// Add default properties from the symbol table, provided they're
// not overridden by existing properties.
[[nodiscard]] auto GetFinalProperties(const SymbolTable<Value>& symbolTable,
                                      const Properties& properties) -> Properties;

struct Point3dFlt
{
  float x;
  float y;
  float z;
};
struct BoundingBox3d
{
  Point3dFlt min;
  Point3dFlt max;
};

[[nodiscard]] auto GetBoundingBox3d(const std::string& filename) -> BoundingBox3d;

} // namespace L_SYSTEM
