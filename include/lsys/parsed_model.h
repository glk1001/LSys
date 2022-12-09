#pragma once

#include "l_sys_model.h"

#include <memory>
#include <string>

namespace LSYS
{

struct Properties
{
  std::string inputFilename = "";
  int maxGen                = -1;
  float turnAngle           = -1.0F;
  float lineWidth           = -1.0F;
  float lineDistance        = -1.0F;
};

[[nodiscard]] auto GetParsedModel(const Properties& properties) -> std::unique_ptr<LSysModel>;

// Add default properties from the symbol table, provided they're
// not overridden by existing properties.
[[nodiscard]] auto GetFinalProperties(const SymbolTable<Value>& symbolTable,
                                      const Properties& initialProperties,
                                      bool setDefaults = true) -> Properties;

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

} // namespace LSYS
