
#include "Material.h"

namespace szgrh {

  Material::Material(const std::string &mat_name):
    name(mat_name),
    shininess(0.f),
    ior(0.f),
    dissolve(0.f),
    illum(2) {
  }


} // namespace szgrh