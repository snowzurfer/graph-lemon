#ifndef _MATERIAL_H
#define _MATERIAL_H

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/array.hpp>

namespace szgrh {

class Material {
public:
  // Name of material
  std::string name;

  float ambient[3];
  float diffuse[3];
  float specular[3];
  float transmittance[3];
  float emission[3];
  float shininess;
  float ior;      // index of refraction
  float dissolve; // 1 == opaque; 0 == fully transparent
  // illumination model (see http://www.fileformat.info/format/material/)
  int illum;

  std::wstring ambient_texname;            // map_Ka
  std::wstring diffuse_texname;            // map_Kd
  std::wstring specular_texname;           // map_Ks
  std::wstring specular_highlight_texname; // map_Ns
  std::wstring bump_texname;               // map_bump, bump
  std::wstring displacement_texname;       // disp
  std::wstring alpha_texname;              // map_d

  std::string shader_name;

  // Default Ctor
  Material() {};
  Material(const std::string &mat_name);

private:
  friend class boost::serialization::access;

  // Allow serialisation
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & name;
    ar & ambient & diffuse & specular & transmittance & emission;
    ar & shininess & ior & dissolve & illum;
    ar & ambient_texname &
      diffuse_texname &
      specular_texname &
      specular_highlight_texname &
      bump_texname &
      displacement_texname &
      alpha_texname;
  }

}; // class Material

} // namespace szgrh


#endif