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
#include "abertay_framework.h"

namespace sz {

class Material {
public:
  // Name of material
  std::string name;
  UInt32 name_crc;

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

  std::string ambient_texname;            // map_Ka
  std::string diffuse_texname;            // map_Kd
  std::string specular_texname;           // map_Ks
  std::string specular_highlight_texname; // map_Ns
  std::string bump_texname;               // map_bump, bump
  std::string displacement_texname;       // disp
  std::string alpha_texname;              // map_d
  UInt32 ambient_texname_crc;            // map_Ka
  UInt32 diffuse_texname_crc;            // map_Kd
  UInt32 specular_texname_crc;           // map_Ks
  UInt32 specular_highlight_texname_crc; // map_Ns
  UInt32 bump_texname_crc;               // map_bump, bump
  UInt32 displacement_texname_crc;       // disp
  UInt32 alpha_texname_crc;              // map_d

  std::string shader_name;

  // Default Ctor
  Material() {};
  Material(const std::string &mat_name);

private:
  friend class boost::serialization::access;

  // Allow serialisation
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & name & name_crc;
    ar & ambient & diffuse & specular & transmittance & emission;
    ar & shininess & ior & dissolve & illum;
    ar & ambient_texname &
      diffuse_texname &
      specular_texname &
      specular_highlight_texname &
      bump_texname &
      displacement_texname &
      alpha_texname;
    ar &              // map_d
      ambient_texname_crc &            // map_Ka
      diffuse_texname_crc &            // map_Kd
      specular_texname_crc &           // map_Ks
      specular_highlight_texname_crc & // map_Ns
      bump_texname_crc &               // map_bump, bump
      displacement_texname_crc &       // disp
      alpha_texname_crc;         // map_d
  }

}; // class Material

} // namespace sz


#endif