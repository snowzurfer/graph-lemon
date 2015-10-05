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

  std::string ambient_texname;            // map_Ka
  std::string diffuse_texname;            // map_Kd
  std::string specular_texname;           // map_Ks
  std::string specular_highlight_texname; // map_Ns
  std::string bump_texname;               // map_bump, bump
  std::string displacement_texname;       // disp
  std::string alpha_texname;              // map_d


  // Default Ctor
  Material(const std::string &mat_name);

private:
  friend class boost::serialization::access;

  // Allow serialisation
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & name;
    ar & ambient & diffuse & specular & transmittance & emission;
    ar & shininess & ior & disssolve & illum;
    ar & ambient_texname &
      diffuse_texname &
      specular_texname &
      specular_highlight_texname &
      bump_texname &
      displacement_texname &
      alpha_texname    
  }

//typedef struct {
//  std::vector<float> positions;
//  std::vector<float> normals;
//  std::vector<float> texcoords;
//  std::vector<unsigned int> indices;
//  std::vector<int> material_ids; // per-mesh material ID
//} mesh_t;
//
//typedef struct {
//  std::string name;
//  mesh_t mesh;
//} shape_t;


}; // class Material

} // namespace szgrh


#endif