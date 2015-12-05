#ifndef _MODEL_H_
#define _MODEL_H_

#include "BaseMesh.h"
#include <vector>
#include "Material.h"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>
#include <d3d11.h>
#include "buffer_resource_manager.h"
#include "shader_resource_manager.h"
#include "abertay_framework.h"
#include <utility>
#include <tuple>
#include <vector>
#include <map>

namespace sz {
  class Material;
  typedef std::map<UInt32, std::pair<const sz::Material *, std::vector<BaseMesh *>>>
    MeshesMatMap;
  typedef std::pair<const sz::Material *, std::vector<BaseMesh *>>
    MatMeshPair;
}

using namespace DirectX;

class Model 
{
public:
  typedef struct 
  {
    float x, y, z;
    float tu, tv;
    float nx, ny, nz;
    float tx, ty, tz, tw;

    friend class boost::serialization::access;

    // Allow serialisation
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
      ar & x & y & z;
      ar & tu & tv;
      ar & nx & ny & nz;
      ar & tx & ty & tz & tw;
    }
  } ModelType;

  Model() {};
  Model(const std::string &model_filename);
  ~Model();

  void Init(ID3D11Device* device, ID3D11DeviceContext *dev_context,
    HWND hwnd, 
    sz::ConstBufManager &buf_man, unsigned int lights_num,
    sz::ShaderManager &shad_man);
  
  void InitBuffers(ID3D11Device* device, ID3D11DeviceContext *dev_context);

  // Set the vertex and index buffers
  void SendData(ID3D11DeviceContext* dev_context, bool tessellate = false);

  // List of mesh componing the model
  std::vector<BaseMesh> meshes_;
  // List of materials used by the model
  std::vector<sz::Material> materials_;

  // Name of the folder of the model
  std::string model_name_;

  ModelType* m_model;

  inline void set_vertices_num(size_t n) {
    vertices_num_ = n;
  }
  inline void set_indices_num(size_t n) {
    indices_num_ = n;
  }
  inline size_t vertices_num() const {
    return vertices_num_;
  }
  inline size_t indices_num() const {
    return indices_num_;
  }

  
  inline sz::MeshesMatMap &meshes_by_material() {
    return meshes_by_material_;
  }

private:
  // Used to batch render by material
  sz::MeshesMatMap meshes_by_material_;

  size_t vertices_num_, indices_num_;

  friend class boost::serialization::access;

  // Allow serialisation
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & meshes_;
    ar & materials_;
    ar & model_name_;
    ar & vertices_num_ & indices_num_;
  }

  // Load in all the textures needed by the materials
  void LoadTextures_(ID3D11Device* device, ID3D11DeviceContext *dev_context,
    HWND hwnd);
  void LoadShaders_(ID3D11Device* device, HWND hwnd,
    sz::ConstBufManager &buf_man, unsigned int lights_num,
    sz::ShaderManager &shad_man);
  void AddMeshesAndMaterials(std::vector<BaseMesh> &meshes,
    const std::vector<sz::Material> &materials);

  ID3D11Buffer *vertex_buf_, *index_buf_;
};

#endif