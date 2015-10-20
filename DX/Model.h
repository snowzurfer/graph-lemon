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
	Model(ID3D11Device* device, WCHAR *model_filename, const std::string &model_name);
	~Model();

  void Init(ID3D11Device* device, HWND hwnd, 
    szgrh::ConstBufManager &buf_man, unsigned int lights_num,
    szgrh::ShaderManager &shad_man);
	
  void InitBuffers(ID3D11Device* device);


	void LoadModel(WCHAR* filename);
 

  // List of mesh componing the model
  std::vector<BaseMesh> meshes_;
  // List of materials used by the model
  std::vector<szgrh::Material> materials_;

  // Name of the folder of the model
  std::string model_name_;

	ModelType* m_model;
 
private:
  friend class boost::serialization::access;

  // Allow serialisation
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & meshes_;
    ar & materials_;
    ar & model_name_;
  }
  
  // Load in all the textures needed by the materials
  void LoadTextures_(ID3D11Device* device, HWND hwnd);
  void LoadShaders_(ID3D11Device* device, HWND hwnd,
    szgrh::ConstBufManager &buf_man, unsigned int lights_num,
    szgrh::ShaderManager &shad_man);
};

#endif