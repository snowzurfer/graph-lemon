#ifndef _MODEL_H_
#define _MODEL_H_

#include "BaseMesh.h"
#include "TokenStream.h"
#include <vector>
#include "Material.h"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

using namespace DirectX;

class Model 
{
public:
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

  Model() {};
	Model(ID3D11Device* device, WCHAR *model_filename, const std::string &model_name);
	~Model();

  void Init(ID3D11Device* device, WCHAR *model_filename, const std::string &model_name);
	
  void InitBuffers(ID3D11Device* device);


	void LoadModel(WCHAR* filename);
 
  // Load in all the textures needed by the materials
  //void LoadTextures_(ID3D11Device* device);

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
};

#endif