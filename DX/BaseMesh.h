// Mesh.h
#ifndef _BASEMESH_H_
#define _BASEMESH_H_

#include <d3d11.h>
#include <directxmath.h>
#include <string>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>

using namespace DirectX;

	typedef struct 
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
  
    friend class boost::serialization::access;

    // Allow serialisation
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
      ar & x & y & z;
      ar & tu & tv;
      ar & nx & ny & nz;
    }
	} ModelType;

class BaseMesh
{
protected:

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};



public:
	BaseMesh();
	~BaseMesh();

	virtual void SendData(ID3D11DeviceContext*);
	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

	virtual void InitBuffers(ID3D11Device*);

  inline void set_mat_id(int id) {
    mat_id_ = id;
  }

  inline void set_vertices(const std::vector<ModelType> &val) {
    vertices_ = val;
  }
  
  inline void set_indices(const std::vector<unsigned int> &val) {
    indices_ = val;
  }
protected:
	void LoadTexture(ID3D11Device*, WCHAR*);

	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	std::wstring m_Texture;

  // List of vertices, used for deserialisation
  std::vector<ModelType> vertices_;
  // List of indices
  std::vector<unsigned int> indices_;
  // The id of the material to be used; it is the index
  // in the array of materials of the model.
  int mat_id_;
 
  friend class boost::serialization::access;

  // Allow serialisation
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & vertices_;
    ar & indices_;
    ar & mat_id_;
  }
};

#endif