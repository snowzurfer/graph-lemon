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

  struct VertexType
  {
    XMFLOAT3 position;
    XMFLOAT2 texture;
    XMFLOAT3 normal;
    XMFLOAT4 tangent;
  };
  
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
  
  typedef struct {
    unsigned int index[3];
  } Triangle;

class BaseMesh
{
protected:

public:
  void* operator new(size_t i)
  {
    return _mm_malloc(i, 16);
  }

  void operator delete(void* p)
  {
    _mm_free(p);
  }

  BaseMesh();
  ~BaseMesh();
  

  static void CalcTangentArray(std::vector<VertexType> &vertices,
    const std::vector<Triangle> &triangles);

  virtual void SendData(ID3D11DeviceContext*);
  int GetIndexCount();
  ID3D11ShaderResourceView* GetTexture();

  virtual void InitBuffers(std::vector<VertexType> &vertices,
    std::vector<unsigned int> &indices);
  virtual void InitBuffers(ID3D11Device* device) {};

  inline XMMATRIX transform() const {
    return XMLoadFloat4x4A(&transform_);
  }

  inline int mat_id() const {
    return mat_id_;
  }

  inline void set_transform(const XMMATRIX &v) {
    XMStoreFloat4x4A(&transform_, v);
  }

  inline void set_mat_id(int id) {
    mat_id_ = id;
  }

  inline void set_vertices(const std::vector<ModelType> &val) {
    vertices_ = val;
  }

  inline void set_indices(const std::vector<unsigned int> &val) {
    indices_ = val;
  }

  inline void set_index_offset(const size_t o) {
    index_offset_ = o;
  }
  inline size_t index_offset() const {
    return index_offset_;
  }
  inline void set_vertex_offset(const size_t o) {
    vertex_offset_ = o;
  }
  inline size_t vertex_offset() const {
    return vertex_offset_;
  }

  inline size_t GetIndicesSize() const {
    return indices_.size();
  }
  inline size_t GetVerticesSize() const {
    return vertices_.size();
  }

protected:
  void LoadTexture(ID3D11Device*, WCHAR*);

  // World transformation matrix
  XMFLOAT4X4A transform_;
  ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
  size_t m_indexCount, m_vertexCount;

  // The offset of the mesh in the index buffer with respect to
  // the model's master buffer
  size_t index_offset_;
  size_t vertex_offset_;

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
    ar & index_offset_ & vertex_offset_;
  }
};

#endif