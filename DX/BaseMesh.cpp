// BaseMesh.cpp
// Base mesh class, for inheriting base mesh functionality.

#include "basemesh.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

BaseMesh::BaseMesh() :
  transform_(XMMatrixIdentity()),
  m_vertexBuffer(nullptr),
  m_indexBuffer(nullptr)
{
}


BaseMesh::~BaseMesh()
{
  // Release the index buffer.
  if (m_indexBuffer)
  {
    m_indexBuffer->Release();
    m_indexBuffer = 0;
  }

  // Release the vertex buffer.
  if (m_vertexBuffer)
  {
    m_vertexBuffer->Release();
    m_vertexBuffer = 0;
  }

  // Release the texture object.
}

//ID3D11ShaderResourceView* BaseMesh::GetTexture()
//{
//  //return Texture::Inst()->GetTexture(m_Texture);
//}


void BaseMesh::SendData(ID3D11DeviceContext* deviceContext)
{
  unsigned int stride;
  unsigned int offset;
  
  // Set vertex buffer stride and offset.
  stride = sizeof(VertexType);
  offset = 0;

  // Set the vertex buffer to active in the input assembler so it can be rendered.
  deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

  // Set the index buffer to active in the input assembler so it can be rendered.
  deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

  // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
  deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
  
int BaseMesh::GetIndexCount() {
  return m_indexCount;
}

void BaseMesh::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
  // Create the texture object.
  //Texture::Inst()->LoadTexture(device, filename);

  //m_Texture = filename;
}

void BaseMesh::InitBuffers(std::vector<VertexType> &vertices,
    std::vector<unsigned int> &indices) {
  // Load the vertex array and index array with data.
  size_t out_i = 0;
  for (size_t i = 0; i < vertices_.size(); i++) {
    out_i = i + vertex_offset_;
    vertices.push_back(VertexType());
    vertices[out_i].position = XMFLOAT3(vertices_[i].x, vertices_[i].y, -vertices_[i].z);
    vertices[out_i].texture = XMFLOAT2(vertices_[i].tu, 1 - vertices_[i].tv);
    vertices[out_i].normal = XMFLOAT3(vertices_[i].nx, vertices_[i].ny, -vertices_[i].nz);
    vertices[out_i].tangent = XMFLOAT4(vertices_[i].tx, vertices_[i].ty, -vertices_[i].tz,
      vertices_[i].tw);
  }

  indices.insert(indices.end(), indices_.begin(), indices_.end());

}

void BaseMesh::CalcTangentArray(std::vector<VertexType> &vertices, 
  const std::vector<Triangle> &triangles) {
  using glm::vec3;
  using glm::vec2;
  using glm::vec4;

  vec3 *tan1 = new vec3[vertices.size() * 2];
  vec3 *tan2 = tan1 + vertices.size();

  for (size_t a = 0; a < triangles.size(); ++a) {
    size_t i1 = triangles[a].index[0];
    size_t i2 = triangles[a].index[1];
    size_t i3 = triangles[a].index[2];
      //vertices[i].position = vec3(vertices_[i].x, vertices_[i].y, -vertices_[i].z);
      //vertices[i].texture = vec2(vertices_[i].tu, vertices_[i].tv);
      //vertices[i].normal = vec3(vertices_[i].nx, vertices_[i].ny, -vertices_[i].nz);
    const vec3 v1 = vec3(vertices[i1].position.x, 
      vertices[i1].position.y, vertices[i1].position.z);
    const vec3 v2 = vec3(vertices[i2].position.x, vertices[i2].position.y, vertices[i2].position.z);
    const vec3 v3 = vec3(vertices[i3].position.x, vertices[i3].position.y, vertices[i3].position.z);
    const vec2 w1 = vec2(vertices[i1].texture.x, vertices[i1].texture.y);
    const vec2 w2 = vec2(vertices[i2].texture.x, vertices[i2].texture.y);
    const vec2 w3 = vec2(vertices[i3].texture.x, vertices[i3].texture.y);

    float x1 = v2.x - v1.x;
    float x2 = v3.x - v1.x;
    float y1 = v2.y - v1.y;
    float y2 = v3.y - v1.y;
    float z1 = v2.z - v1.z;
    float z2 = v3.z - v1.z;

    float s1 = w2.x - w1.x;
    float s2 = w3.x - w1.x;
    float t1 = w2.y - w1.y;
    float t2 = w3.y - w1.y;

    float r = 1.0F / (s1 * t2 - s2 * t1);
    vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
      (t2 * z1 - t1 * z2) * r);
    vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
      (s1 * z2 - s2 * z1) * r);

    tan1[i1] += sdir;
    tan1[i2] += sdir; 
    tan1[i3] += sdir; 
    tan2[i1] += tdir; 
    tan2[i2] += tdir; 
    tan2[i3] += tdir; 

  }

  for (size_t a = 0; a < vertices.size(); ++a) {
    const vec3 n = vec3(vertices[a].normal.x, vertices[a].normal.y, vertices[a].normal.z);
    const vec3 &t = tan1[a];

    // Gram-Schmidt orthogonalise
    vec4 tangent = glm::normalize(vec4((t - n * glm::dot(n, t)), 1.f));

    // Calculate the handedness
    if (glm::dot(glm::cross(n, t), tan2[a]) < 0.f) {
      tangent.w = -1.f;
    }
    else {
      tangent.w = 1.f;
    }

    // Assign the calculated tangent
    vertices[a].tangent.x = tangent.x;
    vertices[a].tangent.y = tangent.y;
    vertices[a].tangent.z = tangent.z;
    vertices[a].tangent.w = tangent.w;

    std::cout << vertices[a].normal.x << " "
      << vertices[a].normal.y << " "
      << vertices[a].normal.z << " \n"
      << vertices[a].tangent.x << " "
      << vertices[a].tangent.y << " "
      << vertices[a].tangent.z << " \n\n" << std::endl;

  }

  delete[] tan1;

}
