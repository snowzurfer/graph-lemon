// BaseMesh.cpp
// Base mesh class, for inheriting base mesh functionality.

#include "basemesh.h"
#include "Texture.h"

BaseMesh::BaseMesh()
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

int BaseMesh::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* BaseMesh::GetTexture()
{
	return Texture::Inst()->GetTexture(m_Texture);
}


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

void BaseMesh::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	// Create the texture object.
	Texture::Inst()->LoadTexture(device, filename);

  m_Texture = filename;
}

void BaseMesh::InitBuffers(ID3D11Device *device) {
    VertexType* vertices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;


    vertices = new VertexType[vertices_.size()];



    // Load the vertex array and index array with data.
    for (int i = 0; i < vertices_.size(); i++) {
      vertices[i].position = XMFLOAT3(vertices_[i].x, vertices_[i].y, -vertices_[i].z);
      vertices[i].texture = XMFLOAT2(vertices_[i].tu, vertices_[i].tv);
      vertices[i].normal = XMFLOAT3(vertices_[i].nx, vertices_[i].ny, -vertices_[i].nz);

    }


    // Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType)* m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Now create the vertex buffer.
    device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);

    // Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long)* m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices_.data();
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // Create the index buffer.
    device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);

    // Release the arrays now that the vertex and index buffers have been created and loaded.
    delete[] vertices;
    vertices = nullptr;

}


