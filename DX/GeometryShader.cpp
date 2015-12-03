// geometry shader.cpp
#include "geometryshader.h"
#include "buffer_types.h"
#include "buffer_resource_manager.h"
#include "Material.h"
#include <DirectXMath.h>

GeometryBoxShader::GeometryBoxShader(ID3D11Device* device, HWND hwnd,
  sz::ConstBufManager &buf_man) : BaseShader(device, hwnd)
{
  InitShader(L"../shaders/geometrybox_vs.hlsl",
    L"../shaders/geometrybox_gs.hlsl", L"../shaders/geometrybox_ps.hlsl",
    buf_man);
}


GeometryBoxShader::~GeometryBoxShader() {
  // Release the layout.
  if (m_layout)
  {
    m_layout->Release();
    m_layout = 0;
  }

  //Release base shader components
  BaseShader::~BaseShader();
}

void GeometryBoxShader::InitShader(WCHAR* vsFilename, WCHAR* gsFilename,
    WCHAR* psFilename,
    sz::ConstBufManager &buf_man) {
  D3D11_BUFFER_DESC matrix_buffer_desc;
  D3D11_INPUT_ELEMENT_DESC polygon_layout[1];

  // Create the vertex input layout description.
  // This setup needs to match the VertexType stucture in the MeshClass and in the shader.
  polygon_layout[0].SemanticName = "POSITION";
  polygon_layout[0].SemanticIndex = 0;
  polygon_layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
  polygon_layout[0].InputSlot = 0;
  polygon_layout[0].AlignedByteOffset = 0;
  polygon_layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
  polygon_layout[0].InstanceDataStepRate = 0;

  // Load (+ compile) shader files
  loadVertexShader(polygon_layout, 1, vsFilename);
  loadGeometryShader(gsFilename);
  loadPixelShader(psFilename);

  // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
  matrix_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
  matrix_buffer_desc.ByteWidth = sizeof(sz::MatrixBufferType);
  matrix_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  matrix_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  matrix_buffer_desc.MiscFlags = 0;
  matrix_buffer_desc.StructureByteStride = 0;

  // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
  m_matrixBuffer = buf_man.CreateD3D11ConstBuffer("mvp_buffer",
    matrix_buffer_desc, m_device);
  assert(m_matrixBuffer != nullptr);
}


void GeometryBoxShader::SetShaderParameters(ID3D11DeviceContext* deviceContext,
    const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix,
    const XMMATRIX &projectionMatrix, const sz::Material &mat) {
  HRESULT result;
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  sz::MatrixBufferType* dataPtr;
  unsigned int bufferNumber;
  XMMATRIX tworld, tview, tproj;

  // Transpose the matrices to prepare them for the shader.
  tworld = DirectX::XMMatrixTranspose(worldMatrix);
  tview = DirectX::XMMatrixTranspose(viewMatrix);
  tproj = DirectX::XMMatrixTranspose(projectionMatrix);

  // Lock the constant buffer so it can be written to.
  result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

  // Get a pointer to the data in the constant buffer.
  dataPtr = (sz::MatrixBufferType*)mappedResource.pData;

  // Copy the matrices into the constant buffer.
  dataPtr->world = tworld;// worldMatrix;
  dataPtr->view = tview;
  dataPtr->projection = tproj;

  // Unlock the constant buffer.
  deviceContext->Unmap(m_matrixBuffer, 0);

  // Set the position of the constant buffer in the vertex shader.
  bufferNumber = 0;

  // Now set the constant buffer in the vertex shader with the updated values.
  deviceContext->GSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

}

void GeometryBoxShader::Render(ID3D11DeviceContext* deviceContext,
    size_t index_count,
    size_t index_start,
    size_t base_vertex) {
  // Base render function.
  BaseShader::Render(deviceContext, index_count, index_start, base_vertex);
}



