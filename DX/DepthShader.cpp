// depth shader.cpp
#include "depthshader.h"
#include "buffer_resource_manager.h"
#include "Material.h"

DepthShader::DepthShader(ID3D11Device* device, HWND hwnd,
    sz::ConstBufManager &buf_man) : BaseShader(device, hwnd) {
  InitShader(buf_man, L"../shaders/depth_vs.hlsl", L"../shaders/depth_ps.hlsl");
}


DepthShader::~DepthShader() {
  // Release the matrix constant buffer.
  //if (m_matrixBuffer)
  //{
  //  m_matrixBuffer->Release();
  //  m_matrixBuffer = 0;
  //}

  // Release the layout.
  if (m_layout)
  {
    m_layout->Release();
    m_layout = 0;
  }

  //Release base shader components
  BaseShader::~BaseShader();
}


void DepthShader::InitShader(sz::ConstBufManager &buf_man, WCHAR* vsFilename,
  WCHAR* psFilename) {
  D3D11_BUFFER_DESC matrixBufferDesc;
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
  loadPixelShader(psFilename);

  // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
  matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  matrixBufferDesc.ByteWidth = sizeof(sz::MatrixBufferType);
  matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  matrixBufferDesc.MiscFlags = 0;
  matrixBufferDesc.StructureByteStride = 0;

  // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
  m_matrixBuffer = buf_man.CreateD3D11ConstBuffer("mvp_buffer",
    matrixBufferDesc, m_device);
  assert(m_matrixBuffer != nullptr);

}


void DepthShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, 
    const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection,
    const sz::Material &mat) {
  HRESULT result;
  D3D11_MAPPED_SUBRESOURCE mapped_resource;
  sz::MatrixBufferType* data_ptr;
  unsigned int bufferNumber;
  XMMATRIX tworld, tview, tproj;

  // Transpose the matrices to prepare them for the shader.
  tworld = XMMatrixTranspose(world);
  tview = XMMatrixTranspose(view);
  tproj = XMMatrixTranspose(projection);

  // Lock the constant buffer so it can be written to.
  result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);

  // Get a pointer to the data in the constant buffer.
  data_ptr = (sz::MatrixBufferType*)mapped_resource.pData;

  // Copy the matrices into the constant buffer.
  data_ptr->world = tworld;// worldMatrix;
  data_ptr->view = tview;
  data_ptr->projection = tproj;

  // Unlock the constant buffer.
  deviceContext->Unmap(m_matrixBuffer, 0);

  // Set the position of the constant buffer in the vertex shader.
  bufferNumber = 0;

  // Now set the constant buffer in the vertex shader with the updated values.
  deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
}

void DepthShader::Render(ID3D11DeviceContext* deviceContext,
    size_t index_count,
    size_t index_start,
    size_t base_vertex) {
  // Base render function.
  BaseShader::Render(deviceContext, index_count, index_start, base_vertex);
}



