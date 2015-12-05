#include "gauss_blur_v_shader.h"
#include "Texture.h"
#include "buffer_types.h"


GaussBlurVShader::GaussBlurVShader(ID3D11Device* device, HWND hwnd,
  sz::ConstBufManager &buf_man) : BaseShader(device, hwnd)
{
  InitShader(buf_man, L"../shaders/gauss_blur_v_vs.hlsl",
    L"../shaders/gauss_blur_v_ps.hlsl");
}


GaussBlurVShader::~GaussBlurVShader() {
  // Release the sampler state.
  if (m_sampleState)
  {
    m_sampleState->Release();
    m_sampleState = 0;
  }

  // Release the matrix constant buffer.
  if (m_matrixBuffer)
  {
    m_matrixBuffer->Release();
    m_matrixBuffer = 0;
  }

  // Release the layout.
  if (m_layout)
  {
    m_layout->Release();
    m_layout = 0;
  }

  //Release base shader components
  BaseShader::~BaseShader();
}


void GaussBlurVShader::InitShader(sz::ConstBufManager &buf_man, 
  WCHAR* vsFilename, WCHAR* psFilename) {
  D3D11_BUFFER_DESC matrix_buffer_desc;
  D3D11_SAMPLER_DESC samplerDesc;
  D3D11_INPUT_ELEMENT_DESC polygon_layout[2];

  // Create the vertex input layout description.
  // This setup needs to match the VertexType stucture in the MeshClass and in the shader.
  polygon_layout[0].SemanticName = "POSITION";
  polygon_layout[0].SemanticIndex = 0;
  polygon_layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
  polygon_layout[0].InputSlot = 0;
  polygon_layout[0].AlignedByteOffset = 0;
  polygon_layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
  polygon_layout[0].InstanceDataStepRate = 0;

  polygon_layout[1].SemanticName = "TEXCOORD";
  polygon_layout[1].SemanticIndex = 0;
  polygon_layout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
  polygon_layout[1].InputSlot = 0;
  polygon_layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
  polygon_layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
  polygon_layout[1].InstanceDataStepRate = 0;


  // Load (+ compile) shader files
  loadVertexShader(polygon_layout, 2, vsFilename);
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

  // Create a texture sampler state description.
  samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
  samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
  samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
  samplerDesc.MipLODBias = 0.0f;
  samplerDesc.MaxAnisotropy = 1;
  samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
  samplerDesc.BorderColor[0] = 0;
  samplerDesc.BorderColor[1] = 0;
  samplerDesc.BorderColor[2] = 0;
  samplerDesc.BorderColor[3] = 0;
  samplerDesc.MinLOD = 0;
  samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

  // Setup constant buffer for the screensize
  D3D11_BUFFER_DESC screensize_buffer_desc;
  screensize_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
  screensize_buffer_desc.ByteWidth = sizeof(sz::ScreenSizeBufferType);
  screensize_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  screensize_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  screensize_buffer_desc.MiscFlags = 0;
  screensize_buffer_desc.StructureByteStride = 0;
  screensize_buf_ = buf_man.CreateD3D11ConstBuffer("screensize_buffer",
    screensize_buffer_desc, m_device);
  assert(screensize_buf_ != nullptr);

  // Create the texture sampler state.
  m_device->CreateSamplerState(&samplerDesc, &m_sampleState);

}


void GaussBlurVShader::SetShaderParameters(ID3D11DeviceContext* deviceContext,
  const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix,
  const XMMATRIX &projectionMatrix, const sz::Material &mat,
  const float screensize) {
  HRESULT result;
  D3D11_MAPPED_SUBRESOURCE mapped_resource;
  sz::MatrixBufferType* dataPtr;
  sz::ScreenSizeBufferType *screensize_ptr = nullptr;
  unsigned int bufferNumber;
  XMMATRIX tworld, tview, tproj;

  // Transpose the matrices to prepare them for the shader.
  tworld = XMMatrixTranspose(worldMatrix);
  tview = XMMatrixTranspose(viewMatrix);
  tproj = XMMatrixTranspose(projectionMatrix);

  // Lock the constant buffer so it can be written to.
  result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);

  // Get a pointer to the data in the constant buffer.
  dataPtr = (sz::MatrixBufferType*)mapped_resource.pData;

  // Copy the matrices into the constant buffer.
  dataPtr->world = tworld;// worldMatrix;
  dataPtr->view = tview;
  dataPtr->projection = tproj;

  // Unlock the constant buffer.
  deviceContext->Unmap(m_matrixBuffer, 0);

  // Set the position of the constant buffer in the vertex shader.
  bufferNumber = 0;

  // Now set the constant buffer in the vertex shader with the updated values.
  deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
  deviceContext->DSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

  // Send size data
  deviceContext->Map(screensize_buf_, 0, D3D11_MAP_WRITE_DISCARD, 0,
    &mapped_resource);
  screensize_ptr = (sz::ScreenSizeBufferType *)mapped_resource.pData;
  screensize_ptr->screen_size = screensize;
  deviceContext->Unmap(screensize_buf_, 0);
  deviceContext->VSSetConstantBuffers(1, 1, &screensize_buf_);
  deviceContext->DSSetConstantBuffers(1, 1, &screensize_buf_);

  ID3D11ShaderResourceView * texture = Texture::Inst()->GetTexture(mat.diffuse_texname_crc);
  // Set shader texture resource in the pixel shader.
  deviceContext->PSSetShaderResources(0, 1, &texture);
}

void GaussBlurVShader::Render(ID3D11DeviceContext* deviceContext,
    size_t index_count,
    size_t index_start,
    size_t base_vertex)
{
  // Set the sampler state in the pixel shader.
  deviceContext->PSSetSamplers(0, 1, &m_sampleState);

  // Base render function.
  BaseShader::Render(deviceContext, index_count, index_start, base_vertex);
}


void GaussBlurVShader::SetShaderFrameParameters(ID3D11DeviceContext* deviceContext) {
  // Do nothing
}
