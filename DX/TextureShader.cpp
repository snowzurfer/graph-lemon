// texture shader.cpp
#include "textureshader.h"
#include "Texture.h"
#include "buffer_types.h"


TextureShader::TextureShader(ID3D11Device* device, HWND hwnd,
  sz::ConstBufManager &buf_man) : BaseShader(device, hwnd)
{
  InitShader(buf_man, L"../shaders/texture_vs.hlsl", L"../shaders/texture_ps.hlsl");
}


TextureShader::~TextureShader()
{
  // Release the sampler state.
  if (m_sampleState)
  {
    m_sampleState->Release();
    m_sampleState = 0;
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


void TextureShader::InitShader(sz::ConstBufManager &buf_man, 
  WCHAR* vsFilename, WCHAR* psFilename) {
  D3D11_BUFFER_DESC matrixBufferDesc;
  D3D11_SAMPLER_DESC samplerDesc;
  D3D11_INPUT_ELEMENT_DESC polygon_layout[3];

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

  polygon_layout[2].SemanticName = "NORMAL";
  polygon_layout[2].SemanticIndex = 0;
  polygon_layout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
  polygon_layout[2].InputSlot = 0;
  polygon_layout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
  polygon_layout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
  polygon_layout[2].InstanceDataStepRate = 0;

  // Load (+ compile) shader files
  loadVertexShader(polygon_layout, 3, vsFilename);
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

  // Create a texture sampler state description.
  samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.MipLODBias = 0.0f;
  samplerDesc.MaxAnisotropy = 1;
  samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
  samplerDesc.BorderColor[0] = 0;
  samplerDesc.BorderColor[1] = 0;
  samplerDesc.BorderColor[2] = 0;
  samplerDesc.BorderColor[3] = 0;
  samplerDesc.MinLOD = 0;
  samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

  // Create the constant buffer for materials
  D3D11_BUFFER_DESC mat_buff_desc;
  // Setup material buffer
  mat_buff_desc.Usage = D3D11_USAGE_DYNAMIC;
  mat_buff_desc.ByteWidth = sizeof(sz::MaterialBufferType);
  mat_buff_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  mat_buff_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  mat_buff_desc.MiscFlags = 0;
  mat_buff_desc.StructureByteStride = 0;
  // Create the buffer
  material_buf_ = buf_man.CreateD3D11ConstBuffer("mat_buffer",
    mat_buff_desc, m_device);
  assert(material_buf_ != nullptr);

  // Create the texture sampler state.
  m_device->CreateSamplerState(&samplerDesc, &m_sampleState);

}


void TextureShader::SetShaderParameters(ID3D11DeviceContext* deviceContext,
  const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix,
  const XMMATRIX &projectionMatrix, const sz::Material &mat) {
  HRESULT result;
  D3D11_MAPPED_SUBRESOURCE mapped_resource;
  sz::MatrixBufferType* dataPtr;
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
  
  // Assign the material data
  sz::MaterialBufferType *mat_buff_ptr;
  result = deviceContext->Map(material_buf_, 0, D3D11_MAP_WRITE_DISCARD, 0,
    &mapped_resource);
  // Get a ptr to the data in the constant buffer
  mat_buff_ptr = (sz::MaterialBufferType *)mapped_resource.pData;
  // Set its data
  mat_buff_ptr->ambient = XMFLOAT4(mat.ambient[0], mat.ambient[1],
    mat.ambient[2], mat.dissolve);
  mat_buff_ptr->diffuse = XMFLOAT4(mat.diffuse[0], mat.diffuse[1],
    mat.diffuse[2], mat.dissolve);
  mat_buff_ptr->specular = XMFLOAT4(mat.specular[0], mat.specular[1],
    mat.specular[2], mat.dissolve);
  mat_buff_ptr->transmittance = XMFLOAT4(mat.transmittance[0], 
    mat.transmittance[1], mat.transmittance[2], 1.f);
  mat_buff_ptr->emission = XMFLOAT4(mat.emission[0], mat.emission[1],
    mat.emission[2], mat.dissolve);
  mat_buff_ptr->shininess = mat.shininess;
  mat_buff_ptr->ior = mat.ior;
  mat_buff_ptr->dissolve = mat.dissolve;
  mat_buff_ptr->illum = mat.illum;
  // Unlock the constant buffer
  deviceContext->Unmap(material_buf_, 0);
  // Set the constant buffer index in the pixel shader
  deviceContext->PSSetConstantBuffers(1, 1, &material_buf_);

  ID3D11ShaderResourceView * texture =
    Texture::Inst()->GetTexture(mat.diffuse_texname_crc);
  // Set shader texture resource in the pixel shader.
  deviceContext->PSSetShaderResources(0, 1, &texture);
}

//void TextureShader::CleanupTextures(ID3D11DeviceContext* deviceContext) {
//  ID3D11ShaderResourceView * texture[1] = { NULL };
//
//  deviceContext->PSSetShaderResources(0, 1, texture);
//}
void TextureShader::Render(ID3D11DeviceContext* deviceContext,
    size_t index_count,
    size_t index_start,
    size_t base_vertex)
{
  // Set the sampler state in the pixel shader.
  deviceContext->PSSetSamplers(0, 1, &m_sampleState);

  // Base render function.
  BaseShader::Render(deviceContext, index_count, index_start, base_vertex);
}


void TextureShader::SetShaderFrameParameters(ID3D11DeviceContext* deviceContext) {
  // Do nothing
}

