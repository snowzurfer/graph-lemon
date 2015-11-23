#include "normal_alpha_spec_map_shader.h"
#include "Texture.h"


NormalAlphaSpecMapShader::NormalAlphaSpecMapShader(ID3D11Device* device, HWND hwnd,
  sz::ConstBufManager &buf_man, unsigned int lights_num) : 
    BaseShader(device, hwnd), material_buf_(nullptr) {
  InitShader(buf_man, lights_num);
}


NormalAlphaSpecMapShader::~NormalAlphaSpecMapShader()
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


void NormalAlphaSpecMapShader::InitShader(sz::ConstBufManager &buf_man,unsigned int lights_num) {
  D3D11_BUFFER_DESC matrixBufferDesc;
  D3D11_SAMPLER_DESC samplerDesc;
  D3D11_BUFFER_DESC lightBufferDesc;
  D3D11_BUFFER_DESC camBufferDesc;
  D3D11_INPUT_ELEMENT_DESC polygon_layout[4];

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

  polygon_layout[3].SemanticName = "TANGENT";
  polygon_layout[3].SemanticIndex = 0;
  polygon_layout[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
  polygon_layout[3].InputSlot = 0;
  polygon_layout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
  polygon_layout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
  polygon_layout[3].InstanceDataStepRate = 0;

  // Load (+ compile) shader files
  loadVertexShader(polygon_layout, 4, L"../shaders/normal_alpha_spec_map_vs.hlsl");
  loadPixelShader(L"../shaders/normal_alpha_spec_map_ps.hlsl");

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
  samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
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

  // Create the texture sampler state.
  m_device->CreateSamplerState(&samplerDesc, &m_sampleState);

  // Setup light buffer
  // Setup the description of the light dynamic constant buffer that is in the pixel shader.
  // Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
  lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  lightBufferDesc.ByteWidth = sizeof(sz::LightBufferType) * lights_num;
  lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  lightBufferDesc.MiscFlags = 0;
  lightBufferDesc.StructureByteStride = 0;

  // Create the constant buffer pointer so we can access the vertex shader constant 
  // buffer from within this class.
  m_lightBuffer = buf_man.CreateD3D11ConstBuffer("scene_lights_buffer",
    lightBufferDesc, m_device);
  assert(m_lightBuffer != nullptr);

  // Setup cam buffer
  camBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  camBufferDesc.ByteWidth = sizeof(sz::CamBufferType);
  camBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  camBufferDesc.MiscFlags = 0;
  camBufferDesc.StructureByteStride = 0;

  // Create the constant buffer pointer so we can access the vertex shader constant 
  // buffer from within this class.
  m_camBuffer = buf_man.CreateD3D11ConstBuffer("scene_cam_buffer",
    camBufferDesc, m_device);
  assert(m_camBuffer != nullptr);

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

}


void NormalAlphaSpecMapShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, 
  const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, 
  const XMMATRIX &projectionMatrix, const sz::Material &mat) {
  HRESULT result;
  D3D11_MAPPED_SUBRESOURCE mapped_resource;
  sz::MatrixBufferType* data_ptr;
  unsigned int bufferNumber;
  XMMATRIX tworld, tview, tproj;


  // Transpose the matrices to prepare them for the shader.
  tworld = XMMatrixTranspose(worldMatrix);
  tview = XMMatrixTranspose(viewMatrix);
  tproj = XMMatrixTranspose(projectionMatrix);

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

  ID3D11ShaderResourceView * texture_diffuse = 
    Texture::Inst()->GetTexture(mat.diffuse_texname_crc);
  ID3D11ShaderResourceView * texture_normal = 
    Texture::Inst()->GetTexture(mat.bump_texname_crc);
  ID3D11ShaderResourceView * texture_alpha = 
    Texture::Inst()->GetTexture(mat.alpha_texname_crc);
  ID3D11ShaderResourceView * texture_spec = 
    Texture::Inst()->GetTexture(mat.specular_texname_crc);
  // Set shader textures resource in the pixel shader.
  deviceContext->PSSetShaderResources(0, 1, &texture_diffuse);
  deviceContext->PSSetShaderResources(1, 1, &texture_normal);
  deviceContext->PSSetShaderResources(2, 1, &texture_alpha);
  deviceContext->PSSetShaderResources(3, 1, &texture_spec);

}

void NormalAlphaSpecMapShader::SetShaderFrameParameters(
  ID3D11DeviceContext* deviceContext, 
  std::vector<Light> &lights, Camera *cam) {
  HRESULT result;
  D3D11_MAPPED_SUBRESOURCE mapped_resource;
  sz::LightBufferType* light_ptr;
  sz::CamBufferType* camPtr;
  unsigned int bufferNumber;

  // Send light data to pixel and vertex shader
  result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
  light_ptr = (sz::LightBufferType*)mapped_resource.pData;
  for (unsigned int i = 0; i < lights.size(); i++) {
    light_ptr[i].diffuse = lights[i].GetDiffuseColour();
    light_ptr[i].ambient = lights[i].GetAmbientColour();
    light_ptr[i].direction = XMFLOAT4(lights[i].GetDirection().x, 
      lights[i].GetDirection().y, lights[i].GetDirection().z, 0.f);
    light_ptr[i].specular = lights[i].GetSpecularColour();
    light_ptr[i].specular_power = lights[i].GetSpecularPower();
    light_ptr[i].attenuation = XMFLOAT4(lights[i].GetAttenuation().x, 
      lights[i].GetAttenuation().y, lights[i].GetAttenuation().z, 1.0);
    light_ptr[i].range = lights[i].GetRange();
    light_ptr[i].position = lights[i].GetPosition4();
    light_ptr[i].active = static_cast<unsigned int>(lights[i].active());
    light_ptr[i].spot_cutoff = lights[i].spot_cutoff();
    light_ptr[i].spot_exponent = lights[i].spot_exponent();
  }
  deviceContext->Unmap(m_lightBuffer, 0);
  bufferNumber = 0;
  deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);
  deviceContext->VSSetConstantBuffers(2, 1, &m_lightBuffer);

  // Send camera data to vertex shader
  result = deviceContext->Map(m_camBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0,
    &mapped_resource);
  camPtr = (sz::CamBufferType*)mapped_resource.pData;
  camPtr->camPos = cam->GetPosition();
  deviceContext->Unmap(m_camBuffer, 0);
  bufferNumber = 1;
  deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_camBuffer);
   
}

  void NormalAlphaSpecMapShader::Render(ID3D11DeviceContext* deviceContext,
    size_t index_count,
    size_t index_start,
    size_t base_vertex)
{
  // Set the sampler state in the pixel shader.
  deviceContext->PSSetSamplers(0, 1, &m_sampleState);

  // Base render function.
  BaseShader::Render(deviceContext, index_count, index_start, base_vertex);
}
