// texture shader.cpp
#include "lightshader.h"
#include "Texture.h"


LightShader::LightShader(ID3D11Device* device, HWND hwnd, 
  szgrh::ConstBufManager &buf_man, unsigned int lights_num) : 
    BaseShader(device, hwnd), material_buf_(nullptr) {
	InitShader(buf_man, L"../shaders/light_vs.hlsl", 
    L"../shaders/light_ps.hlsl", lights_num);
}


LightShader::~LightShader()
{
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

	// Release the light constant buffer.
	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}


void LightShader::InitShader(szgrh::ConstBufManager &buf_man, 
  WCHAR* vsFilename, WCHAR* psFilename, unsigned int lights_num) {
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC camBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
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
	lightBufferDesc.ByteWidth = sizeof(LightBufferType) * lights_num;
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
	camBufferDesc.ByteWidth = sizeof(CamBufferType);
	camBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	camBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
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
	mat_buff_desc.ByteWidth = sizeof(MaterialBufferType);
	mat_buff_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mat_buff_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	mat_buff_desc.MiscFlags = 0;
	mat_buff_desc.StructureByteStride = 0;
  // Create the buffer
  material_buf_ = buf_man.CreateD3D11ConstBuffer("mat_buffer",
    mat_buff_desc, m_device);
  assert(material_buf_ != nullptr);

}


void LightShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, 
  const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, 
  const XMMATRIX &projectionMatrix, const szgrh::Material &mat) {
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mapped_resource;
	MatrixBufferType* data_ptr;
	unsigned int bufferNumber;
	XMMATRIX tworld, tview, tproj;


	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);

	// Get a pointer to the data in the constant buffer.
	data_ptr = (MatrixBufferType*)mapped_resource.pData;

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
  MaterialBufferType *mat_buff_ptr;
  result = deviceContext->Map(material_buf_, 0, D3D11_MAP_WRITE_DISCARD, 0,
    &mapped_resource);
  // Get a ptr to the data in the constant buffer
  mat_buff_ptr = (MaterialBufferType *)mapped_resource.pData;
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


  ID3D11ShaderResourceView * texture = Texture::Inst()->GetTexture(mat.diffuse_texname.c_str());
  // Set shader texture resource in the pixel shader.
  deviceContext->PSSetShaderResources(0, 1, &texture);
}

void LightShader::SetShaderFrameParameters(ID3D11DeviceContext* deviceContext, std::vector<Light> &lights, Camera *cam) {
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mapped_resource;
	LightBufferType* light_ptr;
	CamBufferType* camPtr;
	unsigned int bufferNumber;
	
  // Send light data to pixel shader
  result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
  light_ptr = (LightBufferType*)mapped_resource.pData;
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

  // Send camera data to vertex shader
	result = deviceContext->Map(m_camBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0,
    &mapped_resource);
	camPtr = (CamBufferType*)mapped_resource.pData;
  camPtr->camPos = cam->GetPosition();
	deviceContext->Unmap(m_camBuffer, 0);
	bufferNumber = 1;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_camBuffer);
   
}

  void LightShader::Render(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Base render function.
	BaseShader::Render(deviceContext, indexCount);
}



