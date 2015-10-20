// base shader.cpp
#include "baseshader.h"
#include "Texture.h"


BaseShader::BaseShader(ID3D11Device* device, HWND hwnd)
{
	m_device = device;
	m_hwnd = hwnd;
}


BaseShader::~BaseShader()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	// Release the pixel shader.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	// Release the hull shader.
	if (m_hullShader)
	{
		m_hullShader->Release();
		m_hullShader = 0;
	}

	// Release the domain shader.
	if (m_domainShader)
	{
		m_domainShader->Release();
		m_domainShader = 0;
	}

	// Release the geometry shader.
	if (m_geometryShader)
	{
		m_geometryShader->Release();
		m_geometryShader = 0;
	}
}


void BaseShader::loadVertexShader(const D3D11_INPUT_ELEMENT_DESC *layout, 
  size_t num_elements, WCHAR* filename) {
	HRESULT result;
	ID3DBlob* errorMessage;
	ID3DBlob* vertexShaderBuffer;
	
	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;

	// Compile the vertex shader code.
	result = D3DCompileFromFile(filename, NULL, NULL, "main", "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, m_hwnd, filename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(m_hwnd, filename, L"Missing Shader File", MB_OK);
		}
		exit(0);
	}

	// Create the vertex shader from the buffer.
	result = m_device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
    int lol = 0;
	}

	
	// Create the vertex input layout.
	result = m_device->CreateInputLayout(layout, num_elements, 
    vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&m_layout);
	if (FAILED(result))
	{
    int lol = 0;
		//return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
}

void BaseShader::loadPixelShader(WCHAR* filename)
{
	HRESULT result;
	ID3DBlob* errorMessage;
	ID3DBlob* pixelShaderBuffer;
	
	// Compile the pixel shader code.
	result = D3DCompileFromFile(filename, NULL, NULL, "main", "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, m_hwnd, filename);
		}
		// If there was  nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(m_hwnd, filename, L"Missing Shader File", MB_OK);
		}

		exit(0);
	}

	// Create the pixel shader from the buffer.
	result = m_device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;
}

void BaseShader::loadHullShader(WCHAR* filename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* hullShaderBuffer;

	// Compile the hull shader code.
	result = D3DCompileFromFile(filename, NULL, NULL, "main", "hs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &hullShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, m_hwnd, filename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(m_hwnd, filename, L"Missing Shader File", MB_OK);
		}

		exit(0);
	}

	// Create the hull shader from the buffer.
	result = m_device->CreateHullShader(hullShaderBuffer->GetBufferPointer(), hullShaderBuffer->GetBufferSize(), NULL, &m_hullShader);
	
	hullShaderBuffer->Release();
	hullShaderBuffer = 0;
}

void BaseShader::loadDomainShader(WCHAR* filename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* domainShaderBuffer;

	// Compile the domain shader code.
	result = D3DCompileFromFile(filename, NULL, NULL, "main", "ds_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &domainShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, m_hwnd, filename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(m_hwnd, filename, L"Missing Shader File", MB_OK);
		}

		exit(0);
	}

	// Create the domain shader from the buffer.
	result = m_device->CreateDomainShader(domainShaderBuffer->GetBufferPointer(), domainShaderBuffer->GetBufferSize(), NULL, &m_domainShader);
	
	domainShaderBuffer->Release();
	domainShaderBuffer = 0;
}

void BaseShader::loadGeometryShader(WCHAR* filename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* geometryShaderBuffer;

	// Compile the domain shader code.
	result = D3DCompileFromFile(filename, NULL, NULL, "main", "gs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &geometryShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, m_hwnd, filename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(m_hwnd, filename, L"Missing Shader File", MB_OK);
		}

		exit(0);
	}

	// Create the domain shader from the buffer.
	m_device->CreateGeometryShader(geometryShaderBuffer->GetBufferPointer(), geometryShaderBuffer->GetBufferSize(), NULL, &m_geometryShader);

	geometryShaderBuffer->Release();
	geometryShaderBuffer = 0;
}

void BaseShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}


void BaseShader::Render(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);
	
	// if Hull shader is not null then set HS and DS
	if (m_hullShader)
	{
		deviceContext->HSSetShader(m_hullShader, NULL, 0);
		deviceContext->DSSetShader(m_domainShader, NULL, 0);
	}
	else
	{
		deviceContext->HSSetShader(NULL, NULL, 0);
		deviceContext->DSSetShader(NULL, NULL, 0);
	}

	// if geometry shader is not null then set GS
	if (m_geometryShader)
	{
		deviceContext->GSSetShader(m_geometryShader, NULL, 0);
	}

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}

void BaseShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, 
  const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, 
  const XMMATRIX &projectionMatrix, const szgrh::Material &mat) {
	//HRESULT result;
	//D3D11_MAPPED_SUBRESOURCE mapped_resource;
	//szgrh::MatrixBufferType* data_ptr;
	//unsigned int bufferNumber;
	//XMMATRIX tworld, tview, tproj;


	//// Transpose the matrices to prepare them for the shader.
	//tworld = XMMatrixTranspose(worldMatrix);
	//tview = XMMatrixTranspose(viewMatrix);
	//tproj = XMMatrixTranspose(projectionMatrix);

	//// Lock the constant buffer so it can be written to.
	//result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);

	//// Get a pointer to the data in the constant buffer.
	//data_ptr = (szgrh::MatrixBufferType*)mapped_resource.pData;

	//// Copy the matrices into the constant buffer.
	//data_ptr->world = tworld;// worldMatrix;
	//data_ptr->view = tview;
	//data_ptr->projection = tproj;

	//// Unlock the constant buffer.
	//deviceContext->Unmap(m_matrixBuffer, 0);

	//// Set the position of the constant buffer in the vertex shader.
	//bufferNumber = 0;

	//// Now set the constant buffer in the vertex shader with the updated values.
	//deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

 // // Assign the material data
 // szgrh::MaterialBufferType *mat_buff_ptr;
 // result = deviceContext->Map(material_buf_, 0, D3D11_MAP_WRITE_DISCARD, 0,
 //   &mapped_resource);
 // // Get a ptr to the data in the constant buffer
 // mat_buff_ptr = (szgrh::MaterialBufferType *)mapped_resource.pData;
 // // Set its data
 // mat_buff_ptr->ambient = XMFLOAT4(mat.ambient[0], mat.ambient[1],
 //   mat.ambient[2], mat.dissolve);
 // mat_buff_ptr->diffuse = XMFLOAT4(mat.diffuse[0], mat.diffuse[1],
 //   mat.diffuse[2], mat.dissolve);
 // mat_buff_ptr->specular = XMFLOAT4(mat.specular[0], mat.specular[1],
 //   mat.specular[2], mat.dissolve);
 // mat_buff_ptr->transmittance = XMFLOAT4(mat.transmittance[0], 
 //   mat.transmittance[1], mat.transmittance[2], 1.f);
 // mat_buff_ptr->emission = XMFLOAT4(mat.emission[0], mat.emission[1],
 //   mat.emission[2], mat.dissolve);
 // mat_buff_ptr->shininess = mat.shininess;
 // mat_buff_ptr->ior = mat.ior;
 // mat_buff_ptr->dissolve = mat.dissolve;
 // mat_buff_ptr->illum = mat.illum;
 // // Unlock the constant buffer
 // deviceContext->Unmap(material_buf_, 0);
 // // Set the constant buffer index in the pixel shader
 // deviceContext->PSSetConstantBuffers(1, 1, &material_buf_);
 //
  //if (mat.ambient_texname != L"") {
  //  ID3D11ShaderResourceView * texture_diffuse = 
  //    Texture::Inst()->GetTexture(mat.diffuse_texname);
  //  ID3D11ShaderResourceView * texture_normal = 
  //    Texture::Inst()->GetTexture(mat.bump_texname);
  //  // Set shader textures resource in the pixel shader.
  //  deviceContext->PSSetShaderResources(0, 1, &texture_diffuse);
  //}
  //if (mat.diffuse_texname != L"") {
  //  ID3D11ShaderResourceView * texture = 
  //    Texture::Inst()->GetTexture(mat.diffuse_texname);
  //  // Set shader textures resource in the pixel shader.
  //  deviceContext->PSSetShaderResources(0, 1, &texture);

  //}
  //if (mat.specular_texname != L"") {
  //  ID3D11ShaderResourceView * texture = 
  //    Texture::Inst()->GetTexture(mat.specular_texname);
  //  // Set shader textures resource in the pixel shader.
  //  deviceContext->PSSetShaderResources(1, 1, &texture);

  //}
  //if (mat.specular_highlight_texname != L"") {
  //  ID3D11ShaderResourceView * texture = 
  //    Texture::Inst()->GetTexture(mat.specular_highlight_texname);
  //  // Set shader textures resource in the pixel shader.
  //  deviceContext->PSSetShaderResources(2, 1, &texture);

  //}
  //if (mat.bump_texname != L"") {
  //  ID3D11ShaderResourceView * texture = 
  //    Texture::Inst()->GetTexture(mat.bump_texname);
  //  // Set shader textures resource in the pixel shader.
  //  deviceContext->PSSetShaderResources(3, 1, &texture);

  //}
  //if (mat.displacement_texname != L"") {
  //  ID3D11ShaderResourceView * texture = 
  //    Texture::Inst()->GetTexture(mat.displacement_texname);
  //  // Set shader textures resource in the pixel shader.
  //  deviceContext->PSSetShaderResources(4, 1, &texture);

  //}
  //if (mat.alpha_texname != L"") {
  //  ID3D11ShaderResourceView * texture = 
  //    Texture::Inst()->GetTexture(mat.alpha_texname);
  //  // Set shader textures resource in the pixel shader.
  //  deviceContext->PSSetShaderResources(5, 1, &texture);

  //}
}