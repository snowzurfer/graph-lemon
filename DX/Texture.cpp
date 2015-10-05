// texture.cpp
#include "texture.h"

std::map<std::wstring, ID3D11ShaderResourceView *> Texture::textures_;

Texture::Texture(ID3D11Device* device, WCHAR* filename)
{
	HRESULT result;

	// check if file exists
	if (!filename)
	{
		filename = L"../res/DefaultDiffuse.png";
	}
	// if not set default texture
	if (!does_file_exist(filename))
	{
		// change default texture
		filename = L"../res/DefaultDiffuse.png";
	}

  // Set its name
  file_name_ = filename;

  // Check if this texture has already been loaded
  std::map<std::wstring, ID3D11ShaderResourceView *>::const_iterator it = textures_.find(filename);
  // If the texture already exists 
  if (it != textures_.end()) {
    // Point this one's ptr to the one just found
    m_texture = it->second;

    return;
  }


	// check file extension for correct loading function.
	std::wstring fn(filename);
	std::string::size_type idx;
	std::wstring extension;

	idx = fn.rfind('.');

	if (idx != std::string::npos)
	{
		extension = fn.substr(idx + 1);
	}
	else
	{
		// No extension found
	}

	// Load the texture in.
	if (extension == L"dds")
	{
		result = CreateDDSTextureFromFile(device, filename, NULL, &m_texture, 0, NULL);
	}
	else
	{
		result = CreateWICTextureFromFile(device, filename, NULL, &m_texture, 0);
	}
	
	if (FAILED(result))
	{
		MessageBox(NULL, L"Texture loading error", L"ERROR", MB_OK);
	}

  // Add texture to the map
  textures_[fn] = m_texture;
}

Texture::~Texture()
{
  // NOTE
  // Should check if a texture is being used by more than once and delete only
  // when the last one has to be deleted; this would require a more 
  // sophisticated approach, which will be implemented in the future.
  // So far, when a texture is deleted, its correspondant resource is also
  // released.

	// Release the texture resource.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}
}


ID3D11ShaderResourceView* Texture::GetTexture()
{
	return m_texture;
}

bool Texture::does_file_exist(const WCHAR *fname)
{
	std::ifstream infile(fname);
	return infile.good();
}