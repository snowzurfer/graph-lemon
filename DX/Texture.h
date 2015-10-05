// texture.h
#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <d3d11.h>
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include <string>
#include <fstream>
#include <map>

using namespace DirectX;

class Texture
{
public:
	Texture(ID3D11Device* device, WCHAR* filename);
	~Texture();

	ID3D11ShaderResourceView* GetTexture();

private:
	bool does_file_exist(const WCHAR *fileName);

	ID3D11ShaderResourceView* m_texture;

  // The filename of the texture, used for the map
  std::wstring file_name_;

  // Have textures be loaded only ONCE in memory
  static std::map<std::wstring, ID3D11ShaderResourceView *> textures_;
};

#endif