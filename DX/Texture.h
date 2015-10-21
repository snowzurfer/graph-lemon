// texture.h
#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <d3d11.h>
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include <string>
#include <fstream>
#include <unordered_map>
#include "Model.h"

using namespace DirectX;

class Texture
{
public:
  // Retrieve instance of singleton
  static Texture *Inst();

  // Resets singleton to not having an instance
  static void ResetInst();

  // Load a texture in memory
  void LoadTexture(ID3D11Device* device, ID3D11DeviceContext *dev_context, 
    WCHAR* filename);
  
  // Delete a texture from memory
  void FreeTexture(const std::wstring &tx_name);

  // Retrieve a texture from its name
  ID3D11ShaderResourceView *GetTexture(const std::wstring &tx_name);
  ID3D11ShaderResourceView *GetTexture(const std::string &tx_name);

  // Disable ctors
  Texture(const Texture &) = delete;
  Texture &operator=(const Texture &) = delete;

private:

  Texture();
	
  bool does_file_exist(const WCHAR *fileName);

  // Have textures be loaded only ONCE in memory
  std::unordered_map<std::wstring, ID3D11ShaderResourceView *> textures_;

  // Ptr to single global instance
  static Texture *single_instance_;
};


void LoadTextures(ID3D11Device *device, Model &model);

#endif