// texture.h
#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <d3d11.h>
#include <WICtextureloader.h>
#include <string>
#include <fstream>
#include <unordered_map>
#include "Model.h"
#include "abertay_framework.h"
#include "crc.h"

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

  void LoadTexture(ID3D11Device* device, ID3D11DeviceContext *dev_context, 
    const std::string &filename);

  // Load a texture in memory from a descriptor and return the created
  // 2D texture
  ID3D11Texture2D *CreateTexture2D(ID3D11Device* device, 
    const D3D11_TEXTURE2D_DESC &text_desc, 
    const D3D11_SHADER_RESOURCE_VIEW_DESC &shad_res_view_desc,
    const std::string &filename);
  
  // Delete a texture from memory
  void FreeTexture(const std::string &tx_name);

  // Retrieve a texture from its name
  ID3D11ShaderResourceView *GetTexture(const std::wstring &tx_name);
  ID3D11ShaderResourceView *GetTexture(const std::string &tx_name);
  ID3D11ShaderResourceView *GetTexture(const UInt32);

  // Disable ctors
  Texture(const Texture &) = delete;
  Texture &operator=(const Texture &) = delete;

private:

  Texture();
  
  bool does_file_exist(const WCHAR *fileName);
  bool does_file_exist(const char *fileName);
  
  std::wstring ConvertToWide(const std::string &x);

  // Have textures be loaded only ONCE in memory
  std::unordered_map<UInt32, ID3D11ShaderResourceView *> textures_;

  // Ptr to single global instance
  static Texture *single_instance_;
};


void LoadTextures(ID3D11Device *device, Model &model);

#endif