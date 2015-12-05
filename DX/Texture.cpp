// texture.cpp
#include "texture.h"
#include <algorithm>
#include <lodepng.h>
#include <locale>
#include <codecvt>
#include <string>
#include <cstdint>

Texture *Texture::single_instance_ = nullptr;

Texture::Texture() {};

Texture *Texture::Inst() {
  // If instance doen't exist
  if (single_instance_ == nullptr) {
    // Create it
    single_instance_ = new Texture();
  }

  return single_instance_;
}

void Texture::ResetInst() {
  // If an instance exists
  if (single_instance_ != nullptr) {
    // Run through every element and free them
    std::for_each(single_instance_->textures_.begin(), single_instance_->textures_.end(),
      [&](std::pair<const UInt32, ID3D11ShaderResourceView *> &n) {
        n.second->Release();
    });

    // Delete the single instance
    delete single_instance_;
    single_instance_ = nullptr;
  }

}

void Texture::LoadTexture(ID3D11Device* device, ID3D11DeviceContext *dev_context,
  WCHAR* filename) {
  //HRESULT result;

  //// check if file exists
  //if (!filename)
  //{
  //  filename = L"../res/DefaultDiffuse.png";
  //}
  //// if not set default texture
  //if (!does_file_exist(filename))
  //{
  //  // change default texture
  //  filename = L"../res/DefaultDiffuse.png";
  //}

 // // Check if this texture has already been loaded
 // std::unordered_map<std::wstring, ID3D11ShaderResourceView *>::const_iterator it = textures_.find(filename);
 // // If the texture already exists 
 // if (it != textures_.end()) {
 //   return;
 // }


  //// check file extension for correct loading function.
  //std::wstring fn(filename);
  //std::string::size_type idx;
  //std::wstring extension;


  //idx = fn.rfind('.');

  //if (idx != std::string::npos)
  //{
  //  extension = fn.substr(idx + 1);
  //}
  //else
  //{
  //  // No extension found
  //}

 // // Create a temporary descriptor
 // ID3D11ShaderResourceView * m_texture = nullptr;

  //// Load the texture in.
  //if (extension == L"dds")
  //{
  //  result = CreateDDSTextureFromFile(device, dev_context, filename, NULL, &m_texture, 0, NULL);
  //}
  //else
  //{
  //  result = CreateWICTextureFromFile(device, dev_context, filename, NULL, &m_texture, 0);
  //}

 // 
  //if (FAILED(result))
  //{
  //  MessageBox(NULL, L"Texture loading error", L"ERROR", MB_OK);
  //}

 // // Add texture to the map
 // textures_[fn] = m_texture;
}

std::wstring Texture::ConvertToWide(const std::string &x) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  std::wstring wide = converter.from_bytes(x);

  return wide;
}

void Texture::LoadTexture(ID3D11Device* device, ID3D11DeviceContext *dev_context,
  const std::string &filename) {
  HRESULT result = S_OK;

  // Convert name to uint
  UInt32 crc_val = abfw::CRC::GetICRC(filename.c_str());

  // Check if this texture has already been loaded
  std::unordered_map<UInt32, ID3D11ShaderResourceView *>::const_iterator it =
    textures_.find(crc_val);
  // If the texture already exists 
  if (it != textures_.end()) {
    return;
  }

  // Use LodePNG to load the texture
  std::vector<std::uint8_t> image; //the raw pixels
  std::uint32_t width, height;
  const size_t bytes_per_pixel = 4; // LodePNG outputs this type

  // Decode the image using LodePNG
  std::uint32_t error = lodepng::decode(image, width, height, filename);

  //if there's an error, display it
  if (error) {
    std::cout << "decoder error " << error
      << ": " << lodepng_error_text(error) << std::endl;
  }

  D3D11_TEXTURE2D_DESC TextureDescription;
  ZeroMemory(&TextureDescription, sizeof(TextureDescription));
  TextureDescription.Width = width;
  TextureDescription.Height = height;
  TextureDescription.ArraySize = 1;
  TextureDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  TextureDescription.Usage = D3D11_USAGE_DEFAULT;
  TextureDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  TextureDescription.MipLevels = 3;
  TextureDescription.SampleDesc.Count = 1;
  TextureDescription.CPUAccessFlags = /*bAllowCpuWriteAccess ? D3D11_CPU_ACCESS_WRITE :*/ 0;

  // We only support mipmaps for this kind of format.
  //if (bGenerateMipmaps)
  //{
  TextureDescription.BindFlags |= D3D11_BIND_RENDER_TARGET;
  TextureDescription.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
  //}

  ID3D11Texture2D *texture_resource = nullptr;
  result = device->CreateTexture2D(&TextureDescription, nullptr, &texture_resource);
  if (FAILED(result)) {
    MessageBox(NULL, L"Texture 2D creation error", L"ERROR", MB_OK);
  }

  dev_context->UpdateSubresource(texture_resource, 0, nullptr, image.data(),
    bytes_per_pixel * width, bytes_per_pixel* width * height);

  D3D11_SHADER_RESOURCE_VIEW_DESC ViewDescription;
  ZeroMemory(&ViewDescription, sizeof(ViewDescription));
  ViewDescription.Texture2D.MipLevels = -1;
  ViewDescription.Texture2D.MostDetailedMip = 0;  
  ViewDescription.Format = TextureDescription.Format;
  ViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

  // Create a resource view
  ID3D11ShaderResourceView * texture_resource_view = nullptr;

  result = device->CreateShaderResourceView(texture_resource,
    &ViewDescription, &texture_resource_view);
  if (FAILED(result)) {
    MessageBox(NULL, L"Texture Resource View creation error", L"ERROR", MB_OK);
  }

  dev_context->GenerateMips(texture_resource_view);


  // Convert to wide
  //std::wstring wide = ConvertToWide(filename);

  // check if file exists
  //if (!wide.c_str())
  //{
  //  wide = L"../res/DefaultDiffuse.png";
  //  crc_val = abfw::CRC::GetICRC("../res/DefaultDiffuse.png");
  //}
  // if not set default texture
  //if (!does_file_exist(wide.c_str()))
  //{
  //   change default texture
  //  wide = L"../res/DefaultDiffuse.png";
  //  crc_val = abfw::CRC::GetICRC("../res/DefaultDiffuse.png");
  //}



  // check file extension for correct loading function.
  //std::wstring fn(wide.c_str());
  //std::string::size_type idx;
  //std::wstring extension;


  //idx = fn.rfind('.');

  //if (idx != std::string::npos)
  //{
  //  extension = fn.substr(idx + 1);
  //}
  //else
  //{
  //   No extension found
  //}


  // Load the texture in.
  //if (extension == L"dds")
  //{
  //  result = CreateDDSTextureFromFile(device, dev_context, wide.c_str(), NULL, &m_texture, 0, NULL);
  //}
  //else
  //{
  //  result = DirectX::CreateWICTextureFromFile(device, dev_context, wide.c_str(),
  //    NULL, &texture_resource_view, 0);
  //}

  //
  //if (FAILED(result))
  //{
  //  MessageBox(NULL, L"Texture loading error", L"ERROR", MB_OK);
  //}

  // Add texture to the map
  textures_[crc_val] = texture_resource_view;

}

void Texture::FreeTexture(const std::string &tx_name) {
  // NOTE
  // Should check if a texture is being used by more than once and delete only
  // when the last one has to be deleted; this would require a more 
  // sophisticated approach, which will be implemented in the future.
  // So far, when a texture is deleted, its correspondant resource is also
  // released.

  // Release the texture resource.
  
  // Convert name to uint
  UInt32 crc_val = abfw::CRC::GetICRC(tx_name.c_str());
 
  // Check that the texture exists
  std::unordered_map<UInt32, ID3D11ShaderResourceView *>::iterator it =
    textures_.find(crc_val);

  if (it != textures_.end()) {
    it->second->Release();
    textures_.erase(it);
  }
}


ID3D11ShaderResourceView* Texture::GetTexture(const std::wstring &tx_name)
{
 // std::unordered_map<std::wstring, ID3D11ShaderResourceView *>::iterator it = textures_.find(tx_name);

  //if (it != textures_.end()) {
  //  return it->second;
  //}

 return nullptr;
}

ID3D11ShaderResourceView* Texture::GetTexture(const std::string &tx_name) {
  // Convert name to uint
  UInt32 crc_val = abfw::CRC::GetICRC(tx_name.c_str());

  std::unordered_map<UInt32,
    ID3D11ShaderResourceView *>::iterator it = textures_.find(crc_val);

  if (it != textures_.end()) {
    return it->second;
  }

  return nullptr;
}
  
ID3D11ShaderResourceView *Texture::GetTexture(const UInt32 crc_val) {
  std::unordered_map<UInt32,
    ID3D11ShaderResourceView *>::iterator it = textures_.find(crc_val);

  if (it != textures_.end()) {
    return it->second;
  }

  return nullptr;
}

bool Texture::does_file_exist(const WCHAR *fname) {
  std::ifstream infile(fname);
  return infile.good();
}
bool Texture::does_file_exist(const char *fname) {
  std::ifstream infile(fname);
  return infile.good();
}

void LoadTextures(ID3D11Device *device, Model &model) {


  //for (unsigned int i = 0; i < model.materials_.size(); ++i) {
  //  std::string path_suffix = "../res/" + model.model_name_ + "/";
  //
  //  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  //  std::wstring wide;
  //  WCHAR cstyle_wide[128];

  //  if (model.materials_[i].ambient_texname != "") {
  //    converter.from_bytes(path_suffix + model.materials_[i].ambient_texname);
  //    wcscpy_s(cstyle_wide, wide.c_str());
  //    Texture::Inst()->LoadTexture(device, cstyle_wide);
  //  }

  //  if (model.materials_[i].diffuse_texname != "") {
  //    wide = converter.from_bytes(path_suffix + model.materials_[i].diffuse_texname);
  //    wcscpy_s(cstyle_wide, wide.c_str());
  //    Texture::Inst()->LoadTexture(device, cstyle_wide);
  //  }

  //  if (model.materials_[i].specular_texname != "") {
  //    wide = converter.from_bytes(path_suffix + model.materials_[i].specular_texname);
  //    wcscpy_s(cstyle_wide, wide.c_str());
  //    Texture::Inst()->LoadTexture(device, cstyle_wide);
  //  }


  //  std::string specular_highlight_texname; // map_Ns

  //  if (model.materials_[i].bump_texname != "") {
  //    wide = converter.from_bytes(path_suffix + model.materials_[i].bump_texname);
  //    wcscpy_s(cstyle_wide, wide.c_str());
  //    Texture::Inst()->LoadTexture(device, cstyle_wide);
  //  }

  //  std::string displacement_texname;       // disp

  //  if (model.materials_[i].alpha_texname != "") {
  //    wide = converter.from_bytes(path_suffix + model.materials_[i].alpha_texname);
  //    wcscpy_s(cstyle_wide, wide.c_str());
  //    Texture::Inst()->LoadTexture(device, cstyle_wide);
  //  }

  //}
}
  
ID3D11Texture2D *Texture::CreateTexture2D(ID3D11Device* device,
  const D3D11_TEXTURE2D_DESC &text_desc,
  const D3D11_SHADER_RESOURCE_VIEW_DESC &shad_res_view_desc,
  const std::string &name) {

  // Attempt creation of texture
  ID3D11Texture2D *texture = nullptr;
  HRESULT result = device->CreateTexture2D(&text_desc, NULL,
    &texture);

  if (FAILED(result)) {
    MessageBox(NULL, L"Texture creation error,\
create texture 2D", L"ERROR", MB_OK);
    return nullptr;
  }
 
  // Attempt to create the shader resource view
  ID3D11ShaderResourceView *shad_res_view = nullptr;
  result = device->CreateShaderResourceView(texture, &shad_res_view_desc,
    &shad_res_view);

  if (FAILED(result)) {
    MessageBox(NULL, L"Shad Res View Creation error,\
create texture 2D", L"ERROR", MB_OK);
    texture->Release();
    texture = nullptr;
    return nullptr;
  }
  
  // Convert name to uint
  UInt32 crc_val = abfw::CRC::GetICRC(name.c_str());

  // Add res view to the map
  textures_[crc_val] = shad_res_view;

  return texture;
}