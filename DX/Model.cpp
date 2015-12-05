// Model mesh and load
// Loads a .obj and creates a mesh object from the data
#include "model.h"
#include <locale>
#include <codecvt>
#include <string>
#include "Texture.h"
#include "LightShader.h"
#include "normal_mapping_shader.h"
#include "light_alpha_map_shader.h"
#include "light_spec_map_shader.h"
#include "light_alpha_spec_map_shader.h"
#include "normal_alpha_map_shader.h"
#include "normal_alpha_spec_map_shader.h"
#include "abertay_framework.h"
#include "crc.h"
#include "Material.h"

Model::Model(const std::string &model_filename) :
  model_name_(model_filename) {
}

void Model::Init(ID3D11Device* device, ID3D11DeviceContext *dev_context,
  HWND hwnd, 
  sz::ConstBufManager &buf_man, unsigned int lights_num,
  sz::ShaderManager &shad_man) {

  // Initialize the vertex and index buffer that hold the geometry for the model.
  InitBuffers(device, dev_context);

  // Load the textures for the materials
  LoadTextures_(device, dev_context, hwnd);

  // Load necessary shaders
  LoadShaders_(device, hwnd, buf_man, lights_num, shad_man);

  AddMeshesAndMaterials(meshes_, materials_);
}

Model::~Model()
{
  // Run parent deconstructor
  if (m_model)  {
    delete[] m_model;
    m_model = 0;
  }
}

// Simple helper function which checks for the presence of a certain
// pattern in a string and replaces it with another one if it was found.
// Returns whether the pattern was found and replaced or not.
bool FindReplace(std::string &str, const std::string &find_pattern,
  const std::string &rep_pattern) { 
  size_t index = 0; 
  index = str.find(find_pattern, index);
  
  if (index != std::string::npos) {
    str.replace(index, rep_pattern.length(), rep_pattern);

    return true;
  }

  return false;
}

std::wstring ConvertToWide(const std::string &x) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  std::wstring full_path = converter.from_bytes(x);

  return full_path;
}

// To the designed of the API:
// we are not oriental. No need for wchars. Like, no.
void Model::LoadTextures_(ID3D11Device* device, 
  ID3D11DeviceContext *dev_context, HWND hwnd) {
  // For each material
  for (unsigned int i = 0; i < materials_.size(); ++i) {
    //std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    //std::wstring full_path;
    //full_path = converter.from_bytes(model_name_);
    std::string path_suffix = "../res/" + model_name_ + "/";
    std::string full_path;
    UInt32 full_path_crc = 0;

    if (materials_[i].ambient_texname != "") {
      // Check if the texture name uses // as parenthesis and if so, fix it
      FindReplace(materials_[i].ambient_texname, "\\", "\/");
      //FindReplace(materials_[i].ambient_texname, ".tga", ".png");

      full_path = path_suffix + materials_[i].ambient_texname;

      Texture::Inst()->LoadTexture(device, dev_context, full_path);
      materials_[i].ambient_texname = full_path;
      materials_[i].ambient_texname_crc = abfw::CRC::GetICRC(full_path.c_str());
    }

    if (materials_[i].diffuse_texname != "") {
      // Check if the texture name uses // as parenthesis and if so, fix it
      FindReplace(materials_[i].diffuse_texname, "\\", "\/");
      //FindReplace(materials_[i].ambient_texname, ".tga", ".png");
      
      //full_path = converter.from_bytes(path_suffix + materials_[i].diffuse_texname);
      full_path = path_suffix + materials_[i].diffuse_texname;

      Texture::Inst()->LoadTexture(device, dev_context, full_path);
      materials_[i].diffuse_texname = full_path;
      materials_[i].diffuse_texname_crc = abfw::CRC::GetICRC(full_path.c_str());
    }

    if (materials_[i].specular_texname != "") {
      // Check if the texture name uses // as parenthesis and if so, fix it
      FindReplace(materials_[i].specular_texname, "\\", "\/");
      //FindReplace(materials_[i].ambient_texname, ".tga", ".png");
      
      //full_path = converter.from_bytes(path_suffix + materials_[i].specular_texname);
      full_path = path_suffix + materials_[i].specular_texname;

      Texture::Inst()->LoadTexture(device, dev_context, full_path);
      materials_[i].specular_texname = full_path;
      materials_[i].specular_texname_crc = abfw::CRC::GetICRC(full_path.c_str());
    }


    std::string specular_highlight_texname; // map_Ns

    if (materials_[i].bump_texname != "") {
      // Check if the texture name uses // as parenthesis and if so, fix it
      FindReplace(materials_[i].bump_texname, "\\", "\/");
      //FindReplace(materials_[i].ambient_texname, ".tga", ".png");
     
      //full_path = converter.from_bytes(path_suffix + materials_[i].bump_texname);
      full_path = path_suffix + materials_[i].bump_texname;

      Texture::Inst()->LoadTexture(device, dev_context, full_path);
      materials_[i].bump_texname = full_path;
      materials_[i].bump_texname_crc = abfw::CRC::GetICRC(full_path.c_str());
    }

    std::string displacement_texname;       // disp

    if (materials_[i].alpha_texname != "") {
      // Check if the texture name uses // as parenthesis and if so, fix it
      FindReplace(materials_[i].alpha_texname, "\\", "\/");
      //FindReplace(materials_[i].ambient_texname, ".tga", ".png");
      
      //full_path = converter.from_bytes(path_suffix + materials_[i].alpha_texname);
      full_path = path_suffix + materials_[i].alpha_texname;

      Texture::Inst()->LoadTexture(device, dev_context, full_path);
      materials_[i].alpha_texname = full_path;
      materials_[i].alpha_texname_crc = abfw::CRC::GetICRC(full_path.c_str());
    }
  }
}

void Model::LoadShaders_(ID3D11Device* device, HWND hwnd,
  sz::ConstBufManager &buf_man, unsigned int lights_num,
  sz::ShaderManager &shad_man) {
  // For each material
  for (unsigned int i = 0; i < materials_.size(); ++i) {
    // Determine which shader to instantiate
    if (materials_[i].diffuse_texname != "" && materials_[i].bump_texname != "" &&
      materials_[i].alpha_texname != "" && materials_[i].specular_texname != "") {
      NormalAlphaSpecMapShader *shader =
        new NormalAlphaSpecMapShader(device, hwnd, buf_man, lights_num);
      if (!shad_man.AddShader("normal_alpha_spec_map_shader", shader)) {
        delete shader;
        shader = nullptr;
      }

      materials_[i].shader_name = "normal_alpha_spec_map_shader";
    }
    else if (materials_[i].diffuse_texname != "" && materials_[i].bump_texname != "" &&
      materials_[i].alpha_texname != "") {
      NormalAlphaMapShader *shader =
        new NormalAlphaMapShader(device, hwnd, buf_man, lights_num);
      if (!shad_man.AddShader("normal_alpha_map_shader", shader)) {
        delete shader;
        shader = nullptr;
      }

      materials_[i].shader_name = "normal_alpha_map_shader";
    }
    else if (materials_[i].diffuse_texname != "" && materials_[i].bump_texname != "") {
      NormalMappingShader *shader =
        new NormalMappingShader(device, hwnd, buf_man, lights_num);
      if (!shad_man.AddShader("normal_mapping_shader", shader)) {
        delete shader;
        shader = nullptr;
      }

      materials_[i].shader_name = "normal_mapping_shader";
    }
    else if (materials_[i].diffuse_texname != "" && materials_[i].alpha_texname != "" &&
      materials_[i].specular_texname != "") {
      LightAlphaSpecMapShader *shader =
        new LightAlphaSpecMapShader(device, hwnd, buf_man, lights_num);
      if (!shad_man.AddShader("light_alpha_spec_map_shader", shader)) {
        delete shader;
        shader = nullptr;
      }
      materials_[i].shader_name = "light_alpha_spec_map_shader";

    }
    else if (materials_[i].diffuse_texname != "" && materials_[i].alpha_texname != "") {
      LightAlphaMapShader *shader =
        new LightAlphaMapShader(device, hwnd, buf_man, lights_num);
      if (!shad_man.AddShader("light_alpha_map_shader", shader)) {
        delete shader;
        shader = nullptr;
      }
      materials_[i].shader_name = "light_alpha_map_shader";

    }
    else if (materials_[i].diffuse_texname != "" && materials_[i].specular_texname != "") {
      LightSpecMapShader *shader =
        new LightSpecMapShader(device, hwnd, buf_man, lights_num);
      if (!shad_man.AddShader("light_spec_map_shader", shader)) {
        delete shader;
        shader = nullptr;
      }
      materials_[i].shader_name = "light_spec_map_shader";

    }
    else if (materials_[i].diffuse_texname != "") {
      LightShader *shader =
        new LightShader(device, hwnd, buf_man, lights_num);
      if (!shad_man.AddShader("light_shader", shader)) {
        delete shader;
        shader = nullptr;
      }
      materials_[i].shader_name = "light_shader";

    }
  }
}

void Model::InitBuffers(ID3D11Device* device,
  ID3D11DeviceContext* deviceContext) {
  D3D11_BUFFER_DESC vertex_buf_desc, index_buf_desc;
  D3D11_SUBRESOURCE_DATA vertex_data, index_data;

  // Create subresources and fill them with data from the meshes
  D3D11_MAPPED_SUBRESOURCE mapped_resource;
  std::vector<VertexType> vertices;
  std::vector<unsigned int> indices;

  // Load the buffers with data from the meshes
  for (size_t i = 0; i < meshes_.size(); ++i) {
    meshes_[i].InitBuffers(vertices, indices);
  }

  // Create the vertex buffer
  vertex_buf_desc.Usage = D3D11_USAGE_DEFAULT;
  vertex_buf_desc.ByteWidth = sizeof(VertexType) * vertices.size();
  vertex_buf_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertex_buf_desc.CPUAccessFlags = 0;
  vertex_buf_desc.MiscFlags = 0;
  vertex_buf_desc.StructureByteStride = 0;

  vertex_data.pSysMem = vertices.data();
  vertex_data.SysMemPitch = 0;
  vertex_data.SysMemSlicePitch = 0;

  HRESULT hr = device->CreateBuffer(&vertex_buf_desc, &vertex_data,
    &vertex_buf_);
  if (hr != S_OK) {
    std::cout << hr << std::endl;
  }

  // Create the index buffer
  index_buf_desc.Usage = D3D11_USAGE_DEFAULT;
  index_buf_desc.ByteWidth = sizeof(unsigned int) * indices.size();
  index_buf_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  index_buf_desc.CPUAccessFlags = 0;
  index_buf_desc.MiscFlags = 0;
  index_buf_desc.StructureByteStride = 0;

  index_data.pSysMem = indices.data();
  index_data.SysMemPitch = 0;
  index_data.SysMemSlicePitch = 0;

  hr = device->CreateBuffer(&index_buf_desc, &index_data, &index_buf_);
  if (hr != S_OK) {
    std::cout << hr << std::endl;
  }
}

void Model::SendData(ID3D11DeviceContext* dev_context, bool tessellate) {
  unsigned int stride;
  unsigned int offset;

  // Set vertex buffer stride and offset.
  stride = sizeof(VertexType);
  offset = 0;

  // Set the vertex buffer to active in the input assembler so it can be rendered.
  dev_context->IASetVertexBuffers(0, 1, &vertex_buf_, &stride, &offset);

  // Set the index buffer to active in the input assembler so it can be rendered.
  dev_context->IASetIndexBuffer(index_buf_, DXGI_FORMAT_R32_UINT, 0);

  // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
  if (tessellate) {
    dev_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
  }
  else {
    dev_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  }
}

void Model::AddMeshesAndMaterials(std::vector<BaseMesh> &meshes,
  const std::vector<sz::Material> &materials) {
  for (size_t i = 0; i < meshes.size(); ++i) {
    // Get the mesh's material's crc name
    size_t m_id = meshes[i].mat_id();
    UInt32 m_crc = materials[m_id].name_crc;

    // Add the mesh in the correct slot
    sz::MeshesMatMap::iterator it = meshes_by_material_.find(m_crc);
    if (it != meshes_by_material_.end()) {
      sz::MatMeshPair &pair =
        it->second;

      pair.second.push_back(&(meshes[i]));
    }
    else {
      sz::MatMeshPair pair(&materials[m_id], std::vector<BaseMesh *>());

      pair.second.push_back(&meshes[i]);

      meshes_by_material_[m_crc] = pair;
    }
  }
}
