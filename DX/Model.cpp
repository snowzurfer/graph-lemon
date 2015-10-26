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

Model::Model(const std::string &model_filename) :
  model_name_(model_filename) {
}

void Model::Init(ID3D11Device* device, ID3D11DeviceContext *dev_context,
  HWND hwnd, 
  szgrh::ConstBufManager &buf_man, unsigned int lights_num,
  szgrh::ShaderManager &shad_man) {

	// Initialize the vertex and index buffer that hold the geometry for the model.
	InitBuffers(device);

  // Load the textures for the materials
  LoadTextures_(device, dev_context, hwnd);

  // Load necessary shaders
  LoadShaders_(device, hwnd, buf_man, lights_num, shad_man);
}

Model::~Model()
{
	// Run parent deconstructor
	if (m_model)	{
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
bool FindReplace(std::wstring &str, const std::wstring &find_pattern,
  const std::wstring &rep_pattern) { 
  size_t index = 0; 
  index = str.find(find_pattern, index);
  
  if (index != std::wstring::npos) {
    str.replace(index, rep_pattern.length(), rep_pattern);

    return true;
  }

  return false;
}

// To the designed of the API:
// we are not oriental. No need for wchars. Like, no.
void Model::LoadTextures_(ID3D11Device* device, 
  ID3D11DeviceContext *dev_context, HWND hwnd) {
  // For each material
  for (unsigned int i = 0; i < materials_.size(); ++i) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wide;
    wide = converter.from_bytes(model_name_);
    std::wstring path_suffix = L"../res/" + wide + L"/";
  
    WCHAR cstyle_wide[128];

    if (materials_[i].ambient_texname != L"") {
      // Check if the texture name uses // as parenthesis and if so, fix it
      FindReplace(materials_[i].ambient_texname, L"\\", L"\/");
      //FindReplace(materials_[i].ambient_texname, L".tga", L".png");

      wide = path_suffix + materials_[i].ambient_texname;
      wcscpy_s(cstyle_wide, wide.c_str());
      Texture::Inst()->LoadTexture(device, dev_context, cstyle_wide);
      materials_[i].ambient_texname = wide;
    }

    if (materials_[i].diffuse_texname != L"") {
      // Check if the texture name uses // as parenthesis and if so, fix it
      FindReplace(materials_[i].diffuse_texname, L"\\", L"\/");
      //FindReplace(materials_[i].ambient_texname, L".tga", L".png");
      
      //wide = converter.from_bytes(path_suffix + materials_[i].diffuse_texname);
      wide = path_suffix + materials_[i].diffuse_texname;
      wcscpy_s(cstyle_wide, wide.c_str());
      Texture::Inst()->LoadTexture(device, dev_context, cstyle_wide);
      materials_[i].diffuse_texname = wide;
    }

    if (materials_[i].specular_texname != L"") {
      // Check if the texture name uses // as parenthesis and if so, fix it
      FindReplace(materials_[i].specular_texname, L"\\", L"\/");
      //FindReplace(materials_[i].ambient_texname, L".tga", L".png");
      
      //wide = converter.from_bytes(path_suffix + materials_[i].specular_texname);
      wide = path_suffix + materials_[i].specular_texname;
      wcscpy_s(cstyle_wide, wide.c_str());
      Texture::Inst()->LoadTexture(device, dev_context, cstyle_wide);
      materials_[i].specular_texname = wide;
    }


    std::string specular_highlight_texname; // map_Ns

    if (materials_[i].bump_texname != L"") {
      // Check if the texture name uses // as parenthesis and if so, fix it
      FindReplace(materials_[i].bump_texname, L"\\", L"\/");
      //FindReplace(materials_[i].ambient_texname, L".tga", L".png");
     
      //wide = converter.from_bytes(path_suffix + materials_[i].bump_texname);
      wide = path_suffix + materials_[i].bump_texname;
      wcscpy_s(cstyle_wide, wide.c_str());
      Texture::Inst()->LoadTexture(device, dev_context, cstyle_wide);
      materials_[i].bump_texname = wide;
    }

    std::string displacement_texname;       // disp

    if (materials_[i].alpha_texname != L"") {
      // Check if the texture name uses // as parenthesis and if so, fix it
      FindReplace(materials_[i].alpha_texname, L"\\", L"\/");
      //FindReplace(materials_[i].ambient_texname, L".tga", L".png");
      
      //wide = converter.from_bytes(path_suffix + materials_[i].alpha_texname);
      wide = path_suffix + materials_[i].alpha_texname;
      wcscpy_s(cstyle_wide, wide.c_str());
      Texture::Inst()->LoadTexture(device, dev_context, cstyle_wide);
      materials_[i].alpha_texname = wide;
    }
  }
}

void Model::LoadShaders_(ID3D11Device* device, HWND hwnd,
  szgrh::ConstBufManager &buf_man, unsigned int lights_num,
  szgrh::ShaderManager &shad_man) {
  // For each material
  for (unsigned int i = 0; i < materials_.size(); ++i) {

    // Determine which shader to instantiate
    if (materials_[i].diffuse_texname != L"" && materials_[i].bump_texname != L"" &&
      materials_[i].alpha_texname != L"" && materials_[i].specular_texname != L"") {
      NormalAlphaSpecMapShader *shader =
        new NormalAlphaSpecMapShader(device, hwnd, buf_man, lights_num);
      if (!shad_man.AddShader("normal_alpha_spec_map_shader", shader)) {
        delete shader;
        shader = nullptr;
      }

      materials_[i].shader_name = "normal_alpha_spec_map_shader";
    }
    else if (materials_[i].diffuse_texname != L"" && materials_[i].bump_texname != L"" &&
      materials_[i].alpha_texname != L"") {
      NormalAlphaMapShader *shader =
        new NormalAlphaMapShader(device, hwnd, buf_man, lights_num);
      if (!shad_man.AddShader("normal_alpha_map_shader", shader)) {
        delete shader;
        shader = nullptr;
      }

      materials_[i].shader_name = "normal_alpha_map_shader";
    }
    else if (materials_[i].diffuse_texname != L"" && materials_[i].bump_texname != L"") {
      NormalMappingShader *shader =
        new NormalMappingShader(device, hwnd, buf_man, lights_num);
      if (!shad_man.AddShader("normal_mapping_shader", shader)) {
        delete shader;
        shader = nullptr;
      }

      materials_[i].shader_name = "normal_mapping_shader";
    }
    else if (materials_[i].diffuse_texname != L"" && materials_[i].alpha_texname != L"" &&
      materials_[i].specular_texname != L"") {
      LightAlphaSpecMapShader *shader =
        new LightAlphaSpecMapShader(device, hwnd, buf_man, lights_num);
      if (!shad_man.AddShader("light_alpha_spec_map_shader", shader)) {
        delete shader;
        shader = nullptr;
      }
      materials_[i].shader_name = "light_alpha_spec_map_shader";

    }
    else if (materials_[i].diffuse_texname != L"" && materials_[i].alpha_texname != L"") {
      LightAlphaMapShader *shader =
        new LightAlphaMapShader(device, hwnd, buf_man, lights_num);
      if (!shad_man.AddShader("light_alpha_map_shader", shader)) {
        delete shader;
        shader = nullptr;
      }
      materials_[i].shader_name = "light_alpha_map_shader";

    }
    else if (materials_[i].diffuse_texname != L"" && materials_[i].specular_texname != L"") {
      LightSpecMapShader *shader =
        new LightSpecMapShader(device, hwnd, buf_man, lights_num);
      if (!shad_man.AddShader("light_spec_map_shader", shader)) {
        delete shader;
        shader = nullptr;
      }
      materials_[i].shader_name = "light_spec_map_shader";

    }
    else if (materials_[i].diffuse_texname != L"") {
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

void Model::InitBuffers(ID3D11Device* device) {
  for (size_t i = 0; i < meshes_.size();  ++i) {
    meshes_[i].InitBuffers(device);
  }
}

void Model::LoadModel(WCHAR* filename) {
	// Process model file
	//std::ifstream fileStream;
	//int fileSize = 0;

	//fileStream.open(filename, std::ifstream::in);

	//if (fileStream.is_open() == false)
	//	MessageBox(NULL, filename, L"Missing Model File", MB_OK);

	//fileStream.seekg(0, std::ios::end);
	//fileSize = (int)fileStream.tellg();
	//fileStream.seekg(0, std::ios::beg);

	//if (fileSize <= 0)
	//	MessageBox(NULL, filename, L"Model file empty", MB_OK);

	//char *buffer = new char[fileSize];

	//if (buffer == 0)
	//	MessageBox(NULL, filename, L"Model buffer is to small", MB_OK);

	//memset(buffer, '\0', fileSize);

	//TokenStream tokenStream, lineStream, faceStream;
	//std::string tempLine, token;

	//fileStream.read(buffer, fileSize);
	//tokenStream.SetTokenStream(buffer);

	//delete[] buffer;

	//tokenStream.ResetStream();

	//std::vector<float> verts, norms, texC;
	//std::vector<int> faces;


	//char lineDelimiters[2] = { '\n', ' ' };

	//while (tokenStream.MoveToNextLine(&tempLine))
	//{
	//	lineStream.SetTokenStream((char*)tempLine.c_str());
	//	tokenStream.GetNextToken(0, 0, 0);

	//	if (!lineStream.GetNextToken(&token, lineDelimiters, 2))
	//		continue;

	//	if (strcmp(token.c_str(), "v") == 0)
	//	{
	//		lineStream.GetNextToken(&token, lineDelimiters, 2);
	//		verts.push_back((float)atof(token.c_str()));

	//		lineStream.GetNextToken(&token, lineDelimiters, 2);
	//		verts.push_back((float)atof(token.c_str()));

	//		lineStream.GetNextToken(&token, lineDelimiters, 2);
	//		verts.push_back((float)atof(token.c_str()));
	//	}
	//	else if (strcmp(token.c_str(), "vn") == 0)
	//	{
	//		lineStream.GetNextToken(&token, lineDelimiters, 2);
	//		norms.push_back((float)atof(token.c_str()));

	//		lineStream.GetNextToken(&token, lineDelimiters, 2);
	//		norms.push_back((float)atof(token.c_str()));

	//		lineStream.GetNextToken(&token, lineDelimiters, 2);
	//		norms.push_back((float)atof(token.c_str()));
	//	}
	//	else if (strcmp(token.c_str(), "vt") == 0)
	//	{
	//		lineStream.GetNextToken(&token, lineDelimiters, 2);
	//		texC.push_back((float)atof(token.c_str()));

	//		lineStream.GetNextToken(&token, lineDelimiters, 2);
	//		texC.push_back((float)atof(token.c_str()));
	//	}
	//	else if (strcmp(token.c_str(), "f") == 0)
	//	{
	//		char faceTokens[3] = { '\n', ' ', '/' };
	//		std::string faceIndex;

	//		faceStream.SetTokenStream((char*)tempLine.c_str());
	//		faceStream.GetNextToken(0, 0, 0);

	//		for (int i = 0; i < 3; i++)
	//		{
	//			faceStream.GetNextToken(&faceIndex, faceTokens, 3);
	//			faces.push_back((int)atoi(faceIndex.c_str()));

	//			faceStream.GetNextToken(&faceIndex, faceTokens, 3);
	//			faces.push_back((int)atoi(faceIndex.c_str()));

	//			faceStream.GetNextToken(&faceIndex, faceTokens, 3);
	//			faces.push_back((int)atoi(faceIndex.c_str()));
	//		}
	//	}
	//	else if (strcmp(token.c_str(), "#") == 0)
	//	{
	//		int a = 0;
	//		int b = a;
	//	}

	//	token[0] = '\0';
	//}

	//// "Unroll" the loaded obj information into a list of triangles.

	//int vIndex = 0, nIndex = 0, tIndex = 0;
	//int numFaces = (int)faces.size() / 9;

	////// Create the model using the vertex count that was read in.
	//m_vertexCount = numFaces * 3;
	//m_model = new ModelType[m_vertexCount];
	//
	//for (int f = 0; f < (int)faces.size(); f += 3)
	//{
	//	m_model[vIndex].x = verts[(faces[f + 0] - 1) * 3 + 0];
	//	m_model[vIndex].y = verts[(faces[f + 0] - 1) * 3 + 1];
	//	m_model[vIndex].z = verts[(faces[f + 0] - 1) * 3 + 2];
	//	m_model[vIndex].tu = texC[(faces[f + 1] - 1) * 2 + 0];
	//	m_model[vIndex].tv = texC[(faces[f + 1] - 1) * 2 + 1];
	//	m_model[vIndex].nx = norms[(faces[f + 2] - 1) * 3 + 0];
	//	m_model[vIndex].ny = norms[(faces[f + 2] - 1) * 3 + 1];
	//	m_model[vIndex].nz = norms[(faces[f + 2] - 1) * 3 + 2];

	//	//increase index count
	//	vIndex++;

	//}
	//m_indexCount = vIndex;

	//verts.clear();
	//norms.clear();
	//texC.clear();
	//faces.clear();
}
