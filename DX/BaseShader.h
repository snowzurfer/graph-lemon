// testure shader.h
#ifndef _BASESHADER_H_
#define _BASESHADER_H_


#include <d3d11.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <fstream>
#include "buffer_resource_manager.h"
#include "Material.h"
#include "buffer_types.h"

using namespace std;
using namespace DirectX;


class BaseShader
{
protected:

public:
  void* operator new(size_t i)
  {
    return _mm_malloc(i, 16);
  }

  void operator delete(void* p)
  {
    _mm_free(p);
  }

  BaseShader(ID3D11Device* device, HWND hwnd);
  virtual ~BaseShader();

  virtual void Render(ID3D11DeviceContext* deviceContext,
    size_t index_count,
    size_t index_start = 0,
    size_t base_vertex = 0);

  // Set the DX shaders and the input layout for this shader class
  void SetInputLayoutAndShaders(ID3D11DeviceContext* deviceContext);

  // Set samplers for the shaders
  virtual void SetSamplers(ID3D11DeviceContext* deviceContext) {};

  // Sadly have to use by non-const reference paramenters, as the framework
  // most of the time does not define const accessors for its classes...
  virtual void SetShaderParameters(ID3D11DeviceContext* deviceContext, 
    const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection,
    const sz::Material &mat);

  void CleanupTextures(ID3D11DeviceContext* deviceContext);

  // Activate tessellation on this shader
  void ActivateTessellation(ID3D11DeviceContext* deviceContext);
  void DeactivateTessellation(ID3D11DeviceContext* deviceContext);

  // Activate waves deformation on this shader
  virtual void ActivateWavesDeformation(ID3D11DeviceContext* deviceContext) {};
  virtual void DeactivateWavesDeformation(ID3D11DeviceContext* deviceContext) {};

protected:
  void InitShader(WCHAR*, WCHAR*) {};
  void ShutdownShader();
  void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
  void loadVertexShader(const D3D11_INPUT_ELEMENT_DESC *layout,
    size_t num_elements, WCHAR* filename);
  void loadVertexShader(WCHAR* filename, ID3D11VertexShader **shader);
  void loadHullShader(WCHAR* filename);
  void loadDomainShader(WCHAR* filename);
  void loadGeometryShader(WCHAR* filename);
  void loadPixelShader(WCHAR* filename);

protected:
  ID3D11Device* m_device;
  HWND m_hwnd;

  ID3D11VertexShader* m_vertexShader;
  ID3D11VertexShader* vertexshader_standard;
  ID3D11VertexShader* vertexshader_tessellation;
  ID3D11PixelShader* m_pixelShader;
  ID3D11HullShader* m_hullShader;
  ID3D11DomainShader* m_domainShader;
  ID3D11GeometryShader* m_geometryShader;
  ID3D11InputLayout* m_layout;
  ID3D11Buffer* m_matrixBuffer;
  ID3D11SamplerState* m_sampleState;

  bool tessellate_;
  
 // ID3D11Buffer* m_matrixBuffer;
  //ID3D11SamplerState* m_sampleState;
  //ID3D11Buffer* m_lightBuffer;
  //ID3D11Buffer* m_camBuffer;
 // ID3D11Buffer* material_buf_;
};

#endif