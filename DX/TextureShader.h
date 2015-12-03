// texture shader.h
#ifndef _TEXTURESHADER_H_
#define _TEXTURESHADER_H_

#include "BaseShader.h"
#include "buffer_resource_manager.h"
#include "Material.h"

using namespace std;
using namespace DirectX;


class TextureShader: public BaseShader {

public:

  TextureShader(ID3D11Device* device, HWND hwnd, 
    sz::ConstBufManager &buf_man);
  ~TextureShader();

  void SetShaderFrameParameters(ID3D11DeviceContext* deviceContext);

  void SetShaderParameters(ID3D11DeviceContext* deviceContext, 
    const XMMATRIX &world, const XMMATRIX &view, 
    const XMMATRIX &projection, const sz::Material &mat);
  
  void Render(ID3D11DeviceContext* deviceContext,
    size_t index_count,
    size_t index_start = 0,
    size_t base_vertex = 0);

  //void CleanupTextures(ID3D11DeviceContext* deviceContext);

private:
  void InitShader(sz::ConstBufManager &buf_man, WCHAR*, WCHAR*);
  
private:
  ID3D11Buffer* m_matrixBuffer;
  ID3D11SamplerState* m_sampleState;
  ID3D11Buffer* material_buf_;
};

#endif