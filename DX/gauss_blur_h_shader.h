#ifndef _GAUSS_BLUR_H_SHADER_H
#define _GAUSS_BLUR_H_SHADER_H

#include "BaseShader.h"
#include "buffer_resource_manager.h"
#include "Material.h"

using namespace std;
using namespace DirectX;

class GaussBlurHShader: public BaseShader {

public:

  GaussBlurHShader(ID3D11Device* device, HWND hwnd, 
    szgrh::ConstBufManager &buf_man);
  ~GaussBlurHShader();

  void SetShaderFrameParameters(ID3D11DeviceContext* deviceContext);

  void SetShaderParameters(ID3D11DeviceContext* deviceContext, 
    const XMMATRIX &world, const XMMATRIX &view, 
    const XMMATRIX &projection, const szgrh::Material &mat,
    const float screensize);
  
  void Render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
  void InitShader(szgrh::ConstBufManager &buf_man, WCHAR*, WCHAR*);
  
private:
  ID3D11Buffer* m_matrixBuffer;
  ID3D11SamplerState* m_sampleState;
  ID3D11Buffer *screensize_buf_;
};

#endif
