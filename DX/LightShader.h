// Light shader.h
// Basic single light shader setup
#ifndef _LIGHTSHADER_H
#define _LIGHTSHADER_H

#include "BaseShader.h"
#include "light.h"
#include "Camera.h"
#include <vector>
#include "Material.h"
#include "buffer_resource_manager.h"
#include "buffer_types.h"

using namespace std;
using namespace DirectX;


class LightShader : public BaseShader {
public:

  LightShader(ID3D11Device* device, HWND hwnd, 
    sz::ConstBufManager &buf_man, unsigned int lights_num);

  ~LightShader();

  // Sadly have to use by non-const reference paramenters, as the framework
  // most of the time does not define const accessors for its classes...
  void SetShaderParameters(ID3D11DeviceContext* deviceContext, 
    const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection,
    const sz::Material &mat);

  // Set the parameters which change only once per frame
  void SetShaderFrameParameters(ID3D11DeviceContext* deviceContext, 
    std::vector<Light> &hlights, Camera *cam);

  void Render(ID3D11DeviceContext* deviceContext,
    size_t index_count,
    size_t index_start = 0,
    size_t base_vertex = 0);

private:
  void InitShader(sz::ConstBufManager &buf_man, WCHAR*, WCHAR*, WCHAR* = nullptr, WCHAR* = nullptr, 
    unsigned int lights_num = 0);

private:
  ID3D11Buffer* m_matrixBuffer;
  ID3D11SamplerState* m_sampleState;
  ID3D11Buffer* m_lightBuffer;
  ID3D11Buffer* m_camBuffer;
  ID3D11Buffer* material_buf_;
};

#endif