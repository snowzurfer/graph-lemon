// Waves vertex deforming shader
// Simple shader which deforms the geometry using a combination of sin and cos
// waves
#ifndef _WAVES_VERTEX_DEFORM_SHADER_H
#define _WAVES_VERTEX_DEFORM_SHADER_H

#include "BaseShader.h"
#include "light.h"
#include "Camera.h"
#include <vector>
#include "Material.h"

using namespace std;
using namespace DirectX;

//const unsigned int kNumLights = 4;

class WavesVertexDeformShader : public BaseShader
{
private:

  struct LightBufferType {
    XMFLOAT4 diffuse;
    XMFLOAT4 ambient;
    XMFLOAT4 direction;
    XMFLOAT4 specular;
    XMFLOAT4 attenuation;
    XMFLOAT4 position;
    // Determines which light is active
    unsigned int active;
    float range;
    float specular_power;
    // Spot cutoff has 2 ranges/values:
    // 180.0 = not a spotlight
    // [0.f, 90.f] = spotlight
    float spot_cutoff;
    float spot_exponent;
    XMFLOAT3 padding;
  };

  struct CamBufferType {
    XMFLOAT3 camPos;
    float padding;
  };

  struct MaterialBufferType {
    XMFLOAT4 ambient;
    XMFLOAT4 diffuse;
    XMFLOAT4 specular;
    XMFLOAT4 transmittance;
    XMFLOAT4 emission;
    float shininess;
    float ior;      // index of refraction
    float dissolve; // 1 == opaque; 0 == fully transparent
    int illum;
  };

  // Buffer which contains time, amplitude and frequency to be used 
  // to displace the vertices using a sine function
  struct TimeAmpFreqBufferType {
    float time;
    float amplitude;
    float speed;
    float padding;
  };

public:

  WavesVertexDeformShader(ID3D11Device* device, HWND hwnd, 
    sz::ConstBufManager &buf_man, unsigned int lights_num);
  ~WavesVertexDeformShader();

  // Sadly have to use by non-const reference paramenters, as the framework
  // most of the time does not define const accessors for its classes...
  void SetShaderParameters(ID3D11DeviceContext* deviceContext, 
    const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection,
    const sz::Material &mat);

  // Set the parameters which change only once per frame
  void SetShaderFrameParameters(ID3D11DeviceContext* deviceContext, 
    std::vector<Light> &hlights, Camera *cam, float time);

  void Render(ID3D11DeviceContext* deviceContext,
    size_t index_count,
    size_t index_start = 0,
    size_t base_vertex = 0);

private:
  void InitShader(sz::ConstBufManager &buf_man, WCHAR*, WCHAR*, 
    unsigned int lights_num);

private:
  ID3D11Buffer* m_matrixBuffer;
  ID3D11SamplerState* m_sampleState;
  ID3D11Buffer* m_lightBuffer;
  ID3D11Buffer* m_camBuffer;
  ID3D11Buffer* material_buf_;
  ID3D11Buffer* time_buf_;
};

#endif
