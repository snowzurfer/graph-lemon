// Normal mapping shader 
// Shader which uses normal maps to perform per-pixel lighting 
#ifndef _NORMAL_MAPPING_SHADER_H
#define _NORMAL_MAPPING_SHADER_H 

#include "BaseShader.h"
#include "light.h"
#include "Camera.h"
#include <vector>
#include "Material.h"

using namespace std;
using namespace DirectX;

//const unsigned int kNumLights = 4;

class NormalMappingShader : public BaseShader
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

public:

	NormalMappingShader(ID3D11Device* device, HWND hwnd, 
    szgrh::ConstBufManager &buf_man, unsigned int lights_num);
	~NormalMappingShader();

  // Sadly have to use by non-const reference paramenters, as the framework
  // most of the time does not define const accessors for its classes...
	void SetShaderParameters(ID3D11DeviceContext* deviceContext, 
    const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection,
    const szgrh::Material &mat);

  // Set the parameters which change only once per frame
  void SetShaderFrameParameters(ID3D11DeviceContext* deviceContext, 
    std::vector<Light> &hlights, Camera *cam, float time);

	void Render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void InitShader(szgrh::ConstBufManager &buf_man, WCHAR*, WCHAR*, 
    unsigned int lights_num);

private:
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_camBuffer;
  ID3D11Buffer* material_buf_;
};

#endif
