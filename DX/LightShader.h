// Light shader.h
// Basic single light shader setup
#ifndef _LIGHTSHADER_H_
#define _LIGHTSHADER_H_

#include "BaseShader.h"
#include "light.h"
#include "Camera.h"
#include <vector>

using namespace std;
using namespace DirectX;

const unsigned int kNumLights = 4;

class LightShader : public BaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 diffuse[kNumLights];
    XMFLOAT4 ambient[kNumLights];
		XMFLOAT4 direction[kNumLights];
    XMFLOAT4 specularColour[kNumLights];
    XMFLOAT4 attenuation[kNumLights];
    XMFLOAT4 position[kNumLights];
    // Determines which light is active
    float active[kNumLights];
    float range[kNumLights];
    float specularPower[kNumLights];
	};

  struct CamBufferType {
    XMFLOAT3 camPos;
    float padding;
  };

public:

	LightShader(ID3D11Device* device, HWND hwnd);
	~LightShader();

  // Sadly have to use by non-const reference paramenters, as the framework
  // most of the time does not define const accessors for its classes...
	void SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, std::vector<Light> &hlights, Camera *cam);
	void Render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void InitShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_camBuffer;
};

#endif