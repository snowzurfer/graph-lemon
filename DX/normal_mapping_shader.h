// Normal mapping shader 
// Shader which uses normal maps to perform per-pixel lighting 
#ifndef _NORMAL_MAPPING_SHADER_H
#define _NORMAL_MAPPING_SHADER_H 

#include "BaseShader.h"
#include "light.h"
#include "Camera.h"
#include <vector>
#include "Material.h"
#include "buffer_types.h"

using namespace std;
using namespace DirectX;

//const unsigned int kNumLights = 4;

class NormalMappingShader : public BaseShader
{
private:

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
    std::vector<Light> &hlights, Camera *cam);

	void Render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void InitShader(szgrh::ConstBufManager &buf_man,
    unsigned int lights_num);

private:
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_camBuffer;
  ID3D11Buffer* material_buf_;
};

#endif
