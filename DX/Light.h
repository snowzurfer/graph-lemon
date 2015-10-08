#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <directxmath.h>

using namespace DirectX;

class Light
{

public:
	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}

	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float, float);

	// Setters
	void SetAmbientColour(float, float, float, float);
	void SetDiffuseColour(float, float, float, float);
	void SetDirection(float, float, float);
	void SetSpecularColour(float, float, float, float);
	void SetSpecularPower(float);
	void SetPosition(float, float, float);
  void SetPosition(float, float, float, float);
	void SetLookAt(float, float, float);
  void SetAttenuation(float, float, float);
  void SetRange(float);

  void set_active(bool v);

	// Getters
	XMFLOAT4 GetAmbientColour();
	XMFLOAT4 GetDiffuseColour();
	XMFLOAT3 GetDirection();
	XMFLOAT4 GetSpecularColour();
	float GetSpecularPower();
	XMFLOAT3 GetPosition3();
  XMFLOAT4 GetPosition4();
  XMVECTOR GetPosVector();
	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix();
  XMFLOAT3 GetAttenuation();
  float GetRange();

  bool active() const;

protected:
	XMFLOAT4 m_ambientColour;
	XMFLOAT4 m_diffuseColour;
	XMFLOAT3 m_direction;
	XMFLOAT4 m_specularColour;
	float m_specularPower;
	XMVECTOR m_position;
	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;
	XMVECTOR m_lookAt;
  XMFLOAT3 m_attenuation;
  float m_range;
 
  // Whether the light is active or not
  bool active_;
};

#endif