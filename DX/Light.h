#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <directxmath.h>

using namespace DirectX;

class Light
{

public:
  // Ctor
  Light();

  void* operator new(size_t i)
  {
    return _mm_malloc(i, 16);
  }

  void operator delete(void* p)
  {
    _mm_free(p);
  }

  void GenerateViewMatrix();
  void GenerateViewMatrixFromDirection();
  void GenerateProjectionMatrix(float, float);
  void UpdateFromGui(size_t light_num);

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
  void set_spot_cutoff(float v);
  void set_spot_exponent(float v);

  // Getters
  XMFLOAT4A GetAmbientColour();
  XMFLOAT4A GetDiffuseColour();
  XMFLOAT3A GetDirection();
  XMFLOAT4A GetSpecularColour();
  float GetSpecularPower();
  XMFLOAT3A GetPosition3() const;
  const XMFLOAT4A &GetPosition4() const;
  XMVECTOR GetPosVector();
  XMMATRIX GetViewMatrix();
  XMMATRIX GetProjectionMatrix();
  const XMFLOAT3A &GetAttenuation() const;
  float GetRange();

  bool active() const;
  float spot_cutoff() const;
  float spot_exponent() const;

protected:
  XMFLOAT4A m_ambientColour;
  XMFLOAT4A m_diffuseColour;
  XMFLOAT3A m_direction;
  XMFLOAT4A m_specularColour;
  float m_specularPower;
  XMFLOAT4A m_position;
  XMFLOAT4X4A m_viewMatrix;
  XMFLOAT4X4A m_projectionMatrix;
  XMFLOAT4A m_lookAt;
  XMFLOAT3A m_attenuation;
  float m_range;
 
  // Whether the light is active or not
  bool active_;

  // The cutoff value for spotlights; set to
  // 180 by default, meaning by default the light is not a spotlight
  float spot_cutoff_;

  // The exponent value for spotlights; determine how concentrated
  // the lightbeam is; by default it's set to 1
  float spot_exponent_;
};

#endif