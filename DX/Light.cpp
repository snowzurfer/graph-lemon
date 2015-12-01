// Light class
// Holds data that represents a light source

#include "light.h"
#define _USE_MATH_DEFINES
#include <math.h>

Light::Light() :
    spot_cutoff_(M_PI),
    spot_exponent_(1.f) {

}

void Light::GenerateViewMatrix()
{
  XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

  //up = XMVectorSetX(up, 0.0f);
  //up = XMVectorSetY(up, 1.0f);
  //up = XMVectorSetZ(up, 0.0f);

  // Create the view matrix from the three vectors.
  m_viewMatrix = XMMatrixLookAtLH(m_position, m_lookAt, up);
}

void Light::GenerateViewMatrixFromDirection()
{
  XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

  //up = XMVectorSetX(up, 0.0f);
  //up = XMVectorSetY(up, 1.0f);
  //up = XMVectorSetZ(up, 0.0f);
  XMVECTOR dir = XMVectorSet(m_direction.x,
    m_direction.y,
    m_direction.z,
    1.f);

  // Create the view matrix from the three vectors.
  m_viewMatrix = XMMatrixLookToLH(m_position, dir, up);
}

void Light::GenerateProjectionMatrix(float screenNear, float screenFar)
{
  float fieldOfView, screenAspect;

  // Setup field of view and screen aspect for a square light source.
  fieldOfView = (float)XM_PI / 2.0f;
  screenAspect = 1.0f;

  // Create the projection matrix for the light.
  m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenFar);
}

void Light::SetAmbientColour(float red, float green, float blue, float alpha)
{
  m_ambientColour = XMFLOAT4(red, green, blue, alpha);
}

void Light::SetDiffuseColour(float red, float green, float blue, float alpha)
{
  m_diffuseColour = XMFLOAT4(red, green, blue, alpha);
}


void Light::SetDirection(float x, float y, float z)
{
  float magnitude = sqrtf(x * x + y * y + z * z);
  m_direction = XMFLOAT3(x / magnitude,
    y / magnitude, z / magnitude);
}

void Light::SetSpecularColour(float red, float green, float blue, float alpha)
{
  m_specularColour = XMFLOAT4(red, green, blue, alpha);
}


void Light::SetSpecularPower(float power)
{
  m_specularPower = power;
}

void Light::SetPosition(float x, float y, float z)
{
  m_position = XMVectorSet(x, y, z, 1.0f);
}

void Light::SetPosition(float x, float y, float z, float w) {
  m_position = XMVectorSet(x, y, z, w);
}

void Light::SetAttenuation(float x, float y, float z) {
  m_attenuation = XMFLOAT3(x, y, z);
}

void Light::SetRange(float x) {
  m_range = x;
}

XMFLOAT4 Light::GetAmbientColour()
{
  return m_ambientColour;
}

XMFLOAT4 Light::GetDiffuseColour()
{
  return m_diffuseColour;
}


XMFLOAT3 Light::GetDirection()
{
  return m_direction;
}

XMFLOAT4 Light::GetSpecularColour()
{
  return m_specularColour;
}


float Light::GetSpecularPower()
{
  return m_specularPower;
}

XMFLOAT3 Light::GetPosition3()
{
  XMFLOAT3 temp(XMVectorGetX(m_position), XMVectorGetY(m_position), XMVectorGetZ(m_position));
  return temp;
}

XMFLOAT4 Light::GetPosition4() {
  XMFLOAT4 temp(XMVectorGetX(m_position), XMVectorGetY(m_position), 
    XMVectorGetZ(m_position), XMVectorGetW(m_position));
  return temp;
}

void Light::SetLookAt(float x, float y, float z)
{
  m_lookAt = XMVectorSet(x, y, z, 1.0f);
}

XMMATRIX Light::GetViewMatrix()
{
  return m_viewMatrix;
}

XMMATRIX Light::GetProjectionMatrix()
{
  return m_projectionMatrix;
}

XMVECTOR Light::GetPosVector() {
  return m_position;
}

XMFLOAT3 Light::GetAttenuation() {
  return m_attenuation;
}

float Light::GetRange() {
  return m_range;
}

bool Light::active() const {
  return active_;
}

float Light::spot_cutoff() const {
  return spot_cutoff_;
}

float Light::spot_exponent() const {
  return spot_exponent_;
}



void Light::set_active(bool v) {
  active_ = v;
}

void Light::set_spot_cutoff(float v) {
  spot_cutoff_ = v;
}

void Light::set_spot_exponent(float v) {
  spot_exponent_ = v;
}