// Light class
// Holds data that represents a light source

#include "light.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <imgui.h>
#include <cstdio>

Light::Light() :
  m_ambientColour(0.f, 0.f, 0.f, 0.f),
  m_diffuseColour(0.f, 0.f, 0.f, 0.f),
  m_direction(0.f, 0.f, 0.f),
  m_specularColour(0.f, 0.f, 0.f, 0.f),
  m_specularPower(0.f),
  m_position(),
  m_viewMatrix(),
  m_projectionMatrix(),
  m_lookAt(),
  m_attenuation(0.f, 0.f, 0.f),
  m_range(0.f),
  active_(false),
  spot_cutoff_(static_cast<float>(M_PI)),
  spot_exponent_(1.f) {

}

void Light::UpdateFromGui(size_t light_num) {
  char buf[25];
  sprintf_s(buf, "Light number %d", light_num);
  //if (ImGui::CollapsingHeader(buf)) {
    //ImGui::SliderFloat("Light direction X", &this->m_direction.x);
    //ImGui::SliderFloat("Light direction Y", &this->m_direction.y);
    //ImGui::SliderFloat("Light direction Z", &this->m_direction.z);
    ImGui::Checkbox("Light active", &this->active_);
  
  //}
}

void Light::GenerateViewMatrix() {
  XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

  //up = XMVectorSetX(up, 0.0f);
  //up = XMVectorSetY(up, 1.0f);
  //up = XMVectorSetZ(up, 0.0f);

  // Create the view matrix from the three vectors.
  XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat4A(&m_position),
    XMLoadFloat4A(&m_lookAt), up);
  XMStoreFloat4x4A(&m_viewMatrix, view);

}

void Light::GenerateViewMatrixFromDirection() {
  XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

  //up = XMVectorSetX(up, 0.0f);
  //up = XMVectorSetY(up, 1.0f);
  //up = XMVectorSetZ(up, 0.0f);
  XMVECTOR dir = XMVectorSet(m_direction.x,
    m_direction.y,
    m_direction.z,
    1.f);

  // Create the view matrix from the three vectors.
  XMMATRIX view = XMMatrixLookToLH(XMLoadFloat4A(&m_position), dir, up);
  XMStoreFloat4x4A(&m_viewMatrix, view);
}

void Light::GenerateProjectionMatrix(float screenNear, float screenFar)
{
  float fieldOfView, screenAspect;

  // Setup field of view and screen aspect for a square light source.
  fieldOfView = (float)XM_PI / 2.0f;
  screenAspect = 1.0f;

  // Create the projection matrix for the light.
  XMMATRIX proj = XMMatrixPerspectiveFovLH(fieldOfView,
    screenAspect, screenNear, screenFar);
  XMStoreFloat4x4A(&m_projectionMatrix, proj);
}

void Light::SetAmbientColour(float red, float green, float blue, float alpha)
{
  m_ambientColour = XMFLOAT4A(red, green, blue, alpha);
}

void Light::SetDiffuseColour(float red, float green, float blue, float alpha)
{
  m_diffuseColour = XMFLOAT4A(red, green, blue, alpha);
}


void Light::SetDirection(float x, float y, float z)
{
  float magnitude = sqrtf(x * x + y * y + z * z);
  m_direction = XMFLOAT3A(x / magnitude,
    y / magnitude, z / magnitude);
}

void Light::SetSpecularColour(float red, float green, float blue, float alpha)
{
  m_specularColour = XMFLOAT4A(red, green, blue, alpha);
}


void Light::SetSpecularPower(float power)
{
  m_specularPower = power;
}

void Light::SetPosition(float x, float y, float z)
{
  m_position = XMFLOAT4A(x, y, z, 1.0f);
}

void Light::SetPosition(float x, float y, float z, float w) {
  m_position = XMFLOAT4A(x, y, z, w);
}

void Light::SetAttenuation(float x, float y, float z) {
  m_attenuation = XMFLOAT3A(x, y, z);
}

void Light::SetRange(float x) {
  m_range = x;
}

XMFLOAT4A Light::GetAmbientColour()
{
  return m_ambientColour;
}

XMFLOAT4A Light::GetDiffuseColour()
{
  return m_diffuseColour;
}


XMFLOAT3A Light::GetDirection()
{
  return m_direction;
}

XMFLOAT4A Light::GetSpecularColour()
{
  return m_specularColour;
}


float Light::GetSpecularPower()
{
  return m_specularPower;
}

XMFLOAT3A Light::GetPosition3()const
{
  XMFLOAT3A temp(m_position.x, m_position.y, m_position.z);
  return temp;
}

const XMFLOAT4A &Light::GetPosition4() const {
  return m_position;
}

void Light::SetLookAt(float x, float y, float z)
{
  m_lookAt = XMFLOAT4A(x, y, z, 1.0f);
}

XMMATRIX Light::GetViewMatrix()
{
  return XMLoadFloat4x4A(&m_viewMatrix);
}

XMMATRIX Light::GetProjectionMatrix()
{
  return XMLoadFloat4x4A(&m_projectionMatrix);
}

XMVECTOR Light::GetPosVector() {
  return XMLoadFloat4A(&m_position);
}

const XMFLOAT3A &Light::GetAttenuation() const {
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

