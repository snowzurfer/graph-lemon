#ifndef _BUFFER_TYPES_H
#define _BUFFER_TYPES_H

#include <DirectXMath.h>

namespace sz {

using namespace DirectX;

struct MatrixBufferType {
  XMMATRIX world;
  XMMATRIX view;
  XMMATRIX projection;
};

struct ScreenSizeBufferType {
  float screen_size;
  XMFLOAT3 padding;
};

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

}

#endif