// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices


// The maximum number of lights in the scene and also the number
// of lights which are passed every frame
#define L_NUM 4

// Represents a single light
struct LightType {
  float4 diffuse;
  float4 ambient;
  float4 direction;
  float4 specular;
  float4 attenuation;
  float4 position;
  // Determines which light is active
  uint active;
  float range;
  float specular_power;
  float spot_cutoff;
  float spot_exponent;
  float3 padding;
  matrix view_matrix;
  matrix proj_matrix;
};

cbuffer MatrixBuffer : register(cb0) {
  matrix worldMatrix;
  matrix viewMatrix;
  matrix projectionMatrix;
};

cbuffer CamBuffer : register(cb1) {
  float3 cam_pos;
  float padding;
};

// Const buffer for lights
cbuffer LightBuffer : register(cb2) {
  LightType lights[L_NUM];
};

struct InputType {
  float4 position : POSITION;
  float2 tex : TEXCOORD0;
  float3 normal : NORMAL;
  float4 tangent : TANGENT;
};

struct ConstantOutputType {
  float edges[3] : SV_TessFactor;
  float inside : SV_InsideTessFactor;
};

struct OutputType {
  float4 position : SV_POSITION;
  float2 tex : TEXCOORD0;
  float3 normal : NORMAL;
  float3 tangent : TANGENT;
  float3 tangent_view_dir : TEXCOORD1;
  float3 tangent_pixel_to_light_vec[L_NUM] : TEXCOORD2;
  float3 pixel_to_light_vec[L_NUM] : TEXCOORD6;
  float3 tangent_light_dir[L_NUM] : COLOR0;
};


// Invert the y and Z components of uvwCoord as these coords are generated in
// UV space and therefore y is positive downward.
// Alternatively you can set the output topology of the hull shader to cw instead of ccw (or vice versa).
float2 interpolate2D(const float2 &v0, const float2 &v1, float2 &v2, const float3 &uvw) {
  return float2(uvw.x) * v0 + float2(-uvw.y) * v1 + float2(-uvw.z) * v2;
}

float3 interpolate3D(const float3 &v0, const float3 &v1, const float3 &v2, const float3 &uvw) {
  return float3(uvw.x) * v0 + float3(-uvw.y) * v1 + float3(-uvw.z) * v2;
}


[domain("tri")]
OutputType main(ConstantOutputType input, float3 uvwCoord : SV_DomainLocation,
  const OutputPatch<InputType, 3> patch {
  float3 vertexPosition;
  OutputType output;
 
  // Determine the position of the new vertex and the texture coordinates of it
  output.position = interpolate3D(patch[0].position, patch[1].position,
    patch[2].position, uvwCoord);
  output.position.w = 1.f;
 
  // Calculate the position of the vertex against the world, view, and projection matrices.
  output.position = mul(output.position, worldMatrix);
  output.position = mul(output.position, viewMatrix);
  output.position = mul(output.position, projectionMatrix);
  
  // Store the texture coordinates for the pixel shader.
  output.tex = input.tex;

  // Calculate the normal vector against the world matrix only.
  output.normal = normal_worldspace;

  return output;
}

