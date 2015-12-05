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

cbuffer MatrixBuffer : register(b0) {
  matrix worldMatrix;
  matrix viewMatrix;
  matrix projectionMatrix;
};

cbuffer CamBuffer : register(b1) {
  float3 cam_pos;
  float padding;
};

// Const buffer for lights
cbuffer LightBuffer : register(b2) {
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
    // How does the suffix modify the type?
    float3 viewDir: TEXCOORD1;
    float4 world_pos : TEXCOORD2;
    float3 pixel_to_light_vec[L_NUM] : TEXCOORD2;
    float4 lightview_position[L_NUM] : TEXCOORD11;
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
  output.position = interpolate3D(patch[0].position.xyz, patch[1].position.xyz,
    patch[2].position.xyz, uvwCoord);
  output.position.w = 1.f;
  output.tex = interpolate2D(patch[0].tex, patch[1].tex,
    patch[2].tex, uvwCoord);
  output.normal = interpolate3D(patch[0].normal.xyz, patch[1].normal.xyz,
    patch[2].normal.xyz, uvwCoord);
 
  // Calculate the position of the vertex in world coordinates
  float4 world_pos = mul(input.position, worldMatrix);

  // Calculate the vector from the pixel in world coordinates to the lights
  for (uint i = 0; i < L_NUM; ++i) {
    output.pixel_to_light_vec[i] = lights[i].position.xyz - world_pos.xyz;
    
    // Calculate the position of the vertex as seen from the light
    output.lightview_position[i] = mul(input.position, worldMatrix);
    output.lightview_position[i] = mul(output.lightview_position[i], lights[i].view_matrix);
    output.lightview_position[i] = mul(output.lightview_position[i], lights[i].proj_matrix);
  }
  
  // Determine the viewing direction based on the position of the camera and
  // the world position of the vertex, and then normalise it
  output.viewDir = normalize(camPos.xyz - world_pos.xyz);
  
  // Calculate the position of the vertex against the world, view, and projection matrices.
  output.position = mul(input.position, worldMatrix);
  output.position = mul(output.position, viewMatrix);
  output.position = mul(output.position, projectionMatrix);

  output.world_pos = world_pos;
  
  // Store the texture coordinates for the pixel shader.
  output.tex = input.tex;

  // Calculate the normal vector against the world matrix only.
  output.normal = mul(input.normal, (float3x3)worldMatrix);

  // Normalize the normal vector.
  output.normal = normalize(output.normal);
  
  return output;
}

