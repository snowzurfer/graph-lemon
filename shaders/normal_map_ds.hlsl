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
    float3 tangent : TANGENT;
    float3 tangent_view_dir : TEXCOORD1;
    float3 tangent_pixel_to_light_vec[L_NUM] : TEXCOORD2;
    float3 pixel_to_light_vec[L_NUM] : TEXCOORD6;
    float3 tangent_light_dir[L_NUM] : TEXCOORD11;
    float4 lightview_position[L_NUM] : TEXCOORD16;
};


// Invert the y and Z components of uvwCoord as these coords are generated in
// UV space and therefore y is positive downward.
// Alternatively you can set the output topology of the hull shader to cw instead of ccw (or vice versa).
float2 interpolate2D(const float2 v0, const float2 v1, float2 v2, const float3 uvw) {
  return float2(uvw.x * v0 + -uvw.y * v1 + -uvw.z * v2);
}

float3 interpolate3D(const float3 v0, const float3 v1, const float3 v2, const float3 uvw) {
  return float3(uvw.x * v0 + -uvw.y * v1 + -uvw.z * v2);
}


[domain("tri")]
OutputType main(ConstantOutputType input_const, float3 uvwCoord : SV_DomainLocation,
  const OutputPatch<InputType, 3> patch) {
  float3 vertexPosition;
  OutputType output;
  InputType input;
 
  // Determine the position of the new vertex and the texture coordinates of it
  input.position.xyz = interpolate3D(patch[0].position.xyz, patch[1].position.xyz,
    patch[2].position.xyz, uvwCoord);
  input.position.w = 1.f;
  input.tex = interpolate2D(patch[0].tex, patch[1].tex,
    patch[2].tex, uvwCoord);
  input.normal = interpolate3D(patch[0].normal.xyz, patch[1].normal.xyz,
    patch[2].normal.xyz, uvwCoord);
  input.tangent.xyz = interpolate3D(patch[0].tangent.xyz, patch[1].tangent.xyz,
    patch[2].tangent.xyz, uvwCoord);
  input.tangent.w = patch[0].tangent.w;
 
  output.tangent = normalize(input.tangent.xyz);

  // Calculate the bitangent B = (N x T) * T.w
  float3 bitangent = cross(input.normal, input.tangent.xyz) * input.tangent.w;

  // Transform tangent, bitangent and normal into world space
  float3 tangent_worldspace = normalize(mul(input.tangent.xyz, (float3x3)worldMatrix));
  float3 bitangent_worldspace = normalize(mul(bitangent, (float3x3)worldMatrix));
  float3 normal_worldspace = normalize(mul(input.normal, (float3x3)worldMatrix));

  // Calculate the position of the vertex in world coordinates
  float4 world_pos = mul(input.position, worldMatrix);
  
  // Transform the view direction into tangent space
  output.tangent_view_dir = cam_pos - world_pos.xyz;
  output.tangent_view_dir = normalize(float3(
    dot(tangent_worldspace, output.tangent_view_dir),
    dot(bitangent_worldspace, output.tangent_view_dir),
    dot(normal_worldspace, output.tangent_view_dir)));
   
  // Calculate the vector from the pixel in world coordinates to the lights
  // and then transform it into tangent space;
  // Also transform the direction of the light to tangent space
  for (uint i = 0; i < L_NUM; ++i) {
    output.tangent_pixel_to_light_vec[i] = lights[i].position.xyz - world_pos.xyz;
    output.pixel_to_light_vec[i] = output.tangent_pixel_to_light_vec[i];
    output.tangent_pixel_to_light_vec[i] = 
    normalize(
    float4(dot(tangent_worldspace, output.tangent_pixel_to_light_vec[i].xyz),
    dot(bitangent_worldspace, output.tangent_pixel_to_light_vec[i].xyz),
    dot(normal_worldspace, output.tangent_pixel_to_light_vec[i].xyz),
    1.f));
    
    output.tangent_light_dir[i] =
    normalize(float4(dot(tangent_worldspace, lights[i].direction.xyz),
    dot(bitangent_worldspace, lights[i].direction.xyz),
    dot(normal_worldspace, lights[i].direction.xyz),
    1.f));
      
    // Calculate the position of the vertex as seen from the light
    output.lightview_position[i] = mul(input.position, worldMatrix);
    output.lightview_position[i] = mul(output.lightview_position[i], lights[i].view_matrix);
    output.lightview_position[i] = mul(output.lightview_position[i], lights[i].proj_matrix);
  }

  // Calculate the position of the vertex against the world, view, and projection matrices.
  output.position = mul(input.position, worldMatrix);
  output.position = mul(output.position, viewMatrix);
  output.position = mul(output.position, projectionMatrix);
  
  // Store the texture coordinates for the pixel shader.
  output.tex = input.tex;

  // Calculate the normal vector against the world matrix only.
  output.normal = normal_worldspace;
  return output;
}

