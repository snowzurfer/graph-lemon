// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader

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

cbuffer MatrixBuffer : register(cb0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CamBuffer : register(cb1) {
  float3 camPos;
  float padding;
};

// Const buffer for lights
cbuffer LightBuffer : register(cb2) {
  LightType lights[L_NUM];
};

cbuffer TimeBuffer : register(cb3) {
  float time;
  float amplitude;
  float speed;
  float padding_1;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    // How does the suffix modify the type?
    float3 viewDir: TEXCOORD1;
    float4 worldPos : TEXCOORD2;
    float3 pixel_to_light_vec[L_NUM] : TEXCOORD3;
};

OutputType main(InputType input)
{
    OutputType output;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;
    
    // Offset y position of the vertex based on a sine function
    input.position.y = amplitude * sin(speed * (input.position.x + time));
    // Update the normal at this point
    input.normal.x = amplitude * (1 - cos(speed * (input.position.x + time)));
    input.normal.y = amplitude * (abs(cos(speed * (input.position.x + time))));

    // Calculate the position of the vertex in world coordinates
    output.worldPos = mul(input.position, worldMatrix);

    // Calculate the vector from the pixel in world coordinates to the lights
    for (uint i = 0; i < L_NUM; ++i) {
      output.pixel_to_light_vec[i] = lights[i].position - output.worldPos;
    }

    // Determine the viewing direction based on the position of the camera and
    // the world position of the vertex, and then normalise it
    output.viewDir = normalize(camPos.xyz - output.worldPos.xyz);

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	 // Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

    return output;
}
