// Tessellation vertex shader.
// Doesn't do much, could manipulate the control points
// Pass forward data, strip out some values not required for example.

cbuffer MatrixBuffer : register(b0) {
  matrix worldMatrix;
  matrix viewMatrix;
  matrix projectionMatrix;
};

struct InputType {
  float4 position : POSITION;
  float2 tex : TEXCOORD0;
  float3 normal : NORMAL;
  float4 tangent : TANGENT;
};

struct OutputType {
  float4 position : POSITION;
  float2 tex : TEXCOORD0;
  float3 normal : NORMAL;
  float4 tangent : TANGENT;
  float4 world_pos : TEXCOORD1;
};

OutputType main(InputType input) {
  OutputType output;
  output.position = input.position;
  output.tex = input.tex;
  output.normal = input.normal;
  output.tangent = input.tangent;

  output.world_pos = input.position;
  output.world_pos.w = 1.f;
  output.world_pos = mul(output.world_pos, worldMatrix);

  return output;
}
