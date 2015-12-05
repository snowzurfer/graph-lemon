// Tessellation vertex shader.
// Doesn't do much, could manipulate the control points
// Pass forward data, strip out some values not required for example.

struct InputType {
  float4 position : POSITION;
  float2 tex : TEXCOORD0;
  float3 normal : NORMAL;
  float4 tangent : TANGENT;
};

InputType main(InputType input) {
    return input;
}
