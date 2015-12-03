cbuffer MatrixBuffer : register(cb0) {
  matrix worldMatrix;
  matrix viewMatrix;
  matrix projectionMatrix;
};

struct InputType {
  float4 position : POSITION;
};

struct OutputType {
  float4 position : SV_POSITION;
};


[maxvertexcount(36)]
void main(point InputType input[1], inout TriangleStream<OutputType> triStream) {
  OutputType output;

  // Array used to build a cube out of triangle faces
  float3 g_positions[12] =
  {
    // Front face
    float3( -1, 1, -1 ),
    float3( -1, -1, -1 ),
    float3( 1, -1, -1 ),
    float3( -1, 1, -1),
    float3( 1, -1, -1),
    float3( 1, 1, -1),

    // Right side face
    float3( 1, 1, -1 ),
    float3( 1, -1, -1 ),
    float3( 1, -1, -1 ),
    float3( 1, 1, -1 ),
    float3( 1, -1, 1),
    float3( 1, 1, 1),
  };
  
  // Change the position vector to be 4 units for proper matrix calculations.
  input[0].position.w = 1.0f;

  // Create cube around the point
  for(int i = 0; i < 12; i++) {
    output.position = input[0].position + g_positions[i];
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
  
    triStream.Append(output);
  }

  triStream.RestartStrip();
}
