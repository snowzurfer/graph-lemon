// Size of box can be increased using the traditional
// scaling of the worldmatrix

cbuffer MatrixBuffer : register(b0) {
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
  float4 g_positions[36] =
  {
    // Front face
    float4( -1.f, 1.f, -1.f, 1.f),
    float4( 1.f, -1.f, -1.f, 1.f),
    float4( -1.f, -1.f, -1.f, 1.f),
    float4( -1.f, 1.f, -1.f, 1.f),
    float4( 1.f, 1.f, -1.f, 1.f),
    float4( 1.f, -1.f, -1.f, 1.f),

    // Right side face
    float4( 1.f, 1.f, -1.f, 1.f),
    float4( 1.f, -1.f, 1.f, 1.f),
    float4( 1.f, -1.f, -1.f, 1.f),
    float4( 1.f, 1.f, -1.f, 1.f),
    float4( 1.f, 1.f, 1.f, 1.f),
    float4( 1.f, -1.f, 1.f, 1.f),
    
    // Back side face
    float4( 1.f, 1.f, 1.f, 1.f),
    float4( -1.f, -1.f, 1.f, 1.f),
    float4( 1.f, -1.f, 1.f, 1.f),
    float4( 1.f, 1.f, 1.f, 1.f),
    float4( -1.f, 1.f, 1.f, 1.f),
    float4( -1.f, -1.f, 1.f, 1.f),
    
    // Left side face
    float4( -1.f, 1.f, 1.f, 1.f),
    float4( -1.f, -1.f, -1.f, 1.f),
    float4( -1.f, -1.f, 1.f, 1.f),
    float4( -1.f, 1.f, 1.f, 1.f),
    float4( -1.f, 1.f, -1.f, 1.f),
    float4( -1.f, -1.f, -1.f, 1.f),
    
    // Top side face
    float4( -1.f, 1.f, -1.f, 1.f),
    float4( 1.f, 1.f, 1.f, 1.f),
    float4( -1.f, 1.f, 1.f, 1.f),
    float4( -1.f, 1.f, -1.f, 1.f),
    float4( 1.f, 1.f, -1.f, 1.f),
    float4( 1.f, 1.f, 1.f, 1.f),
    
    // Bottom side face
    float4( 1.f, -1.f, 1.f, 1.f),
    float4( -1.f, -1.f, -1.f, 1.f),
    float4( 1.f, -1.f, -1.f, 1.f),
    float4( 1.f, -1.f, 1.f, 1.f),
    float4( -1.f, -1.f, 1.f, 1.f),
    float4( -1.f, -1.f, -1.f, 1.f)
  };
  
  // Change the position vector to be 4 units for proper matrix calculations.
  input[0].position.w = 1.0f;

  // Create cube around the point
  for(int i = 0; i < 36; i++) {
    output.position = input[0].position + g_positions[i];
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
  
    triStream.Append(output);
  }

  triStream.RestartStrip();
}
