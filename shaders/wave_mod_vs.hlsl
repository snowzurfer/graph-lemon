// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader

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

cbuffer TimeBuffer : register(cb2) {
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
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    // How does the suffix modify the type?
    float3 viewDir: TEXCOORD1;
    float4 worldPos : TEXCOORD2;
};

OutputType main(InputType input)
{
    OutputType output;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;
    
    // Offset y position of the vertex based on a sine function
    input.position.y = amplitude * sin(speed * (input.position.x + time));
    // Update the normal at this point
    input.normal.x = 1 - cos(input.position.x + time);
    input.normal.y = abs(cos(input.position.x + time));

    // Calculate the position of the vertex in world coordinates
    output.worldPos = mul(input.position, worldMatrix);

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
