// Tessellation Hull Shader
// Prepares control points for tessellation

cbuffer TessellationBuffer : register(b0) {
  float max_tessellation_factor;
  float3 padding;
};

cbuffer CamBuffer : register(b1) {
  float3 cam_pos;
  float padding_cam;
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

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 3> inputPatch, uint patchId : SV_PrimitiveID) {
    ConstantOutputType output;

    // Set the tessellation factors for the three edges of the triangle.
    output.edges[0] = max_tessellation_factor;
    output.edges[1] = max_tessellation_factor;
    output.edges[2] = max_tessellation_factor;

    // Set the tessellation factor for tessallating inside the triangle.
    output.inside = max_tessellation_factor;

    return output;
}


[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstantFunction")]
InputType main(InputPatch<InputType, 3> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    return patch[pointId];
}
