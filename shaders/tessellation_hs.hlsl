// Tessellation Hull Shader
// Prepares control points for tessellation

cbuffer TessellationBuffer : register(b1) {
  float max_tessellation_factor;
  float tessellation_distance;
  float2 padding;
};

cbuffer CamBuffer : register(b0) {
  float3 cam_pos;
  float padding_cam;
};

struct InputType {
  float4 position : POSITION;
  float2 tex : TEXCOORD0;
  float3 normal : NORMAL;
  float4 tangent : TANGENT;
  float4 world_pos : TEXCOORD1;
};

struct OutputType {
  float4 position : POSITION;
  float2 tex : TEXCOORD0;
  float3 normal : NORMAL;
  float4 tangent : TANGENT;
};

struct ConstantOutputType {
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

int CalcTessellationFactorBasedOnDistance(float3 v_pos, float3 camera_pos) {
  // Constant defining the max distance to which apply tessellation
  const float max_dist_sqr = tessellation_distance * tessellation_distance;
  // Calculate square distance
  float3 diff = v_pos - camera_pos;
  float dist_sqr = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

  if(dist_sqr > max_dist_sqr) {
    return 1;
  }
 
  float ratio = 1.f - (dist_sqr / max_dist_sqr);
  
  return (int)(max(max_tessellation_factor * ratio, 1.f));
}

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 3> inputPatch, uint patchId : SV_PrimitiveID) {
    ConstantOutputType output;

    // Set the tessellation factors for the three edges of the triangle.
    float3 edgepoint = inputPatch[0].world_pos.xyz + (inputPatch[1].world_pos.xyz - inputPatch[0].world_pos.xyz);
    output.edges[0] = CalcTessellationFactorBasedOnDistance(edgepoint, cam_pos);
    edgepoint = inputPatch[1].world_pos.xyz + (inputPatch[2].world_pos.xyz - inputPatch[1].world_pos.xyz);
    output.edges[1] = CalcTessellationFactorBasedOnDistance(edgepoint, cam_pos);
    edgepoint = inputPatch[2].world_pos.xyz + (inputPatch[0].world_pos.xyz - inputPatch[2].world_pos.xyz);
    output.edges[2] = CalcTessellationFactorBasedOnDistance(edgepoint, cam_pos);

    // Set the tessellation factor for tessallating inside the triangle.
    float x = (inputPatch[0].world_pos.x + inputPatch[1].world_pos.x + inputPatch[2].world_pos.x) / 3.f;
    float y = (inputPatch[0].world_pos.y + inputPatch[1].world_pos.y + inputPatch[2].world_pos.y) / 3.f;
    float z = (inputPatch[0].world_pos.z + inputPatch[1].world_pos.z + inputPatch[2].world_pos.z) / 3.f;

    output.inside = CalcTessellationFactorBasedOnDistance(float3(x, y, z), cam_pos);

    return output;
}


[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 3> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
  OutputType output;
  output.position = patch[pointId].position;
  output.tex = patch[pointId].tex;
  output.normal = patch[pointId].normal;
  output.tangent = patch[pointId].tangent;

  return output;
}
