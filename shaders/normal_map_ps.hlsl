// Light pixel shader
// Calculate ambient and diffuse lighting for a single light (also texturing)

// The maximum number of lights in the scene and also the number
// of lights which are passed every frame
#define L_NUM 4

Texture2D texture_diffuse : register(t0);
Texture2D texture_normal : register(t1);
Texture2D texture_light_depth[L_NUM] : register(t4);
SamplerState SampleType : register(s0);


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

// Represents a material
struct MaterialType {
  float4 ambient;
  float4 diffuse;
  float4 specular;
  float4 transmittance;
  float4 emission;
  float shininess;
  float ior;      // index of refraction
  float dissolve; // 1 == opaque; 0 == fully transparent
  int illum;
};

// Const buffer for lights
cbuffer LightBuffer : register(b0) {
  LightType lights[L_NUM];
};

// Const buffer for the material
cbuffer MatBuffer : register(b1) {
  MaterialType mat;
};


struct InputType
{
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

float4 main(InputType input) : SV_TARGET {
  // Colour sampler from the colour map
  float4 sampled_diffuse;
  // Final colour to be output by the shader
  float4 colour = { 0.f, 0.f, 0.f, 0.f };
  // Global, constant ambient contribution
  float4 ambient_global_colour = {0.08f, 0.08f, 0.08f, 1.f};
  // Total contribution of the lights
  float4 total_light_contribution = { 0.f, 0.f, 0.f, 0.f };
  // Bias for shawdowing
  float bias = 0.001f;

  // Sample the pixel color from the texture using the sampler at this texture coordinate location.
  sampled_diffuse = texture_diffuse.Sample(SampleType, input.tex);

  // Sample normal from normal map
  float3 sampled_normal = (2.f * texture_normal.Sample(SampleType, input.tex).xyz) - 1.f;
  sampled_normal = normalize(sampled_normal);
  // Calculate the global constant ambient contribution
  ambient_global_colour *= sampled_diffuse * mat.ambient;

  // For each light in the scene
  for (uint i = 0; i < L_NUM; ++i) {
    // If the light is active
    if (lights[i].active == 0) {
      // Skip this light
      continue;
    }
  
    // Vector from the light to the fragment
    float3 calc_light_dir;
    // How much the pixel is influenced by light
    float light_intensity = 0.f;
    // Falloff factor for point lights 
    float falloff = 1.f;
    // The final contribution of the specular part of the light
    float4 final_spec_contribution = { 0.f, 0.f, 0.f, 0.f };
    // The final contribution of the diffuse part of the light
    float4 final_diff_contribution = { 0.f, 0.f, 0.f, 0.f };
    // The final contribution of the ambient part of the light
    float4 final_amb_contribution = lights[i].ambient * sampled_diffuse *
      mat.ambient;
    // The spotlight effect in case the light is spotlight
    float spot_effect = 1.f;
    // The amount of shadowing on this pixel
    float shadow_effect = 1.f;
  
    // Calculate the projected texture coordinates
    float2 proj_tex_coord;
    proj_tex_coord.x = input.lightview_position[i].x /
      input.lightview_position[i].w / 2.f + 0.5f;
    proj_tex_coord.y = -input.lightview_position[i].y /
      input.lightview_position[i].w / 2.f + 0.5f;
    
    // Determine the type of light
    if (lights[i].position.w > 0.f) { // Directional
      // Set the calculated light direction
      calc_light_dir = normalize(input.tangent_light_dir[i]);
      //calc_light_dir = lights[i].direction.xyz;

      // Calculate the amount of light on this pixel.
      light_intensity = saturate(dot(sampled_normal, -calc_light_dir));
      //light_intensity = saturate(dot(float4(input.normal
        //, 0.f), -calc_light_dir));
    }
    else if (lights[i].position.w == 0.f) { // Point
      // Save the vector from the pixel in world coordinates to 
      // the light
      float3 pixel_to_light_vec = input.pixel_to_light_vec[i];

      // Store the distance between light and pixel
      float dist = length(pixel_to_light_vec);

      // If the pixel isn't too far away from the light source
      if (dist <= lights[i].range) {
        // Normalise the pixel to light vector in tangent space
        pixel_to_light_vec = normalize(input.tangent_pixel_to_light_vec[i]);

        // Set the calculated light direction; need to invert the direction
        // as pixel_to_light_vec goes from pixel to light, but for the specular
        // calculations we need it to be going in the opposite direction
        calc_light_dir = -pixel_to_light_vec;

        // Calculate the intensity of the point light
        light_intensity = saturate(dot(pixel_to_light_vec, 
          sampled_normal));
        
        // If the light is striking the front of the pixel
        if (light_intensity > 0.f) {
          // Calculate the falloff factor
          falloff = lights[i].attenuation[0] + (lights[i].attenuation[1] * dist) +
            (lights[i].attenuation[2] * (dist * dist));
        }

        // If the light is a spotlight
        if (lights[i].spot_cutoff != 3.14159265358979323846f) {
          // Calculate the cosine of the angle between the direction of the light
          // and the vector from the light to the pixel, in tangent space
          float cos_directions = max(dot(calc_light_dir, 
            normalize(input.tangent_light_dir[i])), 0);



          // If the pixel lies within the cone of illumination 
          if (cos_directions > cos(lights[i].spot_cutoff)) {
            // Calculate the spotlight effect
            spot_effect = pow(cos_directions, lights[i].spot_exponent);

    
            
            // Determine if the projected coordinates are in the 0 to 1 range
            if((saturate(proj_tex_coord.x) == proj_tex_coord.x) &&
              (saturate(proj_tex_coord.y) == proj_tex_coord.y)) {
              float sample_depth = 1.f;
              
              // Sample the depth map
              switch(i) {
                case 0:
                  sample_depth = texture_light_depth[0].Sample(SampleType, proj_tex_coord).r;
                  break;
                case 1:
                  sample_depth = texture_light_depth[1].Sample(SampleType, proj_tex_coord).r;
                  break;
                case 2:
                  sample_depth = texture_light_depth[2].Sample(SampleType, proj_tex_coord).r;
                  break;
                case 3:
                  sample_depth = texture_light_depth[3].Sample(SampleType, proj_tex_coord).r;
                  break;
              }
            
              // Calculate depth at this pixel
              float light_depth = input.lightview_position[i].z /
                input.lightview_position[i].w;
              light_depth -= bias;

              // Determine whether to shadow this pixel or not
              if(light_depth > sample_depth) {
                // Calculate the amount of shadowing on this pixel
                shadow_effect = 0.f;
              }
              //else if (light_depth == sample_depth){
                //shadow_effect = 0.5;
              //}
              //else {
                //shadow_effect = 1.f;
              //}
              //colour = float4(shadow_effect, shadow_effect, shadow_effect, 1.f); 

 

            }
          }
          // If the pixel doesn't lie within the cone
          else {
            spot_effect = 0.f;

    
          }

        }
      }
    }
    
    // If the light is striking the front of the pixel
    if (light_intensity > 0.f) {
      // Add the diffuse colour contribution
      final_diff_contribution = saturate(lights[i].diffuse * light_intensity *
        sampled_diffuse * mat.diffuse);
      // Calculate the reflection vector based on the light intensity, the
      // normal vector and the light direction
      float3 reflection = reflect(calc_light_dir.xyz, sampled_normal);

      // Determine the amount of specular light based on the reflection vector,
      // the viewing direction and the specular power
      float specular_intensity = pow(saturate(dot(reflection, 
        normalize(input.tangent_view_dir))), mat.shininess);

      // Calculate the colour of the specular, diminished by the falloff factor
      final_spec_contribution = saturate(specular_intensity * 
        lights[i].specular * mat.specular * sampled_diffuse);
     
      // Add specular and diffuse to the total contribution of the light also
      // accounting for the falloff factor
      total_light_contribution += ((final_amb_contribution + 
        final_diff_contribution + final_spec_contribution) / falloff
        * spot_effect * shadow_effect);
      total_light_contribution = saturate(total_light_contribution);

    }

  }
   

  // Add the ambient component to the diffuse to obtain the outpu colour
  colour = float4(ambient_global_colour + total_light_contribution);
  //colour = float4(saturate((sampled_normal + 1 ) * 0.5), 1.f);

  return colour;
}

