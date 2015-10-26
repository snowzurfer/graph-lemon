// Light pixel shader
// Calculate ambient and diffuse lighting for a single light (also texturing)

Texture2D texture_diff : register(t0);
Texture2D texture_alpha: register(t1);
Texture2D texture_spec: register(t2);
SamplerState SampleType : register(s0);

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
cbuffer LightBuffer : register(cb0) {
  LightType lights[L_NUM];
};

// Const buffer for the material
cbuffer MatBuffer : register(cb1) {
  MaterialType mat;
};


struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDir : TEXCOORD1;
    float4 pixel_to_light_vec[L_NUM] : TEXCOORD2;
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

  // Sample the pixel color from the texture using the sampler at this texture coordinate location.
  sampled_diffuse = texture_diff.Sample(SampleType, input.tex);
  // Sample the alpha value from the texture using the sampler
  float sampled_alpha = texture_alpha.Sample(SampleType, input.tex).x; 
  // Sample alpha from alpha map, where the w component is the shininess
  float4 sampled_spec = texture_spec.Sample(SampleType, input.tex); 

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
    float4 calc_light_dir;
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
  
    // Determine the type of light
    if (lights[i].position.w > 0.f) { // Directional
      // Set the calculated light direction
      calc_light_dir = lights[i].direction;

      // Calculate the amount of light on this pixel.
      light_intensity = saturate(dot(float4(input.normal, 0.f), -calc_light_dir));
    }
    else if (lights[i].position.w == 0.f) { // Point
      // Calculate the vector from the pixel in world coordinates to 
      // the light
      float4 pixel_to_light_vec = input.pixel_to_light_vec[i];

      // Store the distance between light and pixel
      float dist = length(pixel_to_light_vec);

      // If the pixel isn't too far away from the light source
      if (dist <= lights[i].range) {
        // Normalise the light to pixel vector
        pixel_to_light_vec = normalize(pixel_to_light_vec);

        // Set the calculated light direction; need to invert the direction
        // as pixel_to_light_vec goes from pixel to light, but for the specular
        // calculations we need it to be going in the opposite direction
        calc_light_dir = -pixel_to_light_vec;

        // Calculate the intensity of the point light
        light_intensity = saturate(dot(pixel_to_light_vec, float4(input.normal, 0.f)));
        
        // If the light is striking the front of the pixel
        if (light_intensity > 0.f) {
          // Calculate the falloff factor
          falloff = lights[i].attenuation[0] + (lights[i].attenuation[1] * dist) +
            (lights[i].attenuation[2] * (dist * dist));
        }

        // If the light is a spotlight
        if (lights[i].spot_cutoff != 180.f) {
          // Calculate the cosine of the angle between the direction of the light
          // and the vector from the light to the pixel
          float cos_directions = max(dot(calc_light_dir, lights[i].direction), 0);

          // If the pixel lies within the cone of illumination 
          if (cos_directions > cos(lights[i].spot_cutoff)) {
            // Calculate the spotlight effect
            spot_effect = pow(cos_directions, lights[i].spot_exponent);
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
      float3 reflection = reflect(calc_light_dir.xyz, input.normal);

      // Determine the amount of specular light based on the reflection vector,
      // the viewing direction and the specular power
      float specular_intensity = pow(saturate(dot(reflection, input.viewDir)), 
        mat.shininess * sampled_spec.w);

      // Calculate the colour of the specular, diminished by the falloff factor
      final_spec_contribution = saturate(specular_intensity * 
        lights[i].specular * mat.specular * sampled_spec);
     
      // Add specular and diffuse to the total contribution of the light also
      // accounting for the falloff factor
      total_light_contribution += ((final_amb_contribution + 
        final_diff_contribution + final_spec_contribution) / falloff
        * spot_effect);
      total_light_contribution = saturate(total_light_contribution);

    }

  }
   

  // Add the ambient component to the diffuse to obtain the outpu colour
  colour = saturate(float4(ambient_global_colour.xyz + 
    total_light_contribution.xyz, sampled_alpha));

  return colour;
}
