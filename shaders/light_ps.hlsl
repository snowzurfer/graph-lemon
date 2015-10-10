// Light pixel shader
// Calculate ambient and diffuse lighting for a single light (also texturing)

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

#define L_NUM 4

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
  float padding;
};

cbuffer LightBuffer : register(cb0) {
  LightType lights[L_NUM];
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDir : TEXCOORD1;
    float4 worldPos : TEXCOORD2;
};

float4 main(InputType input) : SV_TARGET {
  // Colour sampler from the colour map
  float4 texture_colour;
  // Final colour to be output by the shader
  float4 colour = { 0.f, 0.f, 0.f, 0.f };
  // Ambient contribution
  float4 ambient_final_colour;
  // Total contribution of the lights
  float4 total_light_contribution = { 0.f, 0.f, 0.f, 0.f };

  // Sample the pixel color from the texture using the sampler at this texture coordinate location.
  texture_colour = shaderTexture.Sample(SampleType, input.tex);

  
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

    // Add ambient contribution
    ambient_final_colour += texture_colour * lights[i].ambient;
  
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
      float4 pixel_to_light_vec = lights[i].position - input.worldPos;

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
      }
    }
    
    // If the light is striking the front of the pixel
    if (light_intensity > 0.f) {
      // Add the diffuse colour contribution
      final_diff_contribution = saturate(lights[i].diffuse * light_intensity);
      final_diff_contribution *= texture_colour;

      // Calculate the diffuse after the falloff factor
      final_diff_contribution /= falloff;

      // Calculate the reflection vector based on the light intensity, the
      // normal vector and the light direction
      float4 reflection = reflect(calc_light_dir, float4(input.normal, 1.f));

      // Determine the amount of specular light based on the reflection vector,
      // the viewing direction and the specular power
      float4 specular = pow(saturate(dot(reflection, float4(input.viewDir, 0.f))), 
        lights[i].specular_power);

      // Calculate the colour of the specular, diminished by the falloff factor
      final_spec_contribution = (specular * lights[i].specular) / falloff;
     
      // Add specular and diffuse to the total contribution of the light
      total_light_contribution += (final_diff_contribution + final_spec_contribution);
      total_light_contribution = saturate(total_light_contribution);
    }

  }
   

  // Add the ambient component to the diffuse to obtain the outpu colour
  colour = saturate(ambient_final_colour + total_light_contribution);

	return colour;
}

