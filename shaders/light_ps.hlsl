// Light pixel shader
// Calculate ambient and diffuse lighting for a single light (also texturing)

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer : register(cb0)
{
    float4 diffuseColour;
    float4 ambientColour;
    float3 lightDirection;
    float specularPower;
    float4 specularColour;
    float3 lightAttenuation;
    float lightRange;
    float4 lightPosition;
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
  float4 textureColour;
  // Vector from the light to the fragment
  float3 calculatedLightDir;
  // How much the pixel is influenced by light
  float lightIntensity = 0.f;
  // Final colour to be output by the shader
  float4 colour = { 0.f, 0.f, 0.f, 0.f };
  // The final contribution of the specular part of the light
  float4 finalSpec = { 0.f, 0.f, 0.f, 0.f };
  // Falloff factor for point lights 
  float falloff = 1.f;

  // Sample the pixel color from the texture using the sampler at this texture coordinate location.
  textureColour = shaderTexture.Sample(SampleType, input.tex);
  
   
  // Determine the type of light
  if (lightPosition.w > 0.f) { // Directional
    // Set the calculated light direction
    calculatedLightDir = lightDirection;

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.normal, -calculatedLightDir));
  }
  else if (lightPosition.w == 0.f) { // Point
    // Calculate the vector from the pixel in world coordinates to 
    // the light
    float3 lightToPixelVec = lightPosition.xyz - input.worldPos.xyz;

    // Store the distance between light and pixel
    float dist = length(lightToPixelVec);

    // If the pixel isn't too far away from the light source
    if (dist <= lightRange) {
      // Normalise the light to pixel vector
      lightToPixelVec = normalize(lightToPixelVec);

      // Set the calculated light direction; need to invert the direction
      // as lightToPixelVec goes from pixel to light, but fur the specular
      // calculations we need it to be going in the opposite direction
      calculatedLightDir = -lightToPixelVec;

      // Calculate the intensity of the point light
      lightIntensity = saturate(dot(lightToPixelVec, input.normal));
      
      // If the light is striking the front of the pixel
      if (lightIntensity > 0.f) {
        // Calculate the falloff factor
        falloff = lightAttenuation[0] + (lightAttenuation[1] * dist) +
          (lightAttenuation[2] * (dist * dist));
      }
    }
  }
  
  // If the light is striking the front of the pixel
  if (lightIntensity > 0.f) {
    // Add light to final colour of the pixel
    colour += saturate(diffuseColour * lightIntensity);

    // Clamp final lighting-based colour
    colour = saturate(colour);

    // Calculate the diffuse after the falloff factor
    colour /= falloff;

    // Calculate the reflection vector based on the light intensity, the
    // normal vector and the light direction
    float3 reflection = reflect(calculatedLightDir, input.normal);

    // Determine the amount of specular light based on the reflection vector,
    // the viewing direction and the specular power
    float4 specular = pow(saturate(dot(reflection, input.viewDir)), specularPower);

    // Calculate the colour of the specular, diminished by the falloff factor
    finalSpec = (specular * specularColour) / falloff;
    
  }
  
  // Set the ambient illumination
  colour += ambientColour;

  // Multiply the texture pixel and the final diffuse color to get the final pixel color result.
  colour = colour * textureColour;

   // Add the specular component to the output colour
  colour = saturate(colour + finalSpec);

	return colour;
}

