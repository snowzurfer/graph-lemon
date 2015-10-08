// Light pixel shader
// Calculate ambient and diffuse lighting for a single light (also texturing)

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

#define L_NUM 2

cbuffer LightBuffer : register(cb0)
{
    float4 l_diffuse[L_NUM];
    float4 l_ambient[L_NUM];
    float3 lightDirection[L_NUM];
    float l_spec_power[L_NUM];
    float4 l_specular[L_NUM];
    float3 lightAttenuation[L_NUM];
    float lightRange[L_NUM];
    float4 lightPosition[L_NUM];
    int lightActive[L_NUM];
    float3 padding[L_NUM];
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDir : TEXCOORD1;
    float4 worldPos : TEXCOORD2;
};

float4 main(InputType input) : SV_TARGET{
  // Colour sampler from the colour map
  float4 textureColour;
  // Vector from the light to the fragment
  float3 calculatedLightDir[L_NUM];
  // How much the pixel is influenced by light
  float lightIntensity[L_NUM] = { 0.f, 0.f};
  // Final colour to be output by the shader
  float4 colour = { 0.f, 0.f, 0.f, 0.f };
  float4 ambient_colour = { 0.f, 0.f, 0.f, 0.f};
  // The final contribution of the specular part of the light
  float4 finalSpec = { 0.f, 0.f, 0.f, 0.f };
  // Falloff factor for point lights 
  float falloff[L_NUM] = { 1.f, 1.f};

  // Sample the pixel color from the texture using the sampler at this texture coordinate location.
  textureColour = shaderTexture.Sample(SampleType, input.tex);
  
  // For all the lights in the scene
  //for (uint i = 0; i < 1; i++) {
    // If the light is not active
    //if (lightActive[i] == 0) {
    //  continue;
    //}
    
    // Determine the type of light
    if (lightPosition[0].w > 0.f) { // Directional
      // Set the calculated light direction
      calculatedLightDir[0] = lightDirection[0];

      // Calculate the amount of light on this pixel.
      lightIntensity[0] = saturate(dot(input.normal, -calculatedLightDir[0]));
    }
    else if (lightPosition[0].w == 0.f) { // Point
      // Calculate the vector from the pixel in world coordinates to 
      // the light
      float3 lightToPixelVec = lightPosition[0].xyz - input.worldPos.xyz;

      // Store the distance between light and pixel
      float dist = length(lightToPixelVec);

      // If the pixel isn't too far away from the light source
      if (dist <= lightRange[0]) {
        // Normalise the light to pixel vector
        lightToPixelVec = normalize(lightToPixelVec);

        // Set the calculated light direction; need to invert the direction
        // as lightToPixelVec goes from pixel to light, but fur the specular
        // calculations we need it to be going in the opposite direction
        calculatedLightDir[0] = -lightToPixelVec;

        // Calculate the intensity of the point light
        lightIntensity[0] = saturate(dot(lightToPixelVec, input.normal));

        // If the light is striking the front of the pixel
        if (lightIntensity[0] > 0.f) {
          // Calculate the falloff factor
          falloff[0] = lightAttenuation[0][0] + (lightAttenuation[0][1] * dist) +
            (lightAttenuation[0][2] * (dist * dist));
        }
      }
    }

    // If the light is striking the front of the pixel
    if (lightIntensity[0] > 0.f) {
      // Add light to final colour of the pixel
      colour = saturate(l_diffuse[0] * lightIntensity[0]);

      // Clamp final lighting-based colour
      colour = saturate(colour);

      // Calculate the diffuse after the falloff factor
      //colour /= falloff[i];

      //// Calculate the reflection vector based on the light intensity, the
      //// normal vector and the light direction
      //float3 reflection = reflect(calculatedLightDir[i], input.normal);

      //// Determine the amount of specular light based on the reflection vector,
      //// the viewing direction and the specular power
      //float4 specular = pow(saturate(dot(reflection, input.viewDir)), 
      //  l_spec_power[i]);

      //// Calculate the colour of the specular, diminished by the falloff factor
      //finalSpec += (specular * l_specular[i]) / falloff[i];
      //finalSpec = saturate(finalSpec);

    }

    // Set the ambient illumination
    //ambient_colour += l_ambient[i] * textureColour;


  //}

  // Multiply the texture pixel and the final diffuse color to get the final pixel color result.
  //colour = colour * textureColour;

  // Add the specular component to the output colour
  colour = saturate(colour + finalSpec + ambient_colour);
	
  return colour;
}

