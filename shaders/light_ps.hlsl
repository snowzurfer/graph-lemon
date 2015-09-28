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
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDir : TEXCOORD1;
};

float4 main(InputType input) : SV_TARGET
{
    float4 textureColour;
    float3 lightDir;
    float lightIntensity;
    float4 colour;
    float3 reflection;
    float4 finalSpec = { 0.f, 0.f, 0.f, 0.f };
    float4 specular = { 1.f, 1.f, 1.f, 1.f};

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColour = shaderTexture.Sample(SampleType, input.tex);
	
	// Invert the light direction for calculations.
    lightDir = -lightDirection;

  // Set the ambient illumination
  colour = ambientColour;

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.normal, lightDir));

  // Check intensity of diffuse light
  if (lightIntensity > 0) {
    // Calculate its value and add it to final colour
    colour += saturate(diffuseColour * lightIntensity);

    // Clamp the final lighting-based colour
    colour = saturate(colour);

    // Calculate the reflection vector based on the light intensity, the
    // normal vector and the light direction
    reflection = reflect(lightDirection, input.normal);

    // Determine the amount of specular light based on the reflection vector,
    // the viewing direction and the specular power
    specular = pow(saturate(dot(reflection, input.viewDir)), specularPower);

    // Calculate the colour of the specular
    finalSpec = specular * specularColour;
  }


  // Multiply the texture pixel and the final diffuse color to get the final pixel color result.
  colour = colour * textureColour;

   // Add the specular component to the output colour
  colour = saturate(colour + finalSpec);
	return colour;
}

