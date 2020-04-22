
//The depthMapTexture is the shadow map. This texture contains the scene depth buffer rendered from the light's perspective.
Texture2D depthMapTexture : register(t0);
Texture2D shaderTexture[2];

//We require a clampbased sampler when sampling the depth buffer so that it doesn't wrap around and sample incorrect information.
SamplerState sampleType : register(s0);
SamplerState SampleTypeClamp : register(s1);

cbuffer LightBuffer
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float specularPower;
    float4 specularColor;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 viewDirection : TEXCOORD1;
    float4 lightViewPosition : TEXCOORD2;
    float3 lightPos : TEXCOORD3;
    float visibility : TEXCOORD4;
};

float CalculateShadow(PixelInputType input, float dotLightNormal)
{
    //float3 pos = (input.lightViewPosition.xyz * 0.5f + 0.5f);
    float3 pos = 0.0f;
    
    pos.x = input.lightViewPosition.x / input.lightViewPosition.w * 0.5f + 0.5f;
    pos.y = -input.lightViewPosition.y / input.lightViewPosition.w * 0.5f + 0.5f;
    pos.z = input.lightViewPosition.z / input.lightViewPosition.w;
    
    if (pos.z > 1.0f)
    {
        pos.z = 1.0f;
    }
    
    float bias = max(0.001f * (1.0f - dotLightNormal), 0.00001f);
    
    //percentage closer filter
    float shadow = 0.f;
    float width = 0.f;
    float height = 0.f;
    depthMapTexture.GetDimensions(width, height);
    
    float2 texelSize = 0.25f * 1.0f / max(width, height);
    
    float karnel[7] = { -3.0f, -2.0f, -1.0f, 0.0f, 1.0f, 2.0f, 3.0f };
    
    for (int i = 0; i < 7; i++)
    {
        float depthValueH = depthMapTexture.Sample(SampleTypeClamp, pos.xy + float2(karnel[i], 0.0f) * texelSize ).r;
        float depthValueV = depthMapTexture.Sample(SampleTypeClamp, pos.xy + float2(0.0f, karnel[i]) * texelSize ).r;
        shadow += (depthValueH + bias) < pos.z ? 0.0f : 1.0f;
        shadow += (depthValueV + bias) < pos.z ? 0.0f : 1.0f;
    }
            
    shadow /= 14.0f;
    
    return shadow;
}


float4 main(PixelInputType input) : SV_TARGET
{
    float4 textureColor = shaderTexture[0].Sample(sampleType, input.tex);
    float4 normalMap = shaderTexture[1].Sample(sampleType, input.tex * 16.0f );
				
	// Expand the range of the normal value from (0, +1) to (-1, +1).
    normalMap = (normalMap * 2.0f) - 1.0f;

	// Calculate the normal from the data in the normal map.
    float3 bumpNormal = (normalMap.x * input.tangent) + (normalMap.y * input.binormal) + (normalMap.z * input.normal);

	// Normalize the resulting bump normal.
    bumpNormal = normalize(bumpNormal);
    
    float lightIntensity = saturate(dot(bumpNormal, input.lightPos));
    float diff = max( lightIntensity, 0.0f);

    // Calculate the reflection vector based on the light intensity, normal vector, and light direction.
    float reflection = normalize(2 * diff * bumpNormal - input.lightPos);

    // Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
    float specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
    
    float shadow = CalculateShadow(input, lightIntensity );
    
    float4 finalColor = (shadow * saturate(diff * diffuseColor + specular) + ambientColor) * textureColor;
    
    return lerp(finalColor, float4(1.0f, 1.0f, 1.0f, 1.0f), input.visibility);
    
}
