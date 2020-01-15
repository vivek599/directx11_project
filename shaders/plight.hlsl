Texture2D shaderTexture;
SamplerState sampleType;

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

float4 LightPixelShader( PixelInputType input ) : SV_TARGET
{
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color = ambientColor;

	textureColor = shaderTexture.Sample(sampleType, input.tex );
	lightDir = -lightDirection;

	lightIntensity = saturate( dot( input.normal, lightDir ) );
	if (lightIntensity > 0.0f)
	{
		color += diffuseColor * lightIntensity;
	}

	color = saturate(color);

	color *= textureColor;
	color.a = 1.0f;
	return color;
}
