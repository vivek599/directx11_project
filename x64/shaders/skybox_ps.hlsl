Texture2D shaderTexture;
SamplerState sampleType;

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
    float  visibility : TEXCOORD1;
    float	fogHeight : TEXCOORD2;
};

float4 main(PixelInputType input) : SV_TARGET
{
	float4 textureColor;

	textureColor = shaderTexture.Sample(sampleType, input.tex );
	
    const float maxFogHeight = 300.0f;
	
    float heightGradiant = input.fogHeight / maxFogHeight;
    
    heightGradiant += input.visibility*0.25f;
        
    heightGradiant = clamp(heightGradiant, 0.0f, 1.0f);
        
    heightGradiant = 1.0f - heightGradiant;
        
    textureColor = lerp(textureColor, float4(1.0f, 1.0f, 1.0f, 1.0f), heightGradiant);
	
    return textureColor;
}
