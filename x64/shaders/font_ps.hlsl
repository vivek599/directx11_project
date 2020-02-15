

Texture2D shaderTexture;
SamplerState SampleType;


struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	
	textureColor = shaderTexture.Sample(SampleType, input.tex);
	
	if( textureColor.r < 0.02f )
	{
		textureColor.a = 0.0f;
	}
	else
	{		
		textureColor.r = 1.0f;
		textureColor.g = 1.0f;
		textureColor.b = 0.0f;
		textureColor.a = 1.0f;
	}

	return textureColor;

}