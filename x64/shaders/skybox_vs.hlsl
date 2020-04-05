cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4	position : SV_POSITION;
	float2	tex : TEXCOORD0;
	float3	normal : NORMAL;
    float	visibility : TEXCOORD1;
    float	fogHeight : TEXCOORD2;
};

PixelInputType main(VertexInputType input)
{
	PixelInputType output;

    input.position.w = 1.0f;
	
    const float fogDensity = 0.0023f;
    const float fogGradient = 2.5f;
	
    output.position = mul(input.position, worldMatrix);
	
    output.fogHeight = output.position.y;
	
    output.position = mul(output.position, viewMatrix);
    float4 distFromCamera = output.position;
	
    output.position = mul(output.position, projMatrix).xyww;
	
	
    float distance = length(distFromCamera.xz);
    output.visibility = exp(-pow((distance * fogDensity), fogGradient));
    output.visibility = clamp(output.visibility, 0.0f, 1.0f);
    output.visibility = 1.0f - output.visibility;

	output.tex = input.tex;

	output.normal = mul( input.normal, (float3x3)worldMatrix );
	output.normal = normalize(output.normal);

	//// Send the unmodified position through to the pixel shader.
	//output.skyboxPosition = input.skyboxPosition;

	return output;

}


