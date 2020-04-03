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
	//float4 skyboxPosition : TEXCOORD1;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	//float4 skyboxPosition : TEXCOORD1;
};

PixelInputType main(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.0f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projMatrix ).xyww;

	output.tex = input.tex;

	output.normal = mul( input.normal, (float3x3)worldMatrix );
	output.normal = normalize(output.normal);

	//// Send the unmodified position through to the pixel shader.
	//output.skyboxPosition = input.skyboxPosition;

	return output;

}


