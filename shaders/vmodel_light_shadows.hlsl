 

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
};

cbuffer LightBuffer2
{
	float3 lightPosition;
	float padding;
};

cbuffer CameraBuffer
{
	float3 cameraPosition;
	float padding1;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
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
};

PixelInputType ShadowVertexShader(VertexInputType input)
{
	PixelInputType output;
	float4 worldPosition;

	input.position.w = 1.0f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projMatrix );
	
	//Here we transform the vertex based on the light's perspective.
    // Calculate the position of the vertice as viewed by the light source.
	output.lightViewPosition = mul(input.position, worldMatrix);
	output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
	output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

	output.tex = input.tex;

	output.normal = mul( input.normal, (float3x3)worldMatrix );
	output.normal = normalize(output.normal);
	
	// Calculate the tangent vector against the world matrix only and then normalize the final value.
	output.tangent = mul(input.tangent, (float3x3) worldMatrix);
	output.tangent = normalize(output.tangent);
	
	// Calculate the binormal vector against the world matrix only and then normalize the final value.
	output.binormal = mul(input.binormal, (float3x3) worldMatrix);
	output.binormal = normalize(output.binormal);

	// Calculate the position of the vertex in the world.
	worldPosition = mul(input.position, worldMatrix);
	
	// Determine the light position based on the position of the light and the position of the vertex in the world.
	output.lightPos = lightPosition.xyz - worldPosition.xyz;
	
	// Normalize the light position vector.
	output.lightPos = normalize(output.lightPos);

	// Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

	// Normalize the viewing direction vector.
	output.viewDirection = normalize(output.viewDirection);

	return output;

}


