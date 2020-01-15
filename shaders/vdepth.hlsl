
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};


struct VertexInputType
{
    float4 position : POSITION;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXTURE0;
};

PixelInputType DepthVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	//As described in the pixel shader input we store a second pair of position coordinates that will not be offset which will be used for depth calculations.
    // Store the position value in a second input value for depth value calculations.
    output.depthPosition = output.position;
	
    return output;
}
