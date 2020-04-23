
struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXTURE0;
};

float4 main(PixelInputType input) : SV_TARGET
{
	float depthValue;
	float4 color;
	
	//First we get the depth value for this pixel, note that it is stored as z/w so we perform the necessary division to get the depth.
	// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
    depthValue = (input.depthPosition.z / input.depthPosition.w );
		
    color = float4(depthValue, depthValue * depthValue, 0.0f, 1.0f);

	return color;
} 