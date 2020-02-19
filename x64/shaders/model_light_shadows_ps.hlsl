
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
};

float4 main(PixelInputType input) : SV_TARGET
{
	float2 projectTexCoord;
	float3 lightDir;
	float lightIntensity;
	float4 textureColor; 
	float4 color = ambientColor;
	
	//float4 color = ambientColor;
	float3 reflection;
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f); 

	float4 normalMap;
	float3 bumpNormal;

	//Shadow mapping requires a bias adjustment when comparing the depth of the light and the depth of the object due to the
	//low floatingpoint precision of the depth map.
    // Set the bias value for fixing the floating point precision issues.
    float bias = 0.000001f;
	
	//Calculate the projected texture coordinates for sampling theshadowmap(depth buffer texture) based on the light's viewing position.
    //Calculate the projected texture coordinates.
	projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w * 0.5f + 0.5f;
	projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w * 0.5f + 0.5f;
    
    input.lightViewPosition.xyz /= input.lightViewPosition.w;
	
	textureColor = shaderTexture[0].Sample(sampleType, input.tex );
	normalMap = shaderTexture[1].Sample(sampleType, input.tex );
				
	// Expand the range of the normal value from (0, +1) to (-1, +1).
	normalMap = (normalMap * 2.0f) - 1.0f;

	// Calculate the normal from the data in the normal map.
	bumpNormal = (normalMap.x * input.tangent) + (normalMap.y * input.binormal) + (normalMap.z * input.normal);

	// Normalize the resulting bump normal.
	bumpNormal = normalize(bumpNormal);  
	
	//Check if the projected coordinates arein the view of the light, if not then the pixel gets just an ambient value.
    // Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
	//if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
  //  if (input.lightViewPosition.x >= -1.0f && input.lightViewPosition.x <= 1.0f &&
		//input.lightViewPosition.y >= -1.0f && input.lightViewPosition.y <= 1.0f &&
  //      input.lightViewPosition.z >= 0.0f /*&& input.lightViewPosition.z <= 1.0f*/)
	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
		//Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
        float depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord).r;
		 
		// Calculate the depth of the light.
		// Subtract the bias from the lightDepthValue.
        float lightDepthValue = input.lightViewPosition.z - bias;
		     
		// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
        // If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
        if (lightDepthValue < depthValue)
        {
            //regular lighting
	        //lightDir = -lightDirection;
            lightIntensity = 1.0f; //saturate(dot(bumpNormal, input.lightPos)); //lightDir));

            if (lightIntensity > 0.0f)
            {
                color += diffuseColor; // * lightIntensity);
                color = saturate(color);
	
                // Calculate the reflection vector based on the light intensity, normal vector, and light direction.
                reflection = normalize(2 * lightIntensity * bumpNormal - input.lightPos);

                // Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
                specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
			    	
		        
            }
        }
 
    }
    
	color *= textureColor;
	// Add the specular component last to the output color. 
	color = saturate(color + specular);
	return color;
}
