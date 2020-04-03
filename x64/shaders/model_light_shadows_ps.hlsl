
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

float4 blurPixel(float4 color, float2 uv )
{
    
    float of = 1.0f / 160.0f;
    
    float karnel[3][3] = 
    {
        { -5.0f * of,  3.0f * of,   5.0f * of  },
        { -3.0f * of,  of,          3.0f * of  },
        { -5.0f * of, -3.0f * of,   5.0f * of },
    };
    
    float4 blured = 0.0f;
    
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            blured += depthMapTexture.Sample(SampleTypeClamp, uv + karnel[i][j]).r;
        }
    }
    
    blured /= 9.0f;
    
    return blured;
}

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

    float bias = 0.000001f;
	
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
	
	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        float depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord).r;
		 
        float lightDepthValue = input.lightViewPosition.z - bias;
		     
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
