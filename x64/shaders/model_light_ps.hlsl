
#define NUM_LIGHTS 4

Texture2D shaderTexture[2];
SamplerState sampleType;

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor[NUM_LIGHTS];
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
	float3 lightPos1 : TEXCOORD2;
	float3 lightPos2 : TEXCOORD3;
	float3 lightPos3 : TEXCOORD4;
	float3 lightPos4 : TEXCOORD5;
};

float4 main(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float lightIntensity1, lightIntensity2, lightIntensity3, lightIntensity4;
	float4 color;
	float4 color1 = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 color2 = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 color3 = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 color4 = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 normalMap;
	float3 bumpNormal;

	float3 lightDir;
	float lightIntensity;
	//float4 color = ambientColor;
	float3 reflection, reflection1, reflection2, reflection3, reflection4;
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular1 = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular2 = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular3 = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular4 = float4(0.0f, 0.0f, 0.0f, 0.0f);

	textureColor = shaderTexture[0].Sample(sampleType, input.tex);
	normalMap = shaderTexture[1].Sample(sampleType, input.tex );
	// Expand the range of the normal value from (0, +1) to (-1, +1).
	normalMap = (normalMap * 2.0f) - 1.0f;

	// Calculate the normal from the data in the normal map.
	bumpNormal = (normalMap.x * input.tangent) + (normalMap.y * input.binormal) + (normalMap.z * input.normal);

	// Normalize the resulting bump normal.
	bumpNormal = normalize(bumpNormal);

	// Calculate the different amounts of light on this pixel based on the positions of the lights.
	lightIntensity1 = saturate(dot(input.normal, input.lightPos1)); 
	lightIntensity2 = saturate(dot(input.normal, input.lightPos2));
	lightIntensity3 = saturate(dot(input.normal, input.lightPos3));
	lightIntensity4 = saturate(dot(input.normal, input.lightPos4));

	// Determine the diffuse color amount of each of the four lights.
	color1 = diffuseColor[0] * lightIntensity1;
	color2 = diffuseColor[1] * lightIntensity2;
	color3 = diffuseColor[2] * lightIntensity3;
	color4 = diffuseColor[3] * lightIntensity4;


	if (lightIntensity1 > 0.0f)
	{
		color1 += diffuseColor[0] * lightIntensity1;
		color1 = saturate(color1);

		// Calculate the reflection vector based on the light intensity, normal vector, and light direction.
		//reflection1 = normalize(2 * lightIntensity1 * bumpNormal - input.lightPos1);

		// Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
	//	specular1 = pow(saturate(dot(reflection1, input.viewDirection)), specularPower);

	}

	if (lightIntensity2 > 0.0f)
	{
		color2 += diffuseColor[1] * lightIntensity2;
		color2 = saturate(color2);

		// Calculate the reflection vector based on the light intensity, normal vector, and light direction.
		//reflection2 = normalize(2 * lightIntensity2 * bumpNormal - input.lightPos2);

		// Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
		//specular2 = pow(saturate(dot(reflection2, input.viewDirection)), specularPower);

	}

	if (lightIntensity3 > 0.0f)
	{
		color3 += diffuseColor[2] * lightIntensity3;
		color3 = saturate(color3);

		// Calculate the reflection vector based on the light intensity, normal vector, and light direction.
	//	reflection3 = normalize(2 * lightIntensity3 * bumpNormal - input.lightPos3);

		// Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
		//specular3 = pow(saturate(dot(reflection3, input.viewDirection)), specularPower);

	}

	if (lightIntensity4 > 0.0f)
	{
		color4 += diffuseColor[3] * lightIntensity4;
		color4 = saturate(color4);

		// Calculate the reflection vector based on the light intensity, normal vector, and light direction.
		//reflection4 = normalize(2 * lightIntensity4 * bumpNormal - input.lightPos4);

		// Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
		//specular4 = pow(saturate(dot(reflection4, input.viewDirection)), specularPower);

	}

	lightDir = -lightDirection;
	lightIntensity = saturate(dot(bumpNormal, lightDir));

	if (lightIntensity > 0.0f)
	{
		// Calculate the reflection vector based on the light intensity, normal vector, and light direction.
		reflection = normalize(2 * lightIntensity * bumpNormal - lightDir);

		// Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
		specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);

	}

	color = saturate(color1 + color2 + color3 + color4 + ambientColor) * textureColor ;
	
	// Add the specular component last to the output color.
	//color = saturate(color + specular1 + specular2 + specular3 + specular4);
	color = saturate(color + specular);

	return color;
}
