#version 460

#extension GL_ARB_shading_language_include : require

#include "/Common/Functions/pbr_functions.glsl"

#include "/Common/Structs/lights.glsl"

#define CASCADES 3

layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gDiffuse;
layout(binding = 3) uniform sampler2D gSpecular;
layout(binding = 4) uniform sampler2D gEmissive;
layout(binding = 5) uniform sampler2DArray gShadowMap;
layout(binding = 6) uniform sampler2D gSSAO;

layout(std140, binding=1) uniform Directional
{
	vec3 directionalDir;  	//12	//0
	vec4 directionalColor;	//16	//16     // note: alpha parameter of colour is the intensity 
};

layout(std140, row_major, binding = 2) uniform LightSpaceMatrices 
{
    mat4 projMatrix[CASCADES];    // 16 // 00 (column 0)
                                  // 16 // 16 (column 1)
                                  // 16 // 32 (column 2)
                                  // 16 // 48 (column 3)
    mat4 viewMatrix[CASCADES];    // 16 // 64 (column 0)
                                  // 16 // 80 (column 1)
                                  // 16 // 96 (column 2)
                                  // 16 // 112 (column 3)
};

layout(std140, binding = 3) uniform CascadePlaneDistances 
{
    float cascadePlaneDistances[CASCADES];
};

// IBL
layout(binding = 8) uniform samplerCube diffuse_IBL;
layout(binding = 9) uniform samplerCube prefiltered_IBL;
layout(binding = 10) uniform sampler2D environmentBRDF;

uniform int numLevels_IBL;
uniform float cubemap_intensity;
uniform int renderMode;

uniform vec3 viewPos;
uniform mat4 cameraViewMatrix;

// Shadow Mapping
uniform float minBias;
uniform float maxBias;
uniform float amount;
uniform int useShadows;
uniform int useVSM;
uniform int useSSAO;
uniform int toggleCSMDebug;

in vec2 TexCoord;

out vec4 outColor;

vec3 calculateDirectionalLight(vec3 N, vec3 V, vec3 Cd, vec3 f0, float roughness)
{
    vec3 L = normalize(-directionalDir);
    vec3 H = (L+V)/length(L+V);

    float dotNL = max(dot(N,L), EPSILON);

    vec3 FS = fresnelSchlick(f0, max(dot(L,H), EPSILON));
    float SV = smithVisibility(dotNL, max(dot(N,V), EPSILON), roughness);
    float GGXND = GGXNormalDistribution(max(dot(N,H), EPSILON), roughness);

    return (Cd*(1-f0)+0.25*FS*SV*GGXND)*directionalColor.rgb*directionalColor.a*dotNL;
}


float ShadowCalculation(vec4 posFromLight, vec3 normal, int layer)
{
    // perform perspective divide
    vec3 projCoords = posFromLight.xyz / posFromLight.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(gShadowMap, vec3(projCoords.xy, layer)).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(minBias * (1.0 - dot(normal, directionalDir)), maxBias);
    // apply a different bias depending on which shadow map we sample from
    if (layer == CASCADES)
    {
        bias *= 1 / (cascadePlaneDistances[CASCADES - 1] * 0.5f);
    }
    else
    {
        bias *= 1 / (cascadePlaneDistances[layer] * 0.5f);
    }

    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

float linstep(float min, float max, float v) 
{   
    float value = (v - min) / (max - min);
    return clamp(value, 0.0, 1.0); 
} 
    
float ReduceLightBleeding(float p_max, float Amount) 
{   // Remove the [0, Amount] tail and linearly rescale (Amount, 1].    
    return linstep(Amount, 1, p_max); 
}

float ChebyshevUpperBound(vec4 posFromLight, int layer)
{
    // perform perspective divide
    vec3 projCoords = posFromLight.xyz / posFromLight.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
	// We retrive the two moments previously stored (depth and depth*depth)
	vec2 moments = texture(gShadowMap, vec3(projCoords.xy, layer)).rg;
		
	// Surface is fully lit. as the current fragment is before the light occluder
	if (projCoords.z <= moments.x)
		return 1.0 ;
	
	// The fragment is either in shadow or penumbra. We now use chebyshev's upperBound to check
	// How likely this pixel is to be lit (p_max)
	float variance = moments.y - (moments.x*moments.x);
	variance = max(variance, 0.00002);
	
	float d = projCoords.z - moments.x;
	float p_max = variance / (variance + d*d);
	
    // Reduce light bleeding
    p_max = ReduceLightBleeding(p_max, amount);

	return p_max;
}

void main()
{             
    // retrieve data from gbuffer
    vec3 fragPos = texture(gPosition, TexCoord).rgb;
    vec3 norm = texture(gNormal, TexCoord).rgb;
    vec4 textureMat = texture(gDiffuse, TexCoord);
    vec4 specularMat = texture(gSpecular, TexCoord);
    vec4 emissiveMat = texture(gEmissive, TexCoord);
    float smoothness = specularMat.a;

    if(renderMode == 0)
    {
        vec3 viewDir = normalize(viewPos - fragPos);

        vec3 Cd = textureMat.rgb;
        vec3 f0 = specularMat.rgb;

        // smoothness and roughness
        float roughness = pow(1-smoothness,2) + EPSILON;

        // Shadow Mapping
        float shadow = 1.0;
        vec3 layerColor = vec3(0.0, 0.0, 0.0);
        if (useShadows > 0)
        {
            vec4 fragPosViewSpace = cameraViewMatrix * vec4(fragPos, 1.0);
            float depthValue = abs(fragPosViewSpace.z);
    
            int layer = -1;
            for (int i = 0; i < CASCADES; ++i)
            {
                if (depthValue < cascadePlaneDistances[i])
                {
                    layer = i;
                    layerColor[i] = 1.0;
                    break;
                }
            }
            if (layer == -1)
            {
                layer = CASCADES;
            }
    
            vec4 fragPosFromLightSpace = projMatrix[layer] * viewMatrix[layer] * vec4(fragPos, 1.0);

            if (useVSM == 1)
            {
                shadow = ChebyshevUpperBound(fragPosFromLightSpace, layer);
            }
            else
            {
                shadow = 1 - ShadowCalculation(fragPosFromLightSpace, norm, layer);
            }            
        }
    
        // Lights
        vec3 Lo = shadow * calculateDirectionalLight(norm, viewDir, Cd, f0, roughness);

        vec3 R = reflect(-viewDir, norm);
        float NdotV = max(dot(norm, viewDir), EPSILON);
        vec3 ambient = GetAmbientLight(norm, R, NdotV, roughness, Cd, f0, diffuse_IBL, prefiltered_IBL, 
            environmentBRDF, numLevels_IBL) * cubemap_intensity;

        vec3 color = ambient + Lo + emissiveMat.rgb;
        color += toggleCSMDebug == 1 ? layerColor : vec3(0.0, 0.0, 0.0);
        color = useSSAO == 1 ? color*texture(gSSAO, TexCoord).r : color;
        outColor = vec4(color, 1.0);
    }
    else if (renderMode == 1)
    {
        outColor = vec4(fragPos, 1.0);
    }
    else if (renderMode == 2)
    {
        outColor = vec4(norm, 1.0);
    }
    else if (renderMode == 3)
    {
        outColor = vec4(textureMat);
    }
    else if (renderMode == 4)
    {
        outColor = vec4(specularMat);
    }
    else if (renderMode == 5)
    {
        outColor = vec4(emissiveMat);
    }
    
}