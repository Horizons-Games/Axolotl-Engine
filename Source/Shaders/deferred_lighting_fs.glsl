#version 460

#extension GL_ARB_shading_language_include : require

#include "/Common/Functions/pbr_functions.glsl"

#include "/Common/Structs/lights.glsl"

layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gDiffuse;
layout(binding = 3) uniform sampler2D gSpecular;

layout(std140, binding=1) uniform Directional
{
	vec3 directionalDir;  	//12	//0
	vec4 directionalColor;	//16	//16     // note: alpha parameter of colour is the intensity 
};

readonly layout(std430, binding=2) buffer PointLights
{
	uint num_point;			//4		//0
	PointLight points[]; 	//32	//16
};

readonly layout(std430, binding=3) buffer SpotLights
{
	uint num_spot;
	SpotLight spots[];
};

readonly layout(std430, binding=4) buffer AreaLightsSphere
{
	uint num_spheres;
	AreaLightSphere areaSphere[];
};

readonly layout(std430, binding=5) buffer AreaLightsTube
{
	uint num_tubes;
	AreaLightTube areaTube[];
};

// IBL
layout(binding = 8) uniform samplerCube diffuse_IBL;
layout(binding = 9) uniform samplerCube prefiltered_IBL;
layout(binding = 10) uniform sampler2D environmentBRDF;

uniform int numLevels_IBL;
uniform float cubemap_intensity;
uniform int renderMode;

in vec2 TexCoord;
vec3 FragPos;
uniform vec3 ViewPos;
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

vec3 calculatePointLights(vec3 N, vec3 V, vec3 Cd, vec3 f0, float roughness)
{
    vec3 Lo = vec3(0.0);

    for (int i = 0; i < num_point; ++i)
    {
        vec3 pos = points[i].position.xyz;
        vec3 color = points[i].color.rgb;
        float radius = points[i].position.w;
        float intensity = points[i].color.a;

        vec3 L = normalize(pos-FragPos);
        vec3 H = (L+V)/length(L+V);
        float dotNL = max(dot(N,L), EPSILON);

        vec3 FS = fresnelSchlick(f0, max(dot(L,H), EPSILON));
        float SV = smithVisibility(dotNL, max(dot(N,V), EPSILON), roughness);
        float GGXND = GGXNormalDistribution(max(dot(N,H), EPSILON), roughness);

        // Attenuation
        float distance = length(FragPos-pos);
        float maxValue = pow(max(1-pow(distance/radius,4), 0),2);
        float attenuation = maxValue/(pow(distance,2) + 1);

        vec3 Li = color*intensity*attenuation;

        Lo += (Cd*(1-f0)+0.25*FS*SV*GGXND)*Li*dotNL;
    }

    return Lo;
}

vec3 calculateSpotLights(vec3 N, vec3 V, vec3 Cd, vec3 f0, float roughness)
{
    vec3 Lo = vec3(0.0);

    for (int i = 0; i < num_spot; ++i)
    {
        vec3 pos = spots[i].position.xyz;
        vec3 aim = spots[i].aim;
        vec3 color = spots[i].color.rgb;
        float radius = spots[i].position.w;
        float intensity = spots[i].color.a;
        float innerAngle = spots[i].innerAngle;
        float outerAngle = spots[i].outerAngle;

        float cosInner = cos(innerAngle);
        float cosOuter = cos(outerAngle);

        vec3 L = normalize(pos-FragPos);
        vec3 H = (L+V)/length(L+V);
        float dotNL = max(dot(N,L), EPSILON);

        vec3 FS = fresnelSchlick(f0, max(dot(L,H), EPSILON));
        float SV = smithVisibility(dotNL, max(dot(N,V), EPSILON), roughness);
        float GGXND = GGXNormalDistribution(max(dot(N,H), EPSILON), roughness);

        // Attenuation
        float distance = dot(FragPos - pos, aim);
        float maxValue = pow(max(1 - pow(distance/radius,4), 0),2);
        float attenuation = maxValue/(pow(distance,2) + 1);

        float C = dot(-L, aim);
        float Catt = 0.0;

        if (C > cosInner)
        {
            Catt = 1.0;
        }
        else if (cosInner > C && C > cosOuter)
        {
            Catt = (C-cosOuter)/(cosInner-cosOuter);
        }
    
        vec3 Li = color*intensity*attenuation*Catt;
            
        Lo += (Cd*(1-f0)+0.25*FS*SV*GGXND)*Li*dotNL;
    }

    return Lo;
}

float GGXNDAreaLight(float dotNH, float roughness, float alpha, float alphaPrime)
{
    float alpha2 = alpha * alpha;
    float alphaPrime2 = alphaPrime * alphaPrime;
    
    return (alpha2 * alphaPrime2) / pow( dotNH * dotNH * ( alpha2 - 1.0 ) + 1.0, 2.0);
}

vec3 calculateAreaLightSpheres(vec3 N, vec3 V, vec3 Cd, vec3 f0, float roughness)
{
    vec3 Lo = vec3(0.0);

    for (int i = 0; i < num_spheres; ++i)
    {
        vec3 sP = areaSphere[i].position.xyz;
        vec3 color = areaSphere[i].color.rgb;
        float sR = areaSphere[i].position.w;
        float intensity = areaSphere[i].color.a;
        float lightRadius = areaSphere[i].lightRadius;

        // calculate closest point light specular
        vec3 oldL = sP - FragPos;
        vec3 R = normalize(reflect(-V, N));
        vec3 centerToRay = max(dot(oldL, R), EPSILON) * R - oldL;
        vec3 closest = oldL + centerToRay * min(sR/length(centerToRay),1.0);

        float distance = length(closest);
        vec3 L = closest/distance;
        vec3 H = normalize(L + V);
        float specularDotNL = max(dot(N,L), EPSILON);
        
        float alpha = roughness * roughness;
        float alphaPrime = clamp(sR/(distance*2.0)+alpha, 0.0f, 1.0f);
        float D = GGXNDAreaLight(max(dot(N,H), EPSILON), roughness, alpha, alphaPrime);
        vec3 F = fresnelSchlick(f0, max(dot(L,H), EPSILON));
        float G = smithVisibility(specularDotNL, max(dot(N,V), EPSILON), roughness);

        // Attenuation from the closest point view
        float maxValue = pow(max(1-pow(distance/lightRadius,4), 0),2);
        float attenuation = maxValue/(pow(distance,2) + 1);
        
        // calculate closest point light diffuse
        closest = sP;

        L = normalize(closest-FragPos);
        float diffuseDotNL = max(dot(N,L), EPSILON);

        vec3 Li = color * intensity * attenuation;
        vec3 LoSpecular = 0.25 * D * F * G * Li * specularDotNL;
        vec3 LoDiffuse = Cd * (1 - f0) * Li * diffuseDotNL;
        Lo += LoDiffuse + LoSpecular;
    }

    return Lo;
}

vec3 calculateAreaLightTubes(vec3 N, vec3 V, vec3 Cd, vec3 f0, float roughness)
{
    vec3 Lo = vec3(0.0);

    for (int i = 0; i < num_tubes; ++i)
    {
        vec3 posA = areaTube[i].positionA.xyz;
        vec3 posB = areaTube[i].positionB.xyz;
        float tubeRadius = areaTube[i].positionA.w;
        vec3 color = areaTube[i].color.rgb;
        float intensity = areaTube[i].color.a;
        float lightRadius = areaTube[i].lightRadius;

        // calculate closest point light specular
        vec3 PA = posA - FragPos;
        vec3 PB = posB - FragPos;
        float distL0 = length( PA );
	    float distL1 = length( PB );
        float NoL0 = dot( PA, N ) / ( 2.0 * distL0 );
	    float NoL1 = dot( PB, N ) / ( 2.0 * distL1 );
	    float NoL = ( 2.0 * clamp( NoL0 + NoL1, 0, 1)) / ( distL0 * distL1 + dot( PA, PB ) + 2.0 );
        vec3 AB = PB - PA;

        vec3 R = normalize(reflect(-V, N));
		
        float dotABRefle = dot(AB, R);
        float num = dot(R, PA) * dotABRefle - dot(PA, AB);
        float denom = length(AB) * length(AB) - dotABRefle * dotABRefle;
        float t = clamp(num/denom, 0.0f, 1.0f);
        
        vec3 closest = PA + AB * t;
   	
        // calculate closest point light specular
        vec3 oldL = closest;
        vec3 centerToRay = dot(oldL, R) * R - oldL;
        closest = oldL + centerToRay * min(tubeRadius/length(centerToRay),1.0);

        float distance = length(closest);
        vec3 L = closest/distance;
        vec3 H = normalize(L + V);
        float specularDotNL = NoL;

        float alpha = roughness * roughness;
        float alphaPrime = clamp(tubeRadius/(distance*2.0)+alpha, 0.0f, 1.0f);
        float D = GGXNDAreaLight(max(dot(N,H), EPSILON), roughness, alpha, alphaPrime);
        vec3 F = fresnelSchlick(f0, max(dot(L,H), EPSILON));
        float G = smithVisibility(specularDotNL, max(dot(N,V), EPSILON), roughness);

        // Attenuation from the closest point view
        float maxValue = pow(max(1-pow(distance/lightRadius,4), 0),2);
        float attenuation = maxValue/(pow(distance,2) + 1);

        // calculate closest point light diffuse
        float a = length(posA-FragPos);
        float b = length(posB-FragPos);
        float x = (a)/(b + a);
        closest = posA + AB * x;

        L = normalize(closest-FragPos);
        float diffuseDotNL = max(dot(N,L), EPSILON);

        vec3 Li = color * intensity * attenuation;
        vec3 LoSpecular = 0.25 * D * F * G * Li * specularDotNL;
        vec3 LoDiffuse = Cd * (1 - f0) * Li * diffuseDotNL;
        Lo += LoDiffuse + LoSpecular;
    }

    return Lo;
}

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoord).rgb;
    vec3 norm = texture(gNormal, TexCoord).rgb;
    vec4 textureMat = texture(gDiffuse, TexCoord);
    vec4 specularMat = texture(gSpecular, TexCoord);
    float smoothness = specularMat.a;

    vec3 viewDir = normalize(ViewPos - FragPos);
    vec4 gammaCorrection = vec4(2.2);

    vec3 f0 = specularMat.rgb;

    // smoothness and roughness
    float roughness = pow(1-smoothness,2) + EPSILON;
    
    // Lights
    vec3 Lo = calculateDirectionalLight(norm, viewDir, textureMat.rgb, f0, roughness);

    if (num_point > 0)
    {
        Lo += calculatePointLights(norm, viewDir, textureMat.rgb, f0, roughness);
    }

    if (num_spot > 0)
    {
        Lo += calculateSpotLights(norm, viewDir, textureMat.rgb, f0, roughness);
    }

    if (num_spheres > 0)
    {
        Lo += calculateAreaLightSpheres(norm, viewDir, textureMat.rgb, f0, roughness);
    }

    if (num_tubes > 0)
    {
        Lo += calculateAreaLightTubes(norm, viewDir, textureMat.rgb, f0, roughness);
    }

    vec3 R = reflect(-viewDir, norm);
    float NdotV = max(dot(norm, viewDir), EPSILON);
    vec3 ambient = GetAmbientLight(norm, R, NdotV, roughness, textureMat.rgb, f0, diffuse_IBL, prefiltered_IBL, 
        environmentBRDF, numLevels_IBL) * cubemap_intensity;

    vec3 color = ambient + Lo;
    
	//hdr rendering
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/gammaCorrection));
   
   if(renderMode == 0)
   {
        outColor = vec4(color,1.0);
   }
    else if (renderMode == 1)
    {
        outColor = vec4(FragPos, 1.0f);
    }
    else if (renderMode == 2)
    {
        outColor = vec4(norm, 1.0f);
    }
    else if (renderMode == 3)
    {
        outColor = vec4(textureMat);
    }
    else if (renderMode == 4)
    {
        outColor = vec4(specularMat);
    }
    
}