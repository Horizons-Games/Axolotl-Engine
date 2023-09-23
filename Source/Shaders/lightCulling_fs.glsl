#version 460

#extension GL_ARB_shading_language_include : require

#include "/Common/Functions/pbr_functions.glsl"

#include "/Common/Structs/lights.glsl"

layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gDiffuse;
layout(binding = 3) uniform sampler2D gSpecular;

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

uniform vec3 viewPos;
uniform vec2 screenSize;
uniform int renderMode;

uniform int lightIndex;
uniform int flagPoint;
uniform int flagSpot;
uniform int flagSphere;
uniform int flagTube;

out vec4 out_color;

vec3 calculatePointLights(vec3 N, vec3 V, vec3 Cd, vec3 f0, float roughness, vec3 fragPos)
{
    vec3 Lo = vec3(0.0);

    vec3 pos = points[lightIndex].position.xyz;
    vec3 color = points[lightIndex].color.rgb;
    float radius = points[lightIndex].position.w;
    float intensity = points[lightIndex].color.a;
    float distance = length(fragPos-pos);

    if(distance <= radius)
    {
        vec3 L = normalize(pos-fragPos);
        vec3 H = (L+V)/length(L+V);
        float dotNL = max(dot(N,L), EPSILON);

        vec3 FS = fresnelSchlick(f0, max(dot(L,H), EPSILON));
        float SV = smithVisibility(dotNL, max(dot(N,V), EPSILON), roughness);
        float GGXND = GGXNormalDistribution(max(dot(N,H), EPSILON), roughness);

        // Attenuation
        float maxValue = pow(max(1-pow(distance/radius,4.0), 0),2.0);
        float attenuation = maxValue/(pow(distance,2.0) + 1.0);

        vec3 Li = color*intensity*attenuation;

        Lo += (Cd*(1-f0)+0.25*FS*SV*GGXND)*Li*dotNL;
    }

    return Lo;
}

vec3 calculateSpotLights(vec3 N, vec3 V, vec3 Cd, vec3 f0, float roughness, vec3 fragPos)
{
    vec3 Lo = vec3(0.0);

    vec3 pos = spots[lightIndex].position.xyz;
    vec3 aim = spots[lightIndex].aim;
    vec3 color = spots[lightIndex].color.rgb;
    float radius = spots[lightIndex].position.w;
    float distance = dot(fragPos - pos, aim);

    if(distance <= radius)
    {
        float intensity = spots[lightIndex].color.a;
        float innerAngle = spots[lightIndex].innerAngle;
        float outerAngle = spots[lightIndex].outerAngle;

        float cosInner = cos(innerAngle);
        float cosOuter = cos(outerAngle);

        vec3 L = normalize(pos-fragPos);
        vec3 H = (L+V)/length(L+V);
        float dotNL = max(dot(N,L), EPSILON);

        vec3 FS = fresnelSchlick(f0, max(dot(L,H), EPSILON));
        float SV = smithVisibility(dotNL, max(dot(N,V), EPSILON), roughness);
        float GGXND = GGXNormalDistribution(max(dot(N,H), EPSILON), roughness);

        // Attenuation
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

vec3 calculateAreaLightSpheres(vec3 N, vec3 V, vec3 Cd, vec3 f0, float roughness, vec3 fragPos)
{
    vec3 Lo = vec3(0.0);

    vec3 sP = areaSphere[lightIndex].position.xyz;
    vec3 color = areaSphere[lightIndex].color.rgb;
    float sR = areaSphere[lightIndex].position.w;
    float intensity = areaSphere[lightIndex].color.a;
    float lightRadius = areaSphere[lightIndex].lightRadius;
    vec3 R = normalize(reflect(-V, N));
    
    // Attenuation from the closest point view
    float closeDistance = max(length(fragPos-sP)-sR, 0.0);
    if(closeDistance <= lightRadius)
    {
        float maxValue = pow(max(1-pow(closeDistance/lightRadius,4), 0),2);
        float attenuation = maxValue/(pow(closeDistance,2) + 1);

        // calculate closest point light specular
        vec3 closest = ClosestRayToSphere(sR, sP - fragPos, R);

        vec3 L = normalize(closest);
        vec3 H = normalize(L + V);
        float specularDotNL = max(dot(N,L), EPSILON);
    
        float alpha = max(roughness * roughness, EPSILON);
        float alphaPrime = clamp(sR/(closeDistance*2.0)+alpha, 0.0f, 1.0f);
        float D = GGXNDAreaLight(max(dot(N,H), EPSILON), roughness, alpha, alphaPrime);
        vec3 F = fresnelSchlick(f0, max(dot(L,H), EPSILON));
        float G = smithVisibility(specularDotNL, max(dot(N,V), EPSILON), roughness);
    
        // calculate closest point light diffuse
        closest = sP;

        L = normalize(closest-fragPos);
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
    vec2 texCoord = gl_FragCoord.xy / screenSize;

    vec3 fragPos = texture(gPosition, texCoord).rgb;
    vec3 norm = texture(gNormal, texCoord).rgb;
    vec4 textureMat = texture(gDiffuse, texCoord);
    vec4 specularMat = texture(gSpecular, texCoord);

    float smoothness = specularMat.a;

    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 Cd = textureMat.rgb;
    vec3 f0 = specularMat.rgb;

    // smoothness and roughness
    float roughness = pow(1-smoothness,2) + EPSILON;

    vec3 Lo = vec3(0.0);

    if (flagPoint == 1)
    {
        Lo += calculatePointLights(norm, viewDir, Cd, f0, roughness, fragPos);
    }
    
    if (flagSpot == 1)
    {
        Lo += calculateSpotLights(norm, viewDir,Cd, f0, roughness, fragPos);
    }
    
    if (flagSphere == 1)
    {
        Lo += calculateAreaLightSpheres(norm, viewDir, Cd, f0, roughness, fragPos);
    }

    out_color = vec4(Lo, 1.0);
    
}