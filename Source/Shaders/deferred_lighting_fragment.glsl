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

uniform vec3 viewPos;

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

vec3 calculatePointLights(vec3 N, vec3 V, vec3 Cd, vec3 f0, float roughness, vec3 fragPos)
{
    vec3 Lo = vec3(0.0);

    for (int i = 0; i < num_point; ++i)
    {
        vec3 pos = points[i].position.xyz;
        vec3 color = points[i].color.rgb;
        float radius = points[i].position.w;
        float intensity = points[i].color.a;

        vec3 L = normalize(pos-fragPos);
        vec3 H = (L+V)/length(L+V);
        float dotNL = max(dot(N,L), EPSILON);

        vec3 FS = fresnelSchlick(f0, max(dot(L,H), EPSILON));
        float SV = smithVisibility(dotNL, max(dot(N,V), EPSILON), roughness);
        float GGXND = GGXNormalDistribution(max(dot(N,H), EPSILON), roughness);

        // Attenuation
        float distance = length(fragPos-pos);
        float maxValue = pow(max(1-pow(distance/radius,4), 0),2);
        float attenuation = maxValue/(pow(distance,2) + 1);

        vec3 Li = color*intensity*attenuation;

        Lo += (Cd*(1-f0)+0.25*FS*SV*GGXND)*Li*dotNL;
    }

    return Lo;
}

vec3 calculateSpotLights(vec3 N, vec3 V, vec3 Cd, vec3 f0, float roughness, vec3 fragPos)
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

        vec3 L = normalize(pos-fragPos);
        vec3 H = (L+V)/length(L+V);
        float dotNL = max(dot(N,L), EPSILON);

        vec3 FS = fresnelSchlick(f0, max(dot(L,H), EPSILON));
        float SV = smithVisibility(dotNL, max(dot(N,V), EPSILON), roughness);
        float GGXND = GGXNormalDistribution(max(dot(N,H), EPSILON), roughness);

        // Attenuation
        float distance = dot(fragPos - pos, aim);
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

    for (int i = 0; i < num_spheres; ++i)
    {
        vec3 sP = areaSphere[i].position.xyz;
        vec3 color = areaSphere[i].color.rgb;
        float sR = areaSphere[i].position.w;
        float intensity = areaSphere[i].color.a;
        float lightRadius = areaSphere[i].lightRadius;

        vec3 R = normalize(reflect(-V, N));
        
        // Attenuation from the closest point view
        float closeDistance = max(length(fragPos-sP)-sR, 0.0);
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

vec3 calculateAreaLightTubes(vec3 N, vec3 V, vec3 Cd, vec3 f0, float roughness, vec3 fragPos)
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

        // Attenuation from the closest point view
        vec3 pointAtt = ClosestPointToLine(fragPos, posA, posB, tubeRadius);
        float closeDistance = length(pointAtt - fragPos);
        float maxValue = pow(max(1-pow(closeDistance/lightRadius,4), 0),2);
        float attenuation = maxValue/(pow(closeDistance,2) + 1);

        vec3 R = normalize(reflect(-V, N));
        
        // calculate closest point light specular
        vec3 closest = ClosestRayToLine(fragPos, posA, posB, R);
   	    closest = ClosestRayToSphere(tubeRadius, closest, R);

        vec3 L = normalize(closest);
        vec3 H = normalize(L + V);
        float specularDotNL = max(dot(N,L), EPSILON);

        float alpha = max(roughness * roughness, EPSILON);
        float alphaPrime = clamp(tubeRadius/(closeDistance*2.0)+alpha, 0.0f, 1.0f);
        float D = GGXNDAreaLight(max(dot(N,H), EPSILON), roughness, alpha, alphaPrime);
        vec3 F = fresnelSchlick(f0, max(dot(L,H), EPSILON));
        float G = smithVisibility(specularDotNL, max(dot(N,V), EPSILON), roughness);

        // calculate closest point light diffuse
        float a = length(posA-fragPos);
        float b = length(posB-fragPos);
        float x = (a)/(b + a);
        closest = BisectionIntersection(fragPos, posA, posB);

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
    // retrieve data from gbuffer
    vec3 fragPos = texture(gPosition, TexCoord).rgb;
    vec3 norm = texture(gNormal, TexCoord).rgb;
    vec4 textureMat = texture(gDiffuse, TexCoord);
    vec4 specularMat = texture(gSpecular, TexCoord);
    float smoothness = specularMat.a;

    vec3 viewDir = normalize(viewPos - fragPos);
    vec4 gammaCorrection = vec4(2.2);

    vec3 Cd = textureMat.rgb;
    vec3 f0 = specularMat.rgb;

    // smoothness and roughness
    float roughness = pow(1-smoothness,2) + EPSILON;
    
    // Lights
    vec3 Lo = calculateDirectionalLight(norm, viewDir, Cd, f0, roughness);

    if (num_point > 0)
    {
        Lo += calculatePointLights(norm, viewDir, Cd, f0, roughness, fragPos);
    }

    if (num_spot > 0)
    {
        Lo += calculateSpotLights(norm, viewDir,Cd, f0, roughness, fragPos);
    }

    if (num_spheres > 0)
    {
        Lo += calculateAreaLightSpheres(norm, viewDir, Cd, f0, roughness, fragPos);
    }

    if (num_tubes > 0)
    {
        Lo += calculateAreaLightTubes(norm, viewDir, Cd, f0, roughness, fragPos);
    }

    vec3 R = reflect(-viewDir, norm);
    float NdotV = max(dot(norm, viewDir), EPSILON);
    vec3 ambient = GetAmbientLight(norm, R, NdotV, roughness, Cd, f0, diffuse_IBL, prefiltered_IBL, 
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
        outColor = vec4(fragPos, 1.0f);
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