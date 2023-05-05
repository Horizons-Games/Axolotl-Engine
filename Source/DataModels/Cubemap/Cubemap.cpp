#include "Cubemap.h"

#include "Application.h"
#include "ModuleProgram.h"

#include "FileSystem/ModuleResources.h"
#include "FileSystem/Json.h"

#include "DataModels/Program/Program.h"
#include "DataModels/Resources/ResourceCubemap.h"

#include "Resources/ResourceTexture.h"

#include "Geometry/Frustum.h"

#define CUBEMAP_RESOLUTION 512
#define IRRADIANCE_MAP_RESOLUTION 128
#define PRE_FILTERED_MAP_RESOLUTION 128
#define ENVIRONMENT_BRDF_RESOLUTION 512

Cubemap::Cubemap() : cubemapRes(nullptr), frameBuffer(0), cubemap(0), irradiance(0), preFiltered(0), environmentBRDF(0),
cubeVAO(0), cubeVBO(0), numMipMaps(0)
{
}

Cubemap::~Cubemap()
{
    glDeleteFramebuffers(1, &frameBuffer);
    glDeleteTextures(1, &cubemap);
    glDeleteTextures(1, &irradiance);
    glDeleteTextures(1, &preFiltered);
    glDeleteTextures(1, &environmentBRDF);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
}

Cubemap::Cubemap(std::shared_ptr<ResourceCubemap> cubemapRes) : cubemapRes(cubemapRes)
{
    GenerateMaps();
}

void Cubemap::SaveOptions(Json& json) const
{
    Json jsonSkybox = json["Cubemap"];

    jsonSkybox["cubemapUID"] = cubemapRes->GetUID();
    jsonSkybox["cubemapAssetPath"] = cubemapRes->GetAssetsPath().c_str();
}

void Cubemap::LoadOptions(Json& json)
{
    Json jsonSkybox = json["Cubemap"];

    UID resUID = jsonSkybox["cubemapUID"];
    std::string resPath = jsonSkybox["cubemapAssetPath"];

#ifdef ENGINE
    cubemapRes = App->GetModule<ModuleResources>()->RequestResource<ResourceCubemap>(resPath);
#else
    cubemapRes = App->GetModule<ModuleResources>()->SearchResource<ResourceCubemap>(resUID);
#endif // ENGINE
    GenerateMaps();
}

void Cubemap::DebugNSight()
{
#ifdef ENGINE
    Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::DEFAULT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubemapRes->GetHDRTexture()->GetGlTexture());

    glBindTexture(GL_TEXTURE_CUBE_MAP, preFiltered);
    program->Deactivate();
#endif
}

void Cubemap::GenerateMaps()
{
    cubemapRes->Load();
    CreateVAO();

    //Generate framebuffer & renderBuffer
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    // Disable depth testing
    glDisable(GL_DEPTH_TEST);

    // from hdr to Cubemap
    glGenTextures(1, &cubemap);

    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
            CUBEMAP_RESOLUTION, CUBEMAP_RESOLUTION, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Program* hdrToCubemapProgram = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::HDR_TO_CUBEMAP);
    hdrToCubemapProgram->Activate();

    hdrToCubemapProgram->BindUniformInt("hdr", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubemapRes->GetHDRTexture()->GetGlTexture());

    RenderToCubeMap(cubemap, hdrToCubemapProgram, CUBEMAP_RESOLUTION);
    hdrToCubemapProgram->Deactivate();

    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // irradianceMap
    glGenTextures(1, &irradiance);

    glBindTexture(GL_TEXTURE_CUBE_MAP, irradiance);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
            IRRADIANCE_MAP_RESOLUTION, IRRADIANCE_MAP_RESOLUTION, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Program* irradianceProgram = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::IRRADIANCE_MAP);
    irradianceProgram->Activate();

    irradianceProgram->BindUniformInt("environment", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

    RenderToCubeMap(irradiance, irradianceProgram, IRRADIANCE_MAP_RESOLUTION);
    irradianceProgram->Deactivate();
    // pre-filtered map
    glGenTextures(1, &preFiltered);

    glBindTexture(GL_TEXTURE_CUBE_MAP, preFiltered);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, PRE_FILTERED_MAP_RESOLUTION,
            PRE_FILTERED_MAP_RESOLUTION, 0, GL_RGB, GL_FLOAT, nullptr);
    }

    numMipMaps = static_cast<int>(log(static_cast<float>(PRE_FILTERED_MAP_RESOLUTION)) / log(2));
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, numMipMaps);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    Program* preFilteredProgram = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::PRE_FILTERED_MAP);
    preFilteredProgram->Activate();

    preFilteredProgram->BindUniformInt("environment", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

    preFilteredProgram->BindUniformFloat(5, static_cast<float>(CUBEMAP_RESOLUTION));

    for (int mipMap = 0; mipMap <= numMipMaps; ++mipMap)
    {
        float roughness = static_cast<float>(mipMap) / static_cast<float>(numMipMaps);
        preFilteredProgram->BindUniformFloat(4, roughness);

        unsigned int mipResolution = static_cast<unsigned int>(PRE_FILTERED_MAP_RESOLUTION * std::pow(0.5, mipMap));
        RenderToCubeMap(preFiltered, preFilteredProgram, mipResolution, mipMap);
    }
    preFilteredProgram->Deactivate();

    //environment BRDF
    glGenTextures(1, &environmentBRDF);

    glBindTexture(GL_TEXTURE_2D, environmentBRDF);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, ENVIRONMENT_BRDF_RESOLUTION, ENVIRONMENT_BRDF_RESOLUTION, 0, GL_RG, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Program* environmentBRDFProgram = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::ENVIRONMENT_BRDF);
    environmentBRDFProgram->Activate();
    glViewport(0, 0, ENVIRONMENT_BRDF_RESOLUTION, ENVIRONMENT_BRDF_RESOLUTION);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, environmentBRDF, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    environmentBRDFProgram->Deactivate();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);
}

void Cubemap::RenderToCubeMap(unsigned int cubemapTex, Program* usedProgram, int resolution, int mipmapLevel)
{
    const float3 front[6] = {float3::unitX, -float3::unitX, float3::unitY, -float3::unitY, float3::unitZ, -float3::unitZ};
    const float3 up[6] = {-float3::unitY, -float3::unitY, float3::unitZ, -float3::unitZ, -float3::unitY, -float3::unitY};

    Frustum frustum;
    frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
    frustum.SetPerspective(math::pi / 2.0f, math::pi / 2.0f);
    frustum.SetPos(float3::zero);
    frustum.SetViewPlaneDistances(0.1f, 100.0f);

    usedProgram->BindUniformFloat4x4(1, frustum.ProjectionMatrix().ptr(), GL_TRUE);

    glViewport(0, 0, resolution, resolution);
    glBindVertexArray(cubeVAO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemapTex, mipmapLevel);
        frustum.SetFront(front[i]);
        frustum.SetUp(up[i]);

        usedProgram->BindUniformFloat4x4(0, frustum.ViewMatrix().ptr(), GL_TRUE);
        // Draw cube
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void Cubemap::CreateVAO()
{
    float vertices[108] = {
        // Front (x, y, z)
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        // Left (x, y, z)
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        // Right (x, y, z)
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        // Back (x, y, z)
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        // Top (x, y, z)
        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        // Bottom (x, y, z)
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}