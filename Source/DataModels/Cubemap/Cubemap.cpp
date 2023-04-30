#include "Cubemap.h"

#include "Application.h"
#include "ModuleProgram.h"

#include "FileSystem/ModuleResources.h"

#include "DataModels/Program/Program.h"

#include "Resources/ResourceTexture.h"

#include "Geometry/Frustum.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define CUBEMAP_RESOLUTION 512
#define IRRADIANCE_MAP_RESOLUTION 128
#define PRE_FILTERED_MAP_RESOLUTION 128
#define ENVIRONMENT_BRDF_RESOLUTION 512

Cubemap::Cubemap()
{
    CreateVAO();
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = stbi_loadf("Assets/Textures/SunsetSkyboxHD.hdr", &width, &height, &nrComponents, 0);
    SDL_assert(data);

    glGenTextures(1, &hdrTexture);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

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

    Program* hdrToCubemapProgram = App->program->GetProgram(ProgramType::HDR_TO_CUBEMAP);
    hdrToCubemapProgram->Activate();

    hdrToCubemapProgram->BindUniformInt("hdr", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    RenderToCubeMap(cubemap, hdrToCubemapProgram, CUBEMAP_RESOLUTION);

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

    Program* irradianceProgram = App->program->GetProgram(ProgramType::IRRADIANCE_MAP);
    irradianceProgram->Activate();

    irradianceProgram->BindUniformInt("environment", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

    RenderToCubeMap(irradiance, irradianceProgram, IRRADIANCE_MAP_RESOLUTION);

    // pre-filtered map
    glGenTextures(1, &preFiltered);

    glBindTexture(GL_TEXTURE_CUBE_MAP, preFiltered);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, PRE_FILTERED_MAP_RESOLUTION,
            PRE_FILTERED_MAP_RESOLUTION, 0, GL_RGB, GL_FLOAT, nullptr);
    }

    int numMipMaps = static_cast<int>(log(static_cast<float>(PRE_FILTERED_MAP_RESOLUTION)) / log(2));
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, numMipMaps);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    Program* preFilteredProgram = App->program->GetProgram(ProgramType::PRE_FILTERED_MAP);
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

    //environment BRDF
    glGenTextures(1, &environmentBRDF);

    glBindTexture(GL_TEXTURE_2D, environmentBRDF);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, ENVIRONMENT_BRDF_RESOLUTION, ENVIRONMENT_BRDF_RESOLUTION, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Program* environmentBRDFProgram = App->program->GetProgram(ProgramType::ENVIRONMENT_BRDF);
    environmentBRDFProgram->Activate();
    glViewport(0, 0, ENVIRONMENT_BRDF_RESOLUTION, ENVIRONMENT_BRDF_RESOLUTION);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, environmentBRDF, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    environmentBRDFProgram->Deactivate();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Cubemap::~Cubemap()
{
    glDeleteFramebuffers(1, &frameBuffer);
    glDeleteRenderbuffers(1, &renderBuffer);
    glDeleteTextures(1, &cubemap);
    glDeleteTextures(1, &irradiance);
    glDeleteTextures(1, &preFiltered);
    glDeleteTextures(1, &environmentBRDF);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
}

void Cubemap::DebugNSight()
{
#ifdef ENGINE
    Program* program = App->program->GetProgram(ProgramType::DEFAULT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glBindTexture(GL_TEXTURE_CUBE_MAP, preFiltered);
    program->Deactivate();
#endif
}

void Cubemap::RenderToCubeMap(unsigned int cubemapTex, Program* usedProgram, int resolution, int roughness)
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
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemapTex, roughness);
        //glClearColor(1.0, 1.0, 1.0, 1.0);
        frustum.SetFront(front[i]);
        frustum.SetUp(up[i]);
        
        usedProgram->BindUniformFloat4x4(0, frustum.ViewMatrix().ptr(), GL_TRUE);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Draw cube
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    usedProgram->Deactivate();
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
