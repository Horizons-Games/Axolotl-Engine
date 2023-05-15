#include "ComponentAreaLight.h"

#include "MathGeoLib/Include/Math/MathConstants.h"
#include "GL/glew.h"

#define SECTOR_SPHERE_COUNT 36
#define STACK_SPHERE_COUNT 34
#define SECTOR_TUBE_COUNT 36
#define STACK_TUBE_COUNT 34

ComponentAreaLight::ComponentAreaLight() : ComponentLight(LightType::AREA, true)
{
}

ComponentAreaLight::ComponentAreaLight(const ComponentAreaLight& componentAreaLight) :
	ComponentLight(componentAreaLight), areaType(componentAreaLight.areaType), radius(componentAreaLight.radius),
    vao(0), vbo(0), ebo(0)
{
}

ComponentAreaLight::ComponentAreaLight(AreaType areaType, GameObject* parent) :
	ComponentLight(LightType::AREA, parent, true), areaType(areaType), radius(1.f), vao(0), vbo(0), ebo(0)
{	
}

ComponentAreaLight::ComponentAreaLight(const float3& color, float intensity, AreaType areaType) :
	ComponentLight(LightType::AREA, color, intensity, true), areaType(areaType), radius(1.f), vao(0), vbo(0), ebo(0)
{
	switch (areaType)
	{
	case AreaType::SPHERE:
        CreateSphere();
        GenerateSphereTriangles();
		break;
	case AreaType::TUBE:
		break;
	case AreaType::QUAD:
		break;
	case AreaType::DISC:
		break;
	case AreaType::NONE:
		break;
	default:
		break;
	}
    CreateEBO();
    CreateVBO();
    CreateVAO();
}

ComponentAreaLight::ComponentAreaLight(const float3& color, float intensity, GameObject* parent, AreaType areaType) :
	ComponentLight(LightType::AREA, color, intensity, parent, true), areaType(areaType), radius(1.f), vao(0), vbo(0), ebo(0)
{
}

ComponentAreaLight::~ComponentAreaLight()
{
}

void ComponentAreaLight::CreateVAO()
{
    if (vao == 0)
    {
        glGenVertexArrays(1, &vao);
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    //positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    //texCoords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(vertices)));

    //normals
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(vertices) + sizeof(texCoords)));
}

void ComponentAreaLight::CreateVBO()
{
    if (vbo == 0)
    {
        glGenBuffers(1, &vbo);
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    unsigned positionSize = sizeof(vertices);
    unsigned uvSize = sizeof(texCoords);
    unsigned normalsSize = sizeof(normals);

    unsigned bufferSize = positionSize + uvSize + normalsSize;

    glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, positionSize, &(vertices[0]));

    GLuint uvOffset = positionSize;

    float2* uvs = (float2*)(glMapBufferRange(GL_ARRAY_BUFFER, uvOffset, uvSize, GL_MAP_WRITE_BIT));

    for (unsigned int i = 0; i < texCoords.size(); ++i)
    {
        uvs[i] = texCoords[i];
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);

    unsigned normalsOffset = positionSize + uvSize;
    glBufferSubData(GL_ARRAY_BUFFER, normalsOffset, normalsSize, &normals[0]);
}

void ComponentAreaLight::CreateEBO()
{
    if (ebo == 0)
    {
        glGenBuffers(1, &ebo);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(facesIndices), nullptr, GL_STATIC_DRAW);

    GLuint* indices = (GLuint*)(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));

    for (unsigned int i = 0; i < facesIndices.size(); ++i)
    {
        assert(facesIndices[i].size()); // note: assume triangles = 3 indices per face
        *(indices++) = facesIndices[i][0];
        *(indices++) = facesIndices[i][1];
        *(indices++) = facesIndices[i][2];
    }

    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

void ComponentAreaLight::CreateSphere()
{
    vertices.clear();
    normals.clear();
    texCoords.clear();

    vertices.reserve(SECTOR_SPHERE_COUNT * STACK_SPHERE_COUNT);
    normals.reserve(SECTOR_SPHERE_COUNT * STACK_SPHERE_COUNT);
    texCoords.reserve(SECTOR_SPHERE_COUNT * STACK_SPHERE_COUNT);

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    float s, t;                                     // vertex texCoord

    float sectorStep = 2 * math::pi / SECTOR_SPHERE_COUNT;
    float stackStep = math::pi / STACK_SPHERE_COUNT;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= STACK_SPHERE_COUNT; ++i)
    {
        stackAngle = math::pi / 2 - i * stackStep;  // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= SECTOR_SPHERE_COUNT; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            vertices.push_back(float3(x, y, z));

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            normals.push_back(float3(nx, ny, nz));

            // vertex tex coord (s, t) range between [0, 1]
            s = static_cast<float>(j) / SECTOR_SPHERE_COUNT;
            t = static_cast<float>(i) / STACK_SPHERE_COUNT;
            texCoords.push_back(float2(s, t));
        }
    }
}

void ComponentAreaLight::GenerateSphereTriangles()
{
    unsigned int k1, k2;
    for (int i = 0; i < STACK_SPHERE_COUNT; ++i)
    {
        k1 = i * (SECTOR_SPHERE_COUNT + 1);     // beginning of current stack
        k2 = k1 + SECTOR_SPHERE_COUNT + 1;      // beginning of next stack

        for (int j = 0; j < SECTOR_SPHERE_COUNT; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0)
            {
                std::vector<unsigned int> face { k1, k2, k1 + 1 };
                facesIndices.push_back(face);
            }

            // k1+1 => k2 => k2+1
            if (i != (STACK_SPHERE_COUNT - 1))
            {
                std::vector<unsigned int> face{ k1 + 1, k2, k2 + 1 };
                facesIndices.push_back(face);
            }

            // store indices for lines
            // vertical lines for all stacks, k1 => k2
            lineIndices.push_back(k1);
            lineIndices.push_back(k2);
            if (i != 0)  // horizontal lines except 1st stack, k1 => k+1
            {
                lineIndices.push_back(k1);
                lineIndices.push_back(k1 + 1);
            }
        }
    }
}
