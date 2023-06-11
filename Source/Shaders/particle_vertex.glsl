#version 460 

layout(location = 0) uniform mat4 proj;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 model;

layout(location = 0) in vec3 vertexPos;
layout(location = 2) in vec2 vertexUv0;

layout(location = 3) in vec3 instanceRight;
layout(location = 4) in vec3 instanceUp;
layout(location = 5) in vec3 instanceFront;
layout(location = 6) in vec3 instanceTrans;
layout(location = 7) in vec4 instanceColor;
layout(location = 8) in float instanceFrame;

layout(location = 9) uniform int xTiles;
layout(location = 10) uniform int yTiles;

out vec2 fragUv0;
out vec2 fragUv1;
out vec3 fragPos;
out vec4 fragColor;

void CalculateUV(in float frame, in int xTiles, in int yTiles, in vec2 srcUv, out vec2 uv)
{
	float X = trunc(mod(frame, xTiles));
	float Y = (yTiles-1)-trunc(frame/xTiles);

	uv.x = mix(X, X+1, srcUv.x)/xTiles;
	uv.y = mix(Y, Y+1, srcUv.y)/yTiles;
}

void main()
{
	mat4 transform = mat4(
		vec4(instanceRight, 0.0), 
		vec4(instanceUp, 0.0), 
		vec4(instanceFront, 0.0), 
		vec4(instanceTrans, 1.0));

	fragPos = (model*transform*vec4(vertexPos, 1.0)).xyz;
	fragColor = instanceColor;

	CalculateUV(instanceFrame, xTiles, yTiles, vertexUv0, fragUv0);

	gl_Position = proj*view*vec4(fragPos, 1.0);
}