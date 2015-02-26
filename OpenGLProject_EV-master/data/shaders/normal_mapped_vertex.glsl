#version 410

in vec4 position;
in vec4 normal;
in vec4 tangent;
in vec2 texCoord;

out vec3 fragPosition;
out vec3 fragNormal;
out vec3 fragTangent;
out vec3 fragBiTangent;
out vec2 fragTexCoord;

uniform mat4 projView;

void main()
{
	fragPosition = position.xyz;
	fragNormal = normal.xyz;
	fragTangent = tangent.xyz;
	fragBiTangent = cross(normal.xyz, tangent.xyz);
	fragTexCoord = texCoord;

	gl_Position = projView * position;
}