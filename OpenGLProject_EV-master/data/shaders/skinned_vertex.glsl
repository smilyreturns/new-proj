#version 410

in vec4 position;
in vec2 texCoord;
in vec4 boneIndices;
in vec4 boneWeights;
in vec4 normal;
in vec4 tangent;

out vec3 fragPosition;
out vec2 fragTexCoord;
out vec3 fragNormal;
out vec3 fragTangent;
out vec3 fragBiTangent;

uniform mat4 projView;
uniform mat4 world;
 		
const int MAX_BONES = 128;
uniform mat4 bones[MAX_BONES];

void main()
{
	fragPosition = position.xyz;
	fragNormal = normal.xyz;
	fragTangent = tangent.xyz;
	fragBiTangent = cross(normal.xyz, tangent.xyz);
	fragTexCoord = texCoord;

	ivec4 indices = ivec4(boneIndices);
	vec4 finalPosition = vec4(0, 0, 0, 0);
	finalPosition += bones[indices.x] * position * boneWeights.x;
	finalPosition += bones[indices.y] * position * boneWeights.y;
	finalPosition += bones[indices.z] * position * boneWeights.z;
	finalPosition += bones[indices.w] * position * boneWeights.w;
	finalPosition.w = 1;

	gl_Position = projView * world * finalPosition;
}

//Inverse transpose of the transformation matrix
//M = SRS
//M = S^-1 * R * S^-1

//#version 410								
//
//in vec4 position; 								
//in vec2 texCoord; 	
//in vec4 boneIndices;
//in vec4 boneWeights;
//		
//out vec2 fragTexCoord; 		
//						
//uniform mat4 projView;
//uniform mat4 world;
// 		
//const int MAX_BONES = 128;
//uniform mat4 bones[MAX_BONES];
//
//void main() 									
//{ 										
//	fragTexCoord = texCoord;
//
//	gl_Position = projView * world * finalPosition;
//}