//#version 410
//
//in vec2 fragTexCoord;
//out vec4 fragColor;
//
//uniform sampler2D diffuse;
//
//void main() 			
//{ 						
//	fragColor = texture(diffuse, fragTexCoord); 
//}					

#version 410 

in vec3 fragNormal;
in vec3 fragPosition;
in vec3 fragTangent;
in vec3 fragBiTangent;
in vec2 fragTexCoord;

out vec4 fragColor;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 ambientLight;
uniform vec3 eyePos;
uniform float specularPower;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D specularTexture;

void main()
{
	mat3 TBN = mat3(normalize(fragTangent),
					normalize(fragBiTangent),
					normalize(fragNormal));

	vec3 sampledNormal = texture(normalTexture, fragTexCoord).xyz;
	vec3 adjustedNormal = sampledNormal * 2 - 1;

	vec3 N = normalize(TBN * adjustedNormal);
	
	vec3 materialColor = texture(diffuseTexture, fragTexCoord).xyz;
	
	vec3 L = normalize(lightDir);
	//Ambient
	vec3 ambient = materialColor * ambientLight;
	//Diffuse
	float d = max(0.0, dot(N, -L));
	vec3 diffuse = vec3(d) * lightColor * materialColor;
	//Specular
	vec3 E = normalize(eyePos - fragPosition);
	vec3 R = reflect(L, N);
	float s = max(0.0, dot(R, E));
	
	vec3 materialSpecular = texture(specularTexture, fragTexCoord).xyz;
	
	s = pow(s, specularPower);
	vec3 specular = vec3(s) * lightColor * materialSpecular;

	fragColor = vec4(ambient + diffuse + specular, 1);

	//fragColor = vec4(ambient.x, ambient.y, ambient.z, 1);

}	