#version 410								

in vec4 position; 								
in vec2 texCoord; 			

out vec2 fragTexCoord; 								
uniform mat4 projView; 			
uniform float time;
uniform float height;

void main() 									
{ 										
	fragTexCoord = texCoord;
	vec4 P = position;
	P.y += sin(time + sqrt(position.x)) * height;							
	gl_Position = projView * P;
}