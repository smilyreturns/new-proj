#version 410

in vec2 fragTexCoord;
out vec4 fragColor;

uniform sampler2D diffuse;

void main() 			
{ 						
	fragColor = texture(diffuse, fragTexCoord).rrrr; 
}						