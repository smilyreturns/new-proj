#version 410
in vec4 outColor;
		
out vec4 fragColor; 
	
in float fragTime;

uniform sampler2D theTexture;

void main() 			
{ 						
	vec4 inverse = vec4(sin(outColor.x),
						cos(outColor.y), 
						tan(outColor.z), 1);
	fragColor = inverse; 
}						