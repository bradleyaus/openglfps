#version 330

uniform sampler2D texMap;

in vec2 texCoordinates;
in vec4 vertColour;

out vec4 fragColour;


void main(void)
{ 
	fragColour = vertColour;
}
