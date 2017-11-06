#version 330 core
in vec2 texCoordinates;

uniform sampler2D texMap;
uniform vec4 colour;

out vec4 fragColour;

void main()
{    
    fragColour = vec4(1.0, 1.0, 1.0, texture(texMap, texCoordinates).r) * colour;
}