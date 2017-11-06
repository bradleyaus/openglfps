#version 330
out vec4 fragColour;

in vec3 texCoordinates;

uniform samplerCube texMap;

void main()
{    
    fragColour = texture(texMap, texCoordinates);
}