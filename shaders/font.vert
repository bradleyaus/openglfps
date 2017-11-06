#version 330 core

layout (location = 0) in vec4 i_vertex; 

uniform mat4 projection_matrix;

out vec2 texCoordinates; 

void main(void) 
{
    gl_Position = projection_matrix * vec4(i_vertex.xy, 0.0, 1.0);
    texCoordinates = i_vertex.zw;
}
