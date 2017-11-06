#version 330

layout (location = 0) in vec3 i_vertex; 

uniform mat4 projection_matrix;
uniform mat4 view_matrix;

out vec3 texCoordinates; 

void main(void) 
{
    texCoordinates = i_vertex;
    gl_Position = projection_matrix * view_matrix * vec4(i_vertex, 1.0);
}
