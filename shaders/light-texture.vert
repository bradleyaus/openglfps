#version 330

layout (location = 0) in vec3 i_vertex; 
layout (location = 1) in vec2 i_texCoordinates;	
layout (location = 2) in vec3 i_normals;	

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform mat4 normal_matrix;

smooth out vec4 vertex;
smooth out vec3 normal;
smooth out vec2 uv; 

void main(void) 
{
    vertex = model_matrix * vec4(i_vertex, 1.0);
	normal = (normal_matrix * vec4(i_normals, 1.0)).xyz;
	uv = i_texCoordinates;
	gl_Position = projection_matrix * view_matrix * vertex;
}
