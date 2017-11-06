#version 330

layout (location = 0) in vec3 i_vertex; 
layout (location = 1) in vec2 i_texCoordinates;	
layout (location = 2) in vec3 i_normals;	

uniform vec3 mtl_diffuse;  

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform int draw_mode;

out vec2 texCoordinates;
out vec4 vertColour;

void main(void)
{

	texCoordinates = i_texCoordinates;
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(i_vertex, 1.0);

	vertColour = vec4(0.5, 0.5, 1.0, 1.0);

	if(draw_mode == 0) {
		//Wireframe mode
	}
	else if(draw_mode == 1){ //Draw normals as RGB
		vec3 rgbNormals = normalize(i_normals * 0.5 + 0.5);
		vertColour = vec4(rgbNormals, 1);
	}
	else { //Draw colour as diffuse material
		vertColour = vec4(mtl_diffuse, 1.0);
	}


}
