#include "Font.h"

#ifdef USEFREETYPE //Use FREETYPE

Font::Font()
{
	loadedCorrectly = false;
	fontColour = glm::vec4(1.0, 1.0, 1.0, 1.0);
}

bool Font::setupFont()
{
	shader = new Shader(vertexPath, fragmentPath);
	shader->use();

	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(1920), 0.0f, static_cast<GLfloat>(1080));
	shader->setUniform("projection_matrix", projection);

	if (FT_Init_FreeType(&FTLibrary)) { //Init freetype
		fprintf(stderr, "Error: Problem initilizing freetype\n");
		return false;
	}

	if (FT_New_Face(FTLibrary, "fonts/VCR.ttf", 0, &FTFace)) { //Load font
		fprintf(stderr, "Error: Problem opening font fonts/FreeSans.ttf\n");
		return false;
	}

	FT_Set_Pixel_Sizes(FTFace, 0, fontSize); //set font size

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  //Only need 1 byte per texture

	

	characterTextures = std::map<char, characterTexture>();

	for (GLubyte i = 0; i < charLimit; i++) {

		if (FT_Load_Char(FTFace, i, FT_LOAD_RENDER)) {
			//Couldn't load char
			continue;
		}
		FT_GlyphSlot glyph = FTFace->glyph;

		/*Create texture for current character*/
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, glyph->bitmap.width, glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);

		/*Set texture parameters*/
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		characterTexture cT;

		cT.textureID = texture;
		cT.size = glm::ivec2(glyph->bitmap.width, glyph->bitmap.rows);
		cT.offset = glm::ivec2(glyph->bitmap_left, glyph->bitmap_top);
		cT.advance = glyph->advance.x;

		characterTextures.insert(std::pair<char, characterTexture>(i, cT));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	/*SETUP VBO/VAO*/

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	FT_Done_Face(FTFace);
	FT_Done_FreeType(FTLibrary);

	loadedCorrectly = true;
	return true;
}

/**
 * Renders a piece of text at the given position
 * @param text     The string to render
 * @param position glm::vec3 of the position to render(in world xy space)
 * @param scale   float to scale the font
 */
void Font::renderText(std::string text, glm::vec3 position, float scale)
{
	if (!loadedCorrectly) {
		fprintf(stderr, "Error: Attempting to draw font when font didnt load correctly\n");
		return;
	}

	shader->use();
	shader->setUniform("colour", fontColour);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	/*These settings are needed to draw text correctly*/
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const char* cText = text.c_str();

	const char* c = cText;
	while (*c != '\0') {

		characterTexture* cT = &characterTextures[*c];

		/*Calculate the positions and size based on the size and scale of the current character*/
		float x2 = position.x + cT->offset.x * scale;
		float y2 = position.y - (cT->size.y - cT->offset.y) * scale;

		float w = cT->size.x * scale;
		float h = cT->size.y * scale;

		GLfloat vertices[6][4] = {
			{ x2,     y2 + h,   0.0, 0.0 },
			{ x2,     y2,       0.0, 1.0 },
			{ x2 + w, y2,       1.0, 1.0 },
			{ x2,     y2 + h,   0.0, 0.0 },
			{ x2 + w, y2,       1.0, 1.0 },
			{ x2 + w, y2 + h,   1.0, 0.0 }
		};

		glBindTexture(GL_TEXTURE_2D, cT->textureID);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		position.x += (cT->advance >> 6) * scale; //Move position to the next location
		c++;
	}

	//Reset the settings we set before
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

}
#else //FREETYPE library doesn't exist, create skeleton class

Font::Font()
{
	fprintf(stderr, "Warning: Not using fonts\n");
}

void Font::renderText(std::string text, glm::vec3 position, float scale)
{
	//Do nothing since rendering text is disabled
}

bool Font::setupFont()
{
	return true;
}
#endif
