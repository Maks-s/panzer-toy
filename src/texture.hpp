#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <GL/gl3w.h>

class texture {
private:
	GLuint textureID;
	int width;
	int height;
	bool hasFailed = false;

public:
	texture(const char* filename, bool flip);
	texture(const char* filename) : texture(filename, false) {};
	void bind();

	int getHeight() const { return height; }
	int getWidth() const { return width; }
	bool fail() const { return hasFailed; }
};

#endif // TEXTURE_HPP
