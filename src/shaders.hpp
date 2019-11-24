#include <GL/gl3w.h>

#define uniformFunc(...) setUniform(GLint location, __VA_ARGS__)

class shader {
private:
	GLuint glProgram;
	bool has_failed;

public:
	shader(const char* vtxFilename, const char* fragFilename);
	void use();
	bool fail();
	GLint getUniformLocation(const char* name);

	// Uniforms overload
	void uniformFunc(float);
	void uniformFunc(float, float, float, float);
	void uniformFunc(double);
};
