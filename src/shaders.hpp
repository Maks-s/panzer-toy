#include <GL/gl3w.h>

class shader {
private:
	GLuint glProgram;
	bool has_failed;

public:
	shader(const char* vtxFilename, const char* fragFilename);
	void use();
	bool fail();
};
