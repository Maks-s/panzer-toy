#include <GL/gl3w.h>

#define PRINT_ERROR(x) std::cerr << x << std::endl

class shader {
private:
	GLuint glProgram;
	bool has_failed;

public:
	shader(const char* vtxFilename, const char* fragFilename);
	void use();
	bool fail();
};
