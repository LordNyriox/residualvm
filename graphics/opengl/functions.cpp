#include "graphics/opengl/functions.h"

#ifndef USE_GLES2

// Define all the functions in functions-list.h

#define GL_FUNC(name, type) \
	type name = nullptr;

#include "graphics/opengl/functions-list.h"

#undef GL_FUNC

#endif // USE_GLES2
