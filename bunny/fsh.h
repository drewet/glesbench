//
// Fill shader
//
const GLchar *g_pFshFill =
#ifdef GL_ES
"precision mediump float;\n"
#endif
"uniform vec3       color;\n"
"\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.);\n"
"}\n";
