//
// Fill shader
//
const GLchar *g_pFshFill =
"uniform vec3       color;\n"
"\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.);\n"
"}\n";

//
// Glyph fragment shader
//
const GLchar *g_pFshGlyph =
"uniform sampler2D  font;\n"
"\n"
"varying vec2       oTexcoord;\n"
"\n"
"void main()\n"
"{\n"
"    vec4 color = texture2D(font, oTexcoord);\n"
"    if (color.a < 1.0)\n"
"        discard;\n"
#ifdef USE_GLES
"    gl_FragColor = vec4(color.bgr, 1.);\n" // There is no GL_BGRA format in OpenGL ES
#else
"    gl_FragColor = vec4(color.rgb, 1.);\n"
#endif
"}\n";
