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
"    gl_FragColor = vec4(color.xyz, 1.);\n"
"}\n";
