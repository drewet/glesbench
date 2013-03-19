//
// Transform shader
//
const GLchar *g_pVshTransform =
"uniform mat4       Mwvp;\n"
"\n"
"attribute vec3     position;\n"
"\n"  
"void main()\n"
"{\n"
"    gl_Position = Mwvp * vec4(position, 1.);\n"
"}\n";

//
// Glyph vertex shader
//
const GLchar *g_pVshGlyph =
"uniform mat4       Mproj;\n"
"\n"
"attribute vec2     position;\n"
"attribute vec2     texcoord;\n"
"\n"
"varying vec2       oTexcoord;\n"
"void main()\n"
"{\n"
"    gl_Position = Mproj * vec4(position, 0., 1.);\n"
"    oTexcoord = texcoord;\n"
"}\n";

//
// Phong lighting vertex shader
//
const GLchar *g_pVshPhong =
"uniform mat4       Mpivot;\n"
"uniform mat4       Mworld;\n"
"uniform mat4       Mwvp;\n"
"\n"
"attribute vec3     position;\n"
"attribute vec3     normal;\n"
"\n"  
"varying vec3       oPos;\n"
"varying vec3       oNormal;\n"
"void main()\n"
"{\n"
"    gl_Position = Mwvp * vec4(position, 1.);\n"
"    oPos = (Mworld * vec4(position, 1.)).xyz;\n"
"    oNormal = (Mpivot * vec4(normal, 1.)).xyz;\n"
"}\n";
