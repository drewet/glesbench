#include "bff.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <memory.h>

#include <fstream>

using namespace std;

#define BFG_RS_NONE  0x0      // Blend flags
#define BFG_RS_ALPHA 0x1
#define BFG_RS_RGB   0x2
#define BFG_RS_RGBA  0x4

#define BFG_MAXSTRING 255     // Maximum string length

#define WIDTH_DATA_OFFSET  20 // Offset to width data with BFF file
#define MAP_DATA_OFFSET   276 // Offset to texture image data with BFF file

enum
{
    GLYPH_WIDTH = 16,
    GLYPH_HEIGHT = 16,
    MAX_STRING_LENGTH = 256
};

//
// CBffFont
//
CBffFont::CBffFont(const char *pFileName):
    CHudBase(MAX_STRING_LENGTH, true)
{
    /*
    FILE *pFile = fopen(pFileName, "rb");

    if (!pFile)
    {
        puts("Failed to open font file.");
        return;
    }
    */

    if (!LoadFont(pFileName))
        return;
}

//
// ~CBffFont
//
CBffFont::~CBffFont()
{
}

//
// DrawString
//
void CBffFont::DrawString(int x, int y, const char *pFormat, ...)
{
    char s[MAX_STRING_LENGTH];
    va_list val;

    // Compose string
    va_start(val, pFormat);
    vsnprintf(s, MAX_STRING_LENGTH, pFormat, val);
    va_end(val);

    static HUD_VERTEX Data[4 * MAX_STRING_LENGTH];
    HUD_VERTEX *pVertices = Data;
    unsigned NumGlyphs = 0;

    if (pVertices)
    {
        const int n = (int)strlen(s);

        for (int i = 0; i < n; ++i)
        {
            char chr = s[i];
            int Row = (chr - Base) / RowPitch;
            int Col = (chr - Base) - Row * RowPitch;

            float u = Col * ColFactor;
            float v = Row * RowFactor;
            float u1 = u + ColFactor;
            float v1 = v + RowFactor;

            AddGlyph(pVertices, (float)x, (float)y, CellX, CellY, u, v1, u1, v);

            ++NumGlyphs;
            pVertices += 4;

            x += Width[chr];
        }

        glBindBuffer(GL_ARRAY_BUFFER, m_VB);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(HUD_VERTEX) * 4 * NumGlyphs, Data);
    }

    BeginDraw();
    {
        glDrawElements(GL_TRIANGLES, NumGlyphs * 6, GL_UNSIGNED_SHORT, 0);
    }
    EndDraw();
}

//
// BeginDraw
//
void CBffFont::BeginDraw()
{
    CHudBase::BeginDraw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(m_Program);
    glUniformMatrix4fv(m_Mproj, 1, GL_FALSE, (const GLfloat *)&m_Ortho);
    glUniform1i(m_Tex, 0); // Texture unit
}

//
// EndDraw
//
void CBffFont::EndDraw()
{
    glDisable(GL_BLEND);

    CHudBase::EndDraw();
}

//
// LoadFont
//
bool CBffFont::LoadFont(const char *fname)
{
 char *dat,*img;
 fstream in;
 unsigned long fileSize;
 char bpp;
 int ImgX,ImgY;

 in.open(fname, ios_base::binary | ios_base::in);

  if(in.fail())
   return false;

 // Get Filesize
 in.seekg(0,ios_base::end);
 fileSize=in.tellg();
 in.seekg(0,ios_base::beg);

 // allocate space for file data
 dat=new char[fileSize];

  // Read filedata
  if(!dat)
   return false;

 in.read(dat,fileSize);

  if(in.fail())
   {
    delete [] dat;
    in.close();
    return false;
   }

 in.close();

  // Check ID is 'BFF2'
  if((unsigned char)dat[0]!=0xBF || (unsigned char)dat[1]!=0xF2)
   {
    delete [] dat;
    return false;
   }

 // Grab the rest of the header
 memcpy(&ImgX,&dat[2],sizeof(int));
 memcpy(&ImgY,&dat[6],sizeof(int));
 memcpy(&CellX,&dat[10],sizeof(int));
 memcpy(&CellY,&dat[14],sizeof(int));
 bpp=dat[18];
 Base=dat[19];

  // Check filesize
  if(fileSize!=((MAP_DATA_OFFSET)+((ImgX*ImgY)*(bpp/8))))
      return false;

 // Calculate font params
 RowPitch=ImgX/CellX;
 ColFactor=(float)CellX/(float)ImgX;
 RowFactor=(float)CellY/(float)ImgY;
 YOffset=CellY;

 // Determine blending options based on BPP
  switch(bpp)
   {
    case 8: // Greyscale
     RenderStyle=BFG_RS_ALPHA;
     break;

    case 24: // RGB
     RenderStyle=BFG_RS_RGB;
     break;

    case 32: // RGBA
     RenderStyle=BFG_RS_RGBA;
     break;

    default: // Unsupported BPP
     delete [] dat;
     return false;
     break;
   }

 // Allocate space for image
 img=new char[(ImgX*ImgY)*(bpp/8)];

  if(!img)
   {
    delete [] dat;
    return false;
   }

 // Grab char widths
 memcpy(Width,&dat[WIDTH_DATA_OFFSET],256);

 // Grab image data
 memcpy(img,&dat[MAP_DATA_OFFSET],(ImgX*ImgY)*(bpp/8));

 // Create Texture
 glGenTextures(1, &m_Texture);
 glBindTexture(GL_TEXTURE_2D, m_Texture);
 // Fonts should be rendered at native resolution so no need for texture filtering
 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
 // Stop chararcters from bleeding over edges
 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

 // Tex creation params are dependent on BPP
  switch(RenderStyle)
   {
    case BFG_RS_ALPHA:
     glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,ImgX,ImgY,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,img);
     break;

    case BFG_RS_RGB:
     glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,ImgX,ImgY,0,GL_RGB,GL_UNSIGNED_BYTE,img);
     break;

    case BFG_RS_RGBA:
     glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,ImgX,ImgY,0,GL_RGBA,GL_UNSIGNED_BYTE,img); // GL ES!
     break;
   }

 // Clean up
 delete [] img;
 delete [] dat;

 return true;
}
