
#ifndef I_SURFACE_H
#define I_SURFACE_H

#include "string.h"
#include "common.h"

namespace Raytracer {

class Surface
{
	enum
	{
		OWNER = 1 //^PWNS!1!
	};

public:
	// constructor / destructors
	Surface( int a_Width, int a_Height );
	Surface( char* a_File );
	
	~Surface();

	// acceso a la data miembro
    Pixel* GetBuffer() { return m_Buffer; } // Pixel es tan sólo un typedef int 
	int GetWidth() { return m_Width; }
	int GetHeight() { return m_Height; }

	// operaciones especiales
	void InitCharset();
	void SetChar( int c, char* c1, char* c2, char* c3, char* c4, char* c5 );
	void Print( char* a_String, int x1, int y1, Pixel color );
	void Clear( Pixel a_Color );

private:
	// atributos
	Pixel* m_Buffer;	
	int m_Width, m_Height;	
	
	// atributos static 
	char s_Font[51][5][5];	
	int s_Transl[256];		
};

}; // namespace Raytracer

#endif