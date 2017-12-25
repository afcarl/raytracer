
#define WIN32_LEAN_AND_MEAN 
// WIN32_LEAN_AND_MEAN elimina el overhead de MFC para acelerar la compilación 
// (elimina las funciones con sufijo afx) 

#include <windows.h>
#include <stdlib.h>
#include "common.h"
#include "raytracer.h"
#include "scene.h"
#include "surface.h"
#include "string.h"

//**-----------------------
// CONTROLADORES DE LUCES: prendido/apagado 
bool light1Switch=true;
bool light2Switch=true;
bool light3Switch=true;


//CONTROLADORES DE COLOR DE LUCES
float redLight=0.4f;
float greenLight=0.4f;
float blueLight=0.4f; 

//CONTROLADOR DE POSICION DE LUCES EN X
float xLightD =0.0f;


//DESPLAZADORES

float xDisplace =0.0f;
float zDisplace =0.0f;


//*--------------------------------------
//*-- DEFINICIÓN DE ESCENA .............
namespace Raytracer {


void Primitive::SetName( char* a_Name )
{
	//delete m_Name; // innecesario, pero garantiza que la referencia no tenga basura  
	m_Name = new char[strlen( a_Name ) + 1]; 
	strcpy( m_Name, a_Name ); 
}

// -----------------------------------------------------------
// Implementación de la clase Material 


Material::Material() :
	m_Color( Color( 0.2f, 0.2f, 0.2f ) ),
	m_Refl( 0 ), m_Diff( 0.2f )
{
}

// -----------------------------------------------------------
// Métodos para la primitiva Sphere

int Sphere::Intersect( Ray& a_Ray, float& a_Dist )
{
	vector3 v = a_Ray.GetOrigin() - m_Centre;
	
	float b = -DOT( v, a_Ray.GetDirection() );
	float det = (b * b) - DOT( v, v ) + m_SqRadius;
	int retval = MISS; // por defecto no intersecta 
	
	if (det > 0) //HA HABIDO INTERSECCION		
	{ // FALTA DETERMINAR SI EMPEZÓ DENTRO DE LA PRIMITIVA O FUE UN PRIMARY HIT
	
		det = sqrtf( det );
		float i1 = b - det;
		float i2 = b + det;
		if (i2 > 0)
		{
			if (i1 < 0) 
			{
				if (i2 < a_Dist) 
				{
					a_Dist = i2;
					retval = INPRIM; //INSIDE PRIMITIVE 
				}
			}
			else
			{
				if (i1 < a_Dist)
				{
					a_Dist = i1;
					retval = HIT; //hit primero 
				}
			}
		}
	}
	return retval;
}

// -----------------------------------------------------------
// Implementación de la primitiva plano 


int PlanePrim::Intersect( Ray& a_Ray, float& a_Dist )
{
	float d = DOT( m_Plane.N, a_Ray.GetDirection() );
	if (d != 0)
	{
		float dist = -(DOT( m_Plane.N, a_Ray.GetOrigin() ) + m_Plane.D) / d;
		if (dist > 0)
		{
			if (dist < a_Dist) 
			{
				a_Dist = dist;
				return HIT;
			}
		}
	}
	return MISS;
}

vector3 PlanePrim::GetNormal( vector3& a_Pos )
{
	return m_Plane.N;
}

////-----------------------------------------------
//
//// IMPLEMENTACIÓN DE LA PRIMITIVA BOX 
//
////-----------------------------------------------
//
//Box::Box() : 
//	m_Box( vector3( 0, 0, 0 ), vector3( 0, 0, 0 ) ), 
//	m_Grid( 0 ) 
//{
//	m_Material = new Material();	
//}
//
//Box::Box( aabb& a_Box ) : 
//	m_Box( a_Box ), 
//	m_Grid( 0 ) 
//{
//	m_Material = new Material();	
//}
//
//Box::~Box()
//{
//	delete m_Material;
//}



// -----------------------------------------------------------
// Implementación de la Clase Escena 

Scene::~Scene()
{
	delete m_Primitive;
}

void Scene::InitScene()
{
	m_Primitive = new Primitive*[100];
	//plano del piso 
	m_Primitive[0] = new PlanePrim( vector3( 0+xDisplace, 1, 0+zDisplace), 4.4f );
	m_Primitive[0]->SetName( "plane" );
	m_Primitive[0]->GetMaterial()->SetReflection( 0 );
	m_Primitive[0]->GetMaterial()->SetDiffuse( 1.0f );
	//m_Primitive[0]->GetMaterial()->SetColor( Color( 0.4f, 0.3f, 0.3f ) );
	m_Primitive[0]->GetMaterial()->SetColor( Color( 0.5f, 0.5f, 0.5f ) );
	
	// ESFERA PEQUEÑA
	m_Primitive[1] = new Sphere( vector3( 1+xDisplace, -0.8f, 3+zDisplace ), 2.5f );
	m_Primitive[1]->SetName( "SMALL sphere" );
	m_Primitive[1]->GetMaterial()->SetReflection( 0.6f );
	m_Primitive[1]->GetMaterial()->SetColor( Color( 0.6f, 0.6f, 0.6f ) );
	// ESFERA GRANDE
	m_Primitive[2] = new Sphere( vector3( -5.5f+xDisplace, -0.5, 7+zDisplace), 4 );
	m_Primitive[2]->SetName( "BIG sphere" );
	m_Primitive[2]->GetMaterial()->SetReflection( 1.0f );
	m_Primitive[2]->GetMaterial()->SetDiffuse( 0.1f );
	m_Primitive[2]->GetMaterial()->SetColor( Color( 0.6f, 0.6f, 0.6f ) );
	
	
	// fuente de luz 1
	m_Primitive[3] = new Sphere( vector3( -2+xDisplace+xLightD, 4, 5+zDisplace ), 1.0f );
	m_Primitive[3]->Light( light1Switch );
	m_Primitive[3]->GetMaterial()->SetColor( Color( redLight, greenLight, blueLight ) ); //luz 
	
	// fuente de luz 2	
	m_Primitive[4] = new Sphere( vector3( 2+xDisplace+xLightD, 5, 1+zDisplace ), 1.0f );
	m_Primitive[4]->Light( light2Switch);
	m_Primitive[4]->GetMaterial()->SetColor( Color( redLight, greenLight, blueLight ) ); //luz 
	
	// fuente de luz 3
	m_Primitive[5] = new Sphere( vector3( -2+xDisplace+xLightD, 5, 1+zDisplace ), 1.0f );
	m_Primitive[5]->Light( light3Switch );
	m_Primitive[5]->GetMaterial()->SetColor( Color(redLight, greenLight, blueLight ) ); // luz 
	
   

	// plano de atrás 

	m_Primitive[7] = new PlanePrim( vector3( 0.4f+xDisplace, 0, 1+zDisplace ), 12 );
	m_Primitive[7]->SetName( "back plane" );
	m_Primitive[7]->GetMaterial()->SetReflection( 0.0f );
	m_Primitive[7]->GetMaterial()->SetDiffuse( 0.6f );
	m_Primitive[7]->GetMaterial()->SetColor( Color( 0.5f, 0.8f, 0.5f ) );
	
	// plano superior

	m_Primitive[8] = new PlanePrim( vector3( 0.0f+xDisplace, -1, 0+zDisplace ), 7.4f );
	m_Primitive[8]->SetName( "ceiling plane" );
	m_Primitive[8]->GetMaterial()->SetReflection( 0.0f );
	m_Primitive[8]->GetMaterial()->SetDiffuse( 0.5f );
	m_Primitive[8]->GetMaterial()->SetColor( Color( 0.4f, 0.7f, 0.7f ) );


	//plano frontal 

	m_Primitive[6] = new PlanePrim( vector3( 0.4f+xDisplace, 0, -1+zDisplace), 12 );
	m_Primitive[6]->SetName( "back plane" );
	m_Primitive[6]->GetMaterial()->SetReflection( 0.0f );
	m_Primitive[6]->GetMaterial()->SetDiffuse( 0.6f );
	m_Primitive[6]->GetMaterial()->SetColor( Color( 0.5f, 0.3f, 0.0f ) );



	m_Primitives = 9; // VARIABLE DE CONTROL 
	
	
}

}; // end namespace Raytracer

//FIN DE DEFINICIÓN DE ESCENA 

#define SCRWIDTH	1200
#define SCRHEIGHT	1000

static WNDCLASS wc;
static HWND wnd;// el window handle 

static char bitmapbuffer[sizeof( BITMAPINFO ) + 16]; 
// el buffer de color debe guardar espacio para 32 

static BITMAPINFO* bh; // 2^32 colores en este bitmap 

HDC window_hdc;

Raytracer::Surface* surface = 0;

Pixel* buffer = 0;

Raytracer::Engine* tracer = 0;

void DrawWindow();

static LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	int result = 0, keycode = 0;
	switch (message)
    //dependiendo del mensaje, se hacen acciones distintas 
	{
	case WM_PAINT:
		
		if (!buffer) break; // sale con buffer vacío 
		StretchDIBits( window_hdc, 0, 0, SCRWIDTH, SCRHEIGHT, 0, 0, SCRWIDTH, SCRHEIGHT, buffer, bh, DIB_RGB_COLORS, SRCCOPY );
		ValidateRect( wnd, NULL );
		break;

		
	case WM_CHAR:
		{	
			

			// mensaje WM_CHAR, el WPARAM es el  "character code"
			// que fue presionado. WPARAM es equivalente al ASCII de la tecla
			
			
			TCHAR tchar = (TCHAR)wParam;// obtener la tecla presionada
			
			if(tchar == '1')
			{
			
				if(light1Switch){
			       light1Switch=false;
				   
				  tracer->GetScene()->InitScene();
				  tracer->SetTarget( surface->GetBuffer(), SCRWIDTH, SCRHEIGHT );
				  
				} break;
				
						       
			}


			if(tchar == 'q'||tchar=='Q')
			{
			
				if(!light1Switch){
			       light1Switch=true;
				   
				  tracer->GetScene()->InitScene();
				  tracer->SetTarget( surface->GetBuffer(), SCRWIDTH, SCRHEIGHT );
				  
				} break;
				
						       
			}


			if(tchar == '2')
			{
			
				if(light2Switch){
			       light2Switch=false;
				   
				  tracer->GetScene()->InitScene();
				  tracer->SetTarget( surface->GetBuffer(), SCRWIDTH, SCRHEIGHT );
				  
				} break;
				
						       
			}


			if(tchar == 'W'||tchar=='w')
			{
			
				if(!light2Switch){
			       light2Switch=true;
				   
				  tracer->GetScene()->InitScene();
				  tracer->SetTarget( surface->GetBuffer(), SCRWIDTH, SCRHEIGHT );
				  
				} break;
				
						       
			}


			if(tchar == '3')
			{
			
				if(light3Switch){
			       light3Switch=false;
				   
				  tracer->GetScene()->InitScene();
				  tracer->SetTarget( surface->GetBuffer(), SCRWIDTH, SCRHEIGHT );
				  
				} break;
				
						       
			}


			if(tchar == 'e'||tchar=='E')
			{
			
				if(!light3Switch){
			       light3Switch=true;
				   
				  tracer->GetScene()->InitScene();
				  tracer->SetTarget( surface->GetBuffer(), SCRWIDTH, SCRHEIGHT );
				  
				} break;
				
						       
			}

			if(tchar == 'j'||tchar=='J')
			{
			
				
			      xDisplace+=0.01f;
				   
				  tracer->GetScene()->InitScene();
				  tracer->SetTarget( surface->GetBuffer(), SCRWIDTH, SCRHEIGHT );
				  
				 break;
				
						       
			}

			if(tchar == 'l'||tchar=='L')
			{
			
				
			      xDisplace-=0.01f;
				   
				  tracer->GetScene()->InitScene();
				  tracer->SetTarget( surface->GetBuffer(), SCRWIDTH, SCRHEIGHT );
				  
				 break;
				
						       
			}


			if(tchar == 'k'||tchar=='K')
			{
			
				
			      zDisplace-=0.01f;
				   
				  tracer->GetScene()->InitScene();
				  tracer->SetTarget( surface->GetBuffer(), SCRWIDTH, SCRHEIGHT );
				  
				 break;
				
						       
			}

			if(tchar == 'i'||tchar=='I')
			{
			
				
			      zDisplace+=0.01f;
				   
				  tracer->GetScene()->InitScene();
				  tracer->SetTarget( surface->GetBuffer(), SCRWIDTH, SCRHEIGHT );
				  
				 break;
				
						       
			}


			if(tchar == 'r'||tchar=='R')
			{
			
				
			      redLight = 0.4f;
				  greenLight=0.0f;
				  blueLight=0.0f;
				   
				  tracer->GetScene()->InitScene();
				  tracer->SetTarget( surface->GetBuffer(), SCRWIDTH, SCRHEIGHT );
				  
				 break;
				
						       
			}


			if(tchar == 'g'||tchar=='G')
			{
			
				
			      redLight = 0.0f;
				  greenLight=0.3f;
				  blueLight=0.0f;
				   
				  tracer->GetScene()->InitScene();
				  tracer->SetTarget( surface->GetBuffer(), SCRWIDTH, SCRHEIGHT );
				  
				 break;
				
						       
			}

			if(tchar == 'b'||tchar=='B')
			{
			
				
			      redLight = 0.0f;
				  greenLight=0.0f;
				  blueLight=0.3f;
				   
				  tracer->GetScene()->InitScene();
				  tracer->SetTarget( surface->GetBuffer(), SCRWIDTH, SCRHEIGHT );
				  
				 break;
				
						       
			}

			if(tchar == 'z'||tchar=='Z')
			{
			
				
			      redLight = 0.5f;
				  greenLight=0.5f;
				  blueLight=0.5f;
				   
				  tracer->GetScene()->InitScene();
				  tracer->SetTarget( surface->GetBuffer(), SCRWIDTH, SCRHEIGHT );
				  
				 break;
				
						       
			}


			if(tchar == 'n'||tchar=='N')
			{
			
				
			      xLightD -= 0.04f; 
				   
				  tracer->GetScene()->InitScene();
				  tracer->SetTarget( surface->GetBuffer(), SCRWIDTH, SCRHEIGHT );
				  
				 break;
				
						       
			}

			if(tchar == 'm'||tchar=='M')
			{
			
				
			      xLightD += 0.04f; 
				   
				  tracer->GetScene()->InitScene();
				  tracer->SetTarget( surface->GetBuffer(), SCRWIDTH, SCRHEIGHT );
				  
				 break;
				
						       
			}

		} break;
				
	case WM_CLOSE:

		ReleaseDC( wnd, window_hdc );
		DestroyWindow( wnd );
		SystemParametersInfo( SPI_SETSCREENSAVEACTIVE, 1, 0, 0 );
		ExitProcess( 0 ); 
		break;
	default:
		result = DefWindowProc(hWnd,message,wParam,lParam);
	}
	return result;
}

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	RECT rect;
	int cc;
	wc.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = wc.cbWndExtra = 0;
	wc.hInstance = 0;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(0,IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "Raytracer Ver2";
	
	if (!RegisterClass(&wc)) return FALSE;
	
	rect.left = rect.top = 0;
	rect.right = SCRWIDTH, rect.bottom = SCRHEIGHT;
	AdjustWindowRect( &rect, WS_POPUP|WS_SYSMENU|WS_CAPTION, 0 );
	rect.right -= rect.left, rect.bottom -= rect.top;
	
	
	wnd = CreateWindowEx( NULL, // no atributos extras a la ventana 
		"Raytracer Ver2", //nombre interno para WNDCLASSEX
		"Raytracer Ver2", //texto para el título 
		WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME, // constantes de estilo de la ventana
		CW_USEDEFAULT, // usar el valor por defecto de posición para la esquina superior izquierda en X
		CW_USEDEFAULT, // usar el valor por defecto de posición para la esquina superior izquierda en y 
		SCRWIDTH,  // WIDTH de la ventana
		SCRHEIGHT, // HEIGHT de la ventana
		NULL, //handle a la ventana padre (no hay)
		NULL, //handle al menu (no hay)
		hInstance, // handle a la instancia de esta ventana, pasado por WinMain
		NULL ); // info extra a pasar a Win Proc (no hay)
	
	
	
	ShowWindow(wnd,SW_NORMAL); // mostrar la ventana (hacerla visible)
	for ( cc = 0; cc < sizeof( BITMAPINFOHEADER ) + 16; cc++ ) bitmapbuffer[cc] = 0;
	
	bh = (BITMAPINFO *)&bitmapbuffer;
	
	bh->bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	
	bh->bmiHeader.biPlanes = 1; // un solo plano de bitmap
	
	bh->bmiHeader.biBitCount = 32;
	//El miembro biBitCount de la estructura BITMAPINFOHEADER 
	//determina el número de bits que definen cada pixel y el máximo número de colores en el mapa de bits.
	
	bh->bmiHeader.biCompression = BI_BITFIELDS;
	bh->bmiHeader.biWidth = SCRWIDTH, bh->bmiHeader.biHeight = -SCRHEIGHT;

	((unsigned long*)bh->bmiColors)[0] = 255 << 16;
	((unsigned long*)bh->bmiColors)[1] = 255 << 8;
	((unsigned long*)bh->bmiColors)[2] = 255;

	window_hdc = GetDC(wnd);
    //DC = "Device Context" 
	//GetDC obtiene el handle del device context 
	//el device context aquí es la ventana de dibujo, está obteniendo el handle de la ventana de dibujo

	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 0, 0, 0);
	
	
	// prepara el canvas de output
	
	surface = new Raytracer::Surface( SCRWIDTH, SCRHEIGHT );
	buffer = surface->GetBuffer();
	surface->Clear( 0 );
	surface->InitCharset();

	
	// prepara el renderer 
	tracer = new Raytracer::Engine();
	tracer->GetScene()->InitScene();
	tracer->SetTarget( surface->GetBuffer(), SCRWIDTH, SCRHEIGHT );
	
	
	while (1)
	{
		
		tracer->InitRender();//debe ejecutarse en ciclo
		
		while (!tracer->Render()) DrawWindow(); 
		// DrawWindow() maneja el paso de mensajes (key presses) con la variable MSG  
		
		
	}
	return 1;
}

void DrawWindow()
{
	MSG message; // un mensaje puede ser un key press 
	HACCEL haccel = 0;
	InvalidateRect( wnd,NULL,TRUE );
	SendMessage( wnd, WM_PAINT, 0, 0 );

	// en el ejemplo de GameTutorials esto estaba en WinMain, aquí llamamos a DrawWindow desde 
	// WinMain 
	while (PeekMessage( &message, wnd, 0, 0, PM_REMOVE ))
	{
		if (TranslateAccelerator( wnd, haccel, &message ) == 0)
		{
			TranslateMessage( &message );
			DispatchMessage( &message );
		}
	}
	Sleep( 0 );
}
