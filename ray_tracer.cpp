

#include "raytracer.h"
#include "scene.h"
#include "common.h"
#include "windows.h"
#include "winbase.h"

namespace Raytracer {

Ray::Ray( vector3& a_Origin, vector3& a_Dir ) : 
	origen_( a_Origin ), 
	m_Direction( a_Dir )
{
}

Engine::Engine()
{
	m_Scene = new Scene();
}

Engine::~Engine()
{
	delete m_Scene;
}

// -----------------------------------------------------------
// Engine::SetTarget

// Fija el canvas de render destino a_Dest y su ancho y alto 

void Engine::SetTarget( Pixel* a_Dest, int a_Width, int a_Height )
{
	// set fijar la dirección y tamaño del pixel buffer 
	m_Dest = a_Dest;
	m_Width = a_Width;
	m_Height = a_Height;
}



// -----------------------------------------------------------
// Engine::Raytrace
// Intersecta el rayo con cada primitiva 
// en la escena para determinar la intersección más cercana 
Primitive* Engine::Raytrace( Ray& a_Ray, Color& a_Acc, int a_Depth, float a_RIndex, float& a_Dist )
{
	if (a_Depth > TRACEDEPTH) return 0;
	// traczar rayo primario 
	a_Dist = 1000000.0f;
	vector3 pi;
	Primitive* prim = 0;
	int result;
	// encontrar la intersección más cercana 
	for ( int s = 0; s < m_Scene->GetNrPrimitives(); s++ )
	{
		Primitive* pr = m_Scene->GetPrimitive( s );
		int res;
		if (res = pr->Intersect( a_Ray, a_Dist )) 
		{
			prim = pr;
			result = res; // 0 = fallo, 1 = hit, -1 = hit desde adentro de la primitiva
		}
	}
	// si no hubo hit a ninguna primitiva, terminar el rayo 
	if (!prim) return 0;
	// manejar intersección
	if (prim->IsLight())
	{
		// caso de parada, se llegó a una luz, dejar trazar 
		a_Acc = Color( 1, 1, 1 );
	}
	else
	{
		// determinar el color en el punto de intersección
		pi = a_Ray.GetOrigin() + a_Ray.GetDirection() * a_Dist;
		// trazar luces
		for ( int l = 0; l < m_Scene->GetNrPrimitives(); l++ )
		{
			Primitive* p = m_Scene->GetPrimitive( l );
			if (p->IsLight()) 
			{
				Primitive* light = p;
				//manejar point light 
				
							
				//shade se inicializa por defecto en 1 
				//shade = 1 representa acceso del rayo a la fuente de luz
                //shade = 0 representa la luz ocluida  
				float shade = 1.0f;
				if (light->GetType() == Primitive::SPHERE)
				{
					vector3 L = ((Sphere*)light)->GetCentre() - pi;
					float tdist = LENGTH( L );
					L *= (1.0f / tdist);
					Ray r = Ray( pi + L * EPSILON, L );
					for ( int s = 0; s < m_Scene->GetNrPrimitives(); s++ )
					{
						Primitive* pr = m_Scene->GetPrimitive( s );
						if ((pr != light) && (pr->Intersect( r, tdist )))
						{
							shade = 0; //la luz es ocluida 
							break;
						}
					}
				}
				// determinar componente difuso 
				vector3 L = ((Sphere*)light)->GetCentre() - pi;
				NORMALIZE( L );
				vector3 N = prim->GetNormal( pi );
				if (prim->GetMaterial()->GetDiffuse() > 0)
				{
					float dot = DOT( L, N );
					if (dot > 0)
					{
						float diff = dot * prim->GetMaterial()->GetDiffuse() * shade;
						// añadir el componente difuso al color del rayo 
						a_Acc += diff * light->GetMaterial()->GetColor() * prim->GetMaterial()->GetColor();
					}
				}
				// determinar el componente especular 
				if (prim->GetMaterial()->GetSpecular() > 0)
				{
					// fuente de luz puntual, productos phong 
					vector3 V = a_Ray.GetDirection();
					vector3 R = L - 2.0f * DOT( L, N ) * N;
					float dot = DOT( V, R );
					if (dot > 0)
					{
						float spec = powf( dot, 20 ) * prim->GetMaterial()->GetSpecular() * shade;
						// añadir componente especular al color del rayo 
						a_Acc += spec * light->GetMaterial()->GetColor();
					}
				}
			}
		}
		// calcular reflección 
		float refl = prim->GetMaterial()->GetReflection();
		if (refl > 0.0f)
		{
			vector3 N = prim->GetNormal( pi );
			vector3 R = a_Ray.GetDirection() - 2.0f * DOT( a_Ray.GetDirection(), N ) * N;
			if (a_Depth < TRACEDEPTH) 
			{
				Color rcol( 0, 0, 0 );
				float dist;
				Raytrace( Ray( pi + R * EPSILON, R ), rcol, a_Depth + 1, a_RIndex, dist );
				a_Acc += refl * rcol * prim->GetMaterial()->GetColor();
			}
		}
	}
	// return apuntador a primitiva golpeada por rayo primario 
	return prim;
}

// -----------------------------------------------------------
// Engine::InitRender
// Inicializa el renderer, reseteando los valores de línea / cuadro 
// y precalculando los deltas de interpolación 

void Engine::InitRender()
{
	// fija la primera línea en que dibujar
	m_CurrLine = 20;
	// fija la dirección del primer píxel en el píxel buffer 
	m_PPos = 20 * m_Width;
	// plano de pnatalla en coordenadas de mundo screen 
	m_WX1 = -4, m_WX2 = 4, m_WY1 = m_SY = 3, m_WY2 = -3;
	// calcular los deltas de interpolación
	m_DX = (m_WX2 - m_WX1) / m_Width;
	m_DY = (m_WY2 - m_WY1) / m_Height;
	m_SY += 20 * m_DY;
	// asignar espacio para guardar apuntadores a las primitivas para la línea previa 
	m_LastRow = new Primitive*[m_Width];
	memset( m_LastRow, 0, m_Width * 4 );
}



//Engine::Render
// manda rayos a la escena de scanline a scanline, 
// barriendo de izquierda a dercha

bool Engine::Render()
{
	// render scene
	vector3 o( 0, 0, -5 );
	// inicializa el timer
	int msecs = GetTickCount();
	// reset al apuntador de la última primitiva encontrada 
	Primitive* lastprim = 0;
	// render a las líneas remanentes
	for ( int y = m_CurrLine; y < (m_Height - 20); y++ )
	{
		m_SX = m_WX1;
		// render a los píxeles para la línea actual 
		for ( int x = 0; x < m_Width; x++ )
		{
			// disparar el rayo primario 
			Color acc( 0, 0, 0 );
			vector3 dir = vector3( m_SX, m_SY, 0 ) - o;
			NORMALIZE( dir );
			Ray r( o, dir );
			float dist;
			Primitive* prim = Raytrace( r, acc, 1, 1.0f, dist );
			int red = (int)(acc.r * 256);
			int green = (int)(acc.g * 256);
			int blue = (int)(acc.b * 256);
			if (red > 255) red = 255;
			if (green > 255) green = 255;
			if (blue > 255) blue = 255;
			m_Dest[m_PPos++] = (red << 16) + (green << 8) + blue;
			m_SX += m_DX;
		}
		m_SY += m_DY;
		// si ha disparado rayos por más de 100 msecs se retorna control 
		// para que la ventana se actualice 
		if ((GetTickCount() - msecs) > 100) 
		{
			// retorna control a la ventana para que la pantalla se actualice 
			m_CurrLine = y + 1;
			return false;
		}
	}
	// listo 
	return true;
}

}; // namespace Raytracer