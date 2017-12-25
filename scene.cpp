

#include "common.h"
#include "string.h"
#include "scene.h"
#include "raytracer.h"

namespace Raytracer {


void Primitive::SetName( char* a_Name )
{
	delete m_Name; // innecesario, pero garantiza que la referencia no tenga basura  
	m_Name = new char[strlen( a_Name ) + 1]; 
	strcpy( m_Name, a_Name ); 
}

// -----------------------------------------------------------
// Implementación de la clase Material 
// -----------------------------------------------------------

Material::Material() :
	m_Color( Color( 0.2f, 0.2f, 0.2f ) ),
	m_Refl( 0 ), m_Diff( 0.2f )
{
}

// -----------------------------------------------------------
// Métodos para la primitiva Sphere
// -----------------------------------------------------------

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
// -----------------------------------------------------------

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

// -----------------------------------------------------------
// Implementación de la Clase Escena 
// -----------------------------------------------------------

Scene::~Scene()
{
	delete m_Primitive;
}

void Scene::InitScene()
{
	m_Primitive = new Primitive*[100];
	//plano del piso 
	m_Primitive[0] = new PlanePrim( vector3( 0, 1, 0 ), 4.4f );
	m_Primitive[0]->SetName( "plane" );
	m_Primitive[0]->GetMaterial()->SetReflection( 0 );
	m_Primitive[0]->GetMaterial()->SetDiffuse( 1.0f );
	m_Primitive[0]->GetMaterial()->SetColor( Color( 0.4f, 0.3f, 0.3f ) );
	// ESFERA PEQUEÑA
	m_Primitive[1] = new Sphere( vector3( 1, -0.8f, 3 ), 2.5f );
	m_Primitive[1]->SetName( "SMALL sphere" );
	m_Primitive[1]->GetMaterial()->SetReflection( 0.6f );
	m_Primitive[1]->GetMaterial()->SetColor( Color( 0.7f, 0.7f, 0.7f ) );
	// ESFERA GRANDE
	m_Primitive[2] = new Sphere( vector3( -5.5f, -0.5, 7 ), 4 );
	m_Primitive[2]->SetName( "BIG sphere" );
	m_Primitive[2]->GetMaterial()->SetReflection( 1.0f );
	m_Primitive[2]->GetMaterial()->SetDiffuse( 0.1f );
	m_Primitive[2]->GetMaterial()->SetColor( Color( 0.7f, 0.7f, 1.0f ) );
	
	
	// fuente de luz 1
	m_Primitive[3] = new Sphere( vector3( -2, 5, 5 ), 0.1f );
	m_Primitive[3]->Light( true );
	m_Primitive[3]->GetMaterial()->SetColor( Color( 0.4f, 0.0f, 0.0f ) ); //luz roja 
	
	// fuente de luz 2	
	m_Primitive[4] = new Sphere( vector3( 2, 5, 1 ), 0.1f );
	m_Primitive[4]->Light( false );
	m_Primitive[4]->GetMaterial()->SetColor( Color( 0.4f, 0.0f, 0.0f ) ); //luz roja
	
	// fuente de luz 3
	m_Primitive[5] = new Sphere( vector3( -2, 5, 1 ), 0.1f );
	m_Primitive[5]->Light( false );
	m_Primitive[5]->GetMaterial()->SetColor( Color( 0.4f, 0.0f, 0.0f ) ); // luz roja
	
    //fuente de luz 4
	m_Primitive[6] = new Sphere( vector3( 5, 5, 5 ), 0.1f );
	m_Primitive[6]->Light( true );
	m_Primitive[6]->GetMaterial()->SetColor( Color( 0.4f, 0.0f, 0.0f ) ); // luz roja 


	// plano de atrás 

	m_Primitive[7] = new PlanePrim( vector3( 0.4f, 0, 1 ), 12 );
	m_Primitive[7]->SetName( "back plane" );
	m_Primitive[7]->GetMaterial()->SetReflection( 0.0f );
	m_Primitive[7]->GetMaterial()->SetDiffuse( 0.6f );
	m_Primitive[7]->GetMaterial()->SetColor( Color( 0.5f, 0.3f, 0.5f ) );
	
	// plano superior

	m_Primitive[8] = new PlanePrim( vector3( 0, -1, 0 ), 7.4f );
	m_Primitive[8]->SetName( "ceiling plane" );
	m_Primitive[8]->GetMaterial()->SetReflection( 0.0f );
	m_Primitive[8]->GetMaterial()->SetDiffuse( 0.5f );
	m_Primitive[8]->GetMaterial()->SetColor( Color( 0.4f, 0.7f, 0.7f ) );


	//plano frontal 

	m_Primitive[6] = new PlanePrim( vector3( 0.4f, 0, -1 ), 12 );
	m_Primitive[6]->SetName( "back plane" );
	m_Primitive[6]->GetMaterial()->SetReflection( 0.0f );
	m_Primitive[6]->GetMaterial()->SetDiffuse( 0.6f );
	m_Primitive[6]->GetMaterial()->SetColor( Color( 0.5f, 0.3f, 0.5f ) );



	m_Primitives = 9; // VARIABLE DE CONTROL 
	
	
}

}; // end namespace Raytracer
