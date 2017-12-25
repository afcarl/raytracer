

#ifndef I_SCENE_H
#define I_SCENE_H

#include "raytracer.h"

namespace Raytracer {

// retornos para el método de intersección
#define HIT		 1		// 1 cuando el rayo toca la primitiva
#define MISS	 0		// 0 cuando el rayo falla en tocar la primitiva
#define INPRIM	-1		// -1 cuando el rayo empezó su ejecución dentro de la primitiva


//definición de la clase de materiales
class Material
{
public:
	Material();
	void SetColor( Color& a_Color ) { m_Color = a_Color; }
	Color GetColor() { return m_Color; }
	void SetDiffuse( float a_Diff ) { m_Diff = a_Diff; }
	void SetReflection( float a_Refl ) { m_Refl = a_Refl; }
	float GetSpecular() { return 1.0f - m_Diff; }
	float GetDiffuse() { return m_Diff; }
	float GetReflection() { return m_Refl; }
private:
	Color m_Color;
	float m_Refl;
	float m_Diff;
};

// -----------------------------------------------------------
// Definición de la clase Primitiva 

class Primitive
{
public:
	enum
	{
		SPHERE = 1,
		PLANE,
		AABB, // axis aligned bouding box
		TRIANGLE 
	};
	Primitive() : m_Name( 0 ), m_Light( false ) {};
	Material* GetMaterial() { return &m_Material; }
	void SetMaterial( Material& a_Mat ) { m_Material = a_Mat; }
	virtual int GetType() = 0; // virtuales, tendrán cuerpo distinto en cada implementación de Primitive
	virtual int Intersect( Ray& a_Ray, float& a_Dist ) = 0;
	virtual vector3 GetNormal( vector3& a_Pos ) = 0;
	virtual Color GetColor( vector3& ) { return m_Material.GetColor(); }
	virtual void Light( bool a_Light ) { m_Light = a_Light; }
	bool IsLight() { return m_Light; }
	void SetName( char* a_Name );
	char* GetName() { return m_Name; }

protected:

	Material m_Material;

	char* m_Name;

	bool m_Light;
};

// -----------------------------------------------------------
// Definición de Primitiva Sphere


class Sphere : public Primitive
{
public:
	int GetType() { return SPHERE; }
	Sphere( vector3& a_Centre, float a_Radius ) : 
		m_Centre( a_Centre ), m_SqRadius( a_Radius * a_Radius ), 
		m_Radius( a_Radius ), m_RRadius( 1.0f / a_Radius ) {};
	vector3& GetCentre() { return m_Centre; }
	float GetSqRadius() { return m_SqRadius; }
	int Intersect( Ray& a_Ray, float& a_Dist );
	vector3 GetNormal( vector3& a_Pos ) { return (a_Pos - m_Centre) * m_RRadius; }
private:
	vector3 m_Centre;
	float m_SqRadius, m_Radius, m_RRadius;
};

// -----------------------------------------------------------
// Definición de la clase PlanePrim


class PlanePrim : public Primitive
{
public:
	int GetType() { return PLANE; }
	PlanePrim( vector3& a_Normal, float a_D ) : m_Plane( plane( a_Normal, a_D ) ) {};
	vector3& GetNormal() { return m_Plane.N; }
	float GetD() { return m_Plane.D; }
	int Intersect( Ray& a_Ray, float& a_Dist );
	vector3 GetNormal( vector3& a_Pos );
private:
	plane m_Plane;
};

//--------------------------
//Definición de la clase Box


class Box : public Primitive
{
public:
	int GetType() { return AABB; }
	Box();
	Box( aabb& a_Box );
	~Box();
	int Intersect( Ray& a_Ray, float& a_Dist );
	bool IntersectBox( aabb& a_Box ) { return m_Box.Intersect( a_Box ); }
	vector3 GetNormal( vector3& );
	bool Contains( vector3& a_Pos ) { return m_Box.Contains( a_Pos ); }
	vector3& GetPos() { return m_Box.GetPos(); }
	vector3& GetSize() { return m_Box.GetSize(); }
	float GetGridX( int a_Idx ) { return m_Grid[a_Idx << 1]; }
	float GetGridY( int a_Idx ) { return m_Grid[(a_Idx << 1) + 1]; }
	void Light( bool a_Light );
	aabb GetAABB() { return m_Box; }
protected:
	aabb m_Box;
	float* m_Grid;
};



// -----------------------------------------------------------
// Definición de la clase scene 

class Scene
{
public:
	Scene() : m_Primitives( 0 ), m_Primitive( 0 ) {};
	~Scene();
	void InitScene();
	int GetNrPrimitives() { return m_Primitives; }
	Primitive* GetPrimitive( int a_Idx ) { return m_Primitive[a_Idx]; }
private:
	int m_Primitives;
	Primitive** m_Primitive;
};

}; // namespace Raytracer

#endif