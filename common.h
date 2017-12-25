

#ifndef I_COMMON_H
#define I_COMMON_H

#include "math.h"
#include "stdlib.h"

typedef unsigned int Pixel; //32 bits de color por Píxel 

//float aleatorio dentro de Rango
inline float Rand( float a_Range ) { return ((float)rand() / RAND_MAX) * a_Range; }

namespace Raytracer {

#define DOT(A,B)		(A.x*B.x+A.y*B.y+A.z*B.z)
#define NORMALIZE(A)	{float l=1/sqrtf(A.x*A.x+A.y*A.y+A.z*A.z);A.x*=l;A.y*=l;A.z*=l;}
#define LENGTH(A)		(sqrtf(A.x*A.x+A.y*A.y+A.z*A.z))
#define SQRLENGTH(A)	(A.x*A.x+A.y*A.y+A.z*A.z)
#define SQRDISTANCE(A,B) ((A.x-B.x)*(A.x-B.x)+(A.y-B.y)*(A.y-B.y)+(A.z-B.z)*(A.z-B.z))

#define EPSILON			0.001f // constante de muestreo para el rayo, reducirlo (en orden de 10)
	                             // trae más artefactos
#define TRACEDEPTH		12 //tope de recursión por rayo  

//efecto importante: cambiar el tope de recursión limita el número de reflexiones entre las esferas
//con TRACEDEPTH=2 hay sólo un reflejo interesfera
//con TRACEDEPTH=14 son visibles hasta 4 

#define PI				3.141592653589793238462f
#define EPSILON			0.001f // constante de muestreo para el rayo, reducirlo (en orden de 10)
	                             // trae más artefactos

class vector3
{
public:

	// el vector se inicializa en el origen en el constructor por defecto
	vector3() : x( 0.0f ), y( 0.0f ), z( 0.0f ) {};

	vector3( float a_X, float a_Y, float a_Z ) : x( a_X ), y( a_Y ), z( a_Z ) {};
	
	void Set( float a_X, float a_Y, float a_Z ) { x = a_X; y = a_Y; z = a_Z; }
	
	void Normalize() { float l = 1.0f / Length(); x *= l; y *= l; z *= l; }
	
	float Length() { return (float)sqrt( x * x + y * y + z * z ); }
	
	float SqrLength() { return x * x + y * y + z * z; }
	
	float Dot( vector3 a_V ) { return x * a_V.x + y * a_V.y + z * a_V.z; }
	
	vector3 Cross( vector3 b ) { return vector3( y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x ); }
	
	void operator += ( vector3& a_V ) { x += a_V.x; y += a_V.y; z += a_V.z; }
	
	void operator += ( vector3* a_V ) { x += a_V->x; y += a_V->y; z += a_V->z; }
	
	void operator -= ( vector3& a_V ) { x -= a_V.x; y -= a_V.y; z -= a_V.z; }
	
	void operator -= ( vector3* a_V ) { x -= a_V->x; y -= a_V->y; z -= a_V->z; }
	
	void operator *= ( float f ) { x *= f; y *= f; z *= f; }
	
	void operator *= ( vector3& a_V ) { x *= a_V.x; y *= a_V.y; z *= a_V.z; }
	
	void operator *= ( vector3* a_V ) { x *= a_V->x; y *= a_V->y; z *= a_V->z; }
	
	vector3 operator- () const { return vector3( -x, -y, -z ); }
	
	//declaradas como friend para permitir el acceso a la data privada 
	friend vector3 operator + ( const vector3& v1, const vector3& v2 ) { return vector3( v1.x + v2.x, v1.y + v2.y, v1.z + v2.z ); }
	
	friend vector3 operator - ( const vector3& v1, const vector3& v2 ) { return vector3( v1.x - v2.x, v1.y - v2.y, v1.z - v2.z ); }
	
	friend vector3 operator + ( const vector3& v1, vector3* v2 ) { return vector3( v1.x + v2->x, v1.y + v2->y, v1.z + v2->z ); }
	
	friend vector3 operator - ( const vector3& v1, vector3* v2 ) { return vector3( v1.x - v2->x, v1.y - v2->y, v1.z - v2->z ); }
	
	friend vector3 operator * ( const vector3& v, float f ) { return vector3( v.x * f, v.y * f, v.z * f ); }
	
	friend vector3 operator * ( const vector3& v1, vector3& v2 ) { return vector3( v1.x * v2.x, v1.y * v2.y, v1.z * v2.z ); }
	
	friend vector3 operator * ( float f, const vector3& v ) { return vector3( v.x * f, v.y * f, v.z * f ); }
	
	union
	{
		struct { float x, y, z; };
		struct { float r, g, b; };
		struct { float cell[3]; };
	};
};

class aabb // axis aligned bounding box- no usado 
{
public:
	aabb() : m_Pos( vector3( 0, 0, 0 ) ), m_Size( vector3( 0, 0, 0 ) ) {};
	aabb( vector3& a_Pos, vector3& a_Size ) : m_Pos( a_Pos ), m_Size( a_Size ) {};
	vector3& GetPos() { return m_Pos; }
	vector3& GetSize() { return m_Size; }
	bool Intersect( aabb& b2 )
	{
		vector3 v1 = b2.GetPos(), v2 = b2.GetPos() + b2.GetSize();
		vector3 v3 = m_Pos, v4 = m_Pos + m_Size;
		return ((v4.x > v1.x) && (v3.x < v2.x) && // solapamiento eje x
				(v4.y > v1.y) && (v3.y < v2.y) && // solapamiento eje y
				(v4.z > v1.z) && (v3.z < v2.z));   //solapamiento eje z 
	}
	bool Contains( vector3 a_Pos )
	{
		vector3 v1 = m_Pos, v2 = m_Pos + m_Size;
		return ((a_Pos.x > (v1.x - EPSILON)) && (a_Pos.x < (v2.x + EPSILON)) &&
				(a_Pos.y > (v1.y - EPSILON)) && (a_Pos.y < (v2.y + EPSILON)) &&
				(a_Pos.z > (v1.z - EPSILON)) && (a_Pos.z < (v2.z + EPSILON)));
	}
private:
	vector3 m_Pos, m_Size;
};


class plane
{
public:

	//el constructor por defecto inicializa la normal de plano como el origen 
	plane() : N( 0, 0, 0 ), D( 0 ) {};
	plane( vector3 a_Normal, float a_D ) : N( a_Normal ), D( a_D ) {};
	union
	{
		struct
		{
			vector3 N;
			float D;
		};
		float cell[4];
	};
};

typedef vector3 Color;

}; // fin namespace Raytracer

#endif