
#ifndef I_RAYTRACER_H
#define I_RAYTRACER_H

#include "common.h"

namespace Raytracer {

// -----------------------------------------------------------
// Definición clase Rayo 

class Ray
{
public:
	Ray() : origen_( vector3( 0, 0, 0 ) ), m_Direction( vector3( 0, 0, 0 ) ) {};
	Ray( vector3& a_Origin, vector3& a_Dir );
	void SetOrigin( vector3& a_Origin ) { origen_ = a_Origin; }
	void SetDirection( vector3& a_Direction ) { m_Direction = a_Direction; }
	vector3& GetOrigin() { return origen_; }
	vector3& GetDirection() { return m_Direction; }
private:
	vector3 origen_;
	vector3 m_Direction;
};

// -----------------------------------------------------------
// Definición de la clase Engine
// Core del raytracer 
// -----------------------------------------------------------
class Primitive; // implementada en DEPLOY.CPP

class Scene; //implementada en DEPLOY.CPP

class Engine
{
public:

	Engine();
	~Engine();
	void SetTarget( Pixel* a_Dest, int a_Width, int a_Height );
	Scene* GetScene() { return m_Scene; }
	Primitive* Raytrace( Ray& a_Ray, Color& a_Acc, int a_Depth, float a_RIndex, float& a_Dist );
	void InitRender();
	bool Render();

protected:
	// data del renderer 

	float m_WX1, m_WY1, m_WX2, m_WY2, m_DX, m_DY, m_SX, m_SY;
	Scene* m_Scene;
	Pixel* m_Dest;
	int m_Width, m_Height, m_CurrLine, m_PPos;
	Primitive** m_LastRow;
};

}; // namespace Raytracer

#endif