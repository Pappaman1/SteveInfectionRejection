#include "MyCamera.h"
#include <math.h>
using namespace Simplex;

//Accessors
void Simplex::MyCamera::SetPosition(vector3 a_v3Position) { m_v3Position = a_v3Position; }

void Simplex::MyCamera::SetTarget(vector3 a_v3Target) { m_v3Target = a_v3Target; }

void Simplex::MyCamera::SetUp(vector3 a_v3Up) { m_v3Up = a_v3Up; }

void Simplex::MyCamera::SetPerspective(bool a_bPerspective) { m_bPerspective = a_bPerspective; }

void Simplex::MyCamera::SetFOV(float a_fFOV) { m_fFOV = a_fFOV; }

void Simplex::MyCamera::SetResolution(vector2 a_v2Resolution) { m_v2Resolution = a_v2Resolution; }

void Simplex::MyCamera::SetNearFar(vector2 a_v2NearFar) { m_v2NearFar = a_v2NearFar; }

void Simplex::MyCamera::SetHorizontalPlanes(vector2 a_v2Horizontal) { m_v2Horizontal = a_v2Horizontal; }

void Simplex::MyCamera::SetVerticalPlanes(vector2 a_v2Vertical) { m_v2Vertical = a_v2Vertical; }

matrix4 Simplex::MyCamera::GetProjectionMatrix(void) { return m_m4Projection; }

matrix4 Simplex::MyCamera::GetViewMatrix(void) { CalculateViewMatrix(); return m_m4View; }

Simplex::MyCamera::MyCamera()
{
	Init(); //Init the object with default values
}

Simplex::MyCamera::MyCamera(vector3 a_v3Position, vector3 a_v3Target, vector3 a_v3Upward)
{
	Init(); //Initialize the object
	SetPositionTargetAndUp(a_v3Position, a_v3Target, a_v3Upward); //set the position, target and up
}

Simplex::MyCamera::MyCamera(MyCamera const& other)
{
	m_v3Position = other.m_v3Position;
	m_v3Target = other.m_v3Target;
	m_v3Up = other.m_v3Up;

	m_bPerspective = other.m_bPerspective;

	m_fFOV = other.m_fFOV;

	m_v2Resolution = other.m_v2Resolution;
	m_v2NearFar = other.m_v2NearFar;

	m_v2Horizontal = other.m_v2Horizontal;
	m_v2Vertical = other.m_v2Vertical;

	m_m4View = other.m_m4View;
	m_m4Projection = other.m_m4Projection;
}

MyCamera& Simplex::MyCamera::operator=(MyCamera const& other)
{
	if (this != &other)
	{
		Release();
		SetPositionTargetAndUp(other.m_v3Position, other.m_v3Target, other.m_v3Up);
		MyCamera temp(other);
		Swap(temp);
	}
	return *this;
}

void Simplex::MyCamera::Init(void)
{
	ResetCamera();
	CalculateProjectionMatrix();
	CalculateViewMatrix();
	//No pointers to initialize here
}

void Simplex::MyCamera::Release(void)
{
	//No pointers to deallocate yet
}

void Simplex::MyCamera::MoveForward(float a_fDistance)
{
vector3 forward = glm::normalize(QuatConversion(m_v3Target) - m_v3Position);
m_v3Position += forward * a_fDistance;
m_v3Target += forward * a_fDistance;
//m_v3Up = glm::normalize(m_v3Up + forward * a_fDistance);

CalculateProjectionMatrix();
CalculateViewMatrix();

printf(" %f %f %f \n", m_v3Target.x, m_v3Target.y, m_v3Target.z);
//m_v3Forward = glm::normalize(m_v3Target - m_v3Position);
//m_v3Upward = glm::normalize(m_v3Top - m_v3Position);
//m_v3Rightward = glm::normalize(glm::cross(m_v3Forward, m_v3Upward));
}

void Simplex::MyCamera::MoveVertical(float a_fDistance)
{
	vector3 up = glm::normalize(m_v3Up);
	m_v3Position += (up * a_fDistance);
	m_v3Target += (up * a_fDistance);
	m_v3Up = glm::normalize(m_v3Up + up * a_fDistance);

	CalculateProjectionMatrix();
	CalculateViewMatrix();
	//m_v3Forward = glm::normalize(m_v3Target - m_v3Position);
	//m_v3Upward = glm::normalize(m_v3Top - m_v3Position);
	//m_v3Rightward = glm::normalize(glm::cross(m_v3Forward, m_v3Upward));
}

void Simplex::MyCamera::MoveSideways(float a_fDistance)
{
	vector3 forward = glm::normalize(m_v3Target - m_v3Position);
	vector3 up = glm::normalize(m_v3Up);
	vector3 right = glm::normalize(glm::cross(forward, up));
	m_v3Position += right * a_fDistance;
	m_v3Target += right * a_fDistance;
	m_v3Up += glm::normalize(m_v3Up - right * a_fDistance);


	CalculateProjectionMatrix();
	CalculateViewMatrix();
	printf(" %f %f %f \n", m_v3Target.x, m_v3Target.y, m_v3Target.z);
	//m_v3Forward = glm::normalize(m_v3Target - m_v3Position);
	//m_v3Upward = glm::normalize(m_v3Top - m_v3Position);
	//m_v3Rightward = glm::normalize(glm::cross(m_v3Forward, m_v3Upward));
}

void Simplex::MyCamera::Swap(MyCamera & other)
{
	std::swap(m_v3Position, other.m_v3Position);
	std::swap(m_v3Target, other.m_v3Target);
	std::swap(m_v3Up, other.m_v3Up);

	std::swap(m_bPerspective, other.m_bPerspective);

	std::swap(m_fFOV, other.m_fFOV);

	std::swap(m_v2Resolution, other.m_v2Resolution);
	std::swap(m_v2NearFar, other.m_v2NearFar);

	std::swap(m_v2Horizontal, other.m_v2Horizontal);
	std::swap(m_v2Vertical, other.m_v2Vertical);

	std::swap(m_m4View, other.m_m4View);
	std::swap(m_m4Projection, other.m_m4Projection);
}

Simplex::MyCamera::~MyCamera(void)
{
	Release();
}

void Simplex::MyCamera::ResetCamera(void)
{
	m_v3Position = vector3(0.0f, 0.0f, 10.0f); //Where my camera is located
	m_v3Target = vector3(0.0f, 0.0f, 0.0f); //What I'm looking at
	m_v3Up = vector3(0.0f, 1.0f, 0.0f); //What is up

	m_bPerspective = true; //perspective view? False is Orthographic

	m_fFOV = 45.0f; //Field of View

	m_v2Resolution = vector2(1280.0f, 720.0f); //Resolution of the window
	m_v2NearFar = vector2(0.001f, 1000.0f); //Near and Far planes

	m_v2Horizontal = vector2(-5.0f, 5.0f); //Ortographic horizontal projection
	m_v2Vertical = vector2(-5.0f, 5.0f); //Ortographic vertical projection

	CalculateProjectionMatrix();
	CalculateViewMatrix();
}

void Simplex::MyCamera::SetPositionTargetAndUp(vector3 a_v3Position, vector3 a_v3Target, vector3 a_v3Upward)
{
	m_v3Position = a_v3Position;
	m_v3Target = a_v3Target;
	m_v3Up = a_v3Upward;
	CalculateProjectionMatrix();
}

void Simplex::MyCamera::CalculateViewMatrix(void)
{
	//Calculate the look at
	m_m4View = glm::lookAt(m_v3Position, QuatConversion(m_v3Target), glm::normalize(m_v3Up));

}

void Simplex::MyCamera::CalculateProjectionMatrix(void)
{
	//perspective
	float fRatio = m_v2Resolution.x / m_v2Resolution.y;
	if (m_bPerspective)
	{
		m_m4Projection = glm::perspective(m_fFOV, fRatio, m_v2NearFar.x, m_v2NearFar.y);
	}
	else
	{
		m_m4Projection = glm::ortho(	m_v2Horizontal.x * fRatio, m_v2Horizontal.y * fRatio, //horizontal
										m_v2Vertical.x, m_v2Vertical.y, //vertical
										m_v2NearFar.x, m_v2NearFar.y); //near and far
	}
}

void Simplex::MyCamera::ChangeYawAndPitch(float yawAngle, float pitchAngle) //changes yaw and pitch in single function
{
//	rotQuat = glm::quat(yawAngle, pitchAngle, 0.0f);

	////these quaternions help avoid gimbal lock
	//quaternion r = glm::angleAxis(0.f, AXIS_Z);
	//quaternion p = glm::angleAxis(-pitchAngle, m_v3Target);
	//quaternion y = glm::angleAxis(yawAngle, m_v3Up);
	//rotQuat = glm::normalize(r * p * y);

	// Abbreviations for the various angular functions
	double cy = cos(yawAngle * 0.5);
	double sy = sin(yawAngle * 0.5);
	double cr = cos(0 * 0.5);
	double sr = sin(0 * 0.5);
	double cp = cos(pitchAngle * 0.5);
	double sp = sin(pitchAngle * 0.5);

	rotQuat.w = cy * cr * cp + sy * sr * sp;
	rotQuat.x = cy * sr * cp - sy * cr * sp;
	rotQuat.y = cy * cr * sp + sy * sr * cp;
	rotQuat.z = sy * cr * cp - cy * sr * sp;
	

}

quaternion Simplex::MyCamera::getConjugate() //gets opposite direction of quaterion
{
	return quaternion(rotQuat.w,-rotQuat.x, -rotQuat.y, -rotQuat.z);
}

vector3 Simplex::MyCamera::QuatConversion(vector3 vector) // convert quaternion into a vec3
{
	//set rotation on angles using quaternions
	vector3 vn(vector); 
	vector3 normVec = vn;
	quaternion vecQuat, resQuat; 
	vecQuat.x = normVec.x; 
	vecQuat.y = normVec.y; 
	vecQuat.z = normVec.z; 
	vecQuat.w = 0.0f;
	resQuat = vecQuat * getConjugate(); 
	resQuat = rotQuat * resQuat;
	return (vector3(resQuat.x, resQuat.y, resQuat.z));

}
