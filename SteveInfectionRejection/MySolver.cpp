#include "MySolver.h"
using namespace Simplex;
//  MySolver
void MySolver::Init(void)
{
	m_v3Acceleration = ZERO_V3;
	m_v3Position = ZERO_V3;
	m_v3Velocity = ZERO_V3;
	m_v3ProperFacing = vector3(0.0f, 0.0f, -1.0f);
	m_fMass = 1.0f;
}
void MySolver::Swap(MySolver& other)
{
	std::swap(m_v3Acceleration, other.m_v3Acceleration);
	std::swap(m_v3Velocity, other.m_v3Velocity);
	std::swap(m_v3Position, other.m_v3Position);
	std::swap(m_fMass, other.m_fMass);
}
void MySolver::Release(void){/*nothing to deallocate*/ }
//The big 3
MySolver::MySolver(void){ Init(); }
MySolver::MySolver(MySolver const& other)
{
	m_v3Acceleration = other.m_v3Acceleration;
	m_v3Velocity = other.m_v3Velocity;
	m_v3Position = other.m_v3Position;
	m_fMass = other.m_fMass;
}
MySolver& MySolver::operator=(MySolver const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MySolver temp(other);
		Swap(temp);
	}
	return *this;
}
MySolver::~MySolver() { Release(); }

//Accessors
void MySolver::SetPosition(vector3 a_v3Position) { m_v3Position = a_v3Position; }
vector3 MySolver::GetPosition(void) { return m_v3Position; }

void MySolver::SetVelocity(vector3 a_v3Velocity) { m_v3Velocity = a_v3Velocity; }
vector3 MySolver::GetVelocity(void) { return m_v3Velocity; }

void MySolver::SetMass(float a_fMass) { m_fMass = a_fMass; }
float MySolver::GetMass(void) { return m_fMass; }

//Methods
void MySolver::ApplyFriction(float a_fFriction)
{
	if (a_fFriction < 0.01f)
		a_fFriction = 0.01f;
	
	m_v3Velocity *= 1.0f - a_fFriction;

	//if velocity is really small make it zero
	if (glm::length(m_v3Velocity) < 0.01f)
		m_v3Velocity = ZERO_V3;
}
void MySolver::ApplyForce(vector3 a_v3Force)
{
	//check minimum mass
	if (m_fMass < 0.01f)
		m_fMass = 0.01f;
	//f = m * a -> a = f / m
	m_v3Acceleration += a_v3Force / m_fMass;

}
vector3 CalculateMaxVelocity(vector3 a_v3Velocity, float maxVelocity)
{
	if (glm::length(a_v3Velocity) > maxVelocity)
	{
		if (a_v3Velocity == ZERO_V3) {
			vector3(0.0000001f, 0.0000001f, 0.0000001f);
		}
		a_v3Velocity = glm::normalize(a_v3Velocity);
		a_v3Velocity *= maxVelocity;
	}
	return a_v3Velocity;
}
vector3 RoundSmallVelocity(vector3 a_v3Velocity, float minVelocity = 0.01f)
{
	if (glm::length(a_v3Velocity) < minVelocity)
	{
		a_v3Velocity = ZERO_V3;
	}
	return a_v3Velocity;
}
void MySolver::Update(float deltaTime)
{
	// add gravity to the total force
	m_v3TotalForce += vector3(0.0f, -0.035f, 0.0f);

	//m_v3TotalForce = glm::clamp(m_v3TotalForce, vector3(0.0f, 0.0f, 0.0f), vector3(8.0f, 0.0f, 8.0f));
	
	// get acceleration from the total force
	ApplyForce(m_v3TotalForce);

	m_v3Velocity += m_v3Acceleration * deltaTime;
	
	m_v3Velocity = CalculateMaxVelocity(m_v3Velocity, m_fMaxVelocity);

	ApplyFriction(0.1f);
	m_v3Velocity = RoundSmallVelocity(m_v3Velocity, 0.028f);

	m_v3Position += m_v3Velocity * deltaTime;

			
	if (m_v3Position.y <= 0)
	{
		m_v3Position.y = 0;
		m_v3Velocity.y = 0;
	}

	

	//Don't want to set the ideal facing to 0.
	if (m_v3Velocity != vector3(0.0f, 0.0f, 0.0f)) {
		//Set properfacing to velocity and then normalize it to get a direction.
		m_v3ProperFacing = m_v3Velocity;
	}
	
	//safety.  Don't normalize vectors of zero or stuff WILL crash.
	if (m_v3ProperFacing != ZERO_V3) {
		glm::normalize(m_v3ProperFacing);
	}
	

	m_v3Acceleration = ZERO_V3;
	m_v3TotalForce = ZERO_V3;

}
void MySolver::ResolveCollision(MySolver* a_pOther)
{
	float fMagThis = glm::length(m_v3Velocity);
	float fMagOther = glm::length(m_v3Velocity);

	if (fMagThis > 0.015f || fMagOther > 0.015f)
	{
		//a_pOther->ApplyForce(GetVelocity());
		ApplyForce(-m_v3Velocity * 2.0f);
		a_pOther->ApplyForce(m_v3Velocity);
	}
	else
	{
		vector3 v3Direction = m_v3Position - a_pOther->m_v3Position;
		if(glm::length(v3Direction) != 0)
			v3Direction = glm::normalize(v3Direction);
		v3Direction *= 0.04f;
		ApplyForce(v3Direction * 2.0f);
		a_pOther->ApplyForce(-v3Direction);
	}
}

void Simplex::MySolver::Seek(vector3 targetPos, float a_fWeight)
{
	// Step 1: Calculate desired velocity
	// Vector pointing from myself to my target

	vector3 v3desiredVelocity = targetPos - m_v3Position;

	// Step 2: Scale desired to max speed
	//DEBUG NOTE: desiredVelocity MAY sometimes end up as zero somehow, breaking things.  Error handling to prevent that for debugging something else
	if (v3desiredVelocity == ZERO_V3) {
		v3desiredVelocity = vector3(0.0000001f, 0.0000001f, 0.0000001f);
	}
	glm::normalize(v3desiredVelocity);
	v3desiredVelocity *= 5.0f;

	// Step 3: Calculate the steering force for seeking
	// Steering = desired - current
	vector3 v3steeringForce = v3desiredVelocity - m_v3Velocity;

	// Step 4: Return steering force -> apply to acceleration
	m_v3TotalForce += v3steeringForce * a_fWeight;
}


void Simplex::MySolver::Flee(vector3 targetPos, float a_fWeight)
{
	vector3 v3desiredVelocity = m_v3Position - targetPos;

	//DEBUG NOTE: desiredVelocity MAY sometimes end up as zero somehow, breaking things.  Error handling to prevent that for debugging something else
	if (v3desiredVelocity == ZERO_V3) {
		v3desiredVelocity = vector3(0.0000001f, 0.0000001f, 0.0000001f);
	}
	glm::normalize(v3desiredVelocity);
	v3desiredVelocity *= 5.0f;

	vector3 v3steeringForce = v3desiredVelocity - m_v3Velocity;

	m_v3TotalForce += v3steeringForce * a_fWeight;
}

void Simplex::MySolver::Arrival(vector3 targetPos)
{
	// Step 1: Calculate desired velocity
	// Vector pointing from myself to my target
	vector3 v3desiredVelocity = targetPos - m_v3Position;

	float mag = glm::length(v3desiredVelocity);
	float fMaxSpeed;

	// if within radius, reduce the speed depending on how close you are
	if (mag <= 8.0f) {
		fMaxSpeed = 5.0f / mag;

		// stop at a certain point
		if (mag <= 1.0f)
		{
			fMaxSpeed = 0.0f;
		}
	}

	// Step 2: Scale desired to max speed
	//DEBUG NOTE: desiredVelocity MAY sometimes end up as zero somehow, breaking things.  Error handling to prevent that for debugging something else
	if (v3desiredVelocity == ZERO_V3) {
		v3desiredVelocity = vector3(0.0000001f, 0.0000001f, 0.0000001f);
	}
	glm::normalize(v3desiredVelocity);
	v3desiredVelocity *= fMaxSpeed;

	// Step 3: Calculate the steering force for seeking
	// Steering = desired - current
	vector3 v3steeringForce = v3desiredVelocity - m_v3Velocity;

	// Step 4: Return steering force -> apply to acceleration
	m_v3TotalForce += v3steeringForce;
}

void Simplex::MySolver::Separate(vector3 targetPos, float a_fWeight)
{
	vector3 v3desiredVelocity = GetPosition() - targetPos;
	float mag = glm::length(v3desiredVelocity);


	//DEBUG NOTE: desiredVelocity MAY sometimes end up as zero somehow, breaking things.  Error handling to prevent that for debugging something else
	if (v3desiredVelocity == ZERO_V3) {
		v3desiredVelocity = vector3(0.0000001f, 0.0000001f, 0.0000001f);
	}
	glm::normalize(v3desiredVelocity);
	v3desiredVelocity *= 5.0f;

	vector3 v3steeringForce = v3desiredVelocity - GetVelocity();

	m_v3TotalForce += (v3steeringForce * (1.0f / mag)) * a_fWeight;
}

vector3 Simplex::MySolver::CalculateWander()
{
	vector3 v3distAhead = GetPosition() + (GetVelocity() * 2.5f);

	// reset random
	srand(time(NULL));

	// random number between 0 and 1
	float f_angle = static_cast<float> (rand()) / static_cast<float>(RAND_MAX);

	// multiply with 2PI
	f_angle = f_angle * (2 * PI);

	// get angles
	float f_x = cos(f_angle) * 0.2f;
	float f_z = sin(f_angle) * 0.2f;

	// new position
	vector3 v3NewPos = vector3(f_x, 0.0f, f_z);

	// add future pos and the new pos
	return (v3NewPos + v3distAhead);
}


bool Simplex::MySolver::OutOfBounds(void)
{

	if (m_v3Position.x <= -40.0f || m_v3Position.x >= 40.0f || m_v3Position.z <= -40.0f || m_v3Position.z >= 40.0f) {
		return true;
	}

	return false;
}


void Simplex::MySolver::SetTotalForce(vector3 totalForce)
{
	m_v3TotalForce = totalForce;
}

void Simplex::MySolver::SetMaxVelocity(float a_fVelocity)
{
	m_fMaxVelocity = a_fVelocity;
}

void Simplex::MySolver::SetDirection(vector3 a_v3Direction)
{
	m_v3Direction = a_v3Direction;
}

//TODO: possibly discard.  No code references instances of getdirection or setdirection.
vector3 Simplex::MySolver::GetDirection(void)
{
	return m_v3Direction;
}


vector3 Simplex::MySolver::GetProperFacing(void) {
	//even MORE security to prevent it passing a zero vector
	if (m_v3ProperFacing == vector3(0.0f, 0.0f, 0.0f) ){

		m_v3ProperFacing = vector3(0.0f, 0.0f, -1.0f);
	}

	return m_v3ProperFacing;
}