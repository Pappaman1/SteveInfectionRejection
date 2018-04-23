/*----------------------------------------------
Programmer: Alberto Bobadilla (labigm@gmail.com)
Date: 2017/07
----------------------------------------------*/
#ifndef __MYSOLVER_H_
#define __MYSOLVER_H_

#include "MyRigidBody.h"

namespace Simplex
{

class MySolver
{
	vector3 m_v3Acceleration = ZERO_V3; //Acceleration of the MySolver
	vector3 m_v3Position = ZERO_V3; //Position of the MySolver
	vector3 m_v3Velocity = ZERO_V3; //Velocity of the MySolver
	vector3 m_v3TotalForce = ZERO_V3;
	vector3 m_v3Direction = ZERO_V3;

	float m_fMaxVelocity = 5.0f;
	float m_fMass = 1.0f; //Mass of the solver
public:
	/*
	USAGE: Constructor
	ARGUMENTS: ---
	OUTPUT: object instance
	*/
	MySolver(void);
	/*
	USAGE: Copy Constructor
	ARGUMENTS: object to copy
	OUTPUT: object instance
	*/
	MySolver(MySolver const& other);
	/*
	USAGE: Copy Assignment Operator
	ARGUMENTS: object to copy
	OUTPUT: this instance
	*/
	MySolver& operator=(MySolver const& other);
	/*
	USAGE: Destructor
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	~MySolver(void);
	/*
	USAGE: Content Swap
	ARGUMENTS: object to swap content with
	OUTPUT: ---
	*/
	void Swap(MySolver& other);

	/*
	USAGE: Sets the position of the solver
	ARGUMENTS: vector3 a_v3Position -> position to set
	OUTPUT: ---
	*/
	void SetPosition(vector3 a_v3Position);
	/*
	USAGE: Gets the position of the solver
	ARGUMENTS: ---
	OUTPUT: position of the solver
	*/
	vector3 GetPosition(void);

	/*
	USAGE: Sets the velocity of the solver
	ARGUMENTS: vector3 a_v3Velocity -> velocity to set
	OUTPUT: ---
	*/
	void SetVelocity(vector3 a_v3Velocity);
	/*
	USAGE: Gets the velocity of the solver
	ARGUMENTS: ---
	OUTPUT: velocity of the solver
	*/
	vector3 GetVelocity(void);

	/*
	USAGE: Sets the mass of the solver
	ARGUMENTS: float a_fMass -> mass to set
	OUTPUT: ---
	*/
	void SetMass(float a_fMass);
	/*
	USAGE: Gets mass of the solver
	ARGUMENTS: ---
	OUTPUT: mass of the object
	*/
	float GetMass(void);

	/*
	USAGE: Applies friction to the movement
	ARGUMENTS: float a_fFriction = 0.1f -> friction to apply negative friction gets minimized to 0.01f
	OUTPUT: ---
	*/
	void ApplyFriction(float a_fFriction = 0.1f);
	/*
	USAGE: Applies a force to the solver
	ARGUMENTS: vector3 a_v3Force -> Force to apply
	OUTPUT: ---
	*/
	void ApplyForce(vector3 a_v3Force);
	/*
	USAGE: Updates the MySolver
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void Update(float deltaTime);
	/*
	USAGE: Resolve the collision between two solvers
	ARGUMENTS: MySolver* a_pOther -> other solver to resolve collision with
	OUTPUT: ---
	*/
	void ResolveCollision(MySolver* a_pOther);

	// ========= FUNCTIONS ADDED FOR THE PROJECT ========

	/*
	USAGE: Seeks a target position to move towards
	ARGUMENTS: vector3 targetPos (position to move towards) and a_fWeight (how important is this force)
	*/
	void Seek(vector3 targetPos, float a_fWeight);
	/*
	USAGE: When getting close to a position, it will slow down the entity
	ARGUMENTS: vector3 targetPos (position to move towards)
	*/
	void Arrival(vector3 targetPos);
	/*
	USAGE: If entities of same type are close to each other, this func will separate them
	ARGUMENTS: vector3 targetPos (position to move away from) and a_fWeight (how important is this force)
	*/
	void Separate(vector3 targetPos, float a_fWeight);
	/*
	USAGE: Calculates a random point to move towards
	ARGUMENTS: ---
	*/
	vector3 CalculateWander();
	/*
	USAGE: Checks if the entity is getting too far away, if so, will Seek the middle
	ARGUMENTS: ---
	*/

	bool OutOfBounds(void);
	/*
	USAGE: Moves away an Entity from the target position
	ARGUMENTS: vector3 targetPos (position to move away from) and a_fWeight (how important is this force)
	*/
	void Flee(vector3 targetPos, float a_fWeight);
	/*
	USAGE: Set's the total force of all added forces
	ARGUMENTS: vector3 totalForce
	*/
	void SetTotalForce(vector3 totalForce);
	/*
	USAGE: Set's the max velocity of the Entity
	ARGUMENTS: vector3 a_fVelocity
	*/
	void SetMaxVelocity(float a_fVelocity);
	/*
	USAGE: Set's the direction of the entity
	ARGUMENTS: vector3a_v3Direction
	*/
	void SetDirection(vector3 a_v3Direction);
	/*
	USAGE: Returns the direction of the entity
	ARGUMENTS: ---
	*/
	vector3 GetDirection(void);



private:
	/*
	Usage: Deallocates member fields
	Arguments: ---
	Output: ---
	*/
	void Release(void);
	/*
	Usage: Allocates member fields
	Arguments: ---
	Output: ---
	*/
	void Init(void);
};//class

} //namespace Simplex

#endif //__SOLVER_H_

/*
USAGE:
ARGUMENTS: ---
OUTPUT: ---
*/