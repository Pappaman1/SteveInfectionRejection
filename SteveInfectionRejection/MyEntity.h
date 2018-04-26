
/*----------------------------------------------
Programmer: Alberto Bobadilla (labigm@gmail.com)
Date: 2017/07
----------------------------------------------*/
#ifndef __MYENTITY_H_
#define __MYENTITY_H_

#include "MySolver.h"

namespace Simplex
{

//System Class
class MyEntity
{
	bool m_bInMemory = false; //loaded flag
	bool m_bSetAxis = false; //render axis flag
	String m_sUniqueID = ""; //Unique identifier name

	bool m_flee = false;	// if a scared Steve
	bool m_angry = false;	// if an angry Steve
	bool m_fMaxVelocity;
	bool m_attacking = false;
	bool m_readyToAttack = false;	// if able to throw ninja star
	uint m_numOfLives = 0;

	// Different entity types
	enum EntityType { Steve, Zombie, Main, Star };

	// this entitys type
	EntityType m_myType;

	MyEntity* m_ninjaParent = nullptr;

	uint m_nDimensionCount = 0; //tells how many dimensions this entity lives in
	uint* m_DimensionArray = nullptr; //Dimensions on which this entity is located

	Model* m_pModel = nullptr; //Model associated with this MyEntity
	MyRigidBody* m_pRigidBody = nullptr; //Rigid Body associated with this MyEntity

	matrix4 m_m4ToWorld = IDENTITY_M4; //Model matrix associated with this MyEntity
	MeshManager* m_pMeshMngr = nullptr; //For rendering shapes

	static std::map<String, MyEntity*> m_IDMap; //a map of the unique ID's

	bool m_bUsePhysicsSolver = false; //Indicates if we will use a physics solver 

	MySolver* m_pSolver = nullptr; //Physics MySolver

public:
	
	//what direction the entity is facing.  Used to rotate model to face direction it's moving in by comparing to how fast it's moving.
	vector3 m_v3Forward = vector3 (0.0f, 0.0f, 1.0f);

	//gives the forward a new vector3 for its new facing direction
	void SetForward(vector3);

	//returns the entity's forward vector.  Used to calculate whether current movement is in current facing direction and, if not, rotate to fit.
	vector3 GetForward();
	
	
	/*
	Usage: Constructor that specifies the name attached to the MyEntity
	Arguments:
	-	String a_sFileName -> Name of the model to load
	-	String a_sUniqueID -> Name wanted as identifier, if not available will generate one
	Output: class object instance
	*/
	MyEntity(String a_sFileName, String a_sUniqueID = "NA");
	/*
	Usage: Copy Constructor
	Arguments: class object to copy
	Output: class object instance
	*/
	MyEntity(MyEntity const& other);
	/*
	Usage: Copy Assignment Operator
	Arguments: class object to copy
	Output: ---
	*/
	MyEntity& operator=(MyEntity const& other);
	/*
	Usage: Destructor
	Arguments: ---
	Output: ---
	*/
	~MyEntity(void);
	/*
	Usage: Changes object contents for other object's
	Arguments: other -> object to swap content from
	Output: ---
	*/
	void Swap(MyEntity& other);
	/*
	USAGE: Gets the model matrix associated with this entity
	ARGUMENTS: ---
	OUTPUT: model to world matrix
	*/
	matrix4 GetModelMatrix(void);
	/*
	USAGE: Sets the model matrix associated with this entity
	ARGUMENTS: matrix4 a_m4ToWorld -> model matrix to set
	OUTPUT: ---
	*/
	void SetModelMatrix(matrix4 a_m4ToWorld);
	/*
	USAGE: Gets the model associated with this entity
	ARGUMENTS: ---
	OUTPUT: Model
	*/
	Model* GetModel(void);
	/*
	USAGE: Gets the Rigid Body associated with this entity
	ARGUMENTS: ---
	OUTPUT: Rigid Body
	*/
	MyRigidBody* GetRigidBody(void);
	/*
	USAGE: Will reply to the question, is the MyEntity Initialized?
	ARGUMENTS: ---
	OUTPUT: initialized?
	*/
	bool IsInitialized(void);
	/*
	USAGE: Adds the entity to the render list
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void AddToRenderList(bool a_bDrawRigidBody = false);
	/*
	USAGE: Tells if this entity is colliding with the incoming one
	ARGUMENTS: MyEntity* const other -> inspected entity
	OUTPUT: are they colliding?
	*/
	bool IsColliding(MyEntity* const other);
	/*
	USAGE: Gets the MyEntity specified by unique ID, nullptr if not exists
	ARGUMENTS: String a_sUniqueID -> unique ID if the queried entity
	OUTPUT: MyEntity specified by unique ID, nullptr if not exists
	*/
	static MyEntity* GetEntity(String a_sUniqueID);
	/*
	USAGE: Will generate a unique id based on the name provided
	ARGUMENTS: String& a_sUniqueID -> desired name
	OUTPUT: will output though the argument
	*/
	void GenUniqueID(String& a_sUniqueID);
	/*
	USAGE: Gets the Unique ID name of this model
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	String GetUniqueID(void);
	/*
	USAGE: Sets the visibility of the axis of this MyEntity
	ARGUMENTS: bool a_bSetAxis = true -> axis visible?
	OUTPUT: ---
	*/
	void SetAxisVisible(bool a_bSetAxis = true);
	/*
	USAGE: Will set a dimension to the MyEntity
	ARGUMENTS: uint a_uDimension -> dimension to set
	OUTPUT: ---
	*/
	void AddDimension(uint a_uDimension);
	/*
	USAGE: Will remove the entity from the specified dimension
	ARGUMENTS: uint a_uDimension -> dimension to remove
	OUTPUT: ---
	*/
	void RemoveDimension(uint a_uDimension);
	/*
	USAGE: will remove all dimensions from entity
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void ClearDimensionSet(void);
	/*
	USAGE: Will ask if the MyEntity is located in a particular dimension
	ARGUMENTS: uint a_uDimension -> dimension queried
	OUTPUT: result
	*/
	bool IsInDimension(uint a_uDimension);
	/*
	USAGE: Asks if this entity shares a dimension with the incoming one
	ARGUMENTS: MyEntity* const a_pOther -> queried entity
	OUTPUT: shares at least one dimension?
	*/
	bool SharesDimension(MyEntity* const a_pOther);

	/*
	USAGE: Clears the collision list of this entity
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void ClearCollisionList(void);

	/*
	USAGE: Will sort the array of dimensions
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void SortDimensions(void);

	/*
	USAGE: Gets the array of rigid bodies pointer this one is colliding with
	ARGUMENTS: ---
	OUTPUT: list of colliding rigid bodies
	*/
	MyRigidBody::PRigidBody* GetColliderArray(void);
	
	/*
	USAGE: Returns the number of objects colliding with this one
	ARGUMENTS: ---
	OUTPUT: colliding count
	*/
	uint GetCollidingCount(void);

	/*
	USAGE: Asks this MyEntity if this is the rigid body that belogs to it
	ARGUMENTS: MyRigidBody* a_pRigidBody -> Queried Rigid Body
	OUTPUT: is this your rigid body?
	*/
	bool HasThisRigidBody(MyRigidBody* a_pRigidBody);

	/*
	USAGE: Asks the entity to resolve the collision with the incoming one
	ARGUMENTS: MyEntity* a_pOther -> Queried entity
	OUTPUT: ---
	*/
	void ResolveCollision(MyEntity* a_pOther);

	/*
	USAGE: Gets the solver applied to this MyEntity
	ARGUMENTS: ---
	OUTPUT: MySolver applied
	*/
	MySolver* GetSolver(void);
	/*
	USAGE: Applies a force to the solver
	ARGUMENTS: vector3 a_v3Force -> force to apply
	OUTPUT: ---
	*/
	void ApplyForce(vector3 a_v3Force);
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
	USAGE: Updates the MyEntity
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void Update(float deltaTime);
	/*
	USAGE: Resolves using physics solver or not in the update
	ARGUMENTS: bool a_bUse = true -> using physics solver?
	OUTPUT: ---
	*/
	void UsePhysicsSolver(bool a_bUse = true);

	// ==== FUNCTIONS ADDED FOR PROJECT ====

	/*
	USAGE: Determines if this is a scared Steve
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void SetFlee(void);
	/*
	USAGE: Determines if this is an angry Steve
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void SetAngry(void);
	/*
	USAGE: A getter for the Angry boolean
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	bool GetAngry(void);
	/*
	USAGE: Sets what type of Entity this is
	ARGUMENTS: a_mType (0, 1, or 2)
	OUTPUT: ---
	*/
	void SetEntityType(uint a_mType);
	/*
	USAGE: Returns the Entity type
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	EntityType GetEntityType();
	/*
	USAGE: Sets the Direction of this Entity
	ARGUMENTS: a_v3Direction direction to face
	OUTPUT: ---
	*/
	void SetDirection(vector3 a_v3Direction);
	/*
	USAGE: Sets the max velocity for this Entity
	ARGUMENTS: a_fVelocity max float
	OUTPUT: ---
	*/
	void SetMaxVelocity(float a_fVelocity);
	/*
	USAGE: Returns the Direction
	ARGUMENTS: a_fVelocity max float
	OUTPUT: ---
	*/
	vector3 GetDirection(void);

	/*
	USAGE: Returns the Direction the character should be facing
	ARGUMENTS: a_fVelocity max float
	OUTPUT: ---
	*/
	vector3 GetProperFacing(void);

	/*
	USAGE: Checks if two entities are close (exist in same dimension)
	ARGUMENTS: other (pointer to another Entity)
	OUTPUT: ---
	*/
	bool IsClose(MyEntity* const other);
	/*
	USAGE: If two Entities are close, then do appropriate Steering Behaviors and stuff
	ARGUMENTS: a_pOther (pointer to another Entity)
	OUTPUT: ---
	*/
	void ResolveBeingClose(MyEntity* a_pOther);
	/*
	USAGE: Returns if the Entity is ready to throw ninja star or not
	ARGUMENTS: ---- 
	OUTPUT: ---
	*/
	bool IsReadyToAttack(void);

	void DidAttack(void);
	
	void SetAttacking(void);

	void SetNinjaParent(MyEntity* a_pParent);
	MyEntity* GetNinjaParent(void);

	void SetNumOfLives(uint a_pNum);
	uint GetNumOfLives(void);

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

#endif //__MYENTITY_H_

/*
USAGE:
ARGUMENTS: ---
OUTPUT: ---
*/