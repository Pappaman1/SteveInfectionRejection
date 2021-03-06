#include "MyEntity.h"
using namespace Simplex;
std::map<String, MyEntity*> MyEntity::m_IDMap;
//  Accessors
Simplex::MySolver* Simplex::MyEntity::GetSolver(void) { return m_pSolver; }
bool Simplex::MyEntity::HasThisRigidBody(MyRigidBody* a_pRigidBody) { return m_pRigidBody == a_pRigidBody; }
Simplex::MyRigidBody::PRigidBody* Simplex::MyEntity::GetColliderArray(void) { return m_pRigidBody->GetColliderArray(); }
uint Simplex::MyEntity::GetCollidingCount(void) { return m_pRigidBody->GetCollidingCount(); }
matrix4 Simplex::MyEntity::GetModelMatrix(void){ return m_m4ToWorld; }
void Simplex::MyEntity::SetModelMatrix(matrix4 a_m4ToWorld)
{
	if (!m_bInMemory)
		return;

	m_m4ToWorld = a_m4ToWorld;
	m_pModel->SetModelMatrix(m_m4ToWorld);
	m_pRigidBody->SetModelMatrix(m_m4ToWorld);
	m_pSolver->SetPosition(vector3(m_m4ToWorld[3]));
}
Model* Simplex::MyEntity::GetModel(void){return m_pModel;}
MyRigidBody* Simplex::MyEntity::GetRigidBody(void){	return m_pRigidBody; }
bool Simplex::MyEntity::IsInitialized(void){ return m_bInMemory; }
String Simplex::MyEntity::GetUniqueID(void) { return m_sUniqueID; }
void Simplex::MyEntity::SetAxisVisible(bool a_bSetAxis) { m_bSetAxis = a_bSetAxis; }
void Simplex::MyEntity::SetPosition(vector3 a_v3Position) { if(m_pSolver) m_pSolver->SetPosition(a_v3Position); }
vector3 m_v3Forward =  vector3(0.0f, 0.0f, -1.0f);
Simplex::vector3 Simplex::MyEntity::GetPosition(void)
{
	if (m_pSolver != nullptr)
		return m_pSolver->GetPosition();
	return vector3();
}

void Simplex::MyEntity::SetVelocity(vector3 a_v3Velocity) { if (m_pSolver) m_pSolver->SetVelocity(a_v3Velocity); }
Simplex::vector3 Simplex::MyEntity::GetVelocity(void)
{
	if (m_pSolver != nullptr)
		return m_pSolver->GetVelocity();
	return vector3();
}

void Simplex::MyEntity::SetMass(float a_fMass) { if (m_pSolver) m_pSolver->SetMass(a_fMass); }
float Simplex::MyEntity::GetMass(void)
{
	if (m_pSolver != nullptr)
		return m_pSolver->GetMass();
	return 1.0f;
}
//  MyEntity
void Simplex::MyEntity::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bInMemory = false;
	m_bSetAxis = false;
	m_pModel = nullptr;
	m_pRigidBody = nullptr;
	m_DimensionArray = nullptr;
	m_m4ToWorld = IDENTITY_M4;
	m_sUniqueID = "";
	m_nDimensionCount = 0;
	m_bUsePhysicsSolver = false;
	m_pSolver = nullptr;
}
void Simplex::MyEntity::Swap(MyEntity& other)
{
	m_bInMemory = false;
	std::swap(m_pModel, other.m_pModel);
	std::swap(m_pRigidBody, other.m_pRigidBody);
	std::swap(m_m4ToWorld, other.m_m4ToWorld);
	std::swap(m_pMeshMngr, other.m_pMeshMngr);
	std::swap(m_bInMemory, other.m_bInMemory);
	std::swap(m_sUniqueID, other.m_sUniqueID);
	std::swap(m_bSetAxis, other.m_bSetAxis);
	std::swap(m_nDimensionCount, other.m_nDimensionCount);
	std::swap(m_DimensionArray, other.m_DimensionArray);
	std::swap(m_pSolver, other.m_pSolver);
}
void Simplex::MyEntity::Release(void)
{
	m_pMeshMngr = nullptr;
	//it is not the job of the entity to release the model, 
	//it is for the mesh manager to do so.
	m_pModel = nullptr;
	if (m_DimensionArray)
	{
		delete[] m_DimensionArray;
		m_DimensionArray = nullptr;
	}
	SafeDelete(m_pRigidBody);
	SafeDelete(m_pSolver);
	m_IDMap.erase(m_sUniqueID);
}
//The big 3
Simplex::MyEntity::MyEntity(String a_sFileName, String a_sUniqueID)
{
	Init();
	m_pModel = new Model();
	m_pModel->Load(a_sFileName);
	//if the model is loaded
	if (m_pModel->GetName() != "")
	{
		GenUniqueID(a_sUniqueID);
		m_sUniqueID = a_sUniqueID;
		m_IDMap[a_sUniqueID] = this;
		m_pRigidBody = new MyRigidBody(m_pModel->GetVertexList()); //generate a rigid body
		m_bInMemory = true; //mark this entity as viable
	}
	m_pSolver = new MySolver(this);
}
Simplex::MyEntity::MyEntity(MyEntity const& other)
{
	m_bInMemory = other.m_bInMemory;
	m_pModel = other.m_pModel;
	//generate a new rigid body we do not share the same rigid body as we do the model
	m_pRigidBody = new MyRigidBody(m_pModel->GetVertexList()); 
	m_m4ToWorld = other.m_m4ToWorld;
	m_pMeshMngr = other.m_pMeshMngr;
	m_sUniqueID = other.m_sUniqueID;
	m_bSetAxis = other.m_bSetAxis;
	m_nDimensionCount = other.m_nDimensionCount;
	m_DimensionArray = other.m_DimensionArray;
	m_pSolver = new MySolver(*other.m_pSolver);
}
MyEntity& Simplex::MyEntity::operator=(MyEntity const& other)
{
	if(this != &other)
	{
		Release();
		Init();
		MyEntity temp(other);
		Swap(temp);
	}
	return *this;
}
MyEntity::~MyEntity(){Release();}
//--- Methods
void Simplex::MyEntity::AddToRenderList(bool a_bDrawRigidBody)
{
	//if not in memory return
	if (!m_bInMemory)
		return;

	//draw model
	m_pModel->AddToRenderList();
	
	//draw rigid body
	if(a_bDrawRigidBody)
		m_pRigidBody->AddToRenderList();

	if (m_bSetAxis)
		m_pMeshMngr->AddAxisToRenderList(m_m4ToWorld);
}
MyEntity* Simplex::MyEntity::GetEntity(String a_sUniqueID)
{
	//look the entity based on the unique id
	auto entity = m_IDMap.find(a_sUniqueID);
	//if not found return nullptr, if found return it
	return entity == m_IDMap.end() ? nullptr : entity->second;
}
void Simplex::MyEntity::GenUniqueID(String& a_sUniqueID)
{
	static uint index = 0;
	String sName = a_sUniqueID;
	MyEntity* pEntity = GetEntity(a_sUniqueID);
	//while MyEntity exists keep changing name
	while (pEntity)
	{
		a_sUniqueID = sName + "_" + std::to_string(index);
		index++;
		pEntity = GetEntity(a_sUniqueID);
	}
	return;
}
void Simplex::MyEntity::AddDimension(uint a_uDimension)
{
	//we need to check that this dimension is not already allocated in the list
	if (IsInDimension(a_uDimension))
		return;//it is, so there is no need to add

	//insert the entry
	uint* pTemp;
	pTemp = new uint[m_nDimensionCount + 1];
	if(m_DimensionArray)
	{
		memcpy(pTemp, m_DimensionArray, sizeof(uint) * m_nDimensionCount);
		delete[] m_DimensionArray;
		m_DimensionArray = nullptr;
	}
	pTemp[m_nDimensionCount] = a_uDimension;
	m_DimensionArray = pTemp;

	++m_nDimensionCount;

	SortDimensions();
}
void Simplex::MyEntity::RemoveDimension(uint a_uDimension)
{
	//if there are no dimensions return
	if (m_nDimensionCount == 0)
		return;

	//we look one by one if its the one wanted
	for (uint i = 0; i < m_nDimensionCount; i++)
	{
		if (m_DimensionArray[i] == a_uDimension)
		{
			//if it is, then we swap it with the last one and then we pop
			std::swap(m_DimensionArray[i], m_DimensionArray[m_nDimensionCount - 1]);
			uint* pTemp;
			pTemp = new uint[m_nDimensionCount - 1];
			if (m_DimensionArray)
			{
				memcpy(pTemp, m_DimensionArray, sizeof(uint) * (m_nDimensionCount-1));
				delete[] m_DimensionArray;
				m_DimensionArray = nullptr;
			}
			m_DimensionArray = pTemp;
			
			--m_nDimensionCount;
			SortDimensions();
			return;
		}
	}
}
void Simplex::MyEntity::ClearDimensionSet(void)
{
	if (m_DimensionArray)
	{
		delete[] m_DimensionArray;
		m_DimensionArray = nullptr;
	}
	m_nDimensionCount = 0;
}
bool Simplex::MyEntity::IsInDimension(uint a_uDimension)
{
	//see if the entry is in the set
	for (uint i = 0; i < m_nDimensionCount; i++)
	{
		if (m_DimensionArray[i] == a_uDimension)
			return true;
	}
	return false;
}
bool Simplex::MyEntity::SharesDimension(MyEntity* const a_pOther)
{
	
	//special case: if there are no dimensions on either MyEntity
	//then they live in the special global dimension
	if (0 == m_nDimensionCount)
	{
		//if no spatial optimization all cases should fall here as every 
		//entity is by default, under the special global dimension only
		if(0 == a_pOther->m_nDimensionCount)
			return true;
	}

	//for each dimension on both Entities we check if there is a common dimension
	for (uint i = 0; i < m_nDimensionCount; ++i)
	{
		for (uint j = 0; j < a_pOther->m_nDimensionCount; j++)
		{
			if (m_DimensionArray[i] == a_pOther->m_DimensionArray[j])
				return true; //as soon as we find one we know they share dimensionality
		}
	}

	//could not find a common dimension
	return false;
}
bool Simplex::MyEntity::IsColliding(MyEntity* const other)
{
	//if not in memory return
	if (!m_bInMemory || !other->m_bInMemory)
		return true;

	//if the entities are not living in the same dimension
	//they are not colliding
	if (!SharesDimension(other))
		return false;

	// returns false if the star collides with the Steve that threw it
	if ( m_myType == Star && other->GetUniqueID() == m_ninjaParent->GetUniqueID() ) {
		return false;
	} else if (m_myType == Steve && other->GetEntityType() == Star) {
		if (GetUniqueID() == other->GetNinjaParent()->GetUniqueID()) {
			return false;
		}
	}

	return m_pRigidBody->IsColliding(other->GetRigidBody());
}
void Simplex::MyEntity::ClearCollisionList(void)
{
	m_pRigidBody->ClearCollidingList();
}
void Simplex::MyEntity::SortDimensions(void)
{
	std::sort(m_DimensionArray, m_DimensionArray + m_nDimensionCount);
}
void Simplex::MyEntity::ApplyForce(vector3 a_v3Force)
{
	m_pSolver->ApplyForce(a_v3Force);
}
void Simplex::MyEntity::Update(float deltaTime)
{
	if (m_bUsePhysicsSolver)
	{
		// all Steves or Zombies wanders and need to check if they are out of bounds
		if (m_myType == Steve || m_myType == Zombie) {

			m_pSolver->Seek(m_pSolver->CalculateWander(), 1.0f);

			// if going too far away, seek the middle
			if (m_pSolver->OutOfBounds()) {
				m_pSolver->Seek(vector3(0.0f, 0.0f, 0.0f), 2.0f);
			}
		}

		static float coolDown = 0.0f;
	

		m_pSolver->Update(deltaTime);

		// cooldown to attack
		if (m_attacking) {
			coolDown = coolDown + deltaTime;
			if (coolDown >= 60.0f) {
				m_readyToAttack = true;
				coolDown = 0.0f;
			}
		}

		SetModelMatrix(glm::translate(m_pSolver->GetPosition()));

		


		//TODO: finish rotation.  Commented out because not working atm.
		/*
		//Get a normalized vector from Solver for the direction the object is currently moving.  We only need X and Z.  We don't want the models facing up or down.
		vector3 transmittedForward = vector3(GetProperFacing().x, 0.0f, GetProperFacing().z);

		m_v3Forward = vector3(0.0f, 0.0f, -1.0f);

		//Compare the current direction being traveled to the entity's current forward.  If they're different, continue.
		if (transmittedForward != m_v3Forward) {
			
			//TODO compare TransmittedForward to m_v3Forward (the Entity's forward) to know how much to rotate the entity so it faces the right way

			//TODO Rotate the entity to be facing the right way

			//Set the entity's logged forward vector to its new one

			float dog = (glm::dot(transmittedForward, m_v3Forward) / (transmittedForward.length() * m_v3Forward.length()));

			vector3 rotationAmount = glm::acos(vector3(dog, dog, dog));
			
			matrix4 tempMatrix = glm::rotate(IDENTITY_M4, rotationAmount.x, vector3(0.0f, 1.0f, 0.0f));
			tempMatrix[3] = m_m4ToWorld[3];
			SetModelMatrix(tempMatrix);

			
		}



		//DEBUG: Making sure the chosen object for rotation won't affect movement
		//matrix4 tempMatrix = glm::rotate(IDENTITY_M4, 20.0f, vector3(0.0f, 1.0f, 0.0f));
		//tempMatrix[3] = m_m4ToWorld[3];
		//SetModelMatrix(tempMatrix);
		
		*/
	}
}
void Simplex::MyEntity::ResolveCollision(MyEntity* a_pOther)
{
	if (m_bUsePhysicsSolver)
	{
		m_pSolver->ResolveCollision(a_pOther->GetSolver());
	}
}
void Simplex::MyEntity::UsePhysicsSolver(bool a_bUse)
{
	m_bUsePhysicsSolver = a_bUse;
}


void Simplex::MyEntity::SetFlee(void)
{
	m_flee = true;
}

void Simplex::MyEntity::SetAngry(void)
{
	m_angry = true;
}

bool Simplex::MyEntity::GetAngry(void)
{
	return m_angry;
}

void Simplex::MyEntity::SetEntityType(uint a_mType)
{
	if (a_mType == 0) m_myType = Steve;
	if (a_mType == 1) m_myType = Zombie;
	if (a_mType == 2) m_myType = Main;
	if (a_mType == 3) m_myType = Star;
}

MyEntity::EntityType Simplex::MyEntity::GetEntityType()
{
	return m_myType;
}

void Simplex::MyEntity::SetDirection(vector3 a_v3Direction)
{
	if (m_pSolver) m_pSolver->SetDirection(a_v3Direction);
}

void Simplex::MyEntity::SetMaxVelocity(float a_fVelocity)
{
	if (m_pSolver) m_pSolver->SetMaxVelocity(a_fVelocity);
}


vector3 Simplex::MyEntity::GetDirection(void)
{
	if (m_pSolver != nullptr)
		return m_pSolver->GetDirection();
	return vector3();
}

vector3 Simplex::MyEntity::GetProperFacing(void)
{
	if (m_pSolver != nullptr)
		return m_pSolver->GetProperFacing();
	return vector3();
}

bool Simplex::MyEntity::IsClose(MyEntity * const other)
{
	if (!m_bInMemory || !other->m_bInMemory)
		return true;

	//if the entities are not living in the same dimension
	//then they can't be close
	if (!SharesDimension(other))
		return false;

	return true;
}


void Simplex::MyEntity::ResolveBeingClose(MyEntity * a_pOther)
{
	// get the distance
	vector3 v3distance = GetPosition() - a_pOther->GetPosition();

	// magnitude of that
	float mag = 0.0f;

	if (v3distance != vector3()) mag = glm::length(v3distance);

	// if they are of same type (Steves, or Zombies)
	// then just check if they need to separate
	if (GetEntityType() == a_pOther->GetEntityType()) {

		// if close, then separate them
		if (mag <= 10.0f) {
			m_pSolver->Separate(a_pOther->GetPosition(), 4.0f);
		}

	} else {

		// if this entity is a zmobie
		// commented out now since we don't spawn other Zombies yet 


		//if (m_myType == Zombie) {
		//
		//	// if the other one is an angry Steve, then flee
		//	if (other->GetAngry()) {
		//		m_pSolver->Flee(other->GetPosition(), 3.0f);
		//	}
		//	// if a scared Steve, then chase
		//	else {
		//		m_pSolver->Seek(other->GetPosition(), 2.0f);
		//	}
		//
		//}
		// if this entity is a Steve
		//else {

		// if he's angry, then Seek the zombie
		if (mag <= 10.0f) {

			// if Main, then main do nothing, but others do things to him
			if (m_myType == Main && a_pOther->GetEntityType() == Steve) {

				// if other is angry, then Seek Main
				if (a_pOther->GetAngry()) {
					a_pOther->m_pSolver->Seek(GetPosition(), 3.0f);

					// set to attack mode
					a_pOther->SetAttacking();
				}
				// if not, then flee from Main
				else {
					a_pOther->m_pSolver->Flee(GetPosition(), 2.0f);
				}
			}
			else if (m_myType == Steve && a_pOther->GetEntityType() == Main || a_pOther->GetEntityType() == Zombie) {
				if (m_angry) {
					m_pSolver->Seek(a_pOther->GetPosition(), 3.0f);

					// set to attack mode
					m_attacking = true;
				}

				// else run away from it
				else {
					m_pSolver->Flee(a_pOther->GetPosition(), 2.0f);
				}
			}

		}
		else {
			// not attacking
			m_attacking = false;
			a_pOther->SetAttacking();
		}

		//}

	}
}

bool Simplex::MyEntity::IsReadyToAttack(void)
{
	return m_readyToAttack;
}

void Simplex::MyEntity::DidAttack(void)
{
	m_readyToAttack = false;
}

void Simplex::MyEntity::SetAttacking(void)
{
	m_attacking = !m_attacking;
}

void Simplex::MyEntity::SetNinjaParent(MyEntity * a_pParent)
{
	m_ninjaParent = a_pParent;
}

MyEntity * Simplex::MyEntity::GetNinjaParent(void)
{
	return m_ninjaParent;
}

void Simplex::MyEntity::SetNumOfLives(uint a_pNum)
{
	m_numOfLives = a_pNum;
}

uint Simplex::MyEntity::GetNumOfLives(void)
{
	return m_numOfLives;
}

void Simplex::MyEntity::ChangeModel(std::string a_fileName)
{


}

vector3 Simplex::MyEntity::GetForward()
{
	return m_v3Forward;
}

void Simplex::MyEntity::SetForward(vector3 freshForward) {
	m_v3Forward = freshForward;
}