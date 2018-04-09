#pragma once
#include "Simplex\Simplex.h"
#include "MyEntity.h"


namespace Simplex
{
	class MyEntityManager
	{
		std::map<String, MyEntity*> MyEntity::m_modelDictionary;

	public:
		MyEntityManager(MyEntity const& other);
		~MyEntityManager();
	};

}

