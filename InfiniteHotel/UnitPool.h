#pragma once


#include <memory>
#include <queue>




class Unit;


class UnitPool
{
public:
	explicit UnitPool(int chunkSize = 64);


protected:
	std::queue<std::unique_ptr<Unit>> m_pool;
	int m_chunkSize;


public:
	std::unique_ptr<Unit> acquireUnit();
	void releaseUnit(std::unique_ptr<Unit>&& unit);
};

