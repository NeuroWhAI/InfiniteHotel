#include "UnitPool.h"

#include "Unit.h"




UnitPool::UnitPool(int chunkSize)
	: m_chunkSize(chunkSize)
{
	if (chunkSize < 1)
		throw std::invalid_argument("chunkSize < 1");
}

//###########################################################################

std::unique_ptr<Unit> UnitPool::acquireUnit()
{
	if (m_pool.empty())
	{
		for (int i = 0; i < m_chunkSize; ++i)
		{
			m_pool.emplace(std::make_unique<Unit>());
		}
	}


	auto unit = std::move(m_pool.front());
	m_pool.pop();


	return unit;
}


void UnitPool::releaseUnit(std::unique_ptr<Unit>&& unit)
{
	m_pool.emplace(std::move(unit));
}

