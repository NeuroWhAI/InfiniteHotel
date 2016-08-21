#include "HotelStat.h"




HotelStat::HotelStat()
	: m_epoch(1)
	
	, m_maxUnitCount(0)
	, m_unitCount(0)
	, m_birth(0)
	, m_death(0)
	, m_maxUnitEnergy(-1)
	, m_maxUnitScore(-1)
	, m_minUnitScore(1)
	, m_mutationCount(0)
	, m_longestEpoch(0)
{

}

//###########################################################################

void HotelStat::reset()
{
	m_epoch = 1;

	m_maxUnitCount = 0;
	m_unitCount = 0;
	m_birth = 0;
	m_death = 0;
	m_maxUnitEnergy = -1;
	m_maxUnitScore = -1;
	m_minUnitScore = 1;
	m_mutationCount = 0;
	m_geneCount.clear();
	m_longestEpoch = 0;
}

//###########################################################################

void HotelStat::updateEpoch(size_t epoch)
{
	m_epoch = epoch;
}

//###########################################################################

void HotelStat::updateUnitCount(size_t unitCount)
{
	m_unitCount = unitCount;

	if (unitCount > m_maxUnitCount)
		m_maxUnitCount = unitCount;
}


size_t HotelStat::getMaxUnitCount() const
{
	return m_maxUnitCount;
}


size_t HotelStat::getUnitCount() const
{
	return m_unitCount;
}


void HotelStat::addBirth(size_t delta)
{
	m_birth += delta;
}


size_t HotelStat::getBirth() const
{
	return m_birth;
}


void HotelStat::addDeath(size_t delta)
{
	m_death += delta;
}


size_t HotelStat::getDeath() const
{
	return m_death;
}


void HotelStat::updateMaxUnitEnergy(double unitEnergy)
{
	if (unitEnergy > m_maxUnitEnergy)
		m_maxUnitEnergy = unitEnergy;
}


double HotelStat::getMaxUnitEnergy() const
{
	return m_maxUnitEnergy;
}


void HotelStat::updateUnitScore(double unitScore)
{
	if (unitScore > m_maxUnitScore)
		m_maxUnitScore = unitScore;

	if (unitScore < m_minUnitScore)
		m_minUnitScore = unitScore;
}


double HotelStat::getMaxUnitScore() const
{
	return m_maxUnitScore;
}


double HotelStat::getMinUnitScore() const
{
	return m_minUnitScore;
}


void HotelStat::addMutationCount(size_t delta)
{
	m_mutationCount += delta;
}


size_t HotelStat::getMutationCount() const
{
	return m_mutationCount;
}


void HotelStat::increaseGeneCount(const Gene& gene)
{
	auto it = m_geneCount.find(gene);

	if (it == m_geneCount.end())
	{
		it = m_geneCount.insert(std::make_pair(gene, GeneInfo()));


		auto& info = it->second;

		info.firstEpoch = m_epoch;
	}
	
	
	auto& info = it->second;

	++info.count;
	if (info.count > info.maxCount)
		info.maxCount = info.count;
}


void HotelStat::decreaseGeneCount(const Gene& gene)
{
	auto it = m_geneCount.find(gene);

	if (it != m_geneCount.end())
	{
		auto& info = it->second;

		--info.count;
		if (info.count <= 0)
		{
			info.count = 0;
			info.endEpoch = m_epoch;

			size_t lifespan = m_epoch - info.firstEpoch;
			if (lifespan > m_longestEpoch)
			{
				m_longestEpoch = lifespan;
			}
		}
	}
}


auto HotelStat::getGeneInfoMap() const -> const GeneInfoMap&
{
	return m_geneCount;
}


size_t HotelStat::getLongestGeneEpoch() const
{
	return m_longestEpoch;
}

