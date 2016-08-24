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

void HotelStat::writeTo(std::ostream& osr) const
{
	using std::endl;


	osr << m_epoch << endl;


	osr << m_maxUnitCount << endl;
	osr << m_unitCount << endl;
	osr << m_birth << endl;
	osr << m_death << endl;
	osr << m_maxUnitEnergy << endl;
	osr << m_maxUnitScore << endl;
	osr << m_minUnitScore << endl;
	osr << m_mutationCount << endl;

	osr << m_geneInfo.size() << endl;
	for (const auto& geneInfo : m_geneInfo)
	{
		auto& gene = geneInfo.first;
		auto& info = geneInfo.second;

		gene.writeTo(osr);

		osr << info.count << endl;
		osr << info.maxCount << endl;
		osr << info.firstEpoch << endl;
		osr << info.endEpoch << endl;
	}

	osr << m_longestEpoch << endl;
}


void HotelStat::readFrom(std::istream& isr)
{
	m_geneInfo.clear();


	isr >> m_epoch;


	isr >> m_maxUnitCount;
	isr >> m_unitCount;
	isr >> m_birth;
	isr >> m_death;
	isr >> m_maxUnitEnergy;
	isr >> m_maxUnitScore;
	isr >> m_minUnitScore;
	isr >> m_mutationCount;

	size_t infoCount = 0;
	isr >> infoCount;

	for (size_t i = 0; i < infoCount; ++i)
	{
		Gene gene;
		gene.readFrom(isr);

		GeneInfo info;
		isr >> info.count;
		isr >> info.maxCount;
		isr >> info.firstEpoch;
		isr >> info.endEpoch;

		m_geneInfo.insert(std::make_pair(std::move(gene), info));
	}

	isr >> m_longestEpoch;
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
	m_geneInfo.clear();
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
	auto it = m_geneInfo.find(gene);

	if (it == m_geneInfo.end())
	{
		it = m_geneInfo.insert(std::make_pair(gene, GeneInfo()));


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
	auto it = m_geneInfo.find(gene);

	if (it != m_geneInfo.end())
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

			// 유전자를 저장할 가치가 없으면 파기한다.
			if (info.maxCount <= 1)
			{
				m_geneInfo.erase(it);
			}
		}
	}
}


auto HotelStat::getGeneInfoMap() const -> const GeneInfoMap&
{
	return m_geneInfo;
}


size_t HotelStat::getLongestGeneEpoch() const
{
	return m_longestEpoch;
}

