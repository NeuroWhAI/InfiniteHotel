#include "Hotel.h"

#include <algorithm>
#include <ctime>

#include "Interpreter.h"

#include "HotelStat.h"
#include "Unit.h"
#include "Gene.h"
#include "UnitPool.h"




Hotel::Hotel()
	: m_randEngine(static_cast<unsigned int>(std::time(nullptr)))
	, m_mutationDist(0, 128)

	, m_hotelStat(std::make_unique<HotelStat>())

	, m_unitPool(std::make_unique<UnitPool>())
	
	, m_worldEnergy(0)
	, m_lockedEnergy(0)
	, m_epoch(1)
	, m_time(0), m_endTime(4096)
{
	
}


Hotel::~Hotel() = default;

//###########################################################################

void Hotel::writeTo(std::ostream& osr) const
{
	using std::endl;


	m_hotelStat->writeTo(osr);


	osr << m_worldEnergy << endl;
	osr << m_lockedEnergy << endl;

	osr << m_roomList.size() << endl;
	for (const auto& room : m_roomList)
	{
		if (room == nullptr)
		{
			osr << '0' << endl;
		}
		else
		{
			osr << '1' << endl;

			room->writeTo(osr);
		}
	}

	osr << m_epoch << endl;
	osr << m_time << ' ' << m_endTime << endl;


	osr << m_scoreRanking.size() << endl;
	for (const auto& index : m_scoreRanking)
	{
		osr << index << ' ';
	}
	osr << endl;
}


void Hotel::readFrom(std::istream& isr)
{
	m_roomList.clear();
	m_scoreRanking.clear();


	m_hotelStat->readFrom(isr);


	isr >> m_worldEnergy;
	isr >> m_lockedEnergy;

	size_t roomCount = 0;
	isr >> roomCount;

	for (size_t i = 0; i < roomCount; ++i)
	{
		int isExists = 0;
		isr >> isExists;

		if (isExists == 0)
		{
			m_roomList.emplace_back(nullptr);
		}
		else
		{
			auto unit = m_unitPool->acquireUnit();
			unit->readFrom(isr, &m_roomList);

			m_roomList.emplace_back(std::move(unit));
		}
	}

	isr >> m_epoch;
	isr >> m_time >> m_endTime;


	size_t scoreRankSize = 0;
	isr >> scoreRankSize;

	m_scoreRanking.resize(scoreRankSize);

	for (size_t i = 0; i < scoreRankSize; ++i)
	{
		size_t index = 0;
		isr >> index;

		m_scoreRanking[i] = index;
	}
}

//###########################################################################

void Hotel::initialize(size_t firstUnitCount, size_t timePerEpoch)
{
	m_hotelStat->reset();


	m_worldEnergy = 0.0;
	m_lockedEnergy = 0.0;
	m_roomList.clear();
	m_epoch = 1;
	m_time = 0;
	m_endTime = timePerEpoch;


	// ���Ƿ� ���� �⺻������ ���� ������ ������ ����.
	for (size_t i = 0; i < 3; ++i)
	{
		Gene testGene = {
			2, 6, 13, 16, 7, 12, 15, 0, 2, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7
		};

		m_worldEnergy += testGene.getEnergy();

		createUnit(testGene);
	}
	if (firstUnitCount < 3)
		firstUnitCount = 0;
	else
		firstUnitCount -= 3;

	for (size_t i = 0; i < firstUnitCount; ++i)
	{
		// ���� ������ ����
		Gene gene;
		gene.initializeRandomly(m_randEngine, Interpreter::CMD_COUNT);
		
		// �ʱ� ������ ä��.
		m_worldEnergy += gene.getEnergy();

		// ���� ����
		createUnit(gene);
	}


	m_worldEnergy *= 2.0;


	updateRanking();
}


void Hotel::update()
{
	// ���� �۾� ����
	updateState();


	// ������ ��� ����
	updateCommunication();


	// �ð� ����
	bool timeover = updateTime();
	if (timeover)
	{
		// ��ŷ ����
		updateRanking();


		// ���� ����
		updateEpoch();
	}


	// ��� ����
	updateLove();


	// ���� ������ ����
	updateEnergy();
}

//###########################################################################

void Hotel::updateState()
{
	size_t count = 0;

	for (auto& room : m_roomList)
	{
		// �� ���� ������ �� �����Ƿ� Ȯ��.
		if (room == nullptr)
			continue;
		++count;


		m_hotelStat->updateMaxUnitEnergy(room->getEnergy());
		m_hotelStat->updateUnitScore(room->getScore());


		if (room->isDead())
		{
			m_hotelStat->addDeath();
			m_hotelStat->decreaseGeneCount(room->getGene());


			// ������ ȯ��
			m_lockedEnergy += room->getEnergy() + room->getUsedEnergy();


			// ���� ����
			m_unitPool->releaseUnit(std::move(room));
			room = nullptr;
		}
		else
		{
			room->updateInterpreter();
		}
	}


	m_hotelStat->updateUnitCount(count);
}


void Hotel::updateCommunication()
{
	for (auto& room : m_roomList)
	{
		// �� ���� ������ �� �����Ƿ� Ȯ��.
		if (room == nullptr)
			continue;


		room->updateCommunication();
	}
}


bool Hotel::updateTime()
{
	++m_time;

	if (m_time >= m_endTime)
	{
		m_time = 0;


		return true;
	}


	return false;
}


void Hotel::updateRanking()
{
	m_scoreRanking.clear();

	// ������ �����ϴ� ���� ��ȣ�� ����
	const size_t roomCount = m_roomList.size();
	for (size_t r = 0; r < roomCount; ++r)
	{
		if (m_roomList[r] != nullptr)
		{
			m_scoreRanking.emplace_back(r);
		}
	}

	// ���� ������ �������� ����
	std::sort(m_scoreRanking.begin(), m_scoreRanking.end(),
		[&roomList = m_roomList](size_t left, size_t right)
	{
		return roomList[left]->getScore() > roomList[right]->getScore();
	});
}


void Hotel::updateLove()
{
	if (m_scoreRanking.size() >= 3)
	{
		// ���踦 ���� �ļ��� ����
		size_t tryCount = 0;
		while (tryCount < m_scoreRanking.size())
		{
			++tryCount;


			// ������ �������� ���õ� Ȯ���� ���� ��.
			std::normal_distribution<> indexDist{
				0, static_cast<double>(m_scoreRanking.size() / 2)
			};

			double normalIndex = std::abs(indexDist(m_randEngine));
			const size_t index = static_cast<size_t>(normalIndex) % m_scoreRanking.size();

			const size_t loverIndex = m_scoreRanking[index];
			Unit* const lover = m_roomList[loverIndex].get();

			if (lover == nullptr)
				continue;


			size_t lovedIndex = lover->getLoveUnitIndex();

			// �ڱ�ְ� �ƴϰ�
			// ��밡 �����Ѵٸ�
			if (lovedIndex != lover->getRoomIndex()
				&& lovedIndex < m_roomList.size()
				&& m_roomList[lovedIndex] != nullptr)
			{
				auto& lovedUnit = m_roomList[lovedIndex];


				// ������ ����
				Gene newGene = lover->getGene();
				newGene.combine(lovedUnit->getGene(), m_randEngine);


				// ��������
				bool wasMutated = false;
				if (m_mutationDist(m_randEngine) == 0)
				{
					newGene.mutate(m_randEngine, Interpreter::CMD_COUNT);

					wasMutated = true;
				}


				// �ʿ��� ������ ���
				double needEnergy = newGene.getEnergy();
				double needFromWorld = needEnergy * 0.8;
				double needFromParent = (needEnergy - needFromWorld) / 2;

				// ���� �������� ����ϸ� ������ �����ϰ�
				// �����ϸ� �ߴ�
				if (m_worldEnergy >= needFromWorld)
				{
					// �θ��� �������� ����ϸ�
					if (lover->getEnergy() > needFromParent
						&& lovedUnit->getEnergy() > needFromParent)
					{
						// �θ� ������ �Ҹ�
						lover->addEnergy(-needFromParent);
						lovedUnit->addEnergy(-needFromParent);

						// ���� ������ �Ҹ�
						m_worldEnergy -= needFromWorld;

						if (wasMutated)
						{
							m_hotelStat->addMutationCount();
						}

						// ���� ����
						createUnit(newGene);
					}
				}
				else
				{
					break;
				}
			}
		}
	}
	else
	{
		initialize(m_roomList.size(), m_endTime);
	}
}


void Hotel::updateEpoch()
{
	// ���� ����
	++m_epoch;


	m_hotelStat->updateEpoch(m_epoch);
}


void Hotel::updateEnergy()
{
	const double returnSpeed = std::max(64.0, m_scoreRanking.size() * 1.0);

	if (m_lockedEnergy >= returnSpeed)
	{
		m_lockedEnergy -= returnSpeed;
		m_worldEnergy += returnSpeed;
	}
}

//###########################################################################

size_t Hotel::createUnit(const Gene& gene)
{
	m_hotelStat->addBirth();
	m_hotelStat->increaseGeneCount(gene);


	const size_t roomCount = m_roomList.size();
	for (size_t r = 0; r < roomCount; ++r)
	{
		// �� ���� �ִٸ� �ű� ������.
		if (m_roomList[r] == nullptr)
		{
			// �����Ǳ����� ��ŷ�� �����Ͽ� �߸��� ��ŷ�� �����Ǵ°� �����Ѵ�.
			const size_t unitCount = m_scoreRanking.size();
			for (size_t u = 0; u < unitCount; ++u)
			{
				if (m_scoreRanking[u] == r)
				{
					m_scoreRanking.erase(m_scoreRanking.begin() + u);

					break;
				}
			}


			m_roomList[r] = m_unitPool->acquireUnit();
			m_roomList[r]->initialize(gene, r, &m_roomList);

			
			return r;
		}
	}


	// ����� ���ٸ� ���� �ϳ� ����.
	m_roomList.emplace_back(m_unitPool->acquireUnit());
	m_roomList[m_roomList.size() - 1]->initialize(gene, m_roomList.size(),
		&m_roomList);


	return m_roomList.size() - 1;
}

//###########################################################################

void Hotel::addEnergy(double energy)
{
	m_worldEnergy += energy;
}

