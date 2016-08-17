#include "Hotel.h"

#include <algorithm>
#include <ctime>

#include "Interpreter.h"

#include "HotelStat.h"
#include "Unit.h"
#include "Gene.h"




Hotel::Hotel()
	: m_randEngine(static_cast<unsigned int>(std::time(nullptr)))
	, m_mutationDist(0, 128)

	, m_hotelStat(std::make_unique<HotelStat>())
	
	, m_totalEnergy(0)
	, m_epoch(1)
	, m_time(0), m_endTime(4096)
{

}


Hotel::~Hotel() = default;

//###########################################################################

void Hotel::initialize(size_t firstUnitCount, size_t timePerEpoch)
{
	m_hotelStat->reset();

	m_totalEnergy = 0.0;
	m_roomList.clear();
	m_epoch = 1;
	m_time = 0;
	m_endTime = timePerEpoch;

	for (size_t i = 0; i < 3; ++i)
	{
		Gene testGene = {
			2, 6, 13, 16, 7, 12, 15, 0, 2, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7
		};

		m_totalEnergy += testGene.getEnergy();

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
		m_totalEnergy += gene.getEnergy();

		// ���� ����
		createUnit(gene);
	}


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
			// ������ ȯ��
			m_totalEnergy += room->getGene().getEnergy();

			// ���� ����
			room = nullptr;


			m_hotelStat->addDeath();
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
				0, static_cast<double>(m_scoreRanking.size())
			};

			double normalIndex = std::abs(indexDist(m_randEngine));
			if (normalIndex >= m_scoreRanking.size())
				normalIndex = 0;
			const size_t index = static_cast<size_t>(normalIndex);

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
				if (m_totalEnergy >= needFromWorld)
				{
					// �θ��� �������� ����ϸ�
					if (lover->getEnergy() > needFromParent
						&& lovedUnit->getEnergy() > needFromParent)
					{
						// �θ� ������ �Ҹ�
						lover->addEnergy(-needFromParent);
						lovedUnit->addEnergy(-needFromParent);

						// ���� ������ �Ҹ�
						m_totalEnergy -= needFromWorld;

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
}

//###########################################################################

size_t Hotel::createUnit(const Gene& gene)
{
	m_hotelStat->addBirth();


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


			m_roomList[r] = std::make_unique<Unit>(gene, r,
				m_roomList);

			
			return r;
		}
	}


	// ����� ���ٸ� ���� �ϳ� ����.
	m_roomList.emplace_back(std::make_unique<Unit>(gene, m_roomList.size(),
		m_roomList));


	return m_roomList.size() - 1;
}

