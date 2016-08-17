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
		// 랜덤 유전자 생성
		Gene gene;
		gene.initializeRandomly(m_randEngine, Interpreter::CMD_COUNT);
		
		// 초기 에너지 채움.
		m_totalEnergy += gene.getEnergy();

		// 유닛 생성
		createUnit(gene);
	}


	updateRanking();
}


void Hotel::update()
{
	// 개인 작업 실행
	updateState();


	// 데이터 통신 실행
	updateCommunication();


	// 시간 갱신
	bool timeover = updateTime();
	if (timeover)
	{
		// 랭킹 갱신
		updateRanking();


		// 세대 갱신
		updateEpoch();
	}


	// 사랑 갱신
	updateLove();
}

//###########################################################################

void Hotel::updateState()
{
	size_t count = 0;

	for (auto& room : m_roomList)
	{
		// 빈 방이 존재할 수 있으므로 확인.
		if (room == nullptr)
			continue;
		++count;


		m_hotelStat->updateMaxUnitEnergy(room->getEnergy());
		m_hotelStat->updateUnitScore(room->getScore());


		if (room->isDead())
		{
			// 에너지 환원
			m_totalEnergy += room->getGene().getEnergy();

			// 유닛 삭제
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
		// 빈 방이 존재할 수 있으므로 확인.
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

	// 유닛이 존재하는 방의 번호만 저장
	const size_t roomCount = m_roomList.size();
	for (size_t r = 0; r < roomCount; ++r)
	{
		if (m_roomList[r] != nullptr)
		{
			m_scoreRanking.emplace_back(r);
		}
	}

	// 받은 점수로 오름차순 정렬
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
		// 교배를 통해 후세대 생성
		size_t tryCount = 0;
		while (tryCount < m_scoreRanking.size())
		{
			++tryCount;


			// 점수가 높을수록 선택될 확률이 높게 함.
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

			// 자기애가 아니고
			// 상대가 존재한다면
			if (lovedIndex != lover->getRoomIndex()
				&& lovedIndex < m_roomList.size()
				&& m_roomList[lovedIndex] != nullptr)
			{
				auto& lovedUnit = m_roomList[lovedIndex];


				// 유전자 배합
				Gene newGene = lover->getGene();
				newGene.combine(lovedUnit->getGene(), m_randEngine);


				// 돌연변이
				bool wasMutated = false;
				if (m_mutationDist(m_randEngine) == 0)
				{
					newGene.mutate(m_randEngine, Interpreter::CMD_COUNT);

					wasMutated = true;
				}


				// 필요한 에너지 계산
				double needEnergy = newGene.getEnergy();
				double needFromWorld = needEnergy * 0.8;
				double needFromParent = (needEnergy - needFromWorld) / 2;

				// 월드 에너지가 충분하면 유닛을 생성하고
				// 부족하면 중단
				if (m_totalEnergy >= needFromWorld)
				{
					// 부모의 에너지가 충분하면
					if (lover->getEnergy() > needFromParent
						&& lovedUnit->getEnergy() > needFromParent)
					{
						// 부모 에너지 소모
						lover->addEnergy(-needFromParent);
						lovedUnit->addEnergy(-needFromParent);

						// 월드 에너지 소모
						m_totalEnergy -= needFromWorld;

						if (wasMutated)
						{
							m_hotelStat->addMutationCount();
						}

						// 유닛 생성
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
	// 세대 증가
	++m_epoch;
}

//###########################################################################

size_t Hotel::createUnit(const Gene& gene)
{
	m_hotelStat->addBirth();


	const size_t roomCount = m_roomList.size();
	for (size_t r = 0; r < roomCount; ++r)
	{
		// 빈 방이 있다면 거기 생성함.
		if (m_roomList[r] == nullptr)
		{
			// 생성되기전에 랭킹을 갱신하여 잘못된 랭킹이 유지되는걸 방지한다.
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


	// 빈방이 없다면 방을 하나 만듬.
	m_roomList.emplace_back(std::make_unique<Unit>(gene, m_roomList.size(),
		m_roomList));


	return m_roomList.size() - 1;
}

