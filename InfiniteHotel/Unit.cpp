#include "Unit.h"

#include <numeric>
#include <algorithm>




Unit::Unit()
	: m_roomList(nullptr)
	
	, m_energy(0)
	, m_usedEnergy(0)
	
	, m_interpreter(std::make_unique<Interpreter>())

	, m_myIndex(0)
	, m_targetUnitIndex(0)
	, m_inputList(1, 0)
	, m_outputList(1, 0)
	, m_score(0)
	, m_nextScore(0)
	, m_targetScore(0)
	, m_loveUnitIndex(0)
{
	
}

//###########################################################################

void Unit::initialize(const Gene& gene, const size_t myIndex,
	const std::vector<std::unique_ptr<Unit>>* roomList)
{
	m_roomList = roomList;

	m_gene = gene;
	m_energy = gene.getEnergy();
	m_usedEnergy = 0;

	m_memory.resize(gene.getLength(), 0);
	m_interpreter->initialize(&m_gene.getCode(),
		&m_memory, this);

	m_myIndex = myIndex;
	m_targetUnitIndex = myIndex;
	m_inputList.clear();
	m_inputList.resize(1, 0);
	m_outputList.clear();
	m_outputList.resize(1, 0);
	m_score = 0;
	m_nextScore = 0;
	m_targetScore = 0;
	m_loveUnitIndex = myIndex;
}

//###########################################################################

bool Unit::isDead() const
{
	return (m_interpreter->isEnd() || m_energy <= 0.0);
}


void Unit::updateInterpreter()
{
	if (m_interpreter->isEnd() == false)
	{
		m_interpreter->update();
	}
}


void Unit::updateCommunication()
{
	const auto& roomList = *m_roomList;


	// 점수 갱신
	m_score = m_nextScore;

	// 통신 및 평가 진행
	if (m_targetUnitIndex < roomList.size()
		&& roomList[m_targetUnitIndex] != nullptr)
	{
		auto& targetUnit = roomList[m_targetUnitIndex];


		// 통신
		size_t channel = static_cast<size_t>(std::abs(m_interpreter->getChannel()));

		if (channel >= m_outputList.size())
		{
			m_outputList.resize(channel + 1, 0);
		}

		targetUnit->input(channel, m_outputList[channel]);
		m_outputList[channel] = 0;


		// 평가
		if (m_targetUnitIndex != m_myIndex)
		{
			targetUnit->vote(m_targetScore);
		}
		m_targetScore = 0;
	}
}

//###########################################################################

const Gene& Unit::getGene() const
{
	return m_gene;
}


double Unit::getEnergy() const
{
	return m_energy;
}


void Unit::addEnergy(double energy)
{
	m_energy += energy;
}


double Unit::getUsedEnergy() const
{
	return m_usedEnergy;
}

//###########################################################################

void Unit::useEnergy(double energy)
{
	m_energy -= energy;
	m_usedEnergy += energy;
}

//###########################################################################

bool Unit::whenIncreasePtr()
{
	useEnergy(0.2);


	return true;
}


bool Unit::whenDecreasePtr()
{
	useEnergy(0.2);


	return true;
}


bool Unit::whenIncreaseData()
{
	useEnergy(1.0); // 1에서 변경하지 말것.


	return true;
}


bool Unit::whenDecreaseData()
{
	useEnergy(1.0); // 1에서 변경하지 말것.


	return true;
}


bool Unit::whenWriteData(char data)
{
	useEnergy(4.0);


	size_t channel = static_cast<size_t>(std::abs(m_interpreter->getChannel()));

	if (channel >= m_outputList.size())
	{
		m_outputList.resize(channel + 1, 0);
	}


	m_outputList[channel] = data;


	// 쓴 데이터만큼 에너지를 추가 소모
	// useEnergy를 사용하지 않는 이유는 이 값(에너지)이 외부로 나갈 예정이라서.
	m_energy -= std::abs(data);


	return true;
}


bool Unit::whenReadData(char& outData)
{
	useEnergy(4.0);


	// NOTE: outData의 이전 값을 저장하지 않고
	// 에너지 손실의 가능성을 그대로 두는건
	// 공격의 가능성으로 사용되길 원하기 때문임.


	size_t channel = static_cast<size_t>(std::abs(m_interpreter->getChannel()));

	if (channel < m_inputList.size())
	{
		outData = m_inputList[channel];
		m_inputList[channel] = 0;


		// 데이터를 에너지로서 흡수
		m_energy += std::abs(outData);
	}
	else
	{
		outData = 0;
	}


	return true;
}


bool Unit::whenJumpIfZero()
{
	useEnergy(0.2);


	return true;
}


bool Unit::whenJumpIfNotZero()
{
	useEnergy(0.2);


	return true;
}


bool Unit::whenWriteRegister(char delta)
{
	useEnergy(0.4 + std::abs(delta));


	return true;
}


bool Unit::whenReadRegister(char delta)
{
	useEnergy(0.4 + std::abs(delta));


	return true;
}


bool Unit::whenWriteChannel(char delta)
{
	useEnergy(0.4 + std::abs(delta));


	return true;
}


bool Unit::whenReadChannel(char delta)
{
	useEnergy(0.4 + std::abs(delta));


	return true;
}


bool Unit::whenMoveTargetLeft()
{
	useEnergy(8.0);


	if (m_targetUnitIndex > 0)
	{
		--m_targetUnitIndex;
	}


	return true;
}


bool Unit::whenMoveTargetRight()
{
	useEnergy(8.0);


	++m_targetUnitIndex;


	return true;
}


bool Unit::whenVote(char how)
{
	useEnergy(std::abs(how));


	m_targetScore += how;


	return true;
}


bool Unit::whenLove()
{
	useEnergy(16.0);


	m_loveUnitIndex = m_targetUnitIndex;


	return true;
}


bool Unit::whenReadTargetEnergy(char& outData)
{
	const auto& roomList = *m_roomList;


	useEnergy(2.0);


	const char oldData = outData;


	outData = 0;


	if (m_targetUnitIndex < roomList.size()
		&& roomList[m_targetUnitIndex] != nullptr)
	{
		auto& targetUnit = roomList[m_targetUnitIndex];


		constexpr auto maxData = std::numeric_limits<char>::max();
		constexpr auto minData = std::numeric_limits<char>::min();


		if (m_energy != 0.0)
		{
			double energyRate = targetUnit->getEnergy() / m_energy;
			energyRate *= maxData;

			if (energyRate > maxData)
			{
				outData = maxData;
			}
			else if (energyRate < minData)
			{
				outData = minData;
			}
			else
			{
				outData = static_cast<char>(energyRate);
			}
		}
	}


	useEnergy(std::abs(oldData - outData));


	return true;
}


bool Unit::whenReadTargetScore(char& outData)
{
	const auto& roomList = *m_roomList;


	useEnergy(2.0);


	const char oldData = outData;


	outData = 0;


	if (m_targetUnitIndex < roomList.size()
		&& roomList[m_targetUnitIndex] != nullptr)
	{
		auto& targetUnit = roomList[m_targetUnitIndex];


		constexpr auto maxData = std::numeric_limits<char>::max();
		constexpr auto minData = std::numeric_limits<char>::min();


		if (m_score == 0.0)
		{
			outData = maxData;
		}
		else
		{
			double scoreRate = targetUnit->getScore() / m_score;
			scoreRate *= maxData;

			if (scoreRate > maxData)
			{
				outData = maxData;
			}
			else if (scoreRate < minData)
			{
				outData = minData;
			}
			else
			{
				outData = static_cast<char>(scoreRate);
			}
		}
	}
	

	useEnergy(std::abs(oldData - outData));


	return true;
}

//###########################################################################

void Unit::input(size_t channel, char data)
{
	if (channel >= m_inputList.size())
	{
		m_inputList.resize(channel + 1, 0);
	}


	m_inputList[channel] = data;
}


void Unit::vote(double deltaScore)
{
	m_nextScore += deltaScore;
}


size_t Unit::getRoomIndex() const
{
	return m_myIndex;
}


double Unit::getScore() const
{
	return m_score;
}


size_t Unit::getLoveUnitIndex() const
{
	return m_loveUnitIndex;	
}

