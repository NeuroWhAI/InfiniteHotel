#include "Unit.h"

#include <numeric>




Unit::Unit(const Gene& gene, const size_t myIndex,
	const std::vector<std::unique_ptr<Unit>>& roomList)
	: m_roomList(roomList)
	
	, m_gene(gene)
	, m_energy(gene.getEnergy())

	, m_memory(gene.getCode().size(), 0)

	, m_myIndex(myIndex)
	, m_targetUnitIndex(myIndex)
	, m_inputList(1, 0)
	, m_outputList(1, 0)
	, m_score(0)
	, m_nextScore(0)
	, m_targetScore(0)
	, m_loveUnitIndex(myIndex)
{
	m_interpreter = std::make_unique<Interpreter>(m_gene.getCode(),
		m_memory, *this);
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
	// 점수 갱신
	m_score = m_nextScore;

	// 통신 및 평가 진행
	if (m_targetUnitIndex < m_roomList.size()
		&& m_roomList[m_targetUnitIndex] != nullptr)
	{
		auto& targetUnit = m_roomList[m_targetUnitIndex];


		// 통신
		size_t channel = static_cast<size_t>(std::abs(m_interpreter->getChannel()));

		if (channel >= m_outputList.size())
		{
			m_outputList.resize(channel + 1, 0);
		}

		targetUnit->input(channel, m_outputList[channel]);


		// 평가
		targetUnit->vote(m_targetScore);
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

//###########################################################################

bool Unit::whenIncreasePtr()
{
	m_energy -= 0.2;


	return true;
}


bool Unit::whenDecreasePtr()
{
	m_energy -= 0.2;


	return true;
}


bool Unit::whenIncreaseData()
{
	m_energy -= 1;


	return true;
}


bool Unit::whenDecreaseData()
{
	m_energy -= 1;


	return true;
}


bool Unit::whenWriteData(char data)
{
	m_energy -= 4;


	size_t channel = static_cast<size_t>(std::abs(m_interpreter->getChannel()));

	if (channel >= m_outputList.size())
	{
		m_outputList.resize(channel + 1, 0);
	}


	m_outputList[channel] = data;


	// 쓴 데이터만큼 에너지를 추가 소모
	m_energy -= std::abs(data);


	return true;
}


bool Unit::whenReadData(char& outData)
{
	m_energy -= 4;


	size_t channel = static_cast<size_t>(std::abs(m_interpreter->getChannel()));

	if (channel < m_inputList.size())
	{
		outData = m_inputList[channel];


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
	m_energy -= 0.2;


	return true;
}


bool Unit::whenJumpIfNotZero()
{
	m_energy -= 0.2;


	return true;
}


bool Unit::whenWriteRegister()
{
	m_energy -= 0.4;


	return true;
}


bool Unit::whenReadRegister()
{
	m_energy -= 0.4;


	return true;
}


bool Unit::whenWriteChannel()
{
	m_energy -= 0.4;


	return true;
}


bool Unit::whenReadChannel()
{
	m_energy -= 0.4;


	return true;
}


bool Unit::whenMoveTargetLeft()
{
	m_energy -= 8;


	if (m_targetUnitIndex > 0)
	{
		--m_targetUnitIndex;
	}


	return true;
}


bool Unit::whenMoveTargetRight()
{
	m_energy -= 8;


	++m_targetUnitIndex;


	return true;
}


bool Unit::whenVote(char how)
{
	m_energy -= std::abs(how);


	m_targetScore += how;


	return true;
}


bool Unit::whenLove()
{
	m_energy -= 16;


	m_loveUnitIndex = m_targetUnitIndex;


	return true;
}


bool Unit::whenReadTargetEnergy(char& outData)
{
	m_energy -= 2;


	outData = 0;


	if (m_targetUnitIndex < m_roomList.size()
		&& m_roomList[m_targetUnitIndex] != nullptr)
	{
		auto& targetUnit = m_roomList[m_targetUnitIndex];


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


	return true;
}


bool Unit::whenReadTargetScore(char& outData)
{
	m_energy -= 2;


	outData = 0;


	if (m_targetUnitIndex < m_roomList.size()
		&& m_roomList[m_targetUnitIndex] != nullptr)
	{
		auto& targetUnit = m_roomList[m_targetUnitIndex];


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

