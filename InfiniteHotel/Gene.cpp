#include "Gene.h"

#include <algorithm>
#include <numeric>




Gene::Gene()
{

}


Gene::Gene(const std::initializer_list<char>& list)
	: m_code(list)
{

}

//###########################################################################

void Gene::writeTo(std::ostream& osr) const
{
	using std::endl;


	osr << m_code.size() << endl;
	for (auto& digit : m_code)
	{
		osr << static_cast<int>(digit) << ' ';
	}
	osr << endl;
}


void Gene::readFrom(std::istream& isr)
{
	size_t length = 0;
	isr >> length;

	m_code.resize(length);

	for (size_t i = 0; i < length; ++i)
	{
		int digit = 0;
		isr >> digit;

		m_code[i] = static_cast<char>(digit);
	}
}

//###########################################################################

void Gene::initializeRandomly(std::mt19937& engine, int cmdSetCount)
{
	m_code.clear();


	std::uniform_int_distribution<size_t> sizeDist{ 8, 512 };
	std::uniform_int_distribution<> cmdDist{ 0, cmdSetCount - 1 };


	size_t length = sizeDist(engine);
	m_code.resize(length);

	for (size_t i = 0; i < length; ++i)
	{
		m_code[i] = static_cast<char>(cmdDist(engine));
	}
}

//###########################################################################

size_t Gene::getLength() const
{
	return m_code.size();
}


const std::vector<char>& Gene::getCode() const
{
	return m_code;
}


double Gene::getEnergy() const
{
	return m_code.size() * 64.0;
}

//###########################################################################

void Gene::combine(const Gene& other, std::mt19937& engine)
{
	size_t minLength = std::min(other.getCode().size(), m_code.size());


	std::uniform_int_distribution<> flagDist{ 0, 1 };


	if (minLength > 0)
	{
		std::uniform_int_distribution<size_t> jumpDist{ 1, minLength };


		const Gene* current = (flagDist(engine) == 0) ? this : &other;

		size_t jumpSize = jumpDist(engine);
		size_t jumpGage = 0;

		for (size_t i = 0; i < minLength; ++i)
		{
			if (jumpGage < jumpSize)
			{
				++jumpGage;


				m_code[i] = current->getCode()[i];
			}
			else
			{
				current = (flagDist(engine) == 0) ? this : &other;

				jumpSize = jumpDist(engine);
				jumpGage = 0;
			}
		}
	}


	if (m_code.size() < other.getCode().size()
		&& flagDist(engine) == 0)
	{
		m_code.insert(m_code.end(), other.getCode().begin() + minLength,
			other.getCode().end());
	}
}


void Gene::mutate(std::mt19937& engine, int cmdSetCount)
{
	if (m_code.size() > 0)
	{
		std::uniform_int_distribution<> rateDist{ 0, 4 };


		// 각 자리별 변이
		std::uniform_int_distribution<size_t> indexDist{ 0, m_code.size() - 1 };
		std::uniform_int_distribution<> cmdDist{ 0, cmdSetCount - 1 };

		size_t repeatCount = indexDist(engine);
		for (size_t i = 0; i <= repeatCount; ++i)
		{
			size_t index = indexDist(engine);

			m_code[index] = static_cast<char>(cmdDist(engine));
		}


		if (rateDist(engine) == 0)
		{
			// 랜덤 구간 역전
			size_t beginReverse = indexDist(engine);
			size_t endReverse = beginReverse + indexDist(engine);

			if (endReverse <= m_code.size())
			{
				std::reverse(m_code.begin() + beginReverse,
					m_code.begin() + endReverse);
			}
		}


		if (rateDist(engine) == 0)
		{
			// 랜덤 구간 복제 삽입
			size_t beginInsert = indexDist(engine);
			size_t endInsert = beginInsert + indexDist(engine);

			if (endInsert <= m_code.size())
			{
				for (size_t i = beginInsert; i < endInsert; ++i)
				{
					m_code.insert(m_code.begin() + (i - beginInsert + endInsert),
						m_code[i]);
				}
			}
		}


		if (rateDist(engine) == 0)
		{
			// 랜덤 구간 삭제
			size_t beginDelete = indexDist(engine);
			size_t endDelete = beginDelete + indexDist(engine);

			if (endDelete < m_code.size())
			{
				m_code.erase(m_code.begin() + beginDelete,
					m_code.begin() + endDelete);
			}
		}
	}
}

//###########################################################################

bool Gene::operator== (const Gene& right) const
{
	auto& otherCode = right.getCode();

	if (m_code.size() == otherCode.size())
	{
		const size_t length = m_code.size();

		for (size_t i = 0; i < length; ++i)
		{
			if (m_code[i] != otherCode[i])
				return false;
		}


		return true;
	}


	return false;
}


bool Gene::operator!= (const Gene& right) const
{
	return !this->operator==(right);
}

//###########################################################################

int Gene::getSum() const
{
	return std::accumulate<decltype(m_code.begin()), int>
		(m_code.begin(), m_code.end(), 0);
}

