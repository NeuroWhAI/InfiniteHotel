#include "Interpreter.h"

#include <stack>

#include "InterpreterListener.h"




Interpreter::Interpreter()
	: m_isEnd(false)
	, m_code(nullptr)
	, m_codeLength(0)
	, m_memory(nullptr)
	, m_memoryLength(0)
	, m_register(0)
	, m_channel(0)
	, m_ptr(0)
	, m_head(0)

	, m_listener(nullptr)
{
	// 명령어 셋 초기화
	m_cmdSet.push_back(&Interpreter::increasePtr);
	m_cmdSet.push_back(&Interpreter::decreasePtr);
	m_cmdSet.push_back(&Interpreter::increaseData);
	m_cmdSet.push_back(&Interpreter::decreaseData);
	m_cmdSet.push_back(&Interpreter::writeData);
	m_cmdSet.push_back(&Interpreter::readData);
	m_cmdSet.push_back(&Interpreter::jumpIfZero);
	m_cmdSet.push_back(&Interpreter::jumpIfNotZero);
	m_cmdSet.push_back(&Interpreter::writeRegister);
	m_cmdSet.push_back(&Interpreter::readRegister);
	m_cmdSet.push_back(&Interpreter::writeChannel);
	m_cmdSet.push_back(&Interpreter::readChannel);
	m_cmdSet.push_back(&Interpreter::moveTargetLeft);
	m_cmdSet.push_back(&Interpreter::moveTargetRight);
	m_cmdSet.push_back(&Interpreter::vote);
	m_cmdSet.push_back(&Interpreter::love);
	m_cmdSet.push_back(&Interpreter::readTargetEnergy);
	m_cmdSet.push_back(&Interpreter::readTargetScore);
}

//###########################################################################

void Interpreter::writeTo(std::ostream& osr) const
{
	using std::endl;


	osr << ((m_isEnd) ? '1' : '0') << endl;
	osr << static_cast<int>(m_register) << endl;
	osr << static_cast<int>(m_channel) << endl;
	osr << m_ptr << endl;
	osr << m_head << endl;
}


void Interpreter::readFrom(std::istream& isr, const std::vector<char>* code,
	std::vector<char>* memory,
	InterpreterListener* listener)
{
	m_code = code;
	m_codeLength = code->size();
	m_memory = memory;
	m_memoryLength = memory->size();
	
	ready(*code);

	m_listener = listener;


	int isEnd = 0;
	isr >> isEnd;
	m_isEnd = ((isEnd == 0) ? false : true);

	int reg = 0;
	isr >> reg;
	m_register = static_cast<char>(reg);

	int channel = 0;
	isr >> channel;
	m_channel = static_cast<char>(channel);

	isr >> m_ptr;
	isr >> m_head;
}

//###########################################################################

void Interpreter::initialize(const std::vector<char>* code,
	std::vector<char>* memory,
	InterpreterListener* listener)
{
	m_isEnd = false;
	m_code = code;
	m_codeLength = code->size();
	m_memory = memory;
	m_memoryLength = memory->size();
	m_register = 0;
	m_channel = 0;
	m_ptr = 0;
	m_head = 0;
	m_getBeginBracket.clear();
	m_getEndBracket.clear();

	m_listener = listener;


	// 코드 전처리
	m_isEnd = !ready(*code);
}

//###########################################################################

bool Interpreter::isEnd() const
{
	return ((m_head >= m_codeLength) || m_isEnd);
}


void Interpreter::update()
{
	m_isEnd = !(this->*m_cmdSet[(*m_code)[m_head]])();
}

//###########################################################################

char Interpreter::getChannel() const
{
	return m_channel;
}

//###########################################################################

bool Interpreter::ready(const std::vector<char>& code)
{
	/// 코드 길이
	const size_t codeLength = code.size();


	// 레지스터를 초기화 한다.
	m_register = 0;

	// 메모리 포인터를 첫부분으로 옮긴다.
	m_ptr = 0;

	// 코드 헤더를 첫부분으로 옮긴다.
	m_head = 0;


	// 괄호 짝의 위치를 미리 계산해둔다.
	m_getBeginBracket.clear();
	m_getBeginBracket.resize(codeLength, 0);

	m_getEndBracket.clear();
	m_getEndBracket.resize(codeLength, 0);


	/// 괄호 짝을 찾기위한 스택
	std::stack<size_t> beginBracket;

	// 괄호 짝을 찾는다.
	for (size_t i = 0; i < codeLength; ++i)
	{
		// 괄호 짝 찾기.
		if (code[i] == 6/*[*/)
		{
			beginBracket.push(i);
		}
		else if (code[i] == 7/*]*/)
		{
			if (beginBracket.empty())
			{
				// 괄호의 짝이 맞지 않으므로 실패.
				return false;
			}
			else
			{
				// 짝을 찾았음.

				size_t beginIndex = beginBracket.top();
				beginBracket.pop();

				m_getBeginBracket[i] = beginIndex;
				m_getEndBracket[beginIndex] = i;
			}
		}
	}


	return beginBracket.empty();
}

//###########################################################################

bool Interpreter::increasePtr()
{
	++m_head;

	if (m_ptr < m_memoryLength - 1)
	{
		++m_ptr;
	}
	else
	{
		m_ptr = 0;
	}


	return m_listener->whenIncreasePtr();
}


bool Interpreter::decreasePtr()
{
	++m_head;

	if (m_ptr > 0)
	{
		--m_ptr;
	}
	else
	{
		m_ptr = m_memoryLength - 1;
	}


	return m_listener->whenDecreasePtr();
}


bool Interpreter::increaseData()
{
	++m_head;

	++(*m_memory)[m_ptr];


	return m_listener->whenIncreaseData();
}


bool Interpreter::decreaseData()
{
	++m_head;

	--(*m_memory)[m_ptr];


	return m_listener->whenDecreaseData();
}


bool Interpreter::writeData()
{
	++m_head;


	return m_listener->whenWriteData((*m_memory)[m_ptr]);
}


bool Interpreter::readData()
{
	++m_head;


	return m_listener->whenReadData((*m_memory)[m_ptr]);
}


bool Interpreter::jumpIfZero()
{
	if ((*m_memory)[m_ptr] == 0)
	{
		m_head = m_getEndBracket[m_head];
	}
	else
	{
		++m_head;
	}


	return m_listener->whenJumpIfZero();
}


bool Interpreter::jumpIfNotZero()
{
	if ((*m_memory)[m_ptr] != 0)
	{
		m_head = m_getBeginBracket[m_head];
	}
	else
	{
		++m_head;
	}


	return m_listener->whenJumpIfNotZero();
}


bool Interpreter::writeRegister()
{
	++m_head;

	auto oldReg = m_register;
	m_register = (*m_memory)[m_ptr];


	return m_listener->whenWriteRegister(m_register - oldReg);
}


bool Interpreter::readRegister()
{
	++m_head;

	auto oldMem = (*m_memory)[m_ptr];
	(*m_memory)[m_ptr] = m_register;


	return m_listener->whenReadRegister(m_register - oldMem);
}


bool Interpreter::writeChannel()
{
	++m_head;

	auto oldChannel = m_channel;
	m_channel = (*m_memory)[m_ptr];


	return m_listener->whenWriteChannel(m_channel - oldChannel);
}


bool Interpreter::readChannel()
{
	++m_head;

	auto oldMem = (*m_memory)[m_ptr];
	(*m_memory)[m_ptr] = m_channel;


	return m_listener->whenReadChannel(m_channel - oldMem);
}


bool Interpreter::moveTargetLeft()
{
	++m_head;


	return m_listener->whenMoveTargetLeft();
}


bool Interpreter::moveTargetRight()
{
	++m_head;


	return m_listener->whenMoveTargetRight();
}


bool Interpreter::vote()
{
	++m_head;


	return m_listener->whenVote((*m_memory)[m_ptr]);
}


bool Interpreter::love()
{
	++m_head;


	return m_listener->whenLove();
}


bool Interpreter::readTargetEnergy()
{
	++m_head;


	return m_listener->whenReadTargetEnergy((*m_memory)[m_ptr]);
}


bool Interpreter::readTargetScore()
{
	++m_head;


	return m_listener->whenReadTargetScore((*m_memory)[m_ptr]);
}

