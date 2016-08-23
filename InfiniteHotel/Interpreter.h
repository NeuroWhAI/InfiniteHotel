#pragma once


#include <vector>
#include <ostream>
#include <istream>




class InterpreterListener;


class Interpreter
{
public:
	static const int CMD_COUNT = 18;


public:
	Interpreter();


protected:
	bool m_isEnd;
	const std::vector<char>* m_code;
	size_t m_codeLength;
	std::vector<char>* m_memory;
	size_t m_memoryLength;
	char m_register;
	char m_channel;
	size_t m_ptr;
	size_t m_head;
	std::vector<size_t> m_getBeginBracket;
	std::vector<size_t> m_getEndBracket;


protected:
	std::vector<bool (Interpreter::*)()> m_cmdSet;
	InterpreterListener* m_listener;


public:
	void writeTo(std::ostream& osr) const;
	void readFrom(std::istream& isr, const std::vector<char>* code,
		std::vector<char>* memory,
		InterpreterListener* listener);


public:
	void initialize(const std::vector<char>* code,
		std::vector<char>* memory,
		InterpreterListener* listener);


public:
	bool isEnd() const;
	void update();


public:
	char getChannel() const;


protected:
	bool ready(const std::vector<char>& code);


protected:
	bool increasePtr();
	bool decreasePtr();
	bool increaseData();
	bool decreaseData();
	bool writeData();
	bool readData();
	bool jumpIfZero();
	bool jumpIfNotZero();
	bool writeRegister();
	bool readRegister();
	bool writeChannel();
	bool readChannel();
	bool moveTargetLeft();
	bool moveTargetRight();
	bool vote();
	bool love();
	bool readTargetEnergy();
	bool readTargetScore();
};

