#pragma once


#include <memory>
#include <vector>

#include "InterpreterListener.h"
#include "Gene.h"
#include "Interpreter.h"




class Unit : public InterpreterListener
{
public:
	explicit Unit(const Gene& gene, const size_t myIndex,
		const std::vector<std::unique_ptr<Unit>>& roomList);


protected:
	const std::vector<std::unique_ptr<Unit>>& m_roomList;


protected:
	Gene m_gene;
	double m_energy;


protected:
	std::vector<char> m_memory;
	std::unique_ptr<Interpreter> m_interpreter;


protected:
	const size_t m_myIndex;
	size_t m_targetUnitIndex;
	std::vector<char> m_inputList;
	std::vector<char> m_outputList;
	double m_score;
	double m_nextScore;
	double m_targetScore;
	size_t m_loveUnitIndex;


public:
	bool isDead() const;
	void updateInterpreter();
	void updateCommunication();


public:
	const Gene& getGene() const;
	double getEnergy() const;
	void addEnergy(double energy);


protected:
	virtual bool whenIncreasePtr() override;
	virtual bool whenDecreasePtr() override;
	virtual bool whenIncreaseData() override;
	virtual bool whenDecreaseData() override;
	virtual bool whenWriteData(char data) override;
	virtual bool whenReadData(char& outData) override;
	virtual bool whenJumpIfZero() override;
	virtual bool whenJumpIfNotZero() override;
	virtual bool whenWriteRegister() override;
	virtual bool whenReadRegister() override;
	virtual bool whenWriteChannel() override;
	virtual bool whenReadChannel() override;
	virtual bool whenMoveTargetLeft() override;
	virtual bool whenMoveTargetRight() override;
	virtual bool whenVote(char how) override;
	virtual bool whenLove() override;
	virtual bool whenReadTargetEnergy(char& outData) override;
	virtual bool whenReadTargetScore(char& outData) override;


public:
	void input(size_t channel, char data);
	void vote(double deltaScore);
	size_t getRoomIndex() const;
	double getScore() const;
	size_t getLoveUnitIndex() const;
};

