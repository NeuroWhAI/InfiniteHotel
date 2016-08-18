#pragma once


class InterpreterListener
{
public:
	virtual bool whenIncreasePtr() = 0;
	virtual bool whenDecreasePtr() = 0;
	virtual bool whenIncreaseData() = 0;
	virtual bool whenDecreaseData() = 0;
	virtual bool whenWriteData(char data) = 0;
	virtual bool whenReadData(char& outData) = 0;
	virtual bool whenJumpIfZero() = 0;
	virtual bool whenJumpIfNotZero() = 0;
	virtual bool whenWriteRegister(char delta) = 0;
	virtual bool whenReadRegister(char delta) = 0;
	virtual bool whenWriteChannel(char delta) = 0;
	virtual bool whenReadChannel(char delta) = 0;
	virtual bool whenMoveTargetLeft() = 0;
	virtual bool whenMoveTargetRight() = 0;
	virtual bool whenVote(char how) = 0;
	virtual bool whenLove() = 0;
	virtual bool whenReadTargetEnergy(char& outData) = 0;
	virtual bool whenReadTargetScore(char& outData) = 0;
};

