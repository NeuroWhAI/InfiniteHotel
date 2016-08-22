#pragma once


#include <memory>
#include <vector>
#include <random>




class HotelStat;
class Unit;
class Gene;
class UnitPool;


class Hotel
{
public:
	Hotel();
	virtual ~Hotel();


protected:
	std::mt19937 m_randEngine;
	std::uniform_int_distribution<> m_mutationDist;


protected:
	std::unique_ptr<HotelStat> m_hotelStat;


protected:
	std::unique_ptr<UnitPool> m_unitPool;


protected:
	double m_worldEnergy;
	double m_lockedEnergy;
	std::vector<std::unique_ptr<Unit>> m_roomList;
	size_t m_epoch;
	size_t m_time, m_endTime;


protected:
	std::vector<size_t> m_scoreRanking;


public:
	void initialize(size_t firstUnitCount, size_t timePerEpoch);
	void update();


protected:
	void updateState();
	void updateCommunication();
	bool updateTime();
	void updateRanking();
	void updateLove();
	void updateEpoch();
	void updateEnergy();


protected:
	size_t createUnit(const Gene& gene);
};

