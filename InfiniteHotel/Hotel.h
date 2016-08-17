#pragma once


#include <memory>
#include <vector>
#include <random>




class HotelStat;
class Unit;
class Gene;


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
	double m_totalEnergy;
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


protected:
	size_t createUnit(const Gene& gene);
};

