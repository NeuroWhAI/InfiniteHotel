#pragma once


class HotelStat
{
public:
	HotelStat();


protected:
	size_t m_maxUnitCount;
	size_t m_unitCount;
	size_t m_birth;
	size_t m_death;
	double m_maxUnitEnergy;
	double m_maxUnitScore;
	double m_minUnitScore;
	size_t m_mutationCount;


public:
	void reset();


public:
	void updateUnitCount(size_t unitCount);
	size_t getMaxUnitCount() const;
	size_t getUnitCount() const;
	void addBirth(size_t delta = 1);
	size_t getBirth() const;
	void addDeath(size_t delta = 1);
	size_t getDeath() const;
	void updateMaxUnitEnergy(double unitEnergy);
	double getMaxUnitEnergy() const;
	void updateUnitScore(double unitScore);
	double getMaxUnitScore() const;
	double getMinUnitScore() const;
	void addMutationCount(size_t delta = 1);
	size_t getMutationCount() const;
};

