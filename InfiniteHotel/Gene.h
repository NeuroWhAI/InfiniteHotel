#pragma once


#include <vector>
#include <random>




class Gene
{
public:
	Gene();
	Gene(const std::initializer_list<char>& list);


protected:
	std::vector<char> m_code;


public:
	void initializeRandomly(std::mt19937& engine, int cmdSetCount);


public:
	const std::vector<char>& getCode() const;
	double getEnergy() const;


public:
	void combine(const Gene& other, std::mt19937& engine);
	void mutate(std::mt19937& engine, int cmdSetCount);
};

