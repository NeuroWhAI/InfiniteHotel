#pragma once


#include <vector>
#include <random>
#include <ostream>
#include <istream>




class Gene
{
public:
	class Hasher
	{
	public:
		size_t operator() (const Gene& gene) const
		{
			return std::hash<int>()(gene.getSum());
		}
	};


public:
	Gene();
	Gene(const std::initializer_list<char>& list);


protected:
	std::vector<char> m_code;


public:
	void writeTo(std::ostream& osr) const;
	void readFrom(std::istream& isr);


public:
	void initializeRandomly(std::mt19937& engine, int cmdSetCount);


public:
	size_t getLength() const;
	const std::vector<char>& getCode() const;
	double getEnergy() const;


public:
	void combine(const Gene& other, std::mt19937& engine);
	void mutate(std::mt19937& engine, int cmdSetCount);


public:
	bool operator== (const Gene& right) const;
	bool operator!= (const Gene& right) const;


protected:
	int getSum() const;
};

