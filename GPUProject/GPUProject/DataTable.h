#pragma once

#include <vector>
#include <string>
#include <ostream>

class DataTable
{

private:
	std::vector<std::string> m_Values;
	unsigned int m_NumColumns;

public:
	DataTable(const std::vector<std::string>& p_Headers);
	DataTable(){};
	void recordValue(unsigned int p_Column, unsigned int p_Row, std::string p_Value);
	void DataTable::printCSV(std::ostream& p_Out);
};