#include "DataTable.h"

DataTable::DataTable(const std::vector<std::string>& p_Headers)
: m_Values(p_Headers), m_NumColumns(p_Headers.size())
{
}

void DataTable::recordValue(unsigned int p_Column, unsigned int p_Row, std::string p_Value)
{
	if(p_Column >= m_NumColumns)
	{
		return;
	}

	if(p_Row >= m_Values.size() / m_NumColumns - 1)
	{
		m_Values.resize((p_Row+2)*m_NumColumns);
	}

	m_Values[(p_Row + 1) * m_NumColumns + p_Column] = p_Value;
}


void DataTable::printCSV(std::ostream& p_Out)
{
	for (unsigned int row = 0; row < m_Values.size() / m_NumColumns; ++row)
	{
		p_Out << m_Values[row * m_NumColumns];
		for (unsigned int col = 1; col < m_NumColumns; ++col)
		{
			p_Out << ';' << m_Values[row * m_NumColumns + col];
		}
		p_Out << '\n';
	}
}