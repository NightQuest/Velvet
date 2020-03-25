#include "preCompiled.h"

DBCRow::DBCRow(DBCFile* file, unsigned int rowSize, BYTE* rowData) : parent(file), dataSize(rowSize), data(nullptr)
{
	data = new BYTE[dataSize];
	memcpy(data, rowData, dataSize);
}

DBCRow::~DBCRow()
{
	if( data != nullptr )
	{
		delete[] data;
		data = nullptr;
	}
}

template <typename T>
T DBCRow::getColumn(unsigned int column)
{
	return *reinterpret_cast<T*>(data + parent->getColumnFormat()[column].position);
}

const char* DBCRow::getColumnAsString(unsigned int column)
{
	return parent->getString(getColumn<unsigned int>(column));
}

DBCFile::DBCFile() : hFile(INVALID_HANDLE_VALUE)
{
}

DBCFile::~DBCFile()
{
	if( hFile != INVALID_HANDLE_VALUE )
	{
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
}

bool DBCFile::open(const std::wstring& filename)
{
	if( filename.empty() )
		return false;

	// Open the file in read-only mode
	if( (hFile = CreateFile(filename.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE )
		return false;

	// Verify the file is big enough for a header
	LARGE_INTEGER fileSize;
	if( GetFileSizeEx(hFile, &fileSize) == 0 || fileSize.QuadPart <= sizeof(DBCHeader))
	{
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
		return false;
	}

	// Read the file header
	DWORD bytesRead;
	if( ReadFile(hFile, &fileHeader, sizeof(DBCHeader), &bytesRead, NULL) == FALSE || bytesRead != sizeof(DBCHeader) || fileHeader.signature != 0x43424457 ) // 'WDBC'
	{
		memset(&fileHeader, 0, sizeof(DBCHeader));
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
		return false;
	}

	// Allocate space for the rows
	unsigned int rowBlockSize = fileHeader.rowSize * fileHeader.rows;
	BYTE* rowBuff;
	if( (rowBuff = new BYTE[rowBlockSize]) == nullptr )
	{
		memset(&fileHeader, 0, sizeof(DBCHeader));
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
		return false;
	}

	// Read the rows from the file
	if( ReadFile(hFile, rowBuff, rowBlockSize, &bytesRead, NULL) == FALSE || bytesRead != rowBlockSize )
	{
		memset(&fileHeader, 0, sizeof(DBCHeader));
		delete[] rowBuff;
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
		return false;
	}

	// Move the rows into DBCRow's, and deallocate our buffer
	for( unsigned int curr = 0; curr < fileHeader.rows; curr++ )
		rows[curr] = new DBCRow(this, fileHeader.rowSize, rowBuff + (fileHeader.rowSize * curr));
	delete[] rowBuff;

	// Allocate our string table
	if( (stringTable = new char[fileHeader.stringTableSize]) == nullptr )
	{
		memset(&fileHeader, 0, sizeof(DBCHeader));
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
		return false;
	}

	// Read our string table from the file
	if( ReadFile(hFile, stringTable, fileHeader.stringTableSize, &bytesRead, NULL) == FALSE || bytesRead != fileHeader.stringTableSize )
	{
		memset(&fileHeader, 0, sizeof(DBCHeader));
		delete[] stringTable;
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
		return false;
	}

	CloseHandle(hFile);
	hFile = INVALID_HANDLE_VALUE;
	return true;
}

bool DBCFile::save(const std::wstring& filename)
{
	return false;
}

void DBCFile::setColumnFormat(const std::wstring& format)
{
	if( columnFormat != nullptr )
	{
		delete[] columnFormat;
		columnFormat = nullptr;
	}

	if( (columnFormat = new DBCColumn[fileHeader.columns-1]) == nullptr )
		return;

	memset(columnFormat, 0, fileHeader.columns-1);

	unsigned int totalRowSize = 0;
	for( size_t x = 0; x < format.length(); x++ )
	{
		if( format[x] == L'b' )
			columnFormat[x].size = 1;
		else if( format[x] == L's' )
			columnFormat[x].size = 2;
		else if( format[x] == L'i' || format[x] == L'f' )
			columnFormat[x].size = 4;

		if( x > 0 )
			columnFormat[x].position = columnFormat[x-1].position + columnFormat[x-1].size;

		totalRowSize += columnFormat[x].size;
	}

	/*
	// Verify the total row size matches
	if( totalSize != fileHeader.rowSize )
		return false;
	*/
}

DBCRow* DBCFile::getRow(unsigned int row)
{
	auto itr = rows.find(row);
	if( itr == rows.end() )
		return nullptr;

	return itr->second;
}

DBCRow* DBCFile::getRowById(unsigned int id)
{
	for( auto row : rows )
	{
		if( row.second->getColumn<unsigned int>(0) == id )
			return row.second;
	}

	return nullptr;
}
