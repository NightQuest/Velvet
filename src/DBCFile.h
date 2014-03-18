#pragma once

struct DBCHeader
{
	unsigned int signature;
	unsigned int rows;
	unsigned int columns;
	unsigned int rowSize;
	unsigned int stringTableSize;
};

struct DBCColumn
{
	unsigned int position;
	unsigned int size;
};

class DBCFile;

class DBCRow
{
private:
	DBCFile* parent;
	BYTE* data;
	unsigned int dataSize;

public:
	DBCRow(DBCFile* file, unsigned int rowSize, BYTE* rowData);
	~DBCRow();

	template <typename T> T getColumn(unsigned int column);
	const char* getColumnAsString(unsigned int column);
};

class DBCFile
{
private:
	DBCHeader fileHeader;
	HANDLE hFile;
	std::map<unsigned int, DBCRow*> rows;
	char* stringTable;
	DBCColumn* columnFormat;

protected:
	friend class DBCRow;
	DBCColumn* getColumnFormat() { return columnFormat; }
	char* getStringTable() { return stringTable; }

public:
	DBCFile();
	~DBCFile();

	bool open(const tstring& filename);
	bool save(const tstring& filename);
	
	void setColumnFormat(const tstring& format);
	DBCHeader getHeader() { return fileHeader; }
	DBCRow* getRow(unsigned int row);
	const char* getString(unsigned int offset) { return reinterpret_cast<const char*>(stringTable + offset); }
};