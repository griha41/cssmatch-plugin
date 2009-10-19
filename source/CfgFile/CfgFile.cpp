#include "CfgFile.h"

#include <iostream>

using std::string;
using std::map;

void CfgFile::stripEndLine(string & str)
{
	size_t strSize = str.size();

	size_t indexComment = str.find("\r");
	if (indexComment != string::npos)
		str = str.substr(0,indexComment);
}

void CfgFile::stripComments(string & str)
{
	size_t strSize = str.size();

	size_t indexComment = str.find("//");

	//std::cout << indexComment << std::endl;
	if (indexComment != string::npos)
		str = str.substr(0,indexComment);
}

void CfgFile::stripString(string & str)
{
	size_t strSize = str.size();
	size_t posDataBegin = 0;
	size_t posDataEnd = strSize;

	string::const_iterator itChar = str.begin();
	string::const_iterator lastChar = str.end();
	bool allFound = false;
	while(itChar!=lastChar && (! allFound))
	{
		if (*itChar == ' ' || *itChar == '\t')
		{
			posDataBegin++;
			itChar++;
		}
		else
			allFound = true;
	}

	string::const_reverse_iterator itRChar = str.rbegin();
	string::const_reverse_iterator lastRChar = str.rend();
	allFound = false;
	while(itRChar!=lastRChar && (! allFound))
	{
		if (*itRChar == ' ' || *itRChar == '\t')
		{
			posDataEnd--;
			itRChar++;
		}
		else
			allFound = true;
	}

	str = str.substr(posDataBegin,posDataEnd-posDataBegin);
}

void CfgFile::searchHeader(string & line)
{
	//std::cout << "	Searching the header in the following line : \"" << line << "\"" << std::endl;
	if (line[0] == '[') 
	{ // It appears we found the header
		
		//std::cout << "		Found \"[\" token" << std::endl;

		size_t posEndHeader = line.find("]");

		if (posEndHeader != string::npos)
		{
			//std::cout << "		Found \"]\" token" << std::endl;
			header = line.substr(1,posEndHeader-1); // We found the [header]
			parseFunction = &CfgFile::searchData;
		}
		/*else
			std::cout << "		Header not found :-(" << std::endl;*/
	}
}

void CfgFile::searchData(string & line)
{
	// We will search the equal symbol '='
	size_t posEqual = string::npos;
	size_t lineSize = line.size();

	bool betweenQuotes = false; // true if we are in a quoted string

	string::const_iterator itChar = line.begin();
	string::const_iterator lastChar = line.end();
	bool equalFound = false;
	while(itChar!=lastChar && (! equalFound))
	{
		posEqual++;
		switch(*itChar) 
		{ 
		case '"': // Quoted string
			betweenQuotes = ! betweenQuotes;
			break;

		case '=':
			/*if (! betweenQuotes)
				equalFound = true; */
				equalFound = ! betweenQuotes; // We found the equal symbol
			break;
		}

		itChar++;
	}

	// Is the equal symbol is not found or is in at the end of the string ?
	if (posEqual != lineSize-1)
	{ // No, we can save the data
		string dataName = line.substr(0,posEqual);	
		stripString(dataName);
		// Stripping the quote if required
		size_t posLastCharDataName = dataName.size()-1;
		if (posLastCharDataName>0)
		{
			if (dataName[0] == '"' && dataName[posLastCharDataName] == '"')
				dataName = dataName.substr(1,posLastCharDataName-1);

			string dataValue = line.substr(posEqual+1,lineSize); // +1 to strip the equal symbol
			stripString(dataValue);
			// Stripping the quotes if required
			size_t posLastCharDataValue = dataValue.size()-1;
			if (posLastCharDataValue>0)
			{
				if (dataValue[0] == '"' && dataValue[posLastCharDataValue] == '"')
					dataValue = dataValue.substr(1,posLastCharDataValue-1);

				if (dataName.size()>0 && dataValue.size()>0)
				{
					data[dataName] = dataValue;
					//std::cout << "Data found : {\"" << dataName << "\" : \"" << dataValue << "\"}" << std::endl;
				}
			}
		}
	}
	// Else we ignore this invalid line
}

CfgFile::CfgFile()
{
}

CfgFile::CfgFile(const string & fileName) throw(CfgFileException) : file(fileName.c_str())
{
	// First, we will need to find the header
	parseFunction = &CfgFile::searchHeader;
	if (! file.fail())
	{
		string line;
		while(std::getline(file,line))
		{
			stripEndLine(line);
			stripComments(line);
			stripString(line);

			//std::cout << "Parsing the following line : \"" << line << "\"" << std::endl;

			// Invoking the suitable parse function
			((*this).*(parseFunction))(line);
		}

		if (header.size() == 0) // Header is required
			throw CfgFileException("No header found for the file \"" + fileName + "\"");
	}
	else
		throw CfgFileException("Unable to open the file \"" + fileName + "\"");
}

CfgFile::~CfgFile()
{
	file.close();
}

const string & CfgFile::getHeader() const
{
	return header;
}

bool CfgFile::exists(const std::string & dataName)
{
	return data.find(dataName) != data.end();
}

string & CfgFile::operator [](const string & dataName)
{
	return data[dataName];
}

const map<string,string> * CfgFile::getMap() const
{
	return &data;
}

int CfgFile::main(int argc, char * * argv)
{
	CfgFile cfgFile;
	string test = "    All Data !	// comment ; fake comment   ";

	string noComment(test);
	CfgFile::stripComments(noComment);
	std::cout << "No Comment : \"" << noComment << "\"" << std::endl;

	string noBlank(test);
	CfgFile::stripString(noBlank);
	std::cout << "No blank char : \"" << noBlank << "\"" << std::endl;

	string header = "		[	Titre	]	// commentaire";
	CfgFile::stripComments(header);
	CfgFile::stripString(header);
	cfgFile.searchHeader(header);
	std::cout << cfgFile.getHeader() << std::endl;

	string data = "		no=m=\"valeur	";
	CfgFile::stripComments(data);
	CfgFile::stripString(data);
	cfgFile.searchData(data);
	std::cout << cfgFile["no"] << std::endl;

	return 0;
}

int main()
{
	try
	{
		//CfgFile::main();

		CfgFile file("french.txt");
		std::cout << "[" << file.getHeader() << "]" << std::endl;

		const map<string,string> * data = file.getMap();
		map<string,string>::const_iterator itData = data->begin();
		map<string,string>::const_iterator lastData = data->end();
		while(itData != lastData)
		{	
			std::cout << "\"" <<  itData->first << "\" = \"" << itData->second << "\"" << std::endl;
			//std::cout << itData->first << " = " << itData->second << std::endl;

			itData++;
		}
	}
	catch (const std::exception & e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}
