#ifndef __CFGFILE_H__
#define __CFGFILE_H__

#include <map>
#include <string>
#include <fstream>
#include <sstream>

#include "CfgFileException/CfgFileException.h"

// Visual C++ 2005 does not support throw(AnyException) statement
#pragma warning(disable : 4290)

/** Main wrapper between the software and an Cfg File */
class CfgFile
{
private:
	/** Input stream on the CfgFile file */
	std::ifstream file;

	/** Header */
	std::string header;
	
	/** {data name => data} */
	std::map<std::string,std::string> data;

	/** Fonction to be invoked when parsing the data */
	void (CfgFile::*parseFunction)(std::string & line);

	/** Search the header 
	 *
	 * @param line The line to be parsed
	 */
	void searchHeader(std::string & line);

	/** Search the data
	 *
	 * @param line The line to be parsed
	 */
	void searchData(std::string & line);

	/** Private default constructor (test constructor) */
	CfgFile();

public:
	/** CfgFiletialize a class instance on an CfgFile file 
	 * 
	 * @param fileName The name of the CfgFile file 
	 */
	CfgFile(const std::string & fileName) throw(CfgFileException);

	/** Free memory and close the CfgFile file */
	virtual ~CfgFile();

	/** Strip any \r
	 * Note : std::getline strip the \r and \n tokens, 
	 * but if the file has been saved on windows, them read from a Unix OS, some \r could be here
	 *
	 * @param str The string to strip
	 */
	static void stripEndLine(std::string & str);

	/** Strip any commentary 
	 *
	 * @param str The string to strip
	 */
	static void stripComments(std::string & str);

	/** Strip the string
	 *
	 * @param str The string to strip
	 */
	static void stripString(std::string & str);
	
	/** Get the header string */
	const std::string & getHeader() const;

	/** Check if a data name exists */
	bool exists(const std::string & dataName);

	/** Getting operator 
	 *
	 * @param dataName The name of the data to return
	 * @return The data, or a empty string if the data does not exists (std::map behavior)
	 */
	std::string & operator [](const std::string & dataName);

	/** Provide a read only access to the std::map generated */
	const std::map<std::string,std::string> * getMap() const;

	/** Test Main */
	static int main(int argc = 0, char * * argv = NULL);
};

#endif // __CFGFILE_H__
