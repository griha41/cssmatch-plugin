#ifndef __CFGFILEEXCEPTION_H__
#define __CFGFILEEXCEPTION_H__

#include <stdexcept>
#include <string>

class CfgFileException : public std::exception
{
private:
	std::string msg;	
public:
	CfgFileException(const std::string & msg);
	~CfgFileException() throw();
	
	const char * what() const throw();
};

#endif // __CFGFILEEXCEPTION_H__
