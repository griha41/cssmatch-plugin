#include "CfgFileException.h"

CfgFileException::CfgFileException(const std::string & msg) : std::exception()
{
	this->msg = msg;
}

CfgFileException::~CfgFileException() throw()
{
}

const char * CfgFileException::what() const throw()
{
	return msg.c_str();
}
