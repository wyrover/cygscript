#ifndef __UTIL__WINERROR_HPP__
#define __UTIL__WINERROR_HPP__

#include <windows.h>
#include <sstream>

namespace cygscript {

/* throw a runtime_error with a message from given error code */
#define THROW_ERROR_CODE(msg, code) \
	do { \
		cygscript::WinError _err(code); \
		std::stringstream _ss; \
		_ss << msg << ": " << (std::string)_err; \
		throw std::runtime_error(_ss.str()); \
	} while(0)

/* throw a runtime_error with a message from last error */
#define THROW_LAST_ERROR(msg) \
	THROW_ERROR_CODE(msg, GetLastError())

class WinError
{
public:
	WinError() :
		_err(GetLastError()) {
	};
	WinError(DWORD err) :
		_err(err) {
	};
	~WinError() {
	};
	operator std::wstring() const;
	operator std::string() const;
private:
	DWORD _err;
	std::wstring _translateMessage() const;
};

/* std::string + WinError operator */
std::string operator+(const std::string &str, const WinError &err);

}

#endif