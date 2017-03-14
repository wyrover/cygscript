#include <vector>
#include <string>

namespace cygscript {

class App
{
	int _argc;
	char* const* _argv;
	enum class Command { NONE, EXEC, REGISTER, UNREGISTER, LIST };
	Command _cmd;
	enum class RegisterType { USER, EVERYONE };
	RegisterType _regType;
	std::string _extension;
	bool _force;
public:
	App(const int argc, char* const argv[]);
	int run();
private:
	void _printUsage(char* progname);
	void _printVersion();
	std::vector<std::wstring> _wideArgs();
	void _checkElevated();
};

}
