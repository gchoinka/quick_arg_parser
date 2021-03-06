//usr/bin/g++ --std=c++17 -Wall $0 -o ${o=`mktemp`} && exec $o $*
#include "quick_arg_parser.hpp"

struct Input : MainArguments<Input> {
	bool verbose = option("verbose", 'V');
	int port = option("port", 'p');
	int secondaryPort = option("port2", 'P') = 999;
	int parts = argument(0) = 1;
};

struct Input2 : MainArguments<Input2> {
	std::vector<int> ports = option("ports", 'p');
	std::shared_ptr<int> downloads = option("downloads", 'd', "The number of downloads");
	std::unique_ptr<int> uploads = option("uploads", 'u');
	std::string file = argument(0);
	std::string logFile = argument(1) = "log.log";
	std::string debugLogFile = argument(2) = "debug.log";

	static std::string help(const std::string& programName) {
		return "Usage\n" + programName + " FILE LOG DEBUGLOG";
	}
	void onHelp() {
		std::cout << "Help called" << std::endl;
	}
	
	static std::string version() {
		return "3.3.7";
	}
	void onVersion() {}
};

struct Input3 : MainArguments<Input3> {
	std::vector<int> ports = option("ports", 'p');
	bool enableHorns = option('h');
	std::string file = argument(0);
	bool enableHooves = option('H');
	bool loud = option("LOUD");
	std::string target = argument(1) = "a.out";

	void onHelp() {}
	static std::string options() {
		return "Don't usese the options, they suck\n";
	}
	inline static const std::string version = "1.0";
	void onVersion() {}
};

template <typename T>
T constructFromString(std::string args) {
	std::vector<char*> segments;
	segments.push_back(&args[0]);
	for (int i = 0; i < int(args.size()); i++) {
		if (args[i] == ' ') {
			segments.push_back(&args[i + 1]);
			args[i] = '\0';
		}
	}
	return T{{int(segments.size()), &segments[0]}};
}

int main() {

	int errors = 0;
	auto verify = [&](auto first, auto second) {
		if (first != second) {
			std::cout << first << " and " << second << " were supposed to be equal" << std::endl;
			errors++;
		}
	};

	std::cout << "First input" << std::endl;
	Input t1 = constructFromString<Input>("super_program -V --port 666 -- 3");
	verify(t1.verbose, true);
	verify(t1.port, 666);
	verify(t1.secondaryPort, 999);
	verify(t1.parts, 3);

	std::cout << "Second input" << std::endl;
	Input2 t2 = constructFromString<Input2>("mega_program -p 23,80,442 -u 3 --help --version -- -lame_file_name log");
	verify(int(t2.ports.size()), 3);
	verify(t2.file, "-lame_file_name");
	verify(bool(t2.downloads), false);
	verify(*t2.uploads, 3);
	verify(t2.logFile, "log");
	verify(t2.debugLogFile, "debug.log");

	std::cout << "Third input" << std::endl;
	Input3 t3 = constructFromString<Input3>("supreme_program file -hH -? -V --LOUD target");
	verify(int(t3.ports.size()), 0);
	verify(t3.file, "file");
	verify(t3.enableHooves, true);
	verify(t3.enableHorns, true);
	verify(t3.target, "target");
	verify(t3.loud, true);

	std::cout << "Errors: " << errors << std::endl;
}
