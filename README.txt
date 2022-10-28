HTTP 1.0 Server
Lucas Streanga
1001612558
CSE 5344-001

REQUIREMENTS
	- Posix-compliant operating system (Unix, Linux)
	- GCC 8.0 or later, for C++17 support
	- make (gnu-make or alternative)
	- Posix thread library (pthread)

DIRECTORY BREAKDOWN
	- / contains Makefile, documentation.pdf, and test files
	- /src contains .cpp files
	- /include contains .h files

COMPILATION
	- The program is written in C++ and is compiled via make
	- run make in the root directory to compile
		- C++17 is required!
		- pthread will be linked!

RUNNING
	- The executable is named http-server and is in the root directory
	- run ./http_request <optional port number> to execute
		- The default port is set to 6789
		- The command line parameter is optional if you wish to change port number
	- in web browser, type localhost:<port number>/<file>

