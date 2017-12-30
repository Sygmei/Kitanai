#include <iostream>

#include <StdLib.hpp>
#include <Token.hpp>
#include <Program.hpp>

using namespace ktn;

int main(int argc, char** argv)
{
    StdLib::FunctionsName["func"] = StdLib::fRightPart("func", StdLib::f_func, 3);
	RunArgParser runParser(argv, argc);
	Program prog;
	if (argc == 2 || runParser.argumentExists("-f")) {
		if (runParser.argumentExists("-e")) Node::_execDebug = true;
		if (runParser.argumentExists("-c")) {
			Node::_compDebug = true;
			prog.compDebug = true;
		}
		if (runParser.argumentExists("-l")) {
			std::ofstream out(runParser.getArgumentValue("-l"));
			std::streambuf *coutbuf = std::cout.rdbuf();
			std::cout.rdbuf(out.rdbuf());
		}
		if (runParser.argumentExists("-a")) {
			Node::_dispMemAd = true;
		}
        if (runParser.argumentExists("-i")) {
            prog.import(runParser.getArgumentValue("-i"));
        }
		
		prog.parseFile((argc == 2) ? argv[1] : runParser.getArgumentValue("-f"));
		prog.exec();
	}

	if (prog.compDebug) std::cout << "Execution over.." << std::endl;

    return 0;
}