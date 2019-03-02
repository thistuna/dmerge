#include <filesystem>
#include <vector>
#include <string>
#include <iostream>

namespace fsys = std::filesystem;

void help(){
	std::cout << "dmerge source to [option]\n" << std::endl;
	std::cout << "-skip     : 重複ファイルをスキップ(default)" << std::endl;
	std::cout << "-sizeup   : ファイルサイズ大優先" << std::endl;
	std::cout << "-sizedown : ファイルサイズ小優先" << std::endl;
	exit(0);
}

int Main(std::vector<std::string> args)
{
	if(args.size() < 3){
		help();
	}

	fsys::path inpath = args[1];
	if (!fsys::is_directory(inpath))
		return 0;
	fsys::path outpath = args[2];
	if (!fsys::is_directory(outpath))
		return 0;

	for (const fsys::directory_entry& x : fsys::recursive_directory_iterator(inpath)) {
		std::cout << x.path() << std::endl;
	}

	return 0;
}

int main(int argc, char** argv)
{
	std::vector<std::string> args(argc);
	for(int i=0; i<argc; ++i)
		args[i] = argv[i];
	return Main(args);
}
