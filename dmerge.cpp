#include <filesystem>
#include <vector>
#include <string>
#include <iostream>

namespace fsys = std::filesystem;

void help(){
	std::cout << "�f�B���N�g���̓��e��ʂ̃f�B���N�g���ɃR�s�[���܂��D" << std::endl;
	std::cout << "dmerge �R�s�[�� �R�s�[�� [�I�v�V����]\n" << std::endl;

	std::cout << "�@�I�v�V����" << std::endl;
	std::cout << "-skip     : �d���t�@�C�����X�L�b�v(default)" << std::endl;
	std::cout << "-sizeup   : �t�@�C���T�C�Y��D��" << std::endl;
	std::cout << "-sizedown : �t�@�C���T�C�Y���D��" << std::endl;
	exit(0);
}

int Main(std::vector<std::string> args)
{
	if(args.size() < 3){
		help();
	}

	fsys::path indirpath = args[1];
	if (!fsys::is_directory(indirpath)) {
		std::cout << "�R�s�[���f�B���N�g����������܂���." << std::endl;
		help();
		return 0;
	}
	fsys::path outdirpath = args[2];
	if (!fsys::is_directory(outdirpath)) {
		std::cout << "�R�s�[��f�B���N�g����������܂���." << std::endl;
		help();
		return 0;
	}

	enum class SizeFlag{skip, sizeup, sizedown,err};
	SizeFlag sizeFlag = SizeFlag::skip;
	bool optionerr = false;
	for (int i = 3; i < args.size(); ++i) {
		if (args[i] == "-skip")
			sizeFlag = SizeFlag::skip;
		else if (args[i] == "-sizeup")
			sizeFlag = SizeFlag::sizeup;
		else if (args[i] == "-sizedown")
			sizeFlag = SizeFlag::sizedown;
		else {
			std::cout << "unknown parameter \"" << args[i] << "\"" << std::endl;
			optionerr = true;
		}
		if (optionerr) {
			return 0;
		}
	}
	std::cout << "Mode:" << (sizeFlag == SizeFlag::skip ? "Skip" : sizeFlag == SizeFlag::sizeup ? "SizeUp" : sizeFlag == SizeFlag::sizedown ? "SizeDown" : "") << std::endl;

	std::cout << "outdir: " << outdirpath.string() << std::endl;
	for (const fsys::directory_entry& x : fsys::recursive_directory_iterator(indirpath)) {
		fsys::path relpath = x.path().lexically_relative(indirpath);
		fsys::path outpath = outdirpath.string() + "\\" + relpath.string();
		outpath = outpath.lexically_normal();
		std::cout << std::setw(24) << std::left << relpath.string() << " | " << std::setw(24) << std::left << outpath.string() << " | " << (x.is_directory()?"DIR":(x.is_regular_file()?"FILE":"")) << std::endl;
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
