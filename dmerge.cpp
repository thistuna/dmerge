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
	std::cout << "-dry      : ���ۂɂ̓t�@�C��������s��Ȃ�(�t�@�C�����̗񋓂̂�)" << std::endl;
//	std::cout << "-del      : �����ς݃t�@�C�����폜" << std::endl;
	exit(0);
}

int GetDigit(int num) {
	if (!num)return 1;
	if (num < 0)num = -num;
	int count = 0;
	while (num) {
		num /= 10;
		count++;
	}
	return count;
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
	//bool enddel = false;
	bool dry = false;
	bool optionerr = false;
	for (int i = 3; i < args.size(); ++i) {
		if (args[i] == "-skip")
			sizeFlag = SizeFlag::skip;
		else if (args[i] == "-sizeup")
			sizeFlag = SizeFlag::sizeup;
		else if (args[i] == "-sizedown")
			sizeFlag = SizeFlag::sizedown;
//		else if (args[i] == "-del")
//			enddel = true;
		else if (args[i] == "-dry")
			dry = true;
		else {
			std::cout << "unknown parameter \"" << args[i] << "\"" << std::endl;
			optionerr = true;
		}
		if (optionerr) {
			help();
			return 0;
		}
	}
	std::cout << "Mode:" << (sizeFlag == SizeFlag::skip ? "Skip" : sizeFlag == SizeFlag::sizeup ? "SizeUp" : sizeFlag == SizeFlag::sizedown ? "SizeDown" : "") << std::endl;

	std::cout << "outdir: " << outdirpath.string() << std::endl;

	int filecount = 0;
	int filenum = 0;
	for (const fsys::directory_entry& x : fsys::recursive_directory_iterator(indirpath)) {
		filenum++;
	}
	for (const fsys::directory_entry& x : fsys::recursive_directory_iterator(indirpath)) {
		fsys::path relpath = x.path().lexically_relative(indirpath);
		fsys::path outpath = outdirpath.string() + "\\" + relpath.string();
		outpath = outpath.lexically_normal();

		std::cout << "[" << std::setw(GetDigit(filenum)) << ++filecount << "/" << filenum << "]";

		if (fsys::exists(outpath)) {
			if (fsys::is_directory(outpath)) {
				std::cout << "[DIR ][SKIP]";
				std::cout << x.path().string() << " > " << outpath.string() << std::endl;
			}
			else if (fsys::is_regular_file(outpath)) {
				std::cout << "[FILE]";
				if (sizeFlag == SizeFlag::skip) {
					std::cout << "[SKIP]";
					std::cout << x.path().string() << " > " << outpath.string() << std::endl;
				}
				else if (sizeFlag == SizeFlag::sizedown) {
					if (x.file_size() >= fsys::file_size(outpath)) {
						std::cout << "[SKIP]";
						std::cout << x.path().string() << " > " << outpath.string() << std::endl;
					}
					else{
						std::cout << x.path().string() << " > " << outpath.string() << std::endl;
						if (!dry)
							fsys::copy_file(x,outpath,fsys::copy_options::overwrite_existing);
					}
				}
				else if (sizeFlag == SizeFlag::sizeup) {
					if (x.file_size() <= fsys::file_size(outpath)) {
						std::cout << "[SKIP]";
						std::cout << x.path().string() << " > " << outpath.string() << std::endl;
					}
					else {
						std::cout << x.path().string() << " > " << outpath.string() << std::endl;
						if (!dry)
							fsys::copy_file(x, outpath, fsys::copy_options::overwrite_existing);
					}
				}
			}
		}
		else {
			if (x.is_directory()) {
				std::cout << "[DIR ]";
				std::cout << x.path().string() << " > " << outpath.string() << std::endl;
				if (!dry)
					fsys::create_directory(outpath);
			}
			else if (x.is_regular_file()){
				std::cout << "[FILE]";
				std::cout << x.path().string() << " > " << outpath.string() << std::endl;
				if (!dry)
					fsys::copy_file(x, outpath);
			}
		}
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
