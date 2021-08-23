#include <filesystem>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <istream>

namespace fsys = std::filesystem;

void help(){
	std::cout << "ディレクトリの内容を別のディレクトリにコピーします．" << std::endl;
	std::cout << "dmerge コピー元 コピー先 [オプション]\n" << std::endl; // Directory Merge

	std::cout << "　オプション" << std::endl;
	std::cout << "-skip                : 重複ファイルをスキップ(default)" << std::endl;
	std::cout << "-sizeup              : ファイルサイズ大優先" << std::endl;
	std::cout << "-sizedown            : ファイルサイズ小優先" << std::endl;
	std::cout << "-ext [ext1,ext2,...] : 指定の拡張子のみ実行" << std::endl;
	std::cout << "-dry                 : 実際にはファイル操作を行わない(ファイル名の列挙のみ)" << std::endl;
	std::cout << "-del                 : 処理済みファイルを削除" << std::endl;
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

inline bool copy_rename_file(const fsys::path& _From, const fsys::path& _To, bool is_rename = false) {
	if (is_rename) {
		return fsys::rename(_From, _To), true;
	}
	return fsys::copy_file(_From, _To, fsys::copy_options::overwrite_existing);
}

int Main(std::vector<std::string> args)
{
	if(args.size() < 3){
		help();
		return 0;
	}

	fsys::path indirpath = args[1];
	if (!fsys::is_directory(indirpath)) {
		std::cout << "コピー元ディレクトリが見つかりません." << std::endl;
		help();
		return 0;
	}
	fsys::path outdirpath = args[2];
	if (!fsys::is_directory(outdirpath)) {
		std::cout << "コピー先ディレクトリが見つかりません." << std::endl;
		help();
		return 0;
	}

	enum class SizeFlag{skip, sizeup, sizedown,err};
	SizeFlag sizeFlag = SizeFlag::skip;
	bool enddel = false;
	bool dry = false;
	bool ext_filter = false;
	std::vector<std::string> ext_list;
	bool optionerr = false;
	for (unsigned int i = 3; i < args.size(); ++i) {
		if (args[i] == "-skip")
			sizeFlag = SizeFlag::skip;
		else if (args[i] == "-sizeup")
			sizeFlag = SizeFlag::sizeup;
		else if (args[i] == "-sizedown")
			sizeFlag = SizeFlag::sizedown;
		else if (args[i] == "-del")
			enddel = true;
		else if (args[i] == "-dry")
			dry = true;
		else if (args[i] == "-ext") {
			ext_filter = true;
			std::stringstream ss{ args[++i] };
			std::string buf;
			while (std::getline(ss, buf, ',')) {
				ext_list.push_back(buf);
			}
		}
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
	std::vector<fsys::directory_entry> srclist;
	for (const fsys::directory_entry& x : fsys::recursive_directory_iterator(indirpath)) {
		if(!ext_filter || x.is_directory())
			srclist.push_back(x);
		else {
			for (const auto ext : ext_list) {
				if (x.path().extension() == ext || x.path().extension() == "." + ext) {
					srclist.push_back(x);
				}
			}
		}
	}
	for (const fsys::directory_entry& x : srclist) {
		fsys::path relpath = x.path().lexically_relative(indirpath);
		fsys::path outpath = outdirpath.string() + "\\" + relpath.string();
		outpath = outpath.lexically_normal();

		std::cout << "[" << std::setw(GetDigit(srclist.size())) << ++filecount << "/" << srclist.size() << "]";

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
						if (enddel) {
							std::cout << "[DEL ]";
							std::cout << x.path().string() << " > " << outpath.string() << std::endl;
							fsys::remove(x);
						}
						else {
							std::cout << "[SKIP]";
							std::cout << x.path().string() << " > " << outpath.string() << std::endl;
						}
					}
					else{
						std::cout << x.path().string() << " > " << outpath.string() << std::endl;
						if (!dry)
							copy_rename_file(x, outpath, enddel);
					}
				}
				else if (sizeFlag == SizeFlag::sizeup) {
					if (x.file_size() <= fsys::file_size(outpath)) {
						if (enddel) {
							std::cout << "[DEL ]";
							std::cout << x.path().string() << " > " << outpath.string() << std::endl;
							fsys::remove(x);
						}
						else {
							std::cout << "[SKIP]";
							std::cout << x.path().string() << " > " << outpath.string() << std::endl;
						}
					}
					else {
						std::cout << x.path().string() << " > " << outpath.string() << std::endl;
						if (!dry)
							copy_rename_file(x, outpath, enddel);
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
					copy_rename_file(x, outpath, enddel);
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
