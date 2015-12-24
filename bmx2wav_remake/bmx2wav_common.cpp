#include "bmx2wav_common.h"

#include <sstream>
#include <sys/stat.h>
#include <wchar.h>

std::wstring to_wstring (int i) { std::wostringstream wss; wss << i; return wss.str(); }

namespace IO {
	std::wstring substitute_extension(const std::wstring& filename, const std::wstring& newext) {
		auto i = filename.find_last_of(L'.');
		if (i == std::wstring::npos)
			return filename + newext;
		return filename.substr(0, i) + newext;
	}

	std::wstring get_filedir(const std::wstring& filename) {
		return filename.substr(0, filename.find_last_of(PATH_SEPARATOR_CHAR));
	}

	std::wstring get_filename(const std::wstring& filename) {
		auto i = filename.find_last_of(PATH_SEPARATOR_CHAR);
		if (i == std::wstring::npos)
			return L"";
		return filename.substr(i + 1);
	}

	bool is_file_exists(const std::wstring& filename) {
		FILE *f;
		_wfopen_s(&f, filename.c_str(), L"r");
		if (!f)
			return false;
		fclose(f);
		return true;
	}

	bool is_directory_exists(const std::wstring& dirpath) {
		struct _stat64i32 s;
		if (_wstat(dirpath.c_str(), &s) == 0) {
			if (s.st_mode & S_IFDIR)
				return true;
			else
				return false;
		}
		else {
			return false;
		}
	}

	std::wstring get_parentdir(const std::wstring& dirpath) {
		auto i = dirpath.find_last_of(PATH_SEPARATOR_CHAR);
		if (i == std::wstring::npos)
			return L"";
		return dirpath.substr(0, i);
	}

	bool create_directory(const std::wstring& filepath) {
		return (_wmkdir(filepath.c_str()) == 0);
	}

	bool make_parent_directory_recursive(const std::wstring& filepath)
	{
		// if current directory is not exist
		// then get parent directory
		// and check it recursively
		// after that, create own.
		if (is_directory_exists(filepath))
			return true;
		if (is_file_exists(filepath))
			return false;
		if (!create_directory(filepath.c_str())) {
			std::wstring parent = get_parentdir(filepath);
			if (NOT(make_parent_directory_recursive(parent))) {
				return false;
			}
			return create_directory(filepath.c_str());
		}
	}
}