#include <filesystem>

#ifdef _WIN32
	#define VC_EXTRALEAN
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#else
	#include <system_error>
#endif

#include "game.hpp"

/**
 * @brief The almighty main
 *
 * cd to the executable directory, or keep the current one if we can't.
 * This prevent issues with relative paths.
 */
int main() {
	std::filesystem::path executable_path;

	#ifdef _WIN32
		std::vector<wchar_t> buf;
		DWORD length;

		do {
			buf.resize(buf.size() + MAX_PATH);
			length = GetModuleFileNameW(nullptr, buf.data(), buf.size());
		} while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);

		buf.resize(length);
		executable_path = std::wstring(buf.begin(), buf.end());
	#else
		// UNTESTED
		std::error_code code;
		executable_path = std::filesystem::read_symlink("/proc/self/exe", code);

		if (code.value() != 0) {
			executable_path.clear();
		}
	#endif

	if (!executable_path.empty()) {
		std::filesystem::current_path(executable_path.parent_path());
	}

	Game game;
	game.run();
	return 0;
}
