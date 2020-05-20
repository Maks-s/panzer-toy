#ifndef PANZERTOY_LOG_HPP
#define PANZERTOY_LOG_HPP

#include <iostream>
#include <string>

/**
 * @namespace Log
 *
 * @brief Log text directly to console
 */
namespace Log {
	template<typename T>
	void error(T msg) {
		std::cerr << msg << '\n';
	}

	template<typename T, typename... Args>
	void error(T first, Args... args) {
		std::cerr << first;
		error(args...);
	}

	template<typename T>
	void info(T msg) {
		std::cout << msg << '\n';
	}

	template<typename T, typename... Args>
	void info(T first, Args... args) {
		std::cout << first;
		info(args...);
	}

	template<typename T>
	void debug(T msg) {
		// Flushes the output stream buffer, so we can see the debug message faster
		std::clog << msg << std::endl;
	}

	template<typename T, typename... Args>
	void debug(T first, Args... args) {
		std::clog << first;
		debug(args...);
	}
}

#endif // PANZERTOY_LOG_HPP
