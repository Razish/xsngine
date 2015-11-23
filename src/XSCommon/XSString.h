#pragma once

#include <string>
#include <vector>

namespace XS {

	namespace String {

		// C string manipulation

		void Concatenate(
			char *dst,
			size_t len,
			const char *src
		);

		void Copy(
			char *dst,
			const char *src,
			size_t len
		);

		int FormatBuffer(
			char *dst,
			size_t len,
			const char *fmt,
			...
		);

		int CompareCase(
			const char *s1,
			const char *s2,
			size_t len = 4096
		);

		int Compare(
			const char *s1,
			const char *s2,
			size_t len = 4096
		);

		// return a mutable copy of a constant string
		// if len is zero it will be calculated as strlen + null-terminator
		// caller must delete[] the return value
		char *AllocMutable(
			const char *src,
			size_t len = 0
		) XS_WARN_UNUSED_RESULT;


		// std::string manipulation

		std::string Format(
			const char *fmt,
			...
		);

		std::vector<std::string> &Split(
			const std::string &s,
			char delim,
			std::vector<std::string> &elems
		);

		std::vector<std::string> Split(
			const std::string &s,
			char delim
		);

		std::string Join(
			const std::vector<std::string> &strings,
			const std::string &separator
		);

	} // namespace String

} // namespace XS
