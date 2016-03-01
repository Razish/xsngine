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
		) XS_WARN_UNUSED_RESULT;

		int Compare(
			const char *s1,
			const char *s2,
			size_t len = 4096
		) XS_WARN_UNUSED_RESULT;

		// std::string manipulation
		std::string Format(
			const char *fmt,
			...
		) XS_WARN_UNUSED_RESULT;

		void Split(
			const std::string &s,
			char delim,
			std::vector<std::string> &elems
		);

		void Join(
			const std::vector<std::string> &strings,
			const std::string &separator,
			std::string &out
		);

	} // namespace String

} // namespace XS
