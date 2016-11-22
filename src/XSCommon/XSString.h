#pragma once

#include <string>
#include <vector>

namespace String {

	// C string manipulation

	void Concatenate(
		char *dst,
		size_t len,
		const char *src
	);

	// safe strncpy that ensures a trailing zero
	void Copy(
		char *dst,
		const char *src,
		size_t len
	);

	// safe sprintf-like
	int FormatBuffer(
		char *dst,
		size_t len,
		const char *fmt,
		...
	);

	// strncmp
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


	std::string &Strip(
		std::string &s,
		const std::string &chars
	);

	// version of the above that does not write in-place
	std::string StripCopy(
		std::string s,
		const std::string &chars
	);

	std::string &StripNonDigit(
		std::string &s
	);

	// version of the above that does not write in-place
	std::string StripNonDigitCopy(
		std::string s
	);

	std::string &StripIfNot(
		std::string &s,
		const std::string &allowed
	);

	// version of the above that does not write in-place
	std::string StripIfNotCopy(
		std::string s,
		const std::string &allowed
	);

} // namespace String
