#include <Windows.h>
#include <cassert>
#include "win32.hpp"

std::string soundsphere::wide_to_utf8(WCHAR* src)
{
	std::string ret;

	int target_len = WideCharToMultiByte(CP_UTF8, 0, src, -1, NULL, 0, NULL, NULL);
	if (target_len == 0)
	{
		return ret;
	}

	char* buf = (char*)malloc(target_len);
	if (buf == NULL)
	{
		return ret;
	}

	int r = WideCharToMultiByte(CP_UTF8, 0, src, -1, buf, target_len, NULL, NULL);
	assert(r == target_len); (void)r;

	ret = buf;
	free(buf);

	return ret;
}

soundsphere::wstring soundsphere::utf8_to_wide(const char* src)
{
	int pathw_len = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0);
	if (pathw_len == 0)
	{
		return NULL;
	}

	size_t buf_sz = pathw_len * sizeof(WCHAR);
	WCHAR* buf = (WCHAR*)malloc(buf_sz);
	if (buf == NULL)
	{
		return NULL;
	}

	int r = MultiByteToWideChar(CP_UTF8, 0, src, -1, buf, pathw_len);
	assert(r == pathw_len); (void)r;

	return soundsphere::wstring(buf, [](WCHAR* s) {free(s); });
}
