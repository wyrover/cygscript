#include "key.hpp"
#include <memory>

namespace registry
{

bool IKey::hasSubKey(const std::wstring& subkey) const {
	HKEY hKey;
	if(ERROR_SUCCESS != RegOpenKeyEx(
		   _hKey, subkey.c_str(), 0, KEY_READ, &hKey)) {
		return false;
	}
	RegCloseKey(hKey);
	return true;
}

void IKey::deleteSubTree(const std::wstring& subkey) const {
	LSTATUS status = RegDeleteTree(_hKey, subkey.c_str());
	if (ERROR_SUCCESS != status) {
		THROW_ERROR_CODE("Failed to delete registry tree", status);
	}
}

Key::Key(const IKey& parent, const std::wstring& subkey, REGSAM access) {
	HKEY hKey;
	LSTATUS status = RegOpenKeyEx(
		parent, subkey.c_str(), 0, access, &hKey);
	if (ERROR_SUCCESS != status) {
		THROW_ERROR_CODE("Failed to open registry key", status);
	}
	_hKey = hKey;
}

Key::~Key() {
	if (_hKey) {
		RegCloseKey(_hKey);
		_hKey = NULL;
	}
}

Key Key::create(const IKey& parent, const std::wstring& subkey, REGSAM access) {
	HKEY hKey;
	LONG result = RegCreateKeyEx(
		parent, subkey.c_str(), 0, NULL, 0,
		access, NULL, &hKey, NULL);
	if (ERROR_SUCCESS != result) {
		THROW_ERROR_CODE("Failed to create registry key", result);
	}
	return Key(hKey);
}

bool Key::valueExists(const std::wstring& name, DWORD type) const {
	LONG status = RegGetValue(
		_hKey, NULL, name.c_str(), type & 0xffff,
		NULL, NULL, NULL);
	if (ERROR_SUCCESS == status) {
		return true;
	}
	return false;
}

const Key& Key::setString(const std::wstring& name,
                          const std::wstring& value) const {
	LONG status = RegSetValueEx(
		_hKey, name.c_str(), 0, REG_SZ,
		(LPBYTE)value.c_str(),
		(DWORD)(value.length() + 1) * sizeof(std::wstring::value_type));
	if (ERROR_SUCCESS != status) {
		THROW_ERROR_CODE("Failed to store string registry value", status);
	}
	return *this;
}

std::wstring Key::getString(const std::wstring& name) const {
	wchar_t buf[256];
	std::wstring ret;
	DWORD len = sizeof(buf);
	LONG status = RegGetValue(
		_hKey, NULL, name.c_str(), RRF_RT_REG_SZ,
		NULL, buf, &len);
	/* if string fits to buffer */
	if (ERROR_SUCCESS == status) {
		/* convert returned value size to number of wchar_t's without null */
		ret.assign(buf, (len >> 1) - 1);
	}
	/* no value */
	else if (ERROR_FILE_NOT_FOUND == status) {
		/* pass, return empty string */
	}
	/* larger buffer required */
	else if (ERROR_MORE_DATA == status) {
		std::unique_ptr<char[]> tmp(new char[len]);
		status = RegGetValue(
			_hKey, NULL, name.c_str(), RRF_RT_REG_SZ,
			NULL, tmp.get(), &len);
		if (ERROR_SUCCESS == status) {
			ret.assign(reinterpret_cast<wchar_t*>(tmp.get()), (len >> 1) - 1);
		}
	}
	/* if RegGetValue failed either on first or second attempt */
	if (ERROR_SUCCESS != status) {
		THROW_ERROR_CODE("Failed to fetch string registry value", status);
	}
	return ret;
}

const Key& Key::setDword(const std::wstring& name, DWORD value) const {
	LONG status = RegSetValueEx(
		_hKey, name.c_str(), 0, REG_DWORD,
		(BYTE *)&value,
		(DWORD)sizeof(DWORD));
	if (ERROR_SUCCESS != status) {
		THROW_ERROR_CODE("Failed to store DWORD registry value", status);
	}
	return *this;
}

}
