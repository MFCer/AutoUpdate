/*
  unlocker -- A header-only, fast, simple unlocker library under Ring3 for Windows.

  Copyright (c) 2010-2017 <http://ez8.co> <orca.zhang@yahoo.com>
  This library is released under the MIT License.

  Please see LICENSE file or visit https://github.com/ez8-co/unlocker for details.
*/
#pragma once

#include <tchar.h>
#include <windows.h>

#include <deque>
#include <vector>
#include <map>
using namespace std;

#include <string>
typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > tstring;

#include <psapi.h>
#pragma comment(lib, "psapi.lib")

#include <TlHelp32.h>

#define STATUS_SUCCESS				((NTSTATUS)0x00000000L)
#define STATUS_INFO_LENGTH_MISMATCH	((NTSTATUS)0xC0000004L)
#define STATUS_BUFFER_OVERFLOW		((NTSTATUS)0x80000005L)

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status)			(((NTSTATUS)(Status)) >= 0)
#endif

#ifndef UNICODE
	static string _W2T(const wchar_t* wcs)
	{
		int len = ::WideCharToMultiByte(CP_ACP, 0, wcs, -1, NULL, 0, 0, 0);
		string ret(len, 0);
		VERIFY(0 != ::WideCharToMultiByte(CP_ACP, 0, wcs, -1, &ret[0], len, 0, 0));
		ret.resize(len - 1);
		return ret;
	}
#else
	#define _W2T(str) wstring(str)
#endif

namespace unlocker {
	
	template<typename T = HANDLE, BOOL (__stdcall *Closer)(T) = CloseHandle>
	class SmartHandleTmpl
	{
		SmartHandleTmpl(const SmartHandleTmpl&);
		SmartHandleTmpl& operator=(const SmartHandleTmpl&);

	public:
		SmartHandleTmpl(T handle = NULL) : _handle(handle) {}
		~SmartHandleTmpl() {
			if (_handle) Closer(_handle);
		}

		operator T() const {return _handle;}
		template<typename F>
		operator F*() const {return (F*)_handle;}
		T* operator&() {return &_handle;}
		T operator=(T handle) {
			if (_handle != handle && _handle) Closer(_handle);
			return _handle = handle;
		}

	private:
		T _handle;
	};

	typedef SmartHandleTmpl<> SmartHandle;

	class File;

	class Path
	{
	public:
		Path(const tstring& path) : _path() {
			if (path.length() > 2 && !_tcsnicmp(path.c_str(), _T("\\\\"), 2))
				_path = path;
			else {
				_path = _T("\\\\?\\");
				_path += path;
			}
		}
		static tstring Combine(const tstring& prefix, const tstring& path) {
			tstring ret(prefix);
			ret += '\\';
			tstring::size_type pos = path.find_first_not_of('\\');
			if (pos != tstring::npos) {
				ret.append(path, pos, path.length() - pos);
			}
			return ret;
		}
		const TCHAR* GetDevicePath() const { return &_path[0]; }
		operator tstring() const { return _path.substr(4, _path.length() - 4); }
		operator const TCHAR*() const { return &_path[4]; }

		static File* Exists(const tstring& path);

		static BOOL Contains(const tstring& path, const tstring& sub_path) {
			return path.length() <= sub_path.length()
				&& !_tcsnicmp(path.c_str(), sub_path.c_str(), path.length())
				&& (path.length() == sub_path.length() || path[path.length() - 1] == '\\' || sub_path[path.length()] == '\\');
		}

	private:
		tstring _path;
	};

	namespace {
		BOOL UnholdFile(const tstring& path);
	}

	class File
	{
	public:
		File(const tstring& path) : _path(path) {}
		virtual operator Path() const { return _path; }
		virtual const TCHAR* GetDevicePath() const { return _path.GetDevicePath(); }
		virtual BOOL Unlock() {
			class PrivilegeHelper
			{
			public:
				PrivilegeHelper() { SetPrivilege(SE_DEBUG_NAME, TRUE); }
				~PrivilegeHelper() { SetPrivilege(SE_DEBUG_NAME, FALSE); }

			private:
				BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
				{
					SmartHandle hToken;
					OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
					LUID luid;
					if (!LookupPrivilegeValue(NULL, lpszPrivilege, &luid)) return FALSE;
					TOKEN_PRIVILEGES tp = { 1,{ luid, (DWORD)(bEnablePrivilege ? SE_PRIVILEGE_ENABLED : 0) } };
					AdjustTokenPrivileges(hToken, FALSE, &tp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
					return (GetLastError() == ERROR_SUCCESS);
				}
			} helper;
			return UnholdFile(_path);
		}
		virtual BOOL ForceDelete() {
			return Delete() || (Unlock() && Delete());
		}
		virtual BOOL Delete() {
			SetFileAttributes(_path, FILE_ATTRIBUTE_NORMAL);
			return DeleteFile(_path);
		}

	protected:
		Path _path;
	};

	class Dir : public File
	{
	public:
		Dir(const tstring& path) : File((!path.empty() && path[path.length() - 1] == '\\') ? path.substr(0, path.length() - 1) : path) {}
		virtual BOOL Delete() {
			deque<Dir> dirs;
			dirs.push_back(Dir(_path));
			while (!dirs.empty()) {
				Path dir(dirs.front());
				WIN32_FIND_DATA fd;
				SmartHandleTmpl<HANDLE, FindClose> hSearch = FindFirstFile(Path::Combine(dir.GetDevicePath(), _T("*")).c_str(), &fd);
				if (hSearch == INVALID_HANDLE_VALUE) // try to examine root directory
					hSearch = FindFirstFile(Path::Combine(dir, _T("*")).c_str(), &fd);
				if (hSearch == INVALID_HANDLE_VALUE)
					return FALSE;
				INT subDirCnt = 0;
				do {
					if (!_tcscmp(fd.cFileName, _T(".")) || !_tcscmp(fd.cFileName, _T("..")))
						continue;
					else if (fd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY)) {
						++subDirCnt;
						dirs.push_front(Dir(Path::Combine(dir, fd.cFileName)));
					}
					else {
						if (!File(Path::Combine(dir, fd.cFileName)).Delete())
							return FALSE;
					}
				} while (FindNextFile(hSearch, &fd) || GetLastError() != ERROR_NO_MORE_FILES);
				if (!subDirCnt) {
					if (!Dir(dir).DeleteDir())
						return FALSE;
					dirs.pop_front();
				}
			}
			return TRUE;
		}
		BOOL DeleteDir() {
			// add backslash for unacceptable-name files
			tstring path(Path::Combine(_path.GetDevicePath(), _T("")));
			SetFileAttributes(path.c_str(), FILE_ATTRIBUTE_NORMAL);
			return RemoveDirectory(path.c_str());
		}
	};

	inline File* Path::Exists(const tstring& path) {
		Path filePath(path);
		WIN32_FIND_DATA fd;
		SmartHandleTmpl<HANDLE, FindClose> hSearch = FindFirstFile(filePath.GetDevicePath(), &fd);
		if (hSearch == INVALID_HANDLE_VALUE) // try to examine root directory
			hSearch = FindFirstFile(filePath, &fd);
		if (hSearch != INVALID_HANDLE_VALUE)
			if (fd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY))
				return new Dir(filePath);
			else
				return new File(filePath);
		return NULL;
	}

	namespace {
		template <class T>
		struct _UNICODE_STRING_T {
			union {
				struct {
					WORD Length;
					WORD MaximumLength;
				};
				T dummy;
			};
			T Buffer;
		};

		typedef struct _OBJECT_NAME_INFORMATION {
			_UNICODE_STRING_T<PWSTR> Name;
			WCHAR NameBuffer[1];
		} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

		typedef enum _OBJECT_INFORMATION_CLASS {
			ObjectNameInformation = 1,
			ObjectTypeInformation = 2,
		} OBJECT_INFORMATION_CLASS, *POBJECT_INFORMATION_CLASS;

		typedef enum _POOL_TYPE {
			NonPagedPool,
			// omit unused enumerations
		} POOL_TYPE, *PPOOL_TYPE;

		typedef struct _OBJECT_TYPE_INFORMATION {
			_UNICODE_STRING_T<PWSTR> Name;
			// omit unused fields
		} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;

		typedef NTSTATUS (WINAPI *NT_QUERY_OBJECT)(
			IN HANDLE Handle OPTIONAL,
			IN OBJECT_INFORMATION_CLASS ObjectInformationClass,
			OUT PVOID ObjectInformation OPTIONAL,
			IN ULONG ObjectInformationLength,
			OUT PULONG ReturnLength OPTIONAL);

		typedef struct _SYSTEM_HANDLE {
			HANDLE ProcessId;
			BYTE ObjectType;
			BYTE Flags;
			WORD Handle;
			PVOID Address;
			DWORD GrantedAccess;
		} SYSTEM_HANDLE, *PSYSTEM_HANDLE;

		typedef struct _SYSTEM_HANDLE_INFORMATION {
			DWORD HandleCount;
			SYSTEM_HANDLE Handles[1];
		} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

		typedef struct _SYSTEM_HANDLE_EX {
			PVOID Object;
			HANDLE ProcessId;
			HANDLE Handle;
			ULONG GrantedAccess;
			USHORT CreatorBackTraceIndex;
			USHORT ObjectTypeIndex;
			ULONG HandleAttributes;
			ULONG Reserved;
		} SYSTEM_HANDLE_EX, *PSYSTEM_HANDLE_EX;

		typedef struct _SYSTEM_HANDLE_INFORMATION_EX {
			ULONG_PTR HandleCount;
			ULONG_PTR Reserved;
			SYSTEM_HANDLE_EX Handles[1];
		} SYSTEM_HANDLE_INFORMATION_EX, *PSYSTEM_HANDLE_INFORMATION_EX;

		typedef enum _SYSTEM_INFORMATION_CLASS {
			SystemHandleInformation = 16,
			SystemHandleInformationEx = 64,
		} SYSTEM_INFORMATION_CLASS;

		typedef NTSTATUS (WINAPI *NT_QUERY_SYSTEM_INFORMATION)(
			IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
			OUT PVOID  SystemInformation,
			IN ULONG   SystemInformationLength,
			OUT PULONG ReturnLength OPTIONAL);

		typedef enum _SECTION_INFORMATION_CLASS {
			SectionBasicInformation = 0,
		} SECTION_INFORMATION_CLASS;

		typedef struct _SECTION_BASIC_INFORMATION {
			ULONG			SectionBaseAddress;
			ULONG			SectionAttributes;
			LARGE_INTEGER	SectionSize;
		} SECTION_BASIC_INFORMATION;

		template <typename T>
		struct _LIST_ENTRY_T {
			T Flink;
			T Blink;
		};

		template <typename T>
		struct _PEB_T {
			T dummy01;
			T Mutant;
			T ImageBaseAddress;
			T Ldr;
			// omit unused fields
		};

		typedef _PEB_T<DWORD> PEB32;
		typedef _PEB_T<DWORD64> PEB64;

		typedef struct _PROCESS_BASIC_INFORMATION64 {
			NTSTATUS ExitStatus;
			UINT32 Reserved0;
			UINT64 PebBaseAddress;
			UINT64 AffinityMask;
			UINT32 BasePriority;
			UINT32 Reserved1;
			UINT64 UniqueProcessId;
			UINT64 InheritedFromUniqueProcessId;
		} PROCESS_BASIC_INFORMATION64;

		typedef struct _PROCESS_BASIC_INFORMATION32 {
			NTSTATUS ExitStatus;
			UINT32 PebBaseAddress;
			UINT32 AffinityMask;
			UINT32 BasePriority;
			UINT32 UniqueProcessId;
			UINT32 InheritedFromUniqueProcessId;
		} PROCESS_BASIC_INFORMATION32;

		template <class T>
		struct _PEB_LDR_DATA_T {
			DWORD Length;
			DWORD Initialized;
			T SsHandle;
			_LIST_ENTRY_T<T> InLoadOrderModuleList;
			// omit unused fields
		};

		typedef _PEB_LDR_DATA_T<DWORD> PEB_LDR_DATA32;
		typedef _PEB_LDR_DATA_T<DWORD64> PEB_LDR_DATA64;

		template <class T>
		struct _LDR_DATA_TABLE_ENTRY_T {
			_LIST_ENTRY_T<T> InLoadOrderLinks;
			_LIST_ENTRY_T<T> InMemoryOrderLinks;
			_LIST_ENTRY_T<T> InInitializationOrderLinks;
			T DllBase;
			T EntryPoint;
			union {
				DWORD SizeOfImage;
				T dummy01;
			};
			_UNICODE_STRING_T<T> FullDllName;
			_UNICODE_STRING_T<T> BaseDllName;
			// omit unused fields
		};

		typedef _LDR_DATA_TABLE_ENTRY_T<DWORD> LDR_DATA_TABLE_ENTRY32;
		typedef _LDR_DATA_TABLE_ENTRY_T<DWORD64> LDR_DATA_TABLE_ENTRY64;

		typedef NTSTATUS (WINAPI *NT_QUERY_SECTION)(
			IN HANDLE	SectionHandle,
			IN SECTION_INFORMATION_CLASS	InformationClass,
			OUT PVOID	InformationBuffer,
			IN ULONG	InformationBufferSize,
			OUT PULONG	ResultLength OPTIONAL );

		typedef NTSTATUS (WINAPI *NT_QUERY_SYSTEM_INFORMATION)(
			IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
			OUT PVOID  SystemInformation,
			IN ULONG   SystemInformationLength,
			OUT PULONG ReturnLength OPTIONAL);

		static HMODULE hNtDll = LoadLibrary(_T("ntdll.dll"));
		static NT_QUERY_SYSTEM_INFORMATION NtQuerySystemInformation = (NT_QUERY_SYSTEM_INFORMATION)GetProcAddress(hNtDll, "NtQuerySystemInformation");
		static NT_QUERY_OBJECT NtQueryObject = (NT_QUERY_OBJECT)GetProcAddress(hNtDll, "NtQueryObject");
		static NT_QUERY_SECTION NtQuerySection = (NT_QUERY_SECTION)GetProcAddress(hNtDll, "NtQuerySection");
		static HMODULE hKernel32 = LoadLibrary(_T("kernel32.dll"));

#ifndef _WIN64

		#define ProcessBasicInformation 0

		typedef NTSTATUS(WINAPI *NT_WOW64_QUERY_INFORMATION_PROCESS64)(
			HANDLE ProcessHandle, UINT32 ProcessInformationClass,
			PVOID ProcessInformation, UINT32 ProcessInformationLength,
			UINT32* ReturnLength);

		typedef NTSTATUS(WINAPI *NT_WOW64_READ_VIRTUAL_MEMORY64)(
			HANDLE ProcessHandle, PVOID64 BaseAddress,
			PVOID BufferData, UINT64 BufferLength,
			PUINT64 ReturnLength);

		static NT_WOW64_QUERY_INFORMATION_PROCESS64 NtWow64QueryInformationProcess64 = (NT_WOW64_QUERY_INFORMATION_PROCESS64)GetProcAddress(hNtDll, "NtWow64QueryInformationProcess64");
		static NT_WOW64_READ_VIRTUAL_MEMORY64 NtWow64ReadVirtualMemory64 = (NT_WOW64_READ_VIRTUAL_MEMORY64)GetProcAddress(hNtDll, "NtWow64ReadVirtualMemory64");

#endif

		BOOL Is64BitOS()
		{
			SYSTEM_INFO systemInfo = { 0 };
			GetNativeSystemInfo(&systemInfo);
			return systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64
				|| systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64;
		}

		static BOOL is64BitOS = Is64BitOS();

		typedef struct _HOLDER_INFO {
			deque<HANDLE> openHandles;
			deque<HANDLE> mmfSections;
		} HOLDER_INFO;

		void GetDeviceDriveMap(map<tstring, tstring>& pathMapping)
		{
			DWORD driveMask = GetLogicalDrives();
			TCHAR drivePath[_MAX_DRIVE] = _T("A:");

			while (driveMask) {
				if (driveMask & 1) {
					TCHAR devicePath[_MAX_PATH] = {0};
					if (QueryDosDevice(drivePath, devicePath, _MAX_PATH)) {
						// UNC or Network drive
						if (GetDriveType(drivePath) == DRIVE_REMOTE) {
							// \Device\HGFS\;z:0000000000010289\vmware-host\Shared Folders -> \Device\HGFS\vmware-host\Shared Folders
							TCHAR *pos = devicePath;
							UCHAR count = 3;
							do {
								if (*pos == '\\' && !--count) {
									count = pos - devicePath;
									while (*++pos && *pos != '\\');
									while (devicePath[++count] = *++pos);
									break;
								}
							} while (*pos++);
						}
						pathMapping[devicePath] = drivePath;
					}
				}
				driveMask >>= 1;
				++drivePath[0];
			}
		}

		BOOL DevicePathToDrivePath(tstring& path)
		{
			static map<tstring, tstring> pathMapping;
			if (pathMapping.empty()) GetDeviceDriveMap(pathMapping);

			if (path.empty()) return TRUE;

			if (!_tcsnicmp(path.c_str(), _T("\\SystemRoot"), 11)) {
				TCHAR windowsRoot[_MAX_PATH] = {0};
				GetWindowsDirectory(windowsRoot, sizeof(windowsRoot));
				path.replace(0, 11, windowsRoot);
				return TRUE;
			}
			else if (!_tcsnicmp(path.c_str(), _T("\\??\\"), 4)) {
				path.erase(0, 4);
				return TRUE;
			}
			for (map<tstring, tstring>::const_iterator it=pathMapping.begin(); it!=pathMapping.end(); ++it) {
				if (!_tcsnicmp(path.c_str(), it->first.c_str(), it->first.length())) {
					path.replace(0, it->first.length(), it->second);
					return TRUE;
				}
			}
			return FALSE;
		}

		BOOL GetHandlePath(HANDLE handle, tstring& path)
		{
			if (!NtQueryObject) return FALSE;

			DWORD dwLength = 0;
			OBJECT_NAME_INFORMATION info = {0};
			NTSTATUS status = NtQueryObject(handle, ObjectNameInformation, &info, sizeof (info), &dwLength);
			if (status != STATUS_SUCCESS && status != STATUS_BUFFER_OVERFLOW && status != STATUS_INFO_LENGTH_MISMATCH) {
				return FALSE;
			}

			POBJECT_NAME_INFORMATION pInfo = (POBJECT_NAME_INFORMATION)malloc(dwLength);
			while (true) {
				status = NtQueryObject (handle, ObjectNameInformation, pInfo, dwLength, &dwLength);
				if (status != STATUS_BUFFER_OVERFLOW && status != STATUS_INFO_LENGTH_MISMATCH) {
					break;
				}
				pInfo = (POBJECT_NAME_INFORMATION)realloc(pInfo, dwLength);
			}

			BOOL bRet = FALSE;
			if (NT_SUCCESS(status)) {
				path = _W2T(pInfo->Name.Buffer ? pInfo->Name.Buffer : _T(""));
				bRet = DevicePathToDrivePath(path);
			}

			free(pInfo);
			return bRet;
		}

		template<typename SYS_HADNLE_INFO_TYPE, SYSTEM_INFORMATION_CLASS sys_info_class>
		SYS_HADNLE_INFO_TYPE* GetSystemHandleInfo()
		{
			if (!NtQuerySystemInformation) return NULL;

			DWORD dwLength = 0;
			SYS_HADNLE_INFO_TYPE shi = {0};
			NTSTATUS status = NtQuerySystemInformation(sys_info_class, &shi, sizeof (shi), &dwLength);
			if (status != STATUS_SUCCESS && status != STATUS_BUFFER_OVERFLOW && status != STATUS_INFO_LENGTH_MISMATCH) {
				return NULL;
			}

			SYS_HADNLE_INFO_TYPE* pshi = (SYS_HADNLE_INFO_TYPE*)malloc(dwLength);
			while (true) {
				status = NtQuerySystemInformation (sys_info_class, pshi, dwLength, &dwLength);
				if (status != STATUS_BUFFER_OVERFLOW && status != STATUS_INFO_LENGTH_MISMATCH) {
					break;
				}
				pshi = (SYS_HADNLE_INFO_TYPE*)realloc(pshi, dwLength);
			}

			if (!NT_SUCCESS (status)) {
				free (pshi);
				pshi = NULL;
			}

			return pshi;
		}

		// IsDeviceHandle 
		// e.g. \Device\Afd, \Device\Beep, \Device\KsecDD, \Device\NamedPipe\XXXX, \Device\WMIDataDevice, \Device\Null
		// thanks to Robert Simpson <http://stackoverflow.com/questions/16127948/hang-on-ntquerysysteminformation-in-winxpx32-but-works-fine-in-win7x64>
		BOOL IsDeviceHandle(HANDLE handle)
		{
			SmartHandle hMapFile = CreateFileMapping(handle, NULL, PAGE_READONLY, 0, 0, NULL);
			return !hMapFile && GetLastError() == ERROR_BAD_EXE_FORMAT;
		}

		// FindFileHandleHolders
		template<typename SYS_HADNLE_INFO_TYPE, SYSTEM_INFORMATION_CLASS sys_info_class>
		BOOL FindFileHandleHolders(LPCTSTR path, map<DWORD, HOLDER_INFO>& holders)
		{
			holders.clear ();

			if (!path || !NtQueryObject || !NtQuerySection) return FALSE;

			SYS_HADNLE_INFO_TYPE* pshi = GetSystemHandleInfo<SYS_HADNLE_INFO_TYPE, sys_info_class> ();
			if (!pshi) return FALSE;

			HANDLE hCrtProc = GetCurrentProcess ();
			for (ULONG_PTR i = 0; i < pshi->HandleCount; ++i) {
				// duplicate handle
				SmartHandle hDupHandle;
				SmartHandle hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, (DWORD)pshi->Handles[i].ProcessId);
				if (!hProcess || !DuplicateHandle(hProcess, (HANDLE)pshi->Handles[i].Handle, hCrtProc, &hDupHandle, 0, FALSE, DUPLICATE_SAME_ACCESS))
					continue;

				// filter out device handle (some of them may cause NtQueryObject hang up, e.g. some pipe handle)
				if (IsDeviceHandle(hDupHandle))
					continue;

				// filter out non-file or non-section handle (identify by ObjectIndex is not correct)
				POBJECT_TYPE_INFORMATION poti = (POBJECT_TYPE_INFORMATION)malloc(0x1000);
				NTSTATUS status = NtQueryObject(hDupHandle, ObjectTypeInformation, poti, 0x1000, NULL);
				if (NT_SUCCESS(status)) {
					if (!_wcsicmp(poti->Name.Buffer, L"File")) {
						tstring filePath;
						if (GetHandlePath(hDupHandle, filePath) && Path::Contains(path, filePath.c_str()))
							holders[(DWORD)pshi->Handles[i].ProcessId].openHandles.push_back((HANDLE)pshi->Handles[i].Handle);
					}
					else if (!_wcsicmp(poti->Name.Buffer, L"Section")) {
						SECTION_BASIC_INFORMATION sbi = {};
						if (NT_SUCCESS(NtQuerySection(hDupHandle, SectionBasicInformation, &sbi, sizeof(sbi), 0)) && sbi.SectionAttributes == SEC_FILE)
							holders[(DWORD)pshi->Handles[i].ProcessId].mmfSections.push_back((HANDLE)pshi->Handles[i].Handle);
					}
				}
				free(poti);
			}

			free(pshi);
			return TRUE;
		}

		BOOL CloseRemoteHandle(HANDLE hProcess, HANDLE hHandle)
		{
			SmartHandle hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "CloseHandle"), hHandle, 0, NULL);
			if (!hThread) return FALSE;
			return WaitForSingleObject(hThread, 1000) == WAIT_OBJECT_0;
		}

		BOOL CloseHandleWithProcess(DWORD dwProcessId, HANDLE hHandle)
		{
			SmartHandle hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, dwProcessId);
			if (hProcess) {
				SmartHandle hDup;
				if (DuplicateHandle(hProcess, hHandle, GetCurrentProcess(), &hDup, DUPLICATE_SAME_ACCESS, FALSE, DUPLICATE_CLOSE_SOURCE) && hDup)
					return TRUE;
			}
			return FALSE;
		}

		BOOL RemoteFreeLibrary(HANDLE hProcess, PVOID modBaseAddr)
		{
			SmartHandle hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(hNtDll, "LdrUnloadDll"), modBaseAddr, 0, NULL);
			if (!hThread) return FALSE;
			WaitForSingleObject(hThread, 1000);
			DWORD dwRet = 0;
			GetExitCodeThread(hThread, &dwRet);
			return (dwRet == ERROR_SUCCESS);
		}

#ifndef _WIN64

		DWORD64 GetModuleHandle64(HANDLE hProcess, const tstring& path, BOOL isPath = TRUE)
		{
			if (!NtWow64QueryInformationProcess64 || !NtWow64ReadVirtualMemory64) return 0;

			PROCESS_BASIC_INFORMATION64 pbi = { 0 };
			NTSTATUS status = NtWow64QueryInformationProcess64(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), NULL);
			if (!NT_SUCCESS(status)) return 0;

			PEB64 peb;
			status = NtWow64ReadVirtualMemory64(hProcess, (PVOID64)pbi.PebBaseAddress, &peb, sizeof(peb), NULL);
			if (!NT_SUCCESS(status)) return 0;

			PEB_LDR_DATA64 ldr;
			status = NtWow64ReadVirtualMemory64(hProcess, (PVOID64)peb.Ldr, (PVOID)&ldr, sizeof(ldr), NULL);
			if (!NT_SUCCESS(status)) return 0;

			DWORD64 LastEntry = peb.Ldr + offsetof(PEB_LDR_DATA64, InLoadOrderModuleList);

			LDR_DATA_TABLE_ENTRY64 head;
			head.InLoadOrderLinks.Flink = ldr.InLoadOrderModuleList.Flink;
			do {
				status = NtWow64ReadVirtualMemory64(hProcess, (PVOID64)head.InLoadOrderLinks.Flink, (PVOID)&head, sizeof(LDR_DATA_TABLE_ENTRY64), NULL);
				if (!NT_SUCCESS(status)) continue;

				_UNICODE_STRING_T<DWORD64>* name = isPath ? &head.FullDllName : &head.BaseDllName;
				wstring modName((size_t)name->MaximumLength, 0);
				status = NtWow64ReadVirtualMemory64(hProcess, (PVOID64)name->Buffer, (PVOID)&modName[0], name->MaximumLength, NULL);
				if (!NT_SUCCESS(status)) continue;

				if (!_tcsicmp(path.c_str(), _W2T(modName).c_str()))
					return head.DllBase;
			} while (head.InLoadOrderLinks.Flink != LastEntry);
			return 0;
		}

		DWORD64 GetProcAddress64(const char* funcName)
		{
			SmartHandle hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
			static DWORD64 hNtdll64 = GetModuleHandle64(hProcess, _T("ntdll.dll"), FALSE);

			if (!hNtdll64 || !NtWow64ReadVirtualMemory64) return 0;

			IMAGE_DOS_HEADER idh;
			NTSTATUS status = NtWow64ReadVirtualMemory64(hProcess, (PVOID64)hNtdll64, (PVOID)&idh, sizeof(idh), NULL);
			if (!NT_SUCCESS(status)) return 0;

			IMAGE_NT_HEADERS64 inh;
			status = NtWow64ReadVirtualMemory64(hProcess, (PVOID64)(hNtdll64 + idh.e_lfanew), (PVOID)&inh, sizeof(inh), NULL);
			if (!NT_SUCCESS(status)) return 0;

			IMAGE_DATA_DIRECTORY& idd = inh.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
			if (!idd.VirtualAddress)return 0;

			IMAGE_EXPORT_DIRECTORY ied;
			status = NtWow64ReadVirtualMemory64(hProcess, (PVOID64)(hNtdll64 + idd.VirtualAddress), (PVOID)&ied, sizeof(ied), NULL);
			if (!NT_SUCCESS(status)) return 0;

			vector<DWORD> nameTable(ied.NumberOfNames);
			status = NtWow64ReadVirtualMemory64(hProcess, (PVOID64)(hNtdll64 + ied.AddressOfNames), (PVOID)&nameTable[0], sizeof(DWORD) * ied.NumberOfNames, NULL);
			if (!NT_SUCCESS(status)) return 0;

			for (DWORD i = 0; i < ied.NumberOfNames; ++i) {
				string func(strlen(funcName), 0);
				status = NtWow64ReadVirtualMemory64(hProcess, (PVOID64)(hNtdll64 + nameTable[i]), (PVOID)&func[0], strlen(funcName), NULL);
				if (!NT_SUCCESS(status)) continue;

				if (func == funcName) {
					WORD ord = 0;
					status = NtWow64ReadVirtualMemory64(hProcess, (PVOID64)(hNtdll64 + ied.AddressOfNameOrdinals + i * sizeof(WORD)), (PVOID)&ord, sizeof(WORD), NULL);
					if (!NT_SUCCESS(status)) continue;

					DWORD rva = 0;
					status = NtWow64ReadVirtualMemory64(hProcess, (PVOID64)(hNtdll64 + ied.AddressOfFunctions + ord * sizeof(DWORD)), (PVOID)&rva, sizeof(DWORD), NULL);
					if (!NT_SUCCESS(status)) continue;

					return hNtdll64 + rva;
				}
			}
			return 0;
		}

	#define _(x) __asm __emit (x)
		__declspec(naked) DWORD64 FakeCall(DWORD64 func, ...)
		{
			_(0x55)_(0x8b)_(0xec)_(0x83)_(0xec)_(0x40)_(0x53)_(0x56)_(0x57)_(0x8b)_(0x45)_(0x10)_(0x0f)_(0x57)_(0xc0)_(0x89)
			_(0x45)_(0xe0)_(0x8b)_(0x45)_(0x14)_(0x89)_(0x45)_(0xe4)_(0x8b)_(0x45)_(0x18)_(0x89)_(0x45)_(0xd8)_(0x8b)_(0x45)
			_(0x1c)_(0x89)_(0x45)_(0xdc)_(0x8b)_(0x45)_(0x20)_(0x89)_(0x45)_(0xd0)_(0x8b)_(0x45)_(0x24)_(0x89)_(0x45)_(0xd4)
			_(0x8b)_(0x45)_(0x28)_(0x89)_(0x45)_(0xc8)_(0x8b)_(0x45)_(0x2c)_(0x89)_(0x45)_(0xcc)_(0x8d)_(0x45)_(0x28)_(0x99)
			_(0x83)_(0xc0)_(0x08)_(0x66)_(0x0f)_(0x13)_(0x45)_(0xe8)_(0x83)_(0xd2)_(0x00)_(0x89)_(0x45)_(0xc0)_(0x89)_(0x55)
			_(0xc4)_(0xc7)_(0x45)_(0xf0)_(0x06)_(0x00)_(0x00)_(0x00)_(0xc7)_(0x45)_(0xf4)_(0x00)_(0x00)_(0x00)_(0x00)_(0xc7)
			_(0x45)_(0xfc)_(0x00)_(0x00)_(0x00)_(0x00)_(0xc7)_(0x45)_(0xf8)_(0x00)_(0x00)_(0x00)_(0x00)_(0x66)_(0x8c)_(0x65)
			_(0xf8)_(0xb8)_(0x2b)_(0x00)_(0x00)_(0x00)_(0x66)_(0x8e)_(0xe0)_(0x89)_(0x65)_(0xfc)_(0x83)_(0xe4)_(0xf0)_(0x6a)
			_(0x33)_(0xe8)_(0x00)_(0x00)_(0x00)_(0x00)_(0x83)_(0x04)_(0x24)_(0x05)_(0xcb)_(0x48)_(0x8b)_(0x4d)_(0xe0)_(0x48)
			_(0x8b)_(0x55)_(0xd8)_(0xff)_(0x75)_(0xd0)_(0x49)_(0x58)_(0xff)_(0x75)_(0xc8)_(0x49)_(0x59)_(0x48)_(0x8b)_(0x45)
			_(0xf0)_(0xa8)_(0x01)_(0x75)_(0x03)_(0x83)_(0xec)_(0x08)_(0x57)_(0x48)_(0x8b)_(0x7d)_(0xc0)_(0x48)_(0x85)_(0xc0)
			_(0x74)_(0x16)_(0x48)_(0x8d)_(0x7c)_(0xc7)_(0xf8)_(0x48)_(0x85)_(0xc0)_(0x74)_(0x0c)_(0xff)_(0x37)_(0x48)_(0x83)
			_(0xef)_(0x08)_(0x48)_(0x83)_(0xe8)_(0x01)_(0xeb)_(0xef)_(0x48)_(0x83)_(0xec)_(0x20)_(0xff)_(0x55)_(0x08)_(0x48)
			_(0x8b)_(0x4d)_(0xf0)_(0x48)_(0x8d)_(0x64)_(0xcc)_(0x20)_(0x5f)_(0x48)_(0x89)_(0x45)_(0xe8)_(0xe8)_(0x00)_(0x00)
			_(0x00)_(0x00)_(0xc7)_(0x44)_(0x24)_(0x04)_(0x23)_(0x00)_(0x00)_(0x00)_(0x83)_(0x04)_(0x24)_(0x0d)_(0xcb)_(0x66)
			_(0x8c)_(0xd8)_(0x66)_(0x8e)_(0xd0)_(0x8b)_(0x65)_(0xfc)_(0x66)_(0x8b)_(0x45)_(0xf8)_(0x66)_(0x8e)_(0xe0)_(0x8b)
			_(0x45)_(0xe8)_(0x8b)_(0x55)_(0xec)_(0x5f)_(0x5e)_(0x5b)_(0x8b)_(0xe5)_(0x5d)_(0xc3)
		}
	#undef _

		BOOL RemoteFreeLibrary64(HANDLE hProcess, DWORD64 modBaseAddr)
		{
			static DWORD64 RtlCreateUserThread64 = GetProcAddress64("RtlCreateUserThread");
			if (!RtlCreateUserThread64) return FALSE;
			static DWORD64 LdrUnloadDll64 = GetProcAddress64("LdrUnloadDll");
			if (!LdrUnloadDll64) return FALSE;

			return NT_SUCCESS((NTSTATUS)FakeCall(RtlCreateUserThread64,
				(DWORD64)hProcess,// ProcessHandle
				(DWORD64)NULL,	  // SecurityDescriptor
				(DWORD64)FALSE,	  // CreateSuspended
				(DWORD64)0,		  // StackZeroBits
				(DWORD64)0,		  // StackReserved
				(DWORD64)NULL,	  // StackCommit
				LdrUnloadDll64,	  // StartAddress
				modBaseAddr,	  // StartParameter
				(DWORD64)NULL,	  // ThreadHandle
				(DWORD64)NULL));  // ClientID
		}
#endif

		BOOL RemoteUnmapViewOfFile(HANDLE hProcess, LPVOID lpBaseAddress)
		{
			SmartHandle hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "UnmapViewOfFile"), lpBaseAddress, 0, NULL);
			if (!hThread) return FALSE;
			WaitForSingleObject(hThread, 1000);
			DWORD dwRet = 0;
			GetExitCodeThread(hThread, &dwRet);
			return (dwRet == ERROR_SUCCESS);
		}

		BOOL CloseMapViewOfFile(HANDLE hProcess, LPCTSTR path)
		{
			BOOL isRemoteWow64 = FALSE;
			IsWow64Process(hProcess, &isRemoteWow64);

			// Windows 32bit limit: 0xFFFFFFFF.
			// Windows 64bit limit: 0x7FFFFFFFFFF.
			unsigned long long maxAddress = (is64BitOS && !isRemoteWow64) ? 0x80000000000 : 0x100000000;

			MEMORY_BASIC_INFORMATION mbi = { 0 }, mbiLast = { 0 };
			BOOL found = FALSE;
			for (unsigned long long address = 0; address < maxAddress; address += mbi.RegionSize) {
				if (!VirtualQueryEx(hProcess, (void*)address, &mbi, sizeof(mbi))) break;
				if ((unsigned long long)mbi.AllocationBase + mbi.RegionSize > maxAddress) break;

				if (mbi.Type == MEM_MAPPED) {
					if (mbiLast.AllocationBase != mbi.AllocationBase) {
						tstring filepath(MAX_PATH, '\0');
						filepath.resize((DWORD)GetMappedFileName(hProcess, mbi.BaseAddress, &filepath[0], filepath.size()));
						DevicePathToDrivePath(filepath);
						if (!filepath.empty() && Path::Contains(path, filepath.c_str())) {
							RemoteUnmapViewOfFile(hProcess, mbi.BaseAddress);
							found = TRUE;
						}
					}
					mbiLast = mbi;
				}
			}
			return found;
		}

		typedef enum FILE_TYPE {
			UNKNOWN_FILE,
			NORMAL_FILE,
			EXE_FILE,
			DLL_FILE
		} FILE_TYPE;

		FILE_TYPE CheckFileType(const tstring& path)
		{
			SmartHandle hFile = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
				return UNKNOWN_FILE;

			if ((hFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL)) == INVALID_HANDLE_VALUE)
				return UNKNOWN_FILE;

			SmartHandleTmpl<LPCVOID, UnmapViewOfFile> pvMem = MapViewOfFile(hFile, FILE_MAP_READ, 0, 0, 0);
			if (!pvMem || *(USHORT*)pvMem != IMAGE_DOS_SIGNATURE
				|| *((DWORD*)((PBYTE)pvMem + ((PIMAGE_DOS_HEADER)pvMem)->e_lfanew)) != IMAGE_NT_SIGNATURE)
				return NORMAL_FILE;

			return (((PIMAGE_FILE_HEADER)((PBYTE)pvMem + ((PIMAGE_DOS_HEADER)pvMem)->e_lfanew + sizeof(DWORD)))->Characteristics
				& IMAGE_FILE_DLL) ? DLL_FILE : EXE_FILE;
		}

		BOOL UnholdPEFile(const tstring& path)
		{
			FILE_TYPE type = CheckFileType(path);
			if (type != DLL_FILE && type != EXE_FILE) return TRUE;

			SmartHandle hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			PROCESSENTRY32 pe32 = { sizeof (pe32) };
			if (Process32First (hSnapshot, &pe32)) {
				do {
					// _tprintf_s(_T("%s [%u]\n"), pe32.szExeFile, pe32.th32ProcessID);
					SmartHandle hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
#ifndef _WIN64
					BOOL isRemoteWow64 = FALSE;
					IsWow64Process(hProcess, &isRemoteWow64);
					if (is64BitOS && !isRemoteWow64) {
						DWORD64 modBaseAddr = GetModuleHandle64(hProcess, path);
						if (modBaseAddr) {
							if (type == EXE_FILE) {
								TerminateProcess(hProcess, 1);
								break;
							}
							else {
								// 32 bit process inject 64 bit process
								RemoteFreeLibrary64(hProcess, modBaseAddr);
							}
						}
					}
					else {
#endif
						SmartHandle hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE32 | TH32CS_SNAPMODULE, pe32.th32ProcessID);
						if (hSnap == INVALID_HANDLE_VALUE)
							continue;
						MODULEENTRY32 mod = { sizeof(mod) };
						if (Module32First(hSnap, &mod)) {
							do {
								if (!_tcsicmp(mod.szExePath, path.c_str())) {
									if (type == EXE_FILE) {
										TerminateProcess(hProcess, 1);
										break;
									}
									else {
										RemoteFreeLibrary(hProcess, mod.modBaseAddr);
									}
								}
							} while (type != EXE_FILE && Module32Next(hSnap, &mod));
						}
#ifndef _WIN64
					}
#endif
				}
				while (Process32Next(hSnapshot, &pe32));
			}
			return TRUE;
		}

		BOOL UnholdFile(const tstring& path)
		{
			map<DWORD, HOLDER_INFO> holders;
			if (!FindFileHandleHolders<SYSTEM_HANDLE_INFORMATION_EX, SystemHandleInformationEx>(path.c_str(), holders))
				if (!FindFileHandleHolders<SYSTEM_HANDLE_INFORMATION, SystemHandleInformation>(path.c_str(), holders))
					return FALSE;
			for (map<DWORD, HOLDER_INFO>::const_iterator it=holders.begin(); it!=holders.end(); ++it) {
				SmartHandle hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, it->first);
				tstring holderPath(MAX_PATH, '\0');
				holderPath.resize((DWORD)GetProcessImageFileName(hProcess, &holderPath[0], holderPath.size()));
				DevicePathToDrivePath(holderPath);
				if (CloseMapViewOfFile(hProcess, path.c_str())) {
					// check memory-mapping file handle in mmfSections
					for (deque<HANDLE>::const_iterator i=it->second.mmfSections.begin(); i!=it->second.mmfSections.end(); ++i) {
						SmartHandle hDupHandle;
						if (!DuplicateHandle(hProcess, *i, GetCurrentProcess(), &hDupHandle, 0, FALSE, DUPLICATE_SAME_ACCESS))
							continue;
						// try MapViewOfFile on Handle
						LPVOID p = MapViewOfFile(hDupHandle, FILE_MAP_READ, 0, 0, 1);
						if (!p)
							continue;
						if (CloseMapViewOfFile(GetCurrentProcess(), path.c_str())) {
							// if specific file occurred, close this handle
							tstring mmfPath;
							GetHandlePath(hDupHandle, mmfPath);
							BOOL ok = CloseHandleWithProcess(it->first, *i);
							// SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ok ? FOREGROUND_GREEN : FOREGROUND_RED);
							// _tprintf_s(_T("%s [%u](0x%lX) <mmf:%s> %s\n"), ok ? _T("OK") : _T("FAIL"), it->first, (ULONG)*i, mmfPath.c_str(), holderPath.c_str());
							// SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
						}
						else
							// UnmapViewOfFile
							UnmapViewOfFile(p);
					}
				}
				for (deque<HANDLE>::const_iterator i=it->second.openHandles.begin(); i!=it->second.openHandles.end(); ++i) {
					BOOL ok = CloseHandleWithProcess(it->first, *i);
					// SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ok ? FOREGROUND_GREEN : FOREGROUND_RED);
					// _tprintf_s(_T("%s [%u](0x%lX) %s\n"), ok ? _T("OK") : _T("FAIL"), it->first, (ULONG)*i, holderPath.c_str());
					// SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
				}
			}
			UnholdPEFile(path);
			return TRUE;
		}
	}
};
