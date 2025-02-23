/*
    File Memory.cxx
    By WzrterFX
*/

#include "Memory.hxx"

namespace Memory {
    NTSTATUS(NTAPI* __NtdllApi::NtOpenProcess)(
        PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PCLIENT_ID
    ) = nullptr;

    NTSTATUS(NTAPI* __NtdllApi::NtReadVirtualMemory)(
        HANDLE, PVOID, PVOID, SIZE_T, PSIZE_T
    ) = nullptr;
    NTSTATUS(NTAPI* __NtdllApi::NtWriteVirtualMemory)(
        HANDLE, PVOID, PVOID, SIZE_T, PSIZE_T
    ) = nullptr;

	Memory::Memory() 
        : _pid(0), _handle(nullptr) {

        HMODULE ntdll = GetModuleHandle(L"ntdll.dll");
        if (!ntdll) throw std::runtime_error(
            "::Memory failed to load 'ntdll.dll'."
        );

        __NtdllApi::NtOpenProcess = reinterpret_cast<decltype(__NtdllApi::NtOpenProcess)>(
            GetProcAddress(ntdll, "NtOpenProcess")
        ); if (!__NtdllApi::NtOpenProcess) throw std::runtime_error(
            "::Memory failed to load 'NtOpenProcess'."
        );

        __NtdllApi::NtReadVirtualMemory = reinterpret_cast<decltype(__NtdllApi::NtReadVirtualMemory)>(
            GetProcAddress(ntdll, "NtReadVirtualMemory")
        ); if (!__NtdllApi::NtReadVirtualMemory) throw std::runtime_error(
            "::Memory failed to load 'NtReadVirtualMemory'."
        );
        __NtdllApi::NtWriteVirtualMemory = reinterpret_cast<decltype(__NtdllApi::NtWriteVirtualMemory)>(
            GetProcAddress(ntdll, "NtWriteVirtualMemory")
        ); if (!__NtdllApi::NtWriteVirtualMemory) throw std::runtime_error(
            "::Memory failed to load 'NtWriteVirtualMemory'."
        );
    };

	Memory::~Memory() {
        if (_handle) {
            CloseHandle(_handle);

            _pid = 0;
            _handle = nullptr;
        }
    }

    std::uintptr_t Memory::GetModule(const std::wstring& name) const {
        if (!_handle) throw std::runtime_error(
            "::Memory module no access, ::_handle missing."
        );

        MODULEENTRY32W moduleEntry32w { };
        moduleEntry32w.dwSize = sizeof(moduleEntry32w);

        HANDLE toolhelp32Snapshot = CreateToolhelp32Snapshot(
            TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, _pid
        ); if (!toolhelp32Snapshot) throw std::runtime_error(
            "::Memory failed to 'CreateToolhelp32Snapshot', make sure what you have access."
        );

        if (!Module32FirstW(toolhelp32Snapshot, &moduleEntry32w)) {
            CloseHandle(toolhelp32Snapshot);

            throw std::runtime_error(
                "::Memory failed, first module missing."
            );
        }

        do {
            if (_wcsicmp(moduleEntry32w.szModule, name.c_str()) == 0) {
                CloseHandle(toolhelp32Snapshot);

                return reinterpret_cast<std::uintptr_t>(moduleEntry32w.modBaseAddr);
            }
        } while (Module32NextW(toolhelp32Snapshot, &moduleEntry32w));

        CloseHandle(toolhelp32Snapshot);

        throw std::runtime_error(
            "::Memory module '" + std::string(name.begin(), name.end()) + "'."
        );
    }

    void Memory::Attach(const std::wstring& name) {
        if (_handle) Memory::~Memory();

        PROCESSENTRY32W processEntry32w { };
        processEntry32w.dwSize = sizeof(processEntry32w);

        HANDLE toolhelp32Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (!toolhelp32Snapshot) throw std::runtime_error(
            "::Memory failed to 'CreateToolhelp32Snapshot', make sure what you have access."
        );

        if (Process32FirstW(toolhelp32Snapshot, &processEntry32w)) {
            do {
                if (name == processEntry32w.szExeFile) {
                    _pid = processEntry32w.th32ProcessID;

                    break;
                }
            } while (Process32NextW(toolhelp32Snapshot, &processEntry32w));
        }

        CloseHandle(toolhelp32Snapshot);

        if (!_pid) throw std::runtime_error(
            "::Memory failed to + '" + std::string(name.begin(), name.end()) + "' + ::_pid."
        );

        __NtdllApi::OBJECT_ATTRIBUTES objectAttributes { };
        InitializeObjectAttributes(
            &objectAttributes, nullptr, 0, nullptr, nullptr
        );

        __NtdllApi::CLIENT_ID clientId{ };
        clientId.UniqueProcess = reinterpret_cast<HANDLE>(static_cast<DWORD_PTR>(_pid));
        clientId.UniqueThread = nullptr;

        __NtdllApi::NtOpenProcess(
            &_handle, PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, &objectAttributes, &clientId
        ); if (!_handle) throw std::runtime_error(
            "::Memory failed to ::_handle with '" + std::string(name.begin(), name.end()) + "'."
        );
    }
}