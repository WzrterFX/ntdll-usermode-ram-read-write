/*
    File Memory.hxx
    By WzrterFX
*/

#ifndef MEMORY_HXX
#define MEMORY_HXX

#pragma once

#define NOMINMAX
#include <windows.h>
#include <tlhelp32.h>
#include <winternl.h>

#include <stdexcept>

namespace Memory {
    using pid_t = std::uint32_t;

    namespace __NtdllApi {
        typedef struct _CLIENT_ID {
            HANDLE UniqueProcess;
            HANDLE UniqueThread;
        } CLIENT_ID, * PCLIENT_ID;

        typedef struct _OBJECT_ATTRIBUTES {
            ULONG Length;
            HANDLE RootDirectory;
            PUNICODE_STRING ObjectName;
            ULONG Attributes;
            PVOID SecurityDescriptor;
            PVOID SecurityQualityOfService;
        } OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;

        extern NTSTATUS(NTAPI* NtOpenProcess)(
            PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PCLIENT_ID
        );

        extern NTSTATUS(NTAPI* NtReadVirtualMemory)(
            HANDLE, PVOID, PVOID, SIZE_T, PSIZE_T
        );
        extern NTSTATUS(NTAPI* NtWriteVirtualMemory)(
            HANDLE, PVOID, PVOID, SIZE_T, PSIZE_T
        );
    }

    class Memory final {
        Memory(const Memory&) = delete;
        Memory& operator=(const Memory&) = delete;

    private:
        pid_t _pid;

        HANDLE _handle;

    public:
        Memory();
        ~Memory();

        void Attach(const std::wstring& name);

        template<typename T>
        [[nodiscard]] T Read(const std::uintptr_t address) const {
            if (!_handle) throw std::runtime_error(
                "::Memory read no access, ::_handle missing."
            );

            T readed { };
            __NtdllApi::NtReadVirtualMemory(
                _handle, reinterpret_cast<PVOID>(address), &readed, sizeof(T), nullptr
            );
            return readed;
        }

        template<typename T>
        void Write(const std::uintptr_t address, const T& value) const {
            if (!_handle) throw std::runtime_error(
                "::Memory write no access, ::_handle missing."
            );

            __NtdllApi::NtWriteVirtualMemory(
                _handle, reinterpret_cast<PVOID>(address), const_cast<T*>(&value), sizeof(T), nullptr
            );
        }

        [[nodiscard]] std::uintptr_t GetModule(const std::wstring& name) const;
    };

    using PMemory = Memory*;
}

#endif /* !MEMORY_HXX */
