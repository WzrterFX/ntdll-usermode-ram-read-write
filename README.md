# ntdll-usermode-ram-read-write

## Overview
User-mode memory library to allow direct reading and writing of process memory.  
By using NtDll API, this avoids detection by anti-cheats that monitor standard Windows API calls like `ReadProcessMemory`, `WriteProcessMemory`, and `OpenProcess`.

## Disclaimer
**ntdll-usermode-ram-read-write** is intended for educational purposes only.  
Use responsibly and ensure compliance with all relevant laws and regulations regarding software manipulation and anti-cheat measures.  
Misuse of this software may result in account bans or legal consequences. 

## Knowledge

### Direct System Calls
**ntdll.dll** is a core Windows DLL that contains low-level system call functions used by the Windows operating system.  
By calling functions directly from `ntdll.dll`, such as undocumented functions `NtReadVirtualMemory` and `NtWriteVirtualMemory`, we can bypass traditional user-mode hooks placed by security tools, which typically monitor high-level functions like `ReadProcessMemory` and `WriteProcessMemory`.

### Process Memory Access
Accessing another process's memory requires opening a handle with the correct permissions.  
This is achieved by using `NtOpenProcess`, which provides access to a target process with bypassed user-mode hooks that anti-cheat systems often place on standard memory management functions.

### Resolving Function Addresses from NTDLL
Instead of statically linking to `ntdll.dll`, function addresses are retrieved dynamically at runtime. The library uses `GetModuleHandle` to load the module and `GetProcAddress` to retrieve the actual function pointers.  
This approach avoids static analysis tools that detect imports directly from `ntdll.dll`. If any of the functions fail to load, the program terminates execution to prevent undefined behavior.
