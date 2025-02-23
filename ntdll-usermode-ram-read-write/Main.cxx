#include <iostream>

#include "Memory/Memory.hxx"

int main(void) {
	Memory::PMemory memory { };

	try {
		memory = new Memory::Memory();

		memory->Attach(L"notepad.exe");
	}
	catch (const std::exception& exception) {
		std::cerr << exception.what() << std::endl;
	}

	return 0;
}