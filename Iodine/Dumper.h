#pragma once
#include "UE.h"
#include <ostream>
#include <fstream>
#include <sstream>
int32_t GetPropertySize(UObject* Struct) {
	if (GVersion < 5.0f) {
		UStruct_Old* TStruct = reinterpret_cast<UStruct_Old*>(Struct);
		return TStruct->Size;
	}
	else if (GVersion < 12.0f) {
		UStruct_New* TStruct = reinterpret_cast<UStruct_New*>(Struct);
		return TStruct->Size;
	}
	else {
		UStruct_New2* TStruct = reinterpret_cast<UStruct_New2*>(Struct);
		return TStruct->Size;
	}
}

std::string Num2Hex(int Num) {
	std::stringstream ss;
	ss << std::hex << Num; // int decimal_value
	std::string res(ss.str());
	return res;
}

std::string GetRuntimePath()
	{
		char result[MAX_PATH];
		std::string path(result, GetModuleFileNameA(nullptr, result, MAX_PATH));
		size_t pos = path.find_last_of("\\/");
		return (std::string::npos == pos) ? "" : path.substr(0, pos);
	}

uintptr_t FreeAddr = 0;
uintptr_t FNameToStringAddr = 0;
uintptr_t ObjsAddr = 0;
uintptr_t ProcessEventAddr = 0;
bool IncludePE = true;

namespace Dumper {
	std::string GetCPPName(UObject* Target) {
		//(TODO) Make this make sense lmao
		std::string FullName = Target->GetFullName();
		if (FullName.find("Class ") != std::string::npos || FullName.find("ScriptStruct ") != std::string::npos) {
			return "U" + Target->GetName();
		}
		else {
			return "F" + Target->GetName();
		}
	}

	

	std::string GetBasic() {
		return R"(#include "./SDK.hpp"
//Made with Iodine by GD
namespace SDK {
	)";
	}

	void DumpStruct(UObject* Struct, bool ForceValues = false) {
		std::ofstream file(std::string(GetRuntimePath() + "/SDK/") + GetCPPName(Struct) + ".hpp");
		file << GetBasic();
		if (ForceValues) {
			//(TODO) Make it dump the Properties
		}
		else {
			//std::string Type = (Struct->Outer != nullptr) ? std::string("public :" + GetCPPName(Struct) + " ") : std::string(" ");
			file << "struct " << GetCPPName(Struct) << /*Type <<*/ std::string("{\n		unsigned char Data[0x") << Num2Hex(GetPropertySize(Struct)) << "];\n	}\n}";
		}
		file.close();
		printf(std::string("\nDumped Struct: " + GetCPPName(Struct)).c_str());
	}

	void Dump() {
		if (GVersion < 5.0f) {
			for (int i = 0; i < ObjObjects->Num(); i++) {
				UObject* Object = ObjObjects->GetByIndex(i);
				if (Object != nullptr) {
					if (Object->GetFullName().find("ScriptStruct ") != std::string::npos) {
						DumpStruct(Object);
					}
				}
				else {
					continue;
				}
			}
		}
		else {
			for (int i = 0; i < GObjs->Num(); i++) {
				UObject* Object = GObjs->GetItemByIndex(i);
				if (Object != nullptr) {
					if (Object->GetFullName().find("ScriptStruct ") != std::string::npos) {
						DumpStruct(Object);
					}
				}
				else {
					continue;
				}
			}
		}
	}

	void InitDumper() {
		FnFree = decltype(FnFree)(FreeAddr);
		FNameToString = decltype(FNameToString)(FNameToStringAddr);
		GObjs = reinterpret_cast<GObjects*>(ObjsAddr);
		ObjObjects = reinterpret_cast<UObjectArray*>(ObjsAddr);
		CreateDirectoryA(std::string(GetRuntimePath() + "/SDK").c_str(), 0);
	}
}