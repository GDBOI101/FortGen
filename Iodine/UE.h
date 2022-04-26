#pragma once
#include "framework.h"
#include <string>
#include <locale>

float GVersion = 0.0f;
struct FString;

void(__fastcall* FNameToString)(void* NameIn, FString& Out);
void(__fastcall* FnFree)(__int64);

template<class T>
struct TArray
{
	friend class FString;

public:
	inline TArray()
	{
		Data = nullptr;
		Count = Max = 0;
	};

	inline int Num() const
	{
		return Count;
	};

	inline T& operator[](int i)
	{
		return Data[i];
	};

	inline T& operator[](int i) const
	{
		return Data[i];
	};

	inline BOOL IsValidIndex(int i) const
	{
		return i < Num();
	}

	inline void Add(T InputData)
	{
		Data = (T*)realloc(Data, sizeof(T) * (Count + 1));
		Data[Count++] = InputData;
		Max = Count;
	};

	inline void Remove(int32_t Index)
	{
		TArray<T> NewArray;
		for (int i = 0; i < this->Count; i++)
		{
			if (i == Index)
				continue;

			NewArray.Add(this->operator[](i));
		}
		this->Data = (T*)realloc(NewArray.Data, sizeof(T) * (NewArray.Count));
		this->Count = NewArray.Count;
		this->Max = NewArray.Count;
	}

	T* Data;
	int Count;
	int Max;
};


struct FString : private TArray<wchar_t>
{
	FString()
	{
	};

	FString(const wchar_t* other)
	{
		Max = Count = *other ? std::wcslen(other) + 1 : 0;

		if (Count)
		{
			Data = const_cast<wchar_t*>(other);
		}
	}

	bool IsValid() const
	{
		return Data != nullptr;
	}

	const wchar_t* c_str() const
	{
		return Data;
	}

	std::string ToString() const
	{
		auto length = std::wcslen(Data);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

		return str;
	}
};

struct FName
{
	uint32_t ComparisonIndex;
	uint32_t DisplayIndex;

	FName() = default;

	std::string ToString()
	{
		FString temp;
		FNameToString(this, temp);

		std::string ret(temp.ToString());
		FnFree((__int64)temp.c_str());

		return ret;
	}
};

template <class TEnum>
class TEnumAsByte
{
public:
	TEnumAsByte()
	{
	}

	TEnumAsByte(TEnum _value)
		: value(static_cast<uint8_t>(_value))
	{
	}

	operator TEnum() const
	{
		return static_cast<TEnum>(value);
	}

	TEnum GetValue() const
	{
		return static_cast<TEnum>(value);
	}

private:
	uint8_t value;
};

struct UObject
{
	void** VTable;
	int32_t ObjectFlags;
	int32_t InternalIndex;
	UObject* Class;
	FName Name;
	UObject* Outer;

	bool IsA(UObject* cmp) const
	{
		if (cmp == Class) {
			return false;
		}
		return false;
	}

	std::string GetName()
	{
		return Name.ToString();
	}

	std::string GetFullName()
	{
		std::string temp;

		for (auto outer = Outer; outer; outer = outer->Outer)
		{
			temp = outer->GetName() + "." + temp;
		}

		temp = reinterpret_cast<UObject*>(Class)->GetName() + " " + temp + this->GetName();

		return temp;
	}
};

struct UField_Old : UObject
{
	UField_Old* Next;
};

struct UField_New : UObject
{
	UField_New* Next;
	void* padding_01;
	void* padding_02;
};

struct UStruct_Old : UField_Old
{
	void* Super;
	UField_Old* Children;
	uint32_t Size;
	char pad[0x44];
};

struct UStruct_New : UField_New
{
	void* Super;
	UField_New* Children;
	uint32_t Size;
	char pad[0x44];
};

struct FField {
	void* vtable;
	void* Class;
	void* Owner;
	void* padding;
	FField* Next;
	FName NamePrivate;
	uint8_t FlagsPrivate;
};

enum ELifetimeCondition
{
	COND_None = 0,
	COND_InitialOnly = 1,
	COND_OwnerOnly = 2,
	COND_SkipOwner = 3,
	COND_SimulatedOnly = 4,
	COND_AutonomousOnly = 5,
	COND_SimulatedOrPhysics = 6,
	COND_InitialOrOwner = 7,
	COND_Custom = 8,
	COND_ReplayOrOwner = 9,
	COND_ReplayOnly = 10,
	COND_SimulatedOnlyNoReplay = 11,
	COND_SimulatedOrPhysicsNoReplay = 12,
	COND_SkipReplay = 13,
	COND_Never = 15,
	COND_Max = 16,
};

struct FProperty :  FField
{
	int32_t ArrayDim;
	int32_t ElementSize;
	uint8_t PropertyFlags;
	uint16_t RepIndex;
	TEnumAsByte<ELifetimeCondition> BlueprintReplicationCondition;
	int32_t Offset_Internal;
	FName RepNotifyFunc;
	FProperty* PropertyLinkNext;
	FProperty* NextRef;
	FProperty* DestructorLinkNext;
	FProperty* PostConstructLinkNext;
};

struct UStruct_New2 : UField_New
{
	//Neonite
	UStruct_New2* SuperStruct;
	UField_New* Children;
	FField* ChildProperties;
	int32_t Size;
	int32_t MinAlignment;
	TArray<uint8_t> Script;
	FProperty* PropertyLink;
	FProperty* RefLink;
	FProperty* DestructorLink;
	FProperty* PostConstructLink;
};

struct UObjectItem
{
	UObject* Object;
	DWORD Flags;
	DWORD ClusterIndex;
	DWORD SerialNumber;
};

struct PreUObjectItem
{
	UObjectItem* FUObject[10];
};

class UObjectArray {
public:
	inline int Num() const
	{
		return NumElements;
	}

	inline UObject* GetByIndex(int32_t index) const
	{
		return Objects[index].Object;
	}

	inline UObjectItem* GetItemByIndex(int32_t index) const
	{
		if (index < NumElements)
		{
			return &Objects[index];
		}
		return nullptr;
	}

private:
	UObjectItem* Objects;
	int MaxElements;
	int NumElements;
};

class NewUObjectArray {
public:
	UObjectItem* Objects[9];
};

struct GObjects
{
	NewUObjectArray* ObjectArray;
	BYTE _padding_0[0xC];
	uint32_t NumElements;

	inline void NumChunks(int* start, int* end) const
	{
		int cStart = 0, cEnd = 0;

		if (!cEnd)
		{
			while (1)
			{
				if (ObjectArray->Objects[cStart] == 0)
				{
					cStart++;
				}
				else
				{
					break;
				}
			}

			cEnd = cStart;
			while (1)
			{
				if (ObjectArray->Objects[cEnd] == 0)
				{
					break;
				}
				else
				{
					cEnd++;
				}
			}
		}

		*start = cStart;
		*end = cEnd;
	}

	inline int32_t Num() const
	{
		return NumElements;
	}

	inline UObject* GetItemByIndex(int32_t index) const
	{
		int cStart = 0, cEnd = 0;
		int chunkIndex = 0, chunkSize = 0xFFFF, chunkPos;
		UObjectItem* Object;

		NumChunks(&cStart, &cEnd);

		chunkIndex = index / chunkSize;
		if (chunkSize * chunkIndex != 0 &&
			chunkSize * chunkIndex == index)
		{
			chunkIndex--;
		}

		chunkPos = cStart + chunkIndex;
		if (chunkPos < cEnd)
		{
			Object = ObjectArray->Objects[chunkPos] + (index - chunkSize * chunkIndex);
			if (!Object) { return NULL; }

			return Object->Object;
		}

		return nullptr;
	}
};

UObjectArray* ObjObjects;
GObjects* GObjs;
