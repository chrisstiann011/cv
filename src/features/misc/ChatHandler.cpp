#include "../../Precompiled.h"

void ChatHandler::SendChatMessage(const char* message)
{
	using fnGetChatObject = void*(__fastcall*)();
	static fnGetChatObject GetChatObject = (fnGetChatObject)Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 48 8B E8 48 85 C0 0F 84 ? ? ? ? 4C 8D 05")), 0x1, 0x0);

	using fnSendChatMessage = void(__fastcall*)(void*, unsigned int, const char*);
	static fnSendChatMessage SendChatMessage = (fnSendChatMessage)Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 49 8B 4E 20 BA ? ? ? ?")), 0x1, 0x0);

	auto pChatObject = GetChatObject();

	SendChatMessage(pChatObject, 0xFFFFFFFFLL, message);
}