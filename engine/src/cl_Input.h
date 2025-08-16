#pragma once

#include <unordered_map>

namespace engine::cl
{
extern std::unordered_map<short, std::string> keymapping; // unicode character to CCmd name

void InputInit(void);
};