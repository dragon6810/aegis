#pragma once

#include <unordered_map>

namespace engine::cl
{
extern std::unordered_map<std::string, std::string> keymapping; // name to CCmd name

void InputInit(void);
};