#include <engine/cl_PlayerInput.h>

#include <engine/Console.h>

void engine::cl::PlayerInput::InitClient(void)
{
    Console::RegisterCCmd( { "+forward", [this](const std::vector<std::string>&){ this->forward = true; }, });
    Console::RegisterCCmd( { "-forward", [this](const std::vector<std::string>&){ this->forward = false; }, });
    Console::RegisterCCmd( { "+back", [this](const std::vector<std::string>&){ this->back = true; }, });
    Console::RegisterCCmd( { "-back", [this](const std::vector<std::string>&){ this->back = false; }, });
    Console::RegisterCCmd( { "+left", [this](const std::vector<std::string>&){ this->left = true; }, });
    Console::RegisterCCmd( { "-left", [this](const std::vector<std::string>&){ this->left = false; }, });
    Console::RegisterCCmd( { "+right", [this](const std::vector<std::string>&){ this->right = true; }, });
    Console::RegisterCCmd( { "-right", [this](const std::vector<std::string>&){ this->right = false; }, });

    Console::SubmitStr("bind w +forward");
    Console::SubmitStr("bind s +back");
    Console::SubmitStr("bind a +left");
    Console::SubmitStr("bind d +right");
}

void engine::cl::PlayerInput::GenerateCmd(void)
{
    cmd = {};

    cmd.type = packet::TYPE_PLAYERCMD;
    cmd.len = sizeof(packet::clsv_playercmd_t) - 4;

    if(left)
        cmd.move |= 0x8;
    if(right)
        cmd.move |= 0x4;
    if(back)
        cmd.move |= 0x2;
    if(forward)
        cmd.move |= 0x1;
}

void engine::cl::PlayerInput::Init(void)
{
    InitClient();
}