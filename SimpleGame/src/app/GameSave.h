#pragma once
#include <filesystem>
#include <string>
class PrototypeWorld;

class GameSave
{
  public:
    static std::filesystem::path Path();
    static bool Exists();
    static bool Save(PrototypeWorld& world, std::string& message);
    static bool Load(PrototypeWorld& world, std::string& message);
};
