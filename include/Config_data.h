﻿#pragma once

#include <string>
#include "../lib/nlohmann/json.hpp"

using std::string;

struct ConfigData {
    struct Actor {
        struct Player {
            string name;
            int health;
            int mana;  
            int attack;
            int defense;  
        } player;
        struct Monster
        {
            string name;
            int health;
            int mana; 
            int attack;
            int defense; 
        }monster;
    } actor;
    void from_json(const nlohmann::json& j);
};

