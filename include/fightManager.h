#pragma once

#include <mutex>
#include <queue>
#include "npc.h"
#include <chrono>

struct FightEvent
{
    std::shared_ptr<NPC> attacker;
    std::shared_ptr<NPC> defender;
};

class FightManager
{
private:
    std::queue<FightEvent> events;
    FightManager() {}
    std::mutex mtx;
public:
    static FightManager &get();
    void add_event(FightEvent &&event);
    void operator()(std::chrono::steady_clock::time_point start_time);
};