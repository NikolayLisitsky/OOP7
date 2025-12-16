#include "../include/fightManager.h"
#include <thread>
#include <optional>
#include "../include/utils.h"

FightManager& FightManager::get()
{
    static FightManager instance;
    return instance;
}

void FightManager::add_event(FightEvent &&event)
{
    std::lock_guard<std::mutex> lck(mtx);
    events.push(event);
}

void FightManager::operator()(std::chrono::steady_clock::time_point start_time)
{
    using namespace std::chrono_literals;
    while (true)
    {
        std::optional<FightEvent> event;

        {
            std::lock_guard<std::mutex> lck(mtx);
            if (!events.empty())
            {
                event = events.front();
                events.pop();
            }
        }

        if (event) {
            if (event->attacker->is_alive()) {
                if (event->defender->is_alive()) {
                    if (event->defender->accept(event->attacker)) {
                        int defense = event->defender->roll_dice(6);
                        int attack = event->attacker->roll_dice(6);
                        if (attack > defense) {
                            event->defender->must_die();
                        }
                    }
                }
            }
        }
        
        std::this_thread::sleep_for(100ms);
        if (game_is_over(start_time))
            break;
    }
}