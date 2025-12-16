#include "include/npc.h"
#include "include/editor.h"
#include "include/utils.h"
#include "include/fightManager.h"

#include <thread>
#include <array>

using namespace std::chrono_literals;

int main()
{
    initialize_editor();

    set_t array;
    const int MAX_X{100};
    const int MAX_Y{100};
    
    my_print() << "Generating 50 NPCs ..." << std::endl;
    generate_npcs(array, 50, MAX_X, MAX_Y);

    my_print() << "\nStarting list:" << std::endl;
    my_print() << array;

    auto start_time = std::chrono::steady_clock::now();

    // Поток для управления боями
    std::thread fight_thread(std::ref(FightManager::get()), start_time);
    
    // Поток для перемещения NPC
    std::thread move_thread([&array, MAX_X, MAX_Y, start_time]()
    {
        while (true)
        {
            // Перемещение всех живых NPC
            for (const std::shared_ptr<NPC> & npc : array)
            {
                if(npc->is_alive()) {
                    auto [dx, dy] = randomDirection(npc->moveDistance());
                    npc->move(dx, dy, MAX_X - 1, MAX_Y - 1);
                }
            }

            // Проверка столкновений для начала боя
            for (const std::shared_ptr<NPC> & npc : array)
            {
                for (const std::shared_ptr<NPC> & other : array) 
                {
                    if ((other != npc) && 
                        (npc->is_alive()) && 
                        (other->is_alive()) && 
                        (npc->is_close(other, npc->killDistance())))
                    {
                        FightManager::get().add_event({npc, other});
                    }
                }
            }
            
            std::this_thread::sleep_for(10ms);
            if (game_is_over(start_time))
                break;
        }        
    });
    
    // Основной поток для отрисовки карты
    const int grid{20}, step_x{MAX_X / grid}, step_y{MAX_Y / grid};
    std::array<char, grid * grid> fields{0};
    
    while (true)
    {
        draw_grid<20>(array, fields, step_x, step_y);
        std::this_thread::sleep_for(1s);

        if (game_is_over(start_time))
            break;
    }
        
    move_thread.join();
    fight_thread.join();

    // Финальная отрисовка
    draw_grid<20>(array, fields, step_x, step_y);

    // Вывод выживших
    print_survivors(array);

    return 0;
}