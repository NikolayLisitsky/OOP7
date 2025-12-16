#include "include/npc.h"
#include "include/editor.h"
#include "include/utils.h"
#include "include/fightManager.h"

#include <thread>
#include <array>
#include <fstream>

using namespace std::chrono_literals;

int main()
{
    initialize_editor();

    set_t array;
    const int MAX_X{100};
    const int MAX_Y{100};
    
    // Попытка загрузить NPC из файла
    my_print() << "Attempting to load NPCs from npc.txt ..." << std::endl;
    array = load("npc.txt");
    
    if (array.empty()) {
        my_print() << "Warning: npc.txt not found or empty!" << std::endl;
        my_print() << "Generating 50 random NPCs ..." << std::endl;
        generate_npcs(array, 50, MAX_X, MAX_Y);
        
        // Сохраняем сгенерированных NPC в файл для будущего использования
        save(array, "npc.txt");
        my_print() << "Generated NPCs saved to npc.txt" << std::endl;
    } else {
        my_print() << "Successfully loaded " << array.size() << " NPCs from npc.txt" << std::endl;
        
        // Проверяем, что все NPC в пределах карты
        int out_of_bounds = 0;
        for (const auto& npc : array) {
            auto [x, y] = npc->position();
            if (x < 0 || x >= MAX_X || y < 0 || y >= MAX_Y) {
                out_of_bounds++;
                my_print() << "Warning: NPC " << npc->get_type() << " at position (" 
                          << x << ", " << y << ") is out of bounds!" << std::endl;
            }
        }
        if (out_of_bounds > 0) {
            my_print() << "Found " << out_of_bounds << " NPCs out of bounds." << std::endl;
        }
    }

    my_print() << "\nStarting list of NPCs:" << std::endl;
    print_npcs(array);

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
    
    my_print() << "\nGame started! Map size: " << MAX_X << "x" << MAX_Y << std::endl;
    my_print() << "Game will run for " << GAME_DURATION_SECONDS << " seconds." << std::endl;
    my_print() << "=========================================" << std::endl;
    
    int frame = 0;
    while (true)
    {
        frame++;
        my_print() << "\nFrame " << frame << " (Time: " 
                  << seconds_since_start(start_time) << "s)" << std::endl;
        draw_grid<20>(array, fields, step_x, step_y);
        std::this_thread::sleep_for(1s);

        if (game_is_over(start_time))
            break;
    }
        
    move_thread.join();
    fight_thread.join();

    // Финальная отрисовка
    my_print() << "\n=========================================" << std::endl;
    my_print() << "Game Over! Final state after " << GAME_DURATION_SECONDS << " seconds:" << std::endl;
    draw_grid<20>(array, fields, step_x, step_y);

    // Вывод выживших
    print_survivors(array);
    
    // Дополнительная статистика
    int total_alive = 0;
    int orks_alive = 0, knights_alive = 0, bears_alive = 0;
    
    for (const auto& npc : array) {
        if (npc->is_alive()) {
            total_alive++;
            switch (npc->get_type()) {
                case OrkType: orks_alive++; break;
                case WanderingKnightType: knights_alive++; break;
                case BearType: bears_alive++; break;
                default: break;
            }
        }
    }
    
    my_print() << "\nFinal Statistics:" << std::endl;
    my_print() << "Total survivors: " << total_alive << " out of " << array.size() << std::endl;
    my_print() << "Orks alive: " << orks_alive << std::endl;
    my_print() << "Wandering Knights alive: " << knights_alive << std::endl;
    my_print() << "Bears alive: " << bears_alive << std::endl;

    return 0;
}