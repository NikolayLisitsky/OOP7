#include <gtest/gtest.h>
#include "../include/npc.h"
#include "../include/ork.h"
#include "../include/wandering_knight.h"
#include "../include/bear.h"
#include "../include/editor.h"
#include "../include/fightManager.h"
#include "../include/utils.h"
#include <fstream>
#include <chrono>
#include <memory>

// ===== Тесты NPC =====

TEST(NPCTest, ConstructorAndPosition) {
    std::string name = "TestNPC";
    Ork ork{name, 10, 20};
    
    auto pos = ork.position();
    EXPECT_EQ(pos.first, 10);
    EXPECT_EQ(pos.second, 20);
    EXPECT_EQ(ork.get_type(), OrkType);
}

TEST(NPCTest, MoveWithinBounds) {
    std::string name = "TestNPC";
    WanderingKnight knight{name, 50, 50};
    
    knight.move(20, -10, 100, 100);
    auto pos = knight.position();
    EXPECT_EQ(pos.first, 70);
    EXPECT_EQ(pos.second, 40);
}

TEST(NPCTest, MoveClampToUpperBounds) {
    std::string name = "TestNPC";
    Bear bear{name, 80, 90};
    
    bear.move(30, 20, 100, 100);
    auto pos = bear.position();
    EXPECT_EQ(pos.first, 100);
    EXPECT_EQ(pos.second, 100);
}

TEST(NPCTest, MoveClampToLowerBounds) {
    std::string name = "TestNPC";
    Ork ork{name, 10, 10};
    
    ork.move(-20, -30, 100, 100);
    auto pos = ork.position();
    EXPECT_EQ(pos.first, 0);
    EXPECT_EQ(pos.second, 0);
}

TEST(NPCTest, IsCloseCalculationCorrect) {
    std::string name1 = "NPC1";
    std::string name2 = "NPC2";
    
    auto npc1 = std::make_shared<Ork>(name1, 0, 0);
    auto npc2 = std::make_shared<WanderingKnight>(name2, 3, 4); // Расстояние = 5
    
    EXPECT_TRUE(npc1->is_close(npc2, 5));  // В радиусе 5
    EXPECT_TRUE(npc1->is_close(npc2, 6));  // В радиусе 6
    EXPECT_FALSE(npc1->is_close(npc2, 4)); // Вне радиуса 4
}

TEST(NPCTest, AliveStatusChanges) {
    std::string name = "TestNPC";
    Bear bear{name, 50, 50};
    
    EXPECT_TRUE(bear.is_alive());
    bear.must_die();
    EXPECT_FALSE(bear.is_alive());
}

TEST(NPCTest, RollDiceWithinRange) {
    std::string name = "TestNPC";
    WanderingKnight knight{name, 0, 0};
    
    for (int i = 0; i < 100; ++i) {
        int roll = knight.roll_dice(6);
        EXPECT_GE(roll, 1);
        EXPECT_LE(roll, 6);
    }
}

TEST(NPCTest, NPCParametersFromTable) {
    std::string name = "TestNPC";
    
    Ork ork{name, 0, 0};
    EXPECT_EQ(ork.moveDistance(), 20);
    EXPECT_EQ(ork.killDistance(), 10);
    
    WanderingKnight knight{name, 0, 0};
    EXPECT_EQ(knight.moveDistance(), 30);
    EXPECT_EQ(knight.killDistance(), 10);
    
    Bear bear{name, 0, 0};
    EXPECT_EQ(bear.moveDistance(), 5);
    EXPECT_EQ(bear.killDistance(), 10);
}

// ===== Тесты Factory =====

TEST(FactoryTest, CreateOrk) {
    std::string name = "OrkTest";
    auto npc = factory(OrkType, name, 10, 20);
    
    ASSERT_NE(npc, nullptr);
    EXPECT_EQ(npc->get_type(), OrkType);
    
    auto pos = npc->position();
    EXPECT_EQ(pos.first, 10);
    EXPECT_EQ(pos.second, 20);
}

TEST(FactoryTest, CreateWanderingKnight) {
    std::string name = "KnightTest";
    auto npc = factory(WanderingKnightType, name, 30, 40);
    
    ASSERT_NE(npc, nullptr);
    EXPECT_EQ(npc->get_type(), WanderingKnightType);
    
    auto pos = npc->position();
    EXPECT_EQ(pos.first, 30);
    EXPECT_EQ(pos.second, 40);
}

TEST(FactoryTest, CreateBear) {
    std::string name = "BearTest";
    auto npc = factory(BearType, name, 50, 60);
    
    ASSERT_NE(npc, nullptr);
    EXPECT_EQ(npc->get_type(), BearType);
    
    auto pos = npc->position();
    EXPECT_EQ(pos.first, 50);
    EXPECT_EQ(pos.second, 60);
}

TEST(FactoryTest, SaveAndLoadNPC) {
    std::string name = "TestSaveNPC";
    auto original = std::make_shared<Ork>(name, 25, 35);
    
    // Сохраняем в строку
    std::stringstream ss;
    original->save(ss);
    
    // Загружаем из строки
    std::string type_str;
    std::string loaded_name;
    int x, y;
    
    ss >> type_str >> loaded_name >> x >> y;
    
    EXPECT_EQ(type_str, "ork");
    EXPECT_EQ(loaded_name, name);
    EXPECT_EQ(x, 25);
    EXPECT_EQ(y, 35);
}

// ===== Тесты FightManager =====

TEST(FightManagerTest, SingletonPattern) {
    auto& manager1 = FightManager::get();
    auto& manager2 = FightManager::get();
    
    // Проверяем, что это один и тот же объект
    EXPECT_EQ(&manager1, &manager2);
}

TEST(FightManagerTest, AddEventToQueue) {
    std::string name1 = "Attacker";
    std::string name2 = "Defender";
    
    auto attacker = std::make_shared<Bear>(name1, 0, 0);
    auto defender = std::make_shared<Ork>(name2, 1, 1);
    
    // Проверяем, что добавление события не вызывает исключений
    EXPECT_NO_THROW({
        FightManager::get().add_event({attacker, defender});
    });
}

// ===== Тесты Utils =====

TEST(UtilsTest, RandomDirectionLength) {
    int length = 10;
    
    // Проверяем несколько раз
    for (int i = 0; i < 50; ++i) {
        auto [dx, dy] = randomDirection(length);
        double distance = sqrt(dx*dx + dy*dy);
        
        // Допускаем небольшую погрешность из-за целочисленного округления
        EXPECT_LE(distance, length + 1.0);
    }
}

TEST(UtilsTest, GameDuration) {
    using namespace std::chrono;
    
    auto start_time = steady_clock::now();
    
    // Сразу после старта игра не должна завершаться
    EXPECT_FALSE(game_is_over(start_time));
    
    // Имитируем прошедшее время (больше 30 секунд)
    auto past_time = start_time - seconds(35);
    EXPECT_TRUE(game_is_over(past_time));
}

TEST(UtilsTest, MyPrintThreadSafety) {
    // Проверяем, что my_print можно использовать из разных контекстов
    EXPECT_NO_THROW({
        my_print() << "Test message 1" << std::endl;
        my_print() << "Test message 2" << std::endl;
    });
}

// ===== Тесты механизма боя =====

TEST(FightTest, VisitorPatternWorks) {
    std::string name1 = "Attacker";
    std::string name2 = "Defender";
    
    auto attacker = std::make_shared<Bear>(name1, 0, 0);
    auto defender = std::make_shared<Ork>(name2, 0, 0);
    
    // Создаем visitor и проверяем вызов метода
    FightVisitor visitor(attacker);
    
    // Преобразуем defender к конкретному типу для теста
    auto ork_defender = std::static_pointer_cast<Ork>(defender);
    EXPECT_NO_THROW(visitor.visit(ork_defender));
}

// ===== Тесты генерации NPC =====

TEST(GenerationTest, GenerateMultipleNPCs) {
    set_t npc_set;
    const int MAX_X = 100;
    const int MAX_Y = 100;
    const int COUNT = 10;
    
    generate_npcs(npc_set, COUNT, MAX_X, MAX_Y);
    
    EXPECT_EQ(npc_set.size(), COUNT);
    
    // Проверяем, что все NPC в пределах карты
    for (const auto& npc : npc_set) {
        auto [x, y] = npc->position();
        EXPECT_GE(x, 0);
        EXPECT_GE(y, 0);
        EXPECT_LE(x, MAX_X - 1);
        EXPECT_LE(y, MAX_Y - 1);
    }
}

// ===== Интеграционный тест =====

TEST(IntegrationTest, FullGameCycle) {
    set_t npc_set;
    const int MAX_X = 100;
    const int MAX_Y = 100;
    
    // Генерируем несколько NPC
    generate_npcs(npc_set, 5, MAX_X, MAX_Y);
    
    // Проверяем, что все NPC созданы
    EXPECT_EQ(npc_set.size(), 5);
    
    // Проверяем, что все NPC живы
    for (const auto& npc : npc_set) {
        EXPECT_TRUE(npc->is_alive());
    }
    
    // Сохраняем и загружаем
    const std::string filename = "test_save.txt";
    save(npc_set, filename);
    
    set_t loaded_set = load(filename);
    EXPECT_EQ(loaded_set.size(), npc_set.size());
    
    // Удаляем тестовый файл
    std::remove(filename.c_str());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}