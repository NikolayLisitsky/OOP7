#include "../include/editor.h"
#include "../include/ork.h"
#include "../include/wandering_knight.h"
#include "../include/bear.h"
#include "../include/npc.h"
#include "../include/utils.h"
#include <ctime>
#include <fstream>
#include <string>
#include <vector>

// Text Observer
class TextObserver : public IFightObserver {
  private:
    TextObserver() {};

  public:
    static std::shared_ptr<IFightObserver> get() {
        static TextObserver instance;
        return std::shared_ptr<IFightObserver>(&instance,
                                               [](IFightObserver *) {});
    }

    void on_fight(const std::shared_ptr<NPC> attacker,
                  const std::shared_ptr<NPC> defender, bool win) override {
        if (win) {
            my_print() << "Murder --------" << std::endl;
            my_print() << "Attacker: ";
            attacker->print();
            my_print() << "Killed: ";
            defender->print();
            my_print() << std::endl;
        }
    }
};

// File Observer
class FileObserver : public IFightObserver {
  private:
    FileObserver() {};

  public:
    static std::shared_ptr<IFightObserver> get() {
        static FileObserver instance;
        return std::shared_ptr<IFightObserver>(&instance,
                                               [](IFightObserver *) {});
    }

    void on_fight(const std::shared_ptr<NPC> attacker,
                  const std::shared_ptr<NPC> defender, bool win) override {
        if (win) {
            std::ofstream ofs("../log.txt", std::ios::app);
            ofs << "Murder --------" << std::endl;
            ofs << "Attacker: ";
            attacker->save(ofs);
            ofs << "Killed: ";
            defender->save(ofs);
            ofs << std::endl;
        }
    }
};

void initialize_editor() { std::ofstream file("../log.txt", std::ios::trunc); }

int get_type_from_string(const std::string &str) {
    if (str == "ork") {
        return OrkType;
    }
    if (str == "wandering_knight") {
        return WanderingKnightType;
    }
    if (str == "bear") {
        return BearType;
    }
    return 0;
}

// Фабрики
std::shared_ptr<NPC> factory(std::istream &is) {
    std::shared_ptr<NPC> result;
    std::string type_str;
    is >> type_str;
    int type = get_type_from_string(type_str);
    if (type) {
        switch (type) {
        case OrkType:
            result = std::make_shared<Ork>(is);
            break;
        case WanderingKnightType:
            result = std::make_shared<WanderingKnight>(is);
            break;
        case BearType:
            result = std::make_shared<Bear>(is);
            break;
        }
    } else
        std::cerr << "unexpected NPC type:" << type << std::endl;

    if (result) {
        result->subscribe(TextObserver::get());
        result->subscribe(FileObserver::get());
    }

    return result;
}

std::shared_ptr<NPC> factory(NpcType type, std::string &name, int x, int y) {
    std::shared_ptr<NPC> result;
    switch (type) {
    case OrkType:
        result = std::make_shared<Ork>(name, x, y);
        break;
    case WanderingKnightType:
        result = std::make_shared<WanderingKnight>(name, x, y);
        break;
    case BearType:
        result = std::make_shared<Bear>(name, x, y);
        break;
    default:
        break;
    }
    if (result) {
        result->subscribe(TextObserver::get());
        result->subscribe(FileObserver::get());
    }

    my_print() << "Created ";
    result->print();

    return result;
}

// Сохранение в файл
void save(const set_t &array, const std::string &filename) {
    std::ofstream fs(filename);
    fs << array.size() << std::endl;
    for (auto &n : array)
        n->save(fs);
    fs.flush();
    fs.close();
}

// Загрузка из файла
set_t load(const std::string &filename) {
    set_t result;
    std::ifstream is(filename);
    if (is.good() && is.is_open()) {
        int count;
        is >> count;
        for (int i = 0; i < count; ++i)
            result.insert(factory(is));
        is.close();
    } else
        std::cerr << "Error: " << std::strerror(errno) << std::endl;
    return result;
}

// Вывод на экран
std::ostream &operator<<(std::ostream &os, const set_t &array) {
    for (auto &n : array)
        n->print();
    return os;
}

// Логика боя
set_t fight(const set_t &array, size_t distance) {
    set_t dead_list;

    for (const auto &attacker : array) {
        for (const auto &defender : array) {
            if ((attacker != defender) &&
                (attacker->is_close(defender, distance))) {
                if (defender->accept(attacker)) {
                    dead_list.insert(defender);
                }
            }
        }
    }

    return dead_list;
}

void print_npcs(const set_t &array) {
    my_print() << "NPCs:" << std::endl;
    for (const auto &n : array)
        n->print();
}

void print_survivors(const set_t &array) {
    my_print() << "Survivors:" << std::endl;
    int number = 0;
    for (const auto &n : array) {
        if (n->is_alive()) {
            n->print();
            number++;
        }
    }
    my_print() << "Total survivors: " << number << std::endl;
}

void simulate_fight(set_t &array, size_t max_fight_distance) {
    my_print() << std::endl;
    my_print() << "Simulating fights ..." << std::endl;
    for (size_t distance = 20;
         (distance <= max_fight_distance) && !array.empty(); distance += 10) {
        my_print() << "Fighting at distance: " << distance << std::endl;
        auto dead_list = fight(array, distance);
        for (auto &d : dead_list)
            array.erase(d);
        my_print() << "Fight stats ----------" << std::endl
                  << "distance: " << distance << std::endl
                  << "killed: " << dead_list.size() << std::endl
                  << std::endl
                  << std::endl;
    }
}

std::string generate_name() {
    static int counter = 0;
    return "NPC_" + std::to_string(counter++);
}

void generate_npcs(set_t &array, size_t count, size_t max_x, size_t max_y) {
    std::srand(std::time(nullptr));
    for (size_t i = 0; i < count; ++i) {
        std::string name = generate_name();
        array.insert(factory(NpcType(std::rand() % 3 + 1), name,
                             std::rand() % max_x, std::rand() % max_y));
    }
}