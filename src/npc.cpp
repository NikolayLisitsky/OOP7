#include "../include/npc.h"
#include <algorithm>

NPC::NPC(NpcType t, std::string &name, int _x, int _y)
    : type(t), name(name), x(_x), y(_y) {}
    
NPC::NPC(NpcType t, std::istream &is) : type(t) {
    is >> name;
    is >> x;
    is >> y;
}

bool FightVisitor::visit(std::shared_ptr<Ork> ork) {
    return attacker->fight(ork);
}

bool FightVisitor::visit(std::shared_ptr<WanderingKnight> knight) {
    return attacker->fight(knight);
}

bool FightVisitor::visit(std::shared_ptr<Bear> bear) {
    return attacker->fight(bear);
}

void NPC::subscribe(std::shared_ptr<IFightObserver> observer) {
    observers.push_back(observer);
}

void NPC::fight_notify(const std::shared_ptr<NPC> defender, bool win) {
    for (auto &o : observers)
        o->on_fight(shared_from_this(), defender, win);
}

bool NPC::is_close(const std::shared_ptr<NPC> &other, size_t distance) const {
    return pow((x - other->x), 2) + pow((y - other->y), 2) <=
           distance * distance;
}

void NPC::save(std::ostream &os) {
    os << name << " " << x << " " << y << std::endl;
}

NpcType NPC::get_type() const { return type; }

std::pair<int, int> NPC::position() const {
    std::shared_lock<std::shared_mutex> lck(mtx);
    return {x, y};
}

void NPC::move(int shift_x, int shift_y, int max_x, int max_y) {
    std::lock_guard<std::shared_mutex> lck(mtx);
    x += shift_x;
    y += shift_y;
    x = std::clamp(x, 0, max_x);
    y = std::clamp(y, 0, max_y);
}

bool NPC::is_alive() const { 
    std::shared_lock<std::shared_mutex> lck(mtx);
    return alive; 
}

void NPC::must_die() {
    std::lock_guard<std::shared_mutex> lck(mtx);
    alive = false;
}

std::ostream &operator<<(std::ostream &os, NPC &npc) {
    os << "name: " << npc.name << " { x:" << npc.x << ", y:" << npc.y << " }";
    return os;
}

int NPC::roll_dice(int sides) { return (std::rand() % sides) + 1; }

int NPC::moveDistance() const { return params().moveDistance; }

int NPC::killDistance() const { return params().killDistance; }