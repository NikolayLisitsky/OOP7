#include "../include/ork.h"
#include "../include/wandering_knight.h"
#include "../include/bear.h"
#include "../include/npc.h"

Ork::Ork(std::string& name, int x, int y) : NPC(OrkType, name, x, y) {}
Ork::Ork(std::istream &is) : NPC(OrkType, is) {}

void Ork::print()
{
    my_print() << *this;
}

bool Ork::accept(std::shared_ptr<NPC> attacker) {
    FightVisitor visitor(attacker);
    return visitor.visit(std::static_pointer_cast<Ork>(shared_from_this()));
}

void Ork::save(std::ostream &os)
{
    os << "ork ";
    NPC::save(os);
}

bool Ork::fight(std::shared_ptr<Ork> other)
{
    int attack = roll_dice(6);
    int defense = other->roll_dice(6);
    bool win = (attack > defense);
    fight_notify(other, win);
    return win;
}

bool Ork::fight(std::shared_ptr<WanderingKnight> other)
{
    int attack = roll_dice(6);
    int defense = other->roll_dice(6);
    bool win = (attack > defense);
    fight_notify(other, win);
    return win;
}

bool Ork::fight(std::shared_ptr<Bear> other)
{
    int attack = roll_dice(6);
    int defense = other->roll_dice(6);
    bool win = (attack > defense);
    fight_notify(other, win);
    return win;
}

std::ostream &operator<<(std::ostream &os, Ork &ork)
{
    os << "ork: " << *static_cast<NPC *>(&ork) << std::endl;
    return os;
}