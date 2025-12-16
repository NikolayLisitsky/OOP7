#include "../include/bear.h"
#include "../include/ork.h"
#include "../include/wandering_knight.h"
#include "../include/npc.h"

Bear::Bear(std::string& name, int x, int y) : NPC(BearType, name, x, y) {}
Bear::Bear(std::istream &is) : NPC(BearType, is) {}

void Bear::print()
{
    my_print() << *this;
}

bool Bear::accept(std::shared_ptr<NPC> attacker) {
    FightVisitor visitor(attacker);
    return visitor.visit(std::static_pointer_cast<Bear>(shared_from_this()));
}

void Bear::save(std::ostream &os)
{
    os << "bear ";
    NPC::save(os);
}

bool Bear::fight(std::shared_ptr<Ork> other)
{
    int attack = roll_dice(6);
    int defense = other->roll_dice(6);
    bool win = (attack > defense);
    fight_notify(other, win);
    return win;
}

bool Bear::fight(std::shared_ptr<WanderingKnight> other)
{
    int attack = roll_dice(6);
    int defense = other->roll_dice(6);
    bool win = (attack > defense);
    fight_notify(other, win);
    return win;
}

bool Bear::fight(std::shared_ptr<Bear> other)
{
    int attack = roll_dice(6);
    int defense = other->roll_dice(6);
    bool win = (attack > defense);
    fight_notify(other, win);
    return win;
}

std::ostream &operator<<(std::ostream &os, Bear &bear)
{
    os << "bear: " << *static_cast<NPC *>(&bear) << std::endl;
    return os;
}