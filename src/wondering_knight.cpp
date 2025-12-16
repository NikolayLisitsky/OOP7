#include "../include/wandering_knight.h"
#include "../include/ork.h"
#include "../include/bear.h"
#include "../include/npc.h"

WanderingKnight::WanderingKnight(std::string& name, int x, int y) 
    : NPC(WanderingKnightType, name, x, y) {}
    
WanderingKnight::WanderingKnight(std::istream &is) 
    : NPC(WanderingKnightType, is) {}

void WanderingKnight::print()
{
    my_print() << *this;
}

bool WanderingKnight::accept(std::shared_ptr<NPC> attacker)
{
    FightVisitor visitor(attacker);
    return visitor.visit(std::static_pointer_cast<WanderingKnight>(shared_from_this()));
}

void WanderingKnight::save(std::ostream &os)
{
    os << "wandering_knight ";
    NPC::save(os);
}

bool WanderingKnight::fight(std::shared_ptr<Ork> other)
{
    int attack = roll_dice(6);
    int defense = other->roll_dice(6);
    bool win = (attack > defense);
    fight_notify(other, win);
    return win;
}

bool WanderingKnight::fight(std::shared_ptr<WanderingKnight> other)
{
    int attack = roll_dice(6);
    int defense = other->roll_dice(6);
    bool win = (attack > defense);
    fight_notify(other, win);
    return win;
}

bool WanderingKnight::fight(std::shared_ptr<Bear> other)
{
    int attack = roll_dice(6);
    int defense = other->roll_dice(6);
    bool win = (attack > defense);
    fight_notify(other, win);
    return win;
}

std::ostream &operator<<(std::ostream &os, WanderingKnight &knight)
{
    os << "wandering_knight: " << *static_cast<NPC *>(&knight) << std::endl;
    return os;
}