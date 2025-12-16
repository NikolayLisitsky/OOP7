#pragma once
#include "npc.h"

struct Bear : public NPC
{
    Bear(std::string& name, int x, int y);
    Bear(std::istream &is);

    void print() override;
    void save(std::ostream &os) override;

    bool accept(std::shared_ptr<NPC> attacker) override;
    bool fight(std::shared_ptr<Ork> other) override;
    bool fight(std::shared_ptr<WanderingKnight> other) override;
    bool fight(std::shared_ptr<Bear> other) override;
    
    friend std::ostream &operator<<(std::ostream &os, Bear &bear);

    static constexpr NPCParams PARAMS{5, 10}; // движение: 5, убийство: 10

    const NPCParams& params() const override { return PARAMS; }
};