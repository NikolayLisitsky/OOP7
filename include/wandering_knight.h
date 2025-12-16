#pragma once
#include "npc.h"

struct WanderingKnight : public NPC
{
    WanderingKnight(std::string& name, int x, int y);
    WanderingKnight(std::istream &is);

    void print() override;
    void save(std::ostream &os) override;

    bool accept(std::shared_ptr<NPC> attacker) override;
    bool fight(std::shared_ptr<Ork> other) override;
    bool fight(std::shared_ptr<WanderingKnight> other) override;
    bool fight(std::shared_ptr<Bear> other) override;
    
    friend std::ostream &operator<<(std::ostream &os, WanderingKnight &knight);

    static constexpr NPCParams PARAMS{30, 10}; // движение: 30, убийство: 10

    const NPCParams& params() const override { return PARAMS; }
};