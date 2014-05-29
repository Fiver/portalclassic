#ifndef _PLAYERBOTCLASSAI_H
#define _PLAYERBOTCLASSAI_H

#include "Common.h"
#include "../World.h"
#include "../SpellMgr.h"
#include "../Player.h"
#include "../ObjectMgr.h"
#include "WorldPacket.h"
#include "../Unit.h"
#include "../SharedDefines.h"
#include "PlayerbotAI.h"

class Player;
class PlayerbotAI;

struct heal_priority
{
    Player* p;
    uint8 hp;
    uint8 type;
    heal_priority(Player* pin, uint8 hpin) : p(pin), hp(hpin), type(0) {}
    heal_priority(Player* pin, uint8 hpin, uint8 t) : p(pin), hp(hpin), type(t) {}
    // overriding the operator like this is not recommended for general use - however we won't use this struct for anything else
    bool operator<(const heal_priority& a) const { return type < a.type; }
};

class MANGOS_DLL_SPEC PlayerbotClassAI
{
public:
    PlayerbotClassAI(Player * const master, Player * const bot, PlayerbotAI * const ai);
    virtual ~PlayerbotClassAI();

    // all combat actions go here
    virtual CombatManeuverReturns DoFirstCombatManeuver(Unit*);
    virtual CombatManeuverReturns DoNextCombatManeuver(Unit*);

    // all non combat actions go here, ex buffs, heals, rezzes
    virtual void DoNonCombatActions();

    // buff a specific player, usually a real PC who is not in group
    virtual bool BuffPlayer(Player* target);

    // Utilities
    Player* GetMaster () { return m_master; }
    Player* GetPlayerBot() { return m_bot; }
    PlayerbotAI* GetAI (){ return m_ai; };

protected:
    CombatManeuverReturns CastSpellNoRanged(uint32 nextAction, Unit *pTarget);
    CombatManeuverReturns CastSpellWand(uint32 nextAction, Unit *pTarget, uint32 SHOOT);
    virtual CombatManeuverReturns HealTarget(Unit* /*target*/) { return RETURN_NO_ACTION_UNKNOWN; }
    virtual Unit* GetHealTarget();

    // These values are used in GetHealTarget and can be overridden per class (to accomodate healing spell health checks)
    uint8 m_MinHealthPercentTank;
    uint8 m_MinHealthPercentHealer;
    uint8 m_MinHealthPercentDPS;
    uint8 m_MinHealthPercentMaster;

    Player* m_master;
    Player* m_bot;
    PlayerbotAI* m_ai;
};

#endif
