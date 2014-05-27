
#include "PlayerbotMageAI.h"

class PlayerbotAI;

PlayerbotMageAI::PlayerbotMageAI(Player* const master, Player* const bot, PlayerbotAI* const ai) : PlayerbotClassAI(master, bot, ai)
{
    ARCANE_MISSILES         = ai->initSpell(ARCANE_MISSILES_1);
    ARCANE_EXPLOSION        = ai->initSpell(ARCANE_EXPLOSION_1);
    COUNTERSPELL            = ai->initSpell(COUNTERSPELL_1);
    SLOW                    = ai->initSpell(SLOW_1);
    ARCANE_BARRAGE          = ai->initSpell(ARCANE_BARRAGE_1);
    ARCANE_BLAST            = ai->initSpell(ARCANE_BLAST_1);
    ARCANE_POWER            = ai->initSpell(ARCANE_POWER_1);
    DAMPEN_MAGIC            = ai->initSpell(DAMPEN_MAGIC_1);
    AMPLIFY_MAGIC           = ai->initSpell(AMPLIFY_MAGIC_1);
    MAGE_ARMOR              = ai->initSpell(MAGE_ARMOR_1);
    MIRROR_IMAGE            = ai->initSpell(MIRROR_IMAGE_1);
    ARCANE_INTELLECT        = ai->initSpell(ARCANE_INTELLECT_1);
    ARCANE_BRILLIANCE       = ai->initSpell(ARCANE_BRILLIANCE_1);
    DALARAN_INTELLECT       = ai->initSpell(DALARAN_INTELLECT_1);
    DALARAN_BRILLIANCE      = ai->initSpell(DALARAN_BRILLIANCE_1);
    MANA_SHIELD             = ai->initSpell(MANA_SHIELD_1);
    CONJURE_WATER           = ai->initSpell(CONJURE_WATER_1);
    CONJURE_FOOD            = ai->initSpell(CONJURE_FOOD_1);
    FIREBALL                = ai->initSpell(FIREBALL_1);
    FIRE_BLAST              = ai->initSpell(FIRE_BLAST_1);
    FLAMESTRIKE             = ai->initSpell(FLAMESTRIKE_1);
    SCORCH                  = ai->initSpell(SCORCH_1);
    PYROBLAST               = ai->initSpell(PYROBLAST_1);
    BLAST_WAVE              = ai->initSpell(BLAST_WAVE_1);
    COMBUSTION              = ai->initSpell(COMBUSTION_1);
    DRAGONS_BREATH          = ai->initSpell(DRAGONS_BREATH_1);
    LIVING_BOMB             = ai->initSpell(LIVING_BOMB_1);
    FROSTFIRE_BOLT          = ai->initSpell(FROSTFIRE_BOLT_1);
    FIRE_WARD               = ai->initSpell(FIRE_WARD_1);
    MOLTEN_ARMOR            = ai->initSpell(MOLTEN_ARMOR_1);
    ICY_VEINS               = ai->initSpell(ICY_VEINS_1);
    DEEP_FREEZE             = ai->initSpell(DEEP_FREEZE_1);
    FROSTBOLT               = ai->initSpell(FROSTBOLT_1);
    FROST_NOVA              = ai->initSpell(FROST_NOVA_1);
    BLIZZARD                = ai->initSpell(BLIZZARD_1);
    CONE_OF_COLD            = ai->initSpell(CONE_OF_COLD_1);
    ICE_BARRIER             = ai->initSpell(ICE_BARRIER_1);
    SUMMON_WATER_ELEMENTAL  = ai->initSpell(SUMMON_WATER_ELEMENTAL_1);
    FROST_WARD              = ai->initSpell(FROST_WARD_1);
    ICE_LANCE               = ai->initSpell(ICE_LANCE_1);
    FROST_ARMOR             = ai->initSpell(FROST_ARMOR_1);
    ICE_ARMOR               = ai->initSpell(ICE_ARMOR_1);
    ICE_BLOCK               = ai->initSpell(ICE_BLOCK_1);
    COLD_SNAP               = ai->initSpell(COLD_SNAP_1);
    
    // RANGED COMBAT
    SHOOT                   = ai->initSpell(SHOOT_2);

    RECENTLY_BANDAGED       = 11196; // first aid check

    // racial
    ARCANE_TORRENT          = ai->initSpell(ARCANE_TORRENT_MANA_CLASSES); // blood elf
    GIFT_OF_THE_NAARU       = ai->initSpell(GIFT_OF_THE_NAARU_MAGE); // draenei
    ESCAPE_ARTIST           = ai->initSpell(ESCAPE_ARTIST_ALL); // gnome
    EVERY_MAN_FOR_HIMSELF   = ai->initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human
    BERSERKING              = ai->initSpell(BERSERKING_ALL); // troll
    WILL_OF_THE_FORSAKEN    = ai->initSpell(WILL_OF_THE_FORSAKEN_ALL); // undead
}

PlayerbotMageAI::~PlayerbotMageAI() {}

bool PlayerbotMageAI::DoFirstCombatManeuver(Unit *pTarget)
{
    return false;
}

void PlayerbotMageAI::DoNextCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    switch (ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            if (FIREBALL > 0)
                ai->CastSpell(FIREBALL);
            return;
    }

    // ------- Non Duel combat ----------
    Player *m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    Unit* pVictim = pTarget->getVictim();
    float dist = m_bot->GetCombatDistance(pTarget, true);
    uint32 spec = m_bot->GetSpec();
    uint32 nextAction = 0;
    bool nextActionTarget = true;

    if (ai->GetCombatStyle() != PlayerbotAI::COMBAT_RANGED && dist > ATTACK_DISTANCE)
        ai->SetCombatStyle(PlayerbotAI::COMBAT_RANGED);
    // if in melee range OR can't shoot OR have no ranged (wand) equipped
    else if(ai->GetCombatStyle() != PlayerbotAI::COMBAT_MELEE && (dist <= ATTACK_DISTANCE || SHOOT == 0 || !m_bot->GetWeaponForAttack(RANGED_ATTACK, true, true)))
        ai->SetCombatStyle(PlayerbotAI::COMBAT_MELEE);

    //Used to determine if this bot is highest on threat
    Unit *newTarget = ai->FindAttacker((PlayerbotAI::ATTACKERINFOTYPE) (PlayerbotAI::AIT_VICTIMSELF | PlayerbotAI::AIT_HIGHESTTHREAT), m_bot);
    if (newTarget) // TODO: && party has a tank
    {
        // Insert instant threat reducing spell (if a mage has one)

        // Have threat, can't quickly lower it. 3 options remain: Stop attacking, lowlevel damage (wand), keep on keeping on.
        if (newTarget->GetHealthPercent() > 25)
        {
            // If elite, do nothing and pray tank gets aggro off you
            // TODO: Is there an IsElite function? If so, find it and insert.
            //if (newTarget->IsElite())
            //    return;

            // Not an elite. You could insert FEAR here but in any PvE situation that's 90-95% likely
            // to worsen the situation for the group. ... So please don't.
            CastSpell(SHOOT, pTarget);
            return;
        }
    }

    switch (spec)
    {
        case MAGE_SPEC_FROST:
            if (ICY_VEINS > 0 && !m_bot->HasAura(ICY_VEINS, EFFECT_INDEX_0) && LastSpellFrost < 1 && ai->GetManaPercent() >= 3)
            {
                LastSpellFrost = LastSpellFrost + 1;
                return CastSpell(ICY_VEINS, m_bot);
            }
            if (FROSTBOLT > 0 && LastSpellFrost < 2 && !pTarget->HasAura(FROSTBOLT, EFFECT_INDEX_0) && ai->GetManaPercent() >= 16)
            {
                LastSpellFrost = LastSpellFrost + 1;
                return CastSpell(FROSTBOLT, pTarget);
            }
            if (FROST_WARD > 0 && LastSpellFrost < 3 && !m_bot->HasAura(FROST_WARD, EFFECT_INDEX_0) && ai->GetManaPercent() >= 19)
            {
                LastSpellFrost = LastSpellFrost + 1;
                return CastSpell(FROST_WARD, m_bot);
            }
            if (FROST_NOVA > 0 && LastSpellFrost < 4 && dist <= ATTACK_DISTANCE && !pTarget->HasAura(FROST_NOVA, EFFECT_INDEX_0) && ai->GetManaPercent() >= 10)
            {
                LastSpellFrost = LastSpellFrost + 1;
                return CastSpell(FROST_NOVA, pTarget);
            }
            if (ICE_LANCE > 0 && LastSpellFrost < 5 && ai->GetManaPercent() >= 7)
            {
                LastSpellFrost = LastSpellFrost + 1;
                return CastSpell(ICE_LANCE, pTarget);
            }
            if (BLIZZARD > 0 && LastSpellFrost < 6 && ai->GetAttackerCount() >= 5 && ai->GetManaPercent() >= 89)
            {
                LastSpellFrost = LastSpellFrost + 1;
                CastSpell(BLIZZARD, pTarget);
                ai->SetIgnoreUpdateTime(8);
                return;
            }
            if (CONE_OF_COLD > 0 && LastSpellFrost < 7 && dist <= ATTACK_DISTANCE && !pTarget->HasAura(CONE_OF_COLD, EFFECT_INDEX_0) && ai->GetManaPercent() >= 35)
            {
                LastSpellFrost = LastSpellFrost + 1;
                return CastSpell(CONE_OF_COLD, pTarget);
            }
            if (DEEP_FREEZE > 0 && LastSpellFrost < 8 && pTarget->HasAura(AURA_STATE_FROZEN, EFFECT_INDEX_0) && !pTarget->HasAura(DEEP_FREEZE, EFFECT_INDEX_0) && ai->GetManaPercent() >= 9)
            {
                LastSpellFrost = LastSpellFrost + 1;
                return CastSpell(DEEP_FREEZE, pTarget);
            }
            if (ICE_BARRIER > 0 && LastSpellFrost < 9 && pVictim == m_bot && !m_bot->HasAura(ICE_BARRIER, EFFECT_INDEX_0) && ai->GetHealthPercent() < 50 && ai->GetManaPercent() >= 30)
            {
                LastSpellFrost = LastSpellFrost + 1;
                return CastSpell(ICE_BARRIER, m_bot);
            }
            if (SUMMON_WATER_ELEMENTAL > 0 && LastSpellFrost < 10 && ai->GetManaPercent() >= 16)
            {
                LastSpellFrost = LastSpellFrost + 1;
                return CastSpell(SUMMON_WATER_ELEMENTAL);
            }
            if (ICE_BLOCK > 0 && LastSpellFrost < 11 && pVictim == m_bot && !m_bot->HasAura(ICE_BLOCK, EFFECT_INDEX_0) && ai->GetHealthPercent() < 30)
            {
                LastSpellFrost = LastSpellFrost + 1;
                return CastSpell(ICE_BLOCK, m_bot);
            }
            if (COLD_SNAP > 0 && LastSpellFrost < 12)
            {
                LastSpellFrost = LastSpellFrost + 1;
                return CastSpell(COLD_SNAP, m_bot);
            }

            LastSpellFrost = 0;

            if (FROSTBOLT > 0 && ai->GetManaPercent() >= 16)
                return CastSpell(FROSTBOLT, pTarget);
            break;

        case MAGE_SPEC_FIRE:
            if (FIRE_WARD > 0 && !m_bot->HasAura(FIRE_WARD, EFFECT_INDEX_0) && LastSpellFire < 1 && ai->GetManaPercent() >= 3)
            {
                LastSpellFire = LastSpellFire + 1;
                return CastSpell(FIRE_WARD, m_bot);
            }
            if (COMBUSTION > 0 && !m_bot->HasAura(COMBUSTION, EFFECT_INDEX_0) && LastSpellFire < 2)
            {
                LastSpellFire = LastSpellFire + 1;
                return CastSpell(COMBUSTION, m_bot);
            }
            if (FIREBALL > 0 && LastSpellFire < 3 && ai->GetManaPercent() >= 23)
            {
                LastSpellFire = LastSpellFire + 1;
                return CastSpell(FIREBALL, pTarget);
            }
            if (FIRE_BLAST > 0 && LastSpellFire < 4 && ai->GetManaPercent() >= 25)
            {
                LastSpellFire = LastSpellFire + 1;
                return CastSpell(FIRE_BLAST, pTarget);
            }
            if (FLAMESTRIKE > 0 && LastSpellFire < 5 && ai->GetManaPercent() >= 35)
            {
                LastSpellFire = LastSpellFire + 1;
                return CastSpell(FLAMESTRIKE, pTarget);
            }
            if (SCORCH > 0 && LastSpellFire < 6 && ai->GetManaPercent() >= 10)
            {
                LastSpellFire = LastSpellFire + 1;
                return CastSpell(SCORCH, pTarget);
            }
            if (PYROBLAST > 0 && LastSpellFire < 7 && !pTarget->HasAura(PYROBLAST, EFFECT_INDEX_0) && ai->GetManaPercent() >= 27)
            {
                LastSpellFire = LastSpellFire + 1;
                return CastSpell(PYROBLAST, pTarget);
            }
            if (BLAST_WAVE > 0 && LastSpellFire < 8 && ai->GetAttackerCount() >= 3 && dist <= ATTACK_DISTANCE && ai->GetManaPercent() >= 34)
            {
                LastSpellFire = LastSpellFire + 1;
                return CastSpell(BLAST_WAVE, pTarget);
            }
            if (DRAGONS_BREATH > 0 && LastSpellFire < 9 && dist <= ATTACK_DISTANCE && ai->GetManaPercent() >= 37)
            {
                LastSpellFire = LastSpellFire + 1;
                return CastSpell(DRAGONS_BREATH, pTarget);
            }
            if (LIVING_BOMB > 0 && LastSpellFire < 10 && !pTarget->HasAura(LIVING_BOMB, EFFECT_INDEX_0) && ai->GetManaPercent() >= 27)
            {
                LastSpellFire = LastSpellFire + 1;
                return CastSpell(LIVING_BOMB, pTarget);
            }
            if (FROSTFIRE_BOLT > 0 && LastSpellFire < 11 && !pTarget->HasAura(FROSTFIRE_BOLT, EFFECT_INDEX_0) && ai->GetManaPercent() >= 14)
            {
                LastSpellFire = LastSpellFire + 1;
                return CastSpell(FROSTFIRE_BOLT, pTarget);
            }

            LastSpellFire = 0;

            if (FIREBALL > 0 && ai->GetManaPercent() >= 23)
                return CastSpell(FIREBALL, pTarget);
            break;

        case MAGE_SPEC_ARCANE:
            if (ARCANE_POWER > 0 && LastSpellArcane < 1 && ai->GetManaPercent() >= 37)
            {
                LastSpellArcane = LastSpellArcane + 1;
                return CastSpell(ARCANE_POWER, pTarget);
            }
            if (ARCANE_MISSILES > 0 && LastSpellArcane < 2 && ai->GetManaPercent() >= 37)
            {
                LastSpellArcane = LastSpellArcane + 1;
                CastSpell(ARCANE_MISSILES, pTarget);
                ai->SetIgnoreUpdateTime(3);
                return;
            }
            if (ARCANE_EXPLOSION > 0 && LastSpellArcane < 3 && ai->GetAttackerCount() >= 3 && dist <= ATTACK_DISTANCE && ai->GetManaPercent() >= 27)
            {
                LastSpellArcane = LastSpellArcane + 1;
                return CastSpell(ARCANE_EXPLOSION, pTarget);
            }
            if (COUNTERSPELL > 0 && pTarget->IsNonMeleeSpellCasted(true) && LastSpellArcane < 4 && ai->GetManaPercent() >= 9)
            {
                LastSpellArcane = LastSpellArcane + 1;
                return CastSpell(COUNTERSPELL, pTarget);
            }
            if (SLOW > 0 && LastSpellArcane < 5 && !pTarget->HasAura(SLOW, EFFECT_INDEX_0) && ai->GetManaPercent() >= 12)
            {
                LastSpellArcane = LastSpellArcane + 1;
                return CastSpell(SLOW, pTarget);
            }
            if (ARCANE_BARRAGE > 0 && LastSpellArcane < 6 && ai->GetManaPercent() >= 27)
            {
                LastSpellArcane = LastSpellArcane + 1;
                return CastSpell(ARCANE_BARRAGE, pTarget);
            }
            if (ARCANE_BLAST > 0 && LastSpellArcane < 7 && ai->GetManaPercent() >= 8)
            {
                LastSpellArcane = LastSpellArcane + 1;
                return CastSpell(ARCANE_BLAST, pTarget);
            }
            if (MIRROR_IMAGE > 0 && LastSpellArcane < 8 && ai->GetManaPercent() >= 10)
            {
                LastSpellArcane = LastSpellArcane + 1;
                return CastSpell(MIRROR_IMAGE);
            }
            if (MANA_SHIELD > 0 && LastSpellArcane < 9 && ai->GetHealthPercent() < 70 && pVictim == m_bot && !m_bot->HasAura(MANA_SHIELD, EFFECT_INDEX_0) && ai->GetManaPercent() >= 8)
            {
                LastSpellArcane = LastSpellArcane + 1;
                return CastSpell(MANA_SHIELD, m_bot);
            }

            LastSpellArcane = 0;

            if (FIREBALL > 0 && ai->GetManaPercent() >= 23)
                return CastSpell(FIREBALL, pTarget);
            break;
    }

    ai->TellMaster("Couldn't find an appropriate spell.");
} // end DoNextCombatManeuver

void PlayerbotMageAI::CastSpell(uint32 nextAction, Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    Player *m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    if (SHOOT > 0 && m_bot->FindCurrentSpellBySpellId(SHOOT) && m_bot->GetWeaponForAttack(RANGED_ATTACK, true, true))
    {
        if (nextAction == SHOOT)
            // At this point we're already shooting and are asked to shoot. Don't cause a global cooldown by stopping to shoot! Leave it be.
            return;

        // We are shooting but wish to cast a spell. Stop 'casting' shoot.
        m_bot->InterruptNonMeleeSpells(true, SHOOT);
        // ai->TellMaster("Interrupting auto shot.");
    }

    // We've stopped ranged (if applicable), if no nextAction just return
    if (nextAction == 0)
        return;

    if (nextAction == SHOOT)
    {
        if (SHOOT > 0 && ai->GetCombatStyle() == PlayerbotAI::COMBAT_RANGED && !m_bot->FindCurrentSpellBySpellId(SHOOT) && m_bot->GetWeaponForAttack(RANGED_ATTACK, true, true))
            ai->CastSpell(SHOOT, *pTarget);
        else
            // Do Melee attack
            return;
        // ai->TellMaster("Starting auto shot.");
    }

    if (pTarget != NULL)
        ai->CastSpell(nextAction, *pTarget);
    else
        ai->CastSpell(nextAction);
}

void PlayerbotMageAI::DoNonCombatActions()
{
    Player * m_bot = GetPlayerBot();
    Player * master = GetMaster();

    if (!m_bot || !master)
        return;

    SpellSequence = SPELL_FROST;
    PlayerbotAI* ai = GetAI();

    // Buff armor
    if (MOLTEN_ARMOR)
    {
        if (ai->SelfBuff(MOLTEN_ARMOR))
            return;
    }
    else if (MAGE_ARMOR)
    {
        if (ai->SelfBuff(MAGE_ARMOR))
            return;
    }
    else if (ICE_ARMOR)
    {
        if (ai->SelfBuff(ICE_ARMOR))
            return;
    }
    else if (FROST_ARMOR)
        if (ai->SelfBuff(FROST_ARMOR))
            return;

    // buff master's group
    if (master->GetGroup())
    {
        // Buff master with group buff...
        if (!master->IsInDuel(master))
            if (ARCANE_BRILLIANCE && ai->HasSpellReagents(ARCANE_BRILLIANCE))
                if (ai->Buff(ARCANE_BRILLIANCE, master))
                    return;

        // ...and check group for new members joined or resurrected, or just buff everyone if no group buff available
        Group::MemberSlotList const& groupSlot = GetMaster()->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *tPlayer = sObjectMgr.GetPlayer(itr->guid);
            if (!tPlayer || !tPlayer->isAlive() || tPlayer == m_bot)
                continue;

            if (tPlayer->IsInDuelWith(master))
                continue;

            // buff
            if (BuffPlayer(tPlayer))
                return;
        }

    }
    // There is no group, buff master
    else if (master->isAlive() && !master->IsInDuel(master))
        if (BuffPlayer(master))
            return;

    // Buff self finally
    if (BuffPlayer(m_bot))
        return;

    // conjure food & water
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = ai->FindDrink();
    Item* fItem = ai->FindBandage();

    if (pItem == NULL && CONJURE_WATER && ai->GetBaseManaPercent() >= 48)
    {
        ai->TellMaster("I'm conjuring some water.");
        ai->CastSpell(CONJURE_WATER, *m_bot);
        ai->SetIgnoreUpdateTime(3);
        return;
    }
    else if (pItem != NULL && ai->GetManaPercent() < 30)
    {
        ai->TellMaster("I could use a drink.");
        ai->UseItem(pItem);
        return;
    }

    pItem = ai->FindFood();

    if (pItem == NULL && CONJURE_FOOD && ai->GetBaseManaPercent() >= 48)
    {
        ai->TellMaster("I'm conjuring some food.");
        ai->CastSpell(CONJURE_FOOD, *m_bot);
        ai->SetIgnoreUpdateTime(3);
    }

    // hp check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    pItem = ai->FindFood();

    if (pItem != NULL && ai->GetHealthPercent() < 30)
    {
        ai->TellMaster("I could use some food.");
        ai->UseItem(pItem);
        return;
    }
    else if (pItem == NULL && fItem != NULL && !m_bot->HasAura(RECENTLY_BANDAGED, EFFECT_INDEX_0) && ai->GetHealthPercent() < 70)
    {
        ai->TellMaster("I could use first aid.");
        ai->UseItem(fItem);
        return;
    }

} // end DoNonCombatActions

bool PlayerbotMageAI::BuffPlayer(Player* target)
{
    PlayerbotAI * ai = GetAI();
    Pet * pet = target->GetPet();

    if ((pet && !pet->HasAuraType(SPELL_AURA_MOD_UNATTACKABLE)) && pet->getPowerType() == POWER_MANA && ai->Buff(ARCANE_INTELLECT, pet))
        return true;

    if (ARCANE_INTELLECT)
        return ai->Buff(ARCANE_INTELLECT, target);
    else
        return false;
}
