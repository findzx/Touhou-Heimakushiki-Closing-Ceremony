#include "testCard.h"
#include "client.h"
#include "engine.h"
#include "general.h"
#include "room.h"



#include "maneuvering.h"
#include "standard-equips.h"

class RoukankenHakuroukenSkill : public WeaponSkill
{
public:
    RoukankenHakuroukenSkill()
        : WeaponSkill("RoukankenHakurouken")
    {
        events << TargetSpecified;
        frequency = Compulsory;
    }

    QList<SkillInvokeDetail> triggerable(TriggerEvent e, const Room *, const QVariant &data) const
    {
        CardUseStruct use = data.value<CardUseStruct>();
        if (!equipAvailable(use.from, EquipCard::WeaponLocation, objectName()))
            return QList<SkillInvokeDetail>();

        if (use.card != NULL && use.card->isKindOf("Slash")) {
            QList<SkillInvokeDetail> d;
            foreach(ServerPlayer *p, use.to) {
                if (p->isAlive()) 
                    d << SkillInvokeDetail(this, use.from, use.from, NULL, true, p);
                
            }
            return d;
        }
        return QList<SkillInvokeDetail>();
    }

    

    bool effect(TriggerEvent, Room *room, QSharedPointer<SkillInvokeDetail> invoke, QVariant &data) const
    {
        CardUseStruct use = data.value<CardUseStruct>();
        QVariantList jink_list = invoke->invoker->tag["Jink_" + use.card->toString()].toList();
        for (int i = 0; i < use.to.length(); i++) {
            if (jink_list.at(i).toInt() == 1)
                jink_list.replace(i, QVariant(2));
        }
        invoke->invoker->tag["Jink_" + use.card->toString()] = QVariant::fromValue(jink_list);
        return false;
    }
};

RoukankenHakurouken::RoukankenHakurouken(Card::Suit suit, int number)
    : Weapon(suit, number, 1)
{
    setObjectName("RoukankenHakurouken");
}


class CameraSkill : public WeaponSkill
{
public:
    CameraSkill()
        : WeaponSkill("Camera")
    {
        events << Damage;
    }

    QList<SkillInvokeDetail> triggerable(TriggerEvent, const Room *, const QVariant &data) const
    {
        DamageStruct damage = data.value<DamageStruct>();
        if (!equipAvailable(damage.from, EquipCard::WeaponLocation, objectName(), damage.to))
            return QList<SkillInvokeDetail>();

        if (damage.card && damage.card->isKindOf("Slash") && damage.from && damage.from->isAlive() && damage.to->isAlive() 
            && damage.to != damage.from && damage.by_user && !damage.chain && !damage.transfer && damage.to->getCards("h").length() > 0 )
            return QList<SkillInvokeDetail>() << SkillInvokeDetail(this, damage.from, damage.from, NULL, false, damage.to);

        return QList<SkillInvokeDetail>();
    }

    bool effect(TriggerEvent, Room *room, QSharedPointer<SkillInvokeDetail> invoke, QVariant &data) const
    {
        ServerPlayer *target = invoke->targets.first();
        if (target->getCards("h").isEmpty())
            return false;

        int id = room->askForCardChosen(invoke->invoker, target, "h", objectName());
        target->addToShownHandCards(QList<int>() << id);

        if (target->getCards("h").isEmpty())
            return false;
        int id2 = room->askForCardChosen(invoke->invoker, target, "h", objectName());
        target->addToShownHandCards(QList<int>() << id2);

        return false;
    }
};

Camera::Camera(Suit suit, int number)
    : Weapon(suit, number, 4)
{
    setObjectName("Camera");
}


class GunSkill : public WeaponSkill
{
public:
    GunSkill()
        : WeaponSkill("Gun")
    {
        events << Damage;
        frequency = Compulsory;
    }

    QList<SkillInvokeDetail> triggerable(TriggerEvent, const Room *, const QVariant &data) const
    {
        DamageStruct damage = data.value<DamageStruct>();
        if (!equipAvailable(damage.from, EquipCard::WeaponLocation, objectName(), damage.to))
            return QList<SkillInvokeDetail>();

        if (damage.card && damage.card->isKindOf("Slash") && damage.from && damage.from->isAlive() && damage.to->isAlive()
            && damage.to != damage.from)
        {
            foreach(const Card *c, damage.to->getCards("e")) {
                if (!damage.to->isBrokenEquip(c->getEffectiveId()))
                    return QList<SkillInvokeDetail>() << SkillInvokeDetail(this, damage.from, damage.from, NULL, true, damage.to);
            }
        }
        return QList<SkillInvokeDetail>();
    }

    bool effect(TriggerEvent, Room *room, QSharedPointer<SkillInvokeDetail> invoke, QVariant &data) const
    {
        ServerPlayer *target = invoke->targets.first();
        QList<int> ids;
        foreach(const Card *c, target->getCards("e")) {
            if (!target->isBrokenEquip(c->getEffectiveId()))
                ids << c->getEffectiveId();
        }
        if (!ids.isEmpty()) {
            room->doAnimate(QSanProtocol::S_ANIMATE_INDICATE, invoke->invoker->objectName(), target->objectName());
            room->touhouLogmessage("#Gun", invoke->invoker, objectName(), QList<ServerPlayer *>() << target);
            target->addBrokenEquips(ids);
        }
        
        return false;
    }
};


Gun::Gun(Suit suit, int number)
    : Weapon(suit, number, 4)
{
    setObjectName("Gun");
}


class JadeSealSkill : public ZeroCardViewAsSkill
{
public:
    JadeSealSkill()
        : ZeroCardViewAsSkill("JadeSeal")
    {
    }

    virtual bool isEnabledAtPlay(const Player *player) const
    {
        return  EquipSkill::equipAvailable(player, EquipCard::TreasureLocation, objectName()) && !player->hasFlag("JadeSeal_used");
        //return true;
    }

    virtual const Card *viewAs() const
    {
        KnownBoth *card = new KnownBoth(Card::SuitToBeDecided, -1);
        card->setSkillName(objectName());
        return card;
    }
};

class JadeSealTriggerSkill : public TreasureSkill
{
public:
    JadeSealTriggerSkill()
        : TreasureSkill("JadeSeal_trigger")
    {
        events << PreCardUsed << EventPhaseChanging;
        frequency = Compulsory;
        global = true;
    }

    void record(TriggerEvent e, Room *room, QVariant &data) const
    {
        if (e == PreCardUsed)
        {
            CardUseStruct use = data.value<CardUseStruct>();
            if (use.from && use.card && use.card->getSkillName() == "JadeSeal")
            {
                room->setPlayerFlag(use.from, "JadeSeal_used");
            }
        }
        else if (e == EventPhaseChanging)
        {
            foreach(ServerPlayer *p, room->getAllPlayers())
                room->setPlayerFlag(p, "-JadeSeal_used");
        }
    }
};

JadeSeal::JadeSeal(Suit suit, int number)
    : Treasure(suit, number)
{
    setObjectName("JadeSeal");
}

void JadeSeal::onUninstall(ServerPlayer *player) const
{
    ServerPlayer *current = player->getRoom()->getCurrent();
    if (current)
        player->getRoom()->setPlayerFlag(current, "-JadeSeal_used");
    Treasure::onUninstall(player);
}


class CamouflageSkill : public ArmorSkill
{
public:
    CamouflageSkill()
        : ArmorSkill("Camouflage")
    {
        events << DamageInflicted << CardsMoveOneTime;
        frequency = Compulsory;
    }

    QList<SkillInvokeDetail> triggerable(TriggerEvent triggerEvent, const Room *room, const QVariant &data) const
    {
        if (triggerEvent == DamageInflicted) {
            DamageStruct damage = data.value<DamageStruct>();
            if (equipAvailable(damage.to, EquipCard::ArmorLocation, objectName())) {
                int count = 0;
                foreach(ServerPlayer *p, room->getAllPlayers())
                {
                    if (p->getArmor())
                        count++;
                }
                if (count ==1)
                    return QList<SkillInvokeDetail>() << SkillInvokeDetail(this, damage.to, damage.to, NULL, true);
            }
        }
        else {
            CardsMoveOneTimeStruct move = data.value<CardsMoveOneTimeStruct>();
            if (move.to && move.to_place == Player::PlaceEquip &&
                (move.reason.m_reason & CardMoveReason::S_MASK_BASIC_REASON) == CardMoveReason::S_REASON_USE) {
                
                ServerPlayer *invoker = qobject_cast<ServerPlayer *>(move.to);
                for (int i = 0; i < move.card_ids.size(); i++) {
                    const Card *card = Sanguosha->getEngineCard(move.card_ids[i]);
                    if (card->objectName() == objectName()) {
                        foreach(ServerPlayer *p, room->getAllPlayers())
                        {
                            if (p->getArmor() && p->getArmor()->objectName() != objectName()) {
                                return QList<SkillInvokeDetail>() << SkillInvokeDetail(this, invoker, invoker, NULL, true);
                            }    
                        }
                    }
                }
            }
        }
        return QList<SkillInvokeDetail>();
    }

    bool effect(TriggerEvent triggerEvent, Room *room, QSharedPointer<SkillInvokeDetail> invoke, QVariant &data) const
    {
        if (triggerEvent == DamageInflicted) {
            DamageStruct damage = data.value<DamageStruct>();
            room->setEmotion(invoke->invoker, "armor/silver_lion");
            
            LogMessage log;
            log.type = "#Camouflage";
            log.from = invoke->invoker;
            log.arg = QString::number(damage.damage);
            log.arg2 = objectName();
            room->sendLog(log);
            return true;
        }
        else {
            QList<ServerPlayer *> targets;
            foreach(ServerPlayer *p, room->getAllPlayers())
            {
                if (p->getArmor() && p->getArmor()->objectName() != objectName()) {
                    targets << p;
                }
            }
            if (!targets.isEmpty()) {
                ServerPlayer *target = room->askForPlayerChosen(invoke->invoker, targets, "camouflage", "@camouflage");
                room->throwCard(target->getArmor()->getId(), target, invoke->invoker);
            }
        }

        return false;
    }
};

Camouflage::Camouflage(Suit suit, int number)
    : Armor(suit, number)
{
    setObjectName("Camouflage");
}


AwaitExhausted::AwaitExhausted(Card::Suit suit, int number)
    : SingleTargetTrick(suit, number)
{
    setObjectName("await_exhausted");
}

bool AwaitExhausted::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const
{
    int total_num = 2 + Sanguosha->correctCardTarget(TargetModSkill::ExtraTarget, Self, this);
    if (targets.isEmpty())
        return to_select == Self && !Self->isCardLimited(this, Card::MethodUse);
    else
        return targets.length() < total_num && !Self->isCardLimited(this, Card::MethodUse);
}

void AwaitExhausted::onEffect(const CardEffectStruct &effect) const
{
    Room *room = effect.from->getRoom();
    effect.to->drawCards(2);
    int num = qMin(2, effect.from->getCards("ehs").length());
    if (num > 0)
        room->askForDiscard(effect.to, "AwaitExhausted", num, num, false, true);
}

SpellDuel::SpellDuel(Card::Suit suit, int number)
    : SingleTargetTrick(suit, number)
{
    setObjectName("spell_duel");
}

bool SpellDuel::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const
{
    QList<int> ids;
    if (isVirtualCard())
        ids = getSubcards();
    else
        ids << getEffectiveId();
    if (ids.length() >= Self->getHandcardNum())
        return false;

    int total_num = 1 + Sanguosha->correctCardTarget(TargetModSkill::ExtraTarget, Self, this);
    return targets.length() < total_num && to_select != Self;
}

void SpellDuel::onEffect(const CardEffectStruct &effect) const
{
    if (effect.from->isKongcheng() || effect.to->isKongcheng())
        return;
    Room *room = effect.from->getRoom();
    if (effect.from->pindian(effect.to, "diaoping", NULL))
        room->damage(DamageStruct(objectName(), effect.from, effect.to));
    else
        room->damage(DamageStruct(objectName(), effect.to, effect.from));
}



Kusuri::Kusuri(Suit suit, int number)
    : BasicCard(suit, number)
{
    setObjectName("kusuri");
}

QString Kusuri::getSubtype() const
{
    return "recover_card";
}

bool Kusuri::isAvailable(const Player *player) const
{
    if (!BasicCard::isAvailable(player))
        return false;
    bool isPlay = Sanguosha->getCurrentCardUseReason() == CardUseStruct::CARD_USE_REASON_PLAY;
    bool ignore = (player->hasSkill("tianqu") && isPlay && !hasFlag("IgnoreFailed"));
    if (ignore)
        return true;
    if (player->isWounded() && !player->isProhibited(player, this))
        return true;

    QList <const Player *> targets = player->getAliveSiblings();
    targets << player;
    foreach(const Player *p, targets) {
        if (!player->isProhibited(p, this)) {
            if (p->hasFlag("Global_Dying") && !isPlay)
                return true;
            if (p->isChained() || !p->getShownHandcards().isEmpty() || !p->faceUp() || !p->getBrokenEquips().isEmpty())
                return true;
        }
    }
    return false;
}



bool Kusuri::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const
{
    
    
        bool globalDying = false;
        QList<const Player *> players = Self->getSiblings();
        players << Self;
        foreach(const Player *p, players) {
            if (p->hasFlag("Global_Dying") && p->isAlive()) {
                globalDying = true;
                break;
            }
        }

        if (globalDying && Sanguosha->getCurrentCardUseReason() == CardUseStruct::CARD_USE_REASON_RESPONSE_USE) {
            return to_select->hasFlag("Global_Dying") && to_select->objectName() == Self->property("currentdying").toString();
        }
        else {
            int total_num = 1 + Sanguosha->correctCardTarget(TargetModSkill::ExtraTarget, Self, this);
            return targets.length() < total_num &&
                (to_select->isChained() || !to_select->getShownHandcards().isEmpty() || !to_select->faceUp() || !to_select->getBrokenEquips().isEmpty());
        }
        return false;
}

void Kusuri::onEffect(const CardEffectStruct &effect) const
{
    Room *room = effect.to->getRoom();
    if (effect.to->hasFlag("Global_Dying") && Sanguosha->getCurrentCardUseReason() != CardUseStruct::CARD_USE_REASON_PLAY) {
        // recover hp
        RecoverStruct recover;
        recover.card = this;
        recover.who = effect.from;
        room->recover(effect.to, recover);
    }
    else {
        if (!effect.to->faceUp())
            effect.to->turnOver();
        if (effect.to->isChained())
            room->setPlayerProperty(effect.to, "chained", false);
        
        if (!effect.to->getBrokenEquips().isEmpty())
            effect.to->removeBrokenEquips(effect.to->getBrokenEquips(), true);

        if (!effect.to->getShownHandcards().isEmpty())
            effect.to->removeShownHandCards(effect.to->getShownHandcards(), true);
    }
}


GassingGarden::GassingGarden(Suit suit, int number)
    : DelayedTrick(suit, number)
{
    setObjectName("gassing_garden");

    judge.pattern = ".|diamond";
    judge.good = true;
    judge.reason = objectName();
}

QString GassingGarden::getSubtype() const
{
    return "unmovable_delayed_trick";
}

bool GassingGarden::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const
{
    return targets.isEmpty() && !to_select->containsTrick(objectName()) && to_select != Self;
}

void GassingGarden::takeEffect(ServerPlayer *target) const
{
    target->getRoom()->loseHp(target);
}

Donation::Donation(Suit suit, int number)
    : DelayedTrick(suit, number)
{
    setObjectName("donation");

    judge.pattern = ".|spade";
    judge.good = true;
    judge.reason = objectName();
}

QString Donation::getSubtype() const
{
    return "unmovable_delayed_trick";
}

bool Donation::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const
{
    return targets.isEmpty() && !to_select->containsTrick(objectName()) && to_select != Self;
}

void Donation::takeEffect(ServerPlayer *target) const
{
    int num = qMin(target->getCards("hes").length(), 2);
    if (num > 0)
        target->getRoom()->askForDiscard(target, objectName(), 2, 2, false, true);
}


class BladeSkill : public WeaponSkill
{
public:
    BladeSkill() : WeaponSkill("Blade")
    {
        events << SlashMissed;
    }

    QList<SkillInvokeDetail> triggerable(TriggerEvent, const Room *, const QVariant &data) const
    {
        SlashEffectStruct effect = data.value<SlashEffectStruct>();
        if (!equipAvailable(effect.from, EquipCard::WeaponLocation, objectName()))
            return QList<SkillInvokeDetail>();
        if (!effect.to->isAlive() || effect.to->getMark("Equips_of_Others_Nullified_to_You") > 0)
            return QList<SkillInvokeDetail>();
        if (!effect.from->canSlash(effect.to, NULL, false))
            return QList<SkillInvokeDetail>();
        return QList<SkillInvokeDetail>() << SkillInvokeDetail(this, effect.from, effect.from);

    }

    bool cost(TriggerEvent, Room *room, QSharedPointer<SkillInvokeDetail> invoke, QVariant &data) const
    {
        SlashEffectStruct effect = data.value<SlashEffectStruct>();

        int weapon_id = effect.from->getWeapon()->getId();
        if (weapon_id > -1)
            room->setCardFlag(weapon_id, "using");
        effect.from->setFlags("BladeUse");
        bool use = room->askForUseSlashTo(effect.from, effect.to, QString("blade-slash:%1").arg(effect.to->objectName()), false, true);
        if (!use) effect.from->setFlags("-BladeUse");
        if (weapon_id > -1)
            room->setCardFlag(weapon_id, "-using");
        return use;
    }
};

Blade::Blade(Suit suit, int number)
    : Weapon(suit, number, 3)
{
    setObjectName("Blade");
}

VSCrossbow::VSCrossbow(Suit suit, int number)
    : Crossbow(suit, number)
{
    setObjectName("vscrossbow");
}

bool VSCrossbow::match(const QString &pattern) const
{
    QStringList patterns = pattern.split("+");
    if (patterns.contains("crossbow"))
        return true;
    else
        return Crossbow::match(pattern);
}

class VSCrossbowSkill : public WeaponSkill
{
public:
    VSCrossbowSkill() : WeaponSkill("vscrossbow")
    {
        events << CardUsed << EventPhaseChanging;
        frequency = Compulsory;
    }

    void record(TriggerEvent e, Room *room, QVariant &data) const
    {
        if (e == CardUsed) {
            CardUseStruct use = data.value<CardUseStruct>();
            if (use.from && use.from->isAlive() && use.card && use.card->isKindOf("Slash")) {
                if (!use.from->hasFlag("crossbow_first"))
                    use.from->setFlags("crossbow_first");
                else if (!use.from->hasFlag("crossbow_second")) {
                    use.from->setFlags("crossbow_second");
                    room->setCardFlag(use.card, "crossbow_second");
                }
            }
            if (e == EventPhaseChanging) {
                foreach(ServerPlayer *p, room->getAllPlayers()) {
                    p->setFlags("-crossbow_first");
                    p->setFlags("-crossbow_second");
                }
            }
        }
    }

    QList<SkillInvokeDetail> triggerable(TriggerEvent e, const Room *, const QVariant &data) const
    {
        if (e == CardUsed) {
            CardUseStruct use = data.value<CardUseStruct>();
            if (use.from && use.from->isAlive() && use.card && use.card->hasFlag("crossbow_second")) {
                if (!equipAvailable(use.from, EquipCard::WeaponLocation, objectName()))
                    return QList<SkillInvokeDetail>();
                return QList<SkillInvokeDetail>() << SkillInvokeDetail(this, use.from, use.from, NULL, true);
            }
        }
        return QList<SkillInvokeDetail>();
    }

    bool cost(TriggerEvent, Room *room, QSharedPointer<SkillInvokeDetail> invoke, QVariant &data) const
    {
        room->touhouLogmessage("#InvokeSkill", invoke->invoker, objectName());
        invoke->invoker->drawCards(1);
        return false;
    }
};


class RobeSkill : public ArmorSkill
{
public:
    RobeSkill()
        : ArmorSkill("Robe")
    {
        events << DamageInflicted;
        frequency = Compulsory;
    }

    QList<SkillInvokeDetail> triggerable(TriggerEvent triggerEvent, const Room *, const QVariant &data) const
    {
        DamageStruct damage = data.value<DamageStruct>();
        if (equipAvailable(damage.to, EquipCard::ArmorLocation, objectName()) && damage.nature != DamageStruct::Normal)
            return QList<SkillInvokeDetail>() << SkillInvokeDetail(this, damage.to, damage.to, NULL, true); 
        return QList<SkillInvokeDetail>();
    }

    bool effect(TriggerEvent triggerEvent, Room *room, QSharedPointer<SkillInvokeDetail> invoke, QVariant &data) const
    {
        DamageStruct damage = data.value<DamageStruct>();
        LogMessage log;
        log.type = "#BreastPlate";
        log.from = invoke->invoker;
        if (damage.from)
            log.to << damage.from;
        log.arg = QString::number(damage.damage);
        if (damage.nature == DamageStruct::Ice)
            log.arg2 = "ice_nature";
        else if (damage.nature == DamageStruct::Fire)
            log.arg2 = "fire_nature";
        else if (damage.nature == DamageStruct::Thunder)
            log.arg2 = "thunder_nature";
        room->sendLog(log);
        return true;
    }
};

Robe::Robe(Suit suit, int number)
    : Armor(suit, number)
{
    setObjectName("Robe");
}


class RaimentSkill : public ArmorSkill
{
public:
    RaimentSkill() : ArmorSkill("Raiment")
    {
        events << CardResponded;
    }

    QList<SkillInvokeDetail> triggerable(TriggerEvent e, const Room *, const QVariant &data) const
    {
        
        CardResponseStruct resp = data.value<CardResponseStruct>();

        if (resp.m_from && resp.m_card && resp.m_card->isKindOf("Jink") && resp.m_isUse
            && equipAvailable(resp.m_from, EquipCard::ArmorLocation, objectName())
                && resp.m_who != NULL && resp.m_who->isAlive()) {
                return QList<SkillInvokeDetail>() << SkillInvokeDetail(this, resp.m_from, resp.m_from, NULL, false, resp.m_who);        
        }
        
        return  QList<SkillInvokeDetail>();
    }

    bool effect(TriggerEvent, Room *room, QSharedPointer<SkillInvokeDetail> invoke, QVariant &data) const
    {
        CardResponseStruct resp = data.value<CardResponseStruct>();
        //QString pattern = "colorless";
        //if (resp.m_card->isRed())
        //    pattern = "red";
        //if (resp.m_card->isBlack())
        //    pattern = "black";
        
        JudgeStruct judge;
        judge.reason = objectName();
        judge.who = invoke->invoker;
        judge.good = true;
        judge.pattern = ".|black";
        //judge.pattern = QString(".|.|.|%1").arg(resp.m_card->getSuitString());

        room->judge(judge);
        //invoke->invoker->gainMark("@" + judge.pattern);
        if (judge.isGood()) {
            room->damage(DamageStruct(objectName(), invoke->invoker, invoke->targets.first(), 1, DamageStruct::Thunder));
        }
        return false;
    }
};


Raiment::Raiment(Suit suit, int number)
    : Armor(suit, number)
{
    setObjectName("Raiment");
}



class ScenerySkill : public TreasureSkill
{
public:
    ScenerySkill()
        : TreasureSkill("Scenery")
    {
        events << QuitDying;
    }


    QList<SkillInvokeDetail> triggerable(TriggerEvent, const Room *room, const QVariant &data) const
    {
        DyingStruct dying = data.value<DyingStruct>();

        if (!dying.who || dying.who->isDead())
            return QList<SkillInvokeDetail>();

        foreach(ServerPlayer *p, room->getAlivePlayers()) {
            if (equipAvailable(p, EquipCard::TreasureLocation, objectName())) {
                if (p == dying.who
                    || (p != dying.who && room->getAlivePlayers().length() > 2))
                    return QList<SkillInvokeDetail>() << SkillInvokeDetail(this, p, p);
            }
        }
        return QList<SkillInvokeDetail>();
    }

    bool cost(TriggerEvent, Room *room, QSharedPointer<SkillInvokeDetail> invoke, QVariant &data) const
    {
        DyingStruct dying = data.value<DyingStruct>();
        QList<ServerPlayer *> targets = room->getOtherPlayers(invoke->invoker);
        targets.removeOne(dying.who);
        ServerPlayer *target = room->askForPlayerChosen(invoke->invoker, targets, objectName(), "@Scenery", true, true);
        if (target)
            invoke->targets << target;
        return target != NULL;
    }

    bool effect(TriggerEvent, Room *room, QSharedPointer<SkillInvokeDetail> invoke, QVariant &data) const
    {
        room->loseHp(invoke->targets.first());
        return false;
    }
};


Scenery::Scenery(Suit suit, int number)
    : Treasure(suit, number)
{
    setObjectName("Scenery");
}


class ChronicleSkill : public TreasureSkill
{
public:
    ChronicleSkill()
        : TreasureSkill("Chronicle")
    {
        events << EventPhaseStart << EventPhaseChanging;
    }


    QList<SkillInvokeDetail> triggerable(TriggerEvent triggerEvent, const Room *room, const QVariant &data) const
    {
        if (triggerEvent == EventPhaseStart) {
            ServerPlayer *player = data.value<ServerPlayer *>();
            if (player->getPhase() == Player::RoundStart && equipAvailable(player, EquipCard::TreasureLocation, objectName())) {
                return QList<SkillInvokeDetail>() << SkillInvokeDetail(this, player, player);
            }
        } else if (triggerEvent == EventPhaseChanging) {
            PhaseChangeStruct change = data.value<PhaseChangeStruct>();
            if (change.player->isAlive() &&  change.to == Player::NotActive && equipAvailable(change.player, EquipCard::TreasureLocation, objectName())) {
                return QList<SkillInvokeDetail>() << SkillInvokeDetail(this, change.player, change.player);
            }
        }
        return QList<SkillInvokeDetail>();
    }


    bool effect(TriggerEvent, Room *room, QSharedPointer<SkillInvokeDetail> invoke, QVariant &data) const
    {
        room->askForGuanxing(invoke->invoker, room->getNCards(1), Room::GuanxingBothSides, objectName());
        return false;
    }
};

Chronicle::Chronicle(Suit suit, int number)
    : Treasure(suit, number)
{
    setObjectName("Chronicle");
}

TestCardPackage::TestCardPackage()
    : Package("testCard", Package::CardPack)
{
    QList<Card *> cards;
    //Basic card
    cards << new Slash(Card::Club, 7)   //   Maku
        << new ThunderSlash(Card::Spade, 8)//   Maku(Thunder)
        << new FireSlash(Card::Heart, 9)//   Maku(Fire)
        << new IceSlash(Card::Diamond, 10) ////   Maku(Ice)
        << new Jink(Card::Diamond, 11)    //     Hei
        << new Peach(Card::Heart, 12)    // Yaku
        << new Analeptic(Card::Spade, 13) // Sake

        //Trick card
        << new Drowning(Card::Club, 1) //   Illegal Dumping
        << new FireAttack(Card::Heart, 5) // Spring Snow Incident
        << new GodSalvation(Card::Heart, 1)// Great Geyser
        << new Nullification(Card::Club, 12)//  Card Breaker
        << new AwaitExhausted(Card::Heart, 1)// Reincarnation of Inside
        << new AmazingGrace(Card::Heart, 4)// Festival Banquet
        << new KnownBoth(Card::Spade, 13)// Thought Reading
        << new IronChain(Card::Club, 13)// Secret Sealing Club
        << new Collateral(Card::Club, 13)// Seija's Ambition
        << new Duel(Card::Club, 1)// Extermination
        << new Dismantlement(Card::Club, 7)// Ayafs Interview

        << new Snatch(Card::Club, 7)// need check  //World of Hidden
        
        << new ExNihilo(Card::Club, 1) // A Special Gift
        << new ArcheryAttack(Card::Club, 1) // Catastrophe
        << new SavageAssault(Card::Club, 1)// Lunar War

        << new GassingGarden(Card::Club, 1) // need check // Gassing Garden
        
        << new Indulgence(Card::Club, 1)// Confinement
       <<new Donation(Card::Club, 1) //Donation
        << new Lightning(Card::Club, 1) // God Penalty
        <<new SupplyShortage(Card::Club, 1)// Lack of Faith

        //Equip Card
        << new GudingBlade(Card::Club, 1) // Hakkero
        << new RoukankenHakurouken(Card::Club, 1) // Roukanken & Hakurouken
        << new QinggangSword(Card::Club, 1) // Qingefs Hairpin

        << new Blade(Card::Club, 1) // need check // Sword of Hisou

        << new KylinBow(Card::Club, 1) // Ofuda
        << new Halberd(Card::Club, 1) // Lunatic Gun
        << new IceSword(Card::Club, 1) // Leica M3
        <<new VSCrossbow(Card::Club, 1)//Hourai Ningyo
        << new Axe(Card::Club, 1) // Honored Pillars
        << new Spear(Card::Club, 1) // Nuclear Control Rod

        
        
        << new SilverLion(Card::Club, 1) // Hina Doll

        << new Vine(Card::Club, 1) //need check //Optical Camouflage
        
        << new Robe(Card::Club, 1) // Robe of the Fire-rat
        << new Raiment(Card::Club, 1) // Angel's Raiment
        << new BreastPlate(Card::Club, 1) // Shanghai Ningyo

        << new RenwangShield(Card::Club, 1)//need check // Tengufs Shield
        
        << new EightDiagram(Card::Club, 1) // Yukari's Gap


        
        
        ;;
    QList<Card *> horses;

    horses << new DefensiveHorse(Card::Spade, 5)
        << new DefensiveHorse(Card::Club, 5)
        << new DefensiveHorse(Card::Heart, 13)
        << new DefensiveHorse(Card::Diamond, 13)
        << new OffensiveHorse(Card::Heart, 5)
        << new OffensiveHorse(Card::Spade, 13)
        << new OffensiveHorse(Card::Diamond, 13);
    horses.at(0)->setObjectName("OrleansNingyo");
    horses.at(1)->setObjectName("DutchNingyo");
    horses.at(2)->setObjectName("TibetanNingyo");
    horses.at(3)->setObjectName("KyotoNingyo");

    horses.at(4)->setObjectName("LondonNingyo");
    horses.at(5)->setObjectName("FrenchNingyo");
    horses.at(6)->setObjectName("RussianNingyo");
    cards << horses;


    //Grimoire of Alice
    //The Scenery of Hell
    cards << new Scenery(Card::Spade, 5) << new Chronicle(Card::Spade, 5);

    foreach(Card *card, cards)
        card->setParent(this);

    skills << new CameraSkill << new GunSkill << new JadeSealSkill << new JadeSealTriggerSkill
        <<new CamouflageSkill << new RoukankenHakuroukenSkill << new BladeSkill << new VSCrossbowSkill << new RobeSkill << new RaimentSkill
        << new ScenerySkill << new ChronicleSkill;
}

ADD_PACKAGE(TestCard)
