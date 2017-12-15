#include "maneuvering.h"
#include "client.h"
#include "engine.h"
#include "general.h"
#include "room.h"

NatureSlash::NatureSlash(Suit suit, int number, DamageStruct::Nature nature)
    : Slash(suit, number)
{
    this->nature = nature;
}

bool NatureSlash::match(const QString &pattern) const
{
    QStringList patterns = pattern.split("+");
    if (patterns.contains("slash"))
        return true;
    else
        return Slash::match(pattern);
}

ThunderSlash::ThunderSlash(Suit suit, int number)
    : NatureSlash(suit, number, DamageStruct::Thunder)
{
    setObjectName("thunder_slash");
}

FireSlash::FireSlash(Suit suit, int number)
    : NatureSlash(suit, number, DamageStruct::Fire)
{
    setObjectName("fire_slash");
    nature = DamageStruct::Fire;
}

IceSlash::IceSlash(Suit suit, int number)
    : NatureSlash(suit, number, DamageStruct::Ice)
{
    setObjectName("ice_slash");
    nature = DamageStruct::Ice;
}

Analeptic::Analeptic(Card::Suit suit, int number)
    : BasicCard(suit, number)
{
    setObjectName("analeptic");
    target_fixed = true;
}

QString Analeptic::getSubtype() const
{
    return "buff_card";
}

bool Analeptic::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const
{
    if (targets.isEmpty()) {
        if (to_select == Self)
            return true;
        if (Self->hasSkill("tianqu") && Sanguosha->getCurrentCardUseReason() == CardUseStruct::CARD_USE_REASON_PLAY && to_select != Self && !hasFlag("IgnoreFailed"))
            return true;
    }
    return false;
}

bool Analeptic::IsAvailable(const Player *player, const Card *analeptic)
{
    Analeptic *newanaleptic = new Analeptic(Card::NoSuit, 0);
    newanaleptic->deleteLater();
#define THIS_ANALEPTIC (analeptic == NULL ? newanaleptic : analeptic)
    if (player->isCardLimited(THIS_ANALEPTIC, Card::MethodUse) || player->isProhibited(player, THIS_ANALEPTIC))
        return false;

    return player->usedTimes("Analeptic") <= Sanguosha->correctCardTarget(TargetModSkill::Residue, player, THIS_ANALEPTIC);
#undef THIS_ANALEPTIC
}

bool Analeptic::isAvailable(const Player *player) const
{
    return IsAvailable(player, this) && BasicCard::isAvailable(player);
}

void Analeptic::onUse(Room *room, const CardUseStruct &card_use) const
{
    CardUseStruct use = card_use;
    if (use.to.isEmpty())
        use.to << use.from;
    BasicCard::onUse(room, use);
}

void Analeptic::onEffect(const CardEffectStruct &effect) const
{
    Room *room = effect.to->getRoom();
    room->setEmotion(effect.to, "analeptic");

    if (effect.to->hasFlag("Global_Dying") && Sanguosha->getCurrentCardUseReason() != CardUseStruct::CARD_USE_REASON_PLAY) {
        // recover hp
        RecoverStruct recover;
        recover.card = this;
        recover.who = effect.from;
        room->recover(effect.to, recover);
    } else {
        room->addPlayerMark(effect.to, "drank");
    }
}

class FanSkill : public WeaponSkill
{
public:
    FanSkill()
        : WeaponSkill("Fan")
    {
        events << TargetSpecifying;
    }

    QList<SkillInvokeDetail> triggerable(TriggerEvent, const Room *, const QVariant &data) const
    {
        CardUseStruct use = data.value<CardUseStruct>();
        if (!equipAvailable(use.from, EquipCard::WeaponLocation, objectName()))
            return QList<SkillInvokeDetail>();
        if (use.card->isKindOf("Slash") && use.from->isAlive()) {
            if (use.card->isKindOf("FireSlash") || !use.card->isKindOf("NatureSlash"))
                return QList<SkillInvokeDetail>() << SkillInvokeDetail(this, use.from, use.from);
        }
        return QList<SkillInvokeDetail>();
    }

    bool effect(TriggerEvent, Room *room, QSharedPointer<SkillInvokeDetail> invoke, QVariant &data) const
    {
        CardUseStruct use = data.value<CardUseStruct>();
        if (use.card->isKindOf("FireSlash"))
            invoke->invoker->drawCards(1);
        else if (!use.card->isKindOf("NatureSlash")) {
            FireSlash *fire_slash = new FireSlash(use.card->getSuit(), use.card->getNumber());
            if (use.card->getSubcards().length() > 0)
                fire_slash->addSubcards(use.card->getSubcards());
            else {
                int id = use.card->getEffectiveId();
                if (id > -1)
                    fire_slash->addSubcard(id);
            }

            // When TargetSpecified, no need to check canSlash()
            //remain the information of origianl card
            fire_slash->setSkillName(use.card->getSkillName());
            QStringList flags = use.card->getFlags();
            foreach (const QString &flag, flags)
                fire_slash->setFlags(flag);
            invoke->invoker->tag["Jink_" + fire_slash->toString()] = invoke->invoker->tag["Jink_" + use.card->toString()];

            use.card = fire_slash;
            data = QVariant::fromValue(use);
        }
        room->setEmotion(invoke->invoker, "weapon/fan");

        return false;
    }
};

Fan::Fan(Suit suit, int number)
    : Weapon(suit, number, 4)
{
    setObjectName("Fan");
}

class GudingBladeSkill : public WeaponSkill
{
public:
    GudingBladeSkill()
        : WeaponSkill("GudingBlade")
    {
        events << DamageCaused;
        frequency = Compulsory;
    }

    QList<SkillInvokeDetail> triggerable(TriggerEvent, const Room *, const QVariant &data) const
    {
        DamageStruct damage = data.value<DamageStruct>();
        if (!equipAvailable(damage.from, EquipCard::WeaponLocation, objectName(), damage.to))
            return QList<SkillInvokeDetail>();

        if (damage.card && damage.card->isKindOf("Slash") && damage.to->isKongcheng() && damage.by_user && !damage.chain && !damage.transfer)
            return QList<SkillInvokeDetail>() << SkillInvokeDetail(this, damage.from, damage.from, NULL, true, damage.to);

        return QList<SkillInvokeDetail>();
    }

    bool effect(TriggerEvent, Room *room, QSharedPointer<SkillInvokeDetail> invoke, QVariant &data) const
    {
        DamageStruct damage = data.value<DamageStruct>();
        room->setEmotion(invoke->invoker, "weapon/guding_blade");

        LogMessage log;
        log.type = "#GudingBladeEffect";
        log.from = invoke->invoker;
        log.to << damage.to;
        log.arg = QString::number(damage.damage);
        log.arg2 = QString::number(++damage.damage);
        room->sendLog(log);

        data = QVariant::fromValue(damage);
        return false;
    }
};

GudingBlade::GudingBlade(Suit suit, int number)
    : Weapon(suit, number, 2)
{
    setObjectName("GudingBlade");
}

class IronArmorSkill : public ArmorSkill
{
public:
    IronArmorSkill()
        : ArmorSkill("IronArmor")
    {
        events << SlashEffected << CardEffected;
        frequency = Compulsory;
    }

    QList<SkillInvokeDetail> triggerable(TriggerEvent triggerEvent, const Room *, const QVariant &data) const
    {
        if (triggerEvent == SlashEffected) {
            SlashEffectStruct effect = data.value<SlashEffectStruct>();
            if (!equipAvailable(effect.to, EquipCard::ArmorLocation, objectName()))
                return QList<SkillInvokeDetail>();
            if (effect.slash->isKindOf("NatureSlash"))
                return QList<SkillInvokeDetail>() << SkillInvokeDetail(this, effect.to, effect.to, NULL, true);
        } else if (triggerEvent == CardEffected) {
            CardEffectStruct effect = data.value<CardEffectStruct>();
            if (equipAvailable(effect.to, EquipCard::ArmorLocation, objectName()) && effect.card != NULL
                && (effect.card->isKindOf("IronChain") || effect.card->isKindOf("FireAttack")))
                return QList<SkillInvokeDetail>() << SkillInvokeDetail(this, effect.to, effect.to, NULL, true);
        }

        return QList<SkillInvokeDetail>();
    }

    bool effect(TriggerEvent triggerEvent, Room *room, QSharedPointer<SkillInvokeDetail> invoke, QVariant &data) const
    {
        LogMessage log;
        log.type = "#ArmorNullify";
        log.from = invoke->invoker;
        log.arg = objectName();
        if (triggerEvent == SlashEffected) {
            SlashEffectStruct effect = data.value<SlashEffectStruct>();
            log.arg2 = effect.slash->objectName();
        } else if (triggerEvent == CardEffected) {
            CardEffectStruct effect = data.value<CardEffectStruct>();
            log.arg2 = effect.card->objectName();
        }
        room->sendLog(log);
        //armor emotion is not ready
        room->setEmotion(invoke->invoker, "skill_nullify");
        invoke->invoker->setFlags("Global_NonSkillNullify");
        return true;
    }
};

IronArmor::IronArmor(Suit suit, int number)
    : Armor(suit, number)
{
    setObjectName("IronArmor");
}

class VineSkill : public ArmorSkill
{
public:
    VineSkill()
        : ArmorSkill("Vine")
    {
        events << DamageInflicted << SlashEffected << CardEffected;
        frequency = Compulsory;
    }

    QList<SkillInvokeDetail> triggerable(TriggerEvent triggerEvent, const Room *, const QVariant &data) const
    {
        if (triggerEvent == SlashEffected) {
            SlashEffectStruct effect = data.value<SlashEffectStruct>();
            if (equipAvailable(effect.to, EquipCard::ArmorLocation, objectName()) && effect.nature == DamageStruct::Normal)
                return QList<SkillInvokeDetail>() << SkillInvokeDetail(this, effect.to, effect.to, NULL, true);
        } else if (triggerEvent == CardEffected) {
            CardEffectStruct effect = data.value<CardEffectStruct>();
            if (equipAvailable(effect.to, EquipCard::ArmorLocation, objectName()) && effect.card != NULL
                && (effect.card->isKindOf("SavageAssault") || effect.card->isKindOf("ArcheryAttack")))
                return QList<SkillInvokeDetail>() << SkillInvokeDetail(this, effect.to, effect.to, NULL, true);
        } else if (triggerEvent == DamageInflicted) {
            DamageStruct damage = data.value<DamageStruct>();
            if (equipAvailable(damage.to, EquipCard::ArmorLocation, objectName()) && damage.nature == DamageStruct::Fire)
                return QList<SkillInvokeDetail>() << SkillInvokeDetail(this, damage.to, damage.to, NULL, true);
        }

        return QList<SkillInvokeDetail>();
    }

    bool effect(TriggerEvent triggerEvent, Room *room, QSharedPointer<SkillInvokeDetail> invoke, QVariant &data) const
    {
        if (triggerEvent == SlashEffected) {
            SlashEffectStruct effect = data.value<SlashEffectStruct>();

            room->setEmotion(invoke->invoker, "armor/vine");
            LogMessage log;
            log.from = invoke->invoker;
            log.type = "#ArmorNullify";
            log.arg = objectName();
            log.arg2 = effect.slash->objectName();
            room->sendLog(log);

            effect.to->setFlags("Global_NonSkillNullify");
            return true;
        } else if (triggerEvent == CardEffected) {
            CardEffectStruct effect = data.value<CardEffectStruct>();
            room->setEmotion(invoke->invoker, "armor/vine");
            LogMessage log;
            log.from = invoke->invoker;
            log.type = "#ArmorNullify";
            log.arg = objectName();
            log.arg2 = effect.card->objectName();
            room->sendLog(log);

            effect.to->setFlags("Global_NonSkillNullify");
            return true;
        } else if (triggerEvent == DamageInflicted) {
            DamageStruct damage = data.value<DamageStruct>();
            room->setEmotion(invoke->invoker, "armor/vineburn");
            LogMessage log;
            log.type = "#VineDamage";
            log.from = invoke->invoker;
            log.arg = QString::number(damage.damage);
            log.arg2 = QString::number(++damage.damage);
            room->sendLog(log);

            data = QVariant::fromValue(damage);
        }

        return false;
    }
};

Vine::Vine(Suit suit, int number)
    : Armor(suit, number)
{
    setObjectName("Vine");
}

class SilverLionSkill : public ArmorSkill
{
public:
    SilverLionSkill()
        : ArmorSkill("SilverLion")
    {
        events << DamageInflicted << CardsMoveOneTime;
        frequency = Compulsory;
    }

    QList<SkillInvokeDetail> triggerable(TriggerEvent triggerEvent, const Room *, const QVariant &data) const
    {
        if (triggerEvent == DamageInflicted) {
            DamageStruct damage = data.value<DamageStruct>();
            if (equipAvailable(damage.to, EquipCard::ArmorLocation, objectName()) && damage.damage > 1)
                return QList<SkillInvokeDetail>() << SkillInvokeDetail(this, damage.to, damage.to, NULL, true);
        } else {
            CardsMoveOneTimeStruct move = data.value<CardsMoveOneTimeStruct>();

            if (!move.from_places.contains(Player::PlaceEquip))
                return QList<SkillInvokeDetail>();
            for (int i = 0; i < move.card_ids.size(); i++) {
                if (move.from_places[i] != Player::PlaceEquip)
                    continue;
                if (move.broken_ids.contains(move.card_ids[i]))
                    continue;
                const Card *card = Sanguosha->getEngineCard(move.card_ids[i]);
                if (card->objectName() == objectName()) {
                    // function  equipAvailable()    need armor remained in equip area
                    if (!move.from->isWounded() || !move.from->tag["Qinggang"].toStringList().isEmpty() || move.from->getMark("Armor_Nullified") > 0
                        || move.from->getMark("Equips_Nullified_to_Yourself") > 0) {
                        move.from->setFlags("-SilverLionRecover");
                        return QList<SkillInvokeDetail>();
                    }
                    ServerPlayer *invoker = qobject_cast<ServerPlayer *>(move.from);
                    if (invoker == NULL || invoker->isDead())
                        return QList<SkillInvokeDetail>();
                    return QList<SkillInvokeDetail>() << SkillInvokeDetail(this, invoker, invoker, NULL, true);
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
            log.type = "#SilverLion";
            log.from = invoke->invoker;
            log.arg = QString::number(damage.damage);
            log.arg2 = objectName();
            room->sendLog(log);

            damage.damage = 1;
            data = QVariant::fromValue(damage);
        } else {
            invoke->invoker->setFlags("-SilverLionRecover");
            room->setEmotion(invoke->invoker, "armor/silver_lion");
            RecoverStruct recover;
            CardsMoveOneTimeStruct move = data.value<CardsMoveOneTimeStruct>();
            for (int i = 0; i < move.card_ids.size(); i++) {
                if (move.from_places[i] != Player::PlaceEquip)
                    continue;
                const Card *card = Sanguosha->getEngineCard(move.card_ids[i]);
                if (card->objectName() == objectName()) {
                    recover.card = card;
                    break;
                }
            }
            room->recover(invoke->invoker, recover);
        }

        return false;
    }
};

SilverLion::SilverLion(Suit suit, int number)
    : Armor(suit, number)
{
    setObjectName("SilverLion");
}

void SilverLion::onUninstall(ServerPlayer *player) const
{
    if (player->isAlive() && EquipSkill::equipAvailable(player, this)) //shenbao?
        player->setFlags("SilverLionRecover");
}

FireAttack::FireAttack(Card::Suit suit, int number)
    : SingleTargetTrick(suit, number)
{
    setObjectName("fire_attack");
    can_damage = true;
}

bool FireAttack::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const
{
    int total_num = 1 + Sanguosha->correctCardTarget(TargetModSkill::ExtraTarget, Self, this);
    //bool ignore = (Self->hasSkill("tianqu") && Sanguosha->getCurrentCardUseReason() == CardUseStruct::CARD_USE_REASON_PLAY && to_select != Self && !hasFlag("IgnoreFailed"));

    return targets.length() < total_num && !to_select->isKongcheng() && (to_select != Self || !Self->isLastHandCard(this, true));
}

void FireAttack::onEffect(const CardEffectStruct &effect) const
{
    Room *room = effect.from->getRoom();
    if (effect.to->isKongcheng())
        return;

    const Card *card = room->askForCardShow(effect.to, effect.from, objectName());
    room->showCard(effect.to, card->getEffectiveId());

    QString suit_str = card->getSuitString();
    QString pattern = QString(".%1").arg(suit_str.at(0).toUpper());
    QString prompt = QString("@fire-attack:%1::%2").arg(effect.to->objectName()).arg(suit_str);
    if (effect.from->isAlive()) {
        const Card *card_to_throw = room->askForCard(effect.from, pattern, prompt);
        if (card_to_throw)
            room->damage(DamageStruct(this, effect.from, effect.to, 1, DamageStruct::Ice));
        else
            effect.from->setFlags("FireAttackFailed_" + effect.to->objectName()); // For AI
    }

    if (card->isVirtualCard())
        delete card;
}

IronChain::IronChain(Card::Suit suit, int number)
    : TrickCard(suit, number)
{
    setObjectName("iron_chain");
    can_recast = true;
}

QString IronChain::getSubtype() const
{
    return "damage_spread";
}

bool IronChain::targetFilter(const QList<const Player *> &targets, const Player *, const Player *Self) const
{
    int total_num = 2 + Sanguosha->correctCardTarget(TargetModSkill::ExtraTarget, Self, this);
    return targets.length() < total_num && !Self->isCardLimited(this, Card::MethodUse);
}

bool IronChain::targetsFeasible(const QList<const Player *> &targets, const Player *Self) const
{
    bool rec = (Sanguosha->currentRoomState()->getCurrentCardUseReason() == CardUseStruct::CARD_USE_REASON_PLAY) && can_recast;
    QList<int> sub;
    if (isVirtualCard())
        sub = subcards;
    else
        sub << getEffectiveId();

    foreach (int id, sub) {
        if (Self->getHandPile().contains(id)) {
            rec = false;
            break;
        } else { // for  skill chaoren
            if (id == Self->property("chaoren").toInt()) {
                rec = false;
                break;
            }
        }
    }
    if (this->getSkillName() == "guaiqi") //modian is not count as HandPile
        rec = false;

    if (rec && Self->isCardLimited(this, Card::MethodUse))
        return targets.length() == 0;

    if (Sanguosha->currentRoomState()->getCurrentCardUseReason() == CardUseStruct::CARD_USE_REASON_RESPONSE_USE)
        return targets.length() != 0;

    int total_num = 2 + Sanguosha->correctCardTarget(TargetModSkill::ExtraTarget, Self, this);
    if (!rec)
        return targets.length() > 0 && targets.length() <= total_num;
    else
        return targets.length() <= total_num;
}

void IronChain::onUse(Room *room, const CardUseStruct &card_use) const
{
    if (card_use.to.isEmpty()) {
        LogMessage log;
        log.type = "#Card_Recast";
        log.from = card_use.from;
        log.card_str = card_use.card->toString();
        room->sendLog(log);

        CardMoveReason reason(CardMoveReason::S_REASON_RECAST, card_use.from->objectName());
        reason.m_skillName = getSkillName();
        room->moveCardTo(this, card_use.from, NULL, Player::DiscardPile, reason);
        card_use.from->broadcastSkillInvoke("@recast");

        card_use.from->drawCards(1);
    } else
        TrickCard::onUse(room, card_use);
}

void IronChain::onEffect(const CardEffectStruct &effect) const
{
    effect.to->getRoom()->setPlayerProperty(effect.to, "chained", !effect.to->isChained());
}

SupplyShortage::SupplyShortage(Card::Suit suit, int number)
    : DelayedTrick(suit, number)
{
    setObjectName("supply_shortage");

    judge.pattern = ".|club";
    judge.good = true;
    judge.reason = objectName();
}

QString SupplyShortage::getSubtype() const
{
    return "unmovable_delayed_trick";
}

bool SupplyShortage::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const
{
    if (!targets.isEmpty() || to_select == Self)
        return false;
    bool ignore
        = (Self && Self->hasSkill("tianqu") && Sanguosha->getCurrentCardUseReason() == CardUseStruct::CARD_USE_REASON_PLAY && to_select != Self && !hasFlag("IgnoreFailed"));
    if (to_select->containsTrick(objectName()) && !ignore)
        return false;
    int distance_limit = 1 + Sanguosha->correctCardTarget(TargetModSkill::DistanceLimit, Self, this);
    int rangefix = 0;
    if (Self->getOffensiveHorse() && subcards.contains(Self->getOffensiveHorse()->getId()))
        rangefix += 1;

    if (Self->distanceTo(to_select, rangefix) > distance_limit)
        return false;

    return true;
}

void SupplyShortage::takeEffect(ServerPlayer *target) const
{
    target->skip(Player::Draw);
}

ManeuveringPackage::ManeuveringPackage()
    : Package("maneuvering", Package::CardPack)
{
    QList<Card *> cards;

    // clang-format off

    // spade
    cards << new GudingBlade(Card::Spade, 1)
          //<< new Vine(Card::Spade, 2)
          << new IronArmor(Card::Spade, 2)
          << new Analeptic(Card::Spade, 3)
          << new ThunderSlash(Card::Spade, 4)
          << new ThunderSlash(Card::Spade, 5)
          << new ThunderSlash(Card::Spade, 6)
          << new ThunderSlash(Card::Spade, 7)
          << new ThunderSlash(Card::Spade, 8)
          << new Analeptic(Card::Spade, 9)
          << new SupplyShortage(Card::Spade, 10)
          << new IronChain(Card::Spade, 11)
          << new IronChain(Card::Spade, 12)
          << new Nullification(Card::Spade, 13);
    // club
    cards << new SilverLion(Card::Club, 1)
          << new Vine(Card::Club, 2)
          << new Analeptic(Card::Club, 3)
          << new SupplyShortage(Card::Club, 4)
          << new ThunderSlash(Card::Club, 5)
          << new ThunderSlash(Card::Club, 6)
          << new ThunderSlash(Card::Club, 7)
          << new ThunderSlash(Card::Club, 8)
          << new Analeptic(Card::Club, 9)
          << new IronChain(Card::Club, 10)
          << new IronChain(Card::Club, 11)
          << new IronChain(Card::Club, 12)
          << new IronChain(Card::Club, 13);

    // heart
    cards << new Nullification(Card::Heart, 1)
          << new FireAttack(Card::Heart, 2)
          << new FireAttack(Card::Heart, 3)
          << new FireSlash(Card::Heart, 4)
          << new Peach(Card::Heart, 5)
          << new Peach(Card::Heart, 6)
          << new FireSlash(Card::Heart, 7)
          << new Jink(Card::Heart, 8)
          << new Jink(Card::Heart, 9)
          << new FireSlash(Card::Heart, 10)
          << new Jink(Card::Heart, 11)
          << new Jink(Card::Heart, 12)
          << new Nullification(Card::Heart, 13);

    // diamond
    cards << new Fan(Card::Diamond, 1)
          << new Peach(Card::Diamond, 2)
          << new Peach(Card::Diamond, 3)
          << new FireSlash(Card::Diamond, 4)
          << new FireSlash(Card::Diamond, 5)
          << new Jink(Card::Diamond, 6)
          << new Jink(Card::Diamond, 7)
          << new Jink(Card::Diamond, 8)
          << new Analeptic(Card::Diamond, 9)
          << new Jink(Card::Diamond, 10)
          << new Jink(Card::Diamond, 11)
          << new FireAttack(Card::Diamond, 12);

    // clang-format on

    DefensiveHorse *hualiu = new DefensiveHorse(Card::Diamond, 13);
    hualiu->setObjectName("HuaLiu");

    cards << hualiu;

    foreach (Card *card, cards)
        card->setParent(this);

    skills << new GudingBladeSkill << new FanSkill << new VineSkill << new SilverLionSkill << new IronArmorSkill;
}

ADD_PACKAGE(Maneuvering)
