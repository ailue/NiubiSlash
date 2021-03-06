#include "newbility.h"
#include "engine.h"
#include "skill.h"
#include "standard.h"
#include "standard-equips.h"
#include "carditem.h"
#include "client.h"
#include "settings.h"
#include "maneuvering.h"

JuicePeach::JuicePeach(Suit suit, int number):Peach(suit, number){
    setObjectName("juice_peach");
    target_fixed = false;
}

QString JuicePeach::getSubtype() const{
    return "recover_card";
}

bool JuicePeach::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && to_select->isWounded();
}

bool JuicePeach::isAvailable(const Player *player) const{
    return true;
}

Stink::Stink(Suit suit, int number):BasicCard(suit, number){
    setObjectName("stink");
    target_fixed = true;
}

QString Stink::getSubtype() const{
    return "disgusting_card";
}

QString Stink::getEffectPath(bool is_male) const{
    return "audio/card/common/stink.ogg";
}

void Stink::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    room->throwCard(this);
    ServerPlayer *nextfriend = targets.isEmpty() ? source->getNextAlive() : targets.first();
    room->setEmotion(nextfriend, "bad");
    if(!room->askForCard(nextfriend, "wall", "haochou")){
        room->swapSeat(nextfriend, nextfriend->getNextAlive());
    }
    else room->setEmotion(nextfriend, "good");
}

Wall::Wall(Suit suit, int number):BasicCard(suit, number){
    setObjectName("wall");
    target_fixed = true;
}

QString Wall::getSubtype() const{
    return "niubi_card";
}

QString Wall::getEffectPath(bool is_male) const{
    return "audio/card/common/wall.ogg";
}

void Wall::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    int id = this->getId();
    if(id < 0){
        id = !this->getSubcards().isEmpty() ? this->getSubcards().first() : -1;
        //if this wall is a clone card.
        if(id < 0) return;
    }
    ServerPlayer *target = targets.isEmpty() ? source : targets.first();
    target->addToPile("wall", id);
    room->acquireSkill(target, objectName(), false);
    room->setEmotion(target, "good");
}

class WallSkill: public TriggerSkill{
public:
    WallSkill():TriggerSkill("wall"){
        events << Predamaged << HpLost;
        frequency = Compulsory;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        if(event == HpLost){
            if(player->getPhase() != Player::NotActive
                || player->getPile("wall").isEmpty())
                return false;
            LogMessage log;
            log.type = "#WallForbidden";
            log.from = player;
            room->sendLog(log);
            return true;
        }

        DamageStruct damage = data.value<DamageStruct>();
        if(damage.damage > 0 && !player->getPile("wall").isEmpty()){
            LogMessage log;
            log.type = "$WallProtect";
            log.to << player;
            log.from = damage.from;
            log.card_str = QString::number(player->getPile("wall").first());
            room->sendLog(log);

            room->throwCard(player->getPile("wall").last());
            if(player->getPile("wall").isEmpty())
                room->detachSkillFromPlayer(player, objectName());

            damage.damage --;
            room->setEmotion(player, "good");
            data = QVariant::fromValue(damage);
        }
        return false;
    }
};

class PoisonSkill: public TriggerSkill{
public:
    PoisonSkill():TriggerSkill("poison_skill"){
        events << PhaseChange << CardUsed;
    }

    virtual int getPriority() const{
        return 2;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        if(event == PhaseChange && player->getPhase() == Player::Start){
            LogMessage log;
            log.from = player;
            if(player->getMark("poison") > 0 && player->getMark("poison") < 4){
                //1~3为中毒期
                log.type = "#Poison_hurt";
                room->sendLog(log);

                DamageStruct damage;
                damage.from = damage.to = player;
                damage.card = NULL;
                room->damage(damage);

                player->addMark("poison");
            }
            else if(player->getMark("poison") > 3){
                //超过3，自动解毒
                room->setPlayerMark(player, "poison", 0);
                room->setEmotion(player, "good");

                log.type = "#Poison_auto";
                room->sendLog(log);
                room->detachSkillFromPlayer(player, objectName());
            }
        }
        else if(event == CardUsed){
            CardUseStruct card_use = data.value<CardUseStruct>();
            const Card *card = card_use.card;

            if(player->getMark("poison") > 0){
                LogMessage log;
                log.from = player;
                if(card->inherits("Analeptic") && player->getState() != "robot"){
                    //机器人喝酒可以不死
                    log.type = "#Poison_die";
                    room->sendLog(log);
                    room->killPlayer(player);
                }
                else if(card->inherits("Peach") && card->getSuit() == Card::Diamond){
                    //方块桃可以解毒
                    room->setPlayerMark(card_use.from, "poison", 0);
                    room->setEmotion(card_use.from, "good");
                    LogMessage log;
                    log.type = "#Poison_out";
                    log.from = player;
                    room->sendLog(log);
                    room->detachSkillFromPlayer(card_use.from, objectName());
                }
            }
        }
        return false;
    }
};

Poison::Poison(Suit suit, int number)
    : BasicCard(suit, number)
{
    setObjectName("poison");
}

QString Poison::getSubtype() const{
    return "niubi_card";
}

QString Poison::getEffectPath(bool is_male) const{
    return "audio/card/common/poison.ogg";
}

bool Poison::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && Self->distanceTo(to_select) <= 1;
}

void Poison::onEffect(const CardEffectStruct &card_effect) const{
    Room *room = card_effect.from->getRoom();

    if(card_effect.to->getMark("poison") == 0){
        room->setEmotion(card_effect.from, "good");
        room->setPlayerMark(card_effect.to, "poison", 1);
        room->setEmotion(card_effect.to, "bad");

        LogMessage log;
        log.type = "#Poison_in";
        log.from = card_effect.to;
        room->sendLog(log);
        room->acquireSkill(card_effect.to, "poison_skill", false);
    }
    else{
        room->setPlayerMark(card_effect.to, "poison", 0);
        room->setEmotion(card_effect.to, "good");
        LogMessage log;
        log.type = "#Poison_out";
        log.from = card_effect.to;
        room->sendLog(log);

        room->detachSkillFromPlayer(card_effect.to, "poison_skill");
    }
}

Sacrifice::Sacrifice(Suit suit, int number)
    :SingleTargetTrick(suit, number, false) {
    setObjectName("sacrifice");
}

bool Sacrifice::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(!targets.isEmpty())
        return false;

    if(!to_select->isWounded())
        return false;

    return true;
}

void Sacrifice::onEffect(const CardEffectStruct &effect) const{
    if(!effect.to->isWounded())
        return;

    Room *room = effect.to->getRoom();
    room->loseHp(effect.from);

    RecoverStruct recover;
    recover.card = this;
    recover.who = effect.from;
    room->recover(effect.to, recover, true);
}

class ClearShirtSkill: public ArmorSkill{
public:
    ClearShirtSkill():ArmorSkill("clear_shirt"){
        events << Predamage;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        if(damage.to->getNextAlive() == damage.from){
            LogMessage log;
            log.type = "#CSProtect";
            log.from = damage.to;
            log.arg = QString::number(damage.damage);
            log.arg2 = objectName();
            player->getRoom()->sendLog(log);
            return true;
        }
        return false;
    }
};

ClearShirt::ClearShirt(Suit suit, int number) :Armor(suit, number){
    setObjectName("clear_shirt");
    skill = new ClearShirtSkill;
}

void ClearShirt::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    ServerPlayer *master = targets.isEmpty() ?  source->getNextAlive() : targets.first();
    if(master->getArmor())
        room->throwCard(master->getArmor());
    room->moveCardTo(this, master, Player::Equip, true);
}

Niubi::Niubi(Suit suit, int number)
    :Armor(suit, number){
}

void Niubi::onUninstall(ServerPlayer *player) const{
    player->getRoom()->detachSkillFromPlayer(player, objectName());
    player->loseSkill(objectName());
}

void Niubi::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    QString owner = room->getNiubiOwner(objectName());
    ServerPlayer *master = targets.isEmpty() ?
                           room->findPlayer(owner) : targets.first();
    if(!master){
        QString sp_owner = "sp_" + owner;
        foreach(ServerPlayer *tmp, room->getAllPlayers()){
            if(tmp->getGeneral2Name() == owner ||
               tmp->getGeneralName() == sp_owner ||
               tmp->getGeneral2Name() == sp_owner){
                master = tmp;
                break;
            }
        }
    }
    LogMessage log;
    if(master && master->isAlive()){
        room->throwCard(master->getArmor());

        room->moveCardTo(this, master, Player::Equip, true);
        log.from = master;
        if(room->getNiubiOwner(objectName(), 2) == "standard")
            log.type = "$Nstandard";  //标准版武将神装台词未定义
        else
            log.type = "$" + objectName();
        room->sendLog(log);

        if(objectName() == "dustbin" || objectName() == "animals" ||
           objectName() == "rollingpin" || objectName() == "wookon")
            room->attachSkillToPlayer(master, objectName()); //左下角显示按钮
        else
            room->acquireSkill(master, objectName(), false); //无按钮
    }
    else{
        room->throwCard(this);
        room->playCardEffect("@recast", source->getGeneral()->isMale());

        log.from = source;
        log.type = "$Thrownb";
        log.card_str = QString::number(getEffectiveId());
        room->sendLog(log);

        source->drawCards(1);
    }
}

//special niubi skills
class EyedropsSkill: public PhaseChangeSkill{
public:
    EyedropsSkill():PhaseChangeSkill("eyedrops"){
    }
    virtual int getPriority() const{
        return 3;
    }
    virtual bool onPhaseChange(ServerPlayer *ear) const{
        if(ear->hasArmorEffect("eyedrops") && ear->getPhase() == Player::NotActive){
           ear->drawCards(2);
        }
        return false;
    }
};

class RedsunglassesSkill: public TriggerSkill{
public:
    RedsunglassesSkill():TriggerSkill("redsunglasses"){
        events << SlashMissed;
    }
    virtual bool trigger(TriggerEvent , ServerPlayer *guanyu, QVariant &data) const{
        if(!guanyu->hasArmorEffect("redsunglasses"))
            return false;
        Room *room = guanyu->getRoom();
        SlashEffectStruct effect = data.value<SlashEffectStruct>();
        const Card *card = room->askForCard(guanyu, "slash", "red-slash");
        if(card){
            if(guanyu->hasFlag("drank"))
                room->setPlayerFlag(guanyu, "-drank");

            QList<ServerPlayer *> canselplayers;
            foreach(ServerPlayer *player, room->getAlivePlayers()){
            if(player->canSlash(effect.to) && effect.to!=player)
                canselplayers << player;
            }
            if(!canselplayers.isEmpty()){
                ServerPlayer *target = room->askForPlayerChosen(guanyu, canselplayers, objectName());
                room->cardEffect(card, guanyu, target);
            }
        }
        return false;
    }
};

class SpeakerSkill: public TriggerSkill{
public:
    SpeakerSkill():TriggerSkill("speakers"){
        events << Predamage;
    }
    virtual bool trigger(TriggerEvent , ServerPlayer *zhangfei, QVariant &data) const{
        if(zhangfei->hasArmorEffect("speakers")){
            DamageStruct damage = data.value<DamageStruct>();
            const Card *reason = damage.card;
            if(reason == NULL)
                return false;

            if(reason->inherits("Slash") && zhangfei->getSlashCount()==2){
                LogMessage log;
                log.type = "#PaoxiaoBuff";
                log.from = zhangfei;
                log.to << damage.to;
                log.arg = QString::number(damage.damage);
                log.arg2 = QString::number(damage.damage + 1);
                zhangfei->getRoom()->sendLog(log);

                damage.damage ++;
                data = QVariant::fromValue(damage);
            }
        }
        return false;
    }
};

class BananaSkill: public TriggerSkill{
public:
    BananaSkill():TriggerSkill("banana"){
        events << CardResponsed;
    }
    virtual bool trigger(TriggerEvent , ServerPlayer *vdyz, QVariant &data) const{
        if(!vdyz->hasArmorEffect("banana"))
            return false;
        CardStar card_star = data.value<CardStar>();
        if(card_star->inherits("Jink")){
            Room *room = vdyz->getRoom();
            ServerPlayer *target = room->askForPlayerChosen(vdyz, room->getAlivePlayers(), objectName());
            CardEffectStruct effect;
            Slash *slash = new Slash(Card::NoSuit, 4);
            slash->setSkillName(objectName());
            effect.card = slash;
            effect.from = vdyz;
            effect.to = target;
            room->cardEffect(effect);
        }
        return false;
    }
};

class FengjieSkill: public TriggerSkill{
public:
    FengjieSkill():TriggerSkill("madamfeng"){
        events << CardLost << PhaseChange;
    }
    virtual bool trigger(TriggerEvent event, ServerPlayer *hhyxyq, QVariant &data) const{
        if(hhyxyq->hasArmorEffect("madamfeng") && hhyxyq->getPhase() == Player::Discard){
            Room *room = hhyxyq->getRoom();
            if(event == CardLost){
                CardMoveStar move = data.value<CardMoveStar>();
                if(move->to_place == Player::DiscardedPile){
                    hhyxyq->addMark("jizhi");
                    if(hhyxyq->getMark("jizhi") == 3){
                        if(hhyxyq->askForSkillInvoke("madamfeng")){
                            ServerPlayer *target = room->askForPlayerChosen(hhyxyq, room->getAllPlayers(), "madamfeng");
                            RecoverStruct recover;
                            recover.who = hhyxyq;
                            room->recover(target, recover);
                        }
                    }
                }
            }else
                hhyxyq->setMark("jizhi", 0);
        }
        return false;
    }
};

class DustbinSkill: public OneCardViewAsSkill{
public:
    DustbinSkill():OneCardViewAsSkill("dustbin"){
    }
    virtual bool isEnabledAtPlay(const Player *player) const{
        return player->hasArmorEffect("dustbin") && player->getFlags()!="dust";
    }
    virtual bool viewFilter(const CardItem *to_select) const{
        return !to_select->isEquipped();
    }
    virtual const Card *viewAs(CardItem *card_item) const{
        DummyCard *dummy = new DummyCard();
        dummy->addSubcard(card_item->getCard());
        dummy->setObjectName(objectName());
        Self->setFlags("dust");
        return dummy;
    }
};

class AnimalsSkill: public OneCardViewAsSkill{
public:
    AnimalsSkill():OneCardViewAsSkill("animals"){
    }
    virtual bool isEnabledAtPlay(const Player *player) const{
        return player->hasArmorEffect("animals");
    }
    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getCard()->inherits("TrickCard") && to_select->getCard()->getSuit() == Card::Spade;
    }
    virtual const Card *viewAs(CardItem *card_item) const{
        const Card *card = card_item->getCard();
        Duel *duel = new Duel(card->getSuit(), card->getNumber());
        duel->addSubcard(card);
        duel->setSkillName(objectName());
        return duel;
    }
};

RollingpinCard::RollingpinCard(){
    once = true;
    target_fixed = true;
}

void RollingpinCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    room->throwCard(this);
    RecoverStruct recover;
    recover.card = this;
    recover.who = source;
    recover.recover = 2;
    room->recover(source, recover, true);
}

class RollingpinSkill: public ViewAsSkill{
public:
    RollingpinSkill():ViewAsSkill("rollingpin"){
    }
    virtual bool isEnabledAtPlay(const Player *player) const{
        return player->hasArmorEffect("rollingpin") && !player->hasUsed("RollingpinCard");
    }
    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        return selected.length() < 2 && !to_select->isEquipped();
    }
    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.length() != 2)
            return NULL;
        RollingpinCard *card = new RollingpinCard;
        card->addSubcards(cards);
        return card;
    }
};

class DeathriSkill: public TriggerSkill{
public:
    DeathriSkill():TriggerSkill("deathrisk"){
        events << CardUsed;
    }
    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        if(player->hasArmorEffect("deathrisk") && player->hasSkill("jijiu")){
            CardUseStruct use = data.value<CardUseStruct>();
            if(use.from == player && use.card->getSkillName() == "jijiu"){
                ServerPlayer *use_to = use.to.isEmpty()? player : use.to.first();
                use_to->addMark("risk");
                if(use_to->getMark("risk") > 2){
                    player->drawCards(3);
                    use_to->setMark("risk", 0);
                }
            }
        }
        return false;
    }
};

class AmazonstonSkill: public TriggerSkill{
public:
    AmazonstonSkill():TriggerSkill("amazonston"){
        events << SlashHit;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();
        Room *rm = player->getRoom();
        if(player->hasSkill("liegong") && player->hasArmorEffect("amazonston")){
            rm->throwCard(effect.to->getDefensiveHorse());
            rm->throwCard(effect.to->getArmor());
            rm->throwCard(effect.to->getOffensiveHorse());
            rm->throwCard(effect.to->getWeapon());
        }
        return false;
    }
};

class GnatSkill: public MasochismSkill{
public:
    GnatSkill():MasochismSkill("gnat"){
    }
    virtual void onDamaged(ServerPlayer *ren, const DamageStruct &) const{
        if(ren->hasArmorEffect("gnat") && !ren->faceUp()){
            ren->turnOver();
            ren->drawCards(2);
        }
    }
};

class TranqgunSkill: public TriggerSkill{
public:
    TranqgunSkill():TriggerSkill("tranqgun"){
        events << PhaseChange;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        ServerPlayer *xh = target->getRoom()->findPlayerBySkillName("duanliang");
        return xh && xh->hasArmorEffect("tranqgun");
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &) const{
        if(player->getPhase() != Player::Draw)
            return false;
        Room *room = player->getRoom();
        ServerPlayer *xh = room->findPlayerBySkillName("duanliang");
        if(xh && room->askForSkillInvoke(xh, "tranqgun", QVariant::fromValue(player)) && room->askForCard(xh,".C","tranqgun_ask")){
            JudgeStruct judge;
            judge.pattern = QRegExp("(.*):(club):(.*)");
            judge.good = true;
            judge.reason = objectName();
            judge.who = player;

            room->judge(judge);
            if(judge.isBad()){
                return true;
            }
        }
        return false;
    }
};

class TombstoneSkill: public TriggerSkill{
public:
    TombstoneSkill():TriggerSkill("tombstone"){
        events << Predamage;
    }
    virtual bool trigger(TriggerEvent , ServerPlayer *moou, QVariant &data) const{
        if(moou->hasArmorEffect("tombstone")){
            DamageStruct damage = data.value<DamageStruct>();
            const Card *reason = damage.card;
            if(reason == NULL)
                return false;
            if(reason->inherits("Slash") || reason->inherits("Shit")){
                LogMessage log;
                log.type = "#TombBuff";
                log.from = moou;
                log.to << damage.to;
                log.arg = QString::number(damage.damage);
                log.arg2 = QString::number(damage.damage + 1);
                moou->getRoom()->sendLog(log);
                damage.damage ++;
                data = QVariant::fromValue(damage);
            }
        }
        return false;
    }
};

class SwitchbdSkill: public TriggerSkill{
public:
    SwitchbdSkill():TriggerSkill("switchbd"){
        events << SlashEffected << CardAsked;
    }

    virtual int getPriority() const{
        return 2;
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return player->hasSkill("bazhen");
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *player, QVariant &data) const{
        if(event == CardAsked){
            QString asked = data.toString();
            if(asked == "jink"){
                Room *room = player->getRoom();
                if(room->askForSkillInvoke(player, objectName())){
                    JudgeStruct judge;
                    judge.pattern = QRegExp("(.*):(heart|diamond):(.*)");
                    judge.good = true;
                    judge.reason = objectName();
                    judge.who = player;
                    room->judge(judge);
                    if(judge.isGood()){
                        Jink *jink = new Jink(Card::NoSuit, 0);
                        jink->setSkillName(objectName());
                        room->provide(jink);
                        room->setEmotion(player, "good");
                        return true;
                    }else
                        room->setEmotion(player, "bad");
                }
            }
            return false;
        }

        SlashEffectStruct effect = data.value<SlashEffectStruct>();
        if(effect.slash->isBlack()){
            LogMessage log;
            log.type = "#ArmorNullify";
            log.from = player;
            log.arg = objectName();
            log.arg2 = effect.slash->objectName();
            player->getRoom()->sendLog(log);

            return true;
        }else
            return false;
    }
};

class GoldlockSkill: public TriggerSkill{
public:
    GoldlockSkill():TriggerSkill("goldlock"){
        events << PhaseChange << Predamaged;
    }
    virtual bool trigger(TriggerEvent event, ServerPlayer *yuanshao, QVariant &data) const{
        if(!yuanshao->hasArmorEffect("goldlock"))
            return false;
        if(event == PhaseChange && yuanshao->getPhase() == Player::Start)
            yuanshao->drawCards(2);
        else if(event == Predamaged)
            if(yuanshao->getPhase() != Player::NotActive)
                return true;
        return false;
    }
};

class AppleSkill: public TriggerSkill{
public:
    AppleSkill():TriggerSkill("apple"){
        events << Pindian;
    }

    virtual int getPriority() const{
        return -1;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target->hasArmorEffect(objectName());
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        PindianStar pindian = data.value<PindianStar>();
        if(pindian->reason == "tianyi" &&
           player->hasSkill("tianyi") &&
           pindian->isSuccess())
            player->obtainCard(pindian->from_card);
        return false;
    }
};

WookonCard::WookonCard(){
    once = true;
    will_throw = false;
    target_fixed = true;
}

void WookonCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    source->addToPile("field", this->getSubcards().first(), false);
}

class WookonSkill: public OneCardViewAsSkill{
public:
    WookonSkill():OneCardViewAsSkill("wookon"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return ! player->hasUsed("WookonCard");
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return !to_select->isEquipped();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        WookonCard *card = new WookonCard;
        card->addSubcard(card_item->getFilteredCard());
        return card;
    }
};


//generals
class Yabian: public TriggerSkill{
public:
    Yabian():TriggerSkill("yabian"){
        events << Damage;
        frequency = Frequent;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *yanpeng, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();

        if(damage.card && damage.card->inherits("Slash") &&
           damage.from == yanpeng && damage.to != yanpeng){
            Room *room = yanpeng->getRoom();
            if(room->askForSkillInvoke(yanpeng, objectName(), data)){
                if(damage.to->getGeneralName() == "shenzhugeliang"){
                    foreach(int card_id, damage.to->getPile("stars"))
                        yanpeng->addToPile("stars", card_id, false);
                }
                //some special generals
                QList<QString> marks;
                marks
                        << "@chaos" /*luanwu*/
                        << "@drig" /*diezhi*/
                        << "@nirvana" /*niepan*/
                        << "@star" /*qixing*/
                        << "@flame" /*yeyan*/
                        << "@wrath" /*baonu*/
						<< "@quan" /*nongquan*/
                        ;
                foreach(QString mark, marks){
                    if(damage.to->getMark(mark)){
                        yanpeng->gainMark(mark, damage.to->getMark(mark));
                        damage.to->loseMark(mark, damage.to->getMark(mark));
                    }
                }
                //some other marks
                QString hisname = damage.to->getGeneralName();
                QString hiskindom = damage.to->getKingdom();
                QString myname = damage.from->getGeneralName();
                QString mykindom = damage.from->getKingdom();
                if(damage.to->isAlive()){
                    room->transfigure(damage.to, myname, false, false);
                    room->setPlayerProperty(damage.to, "kingdom", mykindom);
                }
                room->transfigure(damage.from, hisname, false, false);
                room->setPlayerProperty(damage.from, "kingdom", hiskindom);
            }
        }
        return false;
    }
};

WutianCard::WutianCard(){
    once = true;
}

bool WutianCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(targets.length() > 1)
        return false;
    return !to_select->isAllNude() && to_select->getPile("wall").length() < 2;
}

void WutianCard::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();
    int card_id = room->askForCardChosen(effect.from, effect.to, "hej", "wutian");
    effect.to->addToPile("wall",card_id);
    room->acquireSkill(effect.to, "wall");
    room->setEmotion(effect.to, "good");
    room->playCardEffect("wall",true);
}

class WutianViewAsSkill: public ZeroCardViewAsSkill{
public:
    WutianViewAsSkill(): ZeroCardViewAsSkill("wutian"){
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return pattern == "@@wutian";
    }

    virtual const Card *viewAs() const{
        WutianCard *card = new WutianCard;
        card->setSkillName("wutian");
        return card;
    }
};

class Wutian: public PhaseChangeSkill{
public:
    Wutian():PhaseChangeSkill("wutian"){
        view_as_skill = new WutianViewAsSkill;
    }

    virtual bool onPhaseChange(ServerPlayer *shixie) const{
        if(shixie->getPhase() == Player::Finish){
            Room *room = shixie->getRoom();
            if(!shixie->isWounded()){
                QList<ServerPlayer *> players = room->getOtherPlayers(shixie);
                foreach(ServerPlayer *player, players){
                    if(shixie->getHandcardNum() > player->getHandcardNum())
                        return false;
                }
            }
            room->askForUseCard(shixie, "@@wutian", "@wutian");
        }
        return false;
    }
};

class Wuzheng: public TriggerSkill{
public:
    Wuzheng():TriggerSkill("wuzheng$"){
        events << GameStart;
    }

    virtual int getPriority() const{
        return -3;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *shixie, QVariant &) const{
        Room *room = shixie->getRoom();
        if(shixie->isLord() && room->askForSkillInvoke(shixie, objectName(), true)){
            ServerPlayer *target = room->askForPlayerChosen(shixie,room->getOtherPlayers(shixie),objectName());
            QString role = target->getRole();
            room->setPlayerProperty(target, "role", shixie->getRole());
            shixie->setRole(role);
            role = room->askForChoice(shixie, "wuzheng-info", shixie->getRole());
        }
        return false;
    }
};

class Leiluo: public TriggerSkill{
public:
    Leiluo():TriggerSkill("leiluo"){
        events << GameStart << TurnStart;
        frequency = Compulsory;
    }
    virtual bool triggerable(const ServerPlayer *player) const{
        Room *room = player->getRoom();
        if(room->getMode() == "02_1v1" || room->getMode() == "06_3v3")
            return false;
        return player->hasSkill(objectName());
    }
    virtual bool trigger(TriggerEvent , ServerPlayer *kanze, QVariant &) const{
        if(kanze->getRole() == "renegade"){
            kanze->setRole("loyalist");
        }
        return false;
    }
};

DiezhiCard::DiezhiCard(){
    target_fixed = true;
}

void DiezhiCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &) const{
    source->loseMark("@drig");
    QString myrole = source->getRole();
    room->broadcastInvoke("animate", "lightbox:$diezhi");

    QList<ServerPlayer *> lucky_players, cup_players;
    foreach(ServerPlayer *player, room->getOtherPlayers(source)){
        QString result = room->askForChoice(player, "drig-guess", "lord+loyalist+rebel+renegade");
        if(result == myrole)
            lucky_players << player;
        else{
            cup_players << player;
        }
    }
    foreach(ServerPlayer *player, cup_players){
        DamageStruct damage;
        damage.from = source;
        damage.to = player;
        damage.nature = DamageStruct::Fire;
        room->setEmotion(player, "bad");
        room->damage(damage);

        if(player->isAlive()){
            damage.from = player;
            damage.nature = DamageStruct::Thunder;
            room->setEmotion(player, "bad");
            room->damage(damage);
            if(player->isAlive())
                room->loseHp(player);
        }
    }
    foreach(ServerPlayer *player, lucky_players){
        player->drawCards(3);
    }
}

class Diezhi: public ZeroCardViewAsSkill{
public:
    Diezhi():ZeroCardViewAsSkill("diezhi"){
        frequency = Limited;
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return player->getMark("@drig") > 0;
    }

    virtual const Card *viewAs() const{
        return new DiezhiCard;
    }

};

class Rangli: public TriggerSkill{
public:
    Rangli():TriggerSkill("rangli"){
        events << PhaseChange << DrawNCards;
    }
    virtual bool triggerable(const ServerPlayer *player) const{;
        return player->hasSkill(objectName()) || player->getMark("@pear") > 0;
    }
    virtual bool trigger(TriggerEvent event, ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        if(event == DrawNCards){
            int pearnum = player->getMark("@pear");
            data = data.toInt() + pearnum * 2;
            player->loseAllMarks("@pear");
            return false;
        }
        if(player->getMark("@pear") > 0 || player->getPhase() != Player::Draw)
            return false;
        if(player->askForSkillInvoke(objectName(), data)){
            QList<ServerPlayer *> players;
            foreach(ServerPlayer *tmp, room->getAlivePlayers()){
                if(tmp->getHandcardNum() < 2){
                    if(tmp->hasSkill("lianying"))
                        players << tmp;
                    else if(tmp->hasSkill("shangshi") && tmp->isWounded())
                        players << tmp;
                    else
                        continue;
                }
                else
                    players << tmp;
            }
            if(players.isEmpty())
                return false;
            ServerPlayer *target = room->askForPlayerChosen(player, players, objectName());
            target->gainMark("@pear");
            player->obtainCard(room->askForCardShow(target, player, objectName()));
            player->obtainCard(room->askForCardShow(target, player, objectName()));
            return true;
        }
        return false;
    }
};

class Jieao: public PhaseChangeSkill{
public:
    Jieao():PhaseChangeSkill("jieao"){
        frequency = Compulsory;
    }

    virtual bool onPhaseChange(ServerPlayer *miheng) const{
        if(miheng->getPhase() == Player::Start && miheng->getHp() > miheng->getHandcardNum()){
            LogMessage log;
            log.type = "#TriggerSkill";
            log.from = miheng;
            log.arg = objectName();
            miheng->getRoom()->sendLog(log);
            miheng->drawCards(2);
        }
        return false;
    }
};

YuluCard::YuluCard(){
    target_fixed = true;
    will_throw = false;
}

void YuluCard::use(Room *, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    foreach(int word_id, this->getSubcards()){
        source->addToPile("word", word_id);
    }
}

class Yulu: public ViewAsSkill{
public:
    Yulu():ViewAsSkill("yulu"){
    }

    virtual bool viewFilter(const QList<CardItem *> &, const CardItem *to_select) const{
        return !to_select->isEquipped();
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.isEmpty())
            return NULL;
        YuluCard *yulu_card = new YuluCard;
        yulu_card->addSubcards(cards);
        return yulu_card;
    }
};

ViewMyWordsCard::ViewMyWordsCard(){
    target_fixed = true;
}

void ViewMyWordsCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    QList<int> words = source->getPile("word");
    if(words.isEmpty())
        return;
    room->fillAG(words, source);
    int card_id = room->askForAG(source, words, true, "viewmywords");
    if(card_id != -1){
        words.removeOne(card_id);
        room->moveCardTo(Sanguosha->getCard(card_id), source, Player::Hand, false);
    }
    source->invoke("clearAG");
    words.clear();
}

class ViewMyWords: public ZeroCardViewAsSkill{
public:
    ViewMyWords():ZeroCardViewAsSkill("numa"){
    }
    virtual const Card *viewAs() const{
        return new ViewMyWordsCard;
    }
};

class Numa: public PhaseChangeSkill{
public:
    Numa():PhaseChangeSkill("numa"){
        view_as_skill = new ViewMyWords;
    }

    virtual bool onPhaseChange(ServerPlayer *miheng) const{
        Room *room = miheng->getRoom();
        if(miheng->getPhase() == Player::Finish &&
           !miheng->getPile("word").isEmpty() &&
           room->askForSkillInvoke(miheng, objectName())){
            Room *room = miheng->getRoom();
            QString c,word;
            foreach(int i, miheng->getPile("word")){
                c = Sanguosha->getCard(i)->getSuitString().left(1);

                LogMessage log;
                log.type = "#Numasingle";
                log.from = miheng;
                log.arg = objectName() + c;
                room->sendLog(log);

                word = word + c;
            }

            LogMessage gitlog;
            gitlog.type = "#Numa_" + word;
            gitlog.from = miheng;
            if(word == "hc"){
                room->sendLog(gitlog);
                //womei:recover self
                RecoverStruct womei;
                womei.card = NULL;
                womei.who = miheng;
                room->recover(miheng, womei);
            }
            else if(word == "dc"){
                room->sendLog(gitlog);
                //nimei:throw single player 2 cards
                QList<ServerPlayer *> players;
                foreach(ServerPlayer *tmp, room->getAlivePlayers()){
                    if(tmp->getHandcardNum() < 2){
                        if(tmp->hasSkill("lianying"))
                            players << tmp;
                        else if(tmp->hasSkill("shangshi") && tmp->isWounded())
                            players << tmp;
                        else
                            continue;
                    }
                    else
                        players << tmp;
                }
                room->askForDiscard(room->askForPlayerChosen(miheng, players, objectName()), objectName(), 2);
            }
            else if(word == "cc"){
                room->sendLog(gitlog);
                //meimei:clear single player's all judge_area
                QList<ServerPlayer *> players;
                foreach(ServerPlayer *tmp, room->getAlivePlayers()){
                    if(!tmp->getJudgingArea().isEmpty())
                        players << tmp;
                }
                if(!players.isEmpty()){
                    ServerPlayer *target = room->askForPlayerChosen(miheng, players, objectName());
                    foreach(const Card *c, target->getJudgingArea()){
                        room->throwCard(c->getId());
                    }
                }
            }
            else if(word == "sd"){
                room->sendLog(gitlog);
                //rini:let single player tribute a card and recover 1 hp
                QList<ServerPlayer *> players;
                foreach(ServerPlayer *tmp, room->getOtherPlayers(miheng)){
                    if(tmp->isWounded() && !tmp->isKongcheng())
                        players << tmp;
                }
                if(!players.isEmpty()){
                    ServerPlayer *target = room->askForPlayerChosen(miheng, players, objectName());
                    const Card *card = room->askForCardShow(target, miheng, objectName());
                    miheng->obtainCard(card);
                    RecoverStruct rini;
                    rini.card = card;
                    rini.who = miheng;
                    room->recover(target, rini);
                }
            }
            else if(word == "hs"){
                room->sendLog(gitlog);
                //wori:get skill fanchun
                JudgeStruct judge;
                judge.pattern = QRegExp("(Peach|GodSalvation):(.*):(.*)");
                judge.good = true;
                judge.reason = objectName();
                judge.who = miheng;
                room->judge(judge);
                if(judge.isGood())
                    room->acquireSkill(miheng, "fanchun");
            }
            else if(word == "hsc" || word == "hsd"){
                room->sendLog(gitlog);
                //worimei&worini:recover hp with a girl or a boy
                QList<ServerPlayer *> players;
                foreach(ServerPlayer *tmp, room->getOtherPlayers(miheng)){
                    if(((word == "hsc" && tmp->getGeneral()->isFemale()) ||
                       (word == "hsd" && tmp->getGeneral()->isMale())) && tmp->isWounded())
                        players << tmp;
                }
                if(!players.isEmpty()){
                    ServerPlayer *target = room->askForPlayerChosen(miheng, players, objectName());
                    RecoverStruct worimei;
                    worimei.card = NULL;
                    worimei.who = miheng;
                    room->recover(target, worimei);
                    room->recover(miheng, worimei);
                }
            }
            else if(word == "dsh"){
                room->sendLog(gitlog);
                //niriwo:call slash me! or taking away all his cards
                QList<ServerPlayer *> players;
                foreach(ServerPlayer *tmp, room->getAlivePlayers()){
                    if(tmp->canSlash(miheng))
                        players << tmp;
                }
                ServerPlayer *target = room->askForPlayerChosen(miheng, players, objectName());
                const Card *slash = room->askForCard(target, "slash", objectName());
                if(slash){
                    CardUseStruct niriwo;
                    niriwo.card = slash;
                    niriwo.to << miheng;
                    niriwo.from = target;
                    room->useCard(niriwo);
                }else if(!target->isNude()){
                    QList<const Card *> cards = target->getCards("hej");
                    foreach(const Card *tmp, cards)
                        room->moveCardTo(tmp, miheng, Player::Hand, false);
                }
            }
            else if(word == "shc"){
                room->sendLog(gitlog);
                //riwomei:let single player damage myself and recover himself
                DamageStruct riwmei;
                ServerPlayer *target = room->askForPlayerChosen(miheng, room->getAlivePlayers(), objectName());
                riwmei.from = target;
                riwmei.to = miheng;
                room->damage(riwmei);

                RecoverStruct riwomei;
                riwomei.card = NULL;
                riwomei.who = miheng;
                room->recover(target, riwomei);
            }
            else if(word == "hhh"){
                room->sendLog(gitlog);
                //wowowo:the same to Jushou
                miheng->turnOver();
                miheng->drawCards(3);
            }
            else if(word == "sss"){
                room->sendLog(gitlog);
                //ririri:the same to Fangzhu
                ServerPlayer *target = room->askForPlayerChosen(miheng, room->getAlivePlayers(), objectName());
                target->turnOver();
                target->drawCards(miheng->getMaxHP() - miheng->getHp());
            }
            else if(word == "ddd"){
                room->sendLog(gitlog);
                //ninini:let a player obtain word-card
                ServerPlayer *target = room->askForPlayerChosen(miheng, room->getAlivePlayers(), objectName());
                foreach(int i, miheng->getPile("word"))
                    room->moveCardTo(Sanguosha->getCard(i), target, Player::Hand);
            }
            else if(word == "ccc"){
                room->sendLog(gitlog);
                //meimeimei:clear single player's all equip_area
                ServerPlayer *target = room->askForPlayerChosen(miheng, room->getAlivePlayers(), objectName());
                target->throwAllEquips();
            }
            else if(word == "dcdc"){
                room->sendLog(gitlog);
                //nimeinimei:make a extra turn
                ServerPlayer *target = room->askForPlayerChosen(miheng, room->getAlivePlayers(), objectName());
                foreach(int i, miheng->getPile("word"))
                    room->throwCard(i);
                room->setCurrent(target);
                room->getThread()->trigger(TurnStart, target);
                room->setCurrent(miheng);
            }
            else if(word == "sdc" || word == "hsdc"){
                room->sendLog(gitlog);
                //rinimei:slash
                //worinimei:drank and slash
                if(word == "hsdc")
                    room->setPlayerFlag(miheng, "drank");

                QList<ServerPlayer *> players;
                foreach(ServerPlayer *tmp, room->getAlivePlayers()){
                    if(tmp->hasSkill("kongcheng") && tmp->isKongcheng())
                        continue;
                    players << tmp;
                }
                ServerPlayer *target = room->askForPlayerChosen(miheng, players, objectName());

                int slashtype = Sanguosha->getCard(miheng->getPile("word").first())->getNumber();

                if(!players.isEmpty()){
                    CardUseStruct worinimei;
                    Card *card;
                    if(word == "sdc" && slashtype < 5)
                        card = new ThunderSlash(Card::NoSuit, 0);
                    else if(word == "sdc" && slashtype >9)
                        card = new FireSlash(Card::NoSuit, 0);
                    else
                        card = new Slash(Card::NoSuit, 0);
                    card->setSkillName(objectName());
                    worinimei.card = card;
                    worinimei.from = miheng;
                    worinimei.to << target;
                    room->useCard(worinimei);
                }
            }
            else if(word == "ccsh"){
                room->sendLog(gitlog);
                //nimeiriwo:hp full
                room->setPlayerProperty(miheng, "hp", miheng->getMaxHP());
            }
            else if(word == "dsdc"){
                room->sendLog(gitlog);
                //nimeiriwo:show one player's handcard to other one
                ServerPlayer *source = room->askForPlayerChosen(miheng, room->getAlivePlayers(), objectName());
                ServerPlayer *target = room->askForPlayerChosen(miheng, room->getAlivePlayers(), objectName());

                LogMessage log;
                log.type = "#Info_dsdc";
                log.from = source;
                log.to << target;
                room->sendLog(log);

                room->showAllCards(target, source);
            }
            else if(word == "dshc"){
                room->sendLog(gitlog);
                //niriwomei:kill-self
                if(Config.FreeChoose && room->askForChoice(miheng, "numat", "kno+kyes") == "kno"){
                    gitlog.type = "#Numa_tequan";
                    gitlog.from = miheng;
                    room->sendLog(gitlog);
                }
                else{
                    room->killPlayer(miheng);
                }
            }
            else if(word == "dshcc"){
                room->sendLog(gitlog);
                //niriwomeimei:throw other 4 card and make 2 damage to self
                ServerPlayer *target = room->askForPlayerChosen(miheng, room->getAlivePlayers(), objectName());
                for(int i = 4; i > 0; i--)
                    room->throwCard(room->askForCardChosen(miheng, target, "he", objectName()));
                DamageStruct niriwomm;
                niriwomm.from = miheng;
                niriwomm.to = miheng;
                niriwomm.damage = 2;
                room->damage(niriwomm);
            }
            else if(word == "hsdcc" && miheng->getMark("hsdcc") == 0){
                room->sendLog(gitlog);
                //worinimeimei:Limited-Skill, like GreatYeyan
                ServerPlayer *target = room->askForPlayerChosen(miheng, room->getAlivePlayers(), objectName());
                DamageStruct worinimm;
                worinimm.from = miheng;
                worinimm.to = target;
                worinimm.nature = DamageStruct::Thunder;
                room->damage(worinimm);
                worinimm.nature = DamageStruct::Fire;
                room->damage(worinimm);
                worinimm.nature = DamageStruct::Normal;
                room->damage(worinimm);
                room->loseHp(miheng, 2);
                miheng->addMark("hsdcc");
            }
            else if(word == "dcshc" && miheng->getMark("dcshc") == 0){
                room->sendLog(gitlog);
                //worinimeimei:Limited-Skill, like Guixin
                room->loseHp(miheng);
                foreach(ServerPlayer *player, room->getAllPlayers()){
                    if(!player->isKongcheng()){
                        int card_id = room->askForCardChosen(miheng, player, "h", objectName());
                        if(room->getCardPlace(card_id) == Player::Hand)
                            room->moveCardTo(Sanguosha->getCard(card_id), miheng, Player::Hand, false);
                        else
                            room->obtainCard(miheng, card_id);
                    }
                }
                miheng->turnOver();
                miheng->addMark("dcshc");
            }
            else if(word == "ssdcc" && miheng->getMark("ssdcc") == 0){
                room->sendLog(gitlog);
                //ririnimeimei:lightning
                QList<ServerPlayer *> players;
                foreach(ServerPlayer *tmp, room->getAlivePlayers()){
                    foreach(const Card *lightning, tmp->getJudgingArea()){
                        if(lightning->objectName() == "lightning"){
                            players << tmp;
                            break;
                        }
                    }
                }
                if(!players.isEmpty()){
                    ServerPlayer *target = room->askForPlayerChosen(miheng, players, objectName());
                    foreach(const Card *lightning, target->getJudgingArea()){
                        if(lightning->objectName() == "lightning"){
                            room->throwCard(lightning->getId());
                            break;
                        }
                    }
                    DamageStruct damage;
                    damage.from = NULL;
                    damage.to = target;
                    damage.nature = DamageStruct::Thunder;
                    damage.damage = 3;
                    room->damage(damage);

                    miheng->addMark("ssdcc");
                }
            }
            else if(word == "ssscc" && miheng->getMark("ssscc") == 0){
                room->sendLog(gitlog);
                //riririmeimei:let single player acquire benghuai or wumou
                QList<ServerPlayer *> players;
                foreach(ServerPlayer *tmp, room->getOtherPlayers(miheng)){
                    if(tmp->getMaxHP() > miheng->getMaxHP())
                        players << tmp;
                }
                if(!players.isEmpty()){
                    ServerPlayer *target = room->askForPlayerChosen(miheng, players, objectName());
                    QString choice = room->askForChoice(target, objectName(), "bthx+wump");
                    if(choice == "bthx"){
                        room->setPlayerProperty(target, "maxhp", target->getMaxHP() + 2);
                        room->acquireSkill(target, "benghuai");
                    }
                    else{
                        room->setPlayerProperty(target, "maxhp", target->getMaxHP() + 1);
                        room->acquireSkill(target, "wumou");
                    }
                    miheng->addMark("ssscc");
                }
            }
            else if(word.length() == 4){
                gitlog.type = "#Numa_4wd";
                gitlog.from = miheng;
                room->sendLog(gitlog);
                //worinimeimei:Wake-Skill, lost all skills
                if(Config.FreeChoose && room->askForChoice(miheng, "numat", "suno+suyes") == "suno"){
                    gitlog.type = "#Numa_tequan";
                    gitlog.from = miheng;
                    room->sendLog(gitlog);
                }
                else{
                    QList<const Skill *> skills = miheng->getVisibleSkillList();
                    foreach(const Skill *skill, skills)
                        room->detachSkillFromPlayer(miheng, skill->objectName());
                    room->setPlayerProperty(miheng, "general", "sujiang");
                    room->setPlayerProperty(miheng, "general2", "sujiangf");
                    room->setPlayerProperty(miheng, "maxhp", miheng->getMaxHP() + 2);
                }
            }
            else if(word.length() == 5 && miheng->getMark("fivewd") == 0){
                gitlog.type = "#Numa_5wd";
                gitlog.from = miheng;
                room->sendLog(gitlog);
                //worinimeimei:Wake-Skill, learn longhun
                if(Config.FreeChoose && room->askForChoice(miheng, "numat", "lhno+lhyes") == "lhno"){
                    gitlog.type = "#Numa_tequan";
                    gitlog.from = miheng;
                    room->sendLog(gitlog);
                }
                else{
                    room->loseMaxHp(miheng);
                    if(miheng->isAlive()){
                        room->acquireSkill(miheng, "longhun");
                        miheng->addMark("fivewd");
                    }
                }
            }
            else if(word.length() > 5 && miheng->getMark("othwd") == 0){
                gitlog.type = "#Numa_wds";
                gitlog.from = miheng;
                room->sendLog(gitlog);
                //worinimeimei:Wake-Skill, learn wuyan and buqu
                room->loseMaxHp(miheng, 2);
                if(miheng->isAlive()){
                    room->acquireSkill(miheng, "wuyan");
                    room->acquireSkill(miheng, "buqu");
                    miheng->addMark("othwd");
                }
            }
            else{
                gitlog.type = "#Numa_git";
                gitlog.from = miheng;
                room->sendLog(gitlog);
            }
            foreach(int i, miheng->getPile("word"))
                room->throwCard(i);
        }
        return false;
    }
};

class Fanchun:public MasochismSkill{
public:
    Fanchun():MasochismSkill("fanchun"){
    }
    virtual void onDamaged(ServerPlayer *mh, const DamageStruct &damage) const{
        Room *room = mh->getRoom();
        const Card *card = damage.card;
        if(!room->obtainable(card, mh))
            return;
        QVariant data = QVariant::fromValue(card);
        if(room->askForSkillInvoke(mh, objectName(), data)){
            if(!card->getSubcards().isEmpty())
                foreach(int cd, card->getSubcards())
                    mh->addToPile("word", cd);
            else
                mh->addToPile("word", card->getId());
        }
    }
};

class Yuren: public TriggerSkill{
public:
    Yuren():TriggerSkill("yuren"){
        events << CardDiscarded << PhaseChange;
        frequency = Frequent;
    }

    virtual int getPriority() const{
        return -1;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        QList<ServerPlayer *> gentlemen;
        foreach(ServerPlayer *tmp, room->getAllPlayers()){
            if(tmp->getGeneral()->isMale())
                gentlemen << tmp;
        }
        if(event == PhaseChange){
            if(player->getPhase() != Player::Start)
                return false;
            if(player->getMark("@yuren") > 0)
                player->loseAllMarks("@yuren");
            if(!gentlemen.isEmpty())
                foreach(ServerPlayer *tmp, gentlemen)
                    if(tmp->getMark("@yuren") > 0)
                        tmp->loseAllMarks("@yuren");
        }
        else if(player->getPhase() == Player::Discard && !gentlemen.isEmpty()){
            QVariantList yuren_card = player->tag["Yuren"].toList();

            CardStar card = data.value<CardStar>();
            foreach(int card_id, card->getSubcards()){
                yuren_card << card_id;
            }
            player->tag["Yuren"] = yuren_card;

            if(!player->askForSkillInvoke(objectName(), data))
                return false;
            ServerPlayer *target = room->askForPlayerChosen(player, gentlemen, "yuren");
            foreach(QVariant card_data, player->tag["Yuren"].toList()){
                int card_id = card_data.toInt();
                if(room->getCardPlace(card_id) == Player::DiscardedPile)
                    room->obtainCard(target, card_id);
            }
            target->gainMark("@yuren");
            player->gainMark("@yuren");
        }
        return false;
    }
};

class YurenRov: public TriggerSkill{
public:
    YurenRov():TriggerSkill("#yuren_recover"){
        events << HpRecover;
    }
    virtual int getPriority() const{
        return -1;
    }
    virtual bool triggerable(const ServerPlayer *target) const{
        return target->getMark("@yuren") > 0 && !target->hasSkill("yuren");
    }
    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        RecoverStruct recover = data.value<RecoverStruct>();
        LogMessage log;
        log.type = "#Yuren_rov";
        log.from = player;
        foreach(ServerPlayer *target, room->getOtherPlayers(player)){
            if(target->getMark("@yuren") > 0)
                log.to << target;
        }
        log.arg = "yuren";
        room->sendLog(log);
        foreach(ServerPlayer *tmp, log.to)
            room->recover(tmp, recover, true);
        return false;
    }
};

class Zhenlie: public TriggerSkill{
public:
    Zhenlie():TriggerSkill("zhenlie"){
        events << Dying;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *girl, QVariant &data) const{
        DyingStruct dying_data = data.value<DyingStruct>();
        if(dying_data.who != girl)
            return false;
        Room *room = girl->getRoom();
        QList<ServerPlayer *> gentlemen;
        foreach(ServerPlayer *tmp, room->getAllPlayers()){
            if(tmp->getGeneral()->isMale())
                gentlemen << tmp;
        }
        if(!gentlemen.isEmpty() && girl->askForSkillInvoke(objectName(), data)){
            ServerPlayer *target = room->askForPlayerChosen(girl, gentlemen, objectName());
            QList<const Card *> cards = girl->getCards("he");
            if(cards.isEmpty())
                return false;
            foreach(const Card *tmp, cards){
                target->obtainCard(tmp);
            }
            ServerPlayer *player = girl->getNextAlive();
            room->setCurrent(target);
            room->killPlayer(girl);
            room->getThread()->trigger(TurnStart, target);
            room->setCurrent(player);
        }
        return false;
    }
};

class BaichuViewAsSkill:public OneCardViewAsSkill{
public:
    BaichuViewAsSkill():OneCardViewAsSkill("baichu"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return ! player->hasUsed("BaichuCard");
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        switch(ClientInstance->getStatus()){
        case Client::Playing:{
                return !to_select->isEquipped();
            }

        case Client::Responsing:{
                const Card *card = to_select->getFilteredCard();
                QString pattern = ClientInstance->getPattern();
                if(pattern == "jink")
                    return card->getNumber() >= Sanguosha->getCard(Self->getPile("ji").first())->getNumber();
            }

        default:
            return false;
        }
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return pattern == "jink";
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        bool isactive = Self->getPhase() != Player::NotActive
                        ? true :false;
        if(isactive){
            BaichuCard *card = new BaichuCard;
            card->addSubcard(card_item->getCard()->getId());
            return card;
        }
        else{
            const Card *card = card_item->getFilteredCard();
            Jink *jink = new Jink(card->getSuit(), card->getNumber());
            jink->addSubcard(card);
            jink->setSkillName(objectName());
            return jink;
        }
        return NULL;
    }
};

BaichuCard::BaichuCard(){
    once = true;
    target_fixed = true;
    will_throw = false;
}

void BaichuCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    room->obtainCard(source, source->getPile("ji").first());
    source->addToPile("ji", this->getSubcards().first());
}

class BaichuPattern: public CardPattern{
public:
    virtual bool match(const Player *player, const Card *card) const{
        return !player->hasEquip(card);
    }
    virtual bool willThrow() const{
        return false;
    }
};

class Baichu: public TriggerSkill{
public:
    Baichu():TriggerSkill("baichu"){
        events << TurnStart << PhaseChange << CardUsed;
        view_as_skill = new BaichuViewAsSkill;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        ServerPlayer *xunyou = room->findPlayerBySkillName("baichu");
        if(!xunyou)
            return false;
        if(event == PhaseChange && player->getPhase() == Player::Finish){
            xunyou->setMark("bc_count", 0);
        }
        if(event == TurnStart && xunyou->getPile("ji").isEmpty()){
            xunyou->drawCards(1);
            const Card *card = room->askForCard(xunyou, ".baichu!", "@baichu", data);
            if(card)
                xunyou->addToPile("ji", card->getId());
            else
                xunyou->addToPile("ji", xunyou->getHandcards().last()->getId());
        }
        if(player != xunyou)
            return false;
        const Card *card = NULL;
        if(xunyou->getPhase() != Player::NotActive){
            if(event == CardUsed){
                CardUseStruct use = data.value<CardUseStruct>();
                if(use.card->getSubtype() != "skill_card")
                    card = use.card;
            }
            if(xunyou->getMark("bc_count") < 5 &&
               card && card->getNumber() < Sanguosha->getCard(xunyou->getPile("ji").first())->getNumber()
                && room->askForSkillInvoke(xunyou, "baichu", data)){
                    xunyou->drawCards(1);
                    xunyou->addMark("bc_count");
                }
        }
        return false;
    }
};

class Nongquan: public TriggerSkill{
public:
    Nongquan():TriggerSkill("nongquan"){
        events << Dying;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *sb, QVariant &data) const{
        DyingStruct dying_data = data.value<DyingStruct>();
        if(dying_data.who != sb || sb->getMark("@quan") < 1)
            return false;

        Room *room = sb->getRoom();
        if(sb->askForSkillInvoke("nongquan", data)){
            sb->loseMark("@quan");
            room->setPlayerProperty(sb, "hp", 1);
            sb->drawCards(3);
            if(!sb->faceUp())
                sb->turnOver();
            if(sb->isChained()){
                room->setPlayerProperty(sb, "chained", false);
            }
        }
        return false;
    }
};

class Beigong: public PhaseChangeSkill{
public:
    Beigong():PhaseChangeSkill("beigong"){
        frequency = Compulsory;
    }

    virtual int getPriority() const{
        return 3;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return PhaseChangeSkill::triggerable(target) &&
                (target->getPhase() == Player::Draw ||
                 target->getPhase() == Player::Discard);
    }

    virtual bool onPhaseChange(ServerPlayer *) const{
        return true;
    }
};

class Yinfu:public TriggerSkill{
public:
    Yinfu():TriggerSkill("yinfu"){
        events << CardFinished;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *ten, QVariant &data) const{
        CardUseStruct use = data.value<CardUseStruct>();
        const Card *card = use.card;
        if(ten->getMark("yinfu") >= 2)
            return false;
        if(card->inherits("ArcheryAttack") || card->inherits("GodSalvation")){
            LogMessage log;
            log.type = "#Yinfu";
            log.from = ten;
            log.arg2 = objectName();
            log.arg = QString::number(1 - ten->getMark("yinfu"));
            ten->getRoom()->sendLog(log);

            ten->gainMark("@quan");
            ten->addMark("yinfu");
        }
        return false;
    }
};

class Jielue: public TriggerSkill{
public:
    Jielue():TriggerSkill("jielue"){
        events << SlashEffect << Pindian;
        frequency = Frequent;
    }

    virtual int getPriority() const{
        return -1;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *player, QVariant &data) const{
        if(event == Pindian){
            PindianStar pindian = data.value<PindianStar>();
            if(pindian->reason == objectName() && pindian->isSuccess())
                pindian->from->obtainCard(pindian->to_card);
            return false;
        }
        SlashEffectStruct effect = data.value<SlashEffectStruct>();
        if(effect.slash->getNumber() == 0)
            return false;
        if(effect.slash && !effect.to->isKongcheng() && effect.from->askForSkillInvoke(objectName(), data))
            effect.from->pindian(effect.to, objectName(), effect.slash);
        return false;
    }
};

NewbilityGeneralPackage::NewbilityGeneralPackage()
    :Package("newbility_general")
{

    General *shixie = new General(this, "shixie", "qun");
    shixie->addSkill(new Wutian);
    shixie->addSkill(new Wuzheng);

    General *kanze = new General(this, "kanze", "wu");
    kanze->addSkill(new Leiluo);
    kanze->addSkill(new Diezhi);
    kanze->addSkill(new MarkAssignSkill("@drig", 1));
    related_skills.insertMulti("diezhi", "#@drig");

    General *yanpeng = new General(this, "yanpeng", "shu");
    yanpeng->addSkill(new Yabian);

    General *kongrong = new General(this, "kongrong", "qun");
    kongrong->addSkill(new Rangli);

    General *miheng = new General(this, "miheng", "god", 3);
    miheng->addSkill(new Yulu);
    miheng->addSkill(new Numa);
    miheng->addSkill(new Jieao);

    General *xunyou = new General(this, "xunyou", "wei", 3);
    xunyou->addSkill(new Baichu);
    patterns.insert(".baichu!", new BaichuPattern);

    General *ganmi = new General(this, "ganmi", "shu", 4, false);
    ganmi->addSkill(new Yuren);
    ganmi->addSkill(new YurenRov);
    ganmi->addSkill(new Zhenlie);
    related_skills.insertMulti("yuren", "#yuren_recover");

    General *tensb = new General(this, "tensb", "god", 1);
    tensb->addSkill(new Nongquan);
    tensb->addSkill(new MarkAssignSkill("@quan", 10));
    related_skills.insertMulti("nongquan", "#@quan");
    tensb->addSkill(new Beigong);
    tensb->addSkill(new Yinfu);

    General *yanbh = new General(this, "yanbh", "wu", 4);
    yanbh->addSkill(new Jielue);

    skills << new Fanchun;
    addMetaObject<DiezhiCard>();
    addMetaObject<WutianCard>();
    addMetaObject<YuluCard>();
    addMetaObject<ViewMyWordsCard>();
    addMetaObject<BaichuCard>();

    type = GeneralPack;
}

ADD_PACKAGE(NewbilityGeneral)

NewbilityPackage::NewbilityPackage()
    :Package("newbility")
{

    QList<Card *> cards;
    cards
            << new JuicePeach(Card::Diamond, 13)
            << new ClearShirt(Card::Spade, 3)
            << new Sacrifice(Card::Heart, 5)
            ;
    cards
            << new Wall(Card::Heart, 12)
            << new Wall(Card::Club, 12)
            << new Wall(Card::Diamond, 12)
            << new Wall(Card::Spade, 12);
    cards
            << new Stink(Card::Heart, 1)
            << new Poison(Card::Heart, 11)
            << new Poison(Card::Club, 11)
            << new Peach(Card::Spade, 4)
            << new Poison(Card::Diamond, 11)
            << new Poison(Card::Spade, 11);

    skills
            << new WallSkill
            << new PoisonSkill;
//armors
    QList<Card *> newbys;
    newbys
            << new Niubi(Card::Spade, 2)
            << new Niubi(Card::Spade, 1)
            << new Niubi(Card::Heart, 7)
            << new Niubi(Card::Heart, 6)
            << new Niubi(Card::Heart, 4)
            << new Niubi(Card::Spade, 4)
            << new Niubi(Card::Spade, 5)
            << new Niubi(Card::Spade, 6)
            << new Niubi(Card::Diamond, 3)
            << new Niubi(Card::Spade, 3)
            << new Niubi(Card::Diamond, 6)
            << new Niubi(Card::Diamond, 4)
            << new Niubi(Card::Heart, 1)
            << new Niubi(Card::Diamond, 2)
            << new Niubi(Card::Heart, 2)
            << new Niubi(Card::Diamond, 1)
            << new Niubi(Card::Spade, 7)
            << new Niubi(Card::Club, 3)
            << new Niubi(Card::Heart, 5)
            << new Niubi(Card::Heart, 3)
            << new Niubi(Card::Diamond, 5)
            << new Niubi(Card::Diamond, 7)
            << new Niubi(Card::Club, 2)
            << new Niubi(Card::Club, 1)
            << new Niubi(Card::Diamond, 8)
//wind
            << new Niubi(Card::Club, 4)
            << new Niubi(Card::Heart, 8)
            << new Niubi(Card::Spade, 8)
            << new Niubi(Card::Club, 5)
            << new Niubi(Card::Diamond, 9)
            << new Niubi(Card::Spade, 9)
            << new Niubi(Card::Heart, 9)
            << new Niubi(Card::Diamond, 10)
//sp
            << new Niubi(Card::Club, 6)
            << new Niubi(Card::Club, 7)
//1j
            << new Niubi(Card::Spade, 2)
            << new Niubi(Card::Club, 3)
            << new Niubi(Card::Diamond, 5)
            << new Niubi(Card::Club, 7)
            << new Niubi(Card::Spade, 10)
            << new Niubi(Card::Heart, 8)
            << new Niubi(Card::Heart, 4)
            << new Niubi(Card::Club, 11)
            << new Niubi(Card::Spade, 6)
            << new Niubi(Card::Diamond, 9)
            << new Niubi(Card::Heart, 12)
//thicket
            << new Niubi(Card::Spade, 12)
            << new Niubi(Card::Spade, 13)
            << new Niubi(Card::Diamond, 12)
            << new Niubi(Card::Diamond, 13)
            << new Niubi(Card::Club, 11)
            << new Niubi(Card::Club, 12)
            << new Niubi(Card::Heart, 12)
            << new Niubi(Card::Heart, 13)
//fire
            << new Niubi(Card::Heart, 10)
            << new Niubi(Card::Heart, 11)
            << new Niubi(Card::Diamond, 11)
            << new Niubi(Card::Club, 8)
            << new Niubi(Card::Club, 9)
            << new Niubi(Card::Spade, 10)
            << new Niubi(Card::Spade, 11)
            << new Niubi(Card::Club, 10)
//sp
            << new Niubi(Card::Club, 13)
//mountain
            << new Niubi(Card::Diamond, 13)
            << new Niubi(Card::Heart, 1)
            << new Niubi(Card::Spade, 4)
            << new Niubi(Card::Club, 2)
            << new Niubi(Card::Club, 9)
            << new Niubi(Card::Diamond, 2)
            << new Niubi(Card::Heart, 10)
            << new Niubi(Card::Spade, 8);

    newbys.at(0)->setObjectName("corrfluid");
    newbys.at(1)->setObjectName("stimulant");
    newbys.at(2)->setObjectName("madamfeng");
    newbys.at(3)->setObjectName("harley");
    newbys.at(4)->setObjectName("telescope");
    newbys.at(5)->setObjectName("flashlight");
    newbys.at(6)->setObjectName("warmbaby");
    newbys.at(7)->setObjectName("linctus");
    newbys.at(8)->setObjectName("towel");
    newbys.at(9)->setObjectName("lubricatingoil");
    newbys.at(10)->setObjectName("underwear");
    newbys.at(11)->setObjectName("whip");
    newbys.at(12)->setObjectName("eyedrops");
    newbys.at(13)->setObjectName("urban");
    newbys.at(14)->setObjectName("redsunglasses");
    newbys.at(15)->setObjectName("brainplatinum");
    newbys.at(16)->setObjectName("sophie");
    newbys.at(17)->setObjectName("yaiba");
    newbys.at(18)->setObjectName("banana");
    newbys.at(19)->setObjectName("speakers");
    newbys.at(20)->setObjectName("cologne");
    newbys.at(21)->setObjectName("dustbin");
    newbys.at(22)->setObjectName("animals");
    newbys.at(23)->setObjectName("deathrisk");
    newbys.at(24)->setObjectName("rollingpin");
//wind
    newbys.at(25)->setObjectName("saw");
    newbys.at(26)->setObjectName("amazonston");
    newbys.at(27)->setObjectName("gnat");
    newbys.at(28)->setObjectName("magicwand");
    newbys.at(29)->setObjectName("chanel5");
    newbys.at(30)->setObjectName("landrover");
    newbys.at(31)->setObjectName("chiropter");
    newbys.at(32)->setObjectName("drum");
//sp
    newbys.at(33)->setObjectName("morin_khuur");
    newbys.at(34)->setObjectName("greatchair");
//1j
    newbys.at(35)->setObjectName("hawksbill");
    newbys.at(36)->setObjectName("torture");
    newbys.at(37)->setObjectName("rotate");
    newbys.at(38)->setObjectName("totocar");
    newbys.at(39)->setObjectName("ch3oh");
    newbys.at(40)->setObjectName("teardan");
    newbys.at(41)->setObjectName("nanafist");
    newbys.at(42)->setObjectName("lrzt9hh");
    newbys.at(43)->setObjectName("mushroom");
    newbys.at(44)->setObjectName("aofrog");
    newbys.at(45)->setObjectName("coptis");
//thicket
    newbys.at(46)->setObjectName("hydrogen");
    newbys.at(47)->setObjectName("tranqgun");
    newbys.at(48)->setObjectName("ghostcar");
    newbys.at(49)->setObjectName("snake");
    newbys.at(50)->setObjectName("voodoo");
    newbys.at(51)->setObjectName("tombstone");
    newbys.at(52)->setObjectName("snapshot");
    newbys.at(53)->setObjectName("fuckav");
//fire
    newbys.at(54)->setObjectName("switchbd");
    newbys.at(55)->setObjectName("reijyutsu");
    newbys.at(56)->setObjectName("apple");
    newbys.at(57)->setObjectName("goldlock");
    newbys.at(58)->setObjectName("tanbi");
    newbys.at(59)->setObjectName("sansyouuo");
    newbys.at(60)->setObjectName("flack");
    newbys.at(61)->setObjectName("coffinlid");
//spyx
    newbys.at(62)->setObjectName("nailclippers");
//mountain
    newbys.at(63)->setObjectName("nunchaku");
    newbys.at(64)->setObjectName("ruan");
    newbys.at(65)->setObjectName("globe");
    newbys.at(66)->setObjectName("wookon");
    newbys.at(67)->setObjectName("greatmug");
    newbys.at(68)->setObjectName("goooo");
    newbys.at(69)->setObjectName("hundan");
    newbys.at(70)->setObjectName("tizanidine");

    cards << newbys;

    foreach(Card *card, cards)
        card->setParent(this);

    type = CardPack;

    skills
            << new EyedropsSkill //liubei
            << new RedsunglassesSkill //guanyu
            << new SpeakerSkill //zhangfei
            << new BananaSkill //zhaoyun
            << new FengjieSkill //huangyueying
            << new DustbinSkill //luxun
            << new AnimalsSkill //lvbu
            << new RollingpinSkill //sunshuishui
            << new DeathriSkill //huatuo
            << new AmazonstonSkill //huangzhong
            << new GnatSkill //caoren
            << new TranqgunSkill //xuhuang
            << new TombstoneSkill //dongzhuo
            << new SwitchbdSkill //wolong
            << new GoldlockSkill //yuanshao
            << new AppleSkill //taishici
            << new WookonSkill //dengai
            ;

    addMetaObject<RollingpinCard>();
    addMetaObject<WookonCard>();
}

ADD_PACKAGE(Newbility);

