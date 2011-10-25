#include "kuso-package.h"
#include "skill.h"
#include "client.h"
#include "engine.h"
#include "carditem.h"
#include "room.h"

class Huaxu: public TriggerSkill{
public:
    Huaxu():TriggerSkill("huaxu"){
        events << Damage << TurnStart;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *player, QVariant &data) const{
        if(event == TurnStart){
            foreach(ServerPlayer *tmp, player->getRoom()->getAlivePlayers()){
                for(int i = 1; i <= 4; i++){
                    QString mark_name = "@hana" + QString::number(i);
                    if(tmp->getMark(mark_name) > 0)
                        tmp->loseAllMarks(mark_name);
                }
            }
            return false;
        }
        if(player->getPhase() == Player::NotActive)
            return false;
        DamageStruct damage = data.value<DamageStruct>();
        if(!damage.card || damage.card->getSuit() == Card::NoSuit)
            return false;
        Room *room = player->getRoom();
        if(damage.to->isDead())
            return false;
        if(player->askForSkillInvoke(objectName(), data)){
            int i = 0;
            switch(damage.card->getSuit()){
            case Card::Heart : i = 1; break;
            case Card::Diamond : i = 2; break;
            case Card::Spade : i = 3; break;
            case Card::Club : i = 4; break;
            default: break;
            }
            QString mark_name = "@hana" + QString::number(i);
            if(damage.to->getMark(mark_name) < 1)
                damage.to->gainMark(mark_name);
            LogMessage log;
            log.type = "#Gotoshit";
            log.from = player;
            log.to << damage.to;
            log.arg = Card::Suit2String(damage.card->getSuit());
            room->sendLog(log);
        }
        return false;
    }
};

class HuaxuEffect:public TriggerSkill{
public:
    HuaxuEffect():TriggerSkill("#huaxu_eft"){
        events << CardUsed;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        for(int i = 1; i <= 4; i++){
            QString mark_name = "@hana" + QString::number(i);
            if(target->getMark(mark_name) > 0)
                return true;
        }
        return false;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        if(player->getPhase() == Player::NotActive)
            return false;
        CardUseStruct use = data.value<CardUseStruct>();
        CardStar card = use.card;
        Card::Suit suit = card->getSuit();
        Room *room = player->getRoom();
        if(card && suit != Card::NoSuit){
            LogMessage log;
            log.from = player;
            log.card_str = card->getEffectIdString();
            if(suit == Card::Heart && player->getMark("@hana1") > 0){
                log.type = "$HuaxuDamage1";
                room->sendLog(log);
                DamageStruct damage;
                damage.from = damage.to = player;
                damage.card = card;
                //damage.damage = player->getMark("@hana1");
                damage.nature = DamageStruct::Fire;
                room->throwCard(card);
                room->damage(damage);
            }
            else if(suit == Card::Diamond && player->getMark("@hana2") > 0){
                log.type = "$HuaxuDamage2";
                room->sendLog(log);
                DamageStruct damage;
                damage.from = damage.to = player;
                damage.card = card;
                //damage.damage = player->getMark("@hana2");
                room->throwCard(card);
                room->damage(damage);
            }
            else if(suit == Card::Spade && player->getMark("@hana3") > 0){
                log.type = "$HuaxuDamage3";
                room->sendLog(log);
                //room->loseHp(player, player->getMark("@hana3"));
                room->throwCard(card);
                room->loseHp(player);
            }
            else if(suit == Card::Club && player->getMark("@hana4") > 0){
                log.type = "$HuaxuDamage4";
                room->sendLog(log);
                DamageStruct damage;
                damage.from = damage.to = player;
                damage.card = card;
                //damage.damage = player->getMark("@hana4");
                damage.nature = DamageStruct::Thunder;
                room->throwCard(card);
                room->damage(damage);
            }
            if(player->isDead())
                return true;
        }
        return false;
    }
};

class Liaoting:public ViewAsSkill{
public:
    Liaoting():ViewAsSkill("liaoting"){
        frequency = Limited;
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return player->getMark("liaot") >= 1;
    }

    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        if(selected.length() > 4)
            return false;
        return !to_select->isEquipped();
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.length() != 4)
            return NULL;

        LiaotingCard *card = new LiaotingCard;
        card->addSubcards(cards);

        return card;
    }
};

LiaotingCard::LiaotingCard(){
    target_fixed = true;
}

void LiaotingCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &) const{
    QList<int> subs = this->getSubcards();
    source->loseMark("liaot");
    foreach(int tmp, subs){
        if(!Sanguosha->getCard(tmp)->inherits("Shit")){
            room->throwCard(this);
            return;
        }
    }
    if(source->isLord() || source->getRole() == "loyalist"){
        foreach(ServerPlayer *tmp, room->getAlivePlayers())
            if(tmp->getRole() == "renegade" || tmp->getRole() == "rebel")
                room->killPlayer(tmp);
    }
    else if(source->getRole() == "renegade"){
        source->drawCards(5, false);
        foreach(ServerPlayer *tmp, room->getAlivePlayers())
            if(tmp->getRole() == "loyalist" || tmp->getRole() == "rebel")
                room->killPlayer(tmp);
    }
    else if(source->getRole() == "rebel")
        room->killPlayer(room->getLord());

    room->throwCard(this);
    if(source->isAlive())
        room->setPlayerProperty(source, "hp", source->getMaxHP());
}

class Skydao:public MasochismSkill{
public:
    Skydao():MasochismSkill("skydao"){
        frequency = Compulsory;
    }

    virtual void onDamaged(ServerPlayer *player, const DamageStruct &damage) const{
        Room *room = player->getRoom();
        if(damage.to && damage.to == player && player->getPhase() == Player::NotActive){
            LogMessage log;
            log.from = player;
            log.type = "#SkydaoMAXHP";
            log.arg2 = objectName();
            room->setPlayerProperty(player, "maxhp", player->getMaxHP() + 1);
            log.arg = QString::number(player->getMaxHP());
            room->sendLog(log);
        }
    }
};

class Noqing:public MasochismSkill{
public:
    Noqing():MasochismSkill("noqing"){
        frequency = Compulsory;
    }

    virtual int getPriority() const{
        return -1;
    }

    virtual QString getDefaultChoice(ServerPlayer *player) const{
        if(player->getMaxHP() >= player->getHp() + 2)
            return "maxhp";
        else
            return "hp";
    }

    virtual void onDamaged(ServerPlayer *player, const DamageStruct &damage) const{
        Room *room = player->getRoom();
        if(damage.to && damage.to == player){
            foreach(ServerPlayer *tmp, room->getOtherPlayers(player))
                if(tmp->getHp() < player->getHp())
                    return;
            foreach(ServerPlayer *tmp, room->getAllPlayers()){
                QString choice = room->askForChoice(tmp, objectName(), "hp+max_hp");
                LogMessage log;
                log.from = player;
                log.to << tmp;
                log.arg = objectName();
                if(choice == "hp"){
                    log.type = "#NoqingLoseHp";
                    room->sendLog(log);
                    room->loseHp(tmp);
                }else{
                    log.type = "#NoqingLoseMaxHp";
                    room->sendLog(log);
                    room->loseMaxHp(tmp);
                }
            }
        }
    }
};

class Shishi: public TriggerSkill{
public:
    Shishi():TriggerSkill("shishi"){
        events << Death;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        ServerPlayer *tenkei = room->findPlayerBySkillName(objectName());
        if(!tenkei)
            return false;
        QVariantList shishi_skills = tenkei->tag["Shishi"].toList();
        if(room->askForSkillInvoke(tenkei, objectName(), data)){
            QStringList shishis;
            foreach(QVariant tmp, shishi_skills)
                shishis << tmp.toString();
            if(!shishis.isEmpty()){
                QString choice = room->askForChoice(tenkei, objectName(), shishis.join("+"));
                room->detachSkillFromPlayer(tenkei, choice);
                shishi_skills.removeOne(choice);
            }
            room->loseMaxHp(tenkei);
            QList<const Skill *> skills = player->getVisibleSkillList();
            foreach(const Skill *skill, skills){
                if(skill->parent()){
                    QString sk = skill->objectName();
                    room->acquireSkill(tenkei, sk);
                    shishi_skills << sk;
                }
            }
            tenkei->tag["Shishi"] = shishi_skills;
        }

        return false;
    }
};

KusoPackage::KusoPackage()
    :Package("kuso")
{
    General *kusoking = new General(this, "kusoking", "god", 4, false);
    kusoking->addSkill(new Huaxu);
    kusoking->addSkill(new HuaxuEffect);
    related_skills.insertMulti("huaxu", "#huaxu_eft");
    kusoking->addSkill(new Liaoting);
    kusoking->addSkill(new MarkAssignSkill("liaot", 1));
    related_skills.insertMulti("liaoting", "#liaot");

    General *tianyin = new General(this, "tianyin", "god", 5, false);
    tianyin->addSkill(new Skydao);
    tianyin->addSkill(new Noqing);

    General *tenkei = new General(this, "tenkei", "god", 5, false);
    tenkei->addSkill(new Shishi);

    addMetaObject<LiaotingCard>();
}

//cards

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

class KawaiiDressSkill: public ArmorSkill{
public:
    KawaiiDressSkill():ArmorSkill("kawaii_dress"){
        events << Predamaged;
    }

    virtual int getPriority() const{
        return 2;
    }

    virtual bool trigger(TriggerEvent, ServerPlayer *player, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        if(player && player == damage.to && player->getHp() == 1){
            LogMessage log;
            log.type = "#KawaiiDressProtect";
            log.from = player;
            log.arg = QString::number(damage.damage);
            if(damage.nature == DamageStruct::Normal)
                log.arg2 = "normal_nature";
            else if(damage.nature == DamageStruct::Fire)
                log.arg2 = "fire_nature";
            else
                log.arg2 = "thunder_nature";
            player->getRoom()->sendLog(log);

            return true;
        }
        return false;
    }
};

KawaiiDress::KawaiiDress(Suit suit, int number) :Armor(suit, number){
    setObjectName("kawaii_dress");
    skill = new KawaiiDressSkill;
}

void KawaiiDress::onInstall(ServerPlayer *player) const{
    EquipCard::onInstall(player);
    if(!player->getGeneral()->isFemale()){
        LogMessage log;
        Room *room = player->getRoom();
        log.from = player;
        log.arg = objectName();
        log.arg2 = "dongzhuo";
        if(player->hasSkill("jiuchi") && player->getHp() <= 2){
            log.type = "#KawaiiAngry";
            room->sendLog(log);
            room->killPlayer(player);
        }
        else{
            log.type = "#KawaiiHurt";
            room->sendLog(log);
            room->loseHp(player);
        }
    }
}

void KawaiiDress::onUninstall(ServerPlayer *player) const{
    if(player->isAlive() && player->getMark("qinggang") == 0){
        player->drawCards(2);
    }
}

class FivelineSkill: public ArmorSkill{
public:
    FivelineSkill():ArmorSkill("fiveline"){
        events << HpChanged;
    }

    virtual int getPriority() const{
        return -1;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &) const{
        int hp = player->getHp();
        if(player->isDead() || hp < 1)
            return false;
        Room *room = player->getRoom();
        QStringList skills;
        skills << "rende" << "jizhi" << "jieyin" << "guose" << "kurou";
        QVariantList has_skills = player->tag["fiveline"].toList();
        foreach(QString str, skills){
            if(has_skills.contains(str))
                continue;
            else{
                room->detachSkillFromPlayer(player, str);
                player->loseSkill(str);
            }
        }
        if(hp <= 5)
            room->acquireSkill(player, skills.at(hp - 1));

        return false;
    }
};

Fiveline::Fiveline(Suit suit, int number) :Armor(suit, number){
    setObjectName("fiveline");
    skill = new FivelineSkill;
}

void Fiveline::onInstall(ServerPlayer *player) const{
    EquipCard::onInstall(player);
    QVariantList skills;
    QStringList fiveskill;
    fiveskill << "rende" << "jizhi" << "jieyin" << "guose" << "kurou";
    foreach(QString str, fiveskill){
        if(player->hasSkill(str))
            skills << str;
    }
    player->tag["fiveline"] = skills;
    player->getRoom()->setPlayerProperty(player, "hp", player->getHp());
}

void Fiveline::onUninstall(ServerPlayer *player) const{
    if(player->isDead())
        return;
    QStringList skills;
    skills << "rende" << "jizhi" << "jieyin" << "guose" << "kurou";
    QVariantList has_skills = player->tag["fiveline"].toList();
    foreach(QString str, skills){
        if(has_skills.contains(str))
            continue;
        else{
            player->getRoom()->detachSkillFromPlayer(player, str);
            player->loseSkill(str);
        }
    }
}

KusoCardPackage::KusoCardPackage()
    :Package("kuso_cards")
{
    (new Sacrifice(Card::Diamond, 7))->setParent(this);
    (new ClearShirt(Card::Club, 3))->setParent(this);
    (new KawaiiDress(Card::Spade, 2))->setParent(this);
    (new Fiveline(Card::Heart, 5))->setParent(this);

    type = CardPack;
}

ADD_PACKAGE(Kuso)
ADD_PACKAGE(KusoCard)