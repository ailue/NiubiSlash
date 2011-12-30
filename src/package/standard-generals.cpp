#include "general.h"
#include "standard.h"
#include "skill.h"
#include "engine.h"
#include "client.h"
#include "carditem.h"
#include "serverplayer.h"
#include "room.h"
#include "standard-skillcards.h"

class Zhizhi: public TriggerSkill{
public:
    Zhizhi():TriggerSkill("zhizhi"){
        events << SlashProceed;
        frequency = Compulsory;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();
        Room *room = player->getRoom();
        room->invokeSkill(player, objectName());

        QString slasher = player->objectName();
        const Card *first_jink = NULL, *second_jink = NULL;
        first_jink = room->askForCard(effect.to, "jink", "@wushuang-jink-1:" + slasher);
        if(first_jink){
            if(first_jink->objectName() == "ingenarg")
                effect.to->drawCards(1);
            second_jink = room->askForCard(effect.to, "jink", "@wushuang-jink-2:" + slasher);
        }

        Card *jink = NULL;
        if(first_jink && second_jink){
            if(second_jink->objectName() == "ingenarg")
                effect.to->drawCards(1);
            jink = new DummyCard;
            jink->addSubcard(first_jink);
            jink->addSubcard(second_jink);
        }
        room->slashResult(effect, jink);

        return true;
    }
};

class Shensi:public DrawCardsSkill{
public:
    Shensi():DrawCardsSkill("shensi"){
        frequency = Compulsory;
    }

    virtual int getDrawNum(ServerPlayer *player, int) const{
        int ext = player->property("draw3").toBool() ? 1 : 0;
        player->getRoom()->invokeSkill(player, objectName());
        return 4 + ext;
    }
};

class Lingxi: public TriggerSkill{
public:
    Lingxi():TriggerSkill("lingxi"){
        events << CardLost;
        frequency = Compulsory;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        if(player->isKongcheng()){
            CardMoveStar move = data.value<CardMoveStar>();
            if(move->from_place == Player::Hand){
                Room *room = player->getRoom();
                room->invokeSkill(player, objectName());
                player->drawCards(1);
            }
        }
        return false;
    }
};

class Jizhi:public DrawCardsSkill{
public:
    Jizhi():DrawCardsSkill("jizhi"){
        frequency = Compulsory;
    }

    virtual int getDrawNum(ServerPlayer *heiji, int n) const{
        int a = heiji->getLostHp();
        if(a > 0)
            heiji->getRoom()->invokeSkill(heiji, objectName());
        return n + a;
    }
};

class Kaituo:public ViewAsSkill{
public:
    Kaituo():ViewAsSkill("kaituo"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return player->isWounded() && player->getHandcardNum() > 1;
    }

    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        if(selected.length() > 2)
            return false;
        return !to_select->isEquipped();
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.length() != 2)
            return NULL;
        KaituoCard *ard = new KaituoCard;
        ard->addSubcards(cards);

        return ard;
    }
};

class Boxue: public PhaseChangeSkill{
public:
    Boxue():PhaseChangeSkill("boxue"){
        frequency = Compulsory;
    }

    virtual bool onPhaseChange(ServerPlayer *player) const{
        if(player->getPhase() == Player::NotActive){
            int x = player->getHp() - player->getHandcardNum();
            if(x > 0){
                player->getRoom()->invokeSkill(player, objectName());
                player->drawCards(x);
            }
        }
        return false;
    }
};

class Tongqing: public TriggerSkill{
public:
    Tongqing():TriggerSkill("tongqing"){
        events << Dying;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        ServerPlayer *ayumi = room->findPlayerBySkillName("tongqing");
        if(!ayumi)
            return false;
        DyingStruct dying = data.value<DyingStruct>();
        const Card *card = room->askForCard(ayumi, "jink", "@tongqing:" + dying.who->objectName());
        if(card){
            ayumi->addMark("tongqing");
            Peach *peach = new Peach(card->getSuit(), card->getNumber());
            peach->addSubcard(card->getEffectiveId());
            peach->setSkillName(objectName());
            CardUseStruct use;
            use.card = peach;
            use.from = ayumi;
            use.to << dying.who;
            room->useCard(use);

            if(ayumi->getMark("tongqing") > 0)
                ayumi->drawCards(qMin(room->getAlivePlayers().length(), ayumi->getMark("tongqing")));
        }
        return false;
    }
};

class Weiya: public TriggerSkill{
public:
    Weiya():TriggerSkill("weiya"){
        events << Damage;
        frequency = Compulsory;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        if(damage.to && !damage.to->isKongcheng()){
            player->getRoom()->invokeSkill(player, objectName());
            player->getRoom()->throwCard(damage.to->getRandomHandCardId());
        }
        return false;
    }
};

class Tiemian:public MasochismSkill{
public:
    Tiemian():MasochismSkill("tiemian"){
    }

    virtual void onDamaged(ServerPlayer *player, const DamageStruct &damage) const{
        Room *room = player->getRoom();
        if(damage.from && damage.to && player->getLostHp() > damage.from->getLostHp() &&
           room->askForDiscard(damage.to, objectName(), 1)){
            DamageStruct dmg;
            dmg.from = player;
            dmg.to = damage.from;
            room->invokeSkill(player, objectName(), false);
            room->damage(dmg);
        }
    }
};

class Checha: public TriggerSkill{
public:
    Checha():TriggerSkill("checha"){
        events << Death;
        frequency = Compulsory;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return !target->hasSkill(objectName());
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        ServerPlayer *miwako = player->getRoom()->findPlayerBySkillName(objectName());
        if(miwako){
            miwako->getRoom()->invokeSkill(miwako, objectName());
            miwako->drawCards(2);
        }
        return false;
    }
};

class Fanjie:public MasochismSkill{
public:
    Fanjie():MasochismSkill("fanjie"){
        frequency = Compulsory;
    }

    virtual void onDamaged(ServerPlayer *player, const DamageStruct &damage) const{
        Room *room = player->getRoom();
        const Card *card = damage.card;
        if(!room->obtainable(card, player))
            return;
        room->invokeSkill(player, objectName());
        player->obtainCard(card);
    }
};

class Gouxian:public OneCardViewAsSkill{
public:
    Gouxian():OneCardViewAsSkill("gouxian"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return Slash::IsAvailable(player);
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return pattern == "slash";
    }

    virtual bool viewFilter(const CardItem *) const{
        return true;
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        const Card *card = card_item->getCard();
        Card *slash = new Slash(card->getSuit(), card->getNumber());
        slash->addSubcard(card->getId());
        slash->setSkillName(objectName());
        return slash;
    }
};

class Shexian: public ZeroCardViewAsSkill{
public:
    Shexian():ZeroCardViewAsSkill("shexian"){

    }
    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->hasUsed("ShexianCard");
    }
    virtual const Card *viewAs() const{
        return new ShexianCard;
    }
};

class Mp1: public ZeroCardViewAsSkill{
public:
    Mp1():ZeroCardViewAsSkill("mp1"){
    }
    virtual bool isEnabledAtPlay(const Player *player) const{
        return player->containsTrick("microphone");
    }
    virtual const Card *viewAs() const{
        return new Mp1Card;
    }
};

class Mp2: public OneCardViewAsSkill{
public:
    Mp2():OneCardViewAsSkill("mp2"){
    }
    virtual bool isEnabledAtPlay(const Player *player) const{
        return player->containsTrick("microphone");
    }
    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getCard()->inherits("Slash");
    }
    virtual const Card *viewAs(CardItem *card_item) const{
        Mp2Card *card = new Mp2Card;
        card->addSubcard(card_item->getCard()->getId());
        return card;
    }
};

class Mp3: public ZeroCardViewAsSkill{
public:
    Mp3():ZeroCardViewAsSkill("mp3"){
    }
    virtual bool isEnabledAtPlay(const Player *player) const{
        return player->containsTrick("microphone");
    }
    virtual const Card *viewAs() const{
        return new Mp3Card;
    }
};

class Mp4: public OneCardViewAsSkill{
public:
    Mp4():OneCardViewAsSkill("mp4"){
    }
    virtual bool isEnabledAtPlay(const Player *player) const{
        return player->containsTrick("microphone") && player->isWounded();
    }
    virtual bool viewFilter(const CardItem *to_select) const{
        return true;
    }
    virtual const Card *viewAs(CardItem *card_item) const{
        Mp4Card *card = new Mp4Card;
        card->addSubcard(card_item->getCard()->getId());
        return card;
    }
};

class Heiyi: public ZeroCardViewAsSkill{
public:
    Heiyi():ZeroCardViewAsSkill("heiyi"){
    }
    virtual bool isEnabledAtPlay(const Player *player) const{
        return player->getRole() == "renegade" && !player->property("draw3").toBool();
    }
    virtual const Card *viewAs() const{
        return new HeiyiCard;
    }
};

void StandardPackage::addGenerals(){
    General *conan = new General(this, "conan", "45s");
    conan->addSkill(new Zhizhi);

    General *ai = new General(this, "ai", "45s", 3, false);
    ai->addSkill(new Shensi);

    General *ran = new General(this, "ran", "45s", 4, false);
    ran->addSkill(new Lingxi);

    General *heiji = new General(this, "heiji", "45s");
    heiji->addSkill(new Jizhi);

    General *kazuha = new General(this, "kazuha", "45s", 4, false);
    kazuha->addSkill(new Skill("jianchi"));

    General *sonoko = new General(this, "sonoko", "45s", 4, false);
    sonoko->addSkill(new Kaituo);
    addMetaObject<KaituoCard>();

    General *hiroshi = new General(this, "hiroshi", "45s");
    hiroshi->addSkill(new Boxue);

    General *tanteitan = new General(this, "tanteitan", "45s");
    tanteitan->addSkill(new Tongqing);

    General *kogorou = new General(this, "kogorou", "45s");
    kogorou->addSkill(new Weiya);

    General *takagi = new General(this, "takagi", "45s", 5);
    takagi->addSkill(new Skill("zhengzhi"));

    General *sato = new General(this, "sato", "45s", 4, false);
    sato->addSkill(new Checha);

    General *megure = new General(this, "megure", "45s");
    megure->addSkill(new Tiemian);

    General *akai = new General(this, "akai", "45s");
    akai->addSkill(new Skill("zhuisuo"));

    General *jodie = new General(this, "jodie", "45s", 4, false);
    jodie->addSkill(new Fanjie);

    General *gin = new General(this, "gin", "45s");
    gin->addSkill(new Gouxian);

    General *vodka = new General(this, "vodka", "45s");
    vodka->addSkill(new Shexian);
    addMetaObject<ShexianCard>();

    skills << new Mp1 << new Mp2 << new Mp3 << new Mp4 << new Heiyi;
    addMetaObject<Mp1Card>();
    addMetaObject<Mp4Card>();
    addMetaObject<Mp3Card>();
    addMetaObject<Mp2Card>();
    addMetaObject<HeiyiCard>();

    addMetaObject<CheatCard>();
}

class Buju:public MasochismSkill{
public:
    Buju():MasochismSkill("buju"){
        frequency = Compulsory;
    }

    virtual void onDamaged(ServerPlayer *player, const DamageStruct &) const{
        Room *room = player->getRoom();
        foreach(ServerPlayer *tmp, room->getOtherPlayers(player)){
            const Card *card = tmp->getRandomHandCard();
            room->invokeSkill(player, objectName());
            if(card)
                player->obtainCard(card);
            else{
                DamageStruct dmg;
                dmg.from = player;
                dmg.to = tmp;
                room->damage(dmg);
            }
        }
    }
};

TestPackage::TestPackage()
    :Package("test")
{
    General *aoyama = new General(this, "aoyama", "45s", 4, true, true);
    aoyama->addSkill(new Buju);

    new General(this, "sujiang", "god", 5, true, true);
    new General(this, "sujiangf", "god", 5, false, true);
}

ADD_PACKAGE(Test)
