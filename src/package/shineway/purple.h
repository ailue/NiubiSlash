#ifndef PURPLE_H
#define PURPLE_H

#include "package.h"
#include "card.h"
#include "standard.h"
/*
class ZhongshuCard: public SkillCard{
    Q_OBJECT

public:
    Q_INVOKABLE ZhongshuCard();

    virtual bool targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const;
    virtual void use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const;
};

class RujiCard: public SkillCard{
    Q_OBJECT

public:
    Q_INVOKABLE RujiCard();

    virtual bool targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const;
    virtual void use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const;
};

class JunlingCard: public SkillCard{
    Q_OBJECT

public:
    Q_INVOKABLE JunlingCard();

    virtual bool targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const;
    virtual bool targetsFeasible(const QList<const Player *> &targets, const Player *Self) const;
    virtual void use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const;
};
*/
class PurplePackage: public Package{
    Q_OBJECT

public:
    PurplePackage();
};

#endif // PURPLE_H