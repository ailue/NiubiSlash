-- translation for RedPackage

return {
	["red"] = "红包", 

	["redzhonghui"] = "钟会",
	["designer:redzhonghui"] = "宇文天启",
	["cv:redzhonghui"] = "",
	["tongmou"] = "同谋",
	[":tongmou"] = "出牌阶段出牌前,指定一名角色为谋友并获得以下技能直到该角色下个回合结束:互为谋友的两名角色在其出牌阶段可以共享对方的手牌(见详解)",
	["tongmou_tie"] = "指定一名角色成为你的谋友",
	["xoxo"] = "基情四射",
	["mycard"] = "我的手牌",
	["tongmouv"] = "同謀",
	["xianhai"] = "陷害",
	[":xianhai"] = "你可以将你的天灾手牌放在任意角色的判定区内",

	["redxunyou"] = "荀攸",
	["designer:redxunyou"] = "黄书",
	["cv:redxunyou"] = "",
	["baichu"] = "百出",
	[":baichu"] = "游戏开始时，额外发给你一张牌，你须将一张手牌牌置于你的武将牌上之称为“计”。出牌阶段，你可以打出任意一张手牌替换一次“计”。回合内你每使用一张点数小于“计”的牌时可摸一张牌(限五次)；回合外你可将点数不小于“计”的牌当【闪】使用或打出",
	["@baichu"] = "请选择一张手牌作为“计”",
	["ji"] = "计",
	
	["redhejin"] = "何进",
	["designer:redhejin"] = "幻象迷宫",
	["cv:redhejin"] = "",
	["tonglu"] = "同戮",
	[":tonglu"] = "出牌阶段，你可以令场上武将牌正面朝上的角色依次选择是否愿意将自己的武将牌翻面。若如此做，你的下一张【杀】造成的伤害+X。X为愿意翻面的武将数量",
	["liehou"] = "列侯",
	[":liehou"] = "锁定技，回合结束阶段，若你没有发动“同戮”，立即摸一张牌",
	["tonglu:agree"] = "翻翻更健康~",
	["tonglu:deny"] = "翻你妹啊！",
	["#TongluBuff"] = "%from 的【同戮】生效，【杀】的伤害增加了 %arg, 达到了 %arg2 点",
	["#Liehou"] = "%from 的锁定技【%arg】被触发",
	["@wocao"] = "蓄力",

	["redguansuo"] = "关索",
	["designer:redguansuo"] = "宇文天启、俊逸不凡",
	["cv:redguansuo"] = "",
	["xiefang"] = "撷芳",
	[":xiefang"] = "你可以将其他角色装备区里的武器当【杀】、非武器当【闪】使用或打出",
	["yanyun"] = "偃云",
	[":yanyun"] = "若你使用的“撷芳”【杀】被闪避，可以继续对目标使用一张非“撷芳”【杀】",
	["yanyun-slash"] = "您可以再打出一张非“撷芳”【杀】来发动青云的追杀效果", 

	["redyanbaihu"] = "严白虎",
	["designer:redyanbaihu"] = "俊逸不凡",
	["cv:redyanbaihu"] = "",
	["jielue"] = "劫掠",
	[":jielue"] = "出牌阶段，你使用的【杀】指定目标后，可先附带拼点的效果，若你赢，你获得目标角色的拼点牌",

	["redwutugu"] = "兀突骨",
	["designer:redwutugu"] = "春哥姨妈",
	["cv:redwutugu"] = "",
	["chuzhen"] = "出阵",
	[":chuzhen"] = "觉醒技，回合开始阶段，若你已受伤，你须增加1点体力上限并获得技能“鳞甲”（锁定技，【南蛮入侵】及普通【杀】对你无效；你受到的火焰伤害+1）及“助兵”（摸牌阶段，你可放弃摸牌并展示牌堆顶X张牌，X为你的已损失体力值。其中每有一张黑桃牌，即视为你将此牌当作【南蛮入侵】使用。之后，你获得其余的牌）",
	["#ChuzhenWake"] = "%from 发现自己已经受伤，大吼一声，正式觉醒！",
	["linjia"] = "鳞甲",
	[":linjia"] = "锁定技，【南蛮入侵】及普通【杀】对你无效；你受到的火焰伤害+1",
	["zhubing"] = "助兵",
	[":zhubing"] = "摸牌阶段，你可放弃摸牌并展示牌堆顶的X张牌，X为你的已损失体力值。其中每有一张黑桃牌，即视为你将此牌当作【南蛮入侵】使用。之后，你获得其余的牌",
	["#LinjiaNullify"] = "%from 的锁定技【%arg】被触发，【%arg2】对其无效", 
	["#LinjiaDamage"] = "%from 的锁定技【鳞甲】的负面技能被触发，由 %arg 点火焰伤害增加到 %arg2 点火焰伤害",

	["redchunyuqiong"] = "淳于琼",
	["designer:redchunyuqiong"] = "连营",
	["cv:redchunyuqiong"] = "",
	["xujiu"] = "酗酒",
	[":xujiu"] = "回合开始阶段，你可将一张黑色手牌置于武将牌上称之为“酿”，你每次使用【杀】造成伤害时，可将一张正面向上的“酿”翻面，若如此做，该伤害+1",
	["niangA"] = "酿",
	["niangB"] = "翻面的酿",
	["xujiu_ask"] = "你可以将一张黑桃或梅花手牌移出游戏作为“酿”",
	["#XujiuBuff"] = "%from 的【酗酒】效果被触发，伤害从 %arg 点上升至 %arg2 点",
	["xuebi"] = "削鼻",
	[":xuebi"] = "锁定技，回合结束阶段，若你的“酿”大于三张，必须立即弃掉所有“酿”并减去一点体力上限",
	["#Xuebi"] = "%from 的锁定技【%arg】被触发，失去了一点体力上限和所有的 %arg2",

--http://www.qsanguosha.com/forum.php?mod=viewthread&tid=4995
	["redsunluban"] = "孙鲁班",
	["designer:redsunluban"] = "天狼欧阳杉",
	["cv:redsunluban"] = "",
	["jiaochong"] = "骄宠",
	[":jiaochong"] = "摸牌阶段摸牌后，你可以弃掉摸到的牌，重新摸取等量的牌，每阶段限用一次",
	["goulian"] = "媾连",
	[":goulian"] = "出牌阶段，你可以弃一张手牌并选择一名男性角色，该角色进行二选一：1.回复1点体力，并在你的下回合开始前替你承担一点伤害；2.摸两张牌，跳过该角色下回合的摸牌阶段，并令你下回合摸牌阶段额外摸两张牌。每阶段限一次",
	["goulian:a"] = "回复1点体力",
	["goulian:b"] = "摸两张牌",
	["@goulian"] = "媾连",
}
