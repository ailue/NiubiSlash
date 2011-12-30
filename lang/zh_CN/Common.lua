return {
	["spade"] = "黑桃",
	["club"] = "梅花",
	["heart"] = "红桃",
	["diamond"] = "方块",

	["trick"] = "线索牌",
	["attack_card"] = "进攻牌",
	["defense_card"] = "防御牌",
	["recover_card"] = "恢复牌",
	["global_effect"] = "全局效果",
	["aoe"] = "范围效果",
	["single_target_trick"] = "单体效果",
	["delayed_trick"] = "硬件系统",
	["microphone"] = "话筒",
	[":microphone"] = "代表发言权的象征，是特殊的卡片，具有面前卡的部分特性，但不属于面前卡。游戏开始时在侦探面前，使用后或持有者出局时自动转移到持有者的上一个行动者面前，也可以通过其他方式来转移话筒位置。",

	["lord"] = "侦探",
	["loyalist"] = "好人",
	["rebel"] = "凶手",
	["renegade"] = "黑衣人",
	["spade_char"] = "♠",
	["club_char"] = "♣",
	["heart_char"] = "♥",
	["diamond_char"] = "♦",
	["no_suit_char"] = "无色",
	["start"] = "开始",
	["judge"] = "判定",
	["draw"] = "摸牌",
	["play"] = "出牌",
	["discard"] = "弃牌",
	["finish"] = "结束",
	["online"] = "在线",
	["offline"] = "离线",
	["robot"] = "电脑",
	["trust"] = "托管",
	["cheat"] = "作弊",
	["yes"] = "是",
	["no"] = "否",

	["slash"] = "指控",
	[":slash"] = "对另一个人进行指控，如果对方不进行辩护，那么对方得到一个罪证。此卡一回合只能使用一张。",
	["jink"] = "辩护",
	[":jink"] = "当有人指控你时，可以使用此卡进行辩护。",
	["peach"] = "不在场证明",
	[":peach"] = "去除自己或其他人的一个罪证，在自己即将出局时也可以使用。",
	["amazing_grace"] = "现场分析",
	[":amazing_grace"] = "从卡堆上翻出在场人数加一的卡，从此卡使用者开始，按行动顺序每人从中挑选一张卡。",
	["archery_attack"] = "你们都有嫌疑",
	[":archery_attack"] = "场上除自己以外的所有人都必须依次进行辩护，否则得到一个罪证。",
	["duel"] = "论争",
	[":duel"] = "和另一个人展开辩论，从对方开始，双方轮流出指控卡，直到有一方不出为止，不出的一方得到一个罪证。",
	["ex_nihilo"] = "思考",
	[":ex_nihilo"] = "抽两张卡",
	["dismantlement"] = "识破",
	[":dismantlement"] = "指定另一个人随机丢弃一张卡。",
	["nullification"] = "异议",
	[":nullification"] = "在其他人使用任意卡时可以使用，那张卡无效化。",

	["ingenarg"] = "巧辩",
	[":ingenarg"] = "当被指控时可以用此卡进行辩护，之后再抽一张卡。",
	["concludence"] = "决定性证据",
	[":concludence"] = "直接使一个人得到一个罪证。",
	["ignore"] = "无视",
	[":ignore"] = "使用后放在自己面前，只要此卡在自己面前存在，当有人指控你或和你辩论时，可以弃掉此卡使指控或论争无效。此卡一个人面前最多只能有一张。",
	["speak"] = "我有话要说",
	[":speak"] = "将话筒拿到自己面前，如果回合结束前没有使用的话，必须放回原处。",

	["slash-jink"] = "%src 使用了【指控】，请打出一张【辩护】或【巧辩】",
	["duel-slash"] = "%src 向您论争，您需要打出一张【指控】",
	["archery-attack-jink"] = "%src 说【你们都有嫌疑】，请打出一张【辩护】或【巧辩】",
	["#Slash"] = "%from 对 %to 使用了【指控】",
	["#Jink"] = "%from 使用了【辩护】",
	["@wushuang-jink-1"] = "%src 拥有【直指】技能，您必须连续出两张【闪】",
	["@wushuang-jink-2"] = "%src 拥有【直指】技能，你还需出一张【闪】",
	["@tongqing"] = "%src 罪证过多即将出局，你可以发动【同情】为其辩护",

	["mp1"] = "①",
	["mp2"] = "②",
	["mp3"] = "③",
	["mp4"] = "④",
	[":mp1"] = "话筒功能1：对另一个人进行指控，不消耗其他卡，也不受一回合只能指控一次的限制。\
★话筒指控不可被异议，但可以触发毛利小五郎和目暮十三的技能。",
	[":mp2"] = "话筒功能2：使用一张指控卡，指控全场自己以外的所有人。\
★虽然是指控但不能无视。因为要使用指控卡，所以可以触发柯南的技能。",
	[":mp3"] = "话筒功能3：指定一个人，本回合对他的指控不受一回合一次的限制。\
★可以先对一个人进行指控，再对另一个人使用话筒来解除指控限制。但是不能相反：先对一个人使用话筒解除指控限制，使用过指控后再指控另一个人。",
	[":mp4"] = "话筒功能4：丢弃任意一张卡，去除自己的一个罪证。\
★即将出局时不能使用。",

	["heiyi"] = "黑衣",
	[":heiyi"] = "黑衣人专属技，你可以亮出身份来使自己在抽卡步骤多抽一张卡",
	["#Heiyi"] = "%from [%arg] 亮出了自己的身份",
	["#Murder"] = "%to【%arg】被 %from 定罪出局",
	["#Suicide"] = "%to【%arg】自首出局",
	["#InvokeSkill"] = "%from 发动了技能【%arg】",
	["#TriggerSkill"] = "%from 的技能【%arg】被触发",
	["#Damage"] = "%from 令 %to 得到了 %arg 个罪证",
	["#DamageNoSource"] = "%to 得到了 %arg 个罪证",
	["#Recover"] = "%from 去除了 %arg 个罪证",
	["#AskForPeaches"] = "%from 需要 %arg 个不在场证明",
	["#NullificationDetails"] = "%from 对 %to 的 %arg 被无效化",
	["#SkillAvoid"] = "%from 的 %arg 技能被触发，这张 %arg2 不能指定其作为目标",
	["#Transfigure"] = "%from 变身为 %arg",
	["#AcquireSkill"] = "%from 获得了技能 【%arg】",
	["$Dismantlement"] = "%from 失去了 %card",
	["$MoveCard"] = "%to 从 %from 处得到了 %card",
	["$MicroMove"] = "%card 从 %from 移动到了 %to 面前",
	["$DiscardCard"] = "%from 弃了 %card",
	["$RecycleCard"] = "%from 从弃牌堆回收了 %card",
	["$ShowCard"] = "%from 展示了 %card",
	["$PutCard"] = "%from 的 %card 被放置在了摸牌堆",
	["#Contingency"] = "%to【%arg】出局了",
	["#SkillNullify"] = "%from 的技能【%arg】被触发，【%arg2】对其无效",
	["#DrawNCards"] = "%from 摸了 %arg 张牌",
	["#MoveNCards"] = "%to 从 %from 处得到 %arg 张牌",
	["$TakeAG"] = "%from 拿走了 %card",
	["#TurnOver"] = "%from 将自己的武将牌翻面，现在是 %arg",
	["face_up"] = "面朝上",
	["face_down"] = "面朝下",
	["#SkipPhase"] = "%from 跳过了 %arg 阶段",
	["#LoseHp"] = "%from 流失了 %arg 点体力",

	["test"] = "测试",
	["god"] = "上帝",
	["sujiang"] = "稻草男",
	["sujiangf"] = "稻草女",

	["3v3:cw"] = "顺时针",
	["3v3:ccw"] = "逆时针",
	["cw"] = "顺时针",
	["ccw"] = "逆时针",
	["#TrickDirection"] = "%from 选择了 %arg 作为结算的顺序",

	["runaway_mode"] = "跑路模式",
	["#Runprex"] = "%from 翻开一张跑路牌，但由于其点数 %arg 不小于场上存活角色数，本次不得跑路",
	["#Runpre"] = "%from 翻开一张跑路牌，他需要跑 %arg 步路",
	["#Runaway2"] = "%arg 位 %from 和 %arg2 位 %to 交换了位置",
	["#Runaway"] = "%arg 位 %from 跑到了 %arg2 位 的位置",
}