-- translation for GreenPackage

return {
	["green"] = "绿包",

--SHU#06 飞羽之首
	["greenyanpeng"] = "焉逢",
	["designer:greenyanpeng"] = "宇文天启",
	["cv:greenyanpeng"] = "",
	["yabian"] = "压变",
	[":yabian"] = "你用【杀】对一名其他角色造成伤害后，可以和该角色交换武将牌",
	["yabian:yes"] = "抢夺对方的武将牌",

--WEI06 鄢陵侯
	["greencaozhang"] = "曹彰",
	["designer:greencaozhang"] = "章鱼哥",
	["cv:greencaozhang"] = "",
	["ba0nu"] = "暴怒",
	[":ba0nu"] = "锁定技，当你的武将牌被翻面后，视为你对任意一名角色使用了一张杀",
	["zhechong"] = "折冲",
	[":zhechong"] = "回合内，你可以选择跳过摸牌，出牌或者弃牌阶段并将你的武将牌翻面，每回合限一次",

--QUN#05 深远的谋士
	["greenjushou"] = "沮授",
	["designer:greenjushou"] = "日月小辰",
	["cv:greenjushou"] = "",
	["yuanlv"] = "远虑",
	[":yuanlv"] = "出牌阶段，你可以自减一点体力，令场上除你之外的一名角色失去所有武将技直到其下次行动结束，每阶段限一次",
	["zhongjian"] = "忠谏",
	[":zhongjian"] = "弃牌阶段，你可以选择一名其他角色：直到你下回合开始，该角色发动一次武将技(仅限非锁定触发技和技能卡)时，你进行一次判定，若为红色，则该角色获得此判定牌，否则你获得此判定牌。将累计获得牌超过3张的角色的武将牌翻面",
	["@vy"] = "忠",
	["#Zj_TriggerSkill"] = "%to 即将发动技能【%arg2】，%from 的技能【%arg】生效",
	["#Zj_SkillCard"] = "%to 发动了技能【%arg2】，%from 的技能【%arg】生效",

--WU#04 谍战英豪
	["greenkanze"] = "阚泽",
	["designer:greenkanze"] = "天山路",
	["cv:greenkanze"] = "",
	["diezhi"] = "谍智",
	[":diezhi"] = "沉睡技，出牌阶段，你可以和任一非主公角色更换身份牌，其他任何角色都不知道你对谁发动了谍智技能，包括被谍智的角色本身，每阶段限一次。任何角色都无法查看谍智后的身份牌，包括你自己。当有角色死亡后，你无法发动本技能直到游戏结束",
	["@drig"] = "谍影",
	["fengjue"] = "封绝",
	[":fengjue"] = "当有角色在回合外摸牌时，你可以弃掉一张牌，阻止其摸牌操作",
--	["fengjue"] = "封绝",
--	[":fengjue"] = "回合内，指定一名角色，其体力值发生变化时立即复原，直到你下回合开始",

--WEI#06 旧朝老臣
	["greenwanglang"] = "王朗",
	["designer:greenwanglang"] = "星空梦幻、宇文天启",
	["cv:greenwanglang"] = "",
	["duizheng"] = "对政",
	[":duizheng"] = "出牌阶段，你可令一名其他角色和你拼点，若其没赢，你可以从对方那里获得其当前手牌的一半（向上取整）；若其赢，你受到对方造成的一点伤害。每阶段限一次",
	["zhuima"] = "坠马",
	[":zhuima"] = "锁定技，杀死你的角色使用的非延时锦囊无效直到游戏结束",
	["#ZhuimaEffect"] = "%from 的锁定技【%arg】被触发，凶手 %to 将被剥夺非延时锦囊使用权",
	["#ZhuimaForbidden"] = "%from 受到坠马效果影响，对 %to 使用的 %arg 无效",
	["@zhuima"] = "坠马",

--WU#05 江上麒麟
	["greenchenwu"] = "陈武",
	["designer:greenchenwu"] = "曉ャ絕對",
	["cv:greenchenwu"] = "",
	["qilin"] = "麒麟",
	[":qilin"] = "回合开始阶段，你可指定一名其他角色，该角色将手牌弃至或补至等同于你当前拥有的手牌数，当补或弃的牌不少于三张时，将你的武将牌翻面",

--SHU#07 刚柔并济
	["greenmayunlu"] = "马云禄",
	["designer:greenmayunlu"] = "安歧大小姐",
	["cv:greenmayunlu"] = "",
	["jinguo"] = "巾帼",
	[":jinguo"] = "回合开始阶段，你可以弃一张牌并指定一名角色，直到你的下回合开始，该角色成为【杀】的目标时，需使用【杀】才能抵消",
	["@jinguo"] = "你可以弃一张牌并指定一名角色，该角色获得一个巾帼标记",
	["@jin"] = "巾帼",
	["#JinguoEffect"] = "%to 受到【%arg】的影响，必须打出一张【杀】来响应 %from 的【杀】",
	["jinguo-jink"] = "%src 使用了【杀】，但你受到【巾帼】影响，必须打出一张【杀】视作【闪】",
	["wuqi"] = "武骑",
	[":wuqi"] = "回合外，任一角色进行判定且为红色时，你可以立即摸一张牌并对攻击范围内的一名角色使用一张【杀】",

--WEI#07 治世之才
	["greenchenqun"] = "陈群",
	["designer:greenchenqun"] = "司马子上",
	["cv:greenchenqun"] = "",
	["quanyi"] = "劝议",
	[":quanyi"] = "出牌阶段，你可以弃一张牌并指定两名角色，前者摸取与你的体力值相等的牌数，对后者使用的【杀】和非延时锦囊(指定了唯一目标)无效直到你下个回合开始阶段，每阶段限一次",
	["zhunsuan"] = "准算",
	[":zhunsuan"] = "回合结束阶段，你可以声明一个数字，直到你下个回合开始，其他角色使用或打出的基本牌进入弃牌堆时，若其点数和你声明的数字之差小于3，你获得这张牌",
	["#Zhunsuan"] = "%from 声明的数字是 %arg",
	["zhunsuan:back"] = "大了",
	["zhunsuan:next"] = "小了",
}
