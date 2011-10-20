-- translation for WisdomPackage

local t = {
	["wisdom"] = "智包", 
	["designer:wisdoms"] = "太阳神三国杀创意小组",

	["wisxuyou"] = "许攸", 
	["wisjiangwei"] = "姜维", 
	["wisjiangwan"] = "蒋琬",
	["wissunce"] = "孙策",
	["wiszhangzhao"] = "张昭",
	["wishuaxiong"] = "华雄",
	["wistianfeng"] = "田丰",
	["wisshuijing"] = "司马徽",

	["juao"] = "倨傲",
	[":juao"] = "出牌阶段,你可以选择两张手牌背面向上移出游戏,指定一名角色,被指定的角色到下个回合开始阶段时,跳过摸牌阶段,得到你所移出游戏的两张牌。每阶段限一次",
	["juaocd"] = "倨傲牌",
	["#JuaoObtain"] = "%from 得到了 %to 发动【%arg】移出游戏的两张牌",
	["tanlan"] = "贪婪",
	[":tanlan"] = "你每受到一次伤害,可与伤害来源进行拼点:若你赢,你获得两张拼点牌",
	["shicai"] = "恃才",
	[":shicai"] = "锁定技,当你拼点成功时,立即摸一张牌",
	["yicai"] = "异才",
	[":yicai"] = "每当你使用一张非延时类锦囊时(在它结算之前),可立即对攻击范围内的角色使用一张【杀】",
	["@yicai"] = "温馨提示：你可以对攻击范围内的一名角色使用一张【杀】",
	["beifa"] = "北伐",
	[":beifa"] = "锁定技,当你失去最后一张手牌时,视为对攻击范围内的一名角色使用了一张【杀】\
★北伐发动时,姜维必须一次做出决定:选择一名攻击范围内的角色(若攻击范围内没有角色,视为选择了自己),然后视为对该角色使用一张【杀】",
	["houyuan"] = "后援",
	[":houyuan"] = "出牌阶段,你可以弃掉两张手牌,指定一名其他角色摸两张牌,每阶段限一次",
	["chouliang"] = "筹粮",
	[":chouliang"] = "回合结束阶段,当你手牌少于三张时,你可以从牌堆顶亮出X张牌(X为4减当前手牌数),拿走其中的基本牌,弃掉其余的牌 ",
	["bawang"] = "霸王",
	[":bawang"] = "当你使用的【杀】被闪避时,你可以和对方拼点:若你赢,可以选择最多两个目标,视为对其分别使用了一张【杀】",
	["@bawang"] = "你可以指定1~2个看上去不爽的家伙为目标",
	["weidai"] = "危殆",
	[":weidai"] = "主公技,当你需要使用一张【酒】时,吴势力角色可以打出一张黑桃2~9的手牌,视为替你出了一张【酒】(见详解)\
★当你需要使用一张【酒】或濒死时,你可以发动危殆。所有吴势力角色按行动顺序依次选择是否打出一张黑桃2~9的手牌,视为“提供”给你(视为由你使用)一张【酒】,直到有一名角色或没有任何角色决定如此做时为止",
--	["analeptic:accept"] = "打出一张黑桃2~9手牌响应【危殆】",
--	["analeptic:ignore"] = "我是打酱油的~",
	["$Weidai"] = "%from 弃掉了 %card",
	["@weidai"] = "你正徘徊在生死边缘，是否发动【危殆】？",
	["@weidai-analeptic"] = "%src 现在需要【酒】<br/>你只要打出一张 黑桃２~９的手牌<br/>就能为 %src 提供一张【酒】",
	["longluo"] = "笼络",
	[":longluo"] = "弃牌阶段,你可以选择一名其他角色摸取与你弃牌数量相同的牌",
	["fuzuo"] = "辅佐",
	[":fuzuo"] = "当有角色拼点时,你可以弃掉自己的一张点数小于8的手牌,让其中一名角色的拼点牌加上这张牌点数的二分之一（向下取整）",
	["fuzuo:cancel"] = "不发动",
	["$Fuzuo"] = "%from 打出了 %card 作为拼点干预牌，干预目标是 %to （拼点干预牌不影响原拼点牌）",
	["$Fuzuo_from"] = "%from 的拼点牌是 %card",
	["$Fuzuo_to"] = "%to 的拼点牌是 %card. 等待 %from 选择拼点干预目标",
	["@fuzuo_card"] = "请打出一张点数小于8的手牌来干预拼点",
	["jincui"] = "尽瘁",
	[":jincui"] = "当你死亡时,可令一名角色立即摸取或者弃掉三张牌",
	["jincui:draw"] = "摸三张牌",
	["jincui:throw"] = "弃三张牌",
	["badao"] = "霸刀",
	[":badao"] = "当你成为黑色的【杀】目标时,你可以立即对你攻击范围内的一名角色使用一张【杀】",
	["@badao"] = "你可以对你攻击范围内的一名角色使用一张【杀】",
	["wenjiu"] = "温酒",
	[":wenjiu"] = "锁定技,你使用黑色的【杀】造成的伤害+1,你无法闪避红色的【杀】",
	["#Wenjiu1"] = "%to 受到【温酒】负面效果的影响，%from 对其使用的红色杀不可闪避",
	["#Wenjiu2"] = "%from 的【温酒】技能被触发，伤害从 %arg 点上升至 %arg2 点",
	["shipo"] = "识破",
	[":shipo"] = "任意角色判定阶段判定前,你可以弃掉两张牌,获得该角色判定区里的所有牌",
	["gushou"] = "固守",
	[":gushou"] = "当你使用或打出一张【闪】时,可立即摸一张牌",
	["yuwen"] = "狱刎",
	[":yuwen"] = "锁定技,当你死亡时,凶手视为自己",
	["#YuwenEffect"] = "%from 的锁定技【%arg】被触发，死亡视为自杀",
	["shouye"] = "授业",
	[":shouye"] = "出牌阶段,你可以弃掉一张红色手牌,指定最多两名其他角色各摸一张牌",
	["jiehuo"] = "解惑",
	[":jiehuo"] = "觉醒技,当你发动【授业】目标累计超过6个时,须减去一点体力上限,将技能“授业”改为每阶段限一次,并永久获得技能“师恩”:其他角色使用非延时锦囊时,(在结算前)可以让你摸一张牌",
	["#JiehuoWake"] = "%from 的觉醒技【%arg】触发，以后你每阶段只能发动一次【%arg2】",
	["shien"] = "师恩",
	[":shien"] = "其他角色使用非延时锦囊时,（在结算前）可以让你摸一张牌",
	["forbid_shien"] = "水镜秘书",
	["forbid_shien:yes"] = "永久关闭【师恩】发动提示（不可逆操作）",
	["forbid_shien:no"] = "不关闭【师恩】发动提示",

	["cv:wisxuyou"] = "", 
	["cv:wisjiangwei"] = "", 
	["cv:wisjiangwan"] = "",
	["cv:wissunce"] = "",
	["cv:wiszhangzhao"] = "",
	["cv:wishuaxiong"] = "",
	["cv:wistianfeng"] = "",
	["cv:wisshuijing"] = "",

	["~wisxuyou"] = "智之许攸，不幸阵亡", 
	["~wisjiangwei"] = "智之姜维，不幸阵亡", 
	["~wisjiangwan"] = "智之蒋琬，不幸阵亡",
	["~wissunce"] = "智之孙策，不幸阵亡",
	["~wiszhangzhao"] = "智之张昭，不幸阵亡",
	["~wishuaxiong"] = "智之华雄，不幸阵亡",
	["~wistianfeng"] = "智之田丰，不幸阵亡",
	["~wisshuijing"] = "智之水镜先生，不幸阵亡",
}

local generals = {"wisxuyou", "wisjiangwei", "wisjiangwan", "wissunce", "wiszhangzhao", "wishuaxiong", "wistianfeng", "wisshuijing"}

for _, general in ipairs(generals) do
	t["designer:" .. general] = t["designer:wisdoms"]
end

return t
