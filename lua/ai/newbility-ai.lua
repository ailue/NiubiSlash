-- Newbility's AI by Ubun.

-- niubi-armor
function SmartAI:useNiubi(card, use)
	use.card = card
	return
end

-- �㽶
sgs.ai_skill_playerchosen["banana"] = function(self, targets)
	self:sort(targets, "hp")
	for _, enemy in ipairs(targets) do
		if self:isEnemy(enemy) then
			return enemy
		end
	end
end

-- ��־��Ƭ
sgs.ai_skill_invoke["madamfeng"] = function(self, data)
	self:sort(self.friends, "hp")
	return self.friends[1]:isWounded()
end
sgs.ai_skill_playerchosen["madamfeng"] = function(self, targets)
	self:sort(targets, "hp")
	for _, friend in ipairs(targets) do
		if self:isFriend(friend) then
			return friend
		end
	end
end

-- ����Ͱ
local dustbin_skill = {}
dustbin_skill.name = "dustbin"
table.insert(sgs.ai_skills, dustbin_skill)
dustbin_skill.getTurnUseCard = function(self)
	if self.player:hasFlag("dust") then return end
	if self.player:hasSkill("lianying") and self.player:getHandcardNum() == 1 then
		local cards = self.player:getHandcards()
		lastcard = sgs.QList2Table(cards)[1]
		return sgs.Card_Parse("$" .. lastcard:getId())
	end
end

-- ����ͼ
local animals_skill={}
animals_skill.name = "animals"
table.insert(sgs.ai_skills, animals_skill)
animals_skill.getTurnUseCard = function(self)
	local cards = self.player:getCards("h")
	cards=sgs.QList2Table(cards)
	local card
	for _,acard in ipairs(cards)  do
		if acard:inherits("TrickCard") and acard:getSuit() == sgs.Card_Spade then
			card = acard
			break
		end
	end
	if not card then return end
	local suit = card:getSuitString()
	local number = card:getNumberString()
	local card_id = card:getEffectiveId()
	local card_str = ("duel:animals[%s:%s]=%d"):format(suit, number, card_id)
	local skillcard = sgs.Card_Parse(card_str)
	assert(skillcard)
	return skillcard
end

-- ߦ����
local rollingpin_skill={}
rollingpin_skill.name = "rollingpin"
table.insert(sgs.ai_skills, rollingpin_skill)
rollingpin_skill.getTurnUseCard=function(self)
	if self.player:hasUsed("RollingpinCard") or self.player:getHandcardNum()<2 then return end
	if self.player:getLostHp() < 2 then return end
	local cards = self.player:getHandcards()
	cards=sgs.QList2Table(cards)
	local first, second
	self:sortByUseValue(cards,true)
	for _, card in ipairs(cards) do
		if card:getTypeId() ~= sgs.Card_Equip then
			if not first then first  = cards[1]:getEffectiveId()
			else second = cards[2]:getEffectiveId()
			end
		end
		if second then break end
	end
	if not second then return end
	local card_str = ("@RollingpinCard=%d+%d"):format(first, second)
	assert(card_str)
	return sgs.Card_Parse(card_str)
end
sgs.ai_skill_use_func["RollingpinCard"] = function(card,use,self)
	use.card = card
end

-- ����ǹ
sgs.ai_skill_invoke["tranqgun"] = function(self, data)
	return self:isEnemy(data:toPlayer())
end

-- clearShirt
function SmartAI:useClearShirt(card, use)
	local next_player = self.player:getNextAlive()
	if self:isFriend(next_player) and next_player:getArmor() then
		if next_player:getArmor():objectName() == "gale-shell" or
			(next_player:getArmor():objectName() == "silver_lion" and next_player:isWounded()) then
			use.card = card
		end
	elseif self:isEnemy(next_player) and (not next_player:getArmor() or
		next_player:getArmor():objectName() ~= "gale-shell") then
		use.card = card
	end
	return
end

-- sacrifice
function SmartAI:useCardSacrifice(sacrifice, use)
	if self.player:hasSkill("wuyan") then return end
    if self.player:getHp() <= 2 then return end
	self:sort(self.friends_noself, "hp")
	for _, friend in ipairs(self.friends_noself) do
		if friend:isWounded() then
			use.card = sacrifice
			if use.to then
				use.to:append(friend)
			end
			break
		end
	end
end

-- diezhi
local diezhi_skill = {}
diezhi_skill.name = "diezhi"
table.insert(sgs.ai_skills, diezhi_skill)
diezhi_skill.getTurnUseCard = function(self)
	if self.player:isLord() or self.player:getMark("@drig") < 1 then return end
	return sgs.Card_Parse("@DiezhiCard=.")
end
sgs.ai_skill_use_func["DiezhiCard"] = function(card, use, self)
	use.card = card
end

sgs.ai_skill_choice["drig-guess"] = function(self, choices)
	local players = self.room:getOtherPlayers(self.player)
	players = sgs.QList2Table(players)
	local kanze
	for _, other in ipairs(players) do
		if other:hasSkill("diezhi") then kanze = other break end
	end
	if kanze:isLord() then return "lord" 
	else
		local r = math.random(0, 1)
		if r == 0 then
			return "loyalist"
		else
			return "rebel"
		end
	end
end

