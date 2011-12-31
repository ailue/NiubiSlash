-- kaituo
local kaituo_skill = {}
kaituo_skill.name = "kaituo"
table.insert(sgs.ai_skills, kaituo_skill)
kaituo_skill.getTurnUseCard = function(self)
	if self.player:getHandcardNum() < 2 or not self.player:isWounded() then return nil end
	local cards = self.player:getHandcards()
	cards=sgs.QList2Table(cards)
	local discard = {}
	self:sortByUseValue(cards, true)
	for _, card in ipairs(cards) do
		table.insert(discard, card:getEffectiveId())
		if #discard == 2 then break end
	end
	if #discard ~= 2 then return end
	return sgs.Card_Parse("@KaituoCard=" .. table.concat(discard, "+"))
end
sgs.ai_skill_use_func["KaituoCard"]=function(card,use,self)
	use.card = card
end

-- gouxian
local gouxian_skill = {}
gouxian_skill.name = "gouxian"
table.insert(sgs.ai_skills, gouxian_skill)
gouxian_skill.getTurnUseCard = function(self, inclusive)
    local cards = self.player:getHandcards()
    cards=sgs.QList2Table(cards)
	self:sortByUseValue(cards,true)
	local gx
	for _,card in ipairs(cards)  do
		if not card:inherits("Slash") and not card:inherits("Peach")
			and ((self:getUseValue(card)<sgs.ai_use_value["Slash"]) or inclusive) then
			gx = card
			break
		end
	end
	if gx then		
		local suit = gx:getSuitString()
    	local number = gx:getNumberString()
		local card_id = gx:getEffectiveId()
		local card_str = ("slash:gouxian[%s:%s]=%d"):format(suit, number, card_id)
		local slash = sgs.Card_Parse(card_str)
		assert(slash)
        return slash
	end
end

-- shexian
local shexian_skill = {}
shexian_skill.name = "shexian"
table.insert(sgs.ai_skills, shexian_skill)
shexian_skill.getTurnUseCard = function(self)
	if (self.player:getHp() > 3 and self.player:getHandcardNum() > self.player:getHp()) or		
		(self.player:getHp() - self.player:getHandcardNum() >= 2) then
		return sgs.Card_Parse("@ShexianCard=.")
	end
	if self.player:hasFlag("mp3") then
		for _, enemy in ipairs(self.enemies) do
			if self.player:canSlash(enemy, true) and self.player:getHp() > 1 then
				return sgs.Card_Parse("@ShexianCard=.")
			end
		end
	end
end
sgs.ai_skill_use_func["ShexianCard"]=function(card,use,self)
	use.card = card
end

-- mp3
local mp3_skill = {}
mp3_skill.name = "mp3"
table.insert(sgs.ai_skills, mp3_skill)
mp3_skill.getTurnUseCard = function(self)
	if not self.player:containsTrick("microphone") then return end
	if self.player:usedTimes("Mp3Card") > 0 or not self.enemies[1] then return end
	local slash = sgs.Card_Parse("@Mp3Card=.")
	assert(slash)
	return slash
end
sgs.ai_skill_use_func["Mp3Card"] = function(card,use,self)
	self:sort(self.enemies, "handcard")
	use.card = card
	if use.to then use.to:append(self.enemies[1]) end
end

-- mp4
local mp4_skill = {}
mp4_skill.name = "mp4"
table.insert(sgs.ai_skills, mp4_skill)
mp4_skill.getTurnUseCard = function(self)
	if not self.player:containsTrick("microphone") or not self.player:isWounded() then return end
	if self.player:usedTimes("Mp4Card") > 0 or self.player:isKongcheng() then return end
	local cards = self.player:getHandcards()
	cards = sgs.QList2Table(cards)
	self:sortByKeepValue(cards)
	local card_str = ("@Mp4Card=%d"):format(cards[1]:getId())
	return sgs.Card_Parse(card_str)
end
sgs.ai_skill_use_func["Mp4Card"] = function(card,use,self)
	use.card = card
end

-- heiyi
local heiyi_skill = {}
heiyi_skill.name = "heiyi"
table.insert(sgs.ai_skills, heiyi_skill)
heiyi_skill.getTurnUseCard = function(self)
	return sgs.Card_Parse("@HeiyiCard=.")
end
sgs.ai_skill_use_func["HeiyiCard"] = function(card,use,self)
	use.card = card
end
