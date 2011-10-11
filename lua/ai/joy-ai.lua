-- when enemy using the peach
sgs.ai_skill_invoke["grab_peach"] = function(self, data)
	local struct= data:toCardUse()
	return self:isEnemy(struct.from)
end

-- when murder Caiwenji

local yxswd_ai = SmartAI:newSubclass "yx_sword"
sgs.ai_skill_invoke["yx_sword"] = function(self, data)
	local damage= data:toDamage()
	if damage.to:hasSkill("duanchang") and damage.to:getHp() - damage.damage < 1 then
		return true
	end
end
