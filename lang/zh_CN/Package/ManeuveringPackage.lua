﻿-- translation for ManeuveringPackage

return {
	["maneuvering"] = "军争篇",

	["fire_slash"] = "火幕",
	[":fire_slash"] = "基本牌<br />出牌时机：出牌阶段<br />使用目标：攻击范围内的一名其他角色<br />作用效果：对目标角色造成1点火焰伤害",

	["thunder_slash"] = "雷幕",
	[":thunder_slash"] = "基本牌<br />出牌时机：出牌阶段<br />使用目标：攻击范围内的一名其他角色<br />作用效果：对目标角色造成1点雷电伤害",

	["analeptic"] = "酒",
	[":analeptic"] = "基本牌<br />出牌时机：1、出牌阶段；2、当自己进入濒死阶段时<br />使用目标：1、包括你的一名角色 2、你<br />作用效果：1、令自己本阶段使用的下一张【杀】将要造成的伤害+1（每阶段限一次）；2、回复1点体力",
	["#UnsetDrankEndOfTurn"] = "%from 的回合结束，【<font color=\"yellow\"><b>酒</b></font>】的效果消失",

	["Fan"] = "朱雀羽扇",
	--[":Fan"] = "装备牌·武器<br />攻击范围：４<br />武器特效：你可以将一张普通【杀】当火【杀】使用",
	[":Fan"] = "装备牌·武器<br />攻击范围：４<br />武器特效：当你使用【杀】指定目标时：若此牌为普通【杀】，你可令之视为火【杀】；若此牌为火【杀】，你可以摸一张牌。",

	["GudingBlade"] = "八卦炉",
	[":GudingBlade"] = "装备牌·武器<br />攻击范围：２<br />武器特效：<font color=\"blue\"><b>锁定技，</b></font>每当你使用【幕】对目标角色造成伤害时，若该角色没有手牌，此伤害+1",
	["#GudingBladeEffect"] = "%from 的【<font color=\"yellow\"><b>古锭刀</b></font>】效果被触发， %to 没有手牌，伤害从 %arg 增加至 %arg2",

	["Vine"] = "光学迷彩",
	[":Vine"] = "装备牌·防具<br />防具效果：<font color=\"blue\"><b>锁定技，</b></font>【月面战争】、【有顶天变】和普通【杀】对你无效。每当你受到火焰伤害时，此伤害+1",
	["#VineDamage"] = "%from 的防具【<font color=\"yellow\"><b>藤甲</b></font>】效果被触发，火焰伤害由 %arg 点增加至 %arg2 点",

	["SilverLion"] = "雏人形",
	[":SilverLion"] = "装备牌·防具<br />防具效果：<font color=\"blue\"><b>锁定技，</b></font>每当你受到多于1点的伤害时，你防止多余的伤害；<font color=\"blue\"><b>锁定技，</b></font>每当你失去装备区里的【白银狮子】时，你回复1点体力。",
	["#SilverLion"] = "%from 的防具【%arg2】防止了 %arg 点伤害，减至 <font color=\"yellow\"><b>1</b></font> 点",

	["fire_attack"] = "春雪异变",
	[":fire_attack"] = "锦囊牌<br />出牌时机：出牌阶段<br />使用目标：一名有手牌的角色。<br />作用效果：目标角色展示一张手牌。若你弃置一张与所展示牌相同花色的手牌，该角色受到1点水属性伤害",
	["fire-attack-card"] = "您可以弃置一张与 %dest 所展示卡牌相同花色(%arg)的牌对 %dest 造成1点冰属性伤害",
	["@fire-attack"] = "%src 展示的牌的花色为 %arg，请弃置一张与其相同花色的手牌",

	["iron_chain"] = "秘封结社",
	[":iron_chain"] = "锦囊牌<br />出牌时机：出牌阶段。<br />使用目标：一至两名角色<br />作用效果：每名目标角色选择一项：1.横置其武将牌；2. 重置其武将牌。 <br />重铸：将此牌置入弃牌堆，然后摸一张牌",

	["supply_shortage"] = "信仰危机",
	[":supply_shortage"] = "延时锦囊牌<br />出牌时机：出牌阶段<br />使用目标：距离为1的一名其他角色。<br />作用效果：目标角色进行判定，若结果不为♣，目标角色跳过摸牌阶段",

	["HuaLiu"] = "骅骝",

	["IronArmor"] = "明光铠",
	--[":IronArmor"] = "装备牌·防具<br /><b>防具技能</b>：<font color=\"blue\"><b>锁定技，</b></font>你不是属性【杀】、【火攻】、【铁索连环】的合法目标。",
	[":IronArmor"] = "装备牌·防具<br /><b>防具技能</b>：<font color=\"blue\"><b>锁定技，</b></font>属性【杀】、【火攻】、【铁索连环】对你无效。",
}
