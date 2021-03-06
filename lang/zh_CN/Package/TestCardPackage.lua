-- translation for ManeuveringPackage

return {
	["testCard"] = "避幕式测试牌堆",

    ["ice_slash"] = "冰幕",
	[":ice_slash"] = "基本牌<br />出牌时机：出牌阶段<br />使用目标：攻击范围内的一名其他角色<br />作用效果：对目标角色造成1点冰属性伤害",
	
	
	["Camera"] = "照相机",
	[":Camera"] = "装备牌·武器<br />攻击范围：４<br />武器特效：当你使用【杀】对目标角色造成伤害后，你可以依次明置其两张手牌。",

	["Gun"] = "狂气之枪",
	[":Gun"] = "装备牌·武器<br />攻击范围：４<br />武器特效：<font color=\"blue\"><b>锁定技，</b></font>当你使用【杀】对其他角色造成伤害后，横置该角色装备区的所有牌。",
	["#Gun"]= "%from 对 %to 触发了 %arg 的效果",
	
	["RoukankenHakurouken"] = "楼观白楼",
	[":RoukankenHakurouken"] = "装备牌·武器<br />攻击范围：1<br />武器特效：<font color=\"blue\"><b>锁定技，</b></font>当你使用【杀】指定目标角色后，需依次使用两张【避】才能抵消此【幕】。",
	--["#Gun"]= "%from 对 %to 触发了 %arg 的效果",
	
	["JadeSeal"] = "玉玺",
	[":JadeSeal"] = "装备牌·宝物\n\n技能：出牌阶段限一次。你可以视为使用【知己知彼】。",
	["~JadeSeal"] = "选择【知己知彼】的目标→点击确定",
	
	
    ["Camouflage"] = "光学迷彩",
	[":Camouflage"] = "装备牌·防具\n\n技能：<font color=\"blue\"><b>锁定技，</b></font>当你受到伤害时，若场上仅有一张防具牌，防止此伤害，此牌因使用进入你的装备区后，你须弃置场上一张不同名的防具牌。",
    ["#Camouflage"] = "%from 的防具【%arg2】防止了 %arg 点伤害",
	
	["await_exhausted"] = "佛心道诲",
	[":await_exhausted"] = "锦囊牌\n\n使用时机：出牌阶段。\n使用目标：至多两名角色（你须为其中之一），\n作用效果：目标角色摸两张牌，然后弃置两张牌。",
	
	["spell_duel"] = "符斗",
	[":spell_duel"] = "锦囊牌\n\n使用时机：出牌阶段。\n使用目标：一名有手牌的角色\n作用效果：与其拼点，若你赢，对其造成1点伤害，否则，你受到其造成的1点伤害。",
	
	["kusuri"] = "药",
	[":kusuri"] = "基本牌<br />\n使用时机：出牌阶段。\n使用目标：包括你在内的一名处于异常状态的角色。\n作用效果：目标角色依次解除以下异常状态（翻面，武将牌横置，装备区的牌横置，明置手牌）"
	.."\n\n使用方法Ⅱ：\n使用时机：当一名角色处于濒死状态时。\n使用目标：一名处于濒死状态的角色。\n作用效果：目标角色回复1点体力。",
	
	
	["gassing_garden"] = "毒气花园",
	[":gassing_garden"] = "延时锦囊牌<br />出牌时机：出牌阶段<br />使用目标：一名角色。<br />作用效果：目标角色进行判定，若结果不为<font color=\"black\"><b>方块</b></font>，则该目标角色失去一格残机；反之，则保留此牌并不执行任何操作。。",
	
	["donation"] = "强制赛钱",
	[":donation"] = "延时锦囊牌<br />出牌时机：出牌阶段<br />使用目标：一名角色。<br />作用效果：目标角色进行判定，若结果不为<font color=\"black\"><b>黑桃</b></font>，其需弃置两张牌。",

	["vscrossbow"] = "蓬莱人形",
	[":vscrossbow"] = "装备牌·武器<br /><b>攻击范围</b>：2<br /><b>武器技能</b>：锁定技。出牌阶段你的【幕】使用次数额外+1。当你于每阶段第二次使用【幕】时，你摸一张牌。",
	
	["Robe"] = "火鼠皮衣",
	[":Robe"] = "装备牌·防具<br /><b>防具技能</b>：<font color=\"blue\"><b>锁定技，</b></font>当你受到属性伤害时，你防止之。",
	["#Robe"] = "%from 防止了 %to 对其造成的 %arg 点伤害[%arg2]",
	
	["Raiment"] = "龙鱼羽衣",
	[":Raiment"] = "装备牌·防具<br /><b>防具技能</b>：当你对一张有来源的【幕】使用【避】后，你可以进行一次判定，若判定结果为黑色，则你对来源造成一点雷属性伤害。",
	
	["Scenery"] = "地狱绘扇",
	[":Scenery"] = "装备牌·宝物<br /><b>宝物技能</b>：当一名角色脱离濒死状态后，你可以令另一名其他角色失去一格残机（你自己与该存活角色除外）。",
	["@Scenery"] = "你可以发动地狱绘扇，另一名其他角色失去1点体力。",
	
	["Chronicle"] = "幻想乡缘起",
	[":Chronicle"] = "装备牌·宝物<br /><b>宝物技能</b>：回合开始时或回合结束时，你可以观看牌堆顶的一张牌，然后选择将该牌置于牌堆底或牌堆顶。",
	["#GuanxingResult"] = "%from 的“<font color=\"yellow\"><b>幻想乡缘起</b></font>”结果：%arg 上 %arg2 下",
	
		["Scroll"] = "魔人经卷",
	[":Scroll"] = "装备牌·宝物<br /><b>宝物技能</b>◆出牌阶段，若装备区内的本牌上不存在牌，你可以将一张手牌明置于本牌上。<br />◆当一张符卡对你生效时，若该符卡与本牌上明置的牌的颜色相同，你可以防止该符卡的效果，然后将该明置的牌加入手牌。<br />◆当本牌从你的装备区内离开时，你获得本牌上明置的牌。",
     ["ScrollTriggerSkill"] = "魔人经卷",
	 
	 ["IbukiGourd"] = "伊吹瓢",
	[":IbukiGourd"] = "装备牌·宝物<br /><b>宝物技能</b>◆判定阶段开始前，你可以跳过判定阶段和摸牌阶段，视为对自己使用一张【酒】。<br />◆当你进入濒死状态时，你可以将装备区内的本牌弃置，视为对自己使用一张【酒】。",
     ["@IbukiGourd2"] = "你可以发动伊吹瓢， 跳过判定阶段和摸牌阶段，视为对自己使用一张【酒】。",
	["@IbukiGourd1"] = "你可以弃置伊吹瓢，视为对自己使用一张【酒】。",
     ["~IbukiGourd"] = "选择你为目标->确定。",
	}
