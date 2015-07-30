local CAMsg = require("Message.CA_Message")
local CLMsg = require("Message.CL_Message")
local CGMsg = require("Message.CG_Message")
local CNMsg = require("Message.CN_Message")
local MSGID = require("Message.MessageConst")

local Robot = class("Robot", sington)

--[[
msgRouter:getInstance():registerFunction(MSGID.MAIN.AC, MSGID.AC.LOGIN_RESULT, self.onLoginResult)
msgRouter:getInstance():registerFunction(MSGID.MAIN.AC, MSGID.AC.SETTOKEN, self.onAddToken)
msgRouter:getInstance():registerFunction(MSGID.MAIN.LC, MSGID.LC.ENTERGAME, self.onEnterGame)
msgRouter:getInstance():registerFunction(MSGID.MAIN.LC, MSGID.LC.CREATEPLAYER, self.onCreateResult)
msgRouter:getInstance():registerFunction(MSGID.MAIN.NC, MSGID.NC.GETNAMES, self.onGetRecommandNames)
]]

local handlerMap = 
{
	[MSGID.MAIN.AC] = 
	{
		[MSGID.AC.LOGIN_RESULT] = function() self:onLoginResult(data) end, 
		[MSGID.AC.SETTOKEN] 	= function() self:onAddToken(data) end, 
	},

	[MSGID.MAIN.LC] = 
	{
		[MSGID.LC.ENTERGAME] 	= function() self:onEnterGame(data) end, 
		[MSGID.LC.CREATEPLAYER] = function() self:onCreateResult(data) end, 
	},

	[MSGID.MAIN.NC] = 
	{
		[MSGID.NC.GETNAMES] = function() self:onGetRecommandNames(data) end, 
	},
}

function Robot:ctor()
	self.sock = nil
	self.strToken = nil
end

function Robot:onMsg(data)
    local mid, sid = data.msgMainId, data.msgMinorId
    if handlerMap[mid] and handlerMap[mid][sid] then
    	handlerMap[mid][sid](data)
    else
    	self:onRecvGateMsg(data)
    end
end



function Robot:login()
    local tab = clone(CAMsg.CA_LoginMsg)
    tab.msgMainId = MSGID.MAIN.CA
    tab.msgMinorId = MSGID.CA.LOGIN
    tab.strAccount = "finger"
    tab.strPassword = "123456"
    
    sendMsg(self.sock, tab)
end

function Robot:onLoginResult(data)
	local tab = {}
    
    tab.msgMainId = MSGID.MAIN.CA
    tab.msgMinorId = MSGID.CA.GETTOKEN

    tab.strAccount = data["strAccount"]
    tab.strGameserverId = data["recommendId"]

    sendMsg(self.sock, tab)
end

function Robot:onAddToken(data)
	self.strToken = msgTable["strToken"]

    local tab = clone(CLMsg.CL_EnterGame)

    tab.msgMainId = MSGID.MAIN.CL
    tab.msgMinorId = MSGID.CL.ENTERGAME

    tab.strAccount = msgTable["strAccount"]
    tab.strToken = msgTable["strToken"]
    tab.strGameserverId = msgTable["strGameserverId"]

    sendMsg(self.sock, tab)
end

function Robot:onEnterGame(data)
	if data["isNewPlayer"] == true then

        log("Robot", "onEnterGame : NewPlayer!!!!")
        
        -- open create UI

        -- to get 20 Names!!!
        local tab = clone(CNMsg.CN_GetNames)
        tab.msgMainId = MSGID.MAIN.CN
        tab.msgMinorId = MSGID.CN.GETNAMES
        tab.strAccount = data["strAccount"]
        tab.dwNumbers = 20

        sendMsg(self.sock, tab)
    else
    	log("Robot", "onEnterGame : Error!!!!")
    end
end

function Robot:onCreateResult(data)
	for k,v in pairs(data) do
        log("Robot", "onCreateResult", k, v)
    end
end

function Robot:onGetRecommandNames(data)
    local nSize = #data["arrNames"]
    local nameIndex = math.random(1, nSize)

    local tab = clone(CLMsg.CL_CreatePlayer)
    tab.msgMainId = MSGID.MAIN.CL
    tab.msgMinorId = MSGID.CL.CREATEPLAYER
    tab.strAccount = "finger"
    tab.strGameserverId = "1001"
    tab.dwRoleCfgId = 2

    local strChoosedName = data["arrNames"][nameIndex]

    tab.strPlayerName = strChoosedName
    tab.bIsRecommend = true 

    sendMsg(self.sock, tab)
end

function Robot:onRecvGateMsg(data)
	local stRecvData = mp.unpack(data)

    local dwMsgMainId = stRecvData["msgMainId"]
    local dwMsgMinorId =stRecvData["msgMinorId"]

    log("Robot", string.format("onRecvGateMsg sock: %d mainId: %d minId: %d", self.sock, dwMsgMainId, dwMsgMinorId))

    --[[
    local processFunc = msgRouter:getInstance():getFunction(dwMsgMainId, dwMsgMinorId)
    if processFunc  ~= nil then
        processFunc(self, connectId, stRecvData)
    else
        
        print("onRecvGateMsg ERRORRRRR!!!!!")
    end
    --]]
end


local count = 0
function Robot:run()
    connectTo(G_SERVER_IP, G_SERVER_PORT, function(sock, success)
        count = count + 1
        
		if sock and success then
			self.sock = sock
			gRobotMgr.robots[sock] = self

			setSockMsgHandler(sock, function(data)
				self:onMsg(data)
			end)

			self:login()
			--[[
			scheduleOnce("login" .. sock, math.random(, function()

			end)
			--]]
		else
			log("Robot", count, "connect fail")
            count = count - 1
		end
	end)
    log("Robot", "connectto", count, G_SERVER_IP, G_SERVER_PORT)
end

return Robot
