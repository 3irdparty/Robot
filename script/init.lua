json = require("ThirdParty/json")
md5 = require("ThirdParty/md5")
mp = require("ThirdParty/MessagePack")

require("framework/log")
require("framework/net")
require("framework/timer")
require("framework/functions")
require("framework/eventCenter")

utf8 = require("framework/utf8")

require("config")

collectgarbage("setpause", Colector.collectpause) 
collectgarbage("setstepmul", Colector.collectinternal)


uu.registHandlers("onMsg", "onTick", "onRemoveSock", "onConnected", "traceback")

---[[
local sockCount = 0
for i=1, G_CLIENT_NUM do
	local sock = connectTo(G_SERVER_IP, G_SERVER_PORT, function(sock, success)
		if sock and success then
			sockCount = sockCount + 1
			scheduleOnece("regist_" .. sock, 1, function()
				local msg = 
				{
					msgMainId   = 3,
			    	msgMinorId  = 1,
			    	strAccount  = "gaofeng" .. sockCount,
			    	strPassword = "222222",
				}
				local str = mp.pack(msg)
			    sendMsg(sock, str)
			end)
		end
		log("init", " on connected: ", sock, success)
	end)

	--log("init", "sock: ", sock)
	--[[
	if sock > 0 then
		sockCount = sockCount + 1
		scheduleOnece("regist_" .. sock, 1, function()
			local msg = 
			{
				msgMainId   = 3,
		    	msgMinorId  = 1,
		    	strAccount  = "gaofeng" .. sockCount,
		    	strPassword = "222222",
			}
			local str = mp.pack(msg)
		    sendMsg(sock, str)
		end)
	end
	--]]
end
--]]

--print("sock: ", uu.connectTo(G_SERVER_IP, G_SERVER_PORT))
