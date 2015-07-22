gSockFdMap = {}

function sendMsg(sock, msgstr)
	uu.createMsg(sock, msgstr)
	uu.sendMsg(sock)
end

local msgRecvCount = 0
function onMsg(sock, msgstr)
	msgRecvCount = msgRecvCount + 1
	local msg = mp.unpack(msgstr)
	
	local out = string.format("\n  -- onMsg sock: %d msgCount: %d: ", sock, msgRecvCount)
	for k, v in pairs(msg) do
		out = out .. "\n     " .. k .. " = " .. v
	end
	log("net", out)
	return true
end



function onRemoveSock(sock)
	log("net", " lua onRemoveSock: ", sock)
	gSockFdMap[sock] = nil
end

local n = 0
function onConnected(sock)
	log("net", " lua onConnected: ", sock)
	gSockFdMap[sock] = sock
	n = n + 1
	if n < G_CLIENT_NUM then
		--print("sock: ", uu.connectTo(G_SERVER_IP, G_SERVER_PORT))
	else

	end
	--[[
	local msg = 
	{
		msgMainId   = 3,
    	msgMinorId  = 1,
    	strAccount  = "finger",
    	strPassword = "123456",
	}
	local str = mp.pack(msg)
    sendMsg(sock, str)
	--]]
end