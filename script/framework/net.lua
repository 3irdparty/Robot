gSockFdMap = {}

local connectCB = {}
local msgHandler = {}
local msgRecvCount = 0

function sendMsg(sock, data)
	local msgstr = mp.pack(data)
	uu.createMsg(sock, msgstr)
	uu.sendMsg(sock)
	log("net", "sendMsg ", sock, string.len(msgstr))
end

function onMsg(sock, msgstr)
	msgRecvCount = msgRecvCount + 1
	local data = mp.unpack(msgstr)
	if msgHandler[sock] then
		--msgHandler[sock](data)
	end

	local out = string.format("\n-- onMsg len: " .. string.len(msgstr) .. " sock: %d msgCount: %d: ", sock, msgRecvCount)
	log("net", out .. "\n")
	for k, v in pairs(data) do
		if type(v) ~= "table" then
			out = out .. "\n     " .. k .. " = " .. v
		end
	end

	log("net", out .. "\n")

	return true
end

function setSockMsgHandler(sock, handler)
	msgHandler[sock] = handler
end

function onRemoveSock(sock)
	log("net", " lua onRemoveSock: ", sock)
	unschedule("regist_" .. sock)
	gSockFdMap[sock] = nil
end


function onConnected(sock, success)
	--log("net", " lua onConnected: ", sock, success)
	gSockFdMap[sock] = sock
	if connectCB[sock] then
		connectCB[sock](sock, success)
		connectCB[sock] = nil
	end
end

function connectTo(ip, port, cb)
	local sock = uu.connectTo(ip, port)
	if sock > 0 then
		connectCB[sock] = cb
		return sock
	end
end