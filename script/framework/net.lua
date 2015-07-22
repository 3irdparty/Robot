gSockFdMap = {}

local connectCB = {}

function sendMsg(sock, msgstr)
	uu.createMsg(sock, msgstr)
	uu.sendMsg(sock)
	log("net", "sendMsg ", sock, string.len(msgstr))
end

local msgRecvCount = 0
function onMsg(sock, msgstr)
	msgRecvCount = msgRecvCount + 1
	local msg = mp.unpack(msgstr)
	
	local out = string.format("\n  -- onMsg sock: %d msgCount: %d: ", sock, msgRecvCount)
	for k, v in pairs(msg) do
		if type(v) ~= "table" then
			out = out .. "\n     " .. k .. " = " .. v
		end
	end
	--out = out .. table.concat(msg)
	log("net", out)

	return true
end

function onRemoveSock(sock)
	log("net", " lua onRemoveSock: ", sock)
	gSockFdMap[sock] = nil
end


function onConnected(sock, success)
	--log("net", " lua onConnected: ", sock, success)
	gSockFdMap[sock] = sock
	if connectCB[sock] then
		connectCB[sock](sock, success)
	end
end

function connectTo(ip, port, cb)
	local sock = uu.connectTo(ip, port)
	if sock > 0 then
		connectCB[sock] = cb
		return sock
	end
end