local cbs = {}
local cbcnt = 0

function luafunc(cb)
	cbcnt = cbcnt + 1
	cbs[cnt] = cb
end

function execluafunc(cnt, ...)
	cbs[cnt](...)
end

function traceback(msg)
    local msg = debug.traceback(msg, 3)
    log("traceback", msg)
    --return msg
end