eventCenter = {}
local events = {}


function eventCenter:new(name, event, cb)
	if events[name] then
		log("eventCenter", " Error: eventCenter new, listener already exists!")
		return
	end
	events[name] = {event=event, cb=cb}
end

function eventCenter:delete(name)
	events[name] = nil
end

function eventCenter:dispatch(event, ...)
	for _, v in pairs(events) do
		if event == v.event then
			v.cb(event, ...)
		end
	end
end

return eventCenter