local ostime = os.time

local timers = {}


function unschedule(name)
	timers[name] = nil
end

function schedule(name, delay, internal, cb)
	timers[name] = 
	{
		start = ostime(),
		delay = delay,
		internal = internal,
		cb = cb,
		active = false,
		calledtimes = 0,
	}
end

function scheduleOnece(name, delay, cb)
	log("timer", name, delay, cb)
	schedule(name, delay, 0, function()
		cb()
		unschedule(name)
	end)
end

function timerTick()
	--print("[lua_print] timerTick 111")
	local currtime = ostime()
	for name, timer in pairs(timers) do
		--log("timer", "timer name: ", name)
		if not timer.active then
			if (currtime - timer.start) > timer.delay then
				timer.start = currtime
				timer.cb()
				timer.active = true
				timer.calledtimes = timer.calledtimes + 1
				log("timer", name, " be activated, run times: ", timer.calledtimes)
			end
		else
			if (currtime - timer.start) > timer.internal then
				timer.start = currtime
				timer.cb()
				timer.calledtimes = timer.calledtimes + 1
				log("timer", name, " run times: ", timer.calledtimes)
			end
		end
		
	end
	--print("[lua_print] timerTick 222")
end

function onTick()
	timerTick()
end