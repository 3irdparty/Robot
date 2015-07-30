require("framework.functions")
local ClassRuntime = class("ClassRuntime")

function ClassRuntime.ctor()

end

function ClassRuntime.log(a, b)
	--local c = (a+b)*(a-b)
	--return c
end



local TableRuntime = {}

function TableRuntime.ctor()

end

function TableRuntime.log(a, b)
	--local c = (a+b)*(a-b)
	--return c
end

local classRuntime = ClassRuntime.new()

local looptime = 1--10000*1000*50
local t1, t2

t1 = os.time()
for i=1, looptime do
	classRuntime.log(8, 3)
end
t2 = os.time()
print(t2-t1)

local llog = TableRuntime.log
t1 = os.time()
for i=1, looptime do
	TableRuntime.log(8, 3)
	--llog(8, 3)
end
t2 = os.time()
print(t2-t1)

local tab = 
{
	a="a",
	b="b",
	c=10,
	d={
		dd = "dd"
	}	
}
local function get()
	return 
	{
		a="a",
		b="b",
		c=10,
		d=
		{
			dd = "dd"
		}	
	}
end


looptime = 10000*1000
t1 = os.time()
for i=1, looptime do
	local xx = clone(tab)
end

t2 = os.time()
print(t2-t1)
t1 = os.time()

for i=1, looptime do
	local xx = get()
end

t2 = os.time()
print(t2-t1)