local Robot = require("Robots.Robot")

local RobotMgr = class("RobotMgr", sington)

function RobotMgr:ctor()
	self.robots = {}
end

function RobotMgr:run()
	log("RobotMgr", "RobotMgr:run 111")
	for i=1, G_CLIENT_NUM do
		Robot.new():run()
	end
	log("RobotMgr", "RobotMgr:run 222")
end

return RobotMgr