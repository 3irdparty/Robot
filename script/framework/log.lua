local logtags =
{
	net = true,
	init = true,
	timer = true,
	eventCenter = true,
}

function log(tag, ...)
	if logtags[tag] then
		print("[lua_print " .. tag .. "]", ...)
	end
end
