local logtags =
{
	net = true,
	init = true,
	timer = true,
	traceback = true,
	eventCenter = true,
}

function log(tag, ...)
	if logtags[tag] then
		print("[lua_print | " .. tag .. "]", ...)
	end
end
