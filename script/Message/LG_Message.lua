local MessageLG = {}

MessageLG.LG_RegisterMsg = 
{
  msgMainId = nil,
  msgMinorId = nil,
  
  gameServerId = "",
}

MessageLG.GL_RegisterMsg = 
{
    msgMainId = nil,
    msgMinorId = nil,

    dwErrorCode = 0,
}

MessageLG.GL_SetToken = 
{
    msgMainId = nil,
    msgMinorId = nil,

    strAccount = "",
    strToken = "",
}

return MessageLG