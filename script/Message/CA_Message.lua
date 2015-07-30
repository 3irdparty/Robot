local MessageCA = {}

MessageCA.CA_LoginMsg = 
{
    msgMainId = nil,
    msgMinorId = nil,

    strAccount = "",
    strPassword = "",
}

MessageCA.AC_LoginMsg = 
{
    msgMainId = nil,
    msgMinorId = nil,

    dwErrorCode = 0,
    strAccount = "",
    gameServers = {},
    recommendId = 0,
}

return MessageCA