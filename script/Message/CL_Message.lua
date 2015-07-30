local MessageCL = {}

MessageCL.CL_EnterGame = 
{
    msgMainId = nil,
    msgMinorId = nil,

    strAccount = "",
    strGameserverId = "",
    strToken = "",
}

MessageCL.LC_EnterGame = 
{
    msgMainId = nil,
    msgMinorId = nil,

    dwErrorCode = 0,

    strAccount = "",
    isNewPlayer = "",

    stPlayerInfo = {},
}

MessageCL.CL_CreatePlayer = 
{
    msgMainId = nil, 
    msgMinorId = nil, 

    strAccount = "",
    strGameserverId = "",
    dwRoleCfgId = 0,
    strPlayerName = "",
    bIsRecommend = false,
}

MessageCL.LC_CreatePlayer = 
{
    msgMainId = nil, 
    msgMinorId = nil,

    strAccount = "",
    strGameserverId = "",
    bCreateResult = false,
    strNameUsed = "",
    dwGender = 1,
    bIsRecommend = false,
    stObjectPlayerInfo = {},
}

return MessageCL