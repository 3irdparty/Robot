local MessageNG = {}

MessageNG.NG_RegisterMsg = 
{
  msgMainId = nil,
  msgMinorId = nil,
  
  nameServerId = "",
}

MessageNG.GN_RegisterMsg = 
{
    msgMainId = nil,
    msgMinorId = nil,

    dwErrorCode = 0,
}

MessageNG.GN_DeleteName = 
{
    msgMainId = nil,
    msgMinorId = nil, 

    strName = "",
    bIsRecommend = false,
    dwGender = 1,
}

return MessageNG