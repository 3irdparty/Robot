local MessageCG = {}

MessageCG.CG_GetToken = 
{
    msgMainId  = nil,
    msgMinorId = nil,
    
    strAccount      = "",
    strGameserverId = "",
}

MessageCG.GC_GetToken = 
{
    msgMainId  = nil,
    msgMinorId = nil,

    strToken = "",
}

MessageCG.GC_SetToken = 
{
    msgMainId = nil,
    msgMinorId = nil,

    strAccount = "",
    strToken = "",
    strGameserverId = "",
}

return MessageCG