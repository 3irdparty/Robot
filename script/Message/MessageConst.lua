
local MSGID = { 
    MAIN = {
        CL = 1,         --Client         --> LogicServer
        LC = 2,        --LogicServer    --> Client
        CA = 3,         --Clinet         --> AccountServer
        AC = 4,         --AccountServer  --> Clinet
        CG = 5,         --Clinet         --> GateServer
        GC = 6,         --GateServer     --> Clinet
        CCHAT = 7,      --Clinet         --> ChatServer
        CHATC = 8,      --ChatServer     --> Clinet
        GL = 9,         --Gate           --> LogicServer
        LG = 10,        --LogicServer    --> Gate
        AG = 11,        --AccountServer  --> Gate
        GA = 12,        --Gate           --> AccountServer
        CN = 13,        --Client   --> NameServer
        NC = 14,        -- NameServer --> Client
        NG = 15,        -- NameServer --> Gate
        GN = 16,        -- Gate --> NameServer
        BG = 17,        -- BattleServer --> GateServer
        GB = 18,        -- GateServer --> BattelServer
        LN = 19,        --LogicServer --> NameServer
        AL = 20,        --Account     --> LogicServer
    },
    CL = {},   --Client --> LogicServer
    LC = {},
    CA = {},   --Clinet --> AccountServer
    AC = {},
    CG = {},   --Clinet --> GateServer
    GC = {},
    CCHAT = {},   --Clinet --> ChatServer
    CHATC = {},
    GL = {},      --LogicServer    --> Gate
    LG = {},
    AG = {},      --AccountServer  --> Gate
    GA = {},

    CN = {}, 
    NC = {},

    GN = {},
    NG = {},

    BG = {},
    GB = {},

    LN = {},
    AL = {},
}

-------------------------------------------------------------CCCCC -->> GGGGGG --------------------------------------------------------------------------
MSGID.CG.GETTOKEN = 1


-------------------------------------------------------------GGGGGG -->> CCCCC  --------------------------------------------------------------------------

MSGID.GC.SETTOKEN = 1


-------------------------------------------------------------CCCCC -->> AAAAA --------------------------------------------------------------------------

MSGID.CA.LOGIN = 1
MSGID.CA.GETTOKEN = 2

-------------------------------------------------------------AAAAA -->>CCCCC  --------------------------------------------------------------------------

MSGID.AC.LOGIN_RESULT = 1
MSGID.AC.SETTOKEN = 2

-------------------------------------------------------------CCCCC -->> LLLLL --------------------------------------------------------------------------


MSGID.CL.ENTERGAME = 1
MSGID.CL.CREATEPLAYER = 2
-------------------------------------------------------------LLLLL -->> CCCCC --------------------------------------------------------------------------

MSGID.LC.ENTERGAME = 1
MSGID.LC.CREATEPLAYER = 2


--[[
	between GateServer and AccountServer(LoginServer)
]]

-------------------------------------------------------------AAAAA -->> GGGGG --------------------------------------------------------------------------
MSGID.AG.REGIST = 1

-------------------------------------------------------------GGGGG -->> AAAAA --------------------------------------------------------------------------

MSGID.GA.REGIST_RESULT = 1

--[[
	between GateServer and LogicServer(GameServer)
]]
-------------------------------------------------------------LLLLL -->> GGGGG --------------------------------------------------------------------------


MSGID.LG.REGIST = 1

-------------------------------------------------------------GGGGG -->> LLLLL --------------------------------------------------------------------------

MSGID.GL.REGIST_RESULT = 1
MSGID.GL.SETTOKEN = 2



--[[
    client <-----> Nameserver
]]

MSGID.CN.GETNAMES = 1
MSGID.NC.GETNAMES = 2

--[[
    Nameserver <-----> GateServer
]]
MSGID.NG.REGIST = 1
MSGID.GN.REGIST = 2

MSGID.NG.DELETE = 3
MSGID.GN.DELETE = 4

--[[
    Battelserver <------> GateServer
]]

MSGID.BG.REGIST = 1
MSGID.GB.REGIST = 2

-------------------------------------------------------------Logic -->> Name --------------------------------------------------------------------------
MSGID.LN.DELETENAME = 1

-------------------------------------------------------------Account -->> Logic --------------------------------------------------------------------------
MSGID.AL.SETTOKEN = 1

return MSGID

