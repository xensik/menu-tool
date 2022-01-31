// Copyright 2023 xensik. All rights reserved.
//
// Use of this source code is governed by a GNU GPLv3 license
// that can be found in the LICENSE file.

#pragma once

namespace xsk::menu::iw5
{

/* window dynamic flags */
//#define WINDOWDYNAMIC_UNK_1               0x00000001
#define WINDOWDYNAMIC_HASFOCUS		        0x00000002
#define WINDOWDYNAMIC_VISIBLE		        0x00000004
#define WINDOWDYNAMIC_FADEOUT		        0x00000010
#define WINDOWDYNAMIC_FADEIN		        0x00000020
//#define WINDOWDYNAMIC_UNK_40              0x00000040
//#define WINDOWDYNAMIC_UNK_80              0x00000080
#define WINDOWDYNAMIC_CLOSED		        0x00000800
//#define WINDOWDYNAMIC_UNK_2000            0x00002000
#define WINDOWDYNAMIC_BACKCOLOR		        0x00008000
#define WINDOWDYNAMIC_FORECOLOR		        0x00010000

/* window static flags */
#define WINDOWSTATIC_DECORATION				0x00100000
#define WINDOWSTATIC_HORIZONTALSCROLL		0x00200000
#define WINDOWSTATIC_SCREENSPACE			0x00400000
#define WINDOWSTATIC_AUTOWRAPPED			0x00800000
#define WINDOWSTATIC_POPUP					0x01000000
#define WINDOWSTATIC_OUTOFBOUNDSCLICK		0x02000000
#define WINDOWSTATIC_LEGACYSPLITSCREENSCALE	0x04000000
#define WINDOWSTATIC_HIDDENDURINGFLASH		0x10000000
#define WINDOWSTATIC_HIDDENDURINGSCOPE		0x20000000
#define WINDOWSTATIC_HIDDENDURINGUI			0x40000000
#define WINDOWSTATIC_TEXTONLYFOCUS			0x80000000

struct ParseContext
{
    bool valid;
    bool needsImplicitSize;
    int impicitSizeRez;
    char horzRefPoint;
    char vertRefPoint;
};

typedef float vec4_t[4];

struct Material
{
    const char *name;
};

struct snd_alias_list_t
{
    const char *aliasName;
};

struct MenuEventHandlerSet;
struct Statement_s;
struct menuDef_t;

enum operationEnum
{
    OP_NOOP = 0x0,
    OP_RIGHTPAREN = 0x1,
    OP_MULTIPLY = 0x2,
    OP_DIVIDE = 0x3,
    OP_MODULUS = 0x4,
    OP_ADD = 0x5,
    OP_SUBTRACT = 0x6,
    OP_NOT = 0x7,
    OP_LESSTHAN = 0x8,
    OP_LESSTHANEQUALTO = 0x9,
    OP_GREATERTHAN = 0xA,
    OP_GREATERTHANEQUALTO = 0xB,
    OP_EQUALS = 0xC,
    OP_NOTEQUAL = 0xD,
    OP_AND = 0xE,
    OP_OR = 0xF,
    OP_LEFTPAREN = 0x10,
    OP_COMMA = 0x11,
    OP_BITWISEAND = 0x12,
    OP_BITWISEOR = 0x13,
    OP_BITWISENOT = 0x14,
    OP_BITSHIFTLEFT = 0x15,
    OP_BITSHIFTRIGHT = 0x16,
    OP_STATICDVARINT = 0x17,
    OP_FIRSTFUNCTIONCALL = 0x17,
    OP_STATICDVARBOOL = 0x18,
    OP_STATICDVARFLOAT = 0x19,
    OP_STATICDVARSTRING = 0x1A,
    OP_TOINT = 0x1B,
    OP_TOSTRING = 0x1C,
    OP_TOFLOAT = 0x1D,
    LAST_COMMONLY_CALLED_FUNCTION = 0x1D,
    OP_SIN = 0x1E,
    OP_COS = 0x1F,
    OP_MIN = 0x20,
    OP_MAX = 0x21,
    OP_MILLISECONDS = 0x22,
    OP_LOCALCLIENTUIMILLISECONDS = 0x23,
    OP_DVARINT = 0x24,
    OP_DVARBOOL = 0x25,
    OP_DVARFLOAT = 0x26,
    OP_DVARSTRING = 0x27,
    OP_UIACTIVE = 0x28,
    OP_FLASHBANGED = 0x29,
    OP_USINGVEHICLE = 0x2A,
    OP_MISSILECAM = 0x2B,
    OP_SCOPED = 0x2C,
    OP_SCOPEDTHERMAL = 0x2D,
    OP_SCOREBOARDVISIBLE = 0x2E,
    OP_INKILLCAM = 0x2F,
    OP_INKILLCAM_NPC = 0x30,
    OP_PLAYERFIELD = 0x31,
    OP_GET_PLAYER_PERK = 0x32,
    OP_SELECTINGLOCATION = 0x33,
    OP_SELECTINGDIRECTION = 0x34,
    OP_TEAMFIELD = 0x35,
    OP_OTHERTEAMFIELD = 0x36,
    OP_MARINESFIELD = 0x37,
    OP_OPFORFIELD = 0x38,
    OP_MENUISOPEN = 0x39,
    OP_WRITINGDATA = 0x3A,
    OP_INLOBBY = 0x3B,
    OP_INGAMELOBBY = 0x3C,
    OP_INPRIVATEPARTY = 0x3D,
    OP_PRIVATEPARTYHOST = 0x3E,
    OP_PRIVATEPARTYHOSTINLOBBY = 0x3F,
    OP_ALONEINPARTY = 0x40,
    OP_ADSJAVELIN = 0x41,
    OP_WEAPLOCKBLINK = 0x42,
    OP_WEAPATTACKTOP = 0x43,
    OP_WEAPATTACKDIRECT = 0x44,
    OP_WEAPLOCKING = 0x45,
    OP_WEAPLOCKED = 0x46,
    OP_WEAPLOCKTOOCLOSE = 0x47,
    OP_WEAPLOCKSCREENPOSX = 0x48,
    OP_WEAPLOCKSCREENPOSY = 0x49,
    OP_SECONDSASTIME = 0x4A,
    OP_TABLELOOKUP = 0x4B,
    OP_TABLELOOKUPBYROW = 0x4C,
    OP_TABLEGETROWNUM = 0x4D,
    OP_LOCALIZESTRING = 0x4E,
    OP_LOCALVARINT = 0x4F,
    OP_LOCALVARBOOL = 0x50,
    OP_LOCALVARFLOAT = 0x51,
    OP_LOCALVARSTRING = 0x52,
    OP_TIMELEFT = 0x53,
    OP_SECONDSASCOUNTDOWN = 0x54,
    OP_GAMEMSGWNDACTIVE = 0x55,
    OP_GAMETYPENAME = 0x56,
    OP_GAMETYPE = 0x57,
    OP_GAMETYPEDESCRIPTION = 0x58,
    OP_SCORE = 0x59,
    OP_FOLLOWING = 0x5A,
    OP_SPECTATINGFREE = 0x5B,
    OP_KEYBINDING = 0x5C,
    OP_ACTIONSLOTUSABLE = 0x5D,
    OP_HUDFADE = 0x5E,
    OP_MAXPLAYERS = 0x5F,
    OP_ACCEPTINGINVITE = 0x60,
    OP_ISINTERMISSION = 0x61,
    OP_GAMEHOST = 0x62,
    OP_PARTYHASMISSINGMAPPACK = 0x63,
    OP_PARTYMISSINGMAPPACKERROR = 0x64,
    OP_ANYNEWMAPPACKS = 0x65,
    OP_AMISELECTED = 0x66,
    OP_PARTYSTATUSSTRING = 0x67,
    OP_ATTACHED_CONTROLLER_COUNT = 0x68,
    OP_IS_SPLIT_SCREEN_ONLINE_POSSIBLE = 0x69,
    OP_SPLITSCREENPLAYERCOUNT = 0x6A,
    OP_GETPLAYERDATA = 0x6B,
    OP_GETPLAYERDATASPLITSCREEN = 0x6C,
    OP_GET_MATCHRULES_DATA = 0x6D,
    OP_GET_SAVED_MATCHRULES_METADATA = 0x6E,
    OP_LEVEL_FOR_EXPERIENCE_MP = 0x6F,
    OP_LEVEL_FOR_EXPERIENCE_SO = 0x70,
    OP_IS_ITEM_UNLOCKED = 0x71,
    OP_IS_ITEM_UNLOCKEDSPLITSCREEN = 0x72,
    OP_IS_CARDICON_UNLOCKED = 0x73,
    OP_IS_CARDTITLE_UNLOCKED = 0x74,
    OP_IS_CARDICON_NEW = 0x75,
    OP_IS_CARDTITLE_NEW = 0x76,
    OP_IS_CARDICON_UNLOCKED_SPLITSCREEN = 0x77,
    OP_IS_CARDTITLE_UNLOCKED_SPLITSCREEN = 0x78,
    OP_IS_CARDICON_NEW_SPLITSCREEN = 0x79,
    OP_IS_CARDTITLE_NEW_SPLITSCREEN = 0x7A,
    OP_IS_PROFILEITEM_UNLOCKED = 0x7B,
    OP_IS_PROFILEITEM_UNLOCKED_SPLITSCREEN = 0x7C,
    OP_IS_PROFILEITEM_NEW = 0x7D,
    OP_IS_PROFILEITEM_NEW_SPLITSCREEN = 0x7E,
    OP_DEBUG_PRINT = 0x7F,
    OP_GETPLAYERDATA_ANYBOOLTRUE = 0x80,
    OP_GETPROFILE_ANYBOOLTRUE = 0x81,
    OP_WEAPON_CLASS_NEW = 0x82,
    OP_WEAPONNAME = 0x83,
    OP_ISRELOADING = 0x84,
    OP_SAVE_GAME_AVAILABLE = 0x85,
    OP_UNLOCKED_ITEM_COUNT = 0x86,
    OP_UNLOCKED_ITEM_COUNT_SPLITSCREEN = 0x87,
    OP_UNLOCKED_ITEM = 0x88,
    OP_UNLOCKED_ITEM_SPLITSCREEN = 0x89,
    OP_RADAR_IS_JAMMED = 0x8A,
    OP_RADAR_JAM_INTENSITY = 0x8B,
    OP_RADAR_IS_ENABLED = 0x8C,
    OP_EMP_JAMMED = 0x8D,
    OP_PLAYERADS = 0x8E,
    OP_WEAPON_HEAT_ACTIVE = 0x8F,
    OP_WEAPON_HEAT_VALUE = 0x90,
    OP_WEAPON_HEAT_OVERHEATED = 0x91,
    OP_SPLASH_TEXT = 0x92,
    OP_SPLASH_DESCRIPTION = 0x93,
    OP_SPLASH_MATERIAL = 0x94,
    OP_SPLASH_HAS_ICON = 0x95,
    OP_SPLASH_ROWNUM = 0x96,
    OP_GETFOCUSED_NAME = 0x97,
    OP_GETFOCUSED_X = 0x98,
    OP_GETFOCUSED_Y = 0x99,
    OP_GETFOCUSED_W = 0x9A,
    OP_GETFOCUSED_H = 0x9B,
    OP_GETITEMDEF_X = 0x9C,
    OP_GETITEMDEF_Y = 0x9D,
    OP_GETITEMDEF_W = 0x9E,
    OP_GETITEMDEF_H = 0x9F,
    OP_PLAYLISTFIELD = 0xA0,
    OP_SCOREBOARD_EXTERNALMUTE_NOTICE = 0xA1,
    OP_CLIENT_MATCH_DATA = 0xA2,
    OP_CLIENT_MATCH_DATA_DEF = 0xA3,
    OP_GET_MAP_NAME = 0xA4,
    OP_GET_MAP_IMAGE = 0xA5,
    OP_GET_MAP_CUSTOM = 0xA6,
    OP_GET_MIGRATION_STATUS = 0xA7,
    OP_GET_PLAYERCARD_INFO = 0xA8,
    OP_IS_OFFLINE_PROFILE_SELECTED = 0xA9,
    OP_COOP_PLAYERFIELD = 0xAA,
    OP_IS_COOP = 0xAB,
    OP_GETPARTYSTATUS = 0xAC,
    OP_GETSEARCHPARAMS = 0xAD,
    OP_GETTIMEPLAYED = 0xAE,
    OP_IS_SELECTED_PLAYER_FRIEND = 0xAF,
    OP_GETCHARBYINDEX = 0xB0,
    OP_GETPLAYERPROFILEDATA = 0xB1,
    OP_GETPLAYERPROFILEDATASPLITSCREEN = 0xB2,
    OP_IS_PROFILE_SIGNED_IN = 0xB3,
    OP_GET_WAIT_POPUP_STATUS = 0xB4,
    OP_GETNATTYPE = 0xB5,
    OP_GETLOCALIZEDNATTYPE = 0xB6,
    OP_GET_ADJUSTED_SAFEAREA_HORIZONTAL = 0xB7,
    OP_GET_ADJUSTED_SAFEAREA_VERTICAL = 0xB8,
    OP_CONNECTION_INFO = 0xB9,
    OP_OFFLINE_PROFILE_CAN_SAVE = 0xBA,
    OP_USER_WITHOUT_OFFLINE_PROFILE = 0xBB,
    OP_ALL_SPLITSCREEN_PROFILES_CAN_SAVE = 0xBC,
    OP_ALL_SPLITSCREEN_PROFILES_ARE_SIGNED_IN = 0xBD,
    OP_DO_WE_HAVE_MAP_PACK = 0xBE,
    OP_MAY_INVITE_PLAYER_TO_PARTY = 0xBF,
    OP_GETPATCHNOTES = 0xC0,
    OP_GETGAMEINFOS = 0xC1,
    OP_COOP_READY = 0xC2,
    OP_VOTE_CAST = 0xC3,
    OP_VOTE_PASSED = 0xC4,
    OP_GET_MAP_VOTE_MAP_IMAGE = 0xC5,
    OP_GET_MAP_VOTE_MAP_NAME = 0xC6,
    OP_GET_MAP_VOTE_GAME_TYPE_NAME = 0xC7,
    OP_IS_FRIEND_INVITABLE = 0xC8,
    OP_IS_FRIEND_JOINABLE = 0xC9,
    OP_GET_SORTED_CHALLENGE_INDEX = 0xCA,
    OP_GET_SORTED_CHALLENGE_NAME = 0xCB,
    OP_GET_SORTED_CHALLENGE_COUNT = 0xCC,
    OP_GET_FILTER_CHALLENGE_COUNT = 0xCD,
    OP_GET_FILTER_CHALLENGE_LOCKED_COUNT = 0xCE,
    OP_GET_FILTER_CHALLENGE_COMPLETE_COUNT = 0xCF,
    OP_IS_SORTED_CHALLENGE_TIERED = 0xD0,
    OP_GET_CHALLENGE_FILTER_CACHE_COUNT = 0xD1,
    OP_GET_CHALLENGE_FILTER_CACHE_COMPLETE_COUNT = 0xD2,
    OP_IS_COOP_SEARCHING = 0xD3,
    OP_IS_COOP_PUBLIC = 0xD4,
    OP_GET_COOP_DISPLAYABLE_GROUP_NUM = 0xD5,
    OP_COOP_HAS_REQUIRED_ONLINE_FILES = 0xD6,
    OP_GET_TEXTWIDTH = 0xD7,
    OP_GET_TEXTHEIGHT = 0xD8,
    OP_DEVELOPER = 0xD9,
    OP_IS_USING_AIRBURST_WEAPON = 0xDA,
    OP_GET_AIRBURST_METERS = 0xDB,
    OP_GET_CROSSHAIR_TRACE_METERS = 0xDC,
    OP_GET_FACEBOOK_STATUS_TEXT = 0xDD,
    OP_IS_FACEBOOK_LOGGED_IN = 0xDE,
    OP_IS_FACEBOOK_CHECKING = 0xDF,
    OP_IS_FACEBOOK_ALLOWED = 0xE0,
    OP_GETPRIVATELOBBYSTATUS = 0xE1,
    OP_INCLUDEDINMAPROTATION = 0xE2,
    OP_SELECT = 0xE3,
    OP_IS_DEMO_PLAYING = 0xE4,
    OP_GET_USER_GROUP_TEXT = 0xE5,
    OP_GET_USER_GROUP_COMMON_INTEREST_TOTAL = 0xE6,
    OP_IS_DEMO_FOLLOW_CAMERA = 0xE7,
    OP_IS_DEMO_FREE_CAMERA = 0xE8,
    OP_IS_DEMO_CAPTURING_SCREENSHOT = 0xE9,
    OP_PARTY_HOST_WAITING_ON_MEMBERS = 0xEA,
    OP_POPUP_PARTY_MEMBER_AWAY = 0xEB,
    OP_SELECTED_PARTY_MEMBER_AWAY = 0xEC,
    OP_GAMETIME = 0xED,
    OP_GAMEENDTIME = 0xEE,
    OP_HAS_FOCUS = 0xEF,
    OP_MENU_HAS_FOCUS = 0xF0,
    OP_GET_DEMO_SEGMENT_COUNT = 0xF1,
    OP_GET_DEMO_SEGMENT_INFORMATION = 0xF2,
    OP_IS_CLIP_MODIFIED = 0xF3,
    OP_IS_USING_RECIPE = 0xF4,
    OP_IS_GUEST = 0xF5,
    OP_GET_FACEBOOK_HELP_TEXT = 0xF6,
    OP_IS_ELITE_CLAN_ALLOWED = 0xF7,
    OP_IS_ENTITLEMENTS_ALLOWED = 0xF8,
    OP_IS_USERGROUPS_ALLOWED = 0xF9,
    OP_IS_WAITING_FOR_ONLINE_SERVICES = 0xFA,
    OP_GET_TEXTWIDTHMODCASE = 0xFB,
    OP_GET_SAVE_SCREEN_TITLE = 0xFC,
    OP_GET_SAVE_SCREEN_DESCRIPTION = 0xFD,
    OP_GET_ONLINEVAULT_SELECTEDITEM_DATA = 0xFE,
    OP_ONLINEVAULT_IS_RESTRICTED = 0xFF,
    OP_IS_CONTENTSERVER_TASK_IN_PROGRESS = 0x100,
    OP_IS_CONTENTSERVER_GET_TASK_PROGRESS = 0x101,
    OP_GET_RECENTGAMES_SELECTEDITEM_DATA = 0x102,
    OP_GAMETYPENAME_ABBREVIATED = 0x103,
    OP_GET_MAP_VOTE_GAME_TYPE_NAME_ABBREVIATED = 0x104,
    OP_IS_USER_SIGNED_IN_TO_LIVE = 0x105,
    OP_USER_CAN_PLAY_ONLINE = 0x106,
    OP_GET_FEEDER_DATA = 0x107,
    OP_PARTY_CLIENTS_UP_TO_DATE = 0x108,
    OP_TRUNCATETEXTWITHELLIPSIS = 0x109,
    OP_UI_STARTED = 0x10A,
    OP_CAN_RENDER_CLIP = 0x10B,
    OP_GET_PREVIEW_MAP_CUSTOM = 0x10C,
    OP_GET_DLC_MAPS_AVAILABLE_COUNT = 0x10D,
    OP_IS_USER_SIGNED_IN = 0x10E,
    OP_USINGINTERMISSIONTIMER = 0x10F,
    OP_ISUSINGCUSTOMMAPROTATION = 0x110,
    OP_MENU_IS_TOPMOST = 0x111,
    OP_FACEBOOK_IS_PLATFORM_FRIEND = 0x112,
    OP_ELITE_CLAN_IS_PLATFORM_FRIEND = 0x113,
    OP_ELITE_CLAN_IS_ME = 0x114,
    OP_ELITE_CLAN_IS_LEADER = 0x115,
    OP_IS_USER_SIGNED_IN_FOR_VAULT = 0x116,
    OP_GET_USING_MATCHRULES_DATA = 0x117,
    OP_CAN_USER_ACCESS_ONLINEVAULT = 0x118,
    OP_FRIEND_GET_GAMERTAG = 0x119,
    OP_RECENTPLAYER_GET_GAMERTAG = 0x11A,
    OP_LIVEPARTY_GET_GAMERTAG = 0x11B,
    OP_FACEBOOK_GET_GAMERTAG = 0x11C,
    OP_ELITECLAN_GET_GAMERTAG = 0x11D,
    OP_LIVEPARTY_IS_ME = 0x11E,
    OP_LIVEPARTY_IS_LOCAL = 0x11F,
    OP_DOUBLECLICK_WAS_RIGHT_CLICK = 0x120,
    OP_IS_DEMO_CLIP_RECORDING = 0x121,
    OP_GET_INDEX_FROM_STRING = 0x122,
    OP_GET_STRING_WIHTOUT_INDEX = 0x123,
    OP_ELITECLAN_GET_NAME = 0x124,
    OP_ELITECLAN_GET_HELP = 0x125,
    OP_ELITECLAN_GET_MOTD = 0x126,
    OP_ELITECLAN_IS_MEMBER = 0x127,
    OP_ELITECLAN_IS_EMBLEM_OK = 0x128,
    OP_FACEBOOKFRIENDS_SHOW_NEXT = 0x129,
    OP_FACEBOOKFRIENDS_SHOW_PREV = 0x12A,
    OP_GET_ONLINEVAULT_FRIEND_GAMERTAG = 0x12B,
    OP_GET_OBJECTIVE_LIST_HEIGHT = 0x12C,
    OP_IS_CLIENT_DEMO_ENABLED = 0x12D,
    OP_IS_USER_SIGNED_IN_TO_DEMONWARE = 0x12E,
    OP_IS_CUSTOM_CLASS_RESTRICTED = 0x12F,
    OP_IS_WEAPON_RESTRICTED = 0x130,
    OP_ANY_SPLITSCREEN_PROFILES_ARE_SIGNED_IN = 0x131,
    OP_IS_GUEST_SPLITSCREEN = 0x132,
    OP_IS_ITEM_UNLOCKED_BY_CLIENT = 0x133,
    OP_IS_ANY_USER_SIGNED_IN_TO_LIVE = 0x134,
    OP_SPLITSCREENACTIVEGAMEPADCOUNT = 0x135,
    OP_SHOW_FRIEND_PLAYERCARD = 0x136,
    OP_GET_FRIEND_PLAYERCARD_PRESENCE = 0x137,
    OP_SHOW_RECENT_PLAYERS_GROUP_ICON = 0x138,
    OP_GET_WRAPPED_TEXT_HEIGHT = 0x139,
    OP_CAN_SAVE = 0x13A,
    OP_GET_GAME_INVITES_COUNT = 0x13B,
    OP_IS_SPLITSCREEN_GAMER_LIVE_ENABLED = 0x13C,
    OP_SO_COOP_SHOW_COMMON_GROUP_ICON = 0x13D,
    OP_STRIP_COLORS_FROM_STRING = 0x13E,
    OP_CAN_USER_ACCESS_THEATRE = 0x13F,
    OP_IS_CHALLENGE_PERIODIC = 0x140,
    OP_GET_CHALLENGE_DATA = 0x141,
    OP_IS_ELITE_APP_PRESENT = 0x142,
    OP_ELITE_CLAN_SELECTED_IS_ME = 0x143,
    OP_ENOUGH_STORAGE_SPACE_FOR_CLIENT_DEMO = 0x144,
    OP_IS_USER_SIGNED_IN_FOR_COMMERCE = 0x145,
    OP_GET_FACEBOOK_MENU_TEXT = 0x146,
    OP_GET_FACEBOOK_IS_POSTING = 0x147,
    OP_MEET_PLAYER_IS_PLATFORM_FRIEND = 0x148,
    OP_IS_SELECTED_PLAYER_GUEST = 0x149,
    OP_GET_SPLITSCREEN_CONTROLLER_CLIENT_NUM = 0x14A,
    OP_IS_CLIENT_DEMO_ENABLED_SPLITSCREEN = 0x14B,
    OP_ITEM_CAN_TAKE_FOCUS = 0x14C,
    OP_GET_TIME_SINCE_LAST_DOUBLECLICK = 0x14D,
    OP_IS_SERVER_LIST_REFRESHING = 0x14E,
    OP_IS_RECIPE_NAME_VALID = 0x14F,
    OP_RECIPE_EXISTS = 0x150,
    OP_GET_FACEBOOK_OPTIONS_HELP_TEXT = 0x151,
    OP_DO_WE_HAVE_ALL_MAP_PACKS = 0x152,
    OP_IS_THERE_NEW_ELITE_ITEMS = 0x153,
    OP_IS_PAYING_SUBSCRIBER = 0x154,
    OP_LOCALUSER_HASMISSINGMAPPACK = 0x155,
    OP_LOCALUSER_MISSINGMAPPACKERROR = 0x156,
    OP_GET_FIRST_SPECOPS_DLC_MAP = 0x157,
    OP_LOCALUSER_MISSINGMAPNAME = 0x158,
    OP_SHOW_STORE_NEW = 0x159,
    OP_USINGBOTS = 0x15A,
    OP_BOTSALLOWED = 0x15B,
    OP_ISCOLORBLIND = 0x15C,
    OP_BOTSCONNECTTYPE = 0x15D,
    OP_BOTSDIFFICULTY = 0x15E,
    OP_SPECTATINGTHIRD = 0x15F,
    OP_GETPLAYERNAME = 0x160,
    OP_SQUAD_SHOW_REPORT = 0x161,
    OP_GETRANKEDPLAYERDATA = 0x162,
    OP_GETPRIVATEPLAYERDATA = 0x163,
    OP_GETCOOPPLAYERDATA = 0x164,
    OP_GETCOMMONPLAYERDATA = 0x165,
    OP_GETRANKEDPLAYERDATASPLITSCREEN = 0x166,
    OP_GETPRIVATEPLAYERDATASPLITSCREEN = 0x167,
    OP_GETCOOPPLAYERDATASPLITSCREEN = 0x168,
    OP_GETCOMMONPLAYERDATASPLITSCREEN = 0x169,
    NUM_OPERATORS = 0x16A,
};

enum expDataType
{
    VAL_INT = 0x0,
    VAL_FLOAT = 0x1,
    VAL_STRING = 0x2,
    NUM_INTERNAL_DATATYPES = 0x3,
    VAL_FUNCTION = 0x3,
    NUM_DATATYPES = 0x4,
};

struct ExpressionString
{
    const char *string;
};

union operandInternalDataUnion
{
    int intVal;
    float floatVal;
    ExpressionString stringVal;
    Statement_s *function;
};

struct Operand
{
    expDataType dataType;
    operandInternalDataUnion internals;
};

union entryInternalData
{
    operationEnum op;
    Operand operand;
};

struct expressionEntry
{
    entryInternalData data;
    int type;
};

struct UIFunctionList
{
    int totalFunctions;
    Statement_s **functions;
};

struct StaticDvar
{
    void *dvar; // dvar_t
    char *dvarName;
};

struct StaticDvarList
{
    int numStaticDvars;
    StaticDvar **staticDvars;
};

struct StringList
{
    int totalStrings;
    const char **strings;
};

struct ExpressionSupportingData
{
    UIFunctionList uifunctions;
    StaticDvarList staticDvarList;
    StringList uiStrings;
};

struct ExpressionPersistentState
{
    int flags;
    int playerDataKey[4];
    int lastExecuteTime[4];
    Operand lastResult[4];
};

struct Statement_s
{
    int numEntries;
    expressionEntry *entries;
    ExpressionSupportingData *supportingData;
    ExpressionPersistentState persistentState;
};

struct SetLocalVarData
{
    const char *localVarName;
    Statement_s *expression;
};

struct ConditionalScript
{
    MenuEventHandlerSet *eventHandlerSet;
    Statement_s *eventExpression;
};

union EventData
{
    const char *unconditionalScript;
    ConditionalScript *conditionalScript;
    MenuEventHandlerSet *elseScript;
    SetLocalVarData *setLocalVarData;
};

enum EventType : char
{
    EVENT_UNCONDITIONAL = 0x0,
    EVENT_IF = 0x1,
    EVENT_ELSE = 0x2,
    EVENT_SET_LOCAL_VAR_BOOL = 0x3,
    EVENT_SET_LOCAL_VAR_INT = 0x4,
    EVENT_SET_LOCAL_VAR_FLOAT = 0x5,
    EVENT_SET_LOCAL_VAR_STRING = 0x6,
    EVENT_COUNT = 0x7,
};

struct MenuEventHandler
{
    EventData eventData;
    EventType eventType;
};

struct MenuEventHandlerSet
{
    int eventHandlerCount;
    MenuEventHandler **eventHandlers;
};

struct ItemKeyHandler
{
    int key;
    MenuEventHandlerSet *action;
    ItemKeyHandler *next;
};

enum ItemFloatExpressionTarget
{
    ITEM_FLOATEXP_TGT_RECT_X = 0x0,
    ITEM_FLOATEXP_TGT_RECT_Y = 0x1,
    ITEM_FLOATEXP_TGT_RECT_W = 0x2,
    ITEM_FLOATEXP_TGT_RECT_H = 0x3,
    ITEM_FLOATEXP_TGT_FORECOLOR_R = 0x4,
    ITEM_FLOATEXP_TGT_FORECOLOR_G = 0x5,
    ITEM_FLOATEXP_TGT_FORECOLOR_B = 0x6,
    ITEM_FLOATEXP_TGT_FORECOLOR_RGB = 0x7,
    ITEM_FLOATEXP_TGT_FORECOLOR_A = 0x8,
    ITEM_FLOATEXP_TGT_GLOWCOLOR_R = 0x9,
    ITEM_FLOATEXP_TGT_GLOWCOLOR_G = 0xA,
    ITEM_FLOATEXP_TGT_GLOWCOLOR_B = 0xB,
    ITEM_FLOATEXP_TGT_GLOWCOLOR_RGB = 0xC,
    ITEM_FLOATEXP_TGT_GLOWCOLOR_A = 0xD,
    ITEM_FLOATEXP_TGT_BACKCOLOR_R = 0xE,
    ITEM_FLOATEXP_TGT_BACKCOLOR_G = 0xF,
    ITEM_FLOATEXP_TGT_BACKCOLOR_B = 0x10,
    ITEM_FLOATEXP_TGT_BACKCOLOR_RGB = 0x11,
    ITEM_FLOATEXP_TGT_BACKCOLOR_A = 0x12,
    ITEM_FLOATEXP_TGT__COUNT = 0x13,
};

struct ItemFloatExpression
{
    int target;
    Statement_s *expression;
};

struct columnInfo_s
{
    int xpos;
    int ypos;
    int width;
    int height;
    int maxChars;
    int alignment;
};

struct listBoxDef_s
{
    int startPos[1];
    int endPos[1];
    int drawPadding;
    float elementWidth;
    float elementHeight;
    int elementStyle;
    int numColumns;
    columnInfo_s columnInfo[16];
    MenuEventHandlerSet *onDoubleClick;
    int notselectable;
    int noScrollBars;
    int usePaging;
    float selectBorder[4];
    Material *selectIcon;
    Statement_s *elementHeightExp;
};

struct editFieldDef_s
{
    float minVal;
    float maxVal;
    float stepVal;
    float range;
    int maxChars;
    int maxCharsGotoNext;
    int maxPaintChars;
    int paintOffset;
};

struct multiDef_s
{
    const char *dvarList[32];
    const char *dvarStr[32];
    float dvarValue[32];
    int count;
    int strDef;
};

struct newsTickerDef_s
{
    int feedId;
    int speed;
    int spacing;
};

struct textScrollDef_s
{
    int startTime;
};

union itemDefData_t
{
    listBoxDef_s *listBox;
    editFieldDef_s *editField;
    multiDef_s *multi;
    const char *enumDvarName;
    newsTickerDef_s *ticker;
    textScrollDef_s *scroll;
    void *data;
};

struct rectDef_s
{
    float x;
    float y;
    float w;
    float h;
    char horzAlign;
    char vertAlign;
};

struct windowDef_t
{
    const char *name;
    rectDef_s rect;
    rectDef_s rectClient;
    const char *group;
    int style;
    int border;
    int ownerDraw;
    int ownerDrawFlags;
    float borderSize;
    int staticFlags;
    int dynamicFlags[1];
    int nextTime;
    float foreColor[4];
    float backColor[4];
    float borderColor[4];
    float outlineColor[4];
    float disableColor[4];
    Material *background;
};

struct itemDef_t
{
    windowDef_t window;
    rectDef_s textRect[1];
    int type;
    int dataType;
    int alignment;
    int fontEnum;
    int textAlignMode;
    float textalignx;
    float textaligny;
    float textscale;
    int textStyle;
    int gameMsgWindowIndex;
    int gameMsgWindowMode;
    const char *text;
    int itemFlags;
    menuDef_t *parent;
    MenuEventHandlerSet *mouseEnterText;
    MenuEventHandlerSet *mouseExitText;
    MenuEventHandlerSet *mouseEnter;
    MenuEventHandlerSet *mouseExit;
    MenuEventHandlerSet *action;
    MenuEventHandlerSet *accept;
    MenuEventHandlerSet *onFocus;
    MenuEventHandlerSet *hasFocus;
    MenuEventHandlerSet *leaveFocus;
    const char *dvar;
    const char *dvarTest;
    ItemKeyHandler *onKey;
    const char *enableDvar;
    const char *localVar;
    int dvarFlags;
    snd_alias_list_t *focusSound;
    float special;
    int cursorPos[1];
    itemDefData_t typeData;
    int floatExpressionCount;
    ItemFloatExpression *floatExpressions;
    Statement_s *visibleExp;
    Statement_s *disabledExp;
    Statement_s *textExp;
    Statement_s *materialExp;
    float glowColor[4];
    bool decayActive;
    int fxBirthTime;
    int fxLetterTime;
    int fxDecayStartTime;
    int fxDecayDuration;
    int lastSoundPlayedTime;
    Statement_s *textAlignYExp;
}; // 0x180

struct menuTransition
{
    int transitionType;
    int startTime;
    float startVal;
    float endVal;
    float time;
    int endTriggerType;
};

struct menuData_t
{
    int fullScreen;
    int fadeCycle;
    float fadeClamp;
    float fadeAmount;
    float fadeInAmount;
    float blurRadius;
    MenuEventHandlerSet *onOpen;
    MenuEventHandlerSet *onCloseRequest;
    MenuEventHandlerSet *onClose;
    MenuEventHandlerSet *onESC;
    MenuEventHandlerSet *onFocusDueToClose;
    ItemKeyHandler *onKey;
    Statement_s *visibleExp;
    const char *allowedBinding;
    const char *soundName;
    float focusColor[4];
    Statement_s *rectXExp;
    Statement_s *rectYExp;
    Statement_s *rectWExp;
    Statement_s *rectHExp;
    Statement_s *openSoundExp;
    Statement_s *closeSoundExp;
    Statement_s *soundLoopExp;
    int cursorItem[1];
    menuTransition scaleTransition[1];
    menuTransition alphaTransition[1];
    menuTransition xTransition[1];
    menuTransition yTransition[1];
    ExpressionSupportingData *expressionData;
    char priority;
}; // 0xD4

struct menuDef_t
{
    menuData_t* data;
    windowDef_t window;
    int itemCount;
    itemDef_t** items;
}; // 0xB0

struct MenuList
{
    const char* name;
    int menuCount;
    menuDef_t** menus;
};

} // namespace xsk::menu::iw5
