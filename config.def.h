/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 10;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 10;       /* vert outer gap between windows and screen edge */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;    /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static       int smartgaps          = 1;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
/* nomes de família do fontconfig (confira com `fc-list | grep -i ubuntumono`);
 * a segunda fonte é usada como fallback para ícones */
static const char *fonts[]          = { "Ubuntu Mono:style=Italic:size=16", "Symbols Nerd Font:size=14" };
static const char dmenufont[]       = "Ubuntu Mono:style=Italic:size=16";
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#ffffff";
static char selfgcolor[]            = "#ffffff";
static char selbordercolor[]        = "#d79921";
static char selbgcolor[]            = "#505050";
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class        instance    title       tags mask     isfloating   monitor */
	{ "Gimp",       NULL,       NULL,       0,            1,           -1 },
	{ "firefox",    NULL,       NULL,       1 << 8,       0,           -1 },
	{ "LibreWolf",  NULL,       NULL,       1 << 8,       0,           -1 },
	{ "waterfox",   NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "[@]",      spiral },
	{ "[\\]",     dwindle },
	{ "H[]",      deck },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "HHH",      grid },
	{ "###",      nrowgrid },
	{ "---",      horizgrid },
	{ ":::",      gaplessgrid },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ NULL,       NULL },
};

/* movestack, cyclelayout, togglefullscr */
#include "extras.c"

/* key definitions */
#define MODKEY Mod4Mask   /* Super (tecla Windows) */
#define ALTKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* statuscmd: nome do processo da barra de status */
#define STATUSBAR "dwmblocks"

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont,
	"-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
static const char *screenshotcmd[] = { "screenshot_satty.sh", NULL };
static const char *screenshoteditcmd[] = { "screenshot_satty.sh", "-e", NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *browsercmd[] = { "librewolf", NULL };
#define FILEMGRCMD SHCMD("xdg-open \"$HOME\"")

/* scratchpad: terminal flutuante que aparece/some */
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "st", "-t", scratchpadname, "-g", "110x30", NULL };

/* volume e brilho: chamam o controle.sh e avisam o dwmblocks (sinais 1 e 2) */
#define VOLCMD(a)    SHCMD("controle.sh " a "; pkill -RTMIN+1 dwmblocks")
#define BRIGHTCMD(a) SHCMD("controle.sh " a "; pkill -RTMIN+2 dwmblocks")

static const Key keys[] = {
	/* teclas de mídia */
	{ 0, XF86XK_AudioRaiseVolume,  spawn, VOLCMD("vol_up") },
	{ 0, XF86XK_AudioLowerVolume,  spawn, VOLCMD("vol_down") },
	{ 0, XF86XK_AudioMute,         spawn, VOLCMD("vol_mute") },
	{ 0, XF86XK_AudioMicMute,      spawn, SHCMD("pactl set-source-mute @DEFAULT_SOURCE@ toggle") },
	{ 0, XF86XK_MonBrightnessUp,   spawn, BRIGHTCMD("bright_up") },
	{ 0, XF86XK_MonBrightnessDown, spawn, BRIGHTCMD("bright_down") },
	{ 0, XF86XK_AudioPlay,         spawn, SHCMD("playerctl play-pause") },
	{ 0, XF86XK_AudioNext,         spawn, SHCMD("playerctl next") },
	{ 0, XF86XK_AudioPrev,         spawn, SHCMD("playerctl previous") },

	/* modifier                     key        function        argument */
	/* programas */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_w,      spawn,          {.v = browsercmd } },
	{ MODKEY,                       XK_e,      spawn,          FILEMGRCMD },
	{ MODKEY,                       XK_grave,  togglescratch,  {.v = scratchpadcmd } },
	{ MODKEY|ShiftMask,             XK_l,      spawn,          SHCMD("slock") },
	{ MODKEY|ShiftMask,             XK_a,      spawn,          {.v = screenshotcmd } },
	{ MODKEY|ShiftMask,             XK_s,      spawn,          {.v = screenshoteditcmd } },

	/* janelas */
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_j,      movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      movestack,      {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_f,      togglefullscr,  {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY|ControlMask,           XK_c,      killclient,     {.ui = 1} },  /* fecha as outras janelas da tag */
	{ MODKEY|ShiftMask|ControlMask, XK_c,      killclient,     {.ui = 2} },  /* fecha todas as janelas da tag */

	/* layouts */
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_r,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_q,      setlayout,      {.v = &layouts[8]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[11]} },
	{ MODKEY|ControlMask,           XK_comma,  cyclelayout,    {.i = -1 } },
	{ MODKEY|ControlMask,           XK_period, cyclelayout,    {.i = +1 } },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },

	/* espaçamentos (vanitygaps): Super+Alt */
	{ MODKEY|ALTKEY,                XK_u,      incrgaps,       {.i = +1 } },
	{ MODKEY|ALTKEY|ShiftMask,      XK_u,      incrgaps,       {.i = -1 } },
	{ MODKEY|ALTKEY,                XK_i,      incrigaps,      {.i = +1 } },
	{ MODKEY|ALTKEY|ShiftMask,      XK_i,      incrigaps,      {.i = -1 } },
	{ MODKEY|ALTKEY,                XK_o,      incrogaps,      {.i = +1 } },
	{ MODKEY|ALTKEY|ShiftMask,      XK_o,      incrogaps,      {.i = -1 } },
	{ MODKEY|ALTKEY,                XK_6,      incrihgaps,     {.i = +1 } },
	{ MODKEY|ALTKEY|ShiftMask,      XK_6,      incrihgaps,     {.i = -1 } },
	{ MODKEY|ALTKEY,                XK_7,      incrivgaps,     {.i = +1 } },
	{ MODKEY|ALTKEY|ShiftMask,      XK_7,      incrivgaps,     {.i = -1 } },
	{ MODKEY|ALTKEY,                XK_8,      incrohgaps,     {.i = +1 } },
	{ MODKEY|ALTKEY|ShiftMask,      XK_8,      incrohgaps,     {.i = -1 } },
	{ MODKEY|ALTKEY,                XK_9,      incrovgaps,     {.i = +1 } },
	{ MODKEY|ALTKEY|ShiftMask,      XK_9,      incrovgaps,     {.i = -1 } },
	{ MODKEY|ALTKEY,                XK_0,      togglegaps,     {0} },
	{ MODKEY|ALTKEY|ShiftMask,      XK_0,      defaultgaps,    {0} },

	/* tags e monitores */
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)

	/* sistema */
	{ MODKEY,                       XK_F5,     xrdb,           {.v = NULL } },
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
	{ MODKEY|ControlMask|ShiftMask, XK_q,      quit,           {1} },  /* reinicia o dwm */
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkLtSymbol,          0,              Button4,        cyclelayout,    {.i = -1 } },
	{ ClkLtSymbol,          0,              Button5,        cyclelayout,    {.i = +1 } },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigstatusbar,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigstatusbar,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigstatusbar,   {.i = 3} },
	{ ClkStatusText,        0,              Button4,        sigstatusbar,   {.i = 4} },
	{ ClkStatusText,        0,              Button5,        sigstatusbar,   {.i = 5} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
