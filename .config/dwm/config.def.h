/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int barheight = 15;
static const unsigned int borderpx = 1; /* border pixel of windows */
static const unsigned int snap = 32;    /* snap pixel */
static const unsigned int gappih = 20;  /* horiz inner gap between windows */
static const unsigned int gappiv = 10;  /* vert inner gap between windows */
static const unsigned int gappoh =
    10; /* horiz outer gap between windows and screen edge */
static const unsigned int gappov =
    30; /* vert outer gap between windows and screen edge */
static int smartgaps =
    0; /* 1 means no outer gap when there is only one window */
static const int showbar = 1; /* 0 means no bar */
static const unsigned int colorfultitle =
    1; /* 0 means title use SchemeTitle and SchemeTitleFloat */
static const unsigned int colorfultag =
    1;                          /* 0 means use SchemeSel for selected tag */
static const int topbar = 1;    /* 0 means bottom bar */
static const int vertpad = 8;   /* vertical padding of bar */
static const int rhoripad = 10; /* right horizontal padding of bar */
static const int sidepad = 10;  /* horizontal padding of bar */
static const char *fonts[] = {"FiraCode Nerd Font:size=14"};
static const char dmenufont[] = "FiraCode Nerd Font Mono:size=16";
static const char col_gray1[] = "#222222";
static const char col_gray2[] = "#444444";
static const char col_gray3[] = "#bbbbbb";
static const char col_gray4[] = "#eeeeee";
static const char col_bg[] = "#1a1b26";
static const char col_dark[] = "#16161E";
static const char col_dark_1[] = "#232433";
static const char col_dark_2[] = "#2a2b3d";
static const char col_dark_3[] = "#3b3d57";
static const char col_fg[] = "#a9b1d6";
static const char col_black[] = "#32344a";
static const char col_br_black[] = "#444b6a";
static const char col_white[] = "#787c99";
static const char col_br_white[] = "#acb0d0";
static const char col_red[] = "#f7768e";
static const char col_br_red[] = "#ff7a93";
static const char col_green[] = "#9ece6a";
static const char col_br_green[] = "#b9f27c";
static const char col_yellow[] = "#e0af68";
static const char col_br_yellow[] = "#ff9e64";
static const char col_blue[] = "#7aa2f7";
static const char col_br_blue[] = "#7da6ff";
static const char col_magenta[] = "#ad8ee6";
static const char col_br_magenta[] = "#bb9af7";
static const char col_cyan[] = "#449dab";
static const char col_br_cyan[] = "#0db9d7";
static const char col_void[] = "#ff4c4c";
static const char col_end[] = "#e1c71a";
static const char *colors[][3] = {
    /*               fg         bg         border   */
    [SchemeNorm] = {col_fg, col_bg, col_bg},        /* \x0b */
    [SchemeSel] = {col_blue, col_dark, col_dark_1}, /* \x0c */
    [SchemeTag] = {col_br_black, col_bg, col_black},
    [SchemeTag1] = {col_void, col_dark, col_black},
    [SchemeTag2] = {col_red, col_dark, col_black},
    [SchemeTag3] = {col_br_yellow, col_dark, col_black},
    [SchemeTag4] = {col_br_blue, col_dark, col_black},
    [SchemeTag5] = {col_br_magenta, col_dark, col_black},
    [SchemeTag6] = {col_br_cyan, col_dark, col_black},
    [SchemeTag7] = {col_br_green, col_dark, col_black},
    [SchemeTag8] = {col_yellow, col_dark, col_black},
    [SchemeTag9] = {col_end, col_dark, col_black},
    [SchemeLayout] = {col_white, col_bg, col_black},
    [SchemeTitle] = {col_fg, col_bg, col_black},
    [SchemeTitleFloat] = {col_br_blue, col_bg, col_black},
    [SchemeTitle1] = {col_void, col_bg, col_black},
    [SchemeTitle2] = {col_red, col_bg, col_black},
    [SchemeTitle3] = {col_br_yellow, col_bg, col_black},
    [SchemeTitle4] = {col_br_blue, col_bg, col_black},
    [SchemeTitle5] = {col_br_magenta, col_bg, col_black},
    [SchemeTitle6] = {col_br_cyan, col_bg, col_black},
    [SchemeTitle7] = {col_br_green, col_bg, col_black},
    [SchemeTitle8] = {col_yellow, col_bg, col_black},
    [SchemeTitle9] = {col_end, col_bg, col_black},
};

/* tagging */
static const char *tags[] = {"", "", "", "", "",
                             "", "", "", ""};

static const unsigned int ulinepad =
    5; /* horizontal padding between the underline and tag */
static const unsigned int ulinestroke =
    2; /* thickness / height of the underline */
static const unsigned int ulinevoffset =
    0; /* how far above the bottom of the bar the line should appear */
static const int ulineall =
    0; /* 1 to show underline on all tags, 0 for just the active ones */

static const Rule rules[] = {
    /* xprop(1):
     *	WM_CLASS(STRING) = instance, class
     *	WM_NAME(STRING) = title
     */
    /* class      instance    title       tags mask     isfloating   monitor */
    {"Gimp", NULL, NULL, 0, 1, -1},
    {"Firefox", NULL, NULL, 1 << 8, 0, -1},
};

/* layout(s) */
static const float mfact = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster = 1;    /* number of clients in master area */
static const int resizehints =
    1; /* 1 means respect size hints in tiled resizals */
static const int attachbelow =
    1; /* 1 means attach after the currently active window */
static const int lockfullscreen =
    1; /* 1 will force focus on the fullscreen window */
#include "vanitygaps.c"
#define FORCE_VSPLIT                                                           \
  1 /* nrowgrid layout: force two clients to always split vertically */

static const Layout layouts[] = {
    /* symbol     arrange function */
    {"{F}", fibonacci},
    {"[T]=", tile}, /* first entry is default */
    {"<B>", bstack},
    {"><>", NULL}, /* no layout function means floating behavior */
};

static const int tagschemes[] = {SchemeTag1, SchemeTag2, SchemeTag3,
                                 SchemeTag4, SchemeTag5, SchemeTag6,
                                 SchemeTag7, SchemeTag8, SchemeTag9};
static const int titleschemes[] = {SchemeTitle1, SchemeTitle2, SchemeTitle3,
                                   SchemeTitle4, SchemeTitle5, SchemeTitle6,
                                   SchemeTitle7, SchemeTitle8, SchemeTitle9};

/* key definitions */
#define ALTKEY Mod1Mask // Alt key
#define MODKEY Mod4Mask // Windows key
#define TAGKEYS(KEY, TAG)                                                      \
  {MODKEY, KEY, view, {.ui = 1 << TAG}},                                       \
      {MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}},               \
      {MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}},                        \
      {MODKEY | ControlMask | ShiftMask, KEY, toggletag, {.ui = 1 << TAG}},

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd)                                                             \
  {                                                                            \
    .v = (const char *[]) { "/bin/sh", "-c", cmd, NULL }                       \
  }

/* commands */
static char dmenumon[2] =
    "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = {
    "dmenu_run", "-i",     "-F",      "-c",      "-bw",     "3",   "-m",
    dmenumon,    "-fn",    dmenufont, "-nb",     col_gray1, "-nf", col_gray3,
    "-sb",       col_cyan, "-sf",     col_gray4, NULL};
static const char *termcmd[] = {"st", NULL};

#include "exitdwm.c"
static const Key keys[] = {
    /* modifier                     key        function        argument */

    // Open launcher
    {MODKEY, XK_d, spawn, {.v = dmenucmd}},
    {MODKEY, XK_Return, spawn, {.v = termcmd}},

    // Full screen like
    {ALTKEY, XK_f, togglebar, {0}},

    // Move around
    {MODKEY, XK_j, focusstack, {.i = +1}},
    {MODKEY, XK_k, focusstack, {.i = -1}},

    // Push and pop
    {MODKEY, XK_p, incnmaster, {.i = +1}},
    {MODKEY | ShiftMask, XK_p, incnmaster, {.i = -1}},

    // Push to stack
    {MODKEY | ShiftMask, XK_Return, zoom, {0}},

    // Resize
    {ALTKEY, XK_k, setcfact, {.f = +0.25}},
    {ALTKEY, XK_j, setcfact, {.f = -0.25}},
    {ALTKEY, XK_h, setmfact, {.f = -0.05}},
    {ALTKEY, XK_l, setmfact, {.f = +0.05}},
    {ALTKEY, XK_o, setrfact, {.f = 0.00}},

    //{ MODKEY|Mod4Mask,              XK_u,      incrgaps,       {.i = +1 } },
    //{ MODKEY|Mod4Mask|ShiftMask,    XK_u,      incrgaps,       {.i = -1 } },
    //{ MODKEY|Mod4Mask,              XK_i,      incrigaps,      {.i = +1 } },
    //{ MODKEY|Mod4Mask|ShiftMask,    XK_i,      incrigaps,      {.i = -1 } },
    //{ MODKEY|Mod4Mask,              XK_o,      incrogaps,      {.i = +1 } },
    //{ MODKEY|Mod4Mask|ShiftMask,    XK_o,      incrogaps,      {.i = -1 } },

    {MODKEY | ControlMask | ShiftMask, XK_j, incrihgaps, {.i = +1}},
    {MODKEY | ControlMask | ShiftMask, XK_k, incrihgaps, {.i = -1}},
    {MODKEY | ControlMask | ShiftMask, XK_l, incrivgaps, {.i = +1}},
    {MODKEY | ControlMask | ShiftMask, XK_h, incrivgaps, {.i = -1}},
    {MODKEY | ControlMask, XK_j, incrohgaps, {.i = +1}},
    {MODKEY | ControlMask, XK_k, incrohgaps, {.i = -1}},
    {MODKEY | ControlMask, XK_h, incrovgaps, {.i = +1}},
    {MODKEY | ControlMask, XK_l, incrovgaps, {.i = -1}},

    {ALTKEY, XK_g, togglegaps, {0}},
    {ALTKEY, XK_r, defaultgaps, {0}},

    {MODKEY, XK_Tab, view, {0}},
    {MODKEY | ShiftMask, XK_q, killclient, {0}},

    {ALTKEY, XK_1, setlayout, {.v = &layouts[0]}},
    {ALTKEY, XK_2, setlayout, {.v = &layouts[1]}},
    {ALTKEY, XK_3, setlayout, {.v = &layouts[2]}},
    {ALTKEY, XK_space, setlayout, {0}},
    {MODKEY | ShiftMask, XK_space, togglefloating, {0}},

    //{ MODKEY|ControlMask,           XK_0,      view,           {.ui = ~0 } },
    //{ MODKEY,                       XK_m,      focusmon,       {.i = -1 } },
    //{ MODKEY,                       XK_u,      focusmon,       {.i = +1 } },
    //{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
    //{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
    //{ MODKEY,			XK_Prior,  zoom,	   {.f = +1} },
    //{ MODKEY,			XK_Next,   zoom,	   {.f = -1} },

    TAGKEYS(XK_1, 0) TAGKEYS(XK_2, 1) TAGKEYS(XK_3, 2) TAGKEYS(XK_4, 3)
        TAGKEYS(XK_5, 4) TAGKEYS(XK_6, 5) TAGKEYS(XK_7, 6) TAGKEYS(XK_8, 7)
            TAGKEYS(XK_9, 8){MODKEY | ShiftMask, XK_0, tag, {.ui = ~0}},
    {MODKEY | ShiftMask, XK_e, exitdwm, {0}},
    {MODKEY | ControlMask | ShiftMask, XK_e, quit, {1}},
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
 * ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
    /* click                event mask      button          function argument */
    {ClkLtSymbol, 0, Button1, setlayout, {0}},
    {ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[2]}},
    {ClkWinTitle, 0, Button2, zoom, {0}},
    {ClkStatusText, 0, Button2, spawn, {.v = termcmd}},
    {ClkClientWin, MODKEY, Button1, movemouse, {0}},
    {ClkClientWin, MODKEY, Button2, togglefloating, {0}},
    {ClkClientWin, MODKEY, Button3, resizemouse, {0}},
    {ClkTagBar, 0, Button1, view, {0}},
    {ClkTagBar, 0, Button3, toggleview, {0}},
};
