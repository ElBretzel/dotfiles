/* user and group to drop privileges to */
static const char *user  = "nobody";
static const char *group = "nogroup";
static const int controlkeyclear = 1;
static const int monitortime = 5;
static const int runonce = 0;
static const int timeoffset = 900;
static const char *command = "loginctl suspend";
static const char *colorname[NUMCOLS] = {
	[INIT] =   "#9ece6a",     /* after initialization */
	[INPUT] =  "#8ec07c",   /* during input */
	[FAILED] = "#ff7a93",   /* wrong password */
	[CAPS]  =  "blue",
	[DELETE] = "yellow",
	[EMPTY] = "gray",
};

/* treat a cleared input like a wrong password (color) */
static const int failonclear = 0;


/* insert grid pattern with scale 1:1, the size can be changed with logosize */
static const int logosize = 42;
/* grid width and height for right center alignment */
static const int logow = 12;
static const int logoh = 12;

//static XRectangle rectangles[9] = {
//	/* x    y       w       h */
//	{ 0,    3,      1,      3 },
//	{ 1,    3,      2,      1 },
//	{ 0,    5,      8,      1 },
//	{ 3,    0,      1,      5 },
//	{ 5,    3,      1,      2 },
//	{ 7,    3,      1,      2 },
//	{ 8,    3,      4,      1 },
//	{ 9,    4,      1,      2 },
//	{ 11,   4,      1,      2 },
//};


/*     (<---W--->)
V= ' ' ' ' ' ' ' ' ' ' ' '
X= ' ' ' ' ' ' ' ' ' ' ' ' 
9= ' ' ' ' ' ' ' ' ' ' ' '    ^
8= ' ' ' ' ' ' ' ' ' ' ' '    |
7= ' ' ' ' ' ' ' ' ' ' ' '    |
6= ' ' ' ' ' ' ' ' ' ' ' '    |
5= o o o o o o o o ' o ' o    H
4= o ' ' o ' o ' o ' o ' o    |
3= o o o o ' o ' o o o o o    |
2= ' ' ' o ' ' ' ' ' ' ' '    |
1= ' ' ' o ' ' ' ' ' ' ' '    v
0= ' ' ' o ' ' ' ' ' ' ' '
   ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^
   0 1 2 3 4 5 6 7 8 9 X V 
*/

/*     (<---W--->)
V= ' ' F F F F F F F ' ' '
X= ' D A ' ' ' ' ' C ' ' ' 
9= E D ' ' G G G G ' ' ' '    ^
8= E ' ' B ' ' ' ' ' ' A E    |
7= E ' H ' ' I I ' ' H ' E    |
6= E ' H ' I ' ' I ' H ' E    |
5= E ' H ' I ' ' I ' H ' E    H
4= E ' H ' ' I I ' ' H ' E    |
3= E A ' ' ' ' ' ' C ' ' E    |
2= ' ' ' ' G G G G ' ' D E    |
1= ' ' ' B ' ' ' ' ' A D '    v
0= ' ' ' F F F F F F F ' '
   ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^
   0 1 2 3 4 5 6 7 8 9 X V 
*/

static XRectangle rectangles[22] = {
	/* x    y       w       h */
	// A
	{ 1,    3,      1,      1 },
	{ 2,    10,     1,      1 },
	{ 9,    1,      1,      1 },
	{ 10,   8,      1,      1 },

	// B
	{ 3,    1,      1,      1 },
	{ 3,    8,      1,      1 },

	// C
	{ 8,    3,      1,      1 },
	{ 8,    10,     1,      1 },

	// D
	{ 1,    9,      1,      2 },
	{ 10,   1,      1,      2 },

	// E
	{ 0,    3,      1,      7 },
	{ 11,   2,      1,      7 },

	// F
	{ 3,    0,      7,      1 },
	{ 2,    11,     7,      1 },

	// G
	{ 4,    2,      4,      1 },
	{ 4,    9,      4,      1 },

	// H
	{ 2,    4,      1,      4 },
	{ 9,    4,      1,      4 },

	// I
	{ 5,    4,      2,      1 },
	{ 5,    7,      2,      1 },
	{ 4,    5,      1,      2 },
	{ 7,    5,      1,      2 },
};

/*Enable blur*/
#define BLUR
/*Set blur radius*/
static const int blurRadius=7;
/*Enable Pixelation*/
//#define PIXELATION
/*Set pixelation radius*/
static const int pixelSize=3;
