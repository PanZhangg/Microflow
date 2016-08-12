$1 : windows 
  you MUST have vc 2008 compiler ,
use vc 2008 command prompt,
cd into this directory ,and type 'nmake windows'

$2 : unix/linux 
  you MUST have gcc installed ,and type 'make linux'

$3:
 before you compile the project ,maybe you should
change some value defined in http.h
like :
/*default listen port*/
#define  PORT 8000

/*default max threads*/
#define WR_MAX_THREADS 10

static const char *IndexFile = "index.html" ;
static const char *RootPath = "www" ;

