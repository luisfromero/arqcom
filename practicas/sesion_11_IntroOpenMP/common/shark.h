#define NHERR localdata->nherr
#define NSHARK localdata->nshark
#define NT localdata->nt
#define TID localdata->id
#define RANDCNT localdata->randcnt	//Unused
#define DOOR localdata->door
#define dimy 600      			//Default ocean size
#define dimx 200 
#define defnh 10000   			//Default herr number
#define defns 1000			//Default shark number (for 120000 cells)
#define defhb 7				//Herr breed period
#define defsb 12			//Same for shark
#define defst 05			//Starvation
#define mod(x,y) ((y)+(x))%(y)		
#define RNDARRSZE 10000			//Unused
#define neigh_i(n) mod(i+neig[(n)][1],dimy)
#define neigh_ii(n) (i+neig[(n)][1]<0)?1:((i+neig[(n)][1]>dimy-1)?dimy-2:(i+neig[(n)][1]))
#define neigh_j(n) mod(j+neig[(n)][0],dimx)



/* cache line 128bytes, 32 integers */
typedef struct {
   int  id;
   int  nt;
   unsigned int  randcnt;
   int  nshark;
   int  nherr;
   int  door;
   int  pad[26];
} thread_data;

int finalize_(int usex);
int drawWorld_(unsigned short int m[][dimx], int usex);
int initialize_(unsigned short int m[][dimx], int  usex, int  askd, thread_data *td);
int item_(unsigned short int m[][dimx], int *pi,int *pj,int *pstpp, thread_data *td);

