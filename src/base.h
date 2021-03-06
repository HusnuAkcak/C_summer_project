#include "allegro5/allegro.h"
#include "allegro5/allegro_ttf.h"
#include "allegro5/allegro_acodec.h"
#include "allegro5/allegro_audio.h"
#include <time.h>
#ifndef _BASE_H_
#define _BASE_H_

/*###########################################################################
                                CONSTANTS
###########################################################################*/
#define LINE_SIZE 240     //used while caves are imported.
#define NAME_LENGTH 75                              //for some strings
#define MAP_FILE "../data/maps.txt"                 //cave maps
#define FONT_FILE "../data/font/commodore.ttf"      //game font type
#define FONT_SIZE 50                                //game font size
#define CELL_SIZE 64                                //icon pixel
#define MINER_LIFE 4                                //miner's initial life
#define FPS 60                                      //display frequence
#define FALL_PS 2.5                                 //falling frequence.
#define MPS 7                                       //miner's speed freq.
#define DIA_OF_SPIDER 8   //dia num that is created after spiders are crushed.
#define DIA_OF_MONSTER 12 //dia num that is created after monsters are crushed.
#define MINER_DEATH_DURATION 2      //duration of time to die.(in second)
#define MUSIC_INCREASE_SLICE 0.1    //time slince to music rhytm increasing.

/*PATHS..                                                                   */
#define AUDIO_PATH "../data/audio"
#define IMG_PATH "../data/img"
/*###########################################################################
                            END OF CONSTANTS
###########################################################################*/

/*###########################################################################
                            GLOBAL VARIABLES
###########################################################################*/
/*ALLEGRO COMPONENTS.                                                       */
ALLEGRO_DISPLAY         *display;
ALLEGRO_EVENT_QUEUE     *event_queue;
ALLEGRO_TIMER           *main_timer, *panel_timer, *falling_timer, *miner_timer;
ALLEGRO_FONT            *font;
ALLEGRO_TRANSFORM       camera;



/*Game muscis samples and sample instances.                              */
ALLEGRO_SAMPLE *background;
ALLEGRO_SAMPLE *miner_dies;
ALLEGRO_SAMPLE *insect_dies;
ALLEGRO_SAMPLE *door_opens;
ALLEGRO_SAMPLE *diamond_collect;
ALLEGRO_SAMPLE *eat_soil;
ALLEGRO_SAMPLE *rock_falling;

ALLEGRO_SAMPLE_INSTANCE *background_instance;
ALLEGRO_SAMPLE_INSTANCE *miner_dies_instance;
ALLEGRO_SAMPLE_INSTANCE *insect_dies_instance;
ALLEGRO_SAMPLE_INSTANCE *door_opens_instance;
ALLEGRO_SAMPLE_INSTANCE *diamond_collect_instance;
ALLEGRO_SAMPLE_INSTANCE *eat_soil_instance;
ALLEGRO_SAMPLE_INSTANCE *rock_falling_instance;


/*Cave bitmaps                                                            */
ALLEGRO_BITMAP *ex_wall;
ALLEGRO_BITMAP *in_wall;
ALLEGRO_BITMAP *soil;
ALLEGRO_BITMAP *diamond;
ALLEGRO_BITMAP *small_diamond;
ALLEGRO_BITMAP *rock;
ALLEGRO_BITMAP *gate;
ALLEGRO_BITMAP *spider;
ALLEGRO_BITMAP *miner;
ALLEGRO_BITMAP *miner_symbol;
ALLEGRO_BITMAP *water;
ALLEGRO_BITMAP *monster;
ALLEGRO_BITMAP *empty_cell;
ALLEGRO_BITMAP *pause_button;
ALLEGRO_BITMAP *play_button;
ALLEGRO_BITMAP *restart_level_button;
ALLEGRO_BITMAP *time_icon;
ALLEGRO_BITMAP *explosion;
ALLEGRO_BITMAP *new_game;
ALLEGRO_BITMAP *exit_game;

/*###########################################################################
                        END OF GLOBAL VARIABLES
###########################################################################*/

/*###########################################################################
                                ENUM TYPES
###########################################################################*/
/*Cave symbols meaning.                                                    */
typedef enum{EX_WALL='W',IN_WALL='w',SOIL='.',DIAMOND='d',ROCK='r',GATE='X',
DEAD_MINER='p', SPIDER='S',MINER='P',WATER='l',MONSTER='M',EMPTY_CELL=' '}Content;
typedef enum{UP, RIGHT, DOWN, LEFT, NONE}Direction; /*movement directions       */
typedef enum{PAUSE, RESTART, END, CONTINUE, NEXT}Status;/*status of the game    */
/*###########################################################################
                            END OF ENUM TYPES
###########################################################################*/

/*###########################################################################
                                STRUCT TYPES
###########################################################################*/
typedef struct Point{
    int c,r;/*column(c) is in place of x, row(r) is in place of y.  */
}Point;

typedef struct Monster{
    Point pos;
    Point *route;               /*the complate route of monster             */
    int route_size;             /*size of dynamic route array               */
    struct Monster *next;
    bool alive;
}Monster;

typedef struct Spider{
    Point pos;
    Point next_pos;
    struct Spider *next;
    Direction move_dir;         /*current movement direction                */
    bool alive;
}Spider;

typedef struct Cave{
    int cave_number;
    char cave_name[NAME_LENGTH];
    int dim_row,dim_col;        /*dimension_x and dimension_y               */
    int left_time, max_time;
    int dia_req, collected_dia, dia_val, ex_dia_val;/*dia =diamond          */
    int water_discharge_period;
    long last_water_discharge_time;
    Point gate_loc;             /*the gate is hidden until req_dia collected*/
    Content **content;
    Monster *head_monster;
    Spider  *head_spider;
    struct Cave *next;
}Cave;

typedef struct Miner{
    int score, curr_cave_score, collected_dia;/*current lvl collected diamond*/
    Point pos;
    int life;
    bool alive;
    Direction move_dir;
    int duration_of_death;
}Miner;

typedef struct Game{
    Miner miner;
    Cave *head_cave;
    Point cam_pos;
    Status status;
}Game;
/*###########################################################################
                            END OF STRUCT TYPES
###########################################################################*/

/*###########################################################################
                            FUNCTION PROTOTYPES
###########################################################################*/

/*--  void load_cave_bitmaps(...)  ------------------------------------------
    Cave icons are loaded.
---------------------------------------------------------------------------*/
void load_cave_bitmaps();

/*-- void destroy_cave_bitmaps(...) -----------------------------------------
    Destroys cave bitmaps.
---------------------------------------------------------------------------*/
void destroy_cave_bitmaps();

/*-- void load_sample_instance(...) --------------------------------------
    loads samples and sample instances.
--------------------------------------------------------------------------*/
void load_sample_instance();

/*-- void destroy_sample_instance(...) --------------------------------------
    destroys samples and sample instances.
--------------------------------------------------------------------------*/
void destroy_sample_instance();

/*-- string_cmp(...) --------------------------------------------------------
    retrun 0 :if two string is equal.
---------------------------------------------------------------------------*/
int string_cmp(const char * str1, const char *str2);

/*-- void string_cpy(...) ---------------------------------------------------
    Copies src to dest.
----------------------------------------------------------------------------*/
void string_cpy(char *dest, const char *src);

/*-- void int_to_str(...) ---------------------------------------------------
    Converts int to char array, then assign it to dest.
---------------------------------------------------------------------------*/
void int_to_str(char *dest, int src);

/*-- void reverse_str(...) -------------------------------------------------
    Reverses given char array.
--------------------------------------------------------------------------*/
void reverse_str(char *str);

/*-- void string_cat(...) --------------------------------------------------
    merges two char arrays.
---------------------------------------------------------------------------*/
void string_cat(char* first, char *sec);

/*###########################################################################
                        END OF FUNCTION PROTOTYPES
###########################################################################*/
#endif
