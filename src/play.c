#include<stdio.h>
#include<stdlib.h>
#include"base.h"
#include"play.h"
#include"cave.h"
#include"overhead.h"
#include"insect.h"
#include"allegro5/allegro.h"
#include"allegro5/allegro_acodec.h"
#include"allegro5/allegro_image.h"
#include"allegro5/allegro_native_dialog.h"

void
intro_game(Game *game,int scr_width,int scr_height){

    ALLEGRO_BITMAP      *boulder_dash;
    ALLEGRO_BITMAP      *title_screen;


    /*background music(It is always played.)                               */
    al_play_sample_instance(background_instance);

    /*First intro image                                                    */
    boulder_dash=al_load_bitmap(IMG_PATH"/boulder_dash.jpg");
    al_draw_bitmap(boulder_dash,scr_width/3,scr_height/3,0);
    al_flip_display();
    al_rest(1);
    al_clear_to_color(al_map_rgb(0,0,0));
    /*Second intro image                                                    */
    title_screen=al_load_bitmap(IMG_PATH"/title_screen.png");
    al_draw_bitmap(title_screen,scr_width/3,scr_height/3,0);
    al_flip_display();
    al_rest(1);
    al_clear_to_color(al_map_rgb(0,0,0));

    al_destroy_bitmap(boulder_dash);
    al_destroy_bitmap(title_screen);
}

void
play_game(Game * g){
    Cave curr_cave;
    ALLEGRO_EVENT ev;
    Point mouse_pos;
    /*increased is about music rhytm, play :display screen, moving: miner moves. */
    bool increased, play, moving;

    /*Miner's initial values.                                               */
    g->miner.life=MINER_LIFE;
    g->miner.alive=true;
    g->miner.score=0;
    g->miner.curr_cave_score=0;
    g->miner.collected_dia=0;

    play=true;              /*if play==true, the screen will be refreshed.      */
    moving=true;            /*when miner moves, it turns out true.              */
    increased=false;        /*at the begining music rhytm starts normal.        */
    g->status=CONTINUE;     /*status is initialized as CONTINUE.                */
    g->miner.move_dir=NONE; /*miner's movement direction                        */

    /*current cave's initial values.                                            */
    curr_cave.content=NULL;
    curr_cave.head_monster=NULL;
    curr_cave.head_spider=NULL;
    curr_cave.dim_row=0;
    curr_cave.dim_col=0;

    al_set_sample_instance_speed(background_instance, 1.0);/*music rhythm           */
    copy_cave(&curr_cave, g->head_cave);
    g->miner.duration_of_death=MINER_DEATH_DURATION;/*waiting time after miner dies.*/
    find_miner_loc(&curr_cave, &(g->miner));        /*Miner's location is found.    */
    while(g->status!=END){
        if(play && al_is_event_queue_empty(event_queue)){

            /*if it is required to level up                                 */
            if(g->status==NEXT){
                if(curr_cave.left_time>0){//left time is being added to miner's score.
                    --curr_cave.left_time;
                    ++(g->miner.score);
                    ++(g->miner.curr_cave_score);
                }
                else{
                    g->status=go_next_cave(g, &curr_cave);
                    moving=true;
                }
            }

            /*if miner moves                                                */
            if(moving){
                al_clear_to_color(al_map_rgb(0,0,0));
                al_identity_transform(&camera);
                set_camera(g, &curr_cave);
                al_translate_transform(&camera, -(g->cam_pos.c), -(g->cam_pos.r));
                al_use_transform(&camera);
                moving=false;
            }

            /*bitmaps of current screen are updated.                        */
            display_curr_screen(&curr_cave, g);
            al_flip_display();/*rendering                                   */

            /*this control is performed after al_flip_display, because if miner
            is dead, we freeze the screen for a while.                      */
            if(g->miner.alive==false){
                al_play_sample_instance(miner_dies_instance);
                g->status=PAUSE;
                if((g->miner.duration_of_death)<=0){
                    --(g->miner.life);
                    if((g->miner.life)>0){
                        /*music rhythm is set its normal speed.             */
                        al_set_sample_instance_speed(background_instance, 1.0);
                        increased=false;/*music rhythm is not increased     */
                        restart_cave(g, &curr_cave);/*cave is restarting.   */
                        moving=true;/*miner's pos is possibly changed.      */
                    }else{
                        g->status=END;
                    }
                }
            }
            play=false;
        }

        al_wait_for_event(event_queue,&ev);

        if(ev.type==ALLEGRO_EVENT_DISPLAY_CLOSE){
            g->status=END;
        }
        else if(ev.type==ALLEGRO_EVENT_KEY_DOWN){
            if(ev.keyboard.keycode==ALLEGRO_KEY_ESCAPE){
                g->status=END;
            }
            else if(ev.keyboard.keycode==ALLEGRO_KEY_DOWN){
                g->miner.move_dir=DOWN;
            }
            else if(ev.keyboard.keycode==ALLEGRO_KEY_UP){
                g->miner.move_dir=UP;
            }
            else if(ev.keyboard.keycode==ALLEGRO_KEY_LEFT){
                g->miner.move_dir=LEFT;
            }
            else if(ev.keyboard.keycode==ALLEGRO_KEY_RIGHT){
                g->miner.move_dir=RIGHT;
            }
        }
        else if(ev.type==ALLEGRO_EVENT_KEY_UP){
            g->miner.move_dir=NONE;
        }
        else if(ev.type==ALLEGRO_EVENT_MOUSE_AXES){
            mouse_pos.c=ev.mouse.x;
            mouse_pos.r=ev.mouse.y;
        }
        else if(ev.type==ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
            /*pause button                                                  */
            if( mouse_pos.r>0 &&
                mouse_pos.r<CELL_SIZE &&
                mouse_pos.c>(18*CELL_SIZE) &&
                mouse_pos.c<(19*CELL_SIZE)
            ){
                if(g->status==CONTINUE){
                    g->status=PAUSE;
                    al_stop_sample_instance(background_instance);/*music is stopped.*/
                }
                else if(g->status==PAUSE){
                    g->status=CONTINUE;
                    al_play_sample_instance(background_instance);/*music is played. */
                }
            }
            /*restart button                                                 */
            else if(mouse_pos.r>0 &&
                    mouse_pos.r<CELL_SIZE &&
                    mouse_pos.c>(19*CELL_SIZE) &&
                    mouse_pos.c<(20*CELL_SIZE)
            ){
                al_stop_sample_instance(background_instance);
                al_set_sample_instance_speed(background_instance, 1.0);
                increased=false;
                al_play_sample_instance(background_instance);
                restart_cave(g, &curr_cave);
                moving=true;
            }
        }
        else if(ev.type==ALLEGRO_EVENT_TIMER){

            if(ev.timer.source==main_timer){
                is_miner_dead(g, &curr_cave, &(g->miner));
                play=true;
            }

            if(ev.timer.source==panel_timer && g->status==CONTINUE){
                --curr_cave.left_time;

                if( (curr_cave.left_time) < (curr_cave.max_time*MUSIC_INCREASE_SLICE) && increased==false){
                    /*music rhythm is inreased.                             */
                    increased=true;
                    al_set_sample_instance_speed(background_instance, 1.08);
                }
            }
            else if(ev.timer.source==panel_timer && (g->miner.alive==false) ){
                /*when miner dies the screen is freezed till miner's duration of death. */
                --(g->miner.duration_of_death);
            }

            if(ev.timer.source==falling_timer && g->status==CONTINUE){
                move_insects(g, &curr_cave);
                control_falling(&(g->miner), &curr_cave);
            }
            /*it is controlled, if miner want to move or not.               */
            if(ev.timer.source==miner_timer && g->miner.move_dir!=NONE && g->status==CONTINUE){
                g->status=move(&curr_cave, &(g->miner));
                moving=true;
            }
        }
    }
    /*when loop is terminated final result is shown to user.                */
    show_final_results(g, &curr_cave);
    return;
}

Status
move(Cave * cave,Miner *m){

    Status status;      /*status of the game                                */
    char target;        /*target cell                                       */
    char after_target;  /*comes after target cell(in the same direction)    */
    Point tp, atp;      /*tp=target point, atp=after target point           */
    Point pre_pos;      /*previous position                                 */

    status=CONTINUE;    /*status is set CONTINUE as default.                */
    detect_target(cave, m, &target, &after_target, &tp, &atp);

    /*it is controlled and if the move possible player's choice is applied. */
    if(target!=IN_WALL && target !=EX_WALL && target!=WATER &&
    !(target==ROCK && m->move_dir==UP) && !(target==ROCK && after_target!=EMPTY_CELL)){

        pre_pos=m->pos;
        m->pos=tp;      /*miner goes to target position(tp)                 */
        cave->content[tp.r][tp.c]=MINER;/*target is occupied.               */

        /*previous location(miner's old pos) is changed as EMPTY_CELL       */
        cave->content[pre_pos.r][pre_pos.c]=EMPTY_CELL;

        if(target==GATE){
            if((cave->dia_req)-(cave->collected_dia)<=0){
                status=NEXT;
            }
        }
        else if(target==DIAMOND){
            ++m->collected_dia;
            ++cave->collected_dia;
            al_stop_sample_instance(diamond_collect_instance);//if there is already a music.
            al_play_sample_instance(diamond_collect_instance);
            if(cave->dia_req==cave->collected_dia){
                al_play_sample_instance(door_opens_instance);
                cave->content[cave->gate_loc.r][cave->gate_loc.c]=GATE;
            }

            if((cave->dia_req)-(cave->collected_dia)>0){
                m->curr_cave_score+=cave->dia_val;
                m->score+=cave->dia_val;    /*general score                 */
            }
            else{
                m->curr_cave_score+=cave->ex_dia_val;
                m->score+=cave->ex_dia_val; /*general score                 */
            }
        }
        else if(target==ROCK && after_target==EMPTY_CELL){
            if(m->move_dir!=UP){
                cave->content[atp.r][atp.c]=ROCK;
            }
        }
        else if(target==SOIL){
            al_stop_sample_instance(eat_soil_instance);//if it is being played.
            al_play_sample_instance(eat_soil_instance);
        }
    }


    return status;
}

void
show_final_results(Game *g, Cave *curr_cave){
    /*string version of the data to use in al_draw_text function            */
    ALLEGRO_EVENT ev;
    ALLEGRO_DISPLAY_MODE disp_data;
    Point mouse_pos;
    char str_score[NAME_LENGTH];
    char str_cave_number[NAME_LENGTH];
    char temp_string[NAME_LENGTH];
    int r,c;

    /*the screen is made clear.                                             */
    al_clear_to_color(al_map_rgb(0,0,0));

    /*camera settings are readjusted as (0,0)                               */
    al_identity_transform(&camera);
    al_translate_transform(&camera, 0, 0);
    al_use_transform(&camera);

    /*Screen width and height informations are recieved with 'disp_data'    */
    al_get_display_mode(0, &disp_data);

    /*initializations for char arrays.                                      */
    str_score[0]=0;
    str_cave_number[0]=0;
    temp_string[0]=0;

    /*the frame is drawed with diamond icons.                               */
    for(r=CELL_SIZE; r<(disp_data.height-2*CELL_SIZE); r+=CELL_SIZE){
        al_draw_bitmap(diamond, 0, r, 0);//left side
        al_draw_bitmap(diamond, (disp_data.width-(2*CELL_SIZE)), r, 0);//right side
    }
    for(c=0; c<(disp_data.width-2*CELL_SIZE); c+=CELL_SIZE){
        al_draw_bitmap(diamond, c, 0, 0);//up side
        al_draw_bitmap(diamond, c, (disp_data.height-(2*CELL_SIZE)), 0);//bottom side
    }

    /*strings are manupilated.                                              */
    string_cpy(str_score,           "SCORE            :");
    int_to_str(temp_string, g->miner.score);
    string_cat(str_score, temp_string);
    temp_string[0]='\0';

    string_cpy(str_cave_number,     "LAST CAVE NUMBER :");
    int_to_str(temp_string, curr_cave->cave_number);
    string_cat(str_cave_number, temp_string);
    temp_string[0]='\0';

    string_cpy(temp_string, curr_cave->cave_name);
    string_cpy(curr_cave->cave_name, "LAST CAVE NAME   :");
    string_cat(curr_cave->cave_name, temp_string);
    temp_string[0]='\0';

    /*Informations are shown on the screen.                                 */
    al_draw_text(font, al_map_rgb(100, 200, 100), (2*CELL_SIZE), 3*CELL_SIZE, ALLEGRO_ALIGN_LEFT, str_score);
    al_draw_text(font, al_map_rgb(100, 200, 100), (2*CELL_SIZE), 4*CELL_SIZE, ALLEGRO_ALIGN_LEFT, str_cave_number);
    al_draw_text(font, al_map_rgb(100, 200, 100), (2*CELL_SIZE), 5*CELL_SIZE, ALLEGRO_ALIGN_LEFT, curr_cave->cave_name);

    al_draw_bitmap(new_game, (7*CELL_SIZE), 6*CELL_SIZE, 0);
    al_draw_bitmap(exit_game, (10*CELL_SIZE), 6*CELL_SIZE, 0);

    al_flip_display();/*rendering                                           */

    /* current screen stays until the user make a decision.                 */
    while(1){
        al_wait_for_event(event_queue ,&ev);
        if(ev.type==ALLEGRO_EVENT_DISPLAY_CLOSE){
            break;
        }
        else if(ev.type==ALLEGRO_EVENT_KEY_DOWN){
            if(ev.keyboard.keycode==ALLEGRO_KEY_ESCAPE)
                break;
        }
        else if(ev.type==ALLEGRO_EVENT_MOUSE_AXES){
            mouse_pos.c=ev.mouse.x;
            mouse_pos.r=ev.mouse.y;
        }
        else if(ev.type==ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
            /*new game button   */
            if( mouse_pos.r>(6*CELL_SIZE) && mouse_pos.r<(8*CELL_SIZE) &&
                mouse_pos.c>(10*CELL_SIZE) && mouse_pos.c<(12*CELL_SIZE)
            ){
                break;
            }
            /*exit button       */
            else if(mouse_pos.r>(6*CELL_SIZE) && mouse_pos.r<(8*CELL_SIZE) &&
                    mouse_pos.c>(7*CELL_SIZE) && mouse_pos.c<(9*CELL_SIZE)
            ){
                play_game(g);
            }
        }
    }

    return;
}
