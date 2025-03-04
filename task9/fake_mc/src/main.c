#include <curses.h>
#include <consts.h>
#include <my_funcs.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <path.h>
#include <indexes.h>

int main()
{
    //Init
    WINDOW* wnd_l, *wnd_r, *text_l, *text_r;
    initscr(); cbreak(); noecho(); keypad(stdscr, TRUE); curs_set(0);
    refresh();

    //Path preparation
    struct dirent** namelist;
    struct PATH path_l;
    struct PATH path_r;
    if(path_init(&path_l, "/") < 0) {
        fprintf(stderr, "Init path: %s", strerror(errno));
        free(path_l.path);
        exit(1);
    }
    if(path_init(&path_r, "/") < 0) {
        fprintf(stderr, "Init path: %s", strerror(errno));
        free(path_r.path);
        exit(1);
    }
    int ndirs = scandir(path_l.path, &namelist, 0, alphasort);
    if(ndirs < 0) {
        fprintf(stderr, "Scanning dirs: %s", strerror(errno));
        path_kill(&path_l);
        exit(1);
    }

    //Windows preparation
    int cols, rows;
    getmaxyx(stdscr, rows, cols);
    wnd_l = newwin(rows, cols/2, 0, 0);
    wnd_r = newwin(rows, cols - cols/2, 0, cols/2);
    box(wnd_l, '|', '-');
    box(wnd_r, '|', '-');
    wrefresh(wnd_l);
    wrefresh(wnd_r);
    refresh();
    text_l = derwin(wnd_l, rows-2, cols/2-2, 1, 1);
    text_r = derwin(wnd_r, rows-2, cols/2-2, 1, 1);
    
    //Print preparation
    int target_l = 0, target_r = 0;
    struct index ind_l;
    struct index ind_r;
    ind_l.up = 0; ind_r.up = 0;
    ind_l.down = rows-2; ind_r.down = rows-2;
    print_dirs(text_l, namelist, &path_l, ndirs, target_l, &ind_l);
    wrefresh(text_l);
    print_dirs(text_r, namelist, &path_r, ndirs, target_r, &ind_r);
    wrefresh(text_r);

    //Main cycle
    int key = 0, cnd = 0;
    while(1) {
        
        switch (key)
        {
            case KEY_DOWN:
                if(cnd)
                    move_down(text_r, namelist, &path_r, ndirs, &target_r, &ind_r);
                else
                    move_down(text_l, namelist, &path_l, ndirs, &target_l, &ind_l);
                break;
            
            case KEY_UP:
                if(cnd)
                    move_up(text_r, namelist, &path_r, ndirs, &target_r, &ind_r);
                else
                    move_up(text_l, namelist, &path_l, ndirs, &target_l, &ind_l);
                break;
            
            case '\n':
                if(cnd) {

                    if(open_dir(text_r, &namelist, &ndirs, &target_r, &path_r, &ind_r) < 0)
                        exit(1);
                }
                else {
                    if(open_dir(text_l, &namelist, &ndirs, &target_l, &path_l, &ind_l) < 0)
                        exit(1);
                }
                break;
            case '\t':
                if(cnd) {
                    cnd = 0;
                    for(int i = 0; i < ndirs; i++) {
                        free(namelist[i]);
                    }
                    free(namelist);
                    ndirs = scandir(path_l.path, &namelist, 0, alphasort);
                    if(ndirs < 0) {
                        fprintf(stderr, "Scanning dirs: %s", strerror(errno));
                        path_kill(&path_l);
                        exit(1);
                    }
                } else {
                    cnd = 1;
                    for(int i = 0; i < ndirs; i++) {
                        free(namelist[i]);
                    }
                    free(namelist);
                    ndirs = scandir(path_r.path, &namelist, 0, alphasort);
                    if(ndirs < 0) {
                        fprintf(stderr, "Scanning dirs: %s", strerror(errno));
                        path_kill(&path_r);
                        exit(1);
                    }
                }
                break;
            case 'q':
                goto LOOP_END;
            default:
                break;
        }
        
        key = getch();
    }
    // Ending
LOOP_END:
    delwin(wnd_l);
    delwin(wnd_r);
    path_kill(&path_l);
    endwin();
    for(int i = 0; i < ndirs; i++) {
        free(namelist[i]);
    }
    free(namelist);
    return 0;
}
