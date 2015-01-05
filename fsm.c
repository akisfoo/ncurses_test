#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <curses.h>
#include <menu.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define ENTER  10

void mn_init(void);
void opt_init(void);
void init_curses_blue(void);
void init_curses_red(void);

ITEM **items, *cur_item;
WINDOW *mn_win;
MENU *menu;
int noptions;

char *options[] = { "Write File", 
				   "View File", 
				   "Clean File", 
				   "About" };

char *descrpt[] = { "Writing file... Done",
				   "File Contents : ", 
				   "Cleaning file... Done", 
				   "About what?" };

const char *help = "-r\tfor the bluephobic\n"
				"-b\tfor blue\n"
				"-h\tthis help\n";

int main(int argc, char **argv) {
	int i;

	if(argc != 2) {
		printf("usage: ./fsm -<color>\n");
		return EXIT_FAILURE;
	}

	for(i=1; i<argc; i++) {
		if(argv[i][0] == '-') {
			switch(argv[i][1]) {
				case 'r':
					init_curses_red();
					bkgd(COLOR_PAIR(2));
					break;

				case 'b':
					init_curses_blue();
					bkgd(COLOR_PAIR(2));
					break;

				case 'h':
					printf("%s", help);
					return 0;

				default:
					fprintf(stderr, "invalid option: '%s'\n", argv[i]);
					return EXIT_FAILURE;
			}
		} else {
			fprintf(stderr, "invalid argument: '%s'\n", argv[i]);
			return EXIT_FAILURE;
		}

	}

	mn_init();
	opt_init();
	endwin();

	return EXIT_SUCCESS;
}

void mn_init(void) {
	int i;

	noptions = ARRAY_SIZE(options);
	items = (ITEM **)calloc(noptions, sizeof(ITEM *));
	
	for(i=0; i<noptions; i++)
		items[i] = new_item(options[i], descrpt[i]);

	/* create menu */
	menu = new_menu((ITEM **)items);
	/* set off menu description */
	menu_opts_off(menu, O_SHOWDESC);
	mn_win = newwin(10, 20, 4, 4);
	keypad(mn_win, TRUE);

	set_menu_win(menu, mn_win);
	set_menu_sub(menu, derwin(mn_win, 6, 18, 3, 1));
	set_menu_format(menu, 4, 1);
	set_menu_mark(menu, "-> ");
	box(mn_win, 0, 0);

	attron(COLOR_PAIR(2));
	
	mvprintw(LINES - 2, 1, "A small program for testing ncurses");
	mvprintw(LINES - 5, 1, "The Fold - Foo Storage Mechanism\n <F10> to Exit");
	attroff(COLOR_PAIR(2));

	refresh();
}

void opt_init(void) {
	FILE *fp;
	int key, i;
	char ch;

	post_menu(menu);
	wrefresh(mn_win);	
	
	while((key = wgetch(mn_win)) != KEY_F(10)) {
		switch(key) {
			case KEY_DOWN:
				menu_driver(menu, REQ_DOWN_ITEM);
				break;
				
			case KEY_UP:
				menu_driver(menu, REQ_UP_ITEM);
				break;
				
			case KEY_LEFT: 
				menu_driver(menu, REQ_LEFT_ITEM);
				break;
				
			case KEY_RIGHT:
				menu_driver(menu, REQ_RIGHT_ITEM);
				break;
				
			case KEY_NPAGE:
				menu_driver(menu, REQ_SCR_DPAGE);
				break;
				
			case KEY_PPAGE:
				menu_driver(menu, REQ_SCR_UPAGE);
				break;
				
			case ENTER:
				cur_item = current_item(menu);
				move(LINES - 2, 0);
				clrtoeol();
				
				/* menu options */
				switch(item_index(cur_item)+1) {
					case 1:
						mvprintw(LINES - 2, 3, "%s", item_description(cur_item));
						if(!(fp = fopen("foo", "w"))) { 
							/* print on a ncurses panel or something the stderr */
							fprintf(stderr, "opt_init: can't write file: %s\n", strerror(errno));
						} else {									
							fprintf(fp, "The Dude abides.");
							fclose(fp);
						}
						
					case 2:
						mvprintw(LINES - 2, 3, "%s", item_description(cur_item));
						if((fp = fopen("foo", "r")) == NULL) {
							fprintf(stderr, "opt_init: can't open file: %s\n", strerror(errno)); /* put %s filename */
						} else {
							while((ch = fgetc(fp)) != EOF) {
								printw("%c", ch);
							}
						}

						fclose(fp);
						
					case 3:
						mvprintw(LINES - 2, 3, "%s", item_description(cur_item));
						if(!(fp = fopen("foo", "w"))) {
							fprintf(stderr, "opt_init: can't open file: %s\n", strerror(errno));
						} else {
							fprintf(fp, NULL);
							fclose(fp);
						}
						
					case 4:
						mvprintw(LINES - 2, 3, "%s", item_description(cur_item));
				}

				refresh();
				pos_menu_cursor(menu);
				break;
		}

		wrefresh(mn_win);
	}

	unpost_menu(menu);
	free_menu(menu);	
	
	for(i=0; i<noptions; i++)
		free_item(items[i]);
}

void init_curses_blue(void) {
	initscr();	
	raw();
	
	start_color();
	init_pair(1,COLOR_WHITE,COLOR_BLACK);
	init_pair(2,COLOR_WHITE,COLOR_BLUE);
	init_pair(3,COLOR_BLACK,COLOR_WHITE);
	init_pair(4,COLOR_BLUE,COLOR_WHITE);
	
	curs_set(0); 						
	noecho();
	keypad(stdscr,TRUE);
}

void init_curses_red(void) {
	initscr();	
	raw();
	
	start_color();
	init_pair(1,COLOR_WHITE,COLOR_BLACK);
	init_pair(2,COLOR_WHITE,COLOR_RED);
	init_pair(3,COLOR_BLACK,COLOR_WHITE);
	init_pair(4,COLOR_RED,COLOR_WHITE);
	
	curs_set(0); 						
	noecho();
	keypad(stdscr,TRUE);
}
