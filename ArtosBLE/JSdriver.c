#include <stdio.h>
#include <X11/Xlib.h>
#include <stdlib.h>
#include "../molib.h"
#include "../btlib.h"
#define FOUNDED_NODE 2
#define UP 1
#define DOWN 2
#define RIGHT 32
#define LEFT 16
#define CENTER 0

Display *display;
Window root;
Window child;

int notify_callback(int lenode, int cticn, unsigned char* buf, int nread)
{
	int x=0,y=0;

	unsigned int mask;
	
    if (!XQueryPointer(display, root, &root, &child, &x, &y, &mask, &mask, &mask)) 
    {
        fprintf(stderr, "Unable to query pointer position\n");
        XCloseDisplay(display);
        return 0;
    }
    
	if(buf[0] == CENTER)
	{
		printf("[C]");
	}
	else if(buf[0] == UP)
	{
		printf("[U]");
		y -= 50;
	}
	else if(buf[0] == DOWN)
	{
		printf("[D]");
		y+=50;
	}
	else if(buf[0] == RIGHT)
	{
		printf("[R]");
		x += 50;
	}
	else if(buf[0] == LEFT)
	{
		x -= 50;
		printf("[L]");
	}
	printf("(%d)\n",buf[1]);

    XWarpPointer(display, None, root, 0, 0, 0, 0, x, y);

    // Flush the request to the server
    XFlush(display);
	return 1;
}
int main()
{
	
    // Open the display
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Unable to open X display\n");
        return 1;
    }

    // Get the root window
    root = DefaultRootWindow(display);


	int flag = 0;
	
	
	if(init_blue("devices.txt") == 0)
	{
		return(0);
	}
	flag |= BOND_NEW;
	flag |= PASSKEY_LOCAL | JUST_WORKS;
	le_scan();
	
	
	set_le_wait(750);
	connect_node(FOUNDED_NODE,CHANNEL_LE,0);
	set_le_wait(750);
    le_pair(FOUNDED_NODE,flag,0);
	
	//[!] Check if found
	//le_pair(FOUNDED_NODE,flag,0);
	//set_le_wait(10000);
	find_ctics(FOUNDED_NODE);
	list_ctics(FOUNDED_NODE,LIST_SHORT | CTIC_R);
	
	//devlist(0);
	/*
	notify_ctic(FOUNDED_NODE,7,NOTIFY_ENABLE,notify_callback);
	notify_ctic(FOUNDED_NODE,9,NOTIFY_ENABLE,notify_callback);
	notify_ctic(FOUNDED_NODE,8,NOTIFY_ENABLE,notify_callback);
*/
	notify_ctic(FOUNDED_NODE,7,NOTIFY_ENABLE,notify_callback);

	
	read_notify(200000);
	printf("Done\n");
	getchar();
	disconnect_node(FOUNDED_NODE);
	
    // Close the display
    XCloseDisplay(display);

	return 1;
	
}
