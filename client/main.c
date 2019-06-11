#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <errno.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>


// most of the clipboard stuff was taken from:
// https://github.com/bstpierre/gtk-examples/blob/master/c/

// global that holds the clipboard contents
gchar *gtext = { 0 };

int client_setup(int *sockfd, struct sockaddr_in *servaddr, char* port, char* ip);
void text_request_callback(GtkClipboard *clipboard, const gchar *text, gpointer data);

int main(int argc, char *argv[])
{
    if(argc < 2) {
        printf("clipsync ip port\n");
        exit(1);
    }

    int sockfd = 0;
    struct sockaddr_in servaddr = { 0 };

    // open a socket to the destination server
     if(client_setup(&sockfd, &servaddr, argv[2], argv[1]) < 0) {
        exit(0);
    }

    gtk_init(&argc, &argv);

    // Get a handle to the given clipboard. 
    GtkClipboard* clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);

    // This is just an arbitrary value to pass through to the callback. 
    int value = 0xDECAFBAD;

    // call text_request_callback to get the text from the clipboard
    gtk_clipboard_request_text(clipboard, text_request_callback, &value);

    // We have to run the GTK main loop so that the events required to
    // fetch the clipboard contents can be processed.
    gtk_main();
    
    // now that we have the clipboard content write it to the server
    if((write(sockfd, gtext, sizeof(gtext)) < 0) {
        printf("error syncing clipboard: %s\n", strerror(errno));
        exit(1);
    }

    return 0;
}


// build the socket and connect to the server
int client_setup(int *sockfd, struct sockaddr_in *servaddr, char* port, char* ip)
{ 
    struct hostent *he { 0 };
    struct sockaddr_in dest_addr = { 0 };

    // make sure the destination address if valid
    if ((he=gethostbyname(ip)) == NULL) {
        herror("gethostbyname");
        return -1;
    }

    // create the socket to connect to the destination server
     if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return -1;
    }

    dest_addr.sin_family = AF_INET;      // host byte order
    dest_addr.sin_port = htons(atoi(port));    // short, network byte order 
    dest_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(dest_addr.sin_zero), 8);     // zero the rest of the struct

    // connect to the server
    if (connect(*sockfd, (struct sockaddr *)&dest_addr, \
                                            sizeof(struct sockaddr)) == -1) {
        perror("connect");
        return -1;
    }
    return 0;
}

void text_request_callback(GtkClipboard *clipboard, const gchar *text, gpointer data)
{
    // since @gtext is a global, calloc in here
    gtext = calloc(500, sizeof gtext);

    // get the clipboard text
    gtext = gtk_clipboard_wait_for_text(clipboard);

    printf("Clipboard text was %s, value is %8X\n",
           gtext, *(int*)data);

    // we are done using the clipboard so clean up
    gtk_main_quit();
}
