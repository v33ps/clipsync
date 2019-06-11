#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <errno.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

#define MAX_SIZE    1024
#define PORT        4445

// create a global that will contain what @text contains now
// this way we can get it out of text_request_callback
// and write it over the socket
gchar *gtext = { 0 };

void text_request_callback(GtkClipboard *clipboard,
                           const gchar *text,
                           gpointer data)
{
    // To demonstrate setting a new value into the clipboard, we
    // choose some text.
    gtext = calloc(500, sizeof gtext);
    gtext = gtk_clipboard_wait_for_text(clipboard);

    printf("Clipboard text was %s, value is %8X\n",
           gtext, *(int*)data);

    gtk_main_quit();
}


int client_setup(int *sockfd, struct sockaddr_in *servaddr);
void teardown(int sockfd, FILE *fp);

int main(int argc, char *argv[])
{
    // define a buffer of max size
    unsigned char buf[MAX_SIZE] = { 0 };
    int bytes_read = 0;
    int bytes_written = 0;
    int sockfd = 0;
    struct sockaddr_in servaddr = { 0 };
    // open a socket to the destination server
     if(client_setup(&sockfd, &servaddr) < 0) {
        exit(0);
    }

    gtk_init(&argc, &argv);

    // Get a handle to the given clipboard. You can also ask for
    // GDK_SELECTION_PRIMARY (the X "primary selection") or
    // GDK_SELECTION_SECONDARY.
    GtkClipboard* clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);

    // This is just an arbitrary value to pass through to the
    // callback. You could pass a pointer to a local struct or
    // something similar if that was useful to you.
    int value = 0xDECAFBAD;

    // There are more complex things you can place in the clipboard,
    // but this demonstrates text. The callback will be invoked when
    // the clipboard contents has been received.
    //
    // For a much simpler method of getting the text in the clipboard,
    // see gtk_clipboard_wait_for_text(), which is used in the example
    // program clipboard_watch.
    gtk_clipboard_request_text(clipboard, text_request_callback, &value);

    // We have to run the GTK main loop so that the events required to
    // fetch the clipboard contents can be processed.
    gtk_main();
    write(sockfd, gtext, sizeof(gtext));

    return 0;
}


// build the socket and connect to the server
int client_setup(int *sockfd, struct sockaddr_in *servaddr)
{ 
    struct hostent *he;
    struct sockaddr_in dest_addr;

    // make sure the destination address if valid
    if ((he=gethostbyname("127.0.0.1")) == NULL) {
        herror("gethostbyname");
        return -1;
    }

    // create the socket to connect to the destination server
     if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return -1;
    }

    dest_addr.sin_family = AF_INET;      // host byte order
    dest_addr.sin_port = htons(PORT);    // short, network byte order 
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

void teardown(int sockfd, FILE *fp) 
{
    printf("teardown\n");
    close(sockfd);
    fclose(fp);
}