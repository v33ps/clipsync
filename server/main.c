#include <stdio.h>
#include <string.h>
#include "comms.h"

#include <gdk/gdk.h>
#include <gtk/gtk.h>

gchar* gnew_clipboard_text = {0};

void text_request_callback(GtkClipboard *clipboard,
                           const gchar *text,
                           gpointer data)
{
    // To demonstrate setting a new value into the clipboard, we
    // choose some text.
    //const gchar* new_clipboard_text = "Clipboard test 1";
    if(text == 0 || g_utf8_collate(text, gnew_clipboard_text) == 0)
    {
        // If the program was already run, and the clipboard contains
        // our string already, use a different string. This way when
        // you run the program multiple times in a row, you'll see the
        // string changing.
        gnew_clipboard_text = "Clipboard test 2";
    }

    // This sets the text. I'm passing -1 because I'm lazy -- the
    // function will call strlen on the string to figure out how long
    // it is.
    gtk_clipboard_set_text(clipboard, gnew_clipboard_text, -1);

    // Display the content and the contents of the variable we passed
    // in.
    printf("Clipboard text was %s, value is %8X\n",
           text, *(int*)data);

    // Now that we've monkeyed with the clipboard, our job is done
    // here.
    gtk_main_quit();
}


int main(int argc, char *argv[]) {
    if(argc < 2) {
        printf("need port number\n");
        exit(1);
    }
    int sockfd = 0;
    startServer(argv[1], &sockfd);
    struct sockaddr_in clientAddr;
	int cliAddrLen = 0;

    // Standard boilerplate: initialize the toolkit.
    gtk_init(&argc, &argv);
    // Get a handle to the given clipboard. You can also ask for
    // GDK_SELECTION_PRIMARY (the X "primary selection") or
    // GDK_SELECTION_SECONDARY.
    GtkClipboard* clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);

	while(1) {
        int cliSock = 0;
        if (0 > (cliSock = accept(sockfd, 
                                (struct sockaddr *)&clientAddr, 
                                &cliAddrLen))) {
			printf("could not open a socket to accept data\n");
			return 1;
		}
        int n = 0;
		int len = 0, maxlen = 100;
		char buffer[maxlen];
        int value = 0xDECAFBAD;

		printf("client connected with ip address: %s\n",
		       inet_ntoa(clientAddr.sin_addr));

		while ((n = recv(cliSock, buffer, maxlen, 0)) > 0) {
			maxlen -= n;
			len += n;

			printf("received: '%s'\n", buffer);
            // since @gnew_clipboard_text is a global, we must calloc
            // here instead of at definition.
            gnew_clipboard_text = calloc(500, sizeof gnew_clipboard_text);
            // copy the value we got from client 
            strncpy(gnew_clipboard_text, buffer, maxlen);

            // these two run to set the clipboard content. This happens in
            // text_request_callback()
            gtk_clipboard_request_text(clipboard, text_request_callback, &value);
            gtk_main();
            // zero the buffer so we don't get repeat data
            memset(&buffer, 0, sizeof(buffer));
		}

		close(cliSock);
    }

    close(sockfd);

	return 0;
}
