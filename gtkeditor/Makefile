# Generated automatically from Makefile.in by configure.
CC = gcc
GTK_CFLAGS = -I/usr/lib/glib/include -I/usr/X11R6/include
GTK_CONFIG = /usr/bin/gtk-config
GTK_LIBS   = -L/usr/lib -L/usr/X11R6/lib -lgtk -lgdk -rdynamic -lgmodule -lglib -ldl -lXi -lXext -lX11 -lm

first_target: all
all:	
	$(CC) -c edit.c $(GTK_CFLAGS)
	$(CC) -c gtkeditor.c $(GTK_CFLAGS)
	$(CC) -c highlight.c $(GTK_CFLAGS)
	$(CC) -c plsql.c $(GTK_CFLAGS)
	$(CC) -c scanner.c $(GTK_CFLAGS)
	$(CC) -c orasoft_gtkeditor.c $(GTK_CFLAGS)
	ar rcs libgtkeditor.a edit.o gtkeditor.o highlight.o plsql.o scanner.o orasoft_gtkeditor.o
	chmod 755 libgtkeditor.a
objectman:
	$(CC) -c edit.c $(GTK_CFLAGS)
	$(CC) -c gtkeditor.c $(GTK_CFLAGS)
	$(CC) -c highlight.c $(GTK_CFLAGS)
	$(CC) -c plsql.c $(GTK_CFLAGS)
	$(CC) -c scanner.c $(GTK_CFLAGS)
	$(CC) -c orasoft_gtkeditor.c $(GTK_CFLAGS)
	ar rcs libgtkeditor.a edit.o gtkeditor.o highlight.o plsql.o scanner.o orasoft_gtkeditor.o
	chmod 755 libgtkeditor.a

clean:	
	rm *.o *.a

install:
	@echo Installing
