#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BOOKMARKS 256
#define MAX_LINE 512

typedef struct {
	char name[128];
	char path[356];
} Bookmark;

// reads -/.bookmarks into 'list'
// returns the number of entries it found
int load_bookmarks(const char* filepath, Bookmark* list, int max);

int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(stderr, "usage: bm <add|get|list|rm> ...\n");
		return 1;
	}

	// TODO: figure out bookmarks file path
	// TODO: dispatch on argv[1]

	return 0;
}

int load_bookmarks(const char* filepath, Bookmark* list, int max) {
	FILE* fptr = fopen(filepath, "r");

}