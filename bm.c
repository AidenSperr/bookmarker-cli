#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>

#define MAX_BOOKMARKS 256
#define MAX_LINE 512

typedef struct {
	char name[128];
	char path[356];
} Bookmark;

int load_bookmarks(const char* filepath, Bookmark* list, int max);
void get_bookmarks_path(char* buffer, size_t buffer_size);
void list_bookmarks(Bookmark* list, int count);
const char* find_bookmark(Bookmark* list, int count, const char* name);
int save_bookmarks(const char* filepath, Bookmark* list, int count);
void get_current_directory(char* buffer, size_t buffer_size);
int remove_bookmark(Bookmark* list, int count, const char* name);

int main(int argc, char* argv[])
{
	if (argc < 2) {
		fprintf(stderr, "usage: bm <add|get|list|rm> ...\n");
		return 1;
	}

	char bookmarks_path[MAX_LINE];
	get_bookmarks_path(bookmarks_path, sizeof(bookmarks_path));

	Bookmark list[MAX_BOOKMARKS];
	int count = load_bookmarks(bookmarks_path, list, MAX_BOOKMARKS);

	if (strcmp(argv[1], "list") == 0) {
		list_bookmarks(list, count);
	}
	else if (strcmp(argv[1], "get") == 0) {
		if (argc < 3) {
			fprintf(stderr, "usage: bm get <name>\n");
			return 1;
		}

		const char* path = find_bookmark(list, count, argv[2]);
		if (path) {
			printf("%s\n", path);
		}
		else {
			fprintf(stderr, "no bookmark named '%s'\n", argv[2]);
			return 1;
		}
	}
	else if (strcmp(argv[1], "add") == 0) {
		if (argc < 3) {
			fprintf(stderr, "usage: bm add <name> [path]\n");
			return 1;
		}

		char path_buffer[MAX_LINE];
		const char* new_path;
		if (argc >= 4) {
			new_path = argv[3];
		}
		else {
			get_current_directory(path_buffer, sizeof(path_buffer));
			new_path = path_buffer;
		}

		// Get the full path of 'new_path'
		char resolved[MAX_LINE];
		if (_fullpath(resolved, new_path, sizeof(resolved)) != NULL) {
			new_path = resolved;
		}

		// Check if this already exists and replace it if yes
		int found = 0;
		for (int i = 0; i < count; i++) {
			if (strcmp(list[i].name, argv[2]) == 0) {
				strncpy(list[i].path, new_path, sizeof(list[i].path) - 1);
				list[i].path[sizeof(list[i].path) - 1] = '\0';
				found = 1;
				break;
			}
		}

		// If a new name, append if there's room
		if (!found) {
			if (count >= MAX_BOOKMARKS) {
				fprintf(stderr, "bookmark list is full\n");
				return 1;
			}
			strncpy(list[count].name, argv[2], sizeof(list[count].name) - 1);
			list[count].name[sizeof(list[count].name) - 1] = '\0';
			strncpy(list[count].path, new_path, sizeof(list[count].path) - 1);
			list[count].path[sizeof(list[count].path) - 1] = '\0';
			count++;
		}

		if (!save_bookmarks(bookmarks_path, list, count)) {
			return 1;
		}
		printf("bookmarked '%s' -> %s\n", argv[2], new_path);
	}
	else if (strcmp(argv[1], "rm") == 0) {
		if (argc < 3) {
			fprintf(stderr, "usage: bm rm <name>\n");
			return 1;
		}

		int new_count = remove_bookmark(list, count, argv[2]);
		if (new_count == -1) {
			fprintf(stderr, "no bookmark name '%s'\n", argv[2]);
			return 1;
		}

		if (!save_bookmarks(bookmarks_path, list, new_count)) {
			return 1;
		}
		printf("removed '%s'\n", argv[2]);
	}
	else {
		fprintf(stderr, "unknown command: %s\n", argv[1]);
		return 1;
	}

	return 0;
}

// reads -/.bookmarks into 'list'
// returns the number of entries it found
int load_bookmarks(const char* filepath, Bookmark* list, int max)
{
	// Make the file pointer and open the file at the passed-in filepath for reading
	FILE* file = fopen(filepath, "r");

	// Buffers for each line
	char line[MAX_LINE];

	// Number of bookmarks found
	int num_bookmarks = 0;

	// Check if the file was open successfully
	if (file != NULL) {
		// Read each line and store it in the line buffer
		// If 'num_bookmarks' exceeds 'max', then stop the loop
		while (num_bookmarks < max && fgets(line, sizeof(line), file)) {
			// Strip off the trailing character (typically \n or \r\n)
			size_t len = strlen(line);
			while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
				line[--len] = '\0';
			}

			// Split the line into two tokens, store in 'b_name' and 'b_path'
			char* b_name = strtok(line, "\t");
			char* b_path = strtok(NULL, "");
			if (!b_name || !b_path) {
				continue;
			}

			// Initialize the new bookmark struct
			Bookmark new_bm;
			strncpy(new_bm.name, b_name, sizeof(new_bm.name) - 1);
			new_bm.name[sizeof(new_bm.name) - 1] = '\0';
			strncpy(new_bm.path, b_path, sizeof(new_bm.path) - 1);
			new_bm.path[sizeof(new_bm.path) - 1] = '\0';

			// Put the new bookmark in the list, then increment 'num_bookmarks'
			list[num_bookmarks] = new_bm;
			num_bookmarks++;
		}

		// Close the file after reading every line
		fclose(file);

		return num_bookmarks;
	}
	else {
		// Return 0 if no bookmarks
		return 0;
	}
}

// snprintf's the bookmark path
void get_bookmarks_path(char* buffer, size_t buffer_size)
{
	const char* home = getenv("USERPROFILE");
	if (!home) {
		// Use the current directory as a fallback instead
		home = ".";
	}

	snprintf(buffer, buffer_size, "%s\\.bookmarks", home);
}

// Prints all bookmarks
void list_bookmarks(Bookmark* list, int count)
{
	if (count == 0) {
		printf("No bookmarks yet. Use 'bm add <name>' to create one.\n");
		return;
	}

	for (int i = 0; i < count; i++) {
		printf("%s | %s\n", list[i].name, list[i].path);
	}
}

// Finds a specific bookmark by its name and returns its path
const char* find_bookmark(Bookmark* list, int count, const char* name)
{
	for (int i = 0; i < count; i++) {
		if (strcmp(list[i].name, name) == 0) {
			return list[i].path;
		}
	}

	return NULL;
}

// Writes all 'count' entries in 'list' out to 'filepath', with one "name\tpath" pair per line
// Returns 1 on success and 0 on failure
int save_bookmarks(const char* filepath, Bookmark* list, int count)
{
	// Make the file pointer and open the file at the passed-in filepath for writing
	FILE* file = fopen(filepath, "w");

	// Check if file wasn't opened
	if (file != NULL) {
		// Loop through each bookmark in the list and write it to the file in its format
		for (int i = 0; i < count; i++) {
			fprintf(file, "%s\t%s\n", list[i].name, list[i].path);
		}

		// Close the file when done and return
		fclose(file);
		return 1;
	}
	else {
		// Print to stderr if the fopen() failed and return
		fprintf(stderr, "failed to open file at path %s", filepath);
		return 0;
	}
}

// Gets the current working directory
void get_current_directory(char* buffer, size_t buffer_size)
{
	if (_getcwd(buffer, (int)buffer_size) == NULL) {
		buffer[0] = '\0';
	}
}

// Removes the bookmark named 'name' from 'list'
// Returns the new count if found and removed, or -1 otherwise
int remove_bookmark(Bookmark* list, int count, const char* name)
{
	for (int i = 0; i < count; i++) {
		if (strcmp(list[i].name, name) == 0) {
			// Found a match
			// Loop until end of list and decrement each item
			for (int j = i; j < count - 1; j++) {
				list[j] = list[j + 1];
			}

			return count - 1;
		}
	}

	return -1;
}