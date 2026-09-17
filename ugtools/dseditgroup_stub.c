#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GROUP_FILE "/private/etc/group"

void append_group(const char *name, const char *gid) {
    FILE *f = fopen(GROUP_FILE, "a");
    if (!f) { perror("fopen"); exit(1); }
    fprintf(f, "%s:*:%s:\n", name, gid);
    fclose(f);
}

int check_member(const char *user, const char *group) {
    FILE *f = fopen(GROUP_FILE, "r");
    if (!f) return 1;
    char line[1024];
    int found = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *tok = strtok(line, ":");
        if (tok && strcmp(tok, group) == 0) {
            strtok(NULL, ":"); // pass
            strtok(NULL, ":"); // gid
            char *members = strtok(NULL, ":");
            if (members) {
                char *m = strtok(members, ",");
                while (m) {
                    if (strcmp(m, user) == 0) found = 0;
                    m = strtok(NULL, ",");
                }
            }
        }
    }
    fclose(f);
    return found;
}

void add_member(const char *user, const char *group) {
    FILE *f = fopen(GROUP_FILE, "r");
    if (!f) { perror("fopen"); exit(1); }
    char new_content[65536] = {0};
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        char orig_line[1024];
        strcpy(orig_line, line);
        orig_line[strcspn(orig_line, "\n")] = 0;
        
        char copy[1024];
        strcpy(copy, orig_line);
        char *tok = strtok(copy, ":");
        if (tok && strcmp(tok, group) == 0) {
            strtok(NULL, ":"); // pass
            strtok(NULL, ":"); // gid
            char *members = strtok(NULL, ":");
            if (members && strlen(members) > 0) {
                sprintf(orig_line + strlen(orig_line), ",%s", user);
            } else {
                sprintf(orig_line + strlen(orig_line), "%s", user);
            }
        }
        strcat(new_content, orig_line);
        strcat(new_content, "\n");
    }
    fclose(f);
    
    f = fopen(GROUP_FILE, "w");
    if (!f) { perror("fopen"); exit(1); }
    fputs(new_content, f);
    fclose(f);
}

int main(int argc, char **argv) {
    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            if (strcmp(argv[i+1], "create") == 0) {
                const char *gid = "0";
                const char *name = argv[argc-1];
                for (int j = 1; j < argc; j++) {
                    if (strcmp(argv[j], "-i") == 0 && j + 1 < argc) {
                        gid = argv[j+1];
                    }
                }
                append_group(name, gid);
                return 0;
            }
            if (strcmp(argv[i+1], "checkmember") == 0) {
                const char *user = NULL;
                const char *group = argv[argc-1];
                for (int j = 1; j < argc; j++) {
                    if (strcmp(argv[j], "-m") == 0 && j + 1 < argc) {
                        user = argv[j+1];
                    }
                }
                return check_member(user, group);
            }
            if (strcmp(argv[i+1], "edit") == 0) {
                const char *user = NULL;
                const char *group = argv[argc-1];
                for (int j = 1; j < argc; j++) {
                    if (strcmp(argv[j], "-a") == 0 && j + 1 < argc) {
                        user = argv[j+1];
                    }
                }
                add_member(user, group);
                return 0;
            }
        }
    }
    return 0;
}
