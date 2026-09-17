#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#define PASSWD_FILE "/private/etc/passwd"
#define GROUP_FILE "/private/etc/group"

void read_group(const char *name, const char *attr) {
    FILE *f = fopen(GROUP_FILE, "r");
    if (!f) exit(1);
    char line[1024];
    int found = 0;
    while (fgets(line, sizeof(line), f)) {
        char copy[1024];
        strcpy(copy, line);
        copy[strcspn(copy, "\n")] = 0;
        char *tok = strtok(copy, ":");
        if (tok && strcmp(tok, name) == 0) {
            found = 1;
            if (attr) {
                if (strcmp(attr, "PrimaryGroupID") == 0) {
                    strtok(NULL, ":"); // pass
                    char *gid = strtok(NULL, ":");
                    if (gid) printf("PrimaryGroupID: %s\n", gid);
                }
            } else {
                printf("%s", line);
            }
        }
    }
    fclose(f);
    exit(found ? 0 : 1);
}

void read_user(const char *name, const char *attr) {
    FILE *f = fopen(PASSWD_FILE, "r");
    if (!f) exit(1);
    char line[1024];
    int found = 0;
    while (fgets(line, sizeof(line), f)) {
        char copy[1024];
        strcpy(copy, line);
        copy[strcspn(copy, "\n")] = 0;
        char *tok = strtok(copy, ":");
        if (tok && strcmp(tok, name) == 0) {
            found = 1;
            if (attr) {
                strtok(NULL, ":"); // pass
                char *uid = strtok(NULL, ":");
                char *gid = strtok(NULL, ":");
                char *gecos = strtok(NULL, ":");
                char *home = strtok(NULL, ":");
                char *shell = strtok(NULL, ":");
                if (strcmp(attr, "UniqueID") == 0 && uid) printf("UniqueID: %s\n", uid);
                else if (strcmp(attr, "PrimaryGroupID") == 0 && gid) printf("PrimaryGroupID: %s\n", gid);
                else if (strcmp(attr, "RealName") == 0 && gecos) printf("RealName: %s\n", gecos);
                else if (strcmp(attr, "NFSHomeDirectory") == 0 && home) printf("NFSHomeDirectory: %s\n", home);
                else if (strcmp(attr, "UserShell") == 0 && shell) printf("UserShell: %s\n", shell);
                else if (strcmp(attr, "IsHidden") == 0) printf("IsHidden: 1\n");
            } else {
                printf("%s", line);
            }
        }
    }
    fclose(f);
    exit(found ? 0 : 1);
}

void update_user(const char *name, const char *attr, const char *value) {
    FILE *f = fopen(PASSWD_FILE, "r");
    if (!f) { perror("fopen"); exit(1); }
    char new_content[65536] = {0};
    char line[1024];
    int found = 0;
    while (fgets(line, sizeof(line), f)) {
        char copy[1024];
        strcpy(copy, line);
        copy[strcspn(copy, "\n")] = 0;
        char *p_name = strtok(copy, ":");
        if (p_name && strcmp(p_name, name) == 0) {
            found = 1;
            char *p_pass = strtok(NULL, ":");
            char *p_uid = strtok(NULL, ":");
            char *p_gid = strtok(NULL, ":");
            char *p_gecos = strtok(NULL, ":");
            char *p_home = strtok(NULL, ":");
            char *p_shell = strtok(NULL, ":");
            
            if (attr && value) {
                if (strcmp(attr, "UniqueID") == 0) p_uid = (char*)value;
                else if (strcmp(attr, "PrimaryGroupID") == 0) p_gid = (char*)value;
                else if (strcmp(attr, "RealName") == 0) p_gecos = (char*)value;
                else if (strcmp(attr, "NFSHomeDirectory") == 0) p_home = (char*)value;
                else if (strcmp(attr, "UserShell") == 0) p_shell = (char*)value;
            }
            
            sprintf(line, "%s:%s:%s:%s:%s:%s:%s\n", 
                p_name, p_pass?p_pass:"*", p_uid?p_uid:"0", p_gid?p_gid:"0", 
                p_gecos?p_gecos:"", p_home?p_home:"", p_shell?p_shell:"");
        }
        strcat(new_content, line);
    }
    fclose(f);
    
    if (!found) {
        char p_name[256]; strcpy(p_name, name);
        char p_uid[256] = "0";
        char p_gid[256] = "0";
        char p_gecos[256] = "Nix User";
        char p_home[256] = "/var/empty";
        char p_shell[256] = "/usr/bin/false";
        
        if (attr && value) {
            if (strcmp(attr, "UniqueID") == 0) strcpy(p_uid, value);
            else if (strcmp(attr, "PrimaryGroupID") == 0) strcpy(p_gid, value);
            else if (strcmp(attr, "RealName") == 0) strcpy(p_gecos, value);
            else if (strcmp(attr, "NFSHomeDirectory") == 0) strcpy(p_home, value);
            else if (strcmp(attr, "UserShell") == 0) strcpy(p_shell, value);
        }
        
        sprintf(line, "%s:*:%s:%s:%s:%s:%s\n", p_name, p_uid, p_gid, p_gecos, p_home, p_shell);
        strcat(new_content, line);
    }
    
    f = fopen(PASSWD_FILE, "w");
    if (!f) { perror("fopen"); exit(1); }
    fputs(new_content, f);
    fclose(f);
}

void update_group(const char *name, const char *attr, const char *value) {
    FILE *f = fopen(GROUP_FILE, "r");
    if (!f) { perror("fopen"); exit(1); }
    char new_content[65536] = {0};
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        char copy[1024];
        strcpy(copy, line);
        copy[strcspn(copy, "\n")] = 0;
        char *p_name = strtok(copy, ":");
        if (p_name && strcmp(p_name, name) == 0) {
            char *p_pass = strtok(NULL, ":");
            char *p_gid = strtok(NULL, ":");
            char *p_mem = strtok(NULL, ":");
            
            if (attr && value && strcmp(attr, "PrimaryGroupID") == 0) p_gid = (char*)value;
            
            sprintf(line, "%s:%s:%s:%s\n", 
                p_name, p_pass?p_pass:"*", p_gid?p_gid:"0", p_mem?p_mem:"");
        }
        strcat(new_content, line);
    }
    fclose(f);
    
    f = fopen(GROUP_FILE, "w");
    if (!f) { perror("fopen"); exit(1); }
    fputs(new_content, f);
    fclose(f);
}

int main(int argc, char **argv) {
    int read_flag = 0, create_flag = 0, append_flag = 0;
    static struct option long_options[] = {
        {"read",   no_argument, 0, 'r'},
        {"create", no_argument, 0, 'c'},
        {"append", no_argument, 0, 'a'},
        {0, 0, 0, 0}
    };

    int opt, option_index = 0;

    // We use getopt_long_only to parse -read, -create, -append anywhere before positional args
    while ((opt = getopt_long_only(argc, argv, "rca", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'r': read_flag = 1; break;
            case 'c': create_flag = 1; break;
            case 'a': append_flag = 1; break;
            case '?': break; // ignore unrecognized
            default: break;
        }
    }

    // In case they didn't use a dash (e.g. `dscl . create ...`), check positional arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "read") == 0) read_flag = 1;
        if (strcmp(argv[i], "create") == 0) create_flag = 1;
        if (strcmp(argv[i], "append") == 0) append_flag = 1;
    }

    // Extract path, attr, value from positional arguments
    const char *path = NULL;
    const char *attr = NULL;
    const char *value = NULL;
    
    for (int i = optind; i < argc; i++) {
        if (strcmp(argv[i], ".") == 0 || strcmp(argv[i], "read") == 0 || 
            strcmp(argv[i], "create") == 0 || strcmp(argv[i], "append") == 0) {
            continue;
        }
        if (!path) path = argv[i];
        else if (!attr) attr = argv[i];
        else if (!value) value = argv[i];
    }

    if (!path) return 0; // Not enough arguments
    
    if (read_flag) {
        if (strncmp(path, "/Groups/", 8) == 0) {
            read_group(path + 8, attr);
        } else if (strncmp(path, "/Users/", 7) == 0) {
            read_user(path + 7, attr);
        }
    } else if (create_flag) {
        if (strncmp(path, "/Users/", 7) == 0) {
            update_user(path + 7, attr, value);
        } else if (strncmp(path, "/Groups/", 8) == 0) {
            update_group(path + 8, attr, value);
        }
    } else if (append_flag) {
        // Nix handles group append via dseditgroup, but we can stub it out if needed
    }
    return 0;
}
