// myls.c ... my very own "ls" implementation

#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __linux__
# include <bsd/string.h>
#endif
#include <sysexits.h>
#include <unistd.h>

#define MAXDIRNAME 256
#define MAXFNAME 256
#define MAXNAME 24

char *rwxmode (mode_t, char *);
char *username (uid_t, char *);
char *groupname (gid_t, char *);

int main (int argc, char *argv[])
{
	// string buffers for various names
	// char uname[MAXNAME+1]; // UNCOMMENT this line
	// char gname[MAXNAME+1]; // UNCOMMENT this line
	// char mode[MAXNAME+1]; // UNCOMMENT this line

	// collect the directory name, with "." as default
	char dirname[MAXDIRNAME] = ".";
	if (argc >= 2)
		strlcpy (dirname, argv[1], MAXDIRNAME);

	// check that the name really is a directory
	struct stat info;
	if (stat (dirname, &info) < 0)
		err (EX_OSERR, "%s", dirname);

	if (! S_ISDIR (info.st_mode)) {
		errno = ENOTDIR;
		err (EX_DATAERR, "%s", dirname);
	}

    // open the directory to start reading
    DIR *df; // UNCOMMENT this line
    // ... TODO ...
    df = opendir(dirname);
    chdir(dirname);
    // read directory entries
    struct dirent *entry; // UNCOMMENT this line
    // ... TODO ...
    for(entry = readdir(df);entry != NULL;entry = readdir(df)) {
        strcpy(dirname, entry->d_name);
        if(dirname[0] == '.')
            continue;
        printType(entry->d_type);
        lstat (dirname, &info);
        printf (
                "%s  %-8.8s %-8.8s %8lld  %s\n",
                rwxmode (info.st_mode, mode),
                username (info.st_uid, uname),
                groupname(info.st_gid, gname),
                (long long)(info.st_size),
                dirname
                );
    }
    
    // finish up
    closedir(df); // UNCOMMENT this line

	return EXIT_SUCCESS;
}

// convert octal mode to -rwxrwxrwx string
char *rwxmode (mode_t mode, char *str)
{
    // ... TODO ...
    char rwx[3] = "rwx";
    int mask = 1 << 8;
    int j = 0;
    for(int i = 0; mask >= 1; i++, mask = mask >> 1) {
        if((mask&mode) == 0)
            str[j] = '-';
        else
            str[j] = rwx[j%3];
        j++;
    }
    return str;
}

// convert user id to user name
char *username (uid_t uid, char *name)
{
	struct passwd *uinfo = getpwuid (uid);
	if (uinfo != NULL)
		snprintf (name, MAXNAME, "%s", uinfo->pw_name);
	else
		snprintf (name, MAXNAME, "%d?", (int) uid);
	return name;
}

// convert group id to group name
char *groupname (gid_t gid, char *name)
{
	struct group *ginfo = getgrgid (gid);
	if (ginfo != NULL)
		snprintf (name, MAXNAME, "%s", ginfo->gr_name);
	else
		snprintf (name, MAXNAME, "%d?", (int) gid);
	return name;
}
