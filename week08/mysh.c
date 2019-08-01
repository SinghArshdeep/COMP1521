// mysh.c ... a minimal shell
// Started by John Shepherd, October 2017
// Completed by <<YOU>>, July 2019

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <assert.h>
#include <ctype.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_PATH "/bin:/usr/bin"
#define DBUG

static void execute (char **, char **, char **);
static bool isExecutable (char *);
static char **tokenise (const char *, const char *);
static void freeTokens (char **);
static char *trim (char *);
static size_t strrspn (const char *, const char *);


// Environment variables are pointed to by `environ', an array of
// strings terminated by a NULL value -- something like:
//     { "VAR1=value", "VAR2=value", NULL }
extern char **environ;

int main (int argc, char *argv[])
{
	// grab the `PATH' environment variable;
	// if it isn't set, use the default path defined above
	char *pathp;
	if ((pathp = getenv ("PATH")) == NULL)
		pathp = DEFAULT_PATH;
	char **path = tokenise (pathp, ":");

#ifdef DBUG
	for (int i = 0; path[i] != NULL; i++)
		printf ("dir[%d] = %s\n", i, path[i]);
#endif

	// main loop: print prompt, read line, execute command
	char line_[BUFSIZ];
	printf ("mysh$ ");
	while (fgets (line_, BUFSIZ, stdin) != NULL) {
		char *line = trim (line_); // remove leading/trailing space
		if (strcmp (line, "exit") == 0) break;
		if (strcmp (line, "") == 0) { printf ("mysh$ "); continue; }

		pid_t pid;   // pid of child process
		int stat;	 // return status of child

		/// TODO: implement the `tokenise, fork, execute, cleanup' code
        char **cl = tokenise(line, " ");
        // Parent process starts
        
        pid = fork();
        stat = pid;
        if (pid != 0) {
            wait(&stat);
        } else {
            execute(cl, path, environ);
        }
        freeTokens(cl);

		printf ("mysh$ ");
	}
	printf ("\n");

	freeTokens (path);

	return EXIT_SUCCESS;
}

// execute: run a program, given command-line args, path and envp
static void execute (char **args, char **path, char **envp)
{
    /// TODO: implement the `find-the-executable and execve(3) it' code
    char *command = NULL;
    char *line = args[0];
    
    
    // line = "my name is arsh"
    // arr =tokenise(line) | arr[0] = my
    // free(arr)
    
    //char **arr = tokenise (line," ");
    
    
    if (line[0] == '/' || line[0] == '.') {
        if (isExecutable(args[0])) {
            command = args[0];
        }
    } else if (strcmp(args[0],"cd") == 0) {
        if (args[1] == NULL) {
            // printf("%s\n" arr[1]);
            char *pathp = getenv("HOME");
            chdir(pathp);
        } else {
            chdir(args[1]);
        }
        
        return;
    } else {
        for (int i = 0; path[i] != NULL; i++) {
            char *command1 = malloc(strlen(args[0]) + strlen(path[i]) + 1);
            strcpy(command1, path[i]);
            strcat(command1, "/");
            strcat(command1, args[0]);
            if (isExecutable(command1)) {
                command = command1;
                break;
            }
        }
    }
    
    if (command == NULL) {
        printf("command not found\n");
        exit(0);
    }
    else {
        printf("%s\n", command);
        int rec = execve(command, args, envp);
        // execution failed
        if(rec == -1) perror("Exec failed");
    }
    
}

/// isExecutable: check whether this process can execute a file
static bool isExecutable (char *cmd)
{
	struct stat s;
	// must be accessible
	if (stat (cmd, &s) < 0) return false;
	// must be a regular file
	if (! S_ISREG (s.st_mode)) return false;
	// if it's owner executable by us, ok
	if (s.st_uid == getuid () && s.st_mode & S_IXUSR) return true;
	// if it's group executable by us, ok
	if (s.st_gid == getgid () && s.st_mode & S_IXGRP) return true;
	// if it's other executable by us, ok
	if (s.st_mode & S_IXOTH) return true;
	// otherwise, no, we can't execute it.
	return false;
}

/// tokenise: split a string around a set of separators
/// create an array of separate strings
/// final array element contains NULL
static char **tokenise (const char *str_, const char *sep)
{
	char **results  = NULL;
	size_t nresults = 0;

	// strsep(3) modifies the input string and the pointer to it,
	// so make a copy and remember where we started.
	char *str = strdup (str_);
	char *tmp = str;

	char *tok;
	while ((tok = strsep (&str, sep)) != NULL) {
		// "push" this token onto the list of resulting strings
		results = realloc (results, ++nresults * sizeof (char *));
		results[nresults - 1] = strdup (tok);
	}

	results = realloc (results, ++nresults * sizeof (char *));
	results[nresults - 1] = NULL;

	free (tmp);
	return results;
}

/// freeTokens: free memory associated with array of tokens
static void freeTokens (char **toks)
{
	for (int i = 0; toks[i] != NULL; i++)
		free (toks[i]);
	free (toks);
}

/// trim: remove leading and trailing whitespace from a string
static char *trim (char *str)
{
	char *space = " \r\n\t";
	str[strrspn (str, space) + 1] = '\0'; // skip trailing whitespace
	str = &str[strspn (str, space)];      // skip leading whitespace
	return str;
}

/// strrspn: find a suffix substring made up of any of `set'.
/// like `strspn(3)', except from the end of the string.
static size_t strrspn (const char *str, const char *set)
{
	size_t len;
	for (len = strlen (str); len != 0; len--)
		if (strchr (set, str[len]) == NULL)
			break;
	return len;
}
