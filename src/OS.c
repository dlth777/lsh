/***************************************************************************//**

  @file         main.c

  @author       Stephen Brennan

  @date         Thursday,  8 January 2015

  @brief        LSH (Libstephen SHell)

*******************************************************************************/

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>

/*
  Function Declarations for builtin shell commands:
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int myls(int argc, char *args[]);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "cd",
  "help",
  "exit",
	"ls"
};

int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit,
	&myls
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/

/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

/**
   @brief Builtin command: print help.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int lsh_help(char **args)
{
  int i;
  printf("Stephen Brennan's LSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int lsh_exit(char **args)
{
  return 0;
}	

int mymkdir(int argc,char *argv[])
{

	if(argc!=2) //If the input is not of the form mkdir pathname, then display error message.
	{
		printf("Invalid number of arguments\n");
		return ;
	}

	if(!mkdir(argv[1],0775)) //Create a directory with the name specified in the argument.
		printf("%s was created\n",argv[1]);
	else
		perror("mkdir");//Display the error occurred while trying to create the directory. 

	return 0;
}
int mypwd()
{
	char buffer[100];
	
	if(getcwd(buffer,sizeof(buffer)))//Get current working directory and store it in buffer.
	{
		printf("%s\n",buffer);
	}
	else
	{
		perror("getcwd");//Display the error occurred with getcwd.
	}
	
	return 0;
}

/*int myls(char *argv){
	DIR *pdir;
	struct dirent *pde;
	struct stat buf;
	
	int i=0;
	int count = 0;
	char *dir_name[255];
	
	memset(dir_name, '\0', sizeof(dir_name));
	memset(&pde, '\0', sizeof(pde));
	memset(&buf, '\0', sizeof(buf));
	
	if((pdir = opendir(argv)) < 0){
		perror("opendir");
		return 1;
	}
	
	chdir(argv);
	
	printf("\n\n");
	printf("### DIR -> %s ###\n", argv);
	while((pde = readdir(pdir)) != NULL){
		lstat(pde->d_name, &buf);
		if(S_ISDIR(buf.st_mode) == 1){
			if(!strcmp(pde->d_name,".") || !strcmp(pde->d_name, ".."))
				;
			else{
				dir_name[count] = pde->d_name;
				count = count + 1;
			}
		}
		printf("%s\t", pde->d_name);
	}
	
	for(i=0; i<count; i++){
		ls(dir_name[i]);
	}
	printf("\n");
	closedir(pdir);
	chdir("..");
	
	if(argv < 2){
		fprintf(stderr, "Usage : file_dir dirname.\n");
		return 1;
	}
	ls(argv[1]);
	return 0;
	
}*/


int myls(int argc, char *argv[])
{	
	DIR *directory; //Stores a pointer to the directory
	struct dirent *file;
	int i;
	struct stat sb;
	struct passwd *pwd;
	struct group *grp;
	if(!strcmp(argv[1], "-l"))
	{
		for(i=2; i<argc; i++)
		{
			if(!access(argv[i],F_OK)) //Check if the directory specified by argv[i] exists.
			{
				directory=opendir(argv[i]); //If it exists, then open the directory.

				if(directory!=NULL)
				{
					while((file=readdir(directory))!=NULL) //Read each entry in the directory.
					{
	
						if (stat(file->d_name, &sb) == -1) 
						{
							perror("stat");
						}
						//Display the type of file.
						switch (sb.st_mode & S_IFMT) 
						{
							case S_IFBLK:  printf("b");            
								       break;
							case S_IFCHR:  printf("c");        
								       break;
							case S_IFDIR:  printf("d");               
								       break;
							case S_IFIFO:  printf("p");               
								       break;
							case S_IFLNK:  printf("l");                 
								       break;
							case S_IFREG:  printf("-");            
								       break;
							case S_IFSOCK: printf("s");                  
								       break;
							default:       printf("u");                
								       break;
						}
						//Display the permissions for user, group and others.
						(sb.st_mode & S_IRUSR)? printf("r"):printf("-");
						(sb.st_mode & S_IWUSR)? printf("w"):printf("-");
						(sb.st_mode & S_IXUSR)? printf("x"):printf("-");
						(sb.st_mode & S_IRGRP)? printf("r"):printf("-");
						(sb.st_mode & S_IWGRP)? printf("w"):printf("-");
						(sb.st_mode & S_IXGRP)? printf("x"):printf("-");
						(sb.st_mode & S_IROTH)? printf("r"):printf("-");
						(sb.st_mode & S_IWOTH)? printf("w"):printf("-");
						(sb.st_mode & S_IXOTH)? printf("x"):printf("-");
	
						printf("%3ld ", (long) sb.st_nlink);//Display the number of links.
						grp = getgrgid(sb.st_gid);
						pwd = getpwuid(sb.st_uid);
						printf("%11s %11s ", pwd->pw_name,grp->gr_name);//Display the username and group name.
						printf("%4lld ",(long long) sb.st_size);//Display the size of the file.
						printf("%s ",file->d_name);//Display the file name.
						printf("%s",ctime(&sb.st_atime)); //Display the last access time of the file.
					}
				}
			}
		}	
	}
	else if(!strcmp(argv[1], "-li"))
	{
		for(i=2; i<argc; i++)
		{
			if(!access(argv[i],F_OK)) //Check if the directory specified by argv[i] exists.
			{
				directory=opendir(argv[i]); //If it exists, then open the directory.

				if(directory!=NULL)
				{
					while((file=readdir(directory))!=NULL) //Read each entry in the directory.
					{
	
						if (lstat(file->d_name, &sb) == -1) 
						{
							perror("stat");
						}
						
						 printf("%6ld ", (long) sb.st_ino);
						//Display the type of file.
						switch (sb.st_mode & S_IFMT) 
						{
							case S_IFBLK:  printf("b");            
								       break;
							case S_IFCHR:  printf("c");        
								       break;
							case S_IFDIR:  printf("d");               
								       break;
							case S_IFIFO:  printf("p");               
								       break;
							case S_IFLNK:  printf("l");                 
								       break;
							case S_IFREG:  printf("-");            
								       break;
							case S_IFSOCK: printf("s");                  
								       break;
							default:       printf("u");                
								       break;
						}
						//Display the permissions for user, group and others.
						(sb.st_mode & S_IRUSR)? printf("r"):printf("-");
						(sb.st_mode & S_IWUSR)? printf("w"):printf("-");
						(sb.st_mode & S_IXUSR)? printf("x"):printf("-");
						(sb.st_mode & S_IRGRP)? printf("r"):printf("-");
						(sb.st_mode & S_IWGRP)? printf("w"):printf("-");
						(sb.st_mode & S_IXGRP)? printf("x"):printf("-");
						(sb.st_mode & S_IROTH)? printf("r"):printf("-");
						(sb.st_mode & S_IWOTH)? printf("w"):printf("-");
						(sb.st_mode & S_IXOTH)? printf("x"):printf("-");
	
						printf("%3ld ", (long) sb.st_nlink);//Display the number of links.
						grp = getgrgid(sb.st_gid);
						pwd = getpwuid(sb.st_uid);
						printf("%11s %11s ", pwd->pw_name,grp->gr_name);//Display the username and group name.
						printf("%4lld ",(long long) sb.st_size);//Display the size of the file.
						printf("%s ",file->d_name);//Display the file name.
						printf("%s",ctime(&sb.st_atime)); //Display the last access time of the file.
					}
				}
			}
		}	
	}
	else
	{
		for(i=1;i<=argc-1;i++)
		{
			if(!access(argv[i],F_OK)) //Check if the directory specified by argv[i] exists.
			{
				directory=opendir(argv[i]); //If it exists, then open the directory.

				if(directory!=NULL)
				{
					while((file=readdir(directory))!=NULL) //Read each entry in the directory.
						printf("%-5s\t",file->d_name);
					printf("\n");
				}
				else
					perror("opendir"); //Display the error occurred while opening the directory.
			}
			else
					perror("access"); //Display an error if the directory does not exists.
		}

	}	
	return 0;
}
	
/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int lsh_launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */
char *lsh_read_line(void)
{
#ifdef LSH_USE_STD_GETLINE
  char *line = NULL;
  ssize_t bufsize = 0; // have getline allocate a buffer for us
  if (getline(&line, &bufsize, stdin) == -1) {
    if (feof(stdin)) {
      exit(EXIT_SUCCESS);  // We recieved an EOF
    } else  {
      perror("lsh: getline\n");
      exit(EXIT_FAILURE);
    }
  }
  return line;
#else
#define LSH_RL_BUFSIZE 1024
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    if (c == EOF) {
      exit(EXIT_SUCCESS);
    } else if (c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
#endif
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
		free(tokens_backup);
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

/**
   @brief Loop getting input and executing it.
 */
void lsh_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    free(line);
    free(args);
  } while (status);
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  lsh_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}

