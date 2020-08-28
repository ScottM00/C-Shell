/*Benjamin Shek, Callum McIlwraith, Scott Maclachlan, Muhtadi Ali, Rohit Acharya
Group 25*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/wait.h>
#define NOHIST 20
#define HFILENAME ".hist_list.txt"
#define AFILENAME ".aliases"

//define global variables and structs
int histpt=0;
typedef struct {
    char command[512];
} his;

his history[NOHIST];

void initialise()
{
    // print out a few lines.
    printf("Simple Shell in C version 0.1 \n");
    printf("Commands exceeding 512 characters will only have 512 characters run \n");
    printf("Enter commands \n \n");
    
}

char* getinput()
{
    //Assign 512 characters for input. Print ">" prompt and get user input.
    char* userin;
    userin = malloc(512* sizeof(char));
    printf(">");
    if (fgets (userin, 512, stdin)!= 0){
        //clear buffer so no unexpected outputs.
        size_t length = strlen(userin);
        if (length >0 && userin[length-1] != '\n'){
            int c;
            while(( c= getchar()) != '\n' && c != EOF);
        }
    }
    else{
        //handle EOF or error
    }
    return userin;
}

char** parsein(char* input){
    /* tokenise input and insert into an array. Function will split tokens based on the delimiter characters. Remaining spaces in token array will be assigned NULL.
     The function will return the tokenised input array.*/
    char** tokens;
    char *token;
    const char delim[10] = " |><&;\t\n";
    int pos = 0;
    // allocate memory for token array
    tokens = malloc(sizeof(char*) * 50);
    for (int i=0; i < 50; i++){
        tokens[i] = (char*) malloc(512*sizeof(char));
    }
    token = strtok(input, delim);
    tokens[pos] = token;
    pos++;
    token = strtok(NULL, delim);
    while(token != NULL){
        //remove delimeters from tokens and set each element in tokens array to the token
        tokens[pos] = token;
        pos++;
        token = strtok(NULL, delim);
    }
    for (int i= pos; i < 50; i++){
        tokens[i] = NULL;
    }
    return tokens;
    
}

int externalcommands(char** args){
    /*runs external commands with arguments. Fork() will run with file as a child process of c_pid of 1. Parent will have
     c_pid of 1 and an error in the fork will have a c_pid of -1.*/
    if (args[0] == NULL){
        printf("No arguments entered.\n \n");
        return 0;
    }
    pid_t c_pid, pid;
    int status;
    c_pid = fork();
    if(c_pid == -1){
        printf ("fork() failed\n");
        _exit (1);
    }
    if (c_pid ==0){
        //printf("executing command \n");
        execvp(args[0], args);
        // an error in the execvp process will print out the message below. Otherwise, break and return.
        perror(args[0]);
        _exit(1);
    }
    else if(c_pid > 0){
        if ((pid = wait (&status)) < 0){
            perror("wait failed\n");
            _exit(1);
        }
    }
    return 0;
}
int getpath(char* fail) { // returns the current path
    if (fail != NULL){
        printf("This command does not require an argument, please re-enter with no arguments \n");
    }
    else{
        printf("The current environment is %s  \n", getenv("PATH"));
    }
    return (0);
}

//sets current path to an other. If path doesnt exist print fail, if path exists then print updated path
int setpath(char* newP, char* fail){
    if (newP == NULL){ // if no path has been entered return error
        printf("Please enter an argument after the command \n");
    }
    else if (fail != NULL){
        printf("Please only enter 1 argument into this command at a time \n");
    }
    else if(setenv("PATH", newP, 1)!= 0){ // sets path if it exists otherwise prints fail
        printf("The path has failed to be updated");
    }
    else {
        printf("Path has been updated \n");
    }
    return 0;
}

// If only 'cd' is typed then return the directory to original home
int returnHome(char* ogHome){
    chdir(ogHome); // changes back to home
    printf("You have been returned to the home directory of %s \n", getenv("HOME"));
    return 0;
}


int cd(char* newD, char* fail){
    char cwd[1000]; // arbitrary value
    
    if (fail != NULL){ // if there is more than one argument exit cd
        perror("Error: Cannot enter more than one argument ");
        return 0;
    }
    if (chdir(newD) ==-1){ //if directory doesnt exist throws error
        perror(newD);
        return 0;
    }
    printf("The directory has been changed to %s \n", getcwd(cwd, 1000));
    return 0;
}

int addHist(his *hist, int pt, char *com[50]) {
    int temp;
    int j;
    
    /*calculates next history slot in circular array*/
    temp = pt % NOHIST;
   
    /*copys first comand token to history slot*/
    strcpy(hist[temp].command, com[0]);

    j = 1;
    /*apends rest of tokens to the slot separated by spaces*/
    while (!(com[j] == NULL)) {
        strcat(hist[temp].command, " ");
        strcat(hist[temp].command, com[j]);
        j++;
    }
    return temp;
}

/*prints history*/
int printHist(his *hist, int pt) {
    int temp;
    int i = 0;
    temp = pt;
    printf("History as follows :\n\n");
    for (; i < NOHIST; i++, temp--) {
        /*accounts for circular array*/
        if (temp < 0) {
            temp = NOHIST + temp;
        }
        /*if history slot not empty print comand*/
        if (strlen(hist[i].command) > 0) {
            printf("[%d] %s\n", i+1, hist[i].command);
        }
        
    }
    return 0;
}

/* 
checkalias: Checks if inputed command is an aliased command, if so replaces the input with the alias command.
*/
void checkalias (char input[512], char *alias[10][2]){

  int i;
	for (i = 0; i < 10; i++){
      if (alias[i][0] != NULL){ // Loop through all non-empty elements

	  if (strcmp (strtok (input, "\n"), alias[i][0]) == 0){ //Check if input matches alias name

	      strcpy (input, alias[i][1]); // If so replace input with alias commande
		
	    }
	}
    }
  
}
void doublealias (char input[512], char *alias[10][2]){

	char temp[512];
	strcpy(temp, input);
	strtok(temp, " ");
  int i;
	for (i = 0; i < 10; i++){
      if (alias[i][0] != NULL){ // Loop through all non-empty elements
		if (strcmp(temp, alias[i][0]) == 0){ //Check if input matches alias name
	      strcpy (input, alias[i][1]); // If so replace input with alias commande
	    }
		}
    }
}


void alias(char *tokens[512], char *alias[10][2]){
	
	int i = 0;
	int nullCounter = 0;
	//FOR THE ALIAS NAME
    char* name;
    //FOR THE ALAIS COMMAND
    char command[512] = "";
    int tokensCounter;
    int index = 0;
    int exists = 1;
    
	//CHECK HOW MANY EMPTY SPACES LEFT IN ALIAS LIST
	for (i = 0; i < 10; i++){
		if(alias[i][0] == NULL){
			nullCounter = nullCounter + 1;
			
		}
	}
	//IF ONLY ALIAS IS ENTERED
	if (tokens[1] == NULL){
	    //IF LIST IS FULLY EMPTY
		if(nullCounter == 10){
			//PRINT NO ALIASES IN LIST
			printf("There no aliases in list.\n");
		}
		else{
			for (i = 0; (i < (10-nullCounter)); i++){
			    //PRINT ALIASES IN LIST
				printf("%s\t%s \n", alias[i][0], alias[i][1]);
			}
		}
		return;
	}
	else{
        //IF NO COMMAND TO ALIAS IS GIVEN
		if(tokens[2] == NULL){
		    //PRINT ERROR MESSAGE
			printf("Error: Incorrect parameters provided to alias command. Please specify <command2>\n");
			return;
		}
		
		//FULLY INITIALISE VARIABLES
		name = tokens[1];
		tokensCounter = 0;

		//COUNT TOKENS IN ARRAY (HOW MANY SEPERATE WORDS HAVE BEEN ENTERED)
		while (tokens[tokensCounter] != NULL){
			tokensCounter ++;
		}

		//CONCATENATE ALL TOKENS - APART FROM ALIAS AND <COMMAND1> TO STORE THE COMMAND
		for (index = 2; index < tokensCounter; index++){
			strcat(command, tokens[index]);
			strcat(command, " ");
		}
		
		//ADD NULL ELEMENT TO END	
		strcat(command, "\0");
		
		//CHECKS IF ALIAS NAME ALREADY EXISTS
		for (i = 0; (i < (10-nullCounter)); i++){
				if(strcmp(name, alias[i][0]) == 0){
					printf("This alias already exists. Overwriting.\n");
					//IF SO REPLACE WITH NEW ALIAS COMMAND
					alias[i][1] =strdup(command);
					exists = 0;
				}
		}
		//IF LIST IS FULL, AND ISNT A DUPLICATE NAME
		if((nullCounter == 0) && (exists == 1)){
			printf("No more aliases can be added, list is full \n");
		}
		else{
           		//ADD ALIAS TO ARRAY IF DOES NOT EXIST ALREADY
			if ((exists == 1)&&(nullCounter != 0)){
				alias[10-nullCounter][0] =strdup(name);
				alias[10-nullCounter][1] =strdup(command);
			}
		}
	}
    return;
}

void unalias(char *tokens[512], char *alias[10][2]){
    
    int i;
	int index;
    char* name;
    int found = 1;
    
    //NO ALIAS NAME GIVEN
	if (tokens[1] == NULL){
		printf("Error: No alias name given. \n");
		return;
	}
	
	//IF MORE THAN JUST NAME IS GIVEN (ERROR)
    if(tokens[2]!=NULL){
        printf("Error: Too many values given. Follow unalias <name>. \n");
		return;
    }
    
    //NAME IS ASSIGNED TO 2ND VALUE
	name = tokens[1];

    
    //SEARCH FOR GIVEN ALIAS NAME
	for (i = 0; i < 10; i++){
		if (alias[i][0] != NULL){
		    //IF ALIAS NAME FOUND
			if(strcmp(name, alias[i][0]) == 0){
			    //REOMVE THE ALIAS BY MAKING NULL
				alias[i][0] = NULL;
				alias[i][1] = NULL;
				index = i;
				found = 0;
			}
		}

	}

    //IF ALIAS NAME CANNOT BE FOUND PRINT ERROR TO USER
	if (found == 1){
		printf("Error: The alias does not exist. \n");
		return;
	}
	else{
		while(index<9){
		    //SHIFTING ALIAS ARRAY DOWN TO NOT LEAVE GAP
			alias[index][0] = alias[index+1][0];		
			alias[index][1] = alias[index+1][1];
			index++;
		}
		alias[9][0] = NULL;					
		alias[9][1] = NULL;
	}
    return;
}


void runcommand(char** tokens, char* ogHome, char* input, char *aliasArray[10][2]){
    //gets the command from the first token, pass in arguments from tokens and run commands with arguments.
    
    int num;
    int historyInvocation = 0;
    //invokes last command entered

		if(tokens[0]!=NULL && 0 == strcmp(tokens[0],"!!")&& tokens[1]==NULL){
			historyInvocation = 1;
			strcpy(input, history[histpt-1].command);
			tokens = parsein(input);
		}
    /*envokes previous history comands*/
	 else if(0 == strncmp(input,"!",1)){
        historyInvocation = 1;
        num = atoi(input+1)-1;
        if( num < 0){
            num = NOHIST + num +1;
        }
        if((0 <= num) && (num < NOHIST)){
            if(!(0==strlen(history[num].command))){
                strcpy(input, history[num].command);
                tokens = parsein(input);
            }
            else{
                
                printf("Not a valid history item\n");
                tokens[0] = NULL;
            }
        }
    }
	//checks to ensure something is entered then runs any commands
    if (tokens[0] != NULL){	
        //strcmp compares the first part of the input to any predefined commands
        //prints out current path
        if (strcmp(tokens[0], "getpath") ==0){
            int rn = getpath(tokens[1]);
        }
        //sets new path
        else if (strcmp(tokens[0], "setpath") ==0){
            int rn = setpath(tokens[1], tokens[2]);
        }
        //changes directory back to home if no other arguments are entered otherwise
        //try to change directory
        else if (strcmp(tokens[0], "cd") == 0){
            if (tokens[1]==NULL){
                int rn = returnHome(ogHome);
            }
            else {
                int rn = cd(tokens[1], tokens[2]);
            }
        }
        else if(strcmp(tokens[0], "alias") == 0){
            alias(tokens, aliasArray);
        }
        else if(strcmp(tokens[0], "unalias") == 0){
            unalias(tokens, aliasArray);
        }
        
        // prints command history
        else if(0 == strcmp(tokens[0], "history") && tokens[1] ==NULL)
            
            if(historyInvocation==0){
                histpt = addHist(history,histpt,tokens);
                printHist(history,histpt);
                histpt++;
            }
        
            else {
                printHist(history,histpt);
            }
        
            else{
                // if not a predefined command, run it as an external command
                externalcommands(tokens);
            }
        //adds the command to history if ! or !! has not been used
        if(!(0 == strcmp(tokens[0], "history")) && historyInvocation==0){
            histpt = addHist(history,histpt,tokens);
            histpt++;
        }
    }
    
}




int saveAlias(char *aliasArray[10][2]){

int num= 0;
   FILE *fptr;
   fptr = fopen(AFILENAME,"w");
   if(fptr == NULL)
   {
      printf("Error!");   
      exit(1);             
   }
	while((aliasArray[num][0] != NULL) && (aliasArray[num][1] != NULL)){
		//saves aliases in the form alias;alias[0];alias[1]
		//word alias added to the front to allow direct passing into the alias function.
		fprintf(fptr,"alias %s %s\n", aliasArray[num][0],aliasArray[num][1]);
		num++;
	}

	if(num == 10){
	printf("Saving contents of alias...\n");
	}
	
    fclose(fptr);
    return 0;
}

int loadAlias(char *aliasArray[10][2]){
	FILE *fptr;
	char* buff;
	char** tokens;
    fptr = fopen(AFILENAME,"r");
	buff = malloc(512* sizeof(char));
	
	if (fptr == NULL){
		printf("Couldn't find file. \n");
		return 0;
	}
	while (fgets(buff, 255, (FILE*)fptr) != NULL){
		//tokenise the line and adds it to the alias.
		tokens = parsein(buff);
		alias(tokens, aliasArray);
	}
    fclose(fptr);
	return 0;
}

int saveHist(his *hist, int pt) {
    int i;
    int temp;
    FILE *fp;
    temp = pt;
    fp = fopen(HFILENAME, "w");
    
    if (fp == NULL) {
        
        printf("Couldn't find file.\n");
    } else {
        i = 0;
        for (; i < NOHIST; i++, temp--) {
            /*accounts for circular array*/
            if (temp < 0) {
                temp = NOHIST + temp;
            }
            /*if there is a comand in slot writes to file*/
            if(strlen(hist[temp].command)>0){
                /*if first history item does not take new line*/
                if(temp ==pt){
			
                    fprintf(fp,"%s",hist[temp].command);
                }else{

                    fprintf(fp,"\n%s",hist[temp].command);
                }
                
            }
            
        }
    }
    fclose(fp);    
    return 0;
}

int loadHist(his *hist) {
    int i;
    char temp[512];
    FILE *fp;
    char *com;
    
    fp = fopen(HFILENAME, "r");
    if (fp == NULL) {
        
        printf("Couldn't find file.\n");
    } else {
	 
        i = 0;
        /*checks for empty file*/
        
	
        while (!feof(fp) && (i<20)) {
            fgets(temp, 512, fp);
            /*if line not empty*/

            if(strlen(temp) > 1){
                /*removes new line character*/
                com = strtok(temp, "\n");
                /*copys comands into history working backwards from slot 20*/

                strcpy(hist[20-i].command,com);
            }
            i++;
        }
        
        
    }
    fclose(fp);
    return 0;
}

int main(){
    //'Skeleton' main allowing for easier understanding of what is happening.
    initialise();
    // define needed variables which may be passed in to functions.
    char *historyy[NOHIST] = {0};
    char* input = "";
    char** tokens;
    char cwd[1000]; // arbitrary value
    const char* ogPath = getenv("PATH"); // saves original Path to constant
    char* ogHome = getenv("HOME"); //Saves original Home to constant
    chdir(ogHome); // sets current directory to Home
    char *alias[10][2] = {{0}};
	loadAlias(alias);
	loadHist(history);
    while (1){
        /*Will contineously prompt and get user input. Typing "exit" or when <CTRL+D> is pressed and exit the loop.
         Input is then passed into the "parsein" function to be tokenised. Tokenised input array is then passed into
         the "runcommand function" which will run a command based on what has been entered. Input string is cleared
         and memory for tokens is freed before looping back. When loop is exited. Print exit shell prompt and
         shell is exited.*/
        input = getinput();
        // will exit when detects <CTRL+D>
        if (feof(stdin)){
            break;
        }
        else{
            if (strcmp(input,"exit\n") == 0){ // \n needed as fgets adds new line exit character to string
                break;
            }
            else{
				checkalias(input, alias);
            }
            
        }
        doublealias(input, alias);
        tokens = parsein(input);
        // tokens, ogHome and input passed into run commands function. ogHome allow for cd to work, input allow for history to work and tokens allow for
        // checking of command to run
        runcommand(tokens, ogHome, input, alias);
        //free memory space
        free(input);
        free(tokens);
    }
    saveAlias(alias);
	saveHist(history,histpt);
    setenv("PATH", ogPath, 1); //returns path to original path
    printf("The path has been returned to %s  \n", getenv("PATH"));
    chdir(ogHome); // returns home to its original home
    printf("The working directory has been restored too %s \n", getcwd(cwd, 1000));
    printf("Path and home has been returned to original\n");
    printf("Exiting shell.\n");
    
    return (0);
}
