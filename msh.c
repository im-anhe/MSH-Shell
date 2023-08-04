// The MIT License (MIT)
// 
// Copyright (c) 2016 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports four arguments


//places the command into history arr, shifting as needed
void update_history(char arr[15][MAX_COMMAND_SIZE], char* command, int amt_cmds)
{
  if(amt_cmds<=15)
  {
    strcpy(arr[amt_cmds-1], command);
  }
  else
  {
    for(int i = 0; i<14; i++)
    {
      strcpy(arr[i], arr[i+1]);
    }
    strcpy(arr[14], command);
  }
}

//places the command into the pid arr, shifting as needed
void update_pid(int arr[], int pid, int pid_count)
{
  if(pid_count<=14)
  {
    arr[pid_count] = pid;
  }
  else
  {
    for(int i = 0; i<14; i++)
    {
      arr[i] = arr[i+1];
    }
    arr[14] = pid;
  }
}


int main()
{

  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );

  //Holds the history up to 15 commands
  char history_arr[15][MAX_COMMAND_SIZE] = {};
  int amt_cmds = 0;

  //Holds the PIDs in an array
  int pids_arr[15];
  int pid_count = 0;

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );


    //Manipulating command_string to only have the number after !
    //Then the number in correspondance to the history_arr is executed
    if(command_string[0] == '!')
    {  
      char * num_token = strtok(command_string, "\n");
      strcpy(command_string, num_token);
      num_token = strtok(command_string, "!");
      //printf("%s", num_token);
      int num = atoi(num_token);

      if(num <= (amt_cmds-1))
      {
        command_string = history_arr[num];
      }
      else
      {
        strcpy(command_string,"\n");
        printf("Command not in history.\n");
      }

      
    }



    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    for( int i = 0; i < MAX_NUM_ARGUMENTS; i++ )
    {
      token[i] = NULL;
    }

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr = NULL;                                         
                                                           
    char *working_string  = strdup( command_string );                

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *head_ptr = working_string;

    // Tokenize the input strings with whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_string, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // Now print the tokenized input as a debug check
    // \TODO Remove this for loop and replace with your shell functionality
    strtok(command_string, "\n");
    
    if(token[0] == NULL)
    {
      //taking care of blank inputs
    }
    else if(strcmp(token[0], "quit") == 0 || strcmp(token[0], "exit") == 0)
    {
      //Exit if exit or quit is entered
      exit(0);
    }
    else if(strcmp(token[0], "cd") == 0)
    {
      //change directory
      amt_cmds++;
      update_pid(pids_arr, -1, pid_count);
      update_history(history_arr, command_string, amt_cmds);
      chdir(token[1]);
    }
    else if(strcmp(token[0], "history") == 0)
    {
      amt_cmds++;
      update_pid(pids_arr, -1, pid_count);
      update_history(history_arr, command_string, amt_cmds);
      if(amt_cmds>16)
      {
        amt_cmds--;
      }

      //making the range of the print statement fit 
      //implements the -p parameter to print out the history
      //with PIDs or no pids
      if(amt_cmds<15)
      {
        for(int i = 0; i<amt_cmds; i++)
        {
          if(token[1]!=NULL && strcmp(token[1], "-p") == 0)
          {
            printf("%d: %s [%d]\n", i, history_arr[i], pids_arr[i]);
          }
          else
          {
            printf("%d: %s\n", i, history_arr[i]);
          }
        }
      }
      else
      {
        //set range of 15 commands
        for(int i = 0; i<15; i++)
        {
          if (token[1]!=NULL && strcmp(token[1], "-p") == 0)
          {
            printf("%d: %s [%d]\n", i, history_arr[i], pids_arr[i]);
          }
          else
          {
            printf("%d: %s\n", i, history_arr[i]);
          }
          
        }
      }
    }
    else
    {
      //takes care of all commands that are needed to fork
      amt_cmds++;
      update_history(history_arr, command_string, amt_cmds);
      pid_t pid = fork( );
      update_pid(pids_arr, pid, pid_count);
      if( pid == 0 )
      {
        int ret = execvp( token[0], &token[0] );  
        if( ret == -1 )
        {
          //takes care of any random commands and returns to parent
          printf("%s: Command not found.\n", token[0]);
          return 0;
        }
      }
      else 
      {
        int status;
        wait( & status );
      }
    }


    // Cleanup allocated memory
    for( int i = 0; i < MAX_NUM_ARGUMENTS; i++ )
    {
      if( token[i] != NULL )
      {
        free( token[i] );
      }
    }

    free( head_ptr );
    pid_count++;
  }

  free( command_string );
  

  return 0;
  // e2520ca2-76f3-90d6-0242ac120003
}
