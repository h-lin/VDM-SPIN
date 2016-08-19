#include "vdmj_parse.h"
unsigned long count_exec_num=0;

//***supportor function********************************************************
// return integer in input string (VDMJ's response)
int int_from_str(char *s)
{
  int i;
  long val = -1;
  char *pt, *endpt;

  //printf("int_from_str_1\n");
  pt = s;
  while(pt[0] != '\0') {
    if (pt[0] == '=' && pt[1] == ' ') {
      pt = pt + 2;
      if (isdigit(*pt)) {
        val = strtol(pt, &endpt, 10);
        return (int) val;
      }
    }
    pt++;
    //printf("int_from_str_2\n");
  }
  printf("ERROR: int not found");
  return -1;
}


// return bool(1 or 0) in input string (VDMJ's response)
bool bool_from_str(char *s)
{
  int i;
  long val = -1;
  char *pt, *endpt;

  //printf("bool_from_str_1\n");
  pt = s;
  while(pt[0] != '\0') {
	if (strncmp(pt,"= true",6) == 0) return true;
	if (strncmp(pt,"= false",7) == 0) return false;
    pt++;
    //printf("bool_from_str_2\n");
  }
  printf("ERROR: bool not found");
  return false;
}


// read the FIFO and retrieve value of int or bool according to option specified
// option = 0 : retrieve nothing
// option = 1 : retrieve an integer value
// option = 2 : retrieve a bool value (1 or 0)
int fifo_read(int option)
{
  int str_length=0, ret_int, count=0, flag=0, start_pos=0;
  bool ret_bool;

  // reset strbuffer at first
  memset(strbuffer, '\0', sizeof(strbuffer));

  while(nbytes != 0) {
    //printf("start to read...\n"); // for test
    // reset readbuffer before read the fifo
    memset(readbuffer, '\0', sizeof(readbuffer));
    //nbytes = 0;
    nbytes = read(fifo_fd[0], readbuffer, sizeof(readbuffer));
    strcat(strbuffer, readbuffer); // append readbuffer to strbuffer
    //printf("readbuffer: %s\n", readbuffer); // test

    // check nbytes to see if continue to further processing.
    if (nbytes > 0)
      str_length += nbytes;
    else
      sleep(1); // nbyte == -1, wait for more to be written

    //printf("<<<nbytes = %d, str_length = %d\n>>>", nbytes, str_length); // for test

    // processing strbuffer
    //if (readbuffer[nbytes-2] == '>') { // response of vdmj end, i.e. vdmj is waiting for a command
    if (str_length>=2) {
      //if (strbuffer[str_length-2] == '>' && strbuffer[str_length-3] == 'm' && strbuffer[str_length-4] == 'd' && strbuffer[str_length-5] == 'v') { // response of VDM-SL tools commandline (vdmde), i.e. vdmdeis waiting for a command
      if (strbuffer[str_length-2] == '>') { // response of vdmj end, i.e. vdmj is waiting for a command
        if (count_exec_num % 10000 == 0)
          printf("read: %s\n", strbuffer); // for test: should print till "> "
        //check if VDMJ returns a runtime error (maybe violation of pre/post-condition or invariant)
        //if (strncmp(strbuffer,"Stopped in ", 11) == 0) {
        if (strncmp(strbuffer,"Error ", 6) == 0) {
	      printf("ERROR returned by VDMJ interpreter...\n");
          printf("read: %s\n", strbuffer); // for test: should print out "Error ..."
          now.VDM_Error = 1; // raise VDM_Error flag
          return -1;
        }

        // adjust strbuffer: erase texts not related to VDM literals

        // for VDMJ - start
        if (strncmp(strbuffer,"= ", 2) == 0) {
          strbuffer[0] = ' ';
          strbuffer[1] = ' ';
          for (i=2; i<str_length; i++) {
            if (flag==1) strbuffer[i] = '\0';
		    else if (strbuffer[i]=='\n') {strbuffer[i] = '\0'; flag = 1;}
		  }
		  if (flag==0) printf("Error: '\\n' not found!\n");
		  flag = 0;
		  //printf("after filtered...\n");
          //printf("%s\n", strbuffer); // for test: should print only first line
          //parse_flag = 1;
          //parse_vdmj();
        } else {
          //printf("\nsomething wrong here...\nread: %s\n", strbuffer); // for test: something wrong
        }
        // for VDMJ - end

/*
        // for VDM-SL tools commandline - start
        for (i=0; i<str_length; i++) {
          if (strbuffer[i]!='\n') strbuffer[i] = ' ';
          else { strbuffer[i] = ' '; start_pos = i+1; break; }
	    }
		if (start_pos==0) printf("Error: 1st '\\n' not found!\n");
        for (i=start_pos; i<str_length; i++) {
          if (flag==1) strbuffer[i] = '\0';
		  else if (strbuffer[i]=='\n') {strbuffer[i] = '\0'; flag = 1;}
        }
		if (flag==0) printf("Error: 2nd '\\n' not found!\n");
		flag = 0; start_pos = 0;
		printf("after filtered...\n");
        printf("%s\n", strbuffer); // for test: should print only first line
        parse_flag = 1;
        // for VDM-SL tools commandline - end
*/

        switch(option) {  // no longer in use
          case 1: // get an integer value from strbuffer
            ret_int = int_from_str(strbuffer);
            //printf("<<<getting integer: %d>>>\n", ret_int); // for test
            return ret_int;
            break;
          case 2: // get an boolean value from strbuffer
            ret_bool = bool_from_str(strbuffer);
            //if (ret_bool==true)
            //  printf("<<<getting boolean: true>>>\n"); // for test
            //else
            //  printf("<<<getting boolean: false>>>\n"); // for test
            return ret_bool;
            break;
          case 0: // do nothing
            //printf("<<<getting nothing...>>>\n"); // for test
            return 0;
            break;
          default: // error happens
            printf("ERROR: action for strbuffer not correctly specified!\n");
            return -1;
        }
      }
    }
    //printf("<<<count++: %d, nbytes = %d>>>\n", count, nbytes); // for test: read is not completed, go next round.
    if (count>=30) return -1; // prevent infinite while loop.
  }
  printf("<<<nbytes == 0>>>\n"); // fifo_ret is closed, job done.
  return 0;
}


// important: the input string s should ended with '\n' or the command will not be executed by VDMJ
int fifo_write(char *s)
{
  //fprintf(stderr, " %lu", count_exec_num);

  if (s[strlen(s)-1] != '\n') {
    printf("ERROR: input string should ended with \'\\n\'!\n");
    return 1;
  }

  memset(writebuffer, '\0', sizeof(writebuffer)); // reset writebuffer before use
  strcpy(writebuffer, s);
  //printf("<<<writing: %s, length = %d>>>\n", writebuffer, strlen(writebuffer)); // for test
  temp = write(fifo_fd[1], writebuffer, strlen(writebuffer));

  return 0;
}

// if Error has occured, restore VDMJ to normal mode to proceed checking
int restore_runtime_error()
{
  if (now.VDM_Error == 0) return 0;
  //printf("VDM_Error == 1...\n");

  //temp = fifo_write("quit\n");
  //temp = fifo_read(0);
  now.VDM_Error = 0; // reset VDM_Error to 0
  return 1;
}


int parse_vdmj()
{
  buffer = yy_scan_string(strbuffer);
  yyparse();
  yy_delete_buffer(buffer);
  //dump_xml(ast_root); // dump for test
  //free_AST(ast_root);
  //parse_flag = 0;

  return 1;
}


int report_number()
{
  temp = restore_runtime_error(); // clear runtime error situation of VDMJ

  count_exec_num++;  // test
  //fprintf(stderr, " r%lu", count_exec_num);
  if (count_exec_num % 1000 == 0)
    fprintf(stderr, " %lu", count_exec_num/1000);
  if (count_exec_num % 10000 == 0)
    printf("<<<report number of execution: %lu>>>\n", count_exec_num); // for test
  if (count_exec_num % 100000 == 0) {
    temp = write(fifo_fd[1], "quit\n", 5);
    temp = fifo_read(0);
    fprintf(stderr, " init...");
  }
  return 0;
}


//***initialization function***************************************************
int initialization()
{
  //printf("<<<start initialization>>>\n");  // for test

  // check if FIFOs are opened
  if (fifo_fd[0] != -1 || fifo_fd[1] != -1) return 0;

  // open two FIFOs: fifo_cmd, fifo_ret
  // note: the FIFOs have to be created in advance.
  //fifo_fd[0] = open("fifo_ret", O_RDONLY|O_NONBLOCK); // for receiving VDMJ's results
  //fifo_fd[0] = open("fifo_ret", O_RDONLY); // for receiving VDMJ's results
  fifo_fd[0] = open("fifo_ret", O_RDWR); // for receiving VDMJ's results
  fifo_fd[1] = open("fifo_cmd", O_WRONLY); // for sending commands to VDMJ
  
  temp = write(fifo_fd[0], "1234\n", 5); // write something to fifo_ret (don't know why, but it works)
  temp = fifo_read(0); // read the first response from VDMJ (successfully loaded module/class...)
  temp = write(fifo_fd[1], "init\n", 5); // initialize module(s) in case
  temp = fifo_read(0);
  //temp = fifo_read(0); // run fifo_read twice! (don't know why, but it works)
/*
  //for VDM-SL tools commandline
  //printf("in init...1\n"); // for test
  temp = write(fifo_fd[1], "init\n", 5);
  //printf("in init...2\n"); // for test
  temp = fifo_read(0);
  temp = write(fifo_fd[1], "push TEST\n", 10);
  //printf("in init...3\n"); // for test
  temp = fifo_read(0);
*/
  //synchronization of state variables
  //temp = count_rd(); // sync now.counter with initial state (count)

  //printf("<<<end initialization>>>\n"); // for test
  return 0;
}


//***state synchronization functions*******************************************
int read_state_SIMPLE()
{
  struct AST *ast, *ast_tmp;
  int i=0;

  temp = fifo_write("print SIMPLE`get_state()\n");
  temp = fifo_read(0); // read the response from VDMJ, the strimmed response will be stored in strbuffer
  //printf("state of AAH after filtered...\n");
  //printf("%s\n", strbuffer); // for test: should print only first line
  //printf("dump state of AAH...\n");
  temp = parse_vdmj();
  // assign value
  ast = ast_root->u.child[0]->u.child[0];
  //printf("type of ast : %s\n", AST_type_str[ast->ast_type]); // for test

  for (i=0; i<10; i++) now.state_simple.pool[i] = 0; // reset all to 0
  now.state_simple.length = 0;
  if (ast->ast_type != AST_set_empty) { // if not empty set, assign
    ast_tmp = ast->u.child[0];
    i = 0;
    while (ast_tmp != NULL) {
    now.state_simple.pool[i] = ast_tmp->u.child[0]->u.num;
    i++;
    if (ast_tmp->num_child > 1) // more element(s)
	  ast_tmp = ast_tmp->u.child[1];
    else
	  ast_tmp = NULL;
    }
  }
  now.state_simple.length = i;

  // for test
  //printf("<<< state_simple.length = %d >>>\n", now.state_simple.length);
  //printf("<<< state_simple.pool... ");
  //for (i=0; i<now.state_simple.length; i++) printf("%d, ", now.state_simple.pool[i]);
  //printf(" >>>\n");
  
  free_AST(ast_root);
  return 1;
}

int write_state_SIMPLE()
{
  int i;
  char str[BUF], str_tmp[5];

  memset(str,     '\0', sizeof(str)); // reset str at first
  memset(str_tmp, '\0', sizeof(str_tmp)); // reset str at first
  
  strcat(str,"print SIMPLE`set_state(mk_S({");
  for (i=0; i<now.state_simple.length; i++) {
    if (i==0) {
      snprintf(str_tmp, sizeof(str_tmp), "%d", now.state_simple.pool[i]);
	}
    else {
      snprintf(str_tmp, sizeof(str_tmp), ",%d", now.state_simple.pool[i]);
    }
    strcat(str,str_tmp);
  }
  strcat(str,"}))\n");

  if (count_exec_num % 10000 == 0)
    printf("write: %s",str); // test

  temp = fifo_write(str);
  temp = fifo_read(0);

  // show state for test
  //temp = fifo_write("print AAH`get_state();\n");
  //temp = fifo_read(0);
  //printf("write result: %s\n",strbuffer);

  return 1;
}

//***operations/functions******************************************************
// SIMPLE`putintoS: set of myNat ==> set of myNat
int SIMPLE_putintoS()
{
  char str[BUF], str_tmp[5];

  //printf("<<<start SIMPLE_putintoS()>>>\n"); // for test

  memset(str,     '\0', sizeof(str)); // reset str at first
  memset(str_tmp, '\0', sizeof(str_tmp)); // reset str at first
  
  strcat(str,"print SIMPLE`putintoS({");
  for (i=0; i<now.param.length; i++) {
    if (i==0) {
      snprintf(str_tmp, sizeof(str_tmp), "%d", now.param.numbers[i]);
	}
    else {
      snprintf(str_tmp, sizeof(str_tmp), ",%d", now.param.numbers[i]);
    }
    strcat(str,str_tmp);
  }
  strcat(str,"})\n");

  if (count_exec_num % 10000 == 0)
    printf("write: %s",str); // test

  temp = fifo_write(str);
  temp = fifo_read(0);

  //printf("<<<end SIMPLE_putintoS()>>>\n"); // for test
  return 1;
}


// SIMPLE`takefromS: set of myNat ==> set of myNat
int SIMPLE_takefromS()
{
  char str[BUF], str_tmp[5];

  //printf("<<<start SIMPLE_takefromS()>>>\n"); // for test

  memset(str,     '\0', sizeof(str)); // reset str at first
  memset(str_tmp, '\0', sizeof(str_tmp)); // reset str at first
  
  strcat(str,"print SIMPLE`takefromS({");
  for (i=0; i<now.param.length; i++) {
    if (i==0) {
      snprintf(str_tmp, sizeof(str_tmp), "%d", now.param.numbers[i]);
	}
    else {
      snprintf(str_tmp, sizeof(str_tmp), ",%d", now.param.numbers[i]);
    }
    strcat(str,str_tmp);
  }
  strcat(str,"})\n");

  if (count_exec_num % 10000 == 0)
    printf("write: %s",str); // test

  temp = fifo_write(str);
  temp = fifo_read(0);

  //printf("<<<end SIMPLE_putintoS()>>>\n"); // for test
  return 1;
}

int pre_SIMPLE_putintoS()
{
  return 1;
}

int pre_SIMPLE_takefromS()
{
  return 1;
}

