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
	      printf("ERROR returned by VDMJ interpreter\n");
          //printf("%s\n", strbuffer); // for test: should print out "Error ..."
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
        } 
        // for VDMJ - start

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

        switch(option) {
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

  temp = fifo_write("quit\n");
  temp = fifo_read(0);
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

// returns mtype (int) of matched string in mtype_map[]
int mtype_from_string(char* s)
{
  int i, len=sizeof(mtype_map)/sizeof(char*);
  for (i=0; i<len; i++)
    if (strncmp(s, mtype_map[i], strlen(mtype_map[i])) == 0)
        return i;

  if (i==len) {
    printf("Error: not found in mtype_from_string...\n");
    return 0;
  }
  return 1;
}

int report_number()
{
  count_exec_num++;  // test
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
int read_state_SAFER()
{
  temp = fifo_write("print SAFER`get_state()\n");
  temp = fifo_read(0); // read the response from VDMJ, the strimmed response will be stored in strbuffer
  //printf("state of SAFER after filtered...\n");
  //printf("%s\n", strbuffer); // for test: should print only first line
  //printf("dump state of SAFER...\n");
  temp = parse_vdmj();
  // assign value
  now.state_safer.clock = ast_root->u.child[0]->u.child[0]->u.num;
  //printf("<<<SAFER_clock = %d>>>\n", now.state_safer.clock); // for test

  free_AST(ast_root);
  return 1;
}

int read_state_AAH()
{
  struct AST *ast[4], *ast_tmp;
  int i;

  temp = fifo_write("print AAH`get_state()\n");
  temp = fifo_read(0); // read the response from VDMJ, the strimmed response will be stored in strbuffer
  //printf("state of AAH after filtered...\n");
  //printf("%s\n", strbuffer); // for test: should print only first line
  //printf("dump state of AAH...\n");
  temp = parse_vdmj();
  // assign value
  ast[0] = ast_root->u.child[0]->u.child[0];
  ast[1] = ast_root->u.child[0]->u.child[1]->u.child[0];
  ast[2] = ast_root->u.child[0]->u.child[1]->u.child[1]->u.child[0];
  ast[3] = ast_root->u.child[0]->u.child[1]->u.child[1]->u.child[1]->u.child[0];
  //for (i=0; i<4; i++) printf("type of ast[%d] : %s\n", i, AST_type_str[ast[i]->ast_type]); // for test

  for (i=0; i<3; i++) now.state_aah.active_axes[i] = 0; // reset all to 0
  if (ast[0]->ast_type != AST_set_empty) { // if not empty set, assign
    ast_tmp = ast[0]->u.child[0];
    i = 0;
    while (ast_tmp != NULL) {
    now.state_aah.active_axes[i] = mtype_from_string(ast_tmp->u.child[0]->u.name);
    i++;
    if (ast_tmp->num_child > 1) // more element(s)
	  ast_tmp = ast_tmp->u.child[1];
    else
	  ast_tmp = NULL;
    }
  }
  //printf("<<< state_aah.active_axes = { %s, %s, %s } >>>\n",
  //  mtype_map[now.state_aah.active_axes[0]],
  //  mtype_map[now.state_aah.active_axes[1]],
  //  mtype_map[now.state_aah.active_axes[2]]);

  for (i=0; i<3; i++) now.state_aah.ignore_hcm[i] = 0; // reset all to 0
  if (ast[1]->ast_type != AST_set_empty) { // if not empty set, assign
    ast_tmp = ast[1]->u.child[0];
    i = 0;
    while (ast_tmp != NULL) {
    now.state_aah.ignore_hcm[i] = mtype_from_string(ast_tmp->u.child[0]->u.name);
    i++;
    if (ast_tmp->num_child > 1) // more element(s)
	  ast_tmp = ast_tmp->u.child[1];
    else
	  ast_tmp = NULL;
    }
  }
  //printf("<<< state_aah.ignore_hcm = { %s, %s, %s } >>>\n",
  //  mtype_map[now.state_aah.ignore_hcm[0]],
  //  mtype_map[now.state_aah.ignore_hcm[1]],
  //  mtype_map[now.state_aah.ignore_hcm[2]]);

  now.state_aah.toggle = mtype_from_string(ast[2]->u.name);
  //printf("<<< state_aah.toggle = %s >>>\n", mtype_map[now.state_aah.toggle]);

  now.state_aah.mytimeout = ast[3]->u.num;
  //printf("<<< state_aah.timeout = %d >>>\n", now.state_aah.mytimeout);
  
  free_AST(ast_root);
  return 1;
}

int write_state_SAFER()
{
  char str[BUF];

  memset(str, '\0', sizeof(str)); // reset str at first
  snprintf(str, sizeof(str), "print SAFER`set_state(mk_SAFER`SAFER(%d))\n", now.state_safer.clock);

  if (count_exec_num % 10000 == 0)
    printf("write: %s",str); // test

  temp = fifo_write(str);
  temp = fifo_read(0);

  // show state for test
  //temp = fifo_write("print SAFER`get_state();\n");
  //temp = fifo_read(0);
  //printf("write resule: %s\n",strbuffer);

  //count_exec_num++;  // test
  //printf("<<<count_exec_num = %lu>>>\n", count_exec_num); // for test

  return 1;
}

int write_state_AAH()
{
  int i;
  char str[BUF];
  char aah1[BUF],aah2[BUF];

  memset(str,  '\0', sizeof(str)); // reset str at first
  memset(aah1, '\0', sizeof(str)); // reset str at first
  memset(aah2, '\0', sizeof(str)); // reset str at first
  
  strcat(aah1,"{");
  for (i=0; i<3; i++) {
    if (now.state_aah.active_axes[i] != 0) {
      if (i==0) {
        strcat(aah1, mtype_map[now.state_aah.active_axes[i]]);
	  }
      else {
        strcat(aah1, ", ");
        strcat(aah1, mtype_map[now.state_aah.active_axes[i]]);
      }
    }
  }
  strcat(aah1,"}");

  strcat(aah2,"{");
  for (i=0; i<3; i++) {
    if (now.state_aah.ignore_hcm[i] != 0) {
	  if (i==0) {
        strcat(aah2, mtype_map[now.state_aah.ignore_hcm[i]]);
	  }
	  else {
        strcat(aah2, ", ");
        strcat(aah2, mtype_map[now.state_aah.ignore_hcm[i]]);
	  }
    }
  }
  strcat(aah2,"}");

  snprintf(str, sizeof(str), "print AAH`set_state(mk_AAH`AAH(%s,%s,%s,%d))\n", aah1, aah2, mtype_map[now.state_aah.toggle], now.state_aah.mytimeout);

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
// SAFER`ControlCycle: HCM`SwitchPositions * HCM`HandGripPosition * AUX`RotCommand ==> TS`ThrusterSet
int SAFER_ControlCycle()
{
  char str[BUF];

  //printf("<<<start SAFER_ControlCycle()>>>\n"); // for test

  temp = restore_runtime_error(); // clear runtime error situation of VDMJ

  memset(str, '\0', sizeof(str)); // reset str at first
  snprintf(
    str, sizeof(str),
    "print SAFER`ControlCycle(mk_HCM`SwitchPositions(%s,%s),mk_HCM`HandGripPosition(%s,%s,%s,%s),{%s|->%s,%s|->%s,%s|->%s})\n",
    mtype_map[now.param1.mode], mtype_map[now.param1.aah],
    mtype_map[now.param2.vert], mtype_map[now.param2.horiz], mtype_map[now.param2.trans], mtype_map[now.param2.twist],
    mtype_map[now.param3.key[0]], mtype_map[now.param3.val[0]],
    mtype_map[now.param3.key[1]], mtype_map[now.param3.val[1]],
    mtype_map[now.param3.key[2]], mtype_map[now.param3.val[2]]
  );

  if (count_exec_num % 10000 == 0)
    printf("write: %s",str); // test

  temp = fifo_write(str);
  temp = fifo_read(0);

  //printf("<<<end SAFER_ControlCycle()>>>\n"); // for test
  return 1;
}

int test_error_msg()
{
  printf("<<<start test_error_msg()>>>\n"); // for test
  temp = fifo_write("print SAFER`ControlCycle(mk_HCM`SwitchPositions(<Tran>,<Down>),mk_HCM`HandGripPosition(<Neg>,<Pos>,<Pos>,<Pos>),{<Roll>|-><Pos>,<Pitch>|-><Pos>,<Neg>|-><Neg>});\n");
  temp = fifo_read(0);
  temp = restore_runtime_error();
  printf("<<<end test_error_msg()>>>\n"); // for test
}

