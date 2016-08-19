//#define a1 (c_expr{leq_zero()})
//#define a2 (c_expr{eq_hundred()})

c_decl{
\#include "simple_types.c"
}

//c_state "int SAFER_clock" "Global"

c_code{
\#include "simple_ops.c"
}

// state of SIMPLE module
typedef SIMPLE_S {
  byte length = 0;  // size
  byte pool[10]; // pool : set of myNat  (0~9)
}

// set of nat (input of operations in SIMPLE module)
typedef set_myNat {
  byte length = 0;
  byte numbers[2]; // max length: 2
}

inline sel_bool(b)
{
  b = 0;
  do
  :: b = 1;
  :: break
  od;
}

inline sel_nat(aaa, min, max) // select a number between (min,max)
{
  printf("here!\n");
  aaa = min;
  do
  :: (aaa < max) -> aaa++;
  :: break
  od;
}

inline sel_num(aa, min, max) // select a number between (min,max)
{
  printf("here1!\n");
  aa = min;
  do
  :: (aa < max) -> aa++;
  :: break
  od;
}

inline sel_SIMPLE_S(a)
{
  a.length = 0;
  sel_bool(flag);
  printf("here_sel_simple_s\n");
  if
  :: (flag == 1) -> {
       sel_num(a.pool[0],0,9); a.length++;
       for (itmp: 1 .. 9) {
         sel_bool(flag);
         if
         :: (flag == 1 && a.pool[itmp-1] < 9) -> sel_num(a.pool[itmp],a.pool[itmp-1]+1,9); a.length++;
         :: else -> break
         fi;
       }
     }
  :: else -> skip;
  fi;
  // set the rest to 0
  for (itmp: a.length .. 9) { a.pool[itmp] = 0; }

  printf("state_simple: length = %d", a.length);
  printf("state_simple: pool = {");
  for (itmp: 0 .. 9) { printf(" %d,", a.pool[itmp]); }
  printf("}\n");
}

inline sel_set_mynat_nonempty(s)
{
  sel_nat(s.numbers[0],0,9); s.length = 1;
  for (itmp: 1 .. 1) {
    sel_bool(flag);
    if
    :: (flag == 1) && (s.numbers[itmp-1] < 9) -> sel_nat(s.numbers[itmp],s.numbers[itmp-1]+1,9); s.length++;
    :: else -> break
    fi;
  }
  // set the rest to 0
  for (itmp: s.length .. 1) { s.numbers[itmp] = 0; }

  printf("param: length = %d", s.length);
  printf("param: numbers = {");
  for (itmp: 0 .. 1) { printf(" %d,", s.numbers[itmp]); }
  printf("}\n");
}
/*
inline sel_set_mynat_nonempty(s)
{
  sel_nat(s.numbers[0],0,9); s.length = 1;
  do
  :: (s.numbers[0] < 9) -> sel_nat(s.numbers[1],s.numbers[0]+1,9); s.length=2;
  :: s.numbers[1] = 0; s.length=1; break
  od;
}
*/
byte itmp;
bool flag;

SIMPLE_S state_simple;
set_myNat param;
bool VDM_Error = 0;

active proctype simple() {
  //c_code{ initialization(); };
  c_code{ initialization(); read_state_SIMPLE(); };

/*
  // non-loop ver
  sel_SIMPLE_S(state_simple);
  sel_set_mynat_nonempty(param);
  if
  :: c_expr{ pre_SIMPLE_putintoS() } ->
     c_code{ report_number(); }
     c_code{ write_state_SIMPLE(); }
     c_code{ SIMPLE_putintoS(); } //read_state_SIMPLE(); };
  :: c_expr{ pre_SIMPLE_takefromS() } ->
     c_code{ report_number(); }
     c_code{ write_state_SIMPLE(); }
     c_code{ SIMPLE_takefromS(); } //read_state_SIMPLE(); };
  fi;
*/
  // loop ver
  do
  :: sel_set_mynat_nonempty(param);
     if
     :: c_expr{ pre_SIMPLE_putintoS() } ->
        c_code{ report_number(); write_state_SIMPLE(); SIMPLE_putintoS(); }; c_code{ read_state_SIMPLE(); };
     :: c_expr{ pre_SIMPLE_takefromS() } ->
        c_code{ report_number(); write_state_SIMPLE(); SIMPLE_takefromS(); }; c_code{ read_state_SIMPLE(); };
     fi;
  :: break
  od;
}

/*
init {
  //c_code{ initialization(); };
  c_code{ initialization(); read_state_SIMPLE(); };
  run producer();
  run consumer();
  
}
*/

/* Never Claim */
never  {    /* <> (VDM_Error==1) */
	skip
T0_init:
	do
	:: atomic { ((VDM_Error==1)) -> assert(!((VDM_Error==1))) }
	:: (1) -> goto T0_init
	od;
accept_all:
	skip
}
