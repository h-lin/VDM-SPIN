
c_decl{
\#include "vending_types.c"
}

c_code{
\#include "vending_ops.c"
}

// sta_promela
typedef sta_promela {
  byte  s1 = 0;
  byte  s2[4]; // size = 4
  short s3 = 0;
  byte  s4[4]; // size = 4
  byte  s5[4]; // size = 4
  byte  s6[30];  // size = 30
  short s7;
}

sta_promela sta;
byte btn = 0;
bool VDM_Error = 0;

active proctype vending_machine_verifyer() {
  c_code{ initialization(); vending_initialization(); read_state_vending(); };

  do
     :: c_expr{ pre_vending_insert_bill() == 1 } ->
        c_code{ report_number(); write_state_vending(); vending_insert_bill(); read_state_vending(); };
     :: c_expr{ pre_vending_insert_coin1() == 1 } ->
        c_code{ report_number(); write_state_vending(); vending_insert_coin1(); read_state_vending(); };
     :: c_expr{ pre_vending_insert_coin2() == 1 } ->
        c_code{ report_number(); write_state_vending(); vending_insert_coin2(); read_state_vending(); };
     :: c_expr{ pre_vending_insert_coin3() == 1 } ->
        c_code{ report_number(); write_state_vending(); vending_insert_coin3(); read_state_vending(); };
     :: c_expr{ pre_vending_insert_coin4() == 1 } ->
        c_code{ report_number(); write_state_vending(); vending_insert_coin4(); read_state_vending(); };
     :: select(btn : 1..30);
        c_expr{ pre_vending_push_btn() == 1 } ->
        c_code{ report_number(); write_state_vending(); vending_push_btn(); read_state_vending(); };
     :: c_code{ report_number(); write_state_vending(); vending_refund(); read_state_vending(); };
//  :: break
  od;
}

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
