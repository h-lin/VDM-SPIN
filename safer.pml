//#define a1 (c_expr{leq_zero()})
//#define a2 (c_expr{eq_hundred()})

c_decl{
\#include "safer_types.c"
}

//c_state "int SAFER_clock" "Global"

c_code{
\#include "safer_ops.c"
}

// quote types of SAFER specification
mtype = { m_Rot, m_Tran }; // HCM`ControlModeSwitch
mtype = { m_Up, m_Down };  // HCM`ControlButton
mtype = { m_Neg, m_Zero, m_Pos };   // AUX`AxisCommand
mtype = { m_Roll, m_Pitch, m_Yaw }; // AUX`RotAxis
mtype = { m_AAH_off, m_AAH_started, m_AAH_on, m_pressed_once, m_AAH_closing, m_pressed_twice }; // AAH`EngageState

// state of SAFER module
typedef SAFER_SAFER {
  byte clock = 0; // clock : nat
}

// state of AAH module
typedef AAH_AAH {
  mtype active_axes[3];     // active_axes : set of AUX`RotAxis
  mtype ignore_hcm[3];      // ignore_hcm  : set of AUX`RotAxis
  mtype toggle = m_AAH_off; // toggle      : AAH`EngageState
  byte mytimeout = 0;       // timeout     : nat
}

typedef HCM_SwitchPositions {
  mtype mode = m_Rot; // mode: HCM`ControlModeSwitch
  mtype aah = m_Up;   // aah : HCM`ControlButton
};

typedef HCM_HandGripPosition {
  mtype vert = m_Zero;  // vert  : AUX`AxisCommand
  mtype horiz = m_Zero; // horiz : AUX`AxisCommand
  mtype trans = m_Zero; // trans : AUX`AxisCommand
  mtype twist;          // twist : AUX`AxisCommand
};

// RotCommand = map RotAxis to AxisCommand
//  inv cmd == dom cmd = rot_axis_set;
typedef AUX_RotCommand {
  mtype key[3] = { m_Roll, m_Pitch, m_Yaw };
  mtype val[3];
};

inline sel_nat(number)
{
  number = 0;
  do
  :: number++;
  :: break
  od;
}

inline sel_AUX_RotAxis(rotaxis)
{
  rotaxis = 0;
  do
  :: rotaxis = m_Roll;
  :: rotaxis = m_Pitch;
  :: rotaxis = m_Yaw;
  :: break
  od;
}

/*
inline sel_set_AUX_RotAxis(elem1,elem2,elem3)
{
  sel_AUX_RotAxis(elem1);
  if
  :: (elem1 == 0) -> elem2 = 0;
  :: (elem1 != 0) -> 
     do
     :: sel_AUX_RotAxis(elem2);
     :: if
        :: (elem1 != elem2) -> break;
        :: (elem1 == elem2) -> skip; // if a2 is same as a1, skip to select again
        fi;
     od;
  fi;
  if
  :: (elem2 == 0) -> elem3 = 0;
  :: (elem2 != 0) ->
     do
     :: sel_AUX_RotAxis(elem3);
     :: if
        :: (elem2 != elem3 && elem1 != elem3) -> break;
        :: (elem2 == elem3 || elem1 == elem3) -> skip; // if a3 is same as a1 or a2, skip to select again
        fi;
     od;
  fi;
}
*/

inline sel_set_AUX_RotAxis(elem1,elem2,elem3)
{
  elem1 = 0; elem2 = 0; elem3 = 0;
  do
  :: elem1 = m_Roll; elem2 = 0; elem3 = 0;
  :: elem1 = m_Pitch; elem2 = 0; elem3 = 0;
  :: elem1 = m_Yaw; elem2 = 0; elem3 = 0;
  :: elem1 = m_Roll; elem2 = m_Pitch; elem3 = 0;
  :: elem1 = m_Roll; elem2 = m_Yaw; elem3 = 0;
  :: elem1 = m_Pitch; elem2 = m_Yaw; elem3 = 0;
  :: elem1 = m_Roll; elem2 = m_Pitch; elem3 = m_Yaw;
  :: break
  od;
}

inline sel_AAH_EngageState(a)
{
  a = m_AAH_off;
  do
  :: a = m_AAH_started;
  :: a = m_AAH_on;
  :: a = m_pressed_once;
  :: a = m_AAH_closing;
  :: a = m_pressed_twice;
  :: break
  od;
}

inline sel_SAFER_SAFER(a)
{
  //sel_nat(a.clock);
  //a.clock = 1;
  // only need 0 or 1 (while state_aah.mytimeout = 0)
  a.clock = 0;
  do
  :: a.clock = 1;
  :: break
  od;
}

inline sel_AAH_AAH(a)
{
  sel_set_AUX_RotAxis(a.active_axes[0],a.active_axes[1],a.active_axes[2]);
  sel_set_AUX_RotAxis(a.ignore_hcm[0],a.ignore_hcm[1],a.ignore_hcm[2]);
  sel_AAH_EngageState(a.toggle);
  //sel_nat(a.mytimeout);
  a.mytimeout = 0;
}

inline sel_HCM_ControlModeSwitch(a)
{
  a = m_Rot;
  do
  //:: a = m_Rot;
  :: a = m_Tran;
  :: break
  od;
}

inline sel_HCM_ControlButton(b)
{
  b = m_Up;
  do
  //:: b = m_Up;
  :: b = m_Down;
  :: break
  od;
}

inline sel_AUX_AxisCommand(a)
{
  a = m_Zero;
  do
  :: a = m_Neg
  //:: a = m_Zero
  :: a = m_Pos
  :: break
  od;
}

inline sel_HCM_SwitchPositions(a)
{
  sel_HCM_ControlModeSwitch(a.mode);
  sel_HCM_ControlButton(a.aah);
}

inline sel_HCM_HandGripPosition(a)
{
  sel_AUX_AxisCommand(a.vert);
  sel_AUX_AxisCommand(a.horiz);
  sel_AUX_AxisCommand(a.trans);
  sel_AUX_AxisCommand(a.twist);
}

inline sel_AUX_RotCommand(a)
{
  sel_AUX_AxisCommand(a.val[0]);
  sel_AUX_AxisCommand(a.val[1]);
  sel_AUX_AxisCommand(a.val[2]);
}


local SAFER_SAFER state_safer;
local AAH_AAH     state_aah;

local HCM_SwitchPositions param1;
local HCM_HandGripPosition param2;
local AUX_RotCommand param3;
bool VDM_Error = 0;

active proctype safer() {
  //c_code{ printf("initialization...\n"); };
  c_code{ initialization(); };
  //c_code{ initialization(); read_state_SAFER(); read_state_AAH(); };

  //do
  //::
  sel_SAFER_SAFER(state_safer);
  sel_AAH_AAH(state_aah);
  sel_HCM_SwitchPositions(param1);
  sel_HCM_HandGripPosition(param2);
  sel_AUX_RotCommand(param3);
  //c_code{ report_number(); }
  //c_code{ report_number(); SAFER_ControlCycle(); }
  c_code{ report_number(); write_state_SAFER(); write_state_AAH(); SAFER_ControlCycle(); }
  //c_code{ write_state_SAFER(); write_state_AAH(); SAFER_ControlCycle(); read_state_SAFER(); read_state_AAH(); }
  //od;

  //c_code{ test_error_msg(); }
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
