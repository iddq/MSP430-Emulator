#include "decoder.h"

void decoder(unsigned char* MEM){
  char reg_name[10];
  unsigned char opcode;
  int done = 0;

  while( !done ){
    opcode = *( (unsigned char*)MEM + 1 ) >> 4;
    memset(reg_name, 0, sizeof(reg_name));
    printf("Got opcode 0x%X\n", opcode);

    if(opcode == 0x1){
      unsigned short instruction = *( (unsigned short*)MEM );
      unsigned char opcode = ( instruction & 0x0380 ) >> 7;
      unsigned char bw_flag = ( instruction & 0x0040 ) >> 6;
      unsigned char as_flag = ( instruction & 0x0030 ) >> 4;
      unsigned char source_reg = ( instruction & 0x000F );
      
      reg_num_to_name(source_reg, reg_name);      

      switch(opcode){
        //# RRC Rotate right through carry
        case 0x0:{
	  if(bw_flag == 1){
	    printf("RRC.B ");
	  }
	  else{
	    printf("RRC ");
	  }
	    
	  //# direct register 
	  if(as_flag == 0x0){
	    printf("%s\n", reg_name);
	  }
	  //# indirect register
	  else if(as_flag == 0x2){
	    printf("@%s\n", reg_name);
	  }
	  //# Indirect autoincrement
	  else if(as_flag == 0x3){
	    printf("@%s+\n", reg_name);
	  }	    

	  break;
	}
	//# SWPB Swap bytes 
	//# bw flag always 0 (word)
	case 0x1:{
	  if( as_flag == 0x0 ){ 
	    printf("SWPB %s\n", reg_name);
	  }
	  else if( as_flag == 0x2 ){
	    printf("SWPB @%s\n", reg_name);
	  }
	  else if( as_flag == 0x3 ){
	    printf("SWPB @%s+\n", reg_name);
	  }
	  
	  break;
	}
        //# RRA Rotate right arithmetic
        case 0x2:{
	  if(bw_flag == 0x1){
	    printf("RRA.B ");
	  }
	  else{
	    printf("RRA ");
	  }

	  if(as_flag == 0x0){
	    printf("%s\n", reg_name);
	  }
	  else if(as_flag == 0x2){
	    printf("@%s\n", reg_name);
	  }
	  else if(as_flag == 0x3){
	    printf("@%s+\n", reg_name);
	  }

	  break;
	}
	//# SXT Sign extend byte to word
	//# bw flag always 0 (word)
        case 0x3:{
	  if(as_flag == 0x0){
	    printf("SXT %s\n", reg_name);
	  }
	  if(as_flag == 0x2){
	    printf("SXT @%s\n", reg_name);
	  }
	  if(as_flag == 0x3){
	    printf("SXT @%s+\n", reg_name);
	  }

	  break;
	}
	//# PUSH push value on to the stack
        case 0x4:{
	    printf("PUSH %s", reg_name);
	  break;
	}
	//# Call subroutine; push PC and move source to PC
        case 0x5:{
	  printf("CALL %s", reg_name);
	  break;
	}
	//# RETI Return from interrupt: Pop SR then pop PC
        case 0x6:{
	  printf("RETI\n");
	  break;
	}
        default:{
	  printf("Unknown Single operand instruction.\n");
        }
      }
      
      MEM += 2;
    }    
    else if(opcode >= 0x2 && opcode <= 3){
      unsigned short instruction = *( (unsigned short*)MEM );
      unsigned char condition = ( instruction & 0x1C00 ) >> 10;
      //# 10 bit signed offset
      unsigned short signed_offset = ( instruction & 0x03FF );

      //printf("Jump Code 0x%X; Condition 0x%X\n", opcode, condition);
      switch(condition){
	 //# JNE/JNZ Jump if not equal/zero
         case 0x0:{
	   printf("JNZ 0x%04X\n", signed_offset);
	   break;
         }
	 //# JEQ/JZ Jump is equal/zero
         case 0x1:{
	   printf("JZ 0x%04X\n", signed_offset);
	   break;
	 }
	 //# JNC/JLO Jump if no carry/lower
         case 0x2:{
	   printf("JNC 0x%04X\n", signed_offset);
	   break;
	 }
	 //# JC/JHS Jump if carry/higher or same
         case 0x3:{
	   printf("JC 0x%04X\n", signed_offset);
	   break;
	 }
	 //# JN Jump if negative
         case 0x4:{
	   printf("JN 0x%04X\n", signed_offset);
	   break;
         }
	 //# JGE Jump if greater or equal (N == V)
	 case 0x5:{
	   printf("JGE 0x%04X\n", signed_offset);
	   break;
         }
	 //# JL Jump if less (N != V)
         case 0x6:{
	   printf("JL 0x%04X\n", signed_offset);
	   break;
         }
	 //# JMP Jump Unconditionally
         case 0x7:{
	   printf("JMP 0x%04X\n", signed_offset);
	   break;
         }
         default:{
	   printf("Undefined Jump operation\n");
         }
      }
      
      MEM += 2;
    }
    else if(opcode >= 0x4){
      unsigned short instruction = *( (unsigned short*)MEM );
      unsigned char source_reg = ( instruction & 0x0F00 ) >> 8;

      //# Name source register
      //switch(source_reg){
	
      //}

      switch(opcode){
        //# MOV source to destination
        case 0x4:{
	  printf("MOV 0x%04X\n", source_reg);
	  break;
	}
        //# ADD source to destination
        case 0x5:{
	  printf("ADD 0x%04X\n", source_reg);
	  break;
	}
	//# ADDC Add w/carry dst += (src+C)
        case 0x6:{
	  printf("ADDC 0x%04X\n", source_reg);
	  break;
        }
	//# SUBC Sub w/carry dst -= (src+C)
        case 0x7:{
	  printf("SUBC 0x%04X\n", source_reg);
	  break;
        }
      }

      MEM += 4;
    }
    else{
      MEM += 2;
    }
    
    command_loop();

  }
}
