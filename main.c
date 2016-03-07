/*
  MSP430 Emulator
  Copyright (C) 2014, 2015 Rudolf Geosits (rgeosits@live.esu.edu)
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses
*/

#include "main.h"

int main(int argc, char *argv[])
{
  Cpu *msp430 = (Cpu *) calloc( 1, sizeof(Cpu) );
  msp430->p1 = (Port *) calloc ( 1, sizeof(Port) );
  msp430->usci = (Usci *) calloc ( 1, sizeof(Usci) );
  
  msp430->debugger = (Debugger *) calloc (1, sizeof(Debugger) );
  msp430->debugger->run = false;
  msp430->debugger->debug_mode = true;
  msp430->debugger->disassemble_mode = false;
  
  if (argv[1] == NULL) {
    display_help();
    exit(1);
  }

  pthread_t web_server_thread;
  if(pthread_create(&web_server_thread, NULL, web_server, 
		    (void *)msp430 )) 
    {
      fprintf(stderr, "Error creating thread\n");
      return 1;
    }

  register_signal(SIGINT);
  initialize_msp_memspace();
  initialize_msp_registers(msp430);  

  setup_port_1(msp430);
  setup_usci(msp430);
  open_pty(msp430);

  load_bootloader(0x0C00);
  load_firmware(argv[1], 0xC000);
  
  // Fetch-Decode-Execute Cycle
  while ( command_loop(msp430) ) {
    decode(msp430, fetch(msp430), EXECUTE); // Instruction Decoder
    handle_port_1(msp430);
    handle_usci(msp430);

    usleep(1);
  }

  uninitialize_msp_memspace(msp430);
  free(msp430->p1);
  free(msp430->usci);
  free(msp430);

  return 0;
}
