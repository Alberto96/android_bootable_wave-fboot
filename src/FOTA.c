/*
 ; This file is part of Badadroid project.
 ;
 ; Copyright (C) 2012 Rebellos, mijoma, b_kubica
 ;
 ;
 ; Badadroid is free software: you can redistribute it and/or modify
 ; it under the terms of the GNU General Public License as published by
 ; the Free Software Foundation, either version 3 of the License, or
 ; (at your option) any later version.
 ;
 ; Badadroid is distributed in the hope that it will be useful,
 ; but WITHOUT ANY WARRANTY; without even the implied warranty of
 ; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ; GNU General Public License for more details.
 ;
 ; You should have received a copy of the GNU General Public License
 ; along with Badadroid.  If not, see <http://www.gnu.org/licenses/>.
 ;
 ;
*/
#include "string.h"
#include "BL3.h"
#include "atag.h"


int main(runMode_t mode)
{
   void* kernelImageNAND = L"/g/zImage";  //"g" - internal memory | "e" - SD card
   void* kernelImageSD = L"/e/zImage";
   char* cmdlnRM = "bootmode=2 loglevel=4";
   char* cmdln = "loglevel=4";
   
   unsigned char ATAG_buf[512]={0};
   t_stat filestat;
   fun_t kernel;
   int fd;
   unsigned long kernelSize=0;
   
   //here we start the real deal :)
   int mmuctrl = MemMMUCacheEnable(gMMUL1PageTable, 1);
   disp_FOTA_Init();
   disp_FOTA_Printf("*----------------------------*");
   disp_FOTA_Printf("|      FOTA BOOTLOADER       |");
   disp_FOTA_Printf("*----------------------------*");
   disp_FOTA_Printf("|   Customized by            |");
   disp_FOTA_Printf("|                Alberto96   |");
   disp_FOTA_Printf("*----------------------------*");
   disp_FOTA_Printf("|                            |");
   disp_FOTA_Printf("|    Welcome to BadaDroid    |");
   disp_FOTA_Printf("|                            |");
   disp_FOTA_Printf("|       Please wait...       |");
   disp_FOTA_Printf("|                            |");
   disp_FOTA_Printf("*----------------------------*");

   //.... Your code here...

   __PfsNandInit();
   __PfsMassInit();
   MemoryCardMount();
   disp_FOTA_Printf("| Mounted partitions         |");
   tfs4_stat(kernelImageNAND, &filestat);
   kernelSize = filestat.st_size;
   if ((fd=tfs4_open(kernelImageNAND, 4)) >= 0)
   {
      disp_FOTA_Printf("| Found a kernel on NAND     |");
      tfs4_read(fd, &KERNEL_BUF, kernelSize);
      tfs4_close(fd);
   } 
   else
   {
      disp_FOTA_Printf("| Kernel not found on NAND   |");
      disp_FOTA_Printf("| Trying to find it on SD    |");
      if ((fd=tfs4_open(kernelImageSD, 4)) >= 0)
	  {
	    disp_FOTA_Printf("| Found a kernel on SD       |");
		tfs4_read(fd, &KERNEL_BUF, kernelSize);
        tfs4_close(fd);
	  }
	  else
	  {
	    disp_FOTA_Printf("| Kernel not found           |");
	    disp_FOTA_Printf("|                 even on SD |");
		disp_FOTA_Printf("*----------------------------*");
        disp_FOTA_Printf("|                            |");		
        disp_FOTA_Printf("|       Failed to boot!      |");
		disp_FOTA_Printf("|                            |");
        disp_FOTA_Printf("|  Pull out the battery and  |");
        disp_FOTA_Printf("| check if zImage is present |");
		disp_FOTA_Printf("|                            |");
        disp_FOTA_Printf("*----------------------------*");		
	  }
   }
	DisableMmuCache(mmuctrl);
	disp_FOTA_Printf("| Disabled MMU               |");
   _CoDisableMmu();
   
   //DRV_Modem_BootingStart
   
   setup_core_tag(ATAG_buf);
   disp_FOTA_Printf("| Setup ATAG                 |");
   setup_serial_tag(0x123, 0x456);
   setup_rev_tag('0');
   if (mode == rm_FOTA_RECOVERY)
   {
      setup_cmdline_tag(cmdlnRM);
	  disp_FOTA_Printf("| Recovery Mode Selected     |");
   }
   else
   {
      setup_cmdline_tag(cmdln);
	  disp_FOTA_Printf("| Normal Mode Selected       |");	  
   }   
   DRV_Modem_BootingStart();
   disp_FOTA_Printf("| Initialized Modem          |");   
   setup_end_tag();
   
   //copy kernel to the right position
   memcpy(&KERNEL_START, &KERNEL_BUF, kernelSize);
   disp_FOTA_Printf("| Loaded Kernel in Memory    |");
   disp_FOTA_Printf("*----------------------------*");
   disp_FOTA_Printf("|                            |");
   disp_FOTA_Printf("|          Booting!          |");
   disp_FOTA_Printf("|                            |");
   disp_FOTA_Printf("*----------------------------*"); 
   
   //SYSCON operations
   *((unsigned int*)SYSCON_NORMAL_CFG) = 0xFFFFFFFF; 
   _CoDisableDCache();
   _System_DisableVIC();
   _System_DisableIRQ();
   _System_DisableFIQ();
   
   kernel = (fun_t)&KERNEL_START;
   kernel(0, 8500, ATAG_buf); //8500 - Wave I | 8530 - Wave II
   //disp_FOTA_Printf("If you can read this");
   //disp_FOTA_Printf("something went wrong");
   
   //loop forever
   while(1);
   
   return 0;

}