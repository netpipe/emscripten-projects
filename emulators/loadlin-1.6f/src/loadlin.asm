TESTX = 0
LOADLIN_VERSION equ '1.6f'

;   >>> this is file LOADLIN.ASM
;============================================================================
;   LOADLIN v1.6 (C) 1994..1996 Hans Lermen (lermen@elserv.ffm.fgan.de)
;   (C) 2008..2010 Samuel Thibault (samuel.thibault@ens-lyon.org)
;
;   This program is free software; you can redistribute it and/or modify
;   it under the terms of the GNU General Public License as published by
;   the Free Software Foundation; either version 2 of the License, or
;   (at your option) any later version.
;
;   This program is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;   GNU General Public License for more details.
;
;   You should have received a copy of the GNU General Public License
;   along with this program; if not, write to the Free Software
;   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
;
;----------------------------------------------------------------------------
;   Comments and bug reports are welcome and may be sent to:
;   E-Mail:    samuel.thibault@ens-lyon.org
;
;============================================================================
;
; NOTE:
;
;   This program could not have been written as quickly without
;   the information found in the source code of F.Coutant's BOOTLIN
;
;   This program contains some modified source code from
;   my protected mode extender (LDOSX (C) 1991..1994 H.Lermen),
;   which was written as a fast multitasking alternative
;   to DJ.Delorie's extender (it runs DJ-GCC compiled binaries under DOS).
;
;   I was too lazy to reprogram the subroutines completely, so,
;   if You wonder what nonsense they are doing, keep in mind that they
;   were written for an other purpose.
;
;============================================================================
;
; Contributions and bug fixes for 1.5:
;
;
;   Javier Achirica <javier@autom.uva.es>
;
;       invented the switch-out-of-setup method (Javier's method),
;       which makes the BIOSINTV and REALBIOS method superfluous on nearly
;       all machines. The trick is: let setup run in V86 and intercept
;       just before going to protected mode.
;       (Thank you Javier for this very good work).
;
; Contributions for 1.6:
;
;   Werner Almesberger <almesber@lrc.epfl.ch>
;
;       Werner and me cooperated in realizing high loading of the kernel
;       and preloading the RAMdisk.
;       The changes needed in setup.S for both, LILO and LOADLIN,
;       are a common work of us.
;       Also the needed kernel patches are the fruits of a very fertile
;       cooperation. The changes to LILO-1.8 as well as to LOADLIN-1.6
;       have been developed 'synchroniously' in order to let the whole
;       Linux community participate on the new boot features.
;
;============================================================================

              name    load_linux
              .386
              locals
              jumps

REALBIOS_FILE equ 'C:\REALBIOS.INT'

SIGNATURE                = 'SrdH' ; "HdrS"

space4k                  =     1000h
space2k                  =      800h
space1k                  =      400h

our_stacksize            =     space2k

kernel_start_ equ     01000h     ; here the kernel must go
kernel_end    equ     09000h
standard_setup_sects  equ 4      ; number of setup sectors, older kernels
maximum_setup_sects   equ (48-1) ; max number of setup sectors for newer kernels
High_Seg_     equ     kernel_end  ; here first 512 + 4*512 + n*512 bytes of image must go
High_Addr_    equ     (High_Seg_*16)

setup_intercept_int equ   7fh


debug_stop macro marker
           mov   ax,0b800h
           mov   ds,ax
           mov   byte ptr ds:[1],70h
           mov   byte ptr ds:[0],marker
           jmp   short $
           endm

align_  macro   start,val
        org     (((($-start)+(val-1))/val)*val)
        endm


psp_seg segment  at 0 use16

; -------------PSP Program Segment Prefix----------------------
        org      2h
  PSP_memend_frame            dw ?
        org      2ch
  PSP_envir_frame             dw ?
        org      80h
  PSP_DTA                     db ? (100h-80h) dup(?)
; -------------------------------------------------------------
psp_seg ends

code    segment  para use16
        assume   cs:code,ds:psp_seg,es:code
code_org0     label   byte

; ---------------------------------------------------------------------
;                        Bootsector (512 bytes)
;                        within this BEFORE start of setup:
;                        (may be set by "LOADLIN")
bootsec       label   byte
              org     20h
CL_MAGIC      dw      ?  ;0020   commandline magic number (=0xA33F)
CL_OFFSET     dw      ?  ;0022   commandline offset
                         ; Address of commandline is calculated:
                         ;   0x90000 + contents of CL_OFFSET
                         ; The command line is parsed by "init/main.c"
                         ; Value of NAME=XXXX are put into the environement
                         ; and can then be interpreted by the drivers
                         ; and /etc/rc. The variabel "root=xxxx" is
                         ; interpreted by main directly, "single" is
                         ; interpreted by init or simpleinit.
              org     1F1h
setup_sects   db      ?  ; no. of sectors
ro_flag       dw      ?  ; =0: root file system should be mounted read-write
                         ;<>0: root file system should be mounted readonly
                         ; (this will be overwritten by the kernel commandline
                         ;  options "ro" / "rw")
;             --------------------------------
;                        within this AFTER setup has run:
              org     0
curr_curs     dw      ?  ;0000 saved cursor position
ext_mem_size  dw      ?  ;0002     extended memory size in Kb (from int 0x15)
              org     80h
hd0_disk_par  label   byte ;080     hd0-disk-parameter from intvector 0x41
hd1_disk_par  label   byte ;090     hd1-disk-parameter from intvector 0x46

;             --------------------------------
;                        within this as loaded from "zImage"
              org     01F4h
kernel_size16 dw      ?  ; size of kernel-part in the image-file
                         ; (in 16 byte units, rounded up)
swap_dev      dw      ?  ; swap device

ram_disk      dw      ?  ;01F8 size of ram-disk (in 1Kb units ) or ZERO
                         ; if ram_disk is nonZERO then the kernel
                         ; (driver/block/ramdisk.c: rd_load() )
                         ; will try to load the contents for the ram-disk
                         ; from the "root_dev" which MUST then have the
                         ; floppyMAJOR.
                         ; The file-system on that floppy must be MINIX
                         ; If rd_load() succeeds it sets the root_dev
                         ; to the ramdisk for mounting it.
                         ;
                         ; LOADLIN parses the commandline for the string
                         ; "ramdisk=nnn" where nnn is the value for "ram_disk"
                         ; after the kernel images has been loaded by LOADLIN
                         ; it asks for inserting the floppy.
                         ; (NOTE: You may have LOADLIN in A: and root_dev in B:
                         ;        or vice versa).

vga_mode      dw      ?  ;01FA VGA-Mode
                         ;      -3 = ask
                         ;      -2 = Extended VGA
                         ;      -1 = Normal VGA
                         ;       0 = as "0" was pressed
                         ;       n = as "n" was pressed
                         ;
                         ; LOADLIN parses the commandline for the string
                         ; "vga=nnn" where nnn is the value for "vga_mode"
                         ; it also excepts:
                         ; "vga=ask","vga=normal",'vga=extended"

root_dev      dw      ?  ;01FC Root Device (high=Major, low=minor)
                         ;(this can be overwritten by the kernel commandline
                         ; option "root=XXXX")
bootmagic     dw      ?  ;01FE Bootsector magic (0AA55h)

; -------------------------------------------------------------------
                         ; this area will be used to pass params
                         ; from LOADLINX to LOADLIN, if the params file name is @@loadlinx@@ switch
                         ;
                         ; NOTE: '@@loadlinx@@' or '@@loadliXXXX'
                         ;       must NOT be used,
                         ;       if starting LOADLIN without LOADLINX or ULOADLIN
              org     0200h
params_from_loadlinX label byte


; -------------------------------------------------------------------
              org     0200h
setup_prog    label   byte
                                    ; the setup-program itself
                                    ; must be started at 9020h:0 !!!
                                    ;                    =======
              jmp     short start_of_setup
                                    ; the setup header
                                    ; (if you have applied the setup.S patch,
                                    ;  or later, if we have it in the standard kernel)
setup_header_sign     dd    ?
setup_header_version  dw    ?
setup_realmode_switch dd    ?
start_sys_seg         dw    ?
kernel_version        dw    ?
                                    ; end of v1.5-header
                                    ; NOTE: above part of header is compatible
                                    ;       with loadlin-1.5 (header v1.5),
                                    ;       must not change it

type_of_loader        db   0        ; = 0, old one (LILO, Loadlin,
                                    ;      Bootlin, SYSLX, bootsect...)
                                    ; else it is set by the loader:
                                    ; 0xTV: T=0 for LILO
                                    ;       T=1 for Loadlin
                                    ;       T=2 for bootsect-loader
                                    ;       V = version
loadflags             db   0        ; unused bits =0
                                    ; (reserved for future development)
LOADED_HIGH           =    1        ; bit within loadflags,
                                    ; if set, then the kernel is loaded high
CAN_USE_HEAP          =   80h       ; if set, the loader also has set heap_end_ptr
                                    ; to tell how much space behind setup.S
                                    ; can be used for heap purposes.
                                    ; Only the loader knows what is free!
setup_move_size       dw  8000h     ; size to move, when we (setup) are not
                                    ; loaded at 0x90000. We will move ourselves
                                    ; to 0x90000 then just before jumping into
                                    ; the kernel. However, only the loader
                                    ; know how much of data behind us also needs
                                    ; to be loaded.
code32_start          dd  1000h     ; here loaders can put a different
                                    ; start address for 32-bit code.
                                    ;   0x1000 = default for zImage
                                    ; 0x100000 = default for big kernel
ramdisk_image         dd  0         ; address of loaded ramdisk image
                                    ; Here the loader (or kernel generator) puts
                                    ; the 32-bit address were it loaded the image.
                                    ; This only will be interpreted by the kernel.
ramdisk_size          dd  0         ; it's size in bytes
bootsect_kludge       dd  0         ; pointing to boot_sect_helper
heap_end_ptr          dw  0         ; pointing to end of setup loacal heap.
                                    ; Space from here (exclusive) down to
                                    ; end of setup code can be used by setup
                                    ; for loacal heap purposes.
                  ; ---- end of v2.0 setup-header --------------------

start_of_setup:
              org     setup_prog
              db      maximum_setup_sects*512 dup(?)
; -------------------------------------------------------------
; the following layout is private to LOADLIN.EXE :

                align_ code_org0,4096
end_of_setup_buffer label byte

pagedir         dd    2 dup(0)  ; must be aligned to 4 K
                                ; NOTE: we have only 1 (one) pagetable, so we
                                ;       need only 1 pagedir entry
                                ;       All current known CPUs (386,486,PENTIUM)
                                ;       tolerate the garbage behind this,
                                ;       as long as there is no access > 4MB.
                                ;       So we may overlap the pagedir with
                                ;       our code

    ; ----------------------------v
    ; the following are the params we have to pass to 32-bit adjustemt code
pageadjlist_ptr  dd    (High_Seg_*16) + (pageadjlist-code_org0)
real_32_startup  dd    1000h
    ; ----------------------------^
; -------------------------------------------------------------
                align_ code_org0,16
;                org    02010h
startup_32:
                ; here we insert the GCC compiled 32-bit code part
                ; it will be our point to start Linux
                ; it's current address is 0x95010
                ;            CHECKIT------^^^^^^^ !!!
IF 1
     INCLUDE PGADJUST.ASM
ELSE
     INCLUDE PGADJTES.ASM
ENDIF
; -------------------------------------------------------------


PSP_frame       dw    0  ; our psp
fhandle         dw    0  ; file handle of imagefile
kernel_start    dw    0  ; place were the kernel must at time of
                         ; start of setup
free_mem_start  dw    0  ; frame of free memory, starting at the begin of LOADLIN.EXE
kernel_load_frame dw  0  ; where to load the image

                         ; The following (High_Seg,High_Addr)
                         ; will be set to lower values, if we have setup v2.0
                         ; _and_ if 9000 is occupied (W95+DRVSPACE)
High_Seg        dw    kernel_end ; here first 512 + 4*512 + n*512 bytes of image must go
;spaeter, damit syntaxfehler enstehen:  High_Addr       dd    (High_Seg_*16)

    ; ----------------------------v
    ; the following values are cleared on each call to "parscommandline"
    ; (see "clear_to_default")
parse_switches  label byte
new_setup_size  dw    0,0	; NOTE: need the high word to be 0
new_vga_mode    dw    0
new_ram_disk    dw    0
cl_pointer      dw    0  ; while parsing: aux pointer to command_line
got_vga_mode    db    0
got_ram_disk    db    0
option_v        db    0
option_t        db    0
option_t_forced db    0
option_realbios db    0
option_rx       db    0
option_ja       db    0
option_clone    db    0
option_oldxd    db    0
option_n        db    0
option_nodiskprompt db 0
option_force    db    0
option_initrd   db    0
option_noheap   db    0
intv_size       dw    0
option_wait     dw    0
option_dskreset db    0
wrong_realbios  db    0
have_to_force_realmode db 0 ; 0 = is in realmode
                            ; 1 = is in V86, have to reenter realmode before
                            ;     kernel goes to protected mode
                            ;
have_to_intercept_setup db 0; 0 = old method, no interception
                            ; 1 = intercept boot/setup.S (Javier's method)
                            ;     of older kernels just before going to
                            ;     protected mode.
                            ; 2 = same as 1, but intercepting newer kernels
                            ;     with the setup.dif patch applied.
                            ;

debug_file_handle dw    0  ; set to file handle if option -d is set
logo_out        db    0
kernelversion   dd    0     ; binary kernel version, decoded from
                            ; the version string as follows:
                            ; "1.2.3 (root@...) #4" becomes 01020304h
                            ; if string is only 1.2.3  ---> #0 is assumed
token_count     db    0
end_of_physmem  dd    0
parse_switches_end label byte
    ; ----------------------------^

can_exit_to_dos db    0
have_VCPI       db    0
have_big_kernel db    0
have_relocated_setup db  0
cannot_load_because_of_windows db 0
print_dots      db    0
cpu_check_status dw   0
setup_version    dw   0       ; =0, if old setup
                              ; else contents of setup_header_version

kernel_size	dd    0       ; will be either calculated from kernel_size16
			      ; or from the filesize of the image

;--------------------higmem stuff -----v
xms_entry dd      0
xms_avail dw      0
xms_handle dw      0
xms_phys_addr dd   0

pblock  struc
  taddr  DD   ? ; linear address where the block of pages
                ; must be moved to
; 65536 pages -> 256MB max
  tstart DW   ? ; index within sources of first entry
  tcount DW   ? ; number of entries for taddr in sources
pblock  ends

pages_list struc
  ncount    DD     ?       ; number of entries in 'sources'
  number_of_blocks DD ?    ; number of valid blocks-items
  auxbuf    DD     ?       ; address of 4096 bytes auxiliary buffer
  blocks    pblock 4 dup(?)
; Dynamic pgadjust uses one indirection.
; 128 pages of adresse+usedby gives us the above 65536 maximum pages.
  sources   DD     128 dup (?) ; list of addresses where the block of pages
pages_list ends



need_mapped_put_buffer db  0
do_mapped_put_buffer db  0    ; 1, if need high load over pagemap

load_buffer_size dd   0
heap_ptr       dd     0
heap_end       dd     0
high_heap_ptr  dd     0
heap_max_pages dd     0

move_anywhere  dw     move_simple   ; this routine gets called
                                    ; when needing to move buffers

high_mem_access db     0     ; = 0, if nothing available
USING_VCPI     =      1
USING_INT15    =      2
USING_XMS      =      3


;-----------------------------^
;NOTE: all uninitialized data has been moved to end of modul
;      since version 1.4

; -------------------------------------------------------------


  _DEALLOCATE_PAGES          = 45h;
  _GET_VERSION               = 46h;
  _ALLOCATE_RAW_PAGES        = 5A01H
  EMM_int                    = 67h;



  descript    struc
    limit     dw    ?
    base0     dw    ?
    base16    db    ?
    typbyte   db    ?
    limit16   db    ?
    base24    db    ?
  descript    ends


  Gdescript struc
    gateoffs0    dw      ?
    gateselector dw      ?
    gatenotused  db      ?
    gatetyppbyte db      ?
    gateoffs16   dw      ?
  Gdescript ends


  ; definition of COMMON decriptor types (bit0..4 of descript.typbyte )
  ; (bit 4 of descript.typbyte =0)
  data_d        =     10000b    ; data segment descriptor
    writable    =     00010b    ; =1 if write acces allowed to data segment
    expand_down =     00100b    ; =1 limit counts down from base
  code_d        =     11000b    ; code segment
    readable    =     00010b    ; =1 if code also can be read (cannot be ovwritten)
    conforming  =     00100b    ; =1 code can be accesses and executed
                                ;    regardless of it's privilege level

  ; definition of SYSTEM decriptor types  (bit0..4 of descript.typbyte )
  ; (bit 4 of descript.typbyte =0)
  TSS286_avail_d   =  01h
  LDT_d            =  02h
  TSS286_busy_d    =  03h
  call_gate_d      =  04h
  task_gate_d      =  05h
  INT286_gate_d    =  06h
  TRAP286_gate_d   =  07h
  TSS386_avail_d   =  09h
  TSS386_busy_d    =  0bh
  call386_gate_d   =  0ch
  INT386_gate_d    =  0eh
  TRAP386_gate_d   =  0fh

  ; definition of privilege levels (bit5..6 of descript.typbyte )
  p0               =  0      ;¿
  p1               =  1*32   ;Ã super visor levels
  p2               =  2*32   ;Ù
  puser            =  3*32


  ;definition of granularity ( bits7..8 in descript.limit16 )
  gran_byte        =  0
  gran_page        =  10000000b  ; 4k granularity
  ; for data_selectors:
  data_USE16       =  0
  gran_big         =  01000000b  ; big segment
  data_USE32       =  gran_big   ; use 32-bit stack pointer ESP instead of SP
  ;                Intel says: relevant only together with expand_down  for data_d
  ;                But that is WRONG :
  ;                BIG segment must be set also if the descriptor is greater 64K
  ;                and is used to load SS !
  ;                (because SP cannot access behind 64K)
  ;
  ; for code_selectors:
  code_USE32       =  01000000b  ; default operand size 32 bit (for code segments)
  code_USE16       =  00000000b  ; default operand size 16 bit (for code segments)


  ; segment present bit (bit7 of descript.typbyte )
  is_present       =128
  not_present      =0

  descriptor macro name,typ,plevel,present,limit,gran,base
  name descript <limit and 0ffffh,base and 0ffffh,low (base shr 16),typ or plevel or present,(limit shr 16) or gran,high (base shr 16)>
            endm


  ;GDT Global Descriptor Table -------------------------v
             align_  code_org0,16
  gdtnull    descript  <?>    ;0000   never accessable
  gdtvcpi_code descript <?>   ;0008
  gdtvcpi2   descript  <?>    ;0010
  gdtvcpi3   descript  <?>    ;0018
  descriptor gdt_core,(data_d+writable),p0,is_present,0fffffh,(gran_page+data_USE32),0
  descriptor gdt_code,(code_d+readable),p0,is_present,0ffffh,(gran_byte+code_USE16),High_Addr_
  descriptor gdt_data,(data_d+writable),p0,is_present,0ffffh,(gran_byte+data_USE16),High_Addr_
  descriptor gdt_ldt,LDT_d,p0,is_present,7,gran_byte,(High_Addr_+(ldtnull-code_org0))
  descriptor gdt_tss,TSS286_avail_d,p0,is_present,0ffh,gran_byte,(High_Addr_+(our_tss-code_org0))
  gdtlast    descript  <?>    ; dummy for addressing

  g_vcpi_code equ      (gdtvcpi_code-gdtnull)
  g_core     equ       (gdt_core-gdtnull)
  g_code     equ       (gdt_code-gdtnull)
  g_data     equ       (gdt_data-gdtnull)
  g_ldt      equ       (gdt_ldt-gdtnull)
  g_tss      equ       (gdt_tss-gdtnull)
  ;GDT Global Descriptor Table -------------------------^


  ;LDT Local  Descriptor Table -------------------------v
  ldtnull    descript  <?>    ;0000   never accessable
  ldtlast    descript  <?>    ; dummy for addressing
  ;LDT Local  Descriptor Table -------------------------^

  ;                  align_  SYSTEMDATA_,1024
  ;IDT Interrupt Descriptor Table -------------------------v
  idtnull    descript  32 dup (<0>)
  idtlast    descript  <?>    ; dummy for addressing
  ;IDT Interrupt Descriptor Table -------------------------^

  our_tss    dd        128 dup (?)    ; our TSS Task State Segment


  ;params for switching TO protected mode -------------------------v
                          ;NOTE: this Data MUST be in LOW_MEM (below 1 Mbyte),
                          ;      data referenced by this structure
                          ;      CAN be in memory above 1 Mbyte
                          ;      On switching to protected mode the server
                          ;      first loads CR3 (paging base) from "our_CR3".
                          ;
                        ; value of CR3 to be loaded by server
    our_CR3    dd       (High_Addr_+(pagedir-code_org0))
                        ; linear address in first Mbyte pointing to
                        ; value of GDTR ("our_GDTR") to be loaded by server
    our_GDTRptr dd      (High_Addr_+(our_GDTR-code_org0))
                        ; linear address in first Mbyte pointing to
                        ; value of IDTR ("our_IDTR") to be loaded by server
    our_IDTRptr dd      (High_Addr_+(our_IDTR-code_org0))
                        ; value of LDTR to be loaded by server
    our_LDTR   dw       g_ldt
                        ; value of TR to be loaded by server
    our_TR     dw       g_tss
                        ; Fword, pointer to code to be started by server
    protected_mode_target DD ?
                          DW g_code
  ;  belongs to above
    our_GDTR   dw       (gdtlast-gdtnull)-1    ;limit (byte gran)
                        ;linear (not physical) base address of "gdtnull"
    laddr_GDT  dd       (High_Addr_+(gdtnull-code_org0))
               dw       ?    ; (just for align "laddr_IDT" to Dword)
    our_IDTR   dw       (idtlast-idtnull)-1    ;limit (byte gran)
                        ;linear (not physical) base address of "Idtnull"
    laddr_IDT  dd       (High_Addr_+(idtnull-code_org0))
  ;-------------------------------------------------------------------^

  server_vcpi_entry df  0 ; this is the address we must call instead of INT67
                          ; when in protected mode
  pagedir_template dd     (High_Addr_+(page0-code_org0)+3)



;=============================================================================


DOS_WRITE_STRING        = 009h ; Display a '$' terminated string
DOS_BUFFERED_INPUT      = 00Ah ; Read text and store it in a buffer
DOS_OPEN_FILE           = 03Dh ; Open an existing file
DOS_CREATE_FILE         = 03Ch ; create a new file
DOS_CLOSE_FILE          = 03Eh ; Close a file
DOS_READ_FROM_HANDLE    = 03Fh ; Read from DOS file handle
DOS_WRITE_TO_HANDLE     = 040h ; write to DOS file handle
DOS_TERMINATE_EXE       = 04Ch ; Terminate program

DosCall macro function_code
    mov ah,function_code
    int 21h
endm
DosInt macro
    int 21h
endm



push_ macro r1,r2,r3,r4,r5,r6,r7,r8,rx
           irp   parm,<&r1,&r2,&r3,&r4,&r5,&r6,&r7,&r8,&rx>
             ifndef parm
               exitm
             endif
             push parm
           endm
         endm
pop_  macro r1,r2,r3,r4,r5,r6,r7,r8
           irp   parm,<&r8,&r7,&r6,&r5,&r4,&r3,&r2,&r1>
             ifdef parm
               pop parm
             endif
           endm
         endm

pushAD_struc macro prefix
           irp   parm,<edi,esi,ebp,esp,ebx,edx,ecx,eax>
             prefix&&parm  dd  ?
           endm
         endm

pushA_struc macro prefix
           irp   parm,<di,si,bp,sp,bx,dx,cx,ax>
             prefix&&parm  dw  ?
           endm
         endm


cpu_86     equ   0
cpu_286    equ   2
cpu_386V86 equ   3       ; is >=386, but in virtual 86 mode
cpu_386GE  equ   4       ; >=386
cpu_386GE_real_paging equ 5
cpu_type   dw    0

cpu_check proc near
        pushf
        cmp     option_force,0
        jnz     is_force_386GE
        xor     ax,ax   ;0000 to ax
        push    ax
        popf            ; try to put that in flags
        pushf
        pop     ax      ; look at what really went into flags
        and     ah,0f0h ; mask off high flag bits
        cmp     ah,0f0h
        je      is_8086
        mov     ax,0f000h
        push    ax      ; try to set the high bits
        popf
        pushf
        pop     ax      ; look at actual flags
        and     ah,0f0h
        je      is_80286
                        ; is x86, x >= 3
                        ; check for V86 or real-paging -mode
        mov     ah,040h ; try to clear IOPL
        push    ax
        popf
        pushf
        pop     ax
        and     ah,030h
        jne     is_v86
        cmp     option_clone,0
        jnz     @@clone
        .386p
        mov     eax,cr0        ; normally this would cause a GP(0)-exception
                               ; (i386 Programmers Reference Guide, INTEL 1987)
                               ; if in V86-mode, but most EMMXXXX drivers
                               ; seem to intercept this exception and allow
                               ; reading the CR0.
        .386
        or      eax,eax
        jz      is_v86         ; not a valid CR0, reserved bits are allways set
                               ; (this may be not true on a 486 clone such as
                               ;  the 486DLC, so if you have trouble with
                               ;  interpreting real mode as V86 use the -clone switch
                               ;
        test    al,01h         ; test PE -bit
        jz      is_greater_equal_80386
is_v86:
        mov     ax,cpu_386V86
cpu_check_exit:
        mov     cpu_type,ax
        popf
        ret
is_8086:
        mov     ax,cpu_86
        jmp     cpu_check_exit
is_80286:
        mov     ax,cpu_286
        jmp     cpu_check_exit
is_greater_equal_80386:
        test    eax,eax         ; test PG - bit
        js      is_386_real_pageing
        mov     ax,cpu_386GE
        jmp     cpu_check_exit
is_force_386GE:
        mov     ax,cpu_386GE
        jmp     cpu_check_exit
is_386_real_pageing:
        mov     ax,cpu_386GE_real_paging
        jmp     cpu_check_exit

@@clone:                 ; on some 486 clones we have problems with CR0,
                         ; so we are looking for EMM, and then
                         ; we assume to be in V86, if we have EMM.
        push    ds
        xor     ax,ax
        mov     ds,ax
        mov     ds,word ptr ds:[emm_int*4+2]
        mov     ax,cpu_386V86
        cmp     dword ptr ds:[10+4],'0XXX'
        jne     @@cl1
        cmp     dword ptr ds:[10],'QMME'
        je      @@clex
        cmp     dword ptr ds:[10],'XMME'
        je      @@clex
@@cl1:
        mov     ax,cpu_386GE
@@clex:
        pop     ds
        jmp     cpu_check_exit

cpu_check endp

;=============================================================================


start:
        .8086    ; we are not sure here if on a 368 CPU
        mov      cs:PSP_frame,es
        mov      ax,cs     ; switch the to our stack
        mov      ss,ax
        lea      sp,stack_top
        mov      ds,ax
        mov      es,ax
        call     clear_uninitialized_data
        mov      ds,cs:PSP_frame
        cld
        lea      si,PSP_DTA+1
        lea      di,comline+1
        mov      cl,PSP_DTA
        xor      ch,ch
        jcxz     start__2
start__:                          ; skip leading blanks
        cmp      byte ptr [si],' '
        jne      start__2
        inc      si
        loop     start__
start__2:
        mov      comline-1,ch
        mov      comline,cl
        inc      cx             ; get the CR too
        rep movsb               ; get the commandline out of psp

        push     cs
        pop      ds
        assume   ds:code
                 ; from now on we have CS=DS=ES=SS

                 ; make sure that size byte is correct
                 ; ( some DOS versions set only CR .. sometimes )
        lea      di,comline+1
        mov      al,13
        call     strlen
        cmp      al,comline
        jnb      start_0
        mov      comline,al
start_0:
        mov      bx,word ptr comline-1
        xchg     bh,bl
        mov      comline[bx+1],0  ; replace CR by ZERO

        mov      logo_out,0
        mov      print_dots,0

        mov      kernel_start,kernel_start_ ; real kernel_start
        mov      free_mem_start,cs  ;save CS as later freemem

                 ; now check if (on error) we can exit do DOS
        lea      bx,comspec_tx
        call     get_env_variable     ; we expect COMSPEC= in the environement
        mov      al,byte ptr es:[di]
        mov      can_exit_to_dos,al
                 ; check if we are running under windows
        lea      bx,windows_tx
        call     get_env_variable     ; we expect WINDIR= in the environement
        mov      al,byte ptr es:[di]
        mov      cannot_load_because_of_windows,al
        push     ds
        pop      es


        lea      ax,modul_end+15
        shr      ax,4
        mov      bx,ax
        add      ax,High_Seg
        mov      es,PSP_frame
IF 1
        sub      ax,es:PSP_memend_frame
ELSE ;TEST (to simulate an occupied 90000 segment )
;        sub      ax,09500h
        sub      ax,05500h
ENDIF
        jb       start_3
                 ; we have the 9000 page occupied by some program
                 ; and we try to move down the setup code below that
                 ; if we later detect an older setup-version,
                 ; we must give up, jumping to "err_uppermem".
        neg      ax
        add      High_Seg,ax
        and      High_Seg,0ff00h  ; allign on next lower page boundary
                                  ; (need this for the pagetables)
        call     relocate_setup_code
start_3:

        push     ds
        pop      es                     ; restore es

        mov      ax,free_mem_start
        cmp      ax,kernel_start
        jnb      start_9
        mov      ax,kernel_start
start_9:
        mov      kernel_load_frame,ax

        mov      token_count,-1
                 ; check if we are on the right CPU
        mov      cpu_check_status,0
        mov      option_clone,1  ; avoid reading CR0 before parsing -clone
        call     cpu_check
        lea      dx,err_wrong_cpu_tx
        cmp      ax,cpu_386V86
        jb       err_print    ; has no 386 or greater at all

        .386    ; now we are sure beeing on a 386(and greater) CPU
        cmp      can_exit_to_dos,0
        jnz      m2
        lea      di,comline+1   ; as DOS (stupidly) converts all from
                                ; CONFIG.SYS to UPPERCASE, we do TOLOWER
        call     tolower
m2:
        call     parscommandline
        call     cpu_check    ; do it once more, because of option_clone

        mov      cpu_check_status,2
        call     get_default_bios_intvectors
        cmp      ax,cpu_386GE
        jb       m2_1
        test     intv_size,0fffch
        jnz      start_continue    ; has $BIOSINTV or REALBIOS.INT
        cmp      option_t_forced,0
        jnz      start_continue
        mov      have_to_intercept_setup,1
        jmp      start_continue
m2_1:
        mov      cpu_check_status,4
                              ; have 386, but are in V86-mode
        call     check_VCPI_present
        jz       err_wrong_cpu   ; has no VCPI-server
        mov      have_vcpi,1
        mov      cpu_check_status,6
        call     check_low_mem_mapping
        jz       err_wrong_cpu   ; has no identical phys/log mapping
        mov      have_to_force_realmode,1
        test     intv_size,0fffch
        jnz      plain_switch    ; has $BIOSINTV or REALBIOS.INT
                                 ; has no $BIOSINTV driver loaded
                                 ; but will try to start Linux anyway
        cmp      option_t_forced,0
        jnz      plain_switch
        mov      word ptr cs:intv_buf+(4*15h+2),0
        mov      have_to_intercept_setup,1
plain_switch:
        mov      cpu_check_status,8


start_continue:
        cmp      token_count,0
        jg       m3
        jz       start_continue_1
        cmp      comline,0   ; have we an emtpy string ?
        jnz      m3
start_continue_1:
        mov      need_mapped_put_buffer,1 ; | we are doing this to
        call     build_buffer_heap        ; | get the avail mem for printing
        lea      dx,empty_tx
        jmp      err_print

                          ; have file-name at minimum
                          ; trying to open it
m3:
        mov      ax,DOS_OPEN_FILE shl 8
        lea      dx,image_name
        DosInt
        jnc      fileopened
m4:
        lea      dx,err_file_notfound_tx
m4_:
        call     print
        lea      dx,enter_commandline_tx
        call     print
        call     readstring
        cmp      comline,0   ; have we an emtpy string ?
        jnz      m2
        lea      dx,abort_tx
        call     print
        jmp      err_exit

fileopened:
        mov      fhandle,ax
IFNDEF DEBUG
        ;---------------------------------------
        mov      ax,High_Seg   ; move us high
        mov      es,ax
        push     cs
        pop      ds
        xor      si,si
        xor      di,di
        lea      cx,modul_end+3
        shr      cx,2
        rep movsd
        push     es
        pop      ds
                 ; we have move ourself up
                 ; must now change cs
        push     ds
        lea      ax,back_from_low
        push     ax
        retf
back_from_low:
        mov      ax,ds     ; switch the stack to top
        mov      ss,ax
        lea      sp,stack_top
ENDIF
                 ; from now on we have CS=DS=ES=SS=setup_memory
        ;-------------------------------------------
        cmp      have_to_force_realmode,0
        je       back_from_low_continue
                 ; we have to do this once more
                 ; because some VCPI-servers rely on an unmovable page0
        call     get_VCPI_interface

back_from_low_continue:
                          ; first look if it is really an image
        mov      bx,fhandle
        mov      ecx,512
        mov      di,cs
        movzx    edi,di
        shl      edi,4
        call     read_handle   ; read the bootsector
        jnc      have_bootsect
fileopened_wrong:
        DosCall  DOS_CLOSE_FILE
        lea      dx,err_wrong_file_tx
        jmp      m4_
err_wrong_setup:
        DosCall  DOS_CLOSE_FILE
        lea      dx,err_wrong_setup_tx
        jmp      m4_
err_setup_too_long:
        DosCall  DOS_CLOSE_FILE
        lea      dx,err_setup_too_long_tx
        jmp      m4_

have_bootsect:
        cmp      ax,cx
        jne      fileopened_wrong
        cmp      bootmagic,0AA55h
        jne      fileopened_wrong
                            ; ok, now get the setup part
        mov      di,cs
        lea      bx,setup_prog
        shr      bx,4
        add      di,bx
        mov      bx,fhandle
        xor      ecx,ecx
        mov      ch,setup_sects
        shl      cx,1
        jnz      new_bootsect
        mov      ch,2*standard_setup_sects
new_bootsect:
        cmp      cx,maximum_setup_sects*512 ; XXX: this check is not enough, now that setup has a BSS since it's C...
        ja       err_setup_too_long
        mov      new_setup_size,cx
        movzx    edi,di
        shl      edi,4
        call     read_handle   ; read setup
        jc       fileopened_wrong
        cmp      ax,cx
        jne      fileopened_wrong
        call     get_setup_version
	call     handle_kernel_size  ; kernel >2.4.5 we may have bzImages > 1Meg
        cmp      setup_version,0201h ; do we have to set setup heap ?
        jb       new_bootsect_3      ; no
        cmp      option_noheap,0     ; yes, but is it disabled ?
        jnz      new_bootsect_3      ; yes
        or       loadflags,CAN_USE_HEAP ;no
        mov      heap_end_ptr,(end_of_setup_buffer-setup_prog)
new_bootsect_3:

                 ; if we have setup-code not at 9000
                 ; we have to check for  setup-version >= 2.0
        cmp      High_seg,High_seg_
        je       have_setup_3  ; no need for version 2.0
        cmp      word ptr setup_version,0200h
        jb       err_uppermem ; we can't continue
                 ; ok, we set the correct move size
        lea      ax,modul_end
        mov      setup_move_size,ax
have_setup_3:
        mov      ax,High_Seg
        sub      ax,kernel_load_frame
        movzx    eax,ax
        shl      eax,4
        mov      load_buffer_size,eax

                 ; if we have setup > 2.0, we set our loader version
        cmp      setup_version,0200h
        jb       have_setup_4

;@@@@@@@@@@@@@@@@@@@@@
                 ; well here the new tricky loadling stuff is prepared

        mov      type_of_loader,10h  ; our loader type + version
                         ; we tell setup not to start the kernel,
                         ; but our special 32-bit page-adjust-routine
        mov      ax,High_Seg
        movzx    eax,ax
        shl      eax,4
        add      eax,(startup_32-code_org0)
        mov      cs:code32_start,eax
                         ; we tell 'read_handle' to use mapped move
        mov      cs:need_mapped_put_buffer,1
                         ; we build the two heaps, low and high
                         ; for this we need to align the low to page boundary
                         ; align the kernel_load_frame to page boundary
        mov      ax,kernel_load_frame
        add      ax,0ffh
        mov      al,0
        mov      kernel_load_frame,ax
        mov      ax,High_Seg
        sub      ax,kernel_load_frame
        movzx    eax,ax
        shl      eax,4
        mov      load_buffer_size,eax
        call     build_buffer_heap

        mov      edi,01000h   ;the address the image must go
        mov      cs:real_32_startup,edi ; were the kernel gets started
                         ; we now determin what kind of zImage we have
        test     cs:loadflags,LOADED_HIGH
        jz       must_start_low
        mov      have_big_kernel,1
        mov      edi,0100000h   ;the address the image must go
        mov      cs:real_32_startup,edi ; were the kernel gets started
must_start_low:
                         ; even if the image starts low,
                         ; we use the 'high load' routines,
                         ; so we force this bit in loadflags
        or       cs:loadflags,LOADED_HIGH
                         ; NOTE: needing EDI from above for open_new_mapped_block
        call     open_new_mapped_block      ; open the first block
;@@@@@@@@@@@@@@@@@@@@@

have_setup_4:
        cmp      have_to_intercept_setup,0
        jz       dont_patch
        cmp      dword ptr ds:setup_header_sign,SIGNATURE
        jnz      not_signed
        mov      have_to_intercept_setup,2
        mov      word ptr ds:setup_realmode_switch,offset real_switch
        mov      ax,High_Seg
        mov      word ptr ds:setup_realmode_switch+2,ax
        mov      ax,kernel_load_frame
        mov      word ptr ds:start_sys_seg,ax
        jmp      dont_patch
not_signed:
        xor      di,di
        mov      cx,new_setup_size
        mov      al,0FAh    ; cli
        cld
keep_searching:
        repnz scasb
        jcxz     err_wrong_setup
        cmp      dword ptr [di],70E680B0h ; mov al,80h
                                          ; out 70h,al
        jnz      keep_searching
        mov      byte ptr [di-1],0CDh   ; int op code
        mov      byte ptr [di],setup_intercept_int  ; int number
        mov      word ptr [di+1],4444h ; inc sp       adjust stack
                                       ; inc sp       (discard flags)
        mov      byte ptr [di+3],90h   ; nop
dont_patch:
                          ; we convert the kernel version string
                          ; to a binary number
        xor      eax,eax
        cmp      have_to_intercept_setup,2
        jne      dont_patch_
        call     get_kernel_version
dont_patch_:
        mov      kernelversion,eax
        call     handle_kernel_specifics
                          ; ok, now check the size of the kernel
;        movzx    eax,kernel_size
;        shl      eax,4
	mov	eax,kernel_size
        cmp      eax,load_buffer_size
        jb       have_space
        lea      dx,err_kernel_to_big_tx
        cmp      option_t,0
        je       err_print

have_space:

                           ; now we update the params
        cmp      command_line,0
        jz       no_comline
        mov      CL_MAGIC,0A33Fh
        lea      ax,command_line
        mov      CL_OFFSET,ax
        mov      si,cl_pointer
        mov      byte ptr [si-1],0 ;delete the last blank


no_comline:
                           ; check for ramdisk
        cmp      got_ram_disk,0
        jz       no_change_on_ramdisk
        mov      ax,new_ram_disk
        mov      ram_disk,ax
no_change_on_ramdisk:
                           ; check for vga
        cmp      got_vga_mode,0
        jz       no_change_on_vga
        mov      ax,new_vga_mode
        mov      vga_mode,ax

no_change_on_vga:

        cmp      cannot_load_because_of_windows,0
        jz       no_windows
        call     force_error_verbose
no_windows:

                           ; check for -v option
        cmp      option_v,0
        jz       no_option_v
                           ; option -v (verbose) is set
                           ; print some information
        call     print_verbose

no_option_v:
                           ; check for -t option
        cmp      option_t,0
        jz       no_option_t
        DosCall  DOS_CLOSE_FILE
        lea      dx,option_t_terminate_tx
        cmp      option_t_forced,0
        jz       no_option_t_forced
        lea      dx,option_t_forced_tx
no_option_t_forced:
        call     print
        jmp      err_exit

no_option_t:
                           ; start of critical section
                           ; =========================
        call     close_debug_file

                            ; now loading the kernel
        mov      bx,fhandle
;        movzx    ecx,kernel_size
;        shl      ecx,4
	mov	ecx,kernel_size
        mov      di,kernel_load_frame
        movzx    edi,di
        shl      edi,4
        mov      print_dots,2
        call     read_handle   ; read the kernel
        call     print_crlf
        mov      print_dots,0
        call     print_crlf
        jc       err_io
	cmp      setup_version,0202h
	jae      no_need_to_roundup
        add      eax,15
        and      al,0f0h
no_need_to_roundup:
        cmp      eax,ecx
        jnz      fileopened_wrong
                           ; ok, all is read into memory
        DosCall  DOS_CLOSE_FILE

        cmp      ram_disk,0 ; have we a ramdisk
        jz       no_ram_disk
        cmp      option_nodiskprompt,0;
        jnz      no_ram_disk
                           ;we must prompt for insertion of floppy
        lea      dx,insert_floppy_tx
        call     print
        call     readstring    ; just to wait for prompt
no_ram_disk:
                 ; now we try to load the initrd ramdisk-image
        call     load_initrd
                 ; now we clean up all entries in the page adjust list
        call     final_page_adjust_list_handling
                 ; that's it

                 ; here we handle -wait and -dskreset
                 ; These option are to avoid outstanding disk-IRQs
                 ; happen, when Linux tries to detect the hardware.
                 ; In most cases we will not need this.
        call     wait_and_reset_dsk

        cli
___go:
        cmp      have_to_intercept_setup,1
        ja       ___go_switch
        je       ___go_skip_move
        call     move_kernel_down
___go_skip_move:
        cmp      have_to_intercept_setup,1
        jne      ___go_switch
        xor      ax,ax
        mov      ds,ax
        mov      word ptr ds:[4*setup_intercept_int],offset real_switch
        mov      word ptr ds:[4*setup_intercept_int+2],cs
___go_switch:
        mov      ax,cs:High_Seg
        mov      ds,ax
        mov      es,ax

        cmp      have_to_intercept_setup,0
        ja       ___go_continue
        call     switch_to_protected_mode_and_return_in_386realmode

___go_continue:
        call     restore_bios_default_vectors
        lea      bx,setup_prog
        shr      bx,4
        mov      ax,High_Seg
        add      ax,bx
        lea      sp,setup_stack_top  ; separate stack during
                                     ; setup and real_switch
        push     ax
        push     0

        retf     ; and now it's the job of setup
                 ; but NOTE:
                 ;   on have_to_force_realmode >0 setup calls real_switch

setup_own_stack df 0


real_switch proc far
        ;  NOTE:  we have to preserve ALL registers !
        ;         to avoid conflicts with future kernels
        cli
        push_    ds,es,fs,gs
        pushad
        mov      al,80h
        out      [70h],al
        mov      ax,cs
        mov      ds,ax
        mov      es,ax
        mov      word ptr setup_own_stack+4,ss
        mov      dword ptr setup_own_stack,esp
                        ; Ok,ok, today setup has our stack (no need to switch)
        mov      ss,ax  ; ... but we want to be sure for the future,
        lea      sp,stack_top ; so we switch to our stack
        cmp      have_to_intercept_setup,2 ; have we to move the kernel down ?
        je       @@3  ; no, setup will do it (knows the location of the image)
                      ; yes, we have patched in the intercept code,
                      ; so the kernel expects the kernel at 10000h
        call     move_kernel_down
@@3:
        call     switch_to_protected_mode_and_return_in_386realmode
        lss      esp,cs:setup_own_stack ; restore setup's stack
        popad
        pop_     ds,es,fs,gs
        retf
real_switch endp

;=============================================================================


move_kernel_down proc near
                           ; is the kernel at its right place ?
        cli
        mov      ax,kernel_start
        cmp      kernel_load_frame,ax
        je       short @@ex  ; yes
                           ; no, must move it down
        push_    ds,es
        cld
mgran = 08000h
        mov      bp,mgran shr 4
        mov      ax,kernel_size16
        mov      bx,kernel_start
        mov      dx,kernel_load_frame
@@loop:
        mov      cx,mgran shr 2
        mov      ds,dx
        mov      es,bx
        xor      si,si
        xor      di,di
        rep movsd
        add      bx,bp
        add      dx,bp
        sub      ax,bp
        cmp      ax,bp
        ja       @@loop
        mov      cx,ax
        shl      cx,2
        mov      ds,dx
        mov      es,bx
        xor      si,si
        xor      di,di
        rep movsd

        pop_     ds,es
        mov      ax,kernel_start
        mov      kernel_load_frame,ax
@@ex:
        ret
move_kernel_down endp


err_wrong_cpu:
        lea      dx,err_cpu_v86_tx
err_print:
        cmp      token_count,0
        jz       err_print_1
        jg       err_print_2
        cmp      word ptr comline-1,0   ; have we an command line
        jnz      err_print_2 ; yes
                             ; no, print help and status
err_print_1:
        push     dx
        lea      dx,usage_tx
        call     print
        pop      dx
err_print_2:
        call     print
        cmp      cpu_type,cpu_386V86
        jb       err_exit
        call     print_verbose_stat
        jmp      err_exit
err_io:
        lea      dx,err_io_tx
        call     print
        jmp      err_exit

err_uppermem:
        lea      dx,err_uppermem_tx
        call     print
err_exit:
        cmp      cs:can_exit_to_dos,0
        jz       idle
        mov      al,1
exit_to_dos:
        call     free_extended_memory
        call     close_debug_file
        DosCall  DOS_TERMINATE_EXE
idle:
        lea      dx,err_in_config_sys_tx
        call     print
        call     close_debug_file
idle_:
        sti
        jmp      idle_

real_print   proc    near
; input: DX = offset of string within CODE
        .8086
        push    ds
        push    cs
        pop     ds
        push_   bx,cx,si
        mov     si,dx
        cld
        xor     ax,ax
@@loop:
        lodsb
        test    ax,ax
        jz      @@ok
        cmp     al,'$'
        jnz     @@loop
@@ok:
        dec     si
        sub     si,dx
        jz      @@null
        mov     cx,si
        mov     bx,1
        DosCall DOS_WRITE_TO_HANDLE
        cmp     debug_file_handle,0
        jz      @@ex
        mov     bx,debug_file_handle
        DosCall DOS_WRITE_TO_HANDLE
@@null:
@@ex:
        pop_    bx,cx,si
        pop     ds
        ret
        .386
real_print   endp

print proc near
        cmp      cs:logo_out,0
        jnz      @@ex
        mov      cs:logo_out,1
        push_    ax,dx
        lea      dx,logo_tx
        call     real_print
        pop_      ax,dx
@@ex:
        call     real_print
        ret
print endp


print_crlf proc near
        pushf
        push    ax
        push    dx
        lea     dx,@@crlf
        call    print
        pop     dx
        pop     ax
        popf
        ret
@@crlf  db      13,10,'$'
print_crlf endp

print_dot proc  near
        pushf
        cmp     cs:print_dots,1
        jb      @@ex
        push    ax
        push    dx
        lea     dx,@@dot_tx
        je      @@1
        lea     dx,@@start_tx
        dec     cs:print_dots
        cmp     cs:print_dots,1
        jbe     @@1
        lea     dx,@@start_tx_2
        dec     cs:print_dots
@@1:
        call    print
        pop     dx
        pop     ax
@@ex:
        popf
        ret
@@start_tx_2 db  13,10,'Now reading INITRD:'
@@start_tx db    13,10,'LOADING'
@@dot_tx  db    '.$'
print_dot endp


granularity = 01000h

handle_kernel_size proc near
; this tries to find out the actual kernel size
; because kernels > 2.4.5 can have bigger bzImages then 1Meg and
; kernel_size16 simply will wrap around without notification
	push	bx
	push	eax
		; first check if the kernel follows the old standard
	cmp	setup_version,0202h
	jae	@@bigger
	movzx	eax,kernel_size16
	shl	eax,4
	jmp	short @@ex

@@bigger:
		; first get the filesize
	mov	bx,fhandle
	call	get_filesize
		; now subtract bootsect and setup.S size
	sub	eax,dword ptr new_setup_size	; minus setup.S
	sub	eax,200h			; minus bootsector
@@ex:
	mov	kernel_size,eax
	pop	eax
	pop	bx
	ret
handle_kernel_size endp

read_handle proc    near
; input:
;   BX=  handle
;  ECX=  count
;  EDI=  linear destination address
; output:
;   CARRY =1 , then read-error
;  EAX= number of bytes transferred (even on CARRY=1)
;
        push    ds
        push    esi
        push    dx
        push    ecx
        push    cs
        pop     ds                 ; target seg = IO_buffer
        lea     dx,aligned_auxbuff ; target off = IO_buffer
        mov     esi,ecx
        mov     ecx,granularity
        jmp     @@start
@@next:
        DosCall DOS_READ_FROM_HANDLE
        call    print_dot
        jc      @@err
        call    put_buffer
        cmp     ax,cx
        jne     @@eof
        sub     esi,ecx
        add     edi,granularity
@@start:
        cmp     esi,ecx
        ja      @@next
        mov     cx,si
        DosCall DOS_READ_FROM_HANDLE
        call    print_dot
        jc      @@err
        call    put_buffer
@@eof:
        movzx   eax,ax
        sub     esi,eax
        pop     ecx
        mov     eax,ecx
        sub     eax,esi
        clc
@@ex:
        pop     dx
        pop     esi
        pop     ds
        ret
@@err:
        pop     ecx
        mov     eax,ecx
        sub     eax,esi
        stc
        jmp     @@ex
read_handle endp


clear_to_default proc near
        push_    ax,di,es
        call     close_debug_file
        push     cs
        pop      es
        xor      ax,ax
        mov      cx,parse_switches_end-parse_switches
        lea      di,parse_switches
        cld
        rep stosb
        pop_     ax,di,es
        ret
clear_to_default endp


;=============================================================================

INCLUDE LOADLINI.ASM
INCLUDE LOADLINJ.ASM
INCLUDE LOADLINM.ASM

;=============================================================================

; -------------------------------------------------------------
clear_uninitialized_data proc near
        .8086   ; we need to have clean 8086 code,
                ; because we don't yet know on which machine we are
        push_   es,ax,cx,di
        xor     ax,ax
        lea     di,uninitialized_data_start
        mov     cx,(uninitialized_data_stop-uninitialized_data_start)/2
        cld
        rep stosw
        call    preset_pagedir_from_template
        les     ax,@@aux
        mov     word ptr pageadjlist.auxbuf,ax
        mov     word ptr pageadjlist.auxbuf+2,es
        pop_    es,ax,cx,di
        ret
        .386
@@aux    dd     ((High_Seg_*16) + (aligned_auxbuff-code_org0))
clear_uninitialized_data endp

preset_pagedir_from_template proc near
        mov     ax,word ptr pagedir_template
        mov     word ptr pagedir,ax
        mov     ax,word ptr pagedir_template+2
        mov     word ptr pagedir+2,ax
        ret
preset_pagedir_from_template endp

;------------------------------------------------------------------------
; Here we expect the realy end of any _preset_ data in the .EXE.
; Below we have _uninitialized_ data, that will not appear in the binary.
; We put the LOADLIN >= 1.6 Magic and suffix-structure here:
          align  dword
          dd     0     ; relative offset within file to previous suffix
                       ; (e.g the size of the appended part)
          dd     0     ; flags, indicating what kind of appended
                       ; file we have
          db     0     ; suffix level, 0= no further suffix
          db     'Loadlin-',LOADLIN_VERSION ; exactly 11 bytes !!!
;------------------------------------------------------------------------


;          align dword       ; NOTE: this is align from above !
                             ; if we realign it here, we may get the
                             ; suffix above corrupted, because it then isn't
                             ; the last part of the executable (.EXE)
          db           space1k dup(?)
setup_stack_top label byte
          db           space1k dup(?)
stack_top label byte


uninitialized_data_start label byte

                db    ?   ; belongs to comline
comline         db    128 dup(?) ; copied from PSP_DTA
                db    2*1024-128 dup(?) ; extended commandline
comline_end     label byte
image_name      db    80 dup(?)
aux_token       db    80 dup(?)
rdimage_name    db    80 dup(?)
;command_line    db    space2k dup(?)  ; kernel accepts maximum of 2Kb
command_line    db    360h dup(?)  ; limited to 1Kb to get bigger heap


  ; -------------------------------v
                             ; this is for 32-bit code
pageadjlist     pages_list   <?>
  ; -------------------------------^

pagelist        dd    1024 dup (?)    ; temporary buffer for page adjust list,
                                      ; to be pushed to high memory.

  ; -------------------------------v
  ; this buffer is for the "default bios interruptvectors"
  ; as is set from the BIOS,
  ; Its contents are delivered by the BIOSINTV.SYS device driver
  ; (must be AT TOP of config.sys).
  ; Or from the REALBIOS.INT file.
  ; We need this after returning from V86-mode
  ; because NOTHING is valid any more but the ROM-BIOS
  ; (and LOADLIN of cause)
  ;
intv_buf        dd     128 dup(?)   ;  intvector  0:0
                dd     128 dup(?)
bios_data       db     256 dup(?)   ;  BIOS-data 40:0
dummy_dos_data  db     (256-4-16-2) dup(?) ; (DOS-data  50:0, not valid at boot tome
real_bios_int15 dw     ?            ; result of int15 at time of realbios
real_bios_magic dw     ?            ; must be 0a5a5h for post alpha-release
reset_jmpop     db     ?            ;  TOP BIOS FFFF:0
reset_entry     dd     ?
biosdate        db     9 dup(?)
machineid       db     ?
                db     ?
masterIMR       db     ?            ;  port 21
slaveIMR        db     ?            ;  port A1
bios_scratch    db     1024 dup(?)  ;  scratch  9FC0:0
realbios_end    label  byte
  ; -------------------------------^

  ; -------------------------------v
                align_ code_org0,4096
aligned_auxbuff dd     1024 dup(?)  ; must be aligned to 4 K
page0           dd     1024 dup(?)  ; must be aligned to 4 K
  ; -------------------------------^


uninitialized_data_stop label byte
; -------------------------------------------------------------


modul_end:

code    ends

        end      start
