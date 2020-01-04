;   >>> this is file LOADLINj.ASM
;============================================================================
;   LOADLIN v1.6 (C) 1994..1996 Hans Lermen (lermen@elserv.ffm.fgan.de)
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

check_VCPI_present proc near
          pushad
          push    es
          push    ds
          lea     esp,[esp-4] ; local space
@@stack struc
  @@handle dw   ?
           dw   ?
  @@ds     dw   ?
  @@es     dw   ?
  pushad_struc @@
@@stack ends
          xor     eax,eax      ; check EMM_interrupt present
          mov     [esp].@@eax,eax
          mov     es,ax
          mov     es,word ptr es:[emm_int*4+2]
          cmp     Dword ptr es:[10+4],'0XXX'
          jne     @@ex
          cmp     dword ptr es:[10],'XMME'
          je      @@ok
          cmp     dword ptr es:[10],'QMME' ; this also works (as told by <J.S.Peatfield@damtp.cambridge.ac.uk>)
          jne     @@ex
@@ok:
          mov     ax,4000h   ; check emm menager status
          int     emm_int
          or      ah,ah
          jnz     @@ex
          mov     ax,_GET_VERSION*256   ; get version
          int     emm_int
          cmp     ax,40h     ; version must be greater 4.0
          jb      @@ex

          mov     ax,_ALLOCATE_RAW_PAGES
          mov     bx,4
          int     emm_int
          cmp     ah,0
          jnz     @@ex
          mov     word ptr [esp].@@handle,dx     ; save handle
          mov     ax,0DE00h   ; check for VCPI present
          int     emm_int
          cmp     ah,0
          jne     short @@notok
          mov     byte ptr [esp].@@eax,1
@@notok:
          mov     dx,word ptr [esp].@@handle
          mov     ax,_DEALLOCATE_PAGES*256
          int     emm_int
@@ex:
          lea     esp,[esp+4] ; discar local space
          pop     ds
          pop     es
          popad
          or      ax,ax
          ret
check_VCPI_present endp

get_VCPI_interface proc near
          push    cs                  ;* buffer for server GDT entries
          push    offset gdtvcpi_code ;*
          push    cs                  ;* buffer for server page0
          push    offset page0        ;*
          call    get_protected_mode_interface
          ret
get_VCPI_interface endp


check_low_mem_mapping proc near
          call    get_VCPI_interface
          xor     edx,edx
          lea     si,page0
          cld
          mov     cx,(640/4)
@@loop:
          lodsd
          and     ax,0f000h
          cmp     eax,edx
          jne     @@ex_false
          add     edx,1000h
          loop    @@loop
          mov     ax,1
@@ex:
          or      ax,ax
          ret
@@ex_false:
          xor     ax,ax
          jmp     @@ex
check_low_mem_mapping endp


get_protected_mode_interface proc near
          ARG     server_pagebuffer:dword,server_selectors:dword = returnpop
          push    bp
          mov     bp,sp
          push    si
          push    di
          push    ds
          push    es

                                    ; convert pagebuffer to linear address
          movzx   eax,word ptr server_pagebuffer+2
          movzx   ebx,word ptr server_pagebuffer
          shl     eax,4
          add     eax,ebx
          shr     eax,4      ; make real mode address

          ;XXXXXXXXXX
          push    ds
          mov     es,ax                ; ES = frame of server_pagebuf
          xor     di,di                ; ES:DI = >buffer for page0-table<
          lds     si,server_selectors ; DS:DI = >3 GDT entries for VCPI
                               ; calling VCPI server
          mov     ax,0DE01h
          int     emm_int
          pop     ds

                                      ; save VCPI server entry for protected mode
          mov     dword ptr server_vcpi_entry,ebx
                                      ; set the VCPI-Code selector
          mov     word ptr server_vcpi_entry+4,g_vcpi_code
          ;XXXXXXXXXXX



          pop     es
          pop     ds
          pop     di
          pop     si
          pop     bp
          ret     returnpop
get_protected_mode_interface endp


try_biosintv proc near
          pushad
          mov      ax,DOS_OPEN_FILE shl 8
          lea      dx,@@fname
          DosInt
          jc       @@ex_false
          mov      bx,ax
          mov      ax,4402h
          mov      cx,size intv_buf
          lea      dx,intv_buf
          DosInt
          pushf
          push     ax
          DosCall  DOS_CLOSE_FILE
          pop      ax
          popf
          jc       @@ex_false
          mov      intv_size,ax
          mov      ax,1
@@ex:
          or       ax,ax
          popad
          ret
@@ex_false:
          xor     ax,ax
          jmp     @@ex
@@fname   db     '$BIOSINT',0
try_biosintv endp

try_realbios proc near
          pushad
          mov      ax,DOS_OPEN_FILE shl 8
          lea      dx,@@fname_realbios
          DosInt
          jc       @@ex_false
          mov      bx,ax
          lea      dx,intv_buf
          mov      cx,realbios_end-intv_buf
          DosCall  DOS_READ_FROM_HANDLE
          pushf
          push     ax
          DosCall  DOS_CLOSE_FILE
          pop      ax
          popf
          jc       @@ex_false
          cmp      ax,realbios_end-intv_buf
          jne      @@ex_false
          mov      intv_size,ax
          cmp      cs:option_rx,1
          je       @@ex_true
                   ; we now check the BIOS-revision to be sure
                   ; that we have the right file
          push     es
          mov      ax,0ffffh
          mov      es,ax
          mov      eax,dword ptr es:[biosdate-reset_jmpop]
          cmp      eax,dword ptr cs:biosdate
          jne      @@ex_wrong_bios
          mov      eax,dword ptr es:[(biosdate-reset_jmpop)+4]
          cmp      eax,dword ptr cs:biosdate+4
          jne      @@ex_wrong_bios
                   ; we check the real_bios_magic
          cmp      cs:real_bios_magic,0a5a5h
          jne      @@ex_wrong_bios
@@ex_true:
          mov      ax,1
          pop      es
@@ex:
          or       ax,ax
          popad
          ret
@@ex_false:
          mov     intv_size,0
          xor     ax,ax
          jmp     @@ex
@@ex_wrong_bios:
          pop     es
          mov     wrong_realbios,1
          call    force_error_verbose
          jmp     @@ex_false

@@fname_realbios db REALBIOS_FILE,0
try_realbios endp

get_default_bios_intvectors proc near
          cmp      option_ja,0
          jnz      @@first_javier
          cmp      option_realbios,0
          jnz      @@first_realbios
          call     try_biosintv
          jnz      @@ex
          call     try_realbios
@@ex:
          ret
@@first_realbios:
          call     try_realbios
          jnz      @@ex
          call     try_biosintv
          jmp      @@ex
@@first_javier:
          mov      intv_size,0
          jmp      @@ex
get_default_bios_intvectors endp


pmode_return_esp   dd   ?  ; used to return from real-mode-switch
real_IDTR          dw       03ffh    ;needed to load real intvector after switch
                   dd       0

pmode_task proc near
; We come here in protected mode via VCPI-CALL
; have only to set up all things to switch back to 386-realmode
;
; We follow the INTEL recomended procedure to do this
; (80386 Programmer's Reference Manual, Chapter 14.5)
;
          .386p
          CLI  ; just to be sure

          ; 1. We know, that paging is enabled,
          ;             that linear addresses have identical mapping
          ;    so we can clear the PG bit

          mov     eax,CR0
          and     eax,07fffffffh
          mov     CR0,eax    ; paging is off
          xor     eax,eax
          mov     CR3,eax    ; clear out paging cache

          ; 2. We must transfer control to segment with 64K limit
          ;    we ARE on such a segment, no need to do it here

          ; 3. Load segment registers SS,DS,ES,FS,GS with a selector
          ;    that points to a descriptor containing the the values:
          ;      limit 64k     =  0FFFFh
          ;      byte granularity
          ;      Expand up
          ;      writable
          ;      present
          ;
          ;    We have that descriptor, its our gdt_data.
          mov     ax,g_data
          mov     ss,ax
          mov     ds,ax
          mov     es,ax
          mov     fs,ax
          mov     gs,ax

          ; 4. Disable Interrrupts
          ;    ( we did that above )
          ; 5. Clear the PE bit in CR0
          mov     eax,CR0
          and     al,0feh
          mov     CR0,eax  ; we are go back to realmode

          ; 6. Flush the instruction queue and load CS with approritate value
          ;jmp     far ptr @@here
          db      0EAh
          dw      @@here
reloc_hseg_1  dw  High_Seg_
@@here    label   far

          ; 7. Use LIDT instruction to load the base and limits of the
          ;    real-mode interrupt vector table
          lidt    fword ptr cs:real_IDTR

          .386
          ; 8. Enable interrupts (we are doing this later)
          ; 9. Load segment register
          mov     ax,High_Seg_
reloc_hseg_2  equ word ptr ($-2)
          mov     ss,ax
          mov     ds,ax
          mov     es,ax
          mov     fs,ax
          mov     gs,ax
          mov     esp,pmode_return_esp
          ; now we are in "real" real-mode

          call    restore_bios_default_vectors
@@ex:
          popad
          ret
pmode_task endp

iodelay   macro
            jmp short $+2
            jmp short $+2
          endm

get_mem_from_CMOS proc near
          pushf
          cli
          push    bx
          mov     al,18h
          out     [070h],al
          dec     al
          mov     bl,al
          iodelay
          in      al,[071h]
          mov     ah,al
          mov     al,bl
          iodelay
          out     [070h],al
          iodelay
          in      al,[071h] ; extended mem read from CMOS
          pop     bx
          popf
          ret
get_mem_from_CMOS endp



restore_bios_default_vectors proc near
          push_   es,ds,ax,cx,si,di
          xor     di,di
          mov     es,di
          cmp     cs:have_to_intercept_setup,0
          jz      @@p2
                              ; No REALBIOS or device driver
          cmp     word ptr cs:intv_buf+(4*15h+2),0
          jne     @@ex        ; Only once
          mov     eax,dword ptr es:[4*15h]
          mov     dword ptr cs:intv_buf+(4*15h),eax
          call    get_mem_from_CMOS
          mov     cs:real_bios_int15,ax
          jmp     @@p1
@@p2:
          mov     cx,cs:intv_size
          shr     cx,2
          jcxz    @@ex
          ; we now reset the intvector to bios-defaults
          push    cs
          pop     ds
          cmp     cx,(size intv_buf)/4  ; have we REALBIOS
          jna     @@1     ; no
                          ; yes
                          ; restoring BIOSdata also
          mov     cx,(dummy_dos_data-intv_buf)/4
@@1:
          lea     si,intv_buf
          cld
          rep movsd
          cmp     cs:intv_size,(size intv_buf)  ; have we REALBIOS
          jna     @@8      ; no
                           ; yes, restoring BIOSscratch also
          mov     di,09FC0h
          mov     es,di
          xor     di,di
          lea     si,bios_scratch
          mov     cx,(size bios_scratch)/4
          rep movsd
                           ; now reprogram the PICs (8259A)
                           ; (may be redirected by VCPI-client)
          mov     al,11h        ; enter programming mode
          out     [20h],al      ; master
          iodelay
          out     [0A0h],al     ; slave
          iodelay
          mov     al,8          ; IRQ0..7 --> INT8..F
          out     [21h],al      ; master
          iodelay
          mov     al,70h        ; IRQ8..F --> INT70..77
          out     [0A1h],al     ; slave
          iodelay
          mov     al,4          ; master bit
          out     [21h],al      ; master
          iodelay
          mov     al,2          ; slave bit
          out     [0A1h],al     ; slave
          iodelay
          mov     al,1          ; 8086 mode
          out     [21h],al      ; master
          iodelay
          out     [0A1h],al     ; slave
          iodelay
          mov     al,cs:slaveIMR
          out     [0A1h],al     ; slave
          iodelay
          mov     al,cs:masterIMR
          out     [21h],al      ; master

@@p1:                      ; we hook on the INT15
          xor     ax,ax
          mov     es,ax
          mov     word ptr es:[4*15h],offset our_int15
          mov     word ptr es:[4*15h+2],cs
                           ; now we must reinit the video controler
                           ; to the values just set in the BIOS-data
                           ; we do it by calling the appropriate INT 10h
          mov     ax,3003h ; al = color mode
          and     ah,byte ptr cs:bios_data+10h ; get EQUIPPEMENT-flags
          cmp     ah,30h
          jne     @@6
          mov     al,7     ; al = mono mode
@@6:
          xor     ah,ah
          INT     10h
@@8:
          push    ds
          pop     es
@@ex:
          mov     cs:intv_size,0   ; avoid second call
          pop_    es,ds,ax,cx,si,di
          ret
restore_bios_default_vectors endp


our_int15 proc near
; interrupt 15 hook, in case of realbios
          cmp     ah,88h
          je      @@we_do_it
          jmp     dword ptr cs:intv_buf+(4*15H)
@@we_do_it:
          mov     ax,cs:real_bios_int15
;;;test          mov     ax,0c00h*4
          iret
our_int15 endp



switch_to_protected_mode_and_return_in_386realmode proc near
; NOTE: must have called get_VCPI_interface before this
;
; returns with interrpts disabled
; all registers preserved, all segment registers = "High_Seg"
          cmp    cs:have_to_force_realmode,0
          jnz    @@goon
          ret
@@goon:
          mov    cs:have_to_force_realmode,0
          pushad
          mov    pmode_return_esp,esp
          lea    ax,pmode_task
          movzx  eax,ax
          mov    dword ptr protected_mode_target,eax
          mov    word ptr protected_mode_target+4,g_code
          movzx   esi,High_Seg
          shl     esi,4
          lea     si,our_CR3[si]
          CLI
          mov     ax,0DE0Ch
          int     emm_int    ; jumps to protected mode
          ; does NOT return here
          ; "pmode_task" does return directly to our caller
switch_to_protected_mode_and_return_in_386realmode endp


; input:
;   ES:DI   pointer to string
;   AL       = terminating character
; output:
;   AX=       length of string, not counting terminating character
;   all other registers preserved
strlen proc near
        .8086
        push_  di,cx
        xor     cx,cx
        dec     cx
        cld
        repne   scasb
        mov     ax,di
        dec     ax
        pop_    di,cx
        sub     ax,di
        .386
        ret
strlen endp


get_kernel_version proc near
  ; Input:
  ;   DS=CS
  ; Output:
  ;   EAX= converted verion number as:
  ;        1.2.3 (root:pipapo) #4 ...  --->  EAX= 01020304h
  ;        1.2.3                       --->  EAX= 01020300h
  ;   all other registers preserved
         xor      eax,eax
         cmp      have_to_intercept_setup,2
         jne      @@ex
         cmp      setup_header_version,0105h
         jb       @@ex
         cmp      kernel_version,0
         jz       @@ex
         push_    esi,edx
         mov      si,kernel_version
         lea      si,setup_prog[si]
         call     value_of
         mov      edx,eax
         call     value_of
         ror      eax,8
         shld     edx,eax,8
         call     value_of
         ror      eax,8
         shld     edx,eax,8
         xor      eax,eax
         cmp      byte ptr [si-1],0  ; end of string ?
         je       @@ex_
         cld
@@loop:
         lodsb
         test     ax,ax
         jz       @@ex_
         cmp      al,'#'
         jne      @@loop
         call     value_of
         ror      eax,8
@@ex_:
         shld     edx,eax,8
         mov      eax,edx
         pop_     esi,edx
@@ex:
         ret
get_kernel_version endp

writechar proc near
         push_   eax,edx
         push    eax
         mov     byte ptr [esp+1],'$'
         mov     dx,sp
         call    print
         add     sp,4
         pop_    eax,edx
         ret
writechar endp


writestring proc near
         cld
@@1:
         lodsb
         test    al,al
         jz      @@ex
         call    writechar
         loop    @@1
@@ex:
         ret
writestring endp

writehexdigit proc near
         push  eax
         and   al,0fh
         add   al,'0'
         cmp   al,'9'
         jna   @@m1
         add   al,'A'-('9'+1)
@@m1:
         call  writechar
         pop   eax
         ret
writehexdigit endp

writehexbyte proc  near
         push    eax
         ror     eax,4
         call    writehexdigit
         rol     eax,4
         call    writehexdigit
         pop     eax
         ret
writehexbyte endp


writehexword proc near
         push    eax
         ror     eax,8
         call    writehexbyte
         rol     eax,8
         call    writehexbyte
         pop     eax
         ret
writehexword endp


writehexdword proc near
         push    eax
         ror     eax,16
         call    writehexword
         rol     eax,16
         call    writehexword
         pop     eax
         ret
writehexdword endp


force_error_verbose proc near
        mov      option_v,1
        mov      option_t,1
        mov      option_t_forced,1
        ret
force_error_verbose endp


get_effective_physmem proc near
; input: none
; output: EAX = number of total physical RAM in bytes
;         (may be set by mem= param)
         cmp      end_of_physmem,0
         jz       @@continue
         mov      eax,end_of_physmem
         ret
@@continue:
         mov      ah,88h
         cmp      intv_size,15h*4
         jnb      @@sim_int15
         cmp      have_to_intercept_setup,0
         jz       @@sim_int15_1
         call     get_mem_from_CMOS
         jmp      @@1
         ; we take int 15 as is, result may be wrong
         ;                       but Linux takes this as memory
@@sim_int15_1:
         int      15h
         jmp      @@1
@@sim_int15:
         cmp      wrong_realbios,0
         jne      @@sim_int15_1
         cmp      cs:real_bios_magic,0a5a5h  ; have we a saved value
         jne      @@sim_int15_2              ; no
         mov      ax,cs:real_bios_int15      ; yes
         jmp      @@1
@@sim_int15_2:
         ; we take int 15 routine as saved by BIOSINTV
         ; hopefully this gets better result
         pushf
         cli
         push     cs
         push     offset @@1
         push     dword ptr intv_buf+(15h*4)
         retf
@@1:
         movzx    eax,ax
         shl      eax,10      ; now have extended mem in bytes
         add      eax,100000h ; we add we 1Meg of conventional mem
         ret
get_effective_physmem endp



print_verbose proc near
         pushad
         push_  ds,es

@@print  macro  tx
          lea    dx,tx
          call   print
         endm
@@cr     macro
          call   print_crlf
         endm
@@prints macro  tx,count
          mov    cx,count
          lea    si,tx
          call   writestring
         endm
@@hexw   macro  v
          mov    ax,v
          call   writehexword
         endm

         @@print  @@t1
         @@prints image_name,<size image_name>
         cmp      have_to_intercept_setup,2
         jne      @@m1
         cmp      setup_header_version,0105h
         jb       @@m1
         cmp      kernel_version,0
         jz       @@m1
         @@print  @@t1_
         mov      dx,kernel_version
         add      dx,offset setup_prog
         call     print
@@m1:
         @@print  @@t2
;         @@hexw   kernel_size
	 mov      eax,kernel_size
	 call     writehexdword
         cmp      have_big_kernel,0
         jz       @@m1_1
         @@print  @@t2_big
         jmp      @@m1_2
@@m1_1:
         @@print  @@t2_
@@m1_2:
         @@hexw   new_setup_size
         cmp      setup_version,0201h
         jb       @@m1_5
         test     loadflags,CAN_USE_HEAP
         jz       @@m1_5
         @@print  @@t2_heap
         mov      ax,heap_end_ptr
         sub      ax,new_setup_size
         @@hexw   ax
@@m1_5:
         @@print  @@v1
         @@hexw   vga_mode
         @@print  @@c2
         lea      di,command_line
         xor      ax,ax
         call     strlen
         push     ax
         call   writehexword
         @@print  @@c1
         pop      cx
         @@prints command_line,cx
         @@cr

@@verbose_stat:
         @@print  @@t21
         mov      eax,load_buffer_size
         call   writehexdword

         movzx    bx,high_mem_access
         shl      bx,1
         mov      dx,@@hmtable[bx]
         call   print

         @@print  @@t21_
         mov      ax,maximum_setup_sects*512
         call   writehexword
         cmp      have_relocated_setup,0
         jz       @@mmt22
         @@print  @@t21_oc
@@mmt22:
         cmp      high_mem_access,0
         jz       @@mmt23
         @@print  @@t22low
         mov      eax,heap_end
         sub      eax,heap_ptr
         call   writehexdword
         @@print  @@t22low2
@@mmt23:
         @@print  @@t22
         call   get_effective_physmem
         call   writehexdword


         @@print  @@t5
         lea      dx,@@real_
         cmp      cpu_type,cpu_386GE
         je       @@3
         lea      dx,@@real_p
         ja       @@3
         lea      dx,@@v86
@@3:
         call   print
         cmp      wrong_realbios,0
         je       @@3__
         @@print  @@t6_
         lea      dx,@@t6__
         jmp      @@3_1
@@3__:
         cmp      intv_size,0
         jz       @@3_1_
         lea      dx,@@t6
         cmp      intv_size,(size intv_buf)  ; have we REALBIOS
         jna      @@3_
         lea      dx,@@t6_
@@3_:
         call   print
         lea      dx,@@no
         cmp      intv_size,0
         jz       @@3_1
         lea      dx,@@yes
@@3_1:
         call   print
@@3_1_:
         @@print  @@t7
         lea      dx,@@no
         cmp      have_to_intercept_setup,1
         jb       @@3_2
         lea      dx,@@setup_
         je       @@3_2
         lea      dx,@@setup
         call   print
         mov      ax,setup_version
         call   writehexword
         jmp      @@3_2_1
@@3_2:
         call   print
@@3_2_1:
         @@print  @@s
         mov      bx,cpu_check_status
         mov      dx,@@stable[bx]
         call   print
         @@print  @@cc2
         lea      di,comline+1
         xor      ax,ax
         call     strlen
         push     ax
         call   writehexword
         @@print  @@cc1
         pop      cx
         @@prints comline+1,cx
         @@print  @@started
         lea      dx,@@rundos
         cmp      can_exit_to_dos,0
         jnz      @@4_1
         lea      dx,@@runshell
@@4_1:
         call     print

         cmp      cannot_load_because_of_windows,0
         jz       @@5
         @@print  @@sw
@@5:
         cmp      cpu_type,cpu_386GE_real_paging
         jne      @@6
         @@print  @@w1
@@6:

;         movzx    eax,kernel_size
;         shl      eax,4
         mov      eax,kernel_size
         cmp      eax,load_buffer_size
         jb       @@7
         @@print  @@w2
@@7:
         cmp      new_setup_size,maximum_setup_sects*512
         jna      @@8
         @@print  @@w3
@@8:
         cmp      option_force,0
         jz       @@8_1
         @@print  @@w4
@@8_1:
         pop_   ds,es
         popad
         ret

@@t1     db         13,10,'Your current LINUX kernel boot configuration is:'
         db         13,10,'  image file:       $'
@@t1_    db         13,10,'  kernel version    $'
@@t2     db         13,10,'  kernel size:     0x$'
;@@t2_    db               '0                  setup size:  0x$'
;@@t2_big db               '0 (high loaded)    setup size:  0x$'
@@t2_    db               '               setup size:  0x$'
@@t2_big db               ' (high loaded) setup size:  0x$'
@@t2_heap db                                                  ', heap: 0x$'

IF 0
@@t3     db         13,10,'  ram disk size:    0x$'
@@t4     db         13,10,'  root device:      0x$'
@@tro    db     '  read only$'
@@trw    db     '  read/write$'
ENDIF

@@v1     db         13,10,'  VGA mode:         0x$'
@@c2     db         13,10,'  command line (size 0x$'
@@c1     db                                         '):'
         db         13,10,'    $'
@@t21    db         13,10,'Your current DOS/CPU configuration is:'
         db         13,10,'  load buffer size: 0x$'
@@hmtable dw    @@lowtx,@@vcpitx,@@int15tx,@@xmstx
@@lowtx  db     '     $'
@@vcpitx db     ' VCPI$'
@@int15tx db    ' EXT $'
@@xmstx  db     ' XMS $'
@@t21_   db               ', setup buffer size:  0x$'
@@t21_oc db                                              ' (reloc setup)$'
@@t22low db         13,10,'  lowmem buffer:    0x$'
@@t22low2 db                                     ' (part of load buffer)$'
@@t22    db         13,10,'  total memory:     0x$'
@@t5     db         13,10,'  CPU is in $'
@@t6     db                               ',  BIOSINTV.SYS:   $'
@@t6_    db                               ',  REALBIOS:       $'
@@t6__   db     'YES, but ROM-BIOS-DATE differs, must rerun REALBIOS.EXE$'
@@t7     db         13,10,'  SetupIntercept: $'
@@yes    db     'YES$'
@@no     db     'NO$'
@@v86    db     'V86 mode$'
@@real_  db     'REAL mode$'
@@real_p db     'undocumented REAL PAGING mode, trying any way$'
@@setup_ db     'YES, patching setup code$'
@@setup  db     'YES, legal intercept, setup header version $'
@@s      db         13,10,'  stat$'
@@s8     db     '4: VCPI_present, physmap=logmap, all OK for switch to realmode$'
@@s6     db     '3: VCPI_present, but physmap != logmap (check aborted)$'
@@s4     db     '2: cpu_V86, but no VCPI available (check aborted)$'
@@s2     db     '1: cpu in real 386 mode, no need to backswitch$'
@@s0     db     '0: cpu < 386$'
@@stable dw     @@s0,@@s2,@@s4,@@s6,@@s8
@@cc2    db         13,10,'  input params (size 0x$'
@@cc1    db                                         '):'
         db         13,10,'    $'
@@started db        13,10,'  LOADLIN started from $'
@@rundos db               'DOS-prompt',13,10,'$'
@@runshell db             'CONFIG.SYS (shell= ...)',13,10,'$'
@@sw     db     '  You are running under MS-WINDOWS or Windows 95',13,10,'$'
@@w1     db     'WARNING:',13,10
         db     '  Your CPU is in realmode, but PAGING is enabled !',13,10
         db     '  This is possible, but neither documented nor supported by INTEL.',13,10
         db     "  May be your motherboard's BIOS is mapping shadow ram this way,",13,10
         db     '  or you are using one of those old and strange EMM managers,',13,10
         db     '  but probably this is an indication of a not exactly compatible',13,10
         db     '  486 clone. If you are not using one of the older EMM-managers',13,10
         db     '  you better should use the -clone switch to work around.',13,10,'$'
@@w2     db     'WARNING: Not enough free memory (load buffer size)$',13,10
@@w3     db     'WARNING: The setup buffer of LOADLIN is too small (must recompile)$',13,10
@@w4     db     'WARNING: Option -f set, CPU-Check bypassed !$',13,10

print_verbose_stat:
         pushad
         push_  ds,es
         jmp    @@verbose_stat
print_verbose endp


close_debug_file proc near
         cmp     debug_file_handle,0
         jz      @@ex
         push_   ax,bx
         mov     bx,debug_file_handle
         DosCall DOS_CLOSE_FILE
         mov     debug_file_handle,0
         pop_    ax,bx
@@ex:
         ret
close_debug_file endp


wait_and_reset_dsk proc near
         pushad
         push_   ds,es,fs,gs
         pushf
         sti
         cmp     cs:option_dskreset,0
         jz      @@wait
         mov     ax,0
         mov     dl,80h            ; reset the whole disk system
         int     13h
         cmp     cs:option_wait,0
         jnz     @@wait
         mov     cs:option_wait,9  ; wait at least 1/2 a second
@@wait:
         cmp     cs:option_wait,0
         jz      @@ex
         cli
         mov     ax,40h
         mov     ds,ax
         movzx   eax,cs:option_wait
         add     eax,dword ptr ds:[6ch]
         sti
         nop
@@loop:
         nop
         cli
         cmp     eax,dword ptr ds:[6ch]
         sti
         ja      @@loop
@@ex:
         popf
         pop_    ds,es,fs,gs
         popad
         ret
wait_and_reset_dsk endp


;=============================================================================
logo_tx               label byte
IFDEF ALPHA
  INCLUDE LOADLINA.ASM
ENDIF
                      db   'LOADLIN v',LOADLIN_VERSION,' (C) 1994..2002 Hans Lermen <lermen@fgan.de>',13,10,'$'
                      db   '              (C) 2008..2010 Samuel Thibault <samuel.thibault@ens-lyon.org>',13,10,'$'
comspec_tx            db   'comspec',0
windows_tx            db   'windir',0
err_uppermem_tx       db   'Not enough free memory between 90000h and 9ffffh,',13,10
                      db   'may be you have some zombie programs, try to reboot.',13,10
                      db   "If this doesn't help, try to find out what driver occupies this space."
newline_tx            db   13,10,'$'
enter_commandline_tx  db   'Please enter name of kernel image file followed by optional ',13,10
                      db   'command line parameters for Linux (e.g.  root=XXXX)',13,10
                      db   'or @file ( file = param file ) or "empty string" to abort:',13,10,'$'
abort_tx              db   'LOADLIN aborted by user',13,10,'$'
err_file_notfound_tx  db   'Image file not found,',13,10,'$'
enter_file_tx         db   'enter file name of Image:',13,10,'$'
err_wrong_file_tx     db   'not an Image file,',13,10,'$'
err_wrong_setup_tx    db   'Your current configuration needs interception of "setup.S,"',13,10
                      db   'but the setup-code in your Image file is *very* old (or wrong)',13,10
                      db   'Please use BIOSINTV/REALBIOS or try an other Image file',13,10,'$'
err_setup_too_long_tx db   'Ooops..., size of "setup.S" has become too long for LOADLIN,',13,10
                      db   'please E-mail this condition to samuel.thibault@ens-lyon.org !',13,10,'$'
err_kernel_to_big_tx  db   'not enough memory below 90000h to load kernel',13,10,'$'
err_io_tx             db   'IO Error during load',13,10,'$'
insert_floppy_tx      db   'Insert floppy disk for loading into RAMDISK',13,10,'$'
err_cpu_v86_tx        db   13,10,'CPU is in V86-mode (may be WINDOWS, EMM386, QEMM, 386MAX, ...)',13,10
                      db   'You need pure 386/486 real mode or a VCPI server to boot Linux',13,10
                      db   'VCPI is supported by most EMS drivers (if EMS is enabled),',13,10
                      db   "but never under WINDOWS-3.1 or WINDOWS'95.",13,10
                      db   "(However, real DOS-Mode of WINDOWS'95 can have EMS driver with VCPI)",13,10
                      db   'If loading via VCPI you also MUST have:',13,10
                      db   '  1. An interceptable setup-code (see MANUAL.TXT)',13,10
                      db   '  2. Identical Physical-to-Virtual mapping for the first 640 Kbytes',13,10,'$'
err_wrong_cpu_tx      db   'You need a 386 CPU at minimum, but you haven''t !',13,10,'$'
err_comfile_tx        db   'error on reading command line response file',13,10,'$'
option_t_terminate_tx db   13,10,'Option -t set, Linux not loaded',13,10,'$'
option_t_forced_tx    db   13,10,'ERROR detected, Linux not loaded',13,10,'$'
err_in_config_sys_tx  db   13,10,'LOADLIN holding in idle loop (You must reboot)',13,10,'$'
;-----------------------------------------------------------------------------
usage_tx  db  13,10
          db  'USAGE:',13,10
          db  '  LOADLIN @params',13,10
          db  '  LOADLIN [zimage_file] [options] [boot_params]',13,10
          db  '    without any params, LOADLIN displays this help message.',13,10
          db  '    @params:',13,10
          db  '       params is a DOS file containing all other options',13,10
          db  '    zimage_file:',13,10
          db  '       DOS file name of compressed Linux kernel image',13,10
          db  '    options:',13,10
          db  '       -v        verbose, show information on params and configuration',13,10
          db  '       -t        test mode, do all but starting Linux, also sets -v',13,10
          db  '       -d file   debug mode, same as -t, but duplicates output to "file"',13,10
          db  "       -clone    ( Please read MANUAL.TXT before using this switch! )",13,10
          db  "       -n        no translation for root=/dev/...",13,10
          db  "       -txmode   switch to textmode 80x25 on startup",13,10
          db  "       -noheap   disable use of setup heap",13,10
          db  "       -wait=nn  after loading wait nn (DOS)ticks before booting Linux",13,10
          db  "       -dskreset after loading reset all disks before booting Linux",13,10
          db  '    boot_params:',13,10
          db  '       root=xxx  filesystem to be mounted by Linux as "/"',13,10
          db  '                 (string passed unchanged to the kernel)',13,10
          db  '            xxx = hex number (e.g. root=201 for /dev/fd1)',13,10
          db  '                = /dev/mmmn (e.g. root=/dev/hda2)',13,10
          db  '                       mmm = fd,hda,hdb,sda,sdb...',13,10
          db  '                       n   = 1..10.. decimal',13,10
          db  '       ro        mount "/" readonly',13,10
          db  '       rw        mount "/" read/write',13,10
          db  '       initrd=x  (for kernels > 1.3.72) load file x into /dev/ram. If FS in x',13,10
          db  '                 contains /linuxrc, execute it, and then remount to root=xxx.',13,10
          db  '                 If root=/dev/ram, just load, bypass execution of /linuxrc',13,10,13,10
          db  '    for more boot params see PARAMS.TXT or Paul Gortmakers HOWTO:',13,10
          db  '      http://sunsite.unc.edu/mdw/HOWTO/BootPrompt-HOWTO.html',13,10
          db  '      http://rsphy1.anu.edu/~gpg109/BootPrompt-HOWTO.html',13,10
empty_tx  db  '$'


