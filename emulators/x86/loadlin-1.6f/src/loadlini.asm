;   >>> this is file LOADLINI.ASM
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

relocate_setup_code proc near
; NOTE: This routine has to be updated whenever a new access to
;       High_seg or High_addr is inserted in the code !
;       It relocates those addresses, if the setup code is
;       not loaded at 9000h
;
         push_  eax,bx
         mov    have_relocated_setup,1
                ; these can simply overwriten
         mov    ax,High_seg
         mov    reloc_hseg_1,ax
         mov    reloc_hseg_2,ax

@@relocd macro  desc
         lea    bx,desc
         call   relocate_descriptor
         endm
                ; these descriptors have to get added the displacement
         @@relocd gdt_code
         @@relocd gdt_data
         @@relocd gdt_ldt
         @@relocd gdt_tss

                ; these DWORDS have to get added the displacement
         sub    ax,High_seg_
         movsx  eax,ax
         shl    eax,4
         add    our_CR3,eax
         add    our_GDTRptr,eax
         add    our_IDTRptr,eax
         add    laddr_GDT,eax
         add    laddr_IDT,eax
         add    pagedir_template,eax
         add    pageadjlist_ptr,eax
         call   preset_pagedir_from_template

         pop_   eax,bx
         ret
relocate_setup_code endp


relocate_descriptor proc near
; input:
;   AX  =  new 'High_seg' frame
;   BX  =  offset of descriptor
;          bases address of descriptor must be relative to 'High_seg_'
; outpout:
;   all registers preserved, descriptor bases updated
         push_  ecx,eax
         mov    ch,[bx].base24
         mov    cl,[bx].base16
         shl    ecx,16
         mov    cx,[bx].base0
         sub    ax,High_seg_
         movsx  eax,ax
         shl    eax,4
         add    eax,ecx
         mov    [bx].base0,ax
         shr    eax,16
         mov    [bx].base16,al
         mov    [bx].base24,ah
         pop_   ecx,eax
         ret
relocate_descriptor endp


read_comline_from_file proc near
;;        mov      ax,DOS_OPEN_FILE shl 8
;;        lea      dx,image_name+1
;;        DosInt
        push     fs
        call     @@open
        jnc      @@isopen
@@err:
        lea      dx,err_comfile_tx
        call     print
        mov      word ptr comline-1,0
        jmp      @@ex
@@ioerr:
;;        DosCall  DOS_CLOSE_FILE
        call     @@close
        jmp      @@err
@@isopen:
        mov      bx,ax
        cld
        lea      di,comline+1
@@loop:
        mov      dx,di
        mov      cx,1
;;        DosCall  DOS_READ_FROM_HANDLE
        call     @@read
        jc       @@ioerr
        cmp      ax,1
        jne      @@eof
        mov      al,byte ptr [di]
        cmp      al,13
        je       @@loop
        cmp      al,'#'
        je       @@skipeol
@@skipcontinue:
        inc      di
        cmp      di, offset comline_end-1
        jnb      @@eof
        cmp      al,' '
        ja       @@loop
        cmp      di,offset comline+2
        jna      @@1
        cmp      byte ptr [di-2],' '  ; ignore multiple spaces
        jne      @@1
        dec      di
        jmp      @@loop
@@1:
        mov      byte ptr [di-1],' '
        jmp      @@loop
@@eof:
        mov      byte ptr [di],0
        mov      ax,di
        sub      ax,offset comline+1
        xchg     al,ah
        mov      word ptr comline-1,ax
;;        DosCall  DOS_CLOSE_FILE
        call     @@close
        jmp      @@ex
@@skipeol:
        mov      dx,di
        mov      cx,1
;;        DosCall  DOS_READ_FROM_HANDLE
        call     @@read
        jc       @@ioerr
        cmp      ax,1
        jne      @@eof
        cmp      byte ptr [di],10
        jne      @@skipeol
        jmp      @@skipcontinue

@@ex:
        pop      fs
        ret

; -----------

@@virtual db     0

@@open:                    ; virtual open routine
        cmp      dword ptr image_name,'ol@@'
        jne      @@open_d
        cmp      dword ptr image_name+4,'ilda'
        je      @@open_v
@@open_d:
        mov      @@virtual,0
        lea      dx,image_name+1
        mov      ax,DOS_OPEN_FILE shl 8
        DosInt
        ret
@@open_v:
        mov      @@virtual,1
        mov      ax,High_Seg
        cmp      dword ptr image_name+4+4,'@@xn'
        jne      @@open_vhex
        mov      fs,ax
        lea      ax,params_from_loadlinX
        clc
        ret
@@open_vhex:
        push     si
        lea      si,image_name+4+2
        mov      word ptr [si],'x0'
        call     value_of
        mov      fs,ax
        xor      ax,ax ; carry is also 0
        pop      si
        ret

@@close:
        cmp      @@virtual,0
        jz       @@close_d
        ret
@@close_d:
        DosCall  DOS_CLOSE_FILE
        ret
@@read:
        cmp      @@virtual,0
        jz       @@read_d
        mov      al, byte ptr fs:[bx]
        or       al,al
        jz       @@read_eof
        inc      bx
        mov      byte ptr [di],al
        mov      ax,1
        clc
        ret
@@read_eof:
        xor      ax,ax
        clc
        ret
@@read_d:
        DosCall  DOS_READ_FROM_HANDLE
        ret

read_comline_from_file endp

build_arglist proc near
; input:
;   ES=DS=CS= seg of all pointers
;   SI      = pointing to commandline like string
;   DI      = target buffer
; output:
;   DI      = pointing to next free byte in target buffer
;   all other registers preserved
@@stack struc
  pushA_struc @@
@@stack ends
        pusha
        mov     bp,sp
        xor     cx,cx
@@1:
        inc     cx
        call    get_token
        jnz     @@1

        mov     di,[bp].@@di
        mov     bx,di
        inc     cx
        shl     cx,1
        add     di,cx
        mov     si,[bp].@@si
@@2:
        mov     word ptr [bx],di
        inc     bx
        inc     bx
        call    get_token
        mov     di,ax
        jnz     @@2
        mov     word ptr [bx-2],0
        mov     [bp].@@di,ax
        popa
        ret
build_arglist endp

handle_response_file proc near
IF 0
        call    read_comline_from_file
ELSE
        pushad
        cld
        lea     di,aligned_auxbuff
        mov     @@list,di
        call    build_arglist     ; we save the rest of the old commandline
        mov     @@ptr,di
        call    read_comline_from_file
        cmp     word ptr aligned_auxbuff,0 ; nothing left in the old ?
        jz      @@ex
        lea     si,comline+1
        mov     @@cptr,si
        cld
        mov     di,@@ptr
        call    build_arglist      ;we scan the read-in commandline
                         ; now we check if there are overwrites
        mov     si,word ptr aligned_auxbuff
        mov     @@list,si
        mov     dx,@@ptr
        mov     si,dx
        mov     si,word ptr [si]
        or      si,si
        jz      @@ex_0
                ; check if we have to replace the image
        lea     di,@@image
        mov     bx,@@list ; string list of old args
        call    check_token
        js      @@2
        mov     bx,word ptr aligned_auxbuff[bx]
        mov     byte ptr [bx],1    ; mark as 'used'
        cmp     byte ptr [bx+5],0
        je      @@2
        cmp     byte ptr [bx+6],0
        je      @@2
        lea     si,[bx+6]    ; take what is behind 'image='
        jmp     @@2
@@1:
        mov     si,dx
        mov     si,word ptr [si]
        or      si,si
        jz      @@ex_0
                           ; we check for the special case 'ro','rw'
        cmp     byte ptr [si+2],0
        jne     @@1_3
        cmp     word ptr [si],'or'
        jne     @@1_1
        lea     di,@@rw    ; search the opposite (rw replaces ro)
        jmp     @@1_2
@@1_1:
        cmp     word ptr [si],'wr'
        jne     @@1_3
        lea     di,@@ro    ; search the opposite (ro replaces rw)
@@1_2:
        mov     bx,@@list ; string list of old args
        call    check_token
        jmp     @@1_4
@@1_3:
        mov     di,si
        mov     bx,@@list ; string list of old args
        call    check_token
@@1_4:
        js      @@2
                ; we have to take the old one
        mov     si,word ptr aligned_auxbuff[bx]
@@2:
        push    si
        mov     di,@@cptr
        call    stringcpy
        pop     si
        mov     @@cptr,di
        mov     byte ptr [di-1],' '
        add     dx,2
        cmp     si,@@list       ; avoid overwriting our constants
        jb      @@1
        mov     byte ptr [si],1
        jmp     @@1
@@ex_0:
                ; we have to take the rest of the old commandline
        lea     bx,aligned_auxbuff ; string list of old args
@@ex_1:
        mov     si,word ptr [bx]
        add     bx,2
        or      si,si
        jz      @@ex_2
        cmp     byte ptr [si],1
        je      @@ex_1       ; skip, what we already replaced
        mov     di,@@cptr
        call    stringcpy
        mov     @@cptr,di
        mov     byte ptr [di-1],' '
        jmp     @@ex_1
@@ex_2:
        mov     bx,@@cptr
        mov     byte ptr [bx-1],0
        sub     bx, offset comline+1
        jz      @@ex_5
        dec     bx
@@ex_5:
        xchg    bh,bl
        mov     word ptr comline-1,bx
@@ex:
        popad
        ret
@@ptr   dw      0
@@list  dw      0
@@cptr  dw      0
@@image db      'image=',0
@@rw    db      'rw',0
@@ro    db      'ro',0
ENDIF
handle_response_file endp


check_token proc near
; input:
;   ES=DS=CS= seg of all pointers
;   DI = pointing to token
;   BX = pointing to string-table, terminated by a ZERO-string
; output:
;   AX = -1, no match found
;      =  0, found a matching string, BX = index*2 of that string
;      =  1, same as AX=0, but token was terminated by '='
;   DI =  pointing to token behind terminator (0 or '=')
;
        push_   cx,dx
        cld
        xor     dx,dx
        jmp     @@loop_entry
@@loop:
        pop     di
        inc     dx
        xchg    bx,di
        xor     ax,ax
        mov     cx,-1
        repnz scasb       ; skip to next string in table
        xchg    bx,di
        cmp     byte ptr [bx],0
        jz      @@not_found
@@loop_entry:
        push    di
        mov     cx,size aux_token
        call    name_compare
        jz      @@loop
        pop     ax     ; clean up stack
        xchg    dx,bx  ; get index
        shl     bx,1
        xor     ax,ax
        cmp     byte ptr [di-1],'='
        jne     @@ex
        inc     ax
@@ex:
        pop_    cx,dx
        test    ax,ax
        ret
@@not_found:
        mov     ax,-1
        jmp     @@ex
check_token endp

get_setup_version proc near
        mov      cs:setup_version,0
        cmp      dword ptr ds:setup_header_sign,SIGNATURE
        jne      @@ex
        push     word ptr cs:setup_header_version
        pop      cs:setup_version
@@ex:
        ret
get_setup_version endp


parscommandline proc near
        mov     image_name,0
parscommandline_:
        call    clear_to_default
        mov     cx,word ptr comline-1
        xchg    ch,cl
        jcxz    @@ex_carry
        mov     si,cx
        mov     comline[si+1],0
        mov     command_line,0
        mov     cl_pointer,offset command_line
        lea     si,comline+1
        cld
        cmp     image_name,0
        jnz     @@1
        lea     di,image_name
        call    get_token   ; get the zImage-file name
        jz      @@ex_carry

        cmp     image_name,'-'
        jne     @@0
        mov     dword ptr image_name,'amiz'
        mov     word ptr image_name+4,'eg'
        mov     image_name+6,0
        jmp     parscommandline_
@@0:
                            ; got zImage-name
                            ; try to get params
        cmp     image_name,'@'
        jne     @@1
        call    handle_response_file
        jmp     parscommandline
@@1:
        lea     di,image_name
        call    tolower
        jmp     @@next_token

@@token_table label byte
              db    'ramdisk',0
              db    'vga',0
              db    'mem',0
              db    'root',0
              db    'ro',0
              db    'rw',0
              db    'no387',0
              db    'single',0
              db    'auto',0
              db    '-v',0
              db    '-t',0
              db    '-d',0
              db    '-rb',0
              db    '-rx',0
              db    '-ja',0
              db    '-clone',0
;              db    '-oldxd',0
              db    'debug',0
              db    'no-hlt',0
              db    'reserve',0
              db    'hd',0
              db    'bmouse',0
              db    'max_scsi_luns',0
              db    'xd',0
              db    '-n',0
              db    '-txmode',0
              db    '-f',0
              db    'initrd',0
              db    '-noheap',0
              db    '-wait',0
              db    '-dskreset',0
              db    0


@@jmp_table   dw  @@ramdisk
              dw  @@vga
              dw  @@mem      ;@@tolower  ; mem
              dw  @@tolower  ; root
              dw  @@tolower  ; ro
              dw  @@tolower  ; rw
              dw  @@tolower  ; no387
              dw  @@tolower  ; single
              dw  @@tolower  ; auto
              dw  @@option_v
              dw  @@option_t
              dw  @@option_d
              dw  @@option_realbios
              dw  @@option_rx
              dw  @@option_ja
              dw  @@option_clone
;              dw  @@option_oldxd
              dw  @@tolower  ; debug
              dw  @@tolower  ; no-hlt
              dw  @@tolower  ; reserve=
              dw  @@tolower  ; hd=
              dw  @@tolower  ; bmouse=
              dw  @@tolower  ; max_scsi_luns=
              dw  @@tolower  ; xd=
              dw  @@option_n
              dw  @@option_txmode
              dw  @@option_force
              dw  @@option_initrd
              dw  @@option_noheap
              dw  @@option_wait
              dw  @@option_dskreset

@@next_token:
        lea     di,aux_token
        call    get_token
        jz      @@ex0
        lea     di,aux_token
        lea     bx,@@token_table
        call    check_token
        js      @@not_my_token
        jmp     @@jmp_table[bx]

@@mem:
        jz      @@not_my_token
        push    di
        lea     di,aux_token
        call    tolower
        pop     di
        cmp     byte ptr [di],'n'  ; = 'nopentium'
        je      @@not_my_token
        xchg    si,di
        call    value_of
        call    adjust_k_or_m
        xchg    si,di
        mov     end_of_physmem,eax
        jmp     @@not_my_token

@@option_wait:
        jz      @@not_my_token
        push    di
        lea     di,aux_token
        call    tolower
        pop     di
        xchg    si,di
        call    value_of
        xchg    si,di
        mov     option_wait,ax
        jmp     @@next_token

@@option_initrd:
        jz      @@not_my_token
        push_   si,di
        xchg    si,di
        lea     di,rdimage_name
        call    stringcpy
        pop_    si,di
        mov     option_initrd,1
        jmp     @@next_token



@@ramdisk:      ; have ramdisk param
        mov     got_ram_disk,1
        mov     new_ram_disk,1440
        jz      @@next_token
        xchg    si,di
        call    value_of
        cmp     word ptr [si],'on' ; disable diskchange prompt ?
        jne     @@ramdisk_1
        mov     option_nodiskprompt,1
@@ramdisk_1:
        xchg    si,di
        mov     new_ram_disk,ax
        jmp     @@next_token

@@token_table_vga label byte
              db    'normal',0
              db    'extended',0
              db    'ask',0
              db    0

@@vga:          ; have vga - param
        mov     got_vga_mode,1
        mov     new_vga_mode,-1  ; default = NORMAL
        jz      @@next_token
        mov     bx,di
        cmp     byte ptr [bx],'-'
        jne     @@vga_1
        inc     bx
@@vga_1:
        cmp     byte ptr [bx],'0'
        jb      @@check_vga_sym
        cmp     byte ptr [bx],'9'
        jna     @@check_vga_num
@@check_vga_sym:
        ; DI already pointing to value
        lea     bx,@@token_table_vga
        call    check_token
        js      @@next_token
        shr     bx,1
        not     bx
        mov     new_vga_mode,bx
        jmp     @@next_token
@@check_vga_num:
        xchg    si,di
        call    value_of
        xchg    si,di
        mov     new_vga_mode,ax
        jmp     @@next_token

@@option_t:
        mov     option_t,1     ; forces also option -v
@@option_v:
        mov     option_v,1
        jmp     @@next_token
@@option_noheap:
        mov     option_noheap,1
        jmp     @@next_token

@@option_d:
        lea     di,aux_token   ; get output file name
        call    get_token
        jz      @@option_t     ; no file name
        push_   bx,cx,dx
        xor     cx,cx
        lea     dx,aux_token
        DosCall DOS_CREATE_FILE
        pop_    bx,cx,dx
        jc      @@option_t
        mov     debug_file_handle,ax
        jmp     @@option_t

@@option_realbios:
        mov     option_realbios,1
        jmp     @@next_token
@@option_rx:
        mov     option_rx,1
        jmp     @@next_token
@@option_ja:
        mov     option_ja,1
        jmp     @@next_token
@@option_clone:
        mov     option_clone,1
        dec     token_count
        jmp     @@next_token
@@option_n:
        mov     option_n,1
        jmp     @@next_token
@@option_force:
        mov     option_force,1
        jmp     @@next_token
@@option_dskreset:
        mov     option_dskreset,1
        jmp     @@next_token

@@option_txmode:
        pusha
        push_   ds,es
        xor     ax,ax
        mov     es,ax
        mov     ax,3003h ; al = color mode
        and     ah,byte ptr es:[0410h] ; get EQUIPPEMENT-flags
        cmp     ah,30h
        jne     @@option_txmode6
        mov     al,7     ; al = mono mode
@@option_txmode6:
        xor     ah,ah
        INT     10h
        pop_    ds,es
        popa
        jmp     @@next_token


@@not_my_token:
        push    si
        lea     si,aux_token
        call    @@append_to_commandline
        pop     si
        jmp     @@next_token
@@tolower:
        lea     di,aux_token
        call    tolower
        jmp     @@not_my_token



@@ex0:
        lea     si,@@boot_tx
        call    @@append_to_commandline
        dec     cl_pointer
        lea     si,image_name
        cld
        mov     di,si
@@ex1:
        lodsb
        test    al,al
        jz      @@ex3
        cmp     al,':'
        je      @@ex2
        cmp     al,'\'
        je      @@ex2
        cmp     al,'/'
        jne     @@ex1
@@ex2:
        mov     di,si
        jmp     @@ex1
@@ex3:
        mov     si,di
        call    @@append_to_commandline
@@ex:
        clc
        ret
@@ex_carry:
        stc
        ret

@@append_to_commandline:
        mov     di,cl_pointer
@@append_to_1:
        lodsb
        stosb
        test    al,al
        jnz     @@append_to_1
        mov     byte ptr [di-1],' '
        mov     byte ptr [di],al
        mov     cl_pointer,di
        ret

@@boot_tx     db    'BOOT_IMAGE=',0

parscommandline endp


handle_kernel_specifics proc near
IF 0
        cmp     kernelversion,0
        jz      @@ex
; put here kernel dependent stuff
@@ex:
ENDIF
        ret
handle_kernel_specifics endp

get_token proc near
; returns:
;   ZFLAG = 1 on EOF
;   AX = increased DI
        push    di
@@skip_loop:
        lodsb
        cmp     al,' '
        je      @@skip_loop
        jmp     @@ok
@@loop:
        lodsb
        cmp     al,' '
        jne     @@ok
        xor     al,al
@@ok:
        stosb
        test    al,al
        jne     @@loop
        dec     si
        mov     ax,di
        pop     di
        cmp     byte ptr es:[di],0
        jz      @@ex
        inc     token_count
@@ex:
        test    byte ptr es:[di],255
        ret
get_token endp


readstring proc near
        push    dx
        mov     comline-1,255-2
        lea     dx,comline-1
        DosCall DOS_BUFFERED_INPUT
        lea     dx,newline_tx
        call    print
        push    bx
        movzx   bx,comline
        mov     comline[bx+1],0
        mov     comline-1,0
        pop     bx
        pop     dx
        ret
readstring endp


stringcpy proc near
; input:
;   DS:SI= source address
;   DS:DI= target address
        push_    ds,ds
        pop     es
        cld
@@loop:
        lodsb
        stosb
        or      al,al
        jnz     @@loop
@@ex:
        pop     ds
        ret
stringcpy endp


tolower proc near
; input:
;   DS:DI= address to ASCIIZ-string
        cld
        xchg    si,di
@@loop:
        lodsb
        or      al,al
        jz      @@ex
        cmp     al,'A'        ; tolower !
        jb      @@loop
        cmp     al,'Z'
        ja      @@loop
        add     byte ptr [si-1],'a'-'A'
        jmp     @@loop
@@ex:
        xchg    si,di
        ret
tolower endp

name_compare proc near
; input:
;   DS:DI= address to ASCIIZ-string  (name1)
;          (a "=" marks end of string also)
;   CS:BX= address to ASCIIZ-string  (name2)
;          (a "=" marks end of string also)
;   CX=    max number of character to compare
;   compare is done by ignoring upper/lower-case
; Output:
;   AX=    0 if not equal else equal
;   DI=    as changed by lodsb, pointing behind terminator (0 or '=')
;   all other register preserved
          cld
          push    bx
          xchg    si,di
          sub     bx,si
@@m3:
          lodsb
          cmp     al,'='        ; may be name in environement
          jne     short @@32
          xor     al,al
          jmp     short @@m35
@@32:
          cmp     al,'A'        ; tolower !
          jb      short @@m35
          cmp     al,'Z'
          ja      short @@m35
          add     al,'a'-'A'
@@m35:
          mov     ah,al
          mov     al,byte ptr cs:[si+bx-1]
          cmp     al,'='        ; may be name in environement
          jne     short @@34
          xor     al,al
          jmp     short @@m36
@@34:
          cmp     al,'A'        ; tolower !
          jb      short @@m36
          cmp     al,'Z'
          ja      short @@m36
          add     al,'a'-'A'
@@m36:
          or      al,al
          jz      short @@ex_
          or      ah,ah
          jz      short @@exfalse
          cmp     ah,al
          jne     @@exfalse
          loop    @@m3
@@extrue:
          mov     ax,1
@@ex:
          xchg    si,di
          pop     bx
          or      ax,ax
          ret
@@ex_:
          or      ax,ax
          jz      @@extrue
@@exfalse:
          xor     ax,ax         ;  not equal
          jmp     @@ex
name_compare endp


get_env_variable proc near
; Input
;    CS:BX= Pointer to string (ASCIIZ) of desired env-variable
; Output:
;    ES:DI=    pointer to string (ASCIIZ) of variable contents (without "=")
;            or ZERO-string if not found
;    All other registers preserved
          push    ds
          push    ax
          push    si
          mov     ds,PSP_frame
          mov     ds,ds:PSP_envir_frame
          xor     si,si

          mov     al,byte ptr [si]
@@m1:
          mov     cx,80
          or      al,al
          mov     di,si
          jz      @@ex
          call    name_compare
          jnz     @@found    ; string is equal, found the variable
          cld
@@m2:                   ; skip to next string
          lodsb
          or      al,al
          jnz     @@m2
          mov     al,byte ptr [si]
          jmp     @@m1
@@found:
@@ex:
          push    ds
          pop     es
          pop     si
          pop     ax
          pop     ds
          ret
get_env_variable endp


adjust_k_or_m proc near
  ; Input:
  ;   DS:SI, EAX as delivered by 'value_of'
          pushf
          push    cx
          xor     cx,cx
          mov     ch, byte ptr [si-1]
          cmp     ch,'k'
          jne     @@1
          mov     cl,10
@@ex:
          shl     eax,cl
@@ex0:
          pop     cx
          popf
          ret
@@1:
          cmp     ch,'m'
          jne     @@ex0
          mov     cl,20
          jmp     @@ex
adjust_k_or_m endp

value_of proc near
  ; Input:
  ;   DS:SI= pointer to string (ASCIIZ), which contains number to be converted
  ;          (i.e.: 1234, 0xa00, ... ), all non-digit-characters (0..9,a..f,A..F)
  ;          are "delimiters" and mark the end of the string
  ; Output:
  ;   EAX= converted number
  ;   SI= pointing at the first byte behind the delimiter
          cmp    byte ptr [si],'-'
          pushf
          jnz    @@m0
          inc    si
  @@m0:
          push_  ebx,ecx,edx,edi,esi
          mov    ebx,10
          xor    edx,edx
          xor    edi,edi
          inc    edi
          cld
          lodsb
          call   @@check_white
          jc     @@ex
          jne    @@m1
          lodsb
          or     al,20h   ; convert to lowercase
          cmp    al,'x'
          jne    @@m1
          mov    ebx,16
          lodsb
@@m1:
          call   @@check_white
          jb     @@ex
          xor    ecx,ecx
@@m2:
          inc    ecx
          lodsb
          call   @@check_white
          jnc    @@m2
          std
          mov    [esp],esi ;advance pointer
          dec    si
          dec    si
@@m3:
          lodsb
          or     al,20h    ; convert to lowercase
          cmp    al,'9'
          jna    short @@m4
          add    al,9
@@m4:
          and    al,0fh
          movzx  eax,al
          imul   eax,edi
          imul   edi,ebx
          add    edx,eax
          loop   @@m3
@@ex:
          mov    eax,edx
          pop_   ebx,ecx,edx,edi,esi
          popf
          jnz    @@ex_
          neg    eax
@@ex_:
          ret

@@check_white:
          mov    ah,'f'
          cmp    bl,16
          je     short @@7
          mov    ah,'9'
@@7:
          or     al,20h
          cmp    al,'0'
          jb     short @@9
          cmp    al,ah
          ja     short @@9
          cmp    al,'9'
          jbe    short @@8
          cmp    al,'a'
          jae    short @@8
@@9:
          mov    al,' '
@@8:
          cmp    al,'0'
          ret

value_of endp


