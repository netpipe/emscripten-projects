;   >>> this is file LOADLINM.ASM
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

;---------------------------------------------------------------------------



call_pmode_routine proc near
; NOTE: must have called get_VCPI_interface before this
;       _AND_ must be already moved high, else the pagetable
;       will not be aligned.
; input:
;  AX  = offset of protected mode routine
; The called routine will have a pointer in ebp to a pushad_struct
; of the other input registers, and all segment register point to
; High_seg
;
@@regs struc
  pushAD_struc @@
@@regs ends
          pushf
          CLI
          pushad
          mov    ebp,esp
                 ; ÚÄÄÄÄÄ  IRET stack for return to VM86
          push   0
           push   gs
          push   0
           push   fs
          push   0
           push   ds
          push   0
           push   es
          push   0
           push   ss
          push   ebp
          sub    sp,4           ; space for EFLAGS
          push   0
           push   cs
          push   0
           push   offset @@pmode_return
                 ; ÀÄÄÄÄÄÄ  IRET stack for return to VM86
          mov    cs:pmode_return_esp,esp
          lea    ax,@@pmode_task
          movzx  eax,ax
          mov    dword ptr protected_mode_target,eax
          mov    word ptr protected_mode_target+4,g_code
          movzx   esi,cs:High_Seg
          shl     esi,4
          lea     si,our_CR3[si]
          mov     ax,0DE0Ch
          int     emm_int    ; jumps to protected mode
          ; and comes here in 16-bit protected mode
@@pmode_task:
          mov     ax,g_data
          mov     ss,ax
          mov     ds,ax
          mov     es,ax
          mov     fs,ax
          mov     gs,ax
          mov     esp,cs:pmode_return_esp

                             ; ebp pointing to pushad_struct
          mov     ax, word ptr [bp].@@eax
          call    ax         ; call the routine

          ; now we return to VM86 mode
          cli      ; to be save
          mov     ax,g_core    ; selector for addressing first MByte
          mov     ds,ax
          .386p
          CLTS                 ; clear taskswitch flag
          .386
          mov     eax,0DE0Ch
          call    far fword ptr cs:server_vcpi_entry  ; back to VM86

          ; and returns here in VM86 mode
@@pmode_return:
          popad
          popf
          ret
call_pmode_routine endp


IF 0
AUXPAGE       =  (modul_end-code_org0) ; behind our code
ELSE
AUXPAGE       =  0 ; at start of our segment, only used in protected mode
ENDIF
AUXPAGE_size  =  02000h
AUXPAGE_entry =  (AUXPAGE shr (12-2))
AUXPAGE_S     =  AUXPAGE
AUXPAGE_T     =  AUXPAGE+AUXPAGE_size

AUXPAGE_S_entry equ page0+AUXPAGE_entry[bx]
AUXPAGE_T_entry equ page0+(AUXPAGE_entry+(AUXPAGE_size shr (12-2)))[bx]
AUXPAGE_BITS  =  3       ; writeable, present

move_anywhere_vcpi proc near
; input:
;   ESI= linear source address
;   EDI= linear target address
;    CX= size to move, must be <= 4096 !!!
; output:
;    all registers preserved
                ; we are in VM86 and at this point assume to have VCPI
        push    ax
        lea     ax,@@pmove
        call    call_pmode_routine
        pop     ax
        ret
                ; this will be executed in 16-bit protected mode
@@pmove:
@@regs struc
  pushAD_struc @@
@@regs ends
                ; save pagetable-entries
        mov     bx,High_Seg
        shr     bx,(12-4-2)
        push    AUXPAGE_S_entry
        push    AUXPAGE_S_entry+4
        push    AUXPAGE_T_entry
        push    AUXPAGE_T_entry+4

mmm     macro   reg,st
        mov     eax,[bp].@@e&reg
        mov     reg,ax
        and     reg,0fffh
        lea     reg,[reg]+AUXPAGE_&st
        and     ax,0f000h
        or      al,AUXPAGE_BITS
        mov     AUXPAGE_&st&_entry,eax
        add     eax,1000h
        mov     AUXPAGE_&st&_entry+4,eax
        endm

        mmm     si,S        ; map source
        mmm     di,T        ; map target
          .386p
        mov     eax,CR3     ; flush TLB
        mov     CR3,eax
          .386

        cld
        movzx   ecx,cx
        ror     ecx,2
        rep movsd
        rol     ecx,2
        rep movsb
                ; restore pagetable-entries
        pop     AUXPAGE_T_entry+4
        pop     AUXPAGE_T_entry
        pop     AUXPAGE_S_entry+4
        pop     AUXPAGE_S_entry
        ret
move_anywhere_vcpi endp


;---------------------------------------------------------------


move_anywhere_INT15 proc near
; input: ESI = source linear address
;        EDI = destination linear address
;        CX  = count
; returns:
;        all registers preserved
;
@@descript    struc
  @@limit     dw    ?
  @@base0     dw    ?
  @@base16    db    ?
  @@typbyte   db    ?
  @@limit16   db    ?
  @@base24    db    ?
@@descript    ends
        push_   es,eax,esi,edi,ecx
        mov     cs:@@src.@@base0,si
        shld    eax,esi,16
        mov     cs:@@src.@@base16,al
        mov     cs:@@src.@@base24,ah
        mov     cs:@@dest.@@base0,di
        shld    eax,edi,16
        mov     cs:@@dest.@@base16,al
        mov     cs:@@dest.@@base24,ah
        mov     cx,(1000h/2)  ; force to be one page
                              ; NOTE: INT15 moves are in WORDS,
                              ; because we always move 1000h,
                              ; we simple do it also for the last page
        push    cs
        pop     es
        lea     si,@@gdt
        mov     ax,08700h
        int     15h
        pop_    es,eax,esi,edi,ecx
        ret
        align   qword
@@gdt   @@descript <0,0,0,0,0,0>
        @@descript <0,0,0,0,0,0>
@@src   @@descript <0ffffh,0,0,093h,0,0>
@@dest  @@descript <0ffffh,0,0,093h,0,0>
        @@descript <0,0,0,0,0,0>      ; BIOS CS
        @@descript <0,0,0,0,0,0>      ; BIOS SS
move_anywhere_INT15 endp

;============================================================================

DUMMY_XMS_BLOCK_START = 100000h

XMS_GET_VERSION   =  0
XMS_QUERY_FREE    =  8
XMS_ALLOC         =  9
XMS_FREE          =  0ah
XMS_MOVE_BLOCK    =  0bh
XMS_LOCK_BLOCK    =  0ch
XMS_UNLOCK_BLOCK  =  0dh

xmscall   macro   function
          mov     ah,function
          call    cs:xms_entry
          endm

check_for_XMS  proc near
          push_   ds,es,ax,bx

          mov     cs:xms_entry,0
          mov     cs:xms_avail,0
          mov     ax,4300h
          INT     2fh
          cmp     al,80h
          jnz     @@ex
          mov     ax,4310h
          INT     2fh
          mov     word ptr cs:xms_entry,bx
          mov     word ptr cs:xms_entry+2,es
          xmscall XMS_GET_VERSION
          cmp     ax,200h
          jb      @@ex0
          xmscall XMS_QUERY_FREE
          or      ax,ax
          jz      @@ex0
          mov     cs:xms_avail,ax
@@ex:
          pop_    ds,es,ax,bx
          cmp     cs:xms_entry,0
          ret
@@ex0:
          mov     cs:xms_entry,0
          jmp     @@ex
check_for_XMS  endp

allocate_xms_block proc near
          push_   bx,dx
          cmp     cs:xms_entry,0
          je      @@ex0
          mov     dx,cs:xms_avail
          xmscall XMS_ALLOC
          or      ax,ax
          je      @@ex0
          mov     cs:xms_handle,dx
@@ex:
          pop_    bx,dx
          or      eax,eax
          ret
@@ex0:
          xor     eax,eax
          jmp     @@ex
allocate_xms_block endp


lock_xms_block proc near
                                      ; try to lock the XMS block
                                      ; (needed to get the phys.address)
          push_   ax,bx,dx
          mov     dx,cs:xms_handle
          xmscall XMS_LOCK_BLOCK
          or      ax,ax
          je      @@err
          mov     word ptr cs:xms_phys_addr,bx
          mov     word ptr cs:xms_phys_addr+2,dx
          pop_    ax,bx,dx
          ret
@@err:
          push    cs
          pop     ds
          lea     dx,@@tx
          call    print
          jmp     exit_to_dos
@@tx      db      13,10,'Cannot lock XMS memory',13,10,'$'
lock_xms_block endp


free_xms_block proc near
          push_   ax,bx,dx
          mov     dx,cs:xms_handle
          xmscall XMS_UNLOCK_BLOCK
          mov     dx, cs:xms_handle
          xmscall XMS_FREE
          pop_    ax,bx,dx
          ret
free_xms_block endp


move_anywhere_xms proc near
; input:
;   ESI= linear source address
;   EDI= linear target address
;    CX= size to move, must be <= 4096 !!!  (actually _is_ always 4096)
; output:
;    all registers preserved
          cmp     edi,DUMMY_XMS_BLOCK_START
          jae     @@high
          cmp     esi,DUMMY_XMS_BLOCK_START
          jb      move_simple
@@high:
          pushad
          push    ds
          mov     ecx,1000h   ; force a length of one page to move
                              ; NOTE: XMS moves have to be even,
                              ; because we alway move 1000h,
                              ; we simple do it also for the last page
          mov     @@length,ecx
          sub     edi,DUMMY_XMS_BLOCK_START
          jb      @@lowdi
          mov     @@doffs,edi  ; high destination, just record
          mov     di,xms_handle
          mov     @@dhandle,di
          jmp     @@source
@@lowdi:
          ror     edi,4
          mov     word ptr [@@doffs+2],di
          xor     di,di
          rol     edi,4
          mov     word ptr [@@doffs],di
          xor     di,di
          mov     word ptr [@@dhandle],di

@@source:
          sub     esi,DUMMY_XMS_BLOCK_START
          jb      @@lowsi
          mov     [@@soffs],esi  ; high destination, just record
          mov     si,xms_handle
          mov     [@@shandle],si
          jmp     @@done
@@lowsi:
          ror     esi,4
          mov     word ptr @@soffs+2,si
          xor     si,si
          rol     esi,4
          mov     word ptr @@soffs,si
          xor     si,si
          mov     word ptr [@@shandle],si

@@done:
          lea     si,[@@length]
          push    cs
          pop     ds
          xmscall XMS_MOVE_BLOCK
          pop     ds
          popad
          ret
@@length     dd      ?
@@shandle    dw      ?
@@soffs      dd      ?
@@dhandle    dw      ?
@@doffs      dd      ?
move_anywhere_xms endp


;--------------------------------------------------------------------

move_simple  proc near
; input:
;   ESI= linear source address
;   EDI= linear target address
;    CX= size to move, must be <= 4096 !!!
; output:
;    all registers preserved
        pushad
        push_   ds,es
        movzx   ecx,cx
        cld
        ror     edi,4
        mov     es,di
        xor     di,di
        rol     edi,4
        ror     esi,4
        mov     ds,si
        xor     si,si
        rol     esi,4
        ror     ecx,2
        rep movsd
        rol     ecx,2
        rep movsb
@@ex:
        pop_    ds,es
        popad
        ret
move_simple  endp




build_buffer_heap proc near
        pushad
                ; first we setup the normal low heap
                ; (why not use it)
        movzx    eax,kernel_load_frame
        shl      eax,4
        mov      heap_ptr,eax
        movzx    eax,High_Seg       ; NOTE: this is page aligned !
        shl      eax,4
        mov      heap_end,eax
        sub      eax,heap_ptr
        shr      eax,12
        mov      heap_max_pages,eax

                ; now we try to get some extended memory
        cmp     need_mapped_put_buffer,0
        jz      @@err
        cmp     cpu_type,cpu_386V86
        je      @@vcpi
                ; we check what we can use to access extended memory
        call    check_for_XMS
        jz      @@need_int15
        cmp     cs:xms_avail,128
        jb      @@need_int15
        sub     cs:xms_avail,64        ; leave a 64K minimum for system
        call    allocate_xms_block
        jz      @@need_int15
        movzx   eax,cs:xms_avail
        shr     eax,2        ; from 1KB to 4KB
        add     heap_max_pages,eax
        mov     high_heap_ptr,DUMMY_XMS_BLOCK_START
        mov     high_mem_access,USING_XMS
        mov     move_anywhere, offset move_anywhere_xms
@@ex:
        mov      eax,heap_max_pages
        shl      eax,12
        mov      load_buffer_size,eax
        popad
        ret

@@need_int15:
        mov     ax,08800h
        int     15h
        sub     ax,64+64     ; leave HMA untouched, we need 64Kminimum
                             ; (else we would waste more mem than winning)
        jbe     @@err
        movzx   eax,ax
        shr     eax,2        ; from 1KB to 4KB
        add     heap_max_pages,eax
        mov     high_heap_ptr,0110000h
        mov     high_mem_access,USING_INT15
        mov     move_anywhere, offset move_anywhere_int15
        jmp     @@ex
@@err:
        mov     high_mem_access,0
        mov     move_anywhere, offset move_simple
        jmp     @@ex
@@vcpi:
        cmp     have_VCPI,0
        jz      @@ex
        push    edx
        mov     ax,0DE03h   ; get avail 4K VCPI-page
        int     emm_int
        sub     edx,(64/4)      ; leave a 64K minimum for system
        jb      @@err
        add     heap_max_pages,edx
        mov     high_mem_access,USING_VCPI
        mov     move_anywhere, offset move_anywhere_vcpi
        pop     edx
        jmp     @@ex
build_buffer_heap endp


get_buffer_from_heap proc near
; output: EAX = linear address of 4K buffer, page aligned
;               (if XMS, then the offset within the buffer)
;         CARRY =1, if memory overflow
        cmp     heap_max_pages,0
        jna     @@err
        dec     heap_max_pages
        mov     eax,heap_ptr
        cmp     eax,heap_end
        jnb     @@high
        add     heap_ptr,1000h
@@ex1:
        clc
@@ex:
        ret
@@high:
        cmp     high_mem_access,USING_VCPI   ; what heap are we using
        je      @@vcpi
        mov     eax,high_heap_ptr
        add     high_heap_ptr,1000h
        jmp     @@ex1
@@vcpi:
        push    edx
        mov     ax,0DE04h   ; get 4K VCPI-page
        int     emm_int
        mov     eax,edx
        and     ax,0f000h
        pop     edx
        jmp     @@ex1
@@err:
        xor     eax,eax
        stc
        jmp     @@ex
get_buffer_from_heap endp

free_extended_memory proc near
        cmp      need_mapped_put_buffer,0
        jz       @@ex
        cmp      high_mem_access,USING_XMS
        je       @@xms
        cmp      high_mem_access,USING_VCPI
        jne      @@ex
@@vcpi:
        push_    eax,bx,ecx,dx,bp,esi,edi
        mov      cx,word ptr pageadjlist.ncount
        sub      cx,1
        jc       @@ex1
        mov      bp,cx
        mov      bx,cx
        shr      bp,(9 - 2)
        shl      bx,3
        and      bp,~03h
@@bigloop:
        and      bx,0fffh
@@loop:
        mov      edx,pagelist[bx]
        cmp      edx,0100000h
        jb       @@ex1
        mov      ax,0DE05h   ; free 4K VCPI-page
        int      emm_int
        dec      cx
        jc       @@ex1
        sub      bx,8
        jnc      @@loop
        sub      bp,4
        ; get the page adresses page
        mov      esi,pageadjlist.sources[bp]
        mov      di,ds
        movzx    edi,di
        shl      edi,4
        add      edi,offset [pagelist]
        push     ecx
        mov      ecx,01000h
        call     move_anywhere
        pop      ecx
        mov      edx,esi
        cmp      edx,0100000h
        jb       @@ex1
        mov      ax,0DE05h   ; free 4K VCPI-page
        int      emm_int
        jmp      @@bigloop
@@ex1:
        pop_     eax,bx,ecx,dx,bp,esi,edi
@@ex:
        ret
@@xms:
        call     free_xms_block
        jmp      @@ex
free_extended_memory endp


final_page_adjust_list_handling proc near
        cmp      need_mapped_put_buffer,0
        jz       @@ex

        push_    eax,bx,ecx,dx,bp,esi,edi
        mov      dx,word ptr [pageadjlist.ncount]
        sub      dx,1
        jc       @@ex1
        ; allocate a page for the pending page adresses
        call     get_buffer_from_heap
        jnc      @@gotmem
        jmp      errnomem
@@gotmem:
        ; always copy the pending page adresses
        mov      edi,eax
        mov      si,ds
        movzx    esi,si
        shl      esi,4
        add      esi,offset [pagelist]
        mov      ecx,01000h
        call     move_anywhere
        xchg     esi,edi
        mov      bp,dx
        shr      bp,(9 - 2)
        and      bp,~03h
        mov      pageadjlist.sources[bp],eax

        cmp      high_mem_access,USING_XMS
        jne      @@ex1
        call     lock_xms_block
        mov      bp,dx
        mov      bx,dx
        shr      bp,(9 - 2)
        shl      bx,3
        and      bp,~03h
@@bigloop:
        and      bx,0fffh
@@loop:
        mov      eax,pagelist[bx]
        sub      eax,DUMMY_XMS_BLOCK_START
        jb       @@done
        add      eax,xms_phys_addr
        mov      pagelist[bx],eax
        sub      dx,1
        jc       @@done
        sub      bx,8
        jnc      @@loop
        xchg     esi,edi
        mov      ecx,01000h
        call     move_anywhere
        sub      edi,DUMMY_XMS_BLOCK_START
        jb       @@low
        add      edi,xms_phys_addr
        mov      pageadjlist.sources[bp],edi
@@low
        sub      bp,4
        ; get the next page adresses page
        mov      esi,pageadjlist.sources[bp]
        mov      di,ds
        movzx    edi,di
        shl      edi,4
        add      edi,offset [pagelist]
        mov      ecx,01000h
        call     move_anywhere
        jmp      @@bigloop
@@done:
        xchg     esi,edi
        mov      ecx,01000h
        call     move_anywhere
        sub      edi,DUMMY_XMS_BLOCK_START
        jb       @@low2
        add      edi,xms_phys_addr
        mov      pageadjlist.sources[bp],edi
	mov  eax, edi
	call writehexdword
@@low2
@@ex1:
        pop_     eax,bx,ecx,dx,bp,esi,edi
@@ex:
        ret
final_page_adjust_list_handling endp



open_new_mapped_block proc near
; input:
;   EDI = linear address of final target address
        cmp      need_mapped_put_buffer,0
        jz       @@ex
        push_    bx,edi
        mov      bx,word ptr pageadjlist.number_of_blocks
        shl      bx,3    ; * sizeof(pblock)
        and      di,0f000h
        mov      pageadjlist.blocks.taddr[bx],edi
        mov      di,word ptr pageadjlist.ncount
        mov      pageadjlist.blocks.tstart[bx],di
        mov      pageadjlist.blocks.tcount[bx],0
        inc      pageadjlist.number_of_blocks
        mov      do_mapped_put_buffer,1    ; notice 'put_buffer' what to do
        pop_     bx,edi
@@ex:
        ret
open_new_mapped_block endp


map_high_page proc near
; input:
;   EDI = linear address of source address
;   because this is called strict sequentially, the target address
;   is allways given by the next free entry in the pageadjlist
        cmp      need_mapped_put_buffer,0
        jz       @@ex
        push_    bx,edi
        mov      ebx,[pageadjlist.ncount]
        test     ebx,ebx
        jz       @@ok
        test     ebx,01ffh
        jnz      @@ok
        ; needs to save the previous page of page addresses
        push_    eax,ecx,esi,edi
        call     get_buffer_from_heap
        jnc      @@gotmem
        jmp      errnomem
@@gotmem:
        mov      edi,eax
        mov      si,ds
        movzx    esi,si
        shl      esi,4
        add      esi,offset [pagelist]
        mov      ecx,01000h
        call     move_anywhere
        mov      di,bx
        shr      di,(9 - 2)
        and      di,~03h
        sub      di,4
        mov      pageadjlist.sources[di],eax
        pop_     eax,ecx,esi,edi
@@ok:
        and      bx,01ffh
        shl      bx,3
        and      di,0f000h
        mov      pagelist[bx],edi
        inc      dword ptr [pageadjlist.ncount]
        mov      bx,word ptr [pageadjlist.number_of_blocks]
        dec      bx
        shl      bx,3    ; * sizeof(pblock)
        inc      dword ptr pageadjlist.blocks.tcount[bx]
        pop_     bx,edi
@@ex:
        ret
map_high_page endp

put_buffer  proc near
; input:
;     AX=  count
;    EDI=  linear destination address (may be > 0100000h)
;  DS:DX=  source address in lowmem
; output:
;   none, all registers preserved
;   in case of memory overflow it doesn't return (jumps to exit_to_dos)
;
        pushad
        push_   ds,es
        movzx   ecx,ax
        cmp     cs:need_mapped_put_buffer,0
        jnz     @@mapped
                ; we can mov it normally
        cld
        ror     edi,4
        mov     es,di
        xor     di,di
        rol     edi,4
        mov     si,dx
        ror     ecx,2
        rep movsd
        rol     ecx,2
        rep movsb
@@ex:
        pop_    ds,es
        popad
        ret
@@mapped:                  ; do high move
        call    get_buffer_from_heap
        jc      @@err
                           ; we don't need EDI any more, because
                           ; if we come here we are writing
                           ; strict sequentialy and have set the
                           ; starting address of the block
                           ; via open_new_mapped_block
        mov     edi,eax
        mov     si,ds
        movzx   esi,si
        shl     esi,4
        movzx   edx,dx
        add     esi,edx
        call    map_high_page
        call    move_anywhere
        jmp     @@ex
@@err:
        pop_    ds,es
        popad
errnomem:
        DosCall  DOS_CLOSE_FILE
        lea     dx,@@tx
        call    print
        jmp     exit_to_dos
@@tx    db      13,10,'Out of memory (may be low or extended)',13,10,'$'
put_buffer  endp

; ---------------------------------------------------------------
; initrd stuff

MAXPHYSMEM_FOR_INT15   = 1000000h

load_initrd proc near
        cmp      cs:option_initrd,0
        jz       @@ex
        pushad
        cmp      need_mapped_put_buffer,0
        jz       @@err_unable

        ; first get ramdisk size
        mov      ax,DOS_OPEN_FILE shl 8
        lea      dx,[rdimage_name]
        DosInt
        jc       @@err_open
        mov      fhandle,ax
        mov      bx,ax
        call     get_filesize
        mov      ramdisk_size,eax

        ; then compute kernel end estimation
;        movzx    eax,kernel_size
;        add      ax,0ffh
;        mov      al,0          ; round up to page boundary
;        shl      eax,4         ; size of compressed kernel
	mov      eax,kernel_size
	add      eax,0fffh	; round up to page boundary
	and      eax,0fffff000h	; size of compressed kernel
        mov      ecx,eax
        shl      eax,1         ; estimated size of decompressed kernel
                               ; (we assume a decompression rate of 1:2)
        sub      ecx,(90000h - 2000h) ; the low buffer size (for bzimages)
        jb       @@3
        add      eax,ecx       ; add padding
@@3:
        add      eax,(100000h+2000h)   ; (+ gunzip-heap), now we have end of kernel
        mov      [@@kernel_limit],eax

        ; then get size of memory
        call     get_effective_physmem
        ; drop 1/16 of the memory to avoid e.g. ACPI data
        mov      ebx,eax
        shr      ebx,4
        sub      eax,ebx
        ; limit ourselves to 2G since we use special values in
        ; pgadjust.c
        and      eax,7fffffffh
        mov      end_of_physmem,eax

        cmp      high_mem_access,USING_INT15
        jne      @@1
                 ; INT15 on some BIOSes will not access above 16Mb
        cmp      eax,MAXPHYSMEM_FOR_INT15
        jbe      @@1

        ; more than 16MB and using INT15, try to load below 16MB, in case the
        ; BIOS is buggy
        mov      eax,MAXPHYSMEM_FOR_INT15
        sub      eax,ramdisk_size
        and      ax,0f000h     ; round down to full page boundary
        cmp      [@@kernel_limit],eax
        jb       @@2           ; yes, fits!
        ; print warning for the user
        mov      dx,offset @@tx16
        call     print
        mov      eax,end_of_physmem

@@1:
        sub      eax,ramdisk_size
        and      ax,0f000h     ; round down to full page boundary
        cmp      [@@kernel_limit],eax
        jnb      @@err_mem
                 ; ok we have place
                            ; now loading the kernel
                         ; NOTE: needing EDI for open_new_mapped_block
@@2:
        mov      ramdisk_image,eax
        mov      edi,ramdisk_image
        call     open_new_mapped_block      ; open the first block
        mov      bx,fhandle
        mov      ecx,ramdisk_size
        mov      print_dots,3
        call     read_handle   ; read the ramdisk
        call     print_crlf
        mov      print_dots,0
        call     print_crlf
        jc       @@err_io
        cmp      eax,ecx
        jnz      @@err_io
                           ; ok, all is read into memory
        DosCall  DOS_CLOSE_FILE
        popad
@@ex:
        ret
@@err:
        popad
        mov     dx,@@errtx_addr
        call    print
        jmp     exit_to_dos
@@err_open:
        mov     @@errtx_addr,offset @@tx
        jmp     @@err
@@err_mem:
        mov     @@errtx_addr,offset @@txmem
        jmp     @@err
@@err_io:
        mov     @@errtx_addr,offset @@txio
        jmp     @@err
@@err_unable:
        mov     @@errtx_addr,offset @@txno
        jmp     @@err
@@tx    db      13,10,"can't open image file for initrd",13,10,'$'
@@txmem db      13,10,"no place after kernel for initrd",13,10,'$'
@@tx16  db      13,10,"warning: loading above 16MB using INT15",13,10,'$'
@@txio  db      13,10,"IO-error while reading initrd",13,10,'$'
@@txno  db      13,10,"no support in setup for reading initrd",13,10,'$'
@@errtx_addr dw      0
@@kernel_limit dd    0
load_initrd endp

get_filesize proc near
; input:
;   bx = filehandle
; output:
;   eax = filesize, all other registers reserved
        push_   cx,dx,esi
        mov     ax,4201h    ; seek to current position
        xor     cx,cx
        xor     dx,dx
        DosInt              ; ... and return current position in DX:AX
        push_   ax,dx       ; save it
        mov     ax,4202h    ; seek to last position
        xor     cx,cx
        xor     dx,dx
        DosInt              ; ... and return EOF position in DX:AX
        mov     si,dx
        shl     esi,16
        mov     si,ax
        pop_    dx,cx
        mov     ax,4200h    ; seek to saved position
        DosInt
        mov     eax,esi
        pop_    cx,dx,esi
        ret
get_filesize endp

