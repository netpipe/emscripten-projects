/*
 *  loadlinh.S  ( part of page adjustment )
 *
 *  stolen from linux/boot/head.S ( Copyright (C) 1991..1995  Linus Torvalds )
 */

.text

#define __ASSEMBLY__

#define OUR_STACK 0xa0000   /* this stack is top of 640Kb */

	.globl startup_32
	
startup_32:
	cli
	cld
	movw %cs,%ax
	addb $8,%al	#KERNEL_DS is always one slot behind KERNEL_CS
#	movl $(KERNEL_DS),%eax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%fs
	mov %ax,%gs
	mov %ax,%ss
	mov $(OUR_STACK),%esp
	xorl %eax,%eax
1:	incl %eax		# check that A20 really IS enabled
	movl %eax,0x000000	# loop forever if it isn't
	cmpl %eax,0x100000
	je 1b
/*
 * Initialize eflags.  Some BIOS's leave bits like NT set.  This would
 * confuse the debugger if this code is traced.
 * XXX - best to initialize before switching to protected mode.
 */
	pushl $0
	popfl
/*
 * Do the page adjustment, and jump to the kernel decompressor..
 */
	call page_adjustment
	xorl %ebx,%ebx
#	ljmp $(KERNEL_CS), $0x1000
	jmp  *%eax
