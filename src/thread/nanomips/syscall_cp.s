.set    noreorder

.global __cp_begin
.hidden __cp_begin
.type   __cp_begin,@function
.global __cp_end
.hidden __cp_end
.type   __cp_end,@function
.global __cp_cancel
.hidden __cp_cancel
.type   __cp_cancel,@function
.hidden __cancel
.global __syscall_cp_asm
.hidden __syscall_cp_asm
.type   __syscall_cp_asm,@function
__syscall_cp_asm:
__cp_begin:
	lw      $a0, 0($a0)
	bnezc   $a0, __cp_cancel	# if self->cancel is set

	move	$t4, $a1		# syscall number in t4
	move    $a0, $a2
	move    $a1, $a3
	move    $a2, $a4
	move    $a3, $a5
	move    $a4, $a6
	move    $a5, $a7
	syscall32

__cp_end:
	jrc     $ra

__cp_cancel:
	lapc	$t0, __cancel
	jrc     $t0			# long __cancel()
