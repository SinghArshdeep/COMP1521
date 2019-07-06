### COMP1521 19t2 ... week 05 lab
### Compute factorials -- recursive function version

## Global data
	.data
msg1:	.asciiz "n  = "
msg2:	.asciiz "n! = "
eol:	.asciiz "\n"

### main() function
	.text
	.globl	main
main:
	# Set up stack frame
	sw	$fp, -4($sp)	# push $fp onto stack
	la	$fp, -4($sp)	# set up $fp for this function
	sw	$ra, -4($fp)	# save return address
	sw	$s0, -8($fp)	# save $s0 to use as ... int n
	addi	$sp, $sp, -12	# reset $sp to last pushed item

	# code for main()
	li	$s0, 0

	la	$a0, msg1
	li	$v0, 4
	syscall			# printf("n  = ")

	li	$v0, 5
	syscall			# scanf("%d", into $v0)
	move	$s0, $v0

	move	$a0, $s0	#         + from $s0
	jal	fac		# % = fac(n)
	nop	#[branch delay] # |
	move	$s0, $v0	# + into $s0; don't need n now

	la	$a0, msg2
	li	$v0, 4
	syscall			# printf("n! = ")

	move	$a0, $s0	# assume $s0 holds n!
	li	$v0, 1
	syscall			# printf ("%d", tmp)

	la	$a0, eol
	li	$v0, 4
	syscall			# printf("\n")

	# clean up stack frame
	lw	$s0, -8($fp)	# restore $s0 value
	lw	$ra, -4($fp)	# restore $ra for return
	la	$sp, 4($fp)	# restore $sp (remove stack frame)
	lw	$fp, ($fp)	# restore $fp (remove stack frame)

	li	$v0, 0
	jr	$ra		# return 0


### fac() function
	.text
	.globl	fac
fac:
	## TODO: set up stack frame
    sw $fp, -4($sp)
    la $fp, -4($sp)
    sw $ra, -8($sp)
    sw $s0, -12($sp)
    sw $s1, -16($sp)
    sw $s2, -20($sp)
    addi $sp, $sp, -24

	## TODO: code for recursive fac()
    move $s1, $a0

base_case:
    li $t0, 1
    bgt   $a0, $t0, rec_case    # Else go to recursive case
    li    $v0, 1
    j     return

rec_case:
    move $s1, $a0
    addi  $a0, $a0, -1
    jal   fac           # Recursive call
    move  $t0, $v0      # Store result in $t0

    mul $v0, $t0, $s1
    j return 
	## TODO: clean up stack frame
return:
    lw $s2, -16($fp)
    lw $s1, -12($fp)
    lw $s0, -8($fp)
    lw $ra, -4($fp)
    la $sp, 4($sp)
    lw $fp, ($fp)

	jr	$ra		# ... and return
