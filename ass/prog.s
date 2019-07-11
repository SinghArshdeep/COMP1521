# COMP1521 19t2 ... Game of Life on a NxN grid
#
# Written by <<YOU>>, June 2019

## Requires (from `boardX.s'):
# - N (word): board dimensions
# - board (byte[][]): initial board state
# - newBoard (byte[][]): next board state

    .data
msg1:   .asciiz "# Iterations: "
msg2:   .asciiz "=== After iteration %d ==="
eol:    .asciiz "\n"

## Provides:
	.globl	main
	.globl	decideCell
	.globl	neighbours
	.globl	copyBackAndShow


########################################################################
# .TEXT <main>
	.text
main:

# Frame:	...
# Uses:		...
# Clobbers:	...

# Locals:	...

# Structure:
#	main
#	-> [prologue]
#	-> ...
#	-> [epilogue]

# Code:

	# Your main program code goes here.  Good luck!
    # Set up stack frame
    sw      $fp, -4($sp)    # push $fp onto stack
    la      $fp, -4($sp)    # set up $fp for this function
    sw      $ra, -4($fp)    # save return address
    sw      $s0, -8($fp)    # save $s0 to save iterations
    sw      $s1, -12($fp)   # save i for the iterations
    sw      $s2, -16($fp)   # save row
    sw      $s3, -20($fp)   # save column
    sw      $s4, -24($fp)   # save # columns and row i.e. N
    sw      $s5, -28($fp)   # save value nn
    sw      $s6, -32($fp)   # save pointer for new board
    sw      $s7, -36($fp)   #
    addi    $sp, $sp, -40   # reset $sp to last pushed item

    # code for main()
    li      $s0, 0

    la      $a0, msg1
    li      $v0, 4
    syscall             # printf("# Iterations: ")

    li      $v0, 5
    syscall             # scanf("%d", into $v0)
    move    $s0, $v0      # save iterations into $s0
    lw      $s4, N        # save # rows and columns
    li      $s1, 0        # load value into i
    li      $s2, 0            # row = 0
    li      $s3, 0            # col = 0
//    li    $s5, 4            # size for int
//    mul   $s4, $s6, $s5    # row size

iterations:
    bge     $s1, $s0, endloop

loop_row:
    bge     $s2, $s4, iterations_end
    li      $s3, 0

loop_col:
    bge     $s3, $s4, loop_row_end    # if (col >= N) break

    move    $a0, $s2
    move    $a1, $s3
    jal     neighbours          # call the neighbours function with row and column

    move    $s5, $v0            # save nn into $s5

    mul     $t0, $s2, $s4       # t0 = row*size of one row
    add     $t0, $t0, $s3       # offset = t0+t1

    la      $t1, newBoard       #Store the addres of newBoard in $t1
    add     $t1, $t0, $t1       # add the offset to the address
    move    $s6, $t1            # save the address into $s6 to access later

    la      $t2, board          #Store the addres of newBoard in $t1
    add     $t2, $t0, $t2       # add the offset to the address
    lb      $t2, ($t2)          # load the byte into $t2

#newboard[i][j] = decideCell (board[i][j], nn)
    move    $a0, $t2
    move    $a1, $s5

    add     $t0, $t0, $s6
    lw      $t1, ($t0)        # t0 = *(matrix+offset)


loop_col_end:
    addi $s3, $s3, 1       # col++
    j    loop_col

loop_row_end:
    addi $s2, $s2, 1       # row++
    j    loop_row

iterations_end:
    addi $s1, $s1, 1
    j    iterations

endloop:
    la    $a0, msg2
    li    $v0, 4
    syscall            # printf("=== After iteration %d ===")
    # CALL copybackandshow function here

    # clean up stack frame
    lw    $s1, -12($fp)   # restore $s1 value
    lw    $s0, -8($fp)    # restore $s0 value
    lw    $ra, -4($fp)    # restore $ra for return
    la    $sp, 4($fp)     # restore $sp (remove stack frame)
    lw    $fp, ($fp)      # restore $fp (remove stack frame)

    li    $v0, 0
    jr    $ra        # return 0

main__post:
	jr	$ra

	# Put your other functions here
