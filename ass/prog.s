# COMP1521 19t2 ... Game of Life on a NxN grid
#
# Written by <<Arshdeep Singh Bhogal>>, June 2019

## Requires (from `boardX.s'):
# - N (word): board dimensions
# - board (byte[][]): initial board state
# - newBoard (byte[][]): next board state

    .data
msg5:   .asciiz " ===\n"
msg1:   .asciiz "# Iterations: "
msg2:   .asciiz "=== After iteration "
eol:    .asciiz "\n"
msg3:   .asciiz "."
msg4:   .asciiz "#"

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

# Code:

        #	-> [prologue]
        # Your main program code goes here.  Good luck!
        # Set up stack frame
        sw      $fp, -4($sp)            # push $fp onto stack
        la      $fp, -4($sp)            # set up $fp for this function
        sw      $ra, -4($fp)            # save return address
        sw      $s0, -8($fp)            # save $s0 to save iterations
        sw      $s1, -12($fp)           # save i for the iterations
        sw      $s2, -16($fp)           # save row
        sw      $s3, -20($fp)           # save column
        sw      $s4, -24($fp)           # save # columns and row i.e. N
        sw      $s5, -28($fp)           # save value nn
        sw      $s6, -32($fp)           # save pointer for new board
        addi    $sp, $sp, -36           # reset $sp to last pushed item

        # code for main()
        li      $s0, 0

        la      $a0, msg1
        li      $v0, 4
        syscall                         # printf("# Iterations: ")

        li      $v0, 5
        syscall                         # scanf("%d", into $v0)
        move    $s0, $v0                # save iterations (maxiters) into $s0

        lw      $s4, N                  # save # rows and columns
        li      $s1, 0                  # load value into i
        li      $s2, 0                  # row = 0
        li      $s3, 0                  # col = 0

# Loops until it reaches maxiters
iterations:
        bge     $s1, $s0, endloop

loop_row:
        bge     $s2, $s4, iterations_end    # if (row >= N) break
        li      $s3, 0

loop_col:
        bge     $s3, $s4, loop_row_end      # if (col >= N) break

        move    $a0, $s2
        move    $a1, $s3
        jal     neighbours              # call the neighbours function with row and column

        move    $s5, $v0                # save nn into $s5

        mul     $t0, $s2, $s4           # t0 = row*size of one row
        add     $t0, $t0, $s3           # offset = t0+t1

        la      $s6, newBoard           # store the address of newBoard in $s6
        add     $s6, $t0, $s6           # add the offset to the address and save the address into $s6 to access later

        la      $t1, board              # store the address of Board in $t1
        add     $t1, $t0, $t1           # add the offset to the address
        lb      $t1, ($t1)              # load the byte into $t1

        # newboard[i][j] = decideCell (board[i][j], nn)
        move    $a0, $t1
        move    $a1, $s5
        jal     decideCell              # call the function 

        sb      $v0, ($s6) 

loop_col_end:
        addi    $s3, $s3, 1             # col++
        j       loop_col

loop_row_end:
        addi    $s2, $s2, 1             # row++
        j       loop_row

iterations_end:
        addi    $s1, $s1, 1
        li      $s2, 0
        
        la      $a0, msg2
        li      $v0, 4
        syscall                         # printf("=== After iteration ")

        # prints the number of the current iteration 
        move    $a0, $s1
        li      $v0, 1
        syscall

        la      $a0, msg5
        li      $v0, 4
        syscall                         # printf(" ===\n")

        jal     copyBackAndShow

        j       iterations

endloop:
        #	-> [epilogue]
        # clean up stack frame
        lw      $s6, -32($fp)           # restore $s6 value
        lw      $s5, -28($fp)           # restore $s5 value
        lw      $s4, -24($fp)           # restore $s4 value
        lw      $s3, -20($fp)           # restore $s3 value
        lw      $s2, -16($fp)           # restore $s2 value     
        lw      $s1, -12($fp)           # restore $s1 value
        lw      $s0, -8($fp)            # restore $s0 value
        lw      $ra, -4($fp)            # restore $ra for return
        la      $sp, 4($fp)             # restore $sp (remove stack frame)
        lw      $fp, ($fp)              # restore $fp (remove stack frame)

        li      $v0, 0
        jr      $ra                     # return 0

main__post:
        jr	    $ra

        # Put your other functions here

decideCell:
        # setup stack frame
        sw	    $fp, -4($sp)	        # push $fp onto stack
        la	    $fp, -4($sp)	        # set up $fp for this function
        sw	    $ra, -4($fp)	        # save return address
        addi	$sp, $sp, -8	        # reset $sp to last pushed item

        # main code 
        li      $t0, 1
        li      $t1, 2 
        li      $t2, 3
        beq     $a0, $t0, cell_one 
        beq     $a1, $t2, cell_two 
        li      $v0, 0 
        j       end_cell 

cell_one:
        bge		$a1, $t1, check_two	    # if $a2 >= $t1 then check_two
        li      $v0, 0
        j		end_cell				# jump to end_cell

check_two:
        beq		$a1, $t1, check_two_ret 	# if $a2 == $t1 then check_two_ret
        beq		$a1, $t2, check_two_ret 	# if $a2 == $t2 then check_two_ret
        li      $v0, 0
        j		end_cell				# jump to end_cell

check_two_ret:
        li      $v0, 1
        j		end_cell				# jump to end_cell

cell_two:
        li      $v0, 1

end_cell:
        # clean up stack frame
        lw  	$ra, -4($fp)	        # restore $ra for return
        la  	$sp, 4($fp)	            # restore $sp (remove stack frame)
        lw  	$fp, ($fp)	            # restore $fp (remove stack frame)
        jr  	$ra		                # return


neighbours:
        # set up stack frame 
        sw	    $fp, -4($sp)	        # push $fp onto stack
        la  	$fp, -4($sp)	        # set up $fp for this function
        sw  	$ra, -4($fp)	        # save return address
        sw  	$s0, -8($fp)	        # save $s0 
        sw      $s1, -12($fp)           # save # rows and columns 
        sw      $s2, -16($fp)           # save N-1
        sw      $s3, -20($fp)
        addi	$sp, $sp, -24	        # reset $sp to last pushed item

        # main code 
        li      $t0, 0                  # use $t0 as nn
        li      $t1, -1                 # use $t1 as x
        li      $t3, 0                  
        li      $s0, 1
        lw      $s1, N                  # save number of rows and columns 
        addi    $s2, $s1, -1            # save N - 1
        la      $s3, board
        

row_loop:
        bgt		$t1, $s0, end_loop	    # if $t1 > $s0 then end_loop 
        li      $t2, -1                 # use $t2 as y

col_loop:
        bgt		$t2, $s0, end_row	    # if $t1 > $s0 then end_row 
        add     $t4, $a0, $t1

        # if (i + x < 0 || i + x > N - 1)
        bltz    $t4, end_col    
        bgt     $t4, $s2, end_col

        add     $t5, $a1, $t2
        # if (j + y < 0 || j + y > N - 1)
        bltz    $t5, end_col    
        bgt     $t5, $s2, end_col

        # if (x == 0 && y == 0)
        bnez    $t1, check_board
        bnez    $t2, check_board
        j       end_col

check_board:
        mul     $t6, $t4, $s1           # t6 = row*size of one row
        add     $t6, $t6, $t5           # offset = t6+t5
        add     $t6, $t6, $s3           # add offset to the address of the board 
        lb      $t6, ($t6)

        bne     $t6, $s0, end_col
        addi    $t0, $t0, 1             # add 1 to nn
        

end_col: 
        addi    $t2, $t2, 1 
        j		col_loop

end_row:
        addi    $t1, $t1, 1 
        j		row_loop				# jump to row_loop
        
        
end_loop:
        move    $v0, $t0

        # clean up stack frame
        lw		$s3, -20($fp)		    # restore value for $s3 
        lw      $s2, -16($fp)           # restore value for $s2 
        lw      $s1, -12($fp)           # restore value for $s1 
        lw	    $s0, -8($fp)	        # restore $s0 value
        lw	    $ra, -4($fp)	        # restore $ra for return
        la	    $sp, 4($fp)	            # restore $sp (remove stack frame)
        lw	    $fp, ($fp)              # restore $fp (remove stack frame)

        jr	    $ra		                # return 

copyBackAndShow:
        # setup stack frame
        sw	    $fp, -4($sp)	        # push $fp onto stack
        la	    $fp, -4($sp)	        # set up $fp for this function
        sw	    $ra, -4($fp)	        # save return address
        sw  	$s0, -8($fp)	        # save $s0 
        sw      $s1, -12($fp)           # save # rows and columns 
        sw      $s2, -16($fp)           # save N-1          
        addi	$sp, $sp, -20	        # reset $sp to last pushed item

        # main code 
        li      $t0, 0                  # load i 
        lw      $t2, N

row:
        bge     $t0, $t2, end
        li      $t1, 0                  # load j

col:
        bge     $t1, $t2, row_end
        
        mul     $t4, $t0, $t2
        add     $t4, $t4, $t1           # $t4 = offset 

        # newBoard[i][j]
        la      $t3, newBoard
        add     $t3, $t3, $t4
        lb      $t3, ($t3)

        # board[i][j]
        la      $t5, board
        add     $t5, $t5, $t4
        sb      $t3, ($t5)
        lb      $t5, ($t5)

        # Check if (board[i][j] == 0)
        beqz $t5, print_dot
        la      $a0, msg4
        li      $v0, 4
        syscall
        j col_end

print_dot:
        la      $a0, msg3
        li      $v0, 4
        syscall

col_end:
        addi	$t1, $t1, 1			    # $t1 = $t1 + 1
        j		col				        # jump to col

row_end:
        la      $a0, eol
        li      $v0, 4
        syscall
        addi	$t0, $t0, 1			    # $t0 = $t0 + 1
        j       row

end:
        # clean up stack frame
        lw      $s2, -16($fp)           # restore value for $s2 
        lw      $s1, -12($fp)           # restore value for $s1 
        lw	    $s0, -8($fp)	        # restore $s0 value
        lw  	$ra, -4($fp)	        # restore $ra for return
        la  	$sp, 4($fp)	            # restore $sp (remove stack frame)
        lw  	$fp, ($fp)	            # restore $fp (remove stack frame)
        jr  	$ra		                # return


