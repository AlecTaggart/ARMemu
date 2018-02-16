.global sum_a
.func sum_a
 
sum_a:
	mov r2, #0
	ldrb r3, [r0, r2]
	ldrb r4, [r1, r2]
	cmp r3, r4
	mov r0, #69
	bx lr
