.global find_max
.func find_max

find_max:
	mov r2, #0 //i
	mov r3, r0 //array memory address
	ldr r4, [r3] //max
	loop:
		cmp r2, r1
		bge finish
		ldr r5, [r3] //storing array value inside r5
		cmp r5, r4
		movgt r4, r5
		add r2, r2, #1
		add r3, r3, #4
		b loop
	finish:
	mov r0, r4
	bx lr
