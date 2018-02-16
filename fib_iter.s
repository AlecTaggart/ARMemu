.global fib_iter
.func fib_iter


fib_iter:
	mov r1, #0 //f1
	mov r2, #1 //f2
	mov r3, #0 //fi
	cmp r0, #0
	moveq r3, #0
	beq finish
	cmp r0, #1
	moveq r3, #1
	beq finish
	mov r4, #2 //loop i variable
	mov r5, r0 // saving original n
	b loop
	
loop:
	cmp r4, r5
	bgt finish
	add r3, r1, r2
	mov r1, r2
	mov r2, r3
	add r4, r4, #1
	b loop



finish:
	mov r0, r3
	bx lr
