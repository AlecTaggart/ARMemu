.global find_str
.func find_str

find_str:
	mov r7, #0
	mov r2, #0 //i
	mov r3, #0 //j
	mov r4, #0 //i + j

loop1:
	ldrb r5, [r0, r2] // get source at i
	cmp r5, #0
	beq notFound
	ldrb r6, [r1, r7] //get first char of sub 
	cmp r5, r6
	moveq r3, #1
	//branch back
	addne r2, r2, #1
	bne loop1
	

loop2:
	add r4, r2, r3 
	ldrb r5, [r0, r4] //get i+j from source
	ldrb r6, [r1, r3] //get j from sub
	cmp r6, #0
	beq found
	cmp r5, r6
	addeq r3, r3, #1
	beq loop2
	//branch back
	addne r2, r2, #1
	bne loop1


notFound:
	mov r0, #0
	sub r0, r0, #1
	bx lr

found:
	mov r0, r2
	bx lr



