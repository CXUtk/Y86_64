	IRMOVD	$0x46464646,	RAX
	IRMOVB	$0x11,			RBX
	ADD		RBX,			RAX
	RMMOVD	RAX,			0x1064()
loop:
	PUSHI	$0x1064
	SYSCALL	0x10
	DEC		RBX
	JNE		loop