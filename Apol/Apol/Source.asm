include \masm32\include\masm32rt.inc
include main_code.inc
.code

main proc
	invoke load
	loopi:
		invoke update
		invoke draw
	jmp loopi
ret
main endp

end main