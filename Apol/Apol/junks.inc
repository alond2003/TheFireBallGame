include \masm32\include\user32.inc;;sound
includelib \masm32\lib\user32.lib

include \masm32\include\winmm.inc;;sound
includelib \masm32\lib\winmm.lib

.code
zukoDflt proc adrObj:dword
	mov ebx, adrObj
	ASSUME ebx: PTR Dmoot
	mov [ebx].drc.x, 0
	mov [ebx].drc.y,0
	ASSUME ebx: nothing
ret
zukoDflt endp

zukoActByKey proc adrObj: dword ,toKeyAdr: dword, toDrcAdr:dword, valuu:dword
	;;if pressed insert value
	mov ebx, adrObj
	add ebx, toKeyAdr
	invoke GetAsyncKeyState, [ebx]
	cmp eax,0
	jne PRESSED;jne
ret

	PRESSED:
	mov edx, valuu
	mov edi, adrObj
	add edi, toDrcAdr
	mov [edi], edx
ret
zukoActByKey endp

checkIfPointOutOfScreen proc Xp: dword, Yp:dword
;;result in ebx -> if out of screen, ebx = 1
	mov ebx, Xp
	cmp ebx,0
	jl OutScreen
	sub ebx, wwid
	;;ebx = Xp-wwid
	cmp ebx, 0
	jge OutScreen

	mov ebx, Yp
	cmp ebx,0
	jl OutScreen
	sub ebx, hhigh
	;;ebx = Yp-hhigh
	cmp ebx, 0
	jg OutScreen

	InScreen:
	mov ebx, 0
ret

	OutScreen:
	mov ebx, 1
ret
checkIfPointOutOfScreen endp

checkPoints proc adrObj:dword, adrArr:dword,sizeofArr:dword, color1:dword, color2: dword
;                                 offset of arr[0], max bytes in arr , bgr, bgr

;result in eax -> 0-colors don't show, 1- one or both colors show

	invoke drd_beginHdc;get handle
	mov esi, eax ; esi = handle
	mov ecx,0;ecx = idx
	dloop:
		mov ebx, adrObj

		mov edi, (Dmoot PTR [ebx]).pos.x
		mov edx, (Dmoot PTR [ebx]).pos.y

		push ecx
		push esi
	
		mov esi, ecx
		add esi, adrArr;esi=adrArr+ idx
		add edi, [esi];point[idx].x
		add edx, [esi + 4];point[idx].y
		;; edi = ctrlPt1X, edx = ctrlPt1Y
		pop esi;hdc = esi
		mov eax, esi ;hdc = eax

		invoke checkIfPointOutOfScreen, edi,edx
		cmp ebx, 1
		je SkipPoint

		invoke GetPixel, eax, edi,edx  ; eax = pixel
		cmp eax, color1
		je SHOW
		cmp eax, color2
		je SHOW



		pop ecx; ecx = idx
		add ecx,8
		cmp ecx, sizeofArr 
		jl dloop;if(ecx+8<sizeofArr) stay in loop

	NOTSHOW:
	mov eax,0
	invoke drd_endHdc, esi
ret

	SHOW:
	invoke drd_endHdc, esi
	mov eax,1
ret

	SkipPoint:
	pop ecx
	invoke drd_endHdc, esi
	mov eax, -1
ret
checkPoints endp

playSoundAsync proc adrPath:dword
	invoke PlaySound , adrPath,NULL, SND_ASYNC
ret
playSoundAsync endp

