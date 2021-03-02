.code


addVal proc adrArr: dword, ArrSize:dword, xVal:dword, yVal:dword
	mov eax,0
	ddlop:
		;;X
		mov ecx, adrArr
		add ecx, eax
		mov edx, xVal
		add [ecx], edx

		;;Y
		add ecx, 4
		mov edx, yVal
		add [ecx], edx
		
		;;check if done
		add eax, 8
		cmp eax, ArrSize
		jne ddlop
ret
addVal endp

SearchFallLoc proc adrObj: dword
;;result in DrcFall

	loopy:
		;;check if right is clear
		invoke checkPoints, adrObj, offset FallPointR,8, groundColor, noColor
		cmp eax,0
		je RIGHT

		;;check if left is clear
		invoke checkPoints , adrObj, offset FallPointL,8, groundColor, noColor
		cmp eax,0
		je LEFT

		;;distance points
		invoke addVal, offset FallPointR,8, 1, 0
		invoke addVal, offset FallPointL,8, -1, 0
	jmp loopy
ret

	RIGHT:
	mov edx, 1
	mov DrcFall, edx
jmp ResetArr

	LEFT:
	mov edx, -1
	mov DrcFall, edx
jmp ResetArr

	ResetArr:
	;;R
	mov eax, ctrlPts[0].x
	mov ebx, ctrlPts[0].y
	mov FallPointR[0].x, eax
	mov FallPointR[0].y, ebx

	;;L
	mov eax, ctrlPts[8].x
	mov ebx, ctrlPts[8].y
	mov FallPointL[0].x, eax
	mov FallPointL[0].y, ebx
ret

SearchFallLoc endp

findNearFloorY proc adrObj: dword
;;result in floorY

	dloopy:
		;;check if this is the floor height
		invoke checkPoints , adrObj, offset floorPts,16, groundColor, errorColor
		cmp eax,1
		je FloorFound
		;;distance points
		invoke addVal, offset floorPts,16, 0,1
	jmp dloopy
ret

	FloorFound:
	mov eax, adrObj
	mov edx, floorPts[0].y
	add edx, (Dmoot PTR [eax]).pos.y
	mov floorY, edx
jmp ResetArr

	ResetArr:
	
	mov eax, ctrlPts[0].x
	mov ebx, ctrlPts[0].y
	mov floorPts[0].x, eax
	mov floorPts[0].y, ebx

	mov eax, ctrlPts[8].x
	mov ebx, ctrlPts[8].y
	mov floorPts[8].x, eax
	mov floorPts[8].y, ebx
ret

findNearFloorY endp

doBot proc adrObj:dword, adrTar:dword
	mov eax, adrObj
	mov ebx, adrTar
		
	;;check if same Y
	mov ecx, (Dmoot PTR [eax]).pos.y
	mov edx, (Dmoot PTR [ebx]).pos.y
	cmp edx,ecx
	je SAMEY

	

	;;check if should copy moves
	mov edx,1
	cmp COPYMOVES,edx
	je COPYMO

	mov edx, -2
	cmp tryTo, edx
	je SAMEY
	;;if deadY=0 then don't check if tries to fall or 
	cmp tryTo, 0
	je SkipCheck

	;;check if tries to fall
	mov edx, -1
	cmp tryTo, edx
	jne Skiped

	;;check if succeeded or still trying
	TriedToFall:
	mov ecx, (Dmoot PTR [eax]).pos.y
	sub ecx, deadY
	;;ecx = posY - deadY
	cmp ecx, 1
	jg SucceededToFall
	jmp TryToFall

	Skiped:
	SkipCheck:

	;;check if should fall/jump
	mov ecx, (Dmoot PTR [eax]).pos.y
	sub ecx, (Dmoot PTR [ebx]).pos.y
	;;ecx = BotY-TarY
	cmp ecx,0
	jl SearchFall
	;;jg TryToJump
ret

	Reg:
	mov eax, adrObj
	mov ebx, adrTar
	pop ecx
jmp ecx
;;------------------------------------------------------------------------
SAMEY:
	;;check if R/L
	mov ecx, (Dmoot PTR [eax]).pos.x
	sub ecx, (Dmoot PTR [ebx]).pos.x
	;;ecx = ObjX-TarX
	cmp ecx,0
	jg LEFT
	jl RIGHT

	;;meet
	MEET1:
	mov COPYMOVES, 1
ret
	RIGHT:
	mov (Dmoot PTR [eax]).drc.x, 1
ret

	LEFT:
	mov (Dmoot PTR [eax]).drc.x, -1
ret
;----------------------------------------------------------

COPYMO:
	mov ecx, (Dmoot PTR [eax]).pos.y
	sub ecx, (Dmoot PTR [ebx]).pos.y
	;;ecx = BotY-TarY
	cmp ecx,0
	jg JUMPAFTER

	RETJUMPAFTER:
	call Reg
jmp SAMEY
ret
	
	JUMPAFTER:
	invoke setAbleToJump, adrObj
	cmp eax,0
	je RETJUMPAFTER
	call Reg
	mov edx, -1
	mov (Dmoot PTR [eax]).drc.y,edx
jmp RETJUMPAFTER

;--------------------------------------------------------------------
	TryToJump:
	;;check if allready jumping
	mov edx ,2
	cmp (Dmoot PTR [eax]).actionOn, edx
	je SAMEY

	invoke setAbleToJump, adrObj
	cmp eax,0
	je CANNOTJUMP
	;pusha
	;;;check if can jump
	;invoke checkPoints , adrObj, offset ctrlPts,16, groundColor, errorColor
	;cmp eax, 0 ;; if 0 then falling
	;jne SAMEY
	;popa
	;;start to Jump
	call Reg

	;;check points above head
	mov ebx, offset ARRpoints
	mov esi, SIZEOF ARRpoints
	invoke checkPoints, adrObj,ebx,esi, groundColor, errorColor
	cmp eax,0
	jne TRYTOBEABLETOJUMP

	call Reg

	invoke setAbleToJump, adrObj
	cmp eax, 0
	je CANNOTJUMP
	call Reg
	mov edx, -1
	mov (Dmoot PTR [eax]).drc.y, edx
	;;reset jumpCounterForBot
	mov jumpCounterForBot,0

	CANNOTJUMP:
	call Reg
jmp SAMEY
;
;TRYTOSAMEY:
;	mov edx, (Dmoot PTR [eax]).pos.y
;	mov deadY, edx
;	;;right
;	mov edx,-1
;	mov (Dmoot PTR [eax]).drc.x, edx
;	cmp (Dmoot PTR [eax]).pos.x, 0
;	je TurnAway
;	mov edx,1
;	mov SkipAll, edx
;	mov edx, 1 
;	mov ecx, ShouldTurnAway
;	cmp ecx, edx
;	je TurnAway
;ret
;
;	TurnAway:
;	mov edx ,1
;	mov ShouldTurnAway, edx
;	mov edx,1
;	mov (Dmoot PTR [eax]).drc.x, edx
;	mov edx, 1
;	mov SkipAll, edx 
;ret

	TRYTOBEABLETOJUMP:
	call Reg

	mov edx, (Dmoot PTR [eax]).pos.y
	mov deadY, edx
	;;right
	mov edx,-1
	mov (Dmoot PTR [eax]).drc.x, edx
	cmp (Dmoot PTR [eax]).pos.x, 0
	je TurnAwayFORJUMP
	mov edx,1
	mov gotoTryJump, edx
	mov edx, 1 
	mov ecx, ShouldTurnAway
	cmp ecx, edx
	je TurnAwayFORJUMP

	mov ebx, offset ARRpoints
	mov esi, SIZEOF ARRpoints
	invoke checkPoints, adrObj,ebx,esi, groundColor, errorColor
	cmp eax, 0
	je FinishTryClearJump
	;;gotoTryJump
ret
	
	FinishTryClearJump:
	mov eax, adrObj
	mov ebx, adrTar

	cmp counterKeepGoing,0
	jne NotInputMem

	mov ecx, (Dmoot PTR [eax]).drc.x
	mov memDrc, ecx

	NotInputMem:
	mov ecx, memDrc
	mov (Dmoot PTR [eax]).drc.x, ecx
	inc counterKeepGoing
	mov edx, 20
	cmp counterKeepGoing, edx
	jge REALLYFinish
ret

	REALLYFinish:
	mov gotoTryJump,0
	mov counterKeepGoing,0
ret

	TurnAwayFORJUMP:
	mov edx ,1
	mov ShouldTurnAway, edx
	mov edx,1
	mov (Dmoot PTR [eax]).drc.x, edx
	mov edx, 1
	mov gotoTryJump, edx 
	mov ebx, offset ARRpoints
	mov esi, SIZEOF ARRpoints
	invoke checkPoints, adrObj,ebx,esi, groundColor, errorColor
	cmp eax, 0
	je FinishTryClearJump
ret

	SearchFall:
	invoke SearchFallLoc, eax
	;;make this code run until start falling
	call Reg
	mov edx, (Dmoot PTR [eax]).pos.y
	mov deadY, edx
	;;same
	mov edx, -1
	mov tryTo, edx
jmp TryToFall

	TryToFall:
	mov edx, DrcFall
	mov (Dmoot PTR [eax]).drc.x, edx
ret

	SucceededToFall:
	invoke findNearFloorY, adrObj
	;;check if tar is above, under or on
	call Reg

	mov ecx, (Dmoot PTR [ebx]).pos.y
	sub ecx, floorY
	add ecx, zukoH
	;; ecx = tar.y - BotY(if it was on floor)
	cmp ecx, 0
	jg FallPlan

	mov edx, -2
	mov tryTo, edx
jmp SAMEY
	
	FallPlan:
	mov ecx, floorY
	sub ecx, zukoH
	mov edx, (Dmoot PTR [eax]).pos.y
	push edx
	mov (Dmoot PTR [eax]).pos.y, ecx
	
	invoke SearchFallLoc, eax
	pop edx

	call Reg
	mov (Dmoot PTR [eax]).pos.y, edx

	mov ecx, floorY
	sub ecx,zukoH
	mov deadY, ecx
	mov edx, -1
	mov tryTo, edx
jmp TryToFall

doBot endp

;old Version
;doBot proc adrObj:dword, adrTar:dword
;	mov eax, adrObj
;	mov ebx, adrTar
;		
;	;;check if same Y
;	mov ecx, (Dmoot PTR [eax]).pos.y
;	mov edx, (Dmoot PTR [ebx]).pos.y
;	cmp edx,ecx
;	je SAMEY
;
;	;;check if should copy moves
;	mov edx,1
;	cmp COPYMOVES,edx
;	je COPYMO
;
;	;;check if tries to fall
;	mov ecx, (Dmoot PTR [eax]).pos.y
;	sub ecx, deadY
;	;;ecx = posY - deadY
;	mov edx, -1
;	cmp ecx,edx
;	jl SKIPSOME
;	mov edx,1
;	mov ecx, SkipAll
;	cmp ecx, edx
;	je TRYTOSAMEY
;
;	SKIPSOME:
;	mov edx, 1
;	cmp gotoTryJump, edx
;	je TRYTOBEABLETOJUMP
;
;	;;check if should fall/jump
;	mov ecx, (Dmoot PTR [eax]).pos.y
;	sub ecx, (Dmoot PTR [ebx]).pos.y
;	;;ecx = BotY-TarY
;	cmp ecx,0
;	jl TryToFall
;	jg TryToJump
;ret
;;;------------------------------------------------------------------------
;SAMEY:
;	;;check if R/L
;	mov ecx, (Dmoot PTR [eax]).pos.x
;	sub ecx, (Dmoot PTR [ebx]).pos.x
;	;;ecx = ObjX-TarX
;	cmp ecx,0
;	jg LEFT
;	jl RIGHT
;
;	;;meet
;	MEET1:
;	mov COPYMOVES, 1
;ret
;	RIGHT:
;	mov (Dmoot PTR [eax]).drc.x, 1
;ret
;
;	LEFT:
;	mov (Dmoot PTR [eax]).drc.x, -1
;ret
;;----------------------------------------------------------
;
;COPYMO:
;	mov ecx, (Dmoot PTR [eax]).pos.y
;	sub ecx, (Dmoot PTR [ebx]).pos.y
;	;;ecx = BotY-TarY
;	cmp ecx,0
;	jg JUMPAFTER
;
;	RETJUMPAFTER:
;	mov eax, adrObj
;	mov ebx, adrTar
;jmp SAMEY
;ret
;	
;	JUMPAFTER:
;	invoke setAbleToJump, adrObj
;	cmp eax,0
;	je RETJUMPAFTER
;	mov eax, adrObj
;	mov ebx, adrTar
;	mov edx, -1
;	mov (Dmoot PTR [eax]).drc.y,edx
;jmp RETJUMPAFTER
;
;;--------------------------------------------------------------------
;	TryToJump:
;	;;check if allready jumping
;	mov edx ,2
;	cmp (Dmoot PTR [eax]).actionOn, edx
;	je SAMEY
;
;	invoke setAbleToJump, adrObj
;	cmp eax,0
;	je CANNOTJUMP
;	;pusha
;	;;;check if can jump
;	;invoke checkPoints , adrObj, offset ctrlPts,16, groundColor, errorColor
;	;cmp eax, 0 ;; if 0 then falling
;	;jne SAMEY
;	;popa
;	;;start to Jump
;	mov eax, adrObj
;	mov ebx, adrTar
;
;	;;check points above head
;	mov ebx, offset ARRpoints
;	mov esi, SIZEOF ARRpoints
;	invoke checkPoints, adrObj,ebx,esi, groundColor, errorColor
;	cmp eax,0
;	jne TRYTOBEABLETOJUMP
;
;	mov eax, adrObj
;	mov ebx, adrTar
;
;	invoke setAbleToJump, adrObj
;	cmp eax, 0
;	je CANNOTJUMP
;	mov eax, adrObj
;	mov ebx, adrTar
;	mov edx, -1
;	mov (Dmoot PTR [eax]).drc.y, edx
;	;;reset jumpCounterForBot
;	mov jumpCounterForBot,0
;
;	CANNOTJUMP:
;	mov eax, adrObj
;	mov ebx, adrTar
;jmp SAMEY
;;
;;TRYTOSAMEY:
;;	mov edx, (Dmoot PTR [eax]).pos.y
;;	mov deadY, edx
;;	;;right
;;	mov edx,-1
;;	mov (Dmoot PTR [eax]).drc.x, edx
;;	cmp (Dmoot PTR [eax]).pos.x, 0
;;	je TurnAway
;;	mov edx,1
;;	mov SkipAll, edx
;;	mov edx, 1 
;;	mov ecx, ShouldTurnAway
;;	cmp ecx, edx
;;	je TurnAway
;;ret
;;
;;	TurnAway:
;;	mov edx ,1
;;	mov ShouldTurnAway, edx
;;	mov edx,1
;;	mov (Dmoot PTR [eax]).drc.x, edx
;;	mov edx, 1
;;	mov SkipAll, edx 
;;ret
;
;	TRYTOBEABLETOJUMP:
;	mov eax, adrObj
;	mov ebx, adrTar
;
;	mov edx, (Dmoot PTR [eax]).pos.y
;	mov deadY, edx
;	;;right
;	mov edx,-1
;	mov (Dmoot PTR [eax]).drc.x, edx
;	cmp (Dmoot PTR [eax]).pos.x, 0
;	je TurnAwayFORJUMP
;	mov edx,1
;	mov gotoTryJump, edx
;	mov edx, 1 
;	mov ecx, ShouldTurnAway
;	cmp ecx, edx
;	je TurnAwayFORJUMP
;
;	mov ebx, offset ARRpoints
;	mov esi, SIZEOF ARRpoints
;	invoke checkPoints, adrObj,ebx,esi, groundColor, errorColor
;	cmp eax, 0
;	je FinishTryClearJump
;	;;gotoTryJump
;ret
;	
;	FinishTryClearJump:
;	mov eax, adrObj
;	mov ebx, adrTar
;
;	cmp counterKeepGoing,0
;	jne NotInputMem
;
;	mov ecx, (Dmoot PTR [eax]).drc.x
;	mov memDrc, ecx
;
;	NotInputMem:
;	mov ecx, memDrc
;	mov (Dmoot PTR [eax]).drc.x, ecx
;	inc counterKeepGoing
;	mov edx, 20
;	cmp counterKeepGoing, edx
;	jge REALLYFinish
;ret
;
;	REALLYFinish:
;	mov gotoTryJump,0
;	mov counterKeepGoing,0
;ret
;
;	TurnAwayFORJUMP:
;	mov edx ,1
;	mov ShouldTurnAway, edx
;	mov edx,1
;	mov (Dmoot PTR [eax]).drc.x, edx
;	mov edx, 1
;	mov gotoTryJump, edx 
;	mov ebx, offset ARRpoints
;	mov esi, SIZEOF ARRpoints
;	invoke checkPoints, adrObj,ebx,esi, groundColor, errorColor
;	cmp eax, 0
;	je FinishTryClearJump
;ret
;
;	TryToFall:
;	mov ecx, (Dmoot PTR [eax]).pos.x
;	sub ecx, (Dmoot PTR [ebx]).pos.x
;	;;ecx = ObjX-TarX
;	cmp ecx,0
;	jg LEFT
;	jl RIGHT
;	;;meet X -> check same Y
;	mov ecx, (Dmoot PTR [eax]).pos.y
;	sub ecx, (Dmoot PTR [ebx]).pos.y
;	;;ecx = BotY-TarY
;	cmp ecx,0
;	jne TRYTOSAMEY
;
;	MEET:
;ret
;
;	TRYTOSAMEY:
;	mov edx, (Dmoot PTR [eax]).pos.y
;	mov deadY, edx
;	;;right
;	mov edx,-1
;	mov (Dmoot PTR [eax]).drc.x, edx
;	cmp (Dmoot PTR [eax]).pos.x, 0
;	je TurnAway
;	mov edx,1
;	mov SkipAll, edx
;	mov edx, 1 
;	mov ecx, ShouldTurnAway
;	cmp ecx, edx
;	je TurnAway
;ret
;
;	TurnAway:
;	mov edx ,1
;	mov ShouldTurnAway, edx
;	mov edx,1
;	mov (Dmoot PTR [eax]).drc.x, edx
;	mov edx, 1
;	mov SkipAll, edx 
;ret
;	
;
;
;doBot endp

;proc test adrObj: dword, adrPoint: dword, val:dword
;	mov eax, adrObj
;	
;	;;X
;	mov ebx, (Dmoot PTR [eax]).pos.x
;	mov ecx, adrPoint
;	mov edx, val
;	add [ecx], ebx
;	add [ecx], edx
;
;	;;Y
;	mov ebx, (Dmoot PTR [eax]).pos.y
;	add ecx, 4
;	mov edx, val
;	add [ecx], ebx
;	add [ecx], edx
;ret
;test endp