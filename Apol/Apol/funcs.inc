.code

setAbleToJump proc adrObj: dword
;;result in eax: 1-can jump, 0- cannot jump
;; cannot jump if falling or jumping
	mov ebx, adrObj
	mov ecx, (Dmoot PTR [ebx]).actionOn
	cmp ecx, 3 ;fall
	je CANNOT
	cmp ecx, 2 ;;jump
	je CANNOT

	;can jump
	mov eax,1
ret

	CANNOT:
	mov eax,0
ret

setAbleToJump endp

setAbleToShoot proc adrObj:dword
;;result in eax: 1-can jump, 0- cannot jump
	mov ebx, adrObj
	mov ecx, (Dmoot PTR [ebx]).actionOn
	cmp ecx, 3 ;fall
	je CANNOT
	cmp ecx, 2 ;;jump
	je CANNOT
	cmp ecx, 4;shooting
	je CANNOTSH

	CAN:
	mov eax, 1
ret

	CANNOT:
	mov eax,0
ret

CANNOTSH:
	mov eax,0
ret
setAbleToShoot endp

createFireBall proc adrObj:dword
	invoke playSoundAsync, offset hd.fireBall.fireSound

	;zuko1.ball = fireBall
	mov ebx, adrObj
	ASSUME ebx: PTR Dmoot
	mov edx, [ebx].ball.fireAnim.ogCount
	mov [ebx].ball.fireAnim.count, edx
	mov [ebx].ball.picIdx, 0

	mov eax, [ebx].pos.x
	mov ecx, [ebx].pos.y

	mov edx, [ebx].facing
	cmp edx,1
	je LeftPt
	cmp edx,0
	je RightPt
	
	retPt:
	mov [ebx].ball.pos.x, eax
	mov [ebx].ball.pos.y, ecx
	
	mov edx, [ebx].facing
	cmp edx,1
	je Left
	cmp edx,0
	je Right
	
	RET_DRC:
	mov [ebx].ball.drcX, edx 

	mov edx,1
	mov [ebx].ball.isShowing, edx
	ASSUME ebx: nothing
ret

	RightPt:
	add eax, createBallR.x
	add ecx, createBallR.y
jmp retPt

	LeftPt:
	add eax, createBallL.x
	add ecx, createBallL.y
jmp retPt

	Right:
	mov edx,2
jmp RET_DRC

	Left:
	mov edx,-2
jmp RET_DRC
createFireBall endp

zukoResetAction proc adrObj: dword, act: dword
	mov eax, adrObj
	ASSUME eax: PTR Dmoot
	mov [eax].picIdx, 0;reset PicIdx
	mov ebx,act

	cmp ebx, 0
	je ResetStand
	cmp ebx,1
	je ResetRunAnim
	cmp ebx, 3
	je ResetFallAnim
	cmp ebx,2
	je ResetJumpAnim
	cmp ebx, 4
	je ResetShootAnim

	RETURN:
	ASSUME eax: nothing
ret

	ResetStand:
	ASSUME eax: PTR Dmoot
	mov edx, [eax].standAnim.ogCount
	mov [eax].standAnim.count, edx
jmp RETURN

	ResetRunAnim:
	ASSUME eax: PTR Dmoot
	mov edx, [eax].runAnim.ogCount
	mov [eax].runAnim.count, edx
jmp RETURN

	ResetFallAnim:
	ASSUME eax: PTR Dmoot
	mov edx, [eax].fallAnim.ogCount
	mov [eax].fallAnim.count,edx
jmp RETURN

	ResetJumpAnim:
	ASSUME eax: PTR Dmoot
	mov edx, [eax].jumpAnim.ogCount
	mov [eax].jumpAnim.count, edx
jmp RETURN

	ResetShootAnim:
	ASSUME eax: PTR Dmoot
	mov edx, [eax].shootAnim.ogCount
	mov [eax].shootAnim.count, edx
jmp RETURN
zukoResetAction endp

shouldKeepShooting proc adrObj:dword
	mov ebx,adrObj
	mov edx, START_FIREBALL
	cmp (Dmoot PTR [ebx]).shootAnim.count, edx
	je CREATEFIREBALL
	cmp (Dmoot PTR [ebx]).shootAnim.count, 0
	je STOP_SHOOTING
ret

	STOP_SHOOTING:
	invoke zukoResetAction,adrObj, 4
	mov ebx, adrObj
	mov (Dmoot PTR [ebx]).actionOn,0
ret

	CREATEFIREBALL:
	invoke createFireBall, adrObj
ret
shouldKeepShooting endp

keyboard proc adrObj: dword
	mov ebx, adrObj

	mov edx, 4
	cmp (Dmoot PTR [ebx]).actionOn, edx
	je WHILE_SHOOT

	invoke zukoActByKey,adrObj,Dmoot.keys.left,Dmoot.drc.x, -1
	invoke zukoActByKey,adrObj,Dmoot.keys.right,Dmoot.drc.x, 1

	mov ebx, adrObj
	mov edx, 1
	cmp (Dmoot PTR [ebx]).drc.x, edx
	je RRR
	RetRRR:

	invoke setAbleToJump, adrObj
	cmp eax,0
	je CANNOTJUMP

	;;can jump
	invoke zukoActByKey, adrObj, Dmoot.keys.up,Dmoot.drc.y, -1

	CANNOTJUMP:
	;;empty jump
	mov edx,ebx
	add edx, Dmoot.keys.up
	invoke zukoActByKey, 0,edx, offset emptyDword, 0

	invoke setAbleToShoot, adrObj
	cmp eax,0
	je CANNOTSHOOT

	;;can shoot
	invoke zukoActByKey, adrObj,Dmoot.keys.shoot,Dmoot.actionOn, 4
	mov edx, 4
	cmp (Dmoot PTR [ebx]).actionOn, edx
	je FIRST_SHOOT

	CANNOTSHOOT:
	;;empty shoot
	mov edx,ebx
	add edx, Dmoot.keys.shoot
	invoke zukoActByKey, 0,edx, offset emptyDword, 0
ret

	FIRST_SHOOT:
ret

	WHILE_SHOOT:
	invoke shouldKeepShooting, ebx
ret

	RRR:
ret
keyboard endp

setJumpState proc adrObj: dword, jmpStt: dword
	mov ebx, adrObj
	mov ecx, jmpStt
	mov (Dmoot PTR [ebx]).jumpState, ecx
ret
setJumpState endp

applyJump proc adrObj: dword
	mov ebx, adrObj
	mov edx, PRE_JUMP
	cmp (Dmoot PTR [ebx]).jumpState, edx
	je FREEZE
	NO_MORE_FREEZE:
	;;change drc
	mov (Dmoot PTR [ebx]).drc.y, -1
ret

	FREEZE:
	cmp (Dmoot PTR [ebx]).jumpFreeze, 0
	je PastFreeze
	;;jumpFreeze--
	mov edx,1
	sub (Dmoot PTR [ebx]).jumpFreeze, edx
ret

	PastFreeze:
	;reset jump freeze
	mov edx, (Dmoot PTR [ebx]).ogJumpFreeze
	mov (Dmoot PTR [ebx]).jumpFreeze, edx
	;change jumpState
	mov edx, WHILE_JUMP
	mov (Dmoot PTR [ebx]).jumpState, edx
	jmp NO_MORE_FREEZE
applyJump endp

shouldKeepJumping proc adrObj:dword
;; result in actionOn
	mov ecx, adrObj
	
	mov ebx, (Dmoot PTR [ecx]).adrUpPts
	mov esi, (Dmoot PTR [ecx]).maxBytsUpPts
	invoke checkPoints, adrObj,ebx,esi, groundColor, errorColor
	
	mov ecx,adrObj
	ASSUME ecx: PTR Dmoot
	cmp eax,1;colors showing
	mov edi, TIKRA
	je StopJump
	
	;;check jumpH
	cmp [ecx].jumpH, 0
	mov edi, JUMPHCAP
	jle StopJump

	;;check out of frame
	mov edx , -10
	cmp [ecx].pos.y, edx
	mov edi, ENDSCREEN
	jle StopJump

	;;if ok then actionOn = 2, jumpState = 1,
	mov edx, WHILE_JUMP
	mov [ecx].jumpState, edx

	ASSUME ecx: NOTHING
ret

	StopJump:
	mov ecx,adrObj
	ASSUME ecx: PTR Dmoot
	mov [ecx].actionOn, 3 ;;change actionOn
	mov ebx, [ecx].ogJumpH;;reset jumpH
	mov [ecx].jumpH, ebx
	mov [ecx].lastStopJumpReason, edi
	ASSUME ecx: NOTHING
ret
shouldKeepJumping endp

gravityC proc adrObj: dword
	mov eax, adrObj
	mov ebx, 2
	cmp (Dmoot PTR [eax]).actionOn, ebx
	je JUMPING

	mov ebx,-1
	cmp (Dmoot PTR [eax]).drc.y,ebx
	je STARTJUMPING

	ASSUME eax: nothing
	invoke checkPoints , adrObj, offset ctrlPts,16, groundColor, errorColor
	cmp eax,0
	je FALLING
ret

	JUMPING:
	mov edx, PRE_JUMP
	cmp (Dmoot PTR [eax]).jumpState, edx
	jne WHILEJUMPING
	;;not moving at all
	invoke applyJump, adrObj
ret
	WHILEJUMPING:
	;;skip gravity
	invoke shouldKeepJumping, adrObj
	mov eax, adrObj
	cmp (Dmoot PTR [eax]).actionOn,2 ;;if not jumping then apply gravity
	jne FALLING
	;jumping 
	invoke applyJump, adrObj
ret

	STARTJUMPING:
	mov ebx,2 
	mov (Dmoot PTR [eax]).actionOn, ebx
	;;apply jump
	invoke setJumpState, adrObj, PRE_JUMP
	invoke applyJump, adrObj
	;;skip gravity
ret
	FALLING:
	mov eax, adrObj
	mov edx, 1
	mov (Dmoot PTR [eax]).drc.y, edx 
ret
gravityC endp

zukoSetAction proc adrObj: dword
	mov edi, adrObj
	mov ecx, (Dmoot PTR [edi]).actionOn ; ecx = lastAction
	mov esi, (Dmoot PTR [edi]).facing ; esi = lastFacing
	
	cmp ecx, 4
	je SHOOT

	mov ebx, (Dmoot PTR [edi]).drc.x
	cmp ebx,1
	je RIGHT
	cmp ebx,-1
	je LEFT
	ReturnX:

	mov edx, (Dmoot PTR [edi]).drc.y
	cmp edx, 1
	je FALLING
	cmp edx,-1
	je JUMPING
	cmp ecx, 2
	je JUMPING
	ReturnY:

	cmp ebx, 0
	je NoX
	ReturnNoX:

	cmp ecx, (Dmoot PTR [edi]).actionOn
	jne ChangedAction

	;sameAction
	cmp esi, (Dmoot PTR [edi]).facing
	jne ChangedFacing
ret

	RIGHT:
	mov (Dmoot PTR [edi]).facing, 0
	mov (Dmoot PTR [edi]).actionOn,1
	jmp ReturnX

	LEFT:
	mov (Dmoot PTR [edi]).facing,1
	mov (Dmoot PTR [edi]).actionOn,1
	jmp ReturnX

	FALLING:
	mov (Dmoot PTR [edi]).actionOn,3
	jmp ReturnY

	JUMPING:
	mov (Dmoot PTR [edi]).actionOn, 2
	mov eax, PRE_JUMP
	cmp (Dmoot PTR [edi]).jumpState, eax
	je PreJump
	;invoke applyJump, offset zuko1
	jmp ReturnY

	PreJump:
	;;freezing
ret

	NoX:
	cmp edx,0
	je Standing
	jmp ReturnNoX

	Standing:
	mov (Dmoot PTR [edi]).actionOn,0
	jmp ReturnNoX

	SHOOT:
ret

	ChangedAction:
	invoke zukoResetAction, adrObj,cx;reset lastAction
ret

	ChangedFacing:
	invoke zukoResetAction,adrObj, ecx;reset lastAction
ret
zukoSetAction endp

moveFireBall proc adrObj: dword
	mov ebx, adrObj
	ASSUME ebx: PTR Dmoot
	cmp [ebx].ball.isShowing, 0
	je notShowing
	;;does show
	cmp [ebx].ball.fireAnim.count, 0
	jle stopFireBall

	;;move ball
	mov edx, [ebx].ball.drcX
	add [ebx].ball.pos.x,edx
	ASSUME ebx: nothing
ret

	notShowing:
ret

	stopFireBall:
	ASSUME ebx: PTR Dmoot
	mov edx, [ebx].ball.fireAnim.ogCount
	mov [ebx].ball.fireAnim.count, edx
	mov [ebx].ball.isShowing,0
	mov [ebx].ball.picIdx, 0
	ASSUME ebx: nothing
ret
moveFireBall endp

handleZukoAdvance proc adrObj: dword
	mov ebx,adrObj
	mov edx,1
	mov (Dmoot PTR [ebx]).advance, edx
ret
handleZukoAdvance endp
zukoMove proc adrObj: dword
	mov eax, adrObj
	;;deal with freeze
	mov edx, 2
	cmp (Dmoot PTR [eax]).actionOn, edx
	je JUMPING

	RET_JUMPING:
	invoke handleZukoAdvance, adrObj
	mov eax,adrObj
	mov ebx, (Dmoot PTR [eax]).advance
	cmp ebx,0
	je dontMove

	;;ELSE
	invoke moveFireBall, adrObj
	mov eax, adrObj
	mov ebx, (Dmoot PTR [eax]).drc.x
	add (Dmoot PTR [eax]).pos.x, ebx
	
	mov edx, (Dmoot PTR [eax]).drc.y
	add (Dmoot PTR [eax]).pos.y, edx
	cmp edx,-1
	je WHILEJUMPING
	RET_WHILEJUMPING:
ret
	
	JUMPING:
	mov edx, PRE_JUMP
	cmp (Dmoot PTR [eax]).jumpState, edx
	je PREJUMP
	jmp RET_JUMPING

	PREJUMP:
	;freeze

ret

	WHILEJUMPING:
	;;jumpH--
	mov edx, 1
	sub (Dmoot PTR [eax]).jumpH, ebx
jmp RET_WHILEJUMPING

	dontMove:
ret
zukoMove endp

fireBallScreenBounds proc adrObj:dword
	mov ebx, adrObj
	cmp (Dmoot PTR [ebx]).ball.isShowing, 0
	je notShowing

	;;is showing
	cmp (Dmoot PTR [ebx]).ball.pos.x, 0
	jl TOO_LEFT

	mov edx,1200
	sub edx, (Dmoot PTR [ebx]).ball.lastW 
	cmp (Dmoot PTR [ebx]).ball.pos.x, edx
	jg TOO_RIGHT

	notShowing:
ret

	TOO_LEFT:
	mov (Dmoot PTR [ebx]).ball.pos.x, 0
ret

	TOO_RIGHT:
	mov (Dmoot PTR [ebx]).ball.pos.x, edx
ret
fireBallScreenBounds endp
screenBounds proc adrObj: dword
	mov ebx, adrObj
	cmp (Dmoot PTR [ebx]).pos.x, 0
	jl TOO_LEFT

	mov edx, 1200
	sub edx, 88
	cmp (Dmoot PTR [ebx]).pos.x,edx;;minus zuko1 width
	jg TOO_RIGHT

	RET_X:

	mov edx, -10
	cmp (Dmoot PTR [ebx]).pos.y, edx
	jl TOO_HIGH

	mov edx, 700
	sub edx, zukoH
	cmp (Dmoot PTR [ebx]).pos.y,edx;;minus zuko1 width
	jg TOO_LOW

	RET_Y:
	invoke fireBallScreenBounds, adrObj
ret
	
	TOO_LEFT:
	mov (Dmoot PTR [ebx]).pos.x, 0
jmp RET_X

	TOO_RIGHT:
	mov edx, 1200
	sub edx, 88
	mov (Dmoot PTR [ebx]).pos.x, edx
jmp RET_X


	TOO_HIGH:
	mov (Dmoot PTR [ebx]).pos.y, 0
jmp RET_Y

	TOO_LOW:
	mov edx, 700
	sub edx, zukoH
	mov (Dmoot PTR [ebx]).pos.y, edx
jmp RET_Y
screenBounds endp

mapCol proc adrObj: dword
	;;changes drcX acording to map objects
	mov ebx, adrObj
	mov eax, (Dmoot PTR [ebx]).drc.x

	cmp eax,1
	je RIGHT
	cmp eax, -1
	je LEFT
ret

	RIGHT:
	mov ecx, (Dmoot PTR [ebx]).adrRightPts
	mov edx, (Dmoot PTR [ebx]).maxBytsRightPts
	invoke checkPoints,adrObj, ecx,edx, groundColor, errorColor
	cmp eax,1;;colors showing
	je STOP
ret
	
	LEFT:
	mov ecx, (Dmoot PTR [ebx]).adrLeftPts
	mov edx, (Dmoot PTR [ebx]).maxBytsLeftPts
	invoke checkPoints,adrObj, ecx,edx, groundColor, errorColor
	cmp eax,1;;colors showing
	je STOP
ret

	STOP:
	mov ebx, adrObj
	mov (Dmoot PTR [ebx]).drc.x,0
	invoke zukoSetAction, adrObj
ret
mapCol endp

; ------------------------
gotHit proc adrObj:dword
	mov eax, adrObj
	mov ebx, ballDamage
	sub (Dmoot PTR [eax]).HP, ebx
	cmp (Dmoot PTR [eax]).HP,0
	jle DEATH
ret

	DEATH:
	mov (Dmoot PTR [eax]).HP,0
	mov ebx, (Dmoot PTR [eax]).lossCode
	mov win, ebx
ret
gotHit endp
hit proc adrHit:dword, adrDef:dword
	mov eax, adrHit
	mov ebx, adrDef
	cmp (Dmoot PTR [eax]).ball.isShowing, 1
	jne NoHit
	
	;;check y hit
	mov ecx,(Dmoot PTR [eax]).ball.pos.y;;ecx = yB
	sub ecx, zukoH ;; ecx = yB-zukoH
	mov edx, (Dmoot PTR [ebx]).pos.y ;;edx = yZ
	cmp edx, ecx
	jle NoHit
	;;if(yZ>=yB-zukoH) -> noHit

	mov ecx,(Dmoot PTR [eax]).ball.pos.y;;ecx = yB
	add ecx, ballH ;; ecx = yB + hB
	cmp edx, ecx
	jge NoHit
	;;if(yZ>=yB+hB) -> noHit

	;;HIT_Y
	mov ecx,(Dmoot PTR [eax]).ball.pos.x;;ecx = xB
	sub ecx, zukoW;; ecx = xB-Wz
	mov edx, (Dmoot PTR [ebx]).pos.x ;;edx =xZ
	cmp edx, ecx
	jle NoHit
	;;if(xZ<=xB-Wz) 0> noHit

	mov ecx,(Dmoot PTR [eax]).ball.pos.x;;ecx = xB
	add ecx, (Dmoot PTR [eax]).ball.lastW ;; ecx = xB + wB
	cmp edx, ecx
	jge NoHit
	;;if(xZ>=xB+wB) -> noHit
	
	jmp HIT
	NoHit:
ret

	HIT:
	mov (Dmoot PTR [eax]).ball.fireAnim.count, 0
	invoke playSoundAsync, offset hd.fireBall.hitSound
	invoke gotHit, adrDef
ret
hit endp

; ------------------------

handleFireBallPicIdx proc adrObj:dword
	mov ebx, adrObj
	ASSUME ebx: PTR Dmoot
	;;count cannot be 0 here
	;;count--
	mov edx, 1
	sub [ebx].ball.fireAnim.count, edx
	;;if(count%cycle==0) picIdx++
	mov eax, [ebx].ball.fireAnim.count;; eax = count
	mov edi, [ebx].ball.fireAnim.cycle;;edi = cycle
	mov edx,0
	div edi; edi = eax%edi = count%cycle
	cmp edx,0
	je incPicIdx
	ASSUME ebx: NOTHING
ret

	incPicIdx:
	ASSUME ebx: PTR Dmoot
	mov edx,4
	add [ebx].ball.picIdx, edx
	ASSUME ebx: NOTHING
ret
handleFireBallPicIdx endp

drawFireBall proc adrObj:dword
	mov ebx, adrObj
	ASSUME ebx: PTR Dmoot
	cmp [ebx].ball.isShowing,0
	je notShowing

	mov ecx, [ebx].ball.picIdx
	mov edx,0
	cmp [ebx].ball.drcX, edx
	jg right
	jl left

	notShowing:
	ASSUME ebx: nothing
ret

	right:
	mov ebx,adrObj
	ASSUME ebx: PTR Dmoot
	mov eax, [ebx].ball.pos.x
	mov edx, [ebx].ball.pos.y
	invoke drd_imageDrawCrop, offset ram.fireBall.moveR, eax,edx, fireBallRSrcX[ecx],0,fireBallRW[ecx],ballH
	mov ecx, [ebx].ball.picIdx
	mov edx, fireBallRW[ecx]
	mov [ebx].ball.lastW, edx
	invoke handleFireBallPicIdx, adrObj
	ASSUME ebx: nothing
ret

	left:
	ASSUME ebx: PTR Dmoot
	invoke drd_imageDrawCrop, offset ram.fireBall.moveL, [ebx].ball.pos.x, [ebx].ball.pos.y, fireBallLSrcX[ecx],0,fireBallLW[ecx],zukoH
	mov ecx, [ebx].ball.picIdx
	mov edx, fireBallRW[ecx]
	mov [ebx].ball.lastW, edx
	invoke handleFireBallPicIdx, adrObj
	ASSUME ebx: nothing
ret
drawFireBall endp

displayDigit proc digit:dword, posX:dword, posY: dword

	;; idx = digit*4
	mov eax, digit
	mov edx,0
	mov ecx, 4
	mul ecx
	;;eax = idx
	invoke drd_imageDrawCrop, offset res.digits, posX,posY, digitSrcX[eax],0,digitW, digitH
ret
displayDigit endp

displayHP proc adrObj:dword
	mov eax, adrObj
	mov ebx, (Dmoot PTR [eax]).startPtHP.x
	mov ecx, (Dmoot PTR [eax]).startPtHP.y
	mov edx, (Dmoot PTR [eax]).HP

	loopy:
		mov edi,10
		mov eax, edx
		mov edx,0
		div edi;; edx = eax%edi = hp%10
		;; eax = eax/edi
		push eax
		;;digit, x, y
		invoke displayDigit, edx, ebx, ecx
		pop eax
		cmp eax,0
		je ENDLOOP
		mov edx, eax ;;edx=hp
		sub ebx, digitW
		mov eax, adrObj
		mov	ecx, (Dmoot PTR [eax]).startPtHP.y
	jmp loopy
	ENDLOOP:
ret
displayHP endp

displayPlayerNum proc adrObj:dword
	mov ebx, adrObj
	mov eax, (Dmoot PTR [ebx]).playerNum
	cmp eax,1
	je PLAYER1

	PLAYER2:
	mov ecx, (Dmoot PTR [ebx]).pos.x
	add ecx, (Dmoot PTR [ebx]).toPlayerNum.x
	mov edx, (Dmoot PTR [ebx]).pos.y
	add edx, (Dmoot PTR [ebx]).toPlayerNum.y

	invoke drd_imageDraw, offset res.dig2, ecx,edx
ret

	PLAYER1:
	mov ecx, (Dmoot PTR [ebx]).pos.x
	add ecx, (Dmoot PTR [ebx]).toPlayerNum.x
	mov edx, (Dmoot PTR [ebx]).pos.y
	add edx, (Dmoot PTR [ebx]).toPlayerNum.y

	invoke drd_imageDraw, offset res.dig1, ecx,edx
ret
displayPlayerNum endp

handleZukoPicIdx proc adrObj:dword, toOgCountAdr: dword, toCountAdr: dword, toCycleAdr: dword
	mov ebx, adrObj
	add ebx, toCountAdr
	mov eax, [ebx]
	cmp eax,0;;if count==0 reset picIdx and count
	je resetCountAndPicIdx

	;;count--
	mov eax, 1
	sub [ebx],eax

	;;if(count%cycle==0) picIdx++
	mov eax, [ebx];;eax = count
	mov ebx,adrObj
	add ebx, toCycleAdr
	mov edi, [ebx];;edi = cycle
	mov edx, 0
	div edi;;edx = eax%edi = count%cycle
	cmp edx,0
	je incPicIdx
ret
	
	incPicIdx:
	mov ebx,4;;picIdx++
	mov eax, adrObj
	add (Dmoot PTR [eax]).picIdx, ebx
ret

	resetPicIdx:
	mov eax, adrObj
	mov (Dmoot PTR [eax]).picIdx, 0
ret

	resetCountAndPicIdx:
	mov eax, adrObj
	mov (Dmoot PTR [eax]).picIdx, 0

	;reset count by ogCount
	ASSUME eax: nothing
	mov ebx, adrObj
	add ebx, toOgCountAdr
	mov eax, adrObj
	add eax, toCountAdr
	mov edx, [ebx]
	mov [eax], edx
ret
handleZukoPicIdx endp

zukoDrawPic proc ramImgAdr:dword, posX:dword, posY:dword, srcX:dword, widZZ:dword, adrObj:dword, toOgCount:dword, toCount:dword, toCycle:dword
	ASSUME eax: nothing
	invoke drd_imageDrawCrop,ramImgAdr,posX,posY,srcX,0, widZZ,zukoH
	invoke handleZukoPicIdx, adrObj, toOgCount, toCount, toCycle
ret
zukoDrawPic endp

drawZuko proc adrObj:dword
	invoke drawFireBall, adrObj
	invoke displayHP, adrObj
	invoke displayPlayerNum, adrObj

	mov eax, adrObj
	ASSUME eax:PTR Dmoot

	mov ecx, [eax].picIdx;ecx = picIdx
	mov edx, [eax].pos.x
	mov edi, [eax].pos.y

	cmp [eax].actionOn, 0
	je standState
	cmp [eax].actionOn,3
	je fallState
	cmp [eax].actionOn, 2;jumping
	je Jump
	cmp [eax].drc.x,1
	je runRState
	cmp [eax].drc.x,-1
	je runLState
	cmp [eax].actionOn, 4
	je shoot
ret

	standState:
	cmp [eax].facing, 0
	je standR
	;;left
	invoke zukoDrawPic,offset ram.zuko.standL, edx, edi,zukoStandLSrcX[ecx],zukoStandWid, adrObj, Dmoot.standAnim.ogCount, Dmoot.standAnim.count, Dmoot.standAnim.cycle
ret

	standR:
	invoke zukoDrawPic,offset ram.zuko.standR, edx, edi,zukoStandRSrcX[ecx],zukoStandWid, adrObj, Dmoot.standAnim.ogCount, Dmoot.standAnim.count, Dmoot.standAnim.cycle
ret
	
	runRState:
	invoke zukoDrawPic,offset ram.zuko.runR,edx,edi,zukoRunRSrcX[ecx], zukoRunRW[ecx],adrObj,  Dmoot.runAnim.ogCount, Dmoot.runAnim.count, Dmoot.runAnim.cycle 
ret
	runLState:
	invoke zukoDrawPic,offset ram.zuko.runL,edx,edi,zukoRunLSrcX[ecx], zukoRunLW[ecx],adrObj,  Dmoot.runAnim.ogCount, Dmoot.runAnim.count, Dmoot.runAnim.cycle 
ret

	fallState:
	cmp [eax].facing,0
	je fallR
	;;left
	invoke zukoDrawPic,offset ram.zuko.fallL,edx,edi,zukoFallLSrcX[ecx], zukoFallLW[ecx],adrObj, Dmoot.fallAnim.ogCount, Dmoot.fallAnim.count, Dmoot.fallAnim.cycle
ret
	
	fallR:
	invoke zukoDrawPic,offset ram.zuko.fallR,edx,edi,zukoFallRSrcX[ecx], zukoFallRW[ecx],adrObj, Dmoot.fallAnim.ogCount, Dmoot.fallAnim.count, Dmoot.fallAnim.cycle
ret
	Jump:
	cmp [eax].facing,0 
	je jumpR
	;;left
	;;split pre-while
	mov esi, PRE_JUMP
	cmp [eax].jumpState, esi
	je preJumpL
	;;jumpL
	invoke zukoDrawPic,offset ram.zuko.jumpL,edx,edi,zukoJumpLSrcX[ecx], zukoJumpLW[ecx],adrObj, Dmoot.jumpAnim.ogCount, Dmoot.jumpAnim.count, Dmoot.jumpAnim.cycle
ret
	
	preJumpL:
	invoke zukoDrawPic,offset ram.zuko.jumpL,edx,edi,zukoPreJumpLSrcX, zukoPreJumpLW,adrObj, Dmoot.jumpAnim.ogCount, Dmoot.jumpAnim.count, Dmoot.jumpAnim.cycle
ret
	jumpR:
	mov esi, PRE_JUMP
	cmp [eax].jumpState, esi
	je preJumpR
	invoke zukoDrawPic,offset ram.zuko.jumpR,edx,edi,zukoJumpRSrcX[ecx], zukoJumpRW[ecx],adrObj, Dmoot.jumpAnim.ogCount, Dmoot.jumpAnim.count, Dmoot.jumpAnim.cycle
ret

	preJumpR:
	invoke zukoDrawPic,offset ram.zuko.jumpR,edx,edi,zukoPreJumpRSrcX, zukoPreJumpRW,adrObj, Dmoot.jumpAnim.ogCount, Dmoot.jumpAnim.count, Dmoot.jumpAnim.cycle
ret

	shoot:
	cmp [eax].facing, 0
	je shootR
	;;shoot L
	invoke zukoDrawPic,offset ram.zuko.shootL,edx,edi,zukoShootLSrcX[ecx], zukoShootLW[ecx],adrObj,  Dmoot.shootAnim.ogCount, Dmoot.shootAnim.count, Dmoot.shootAnim.cycle
ret

	shootR:
	invoke zukoDrawPic,offset ram.zuko.shootR,edx,edi,zukoShootRSrcX[ecx], zukoShootRW[ecx],adrObj,  Dmoot.shootAnim.ogCount, Dmoot.shootAnim.count, Dmoot.shootAnim.cycle
ret
drawZuko endp

;------------------------------
winScreen proc adrObj1: dword, adrObj2: dword
	mov eax, win
	mov ebx, adrObj1
	cmp eax, (Dmoot PTR [ebx]).playerNum
	je PLAYER1WON

	PLAYER2WON:
	invoke drd_imageDraw,offset res.win2,0,0

ret
	PLAYER1WON:
	invoke drd_imageDraw,offset res.win1,0,0
	
ret
winScreen endp

resetGame proc adrObj: dword
	mov eax, adrObj
	mov ebx, (Dmoot PTR [eax]).startPos.x
	mov (Dmoot PTR [eax]).pos.x, ebx
	
	mov ebx, (Dmoot PTR [eax]).startPos.y
	mov (Dmoot PTR [eax]).pos.y, ebx

	mov ebx, StartHP
	mov (Dmoot PTR [eax]).HP, ebx
	
	mov ebx, (Dmoot PTR [eax]).actionOn
	invoke zukoResetAction, adrObj, ebx
ret
resetGame endp

