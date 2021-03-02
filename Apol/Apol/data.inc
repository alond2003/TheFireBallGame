.const
;;structs
HDzuko struct
	standR byte "zuko/standR.png",0
	standL byte "zuko/standL.png",0
	runR byte "zuko/runR.png",0
	runL byte "zuko/runL.png",0
	jump byte "zuko/jump.png",0
	fallR byte "zuko/fallR.png",0
	fallL byte "zuko/fallL.png",0
	jumpR byte "zuko/jumpR.png",0
	jumpL byte "zuko/jumpL.png",0
	shootR byte "zuko/shootR.png",0
	shootL byte "zuko/shootL.png",0
HDzuko ends

HDfireBall struct
	moveR byte "otherPics/fireBallR.png",0
	moveL byte "otherPics/fireBallL.png",0
	fireSound byte "sound/fireBall.wav",0
	hitSound byte "sound/hit.wav",0
HDfireBall ends

HD struct
	zuko HDzuko<>
	fireBall HDfireBall<>
HD ends

AnimfireBall struct
	moveR Img<>
	moveL Img<>
AnimfireBall ends

Anim struct
	standR Img<>
	standL Img<>
	runR Img<>
	runL Img<>
	fallR Img<>
	fallL Img<>
	jumpR Img<>
	jumpL Img<>
	shootR Img<>
	shootL Img<>
Anim ends

Ram struct
	zuko Anim<>
	fireBall AnimfireBall<>
Ram ends

Resorses struct
	bgpath1 byte "otherPics/newFullBg.png",0
	bg1 Img<>
	digitsPath byte "otherPics/digits.png",0
	digits Img<>
	win1Path byte "otherPics/player1won.png",0
	win1 Img<>
	win2Path byte "otherPics/player2won.png",0
	win2 Img<>
	dig1Path byte "otherPics/dig1.png",0
	dig1 Img<>
	dig2Path byte "otherPics/dig2.png",0
	dig2 Img<>
Resorses ends

Keys struct
	left dword ?
	right dword ?
	up dword ?
	shoot dword ?
Keys ends

Action struct
	ogCount dword ?
	count dword ?
	cycle dword ?
	;ogCount/cycle = number of frames in sprite sheet
	;ogCount = cycle*number_of_frames_in_sprite_sheet
Action ends

fireBall struct
	pos POINT<0,0>
	drcX dword 0
	isShowing dword 0; 0- not showing, 1- showing
	fireAnim Action<400,400,50>
	picIdx dword 0;;idx in Anim
	lastW dword 0 
fireBall ends

Dmoot struct
	pos POINT<>
	startPos POINT<>;;the position Dmoot starts at the begining of the game
	drc POINT<>
	keys Keys<>
	adrUpPts dword ?
	maxBytsUpPts dword ?
	adrLeftPts dword ?
	maxBytsLeftPts dword ?
	adrRightPts dword ?
	maxBytsRightPts dword ?
	
	startPtHP POINT<>;; the place to draw the HP at
	lossCode dword ?
	playerNum dword ?

	toPlayerNum POINT <15,-22>

	moveAct Action<4,4,2>
	standAnim Action<100,100,50>
	runAnim Action<200,200,25>
	shootAnim Action<200,200,50>
	fallAnim Action<100,100,50>
	jumpAnim Action<80,80,25>
  

	picIdx dword 0;;idx in animation
	facing dword 0; 0-right, 1-left
	actionOn dword 0 ;; 0-stand, 1-run, 2-jump, 3-fall, 4-shooting
	jumpState dword 0 ;; 0-pre, 1-jump
	advance dword 1 ;; 0-don't, 1-do

	ogJumpH dword 204
	jumpH dword 204 ;; at that height starts to fall after jump
	ogJumpFreeze dword 50
	jumpFreeze dword 50;;freeze before jump for that amount of time
	
	ctrlPt1 POINT<0,102>;fall
	ctrlPt2 POINT<76,102>;fall
	
	ball fireBall<>
	HP dword 200

	lastStopJumpReason dword 0
Dmoot ends

.data

emptyDword dword 0
Rkey dword VK_R

res Resorses<>
hd HD<>
ram Ram<>

win dword 0 ;; 0-game, 2-zuko2 won, 1-zuko1 won

wwid dword 1200
hhigh dword 700

StartHP dword 200

ARRpoints POINT 44 dup(<0,-1>,<66,-1>,<0,-2>,<66,-2>,<0,-3>,<66,-3>,<0,-4>,<66,-4>,<0,-5>,<66,-5>,<0,-6>,<66,-7>,<0,-8>,<66,-8>,<0,-9>,<66,-9>,<0,-10>,<66,-10>,<0,-11>,<66,-11>,<0,-12>,<66,-12>,<0,-13>,<66,-13>,<0,-14>,<66,-14>,<0,-15>,<66,-15>,<0,-16>,<66,-16>,<0,-17>,<66,-17>,<0,-18>,<66,-18>,<0,-19>,<66,-19>,<0,-20>,<66,-20>,<-1,-1>,<65,-1>,<-2,-1>,<64,-1>,<-3,-1>,<63,-1>,<-4,-1>,<62,-1>)

UpPts POINT 2 dup(<0,-1>,<55,-1>)
LeftPts POINT 3 dup(<-1,0>,<-1,52>,<-1,101>)
RightPts POINT 5 dup(<56,0>,<56,28> ,<56,52>,<56,80>, <56,101>)

zuko1 Dmoot<<100,300>,<100,300>,<0,0>,<VK_LEFT,VK_RIGHT,VK_UP,VK_DOWN>,offset UpPts,SIZEOF UpPts, offset LeftPts, SIZEOF LeftPts, offset RightPts, SIZEOF RightPts,<110,608>,2,1>;sizeof - number of bytes
zuko2 Dmoot<<1110,300>,<1110,300>,<0,0>,<VK_A,VK_D,VK_W,VK_S>,offset UpPts,SIZEOF UpPts, offset LeftPts, SIZEOF LeftPts, offset RightPts, SIZEOF RightPts,<1142,608>,1,2>;sizeof - n

groundColor dword 000486880h 
errorColor dword 000ffffffh
blackColor dword 000000000h
noColor dword 022222222h 

zukoH dword 102
zukoW dword 76

zukoStandWid dword 62
zukoStandRSrcX dword 2 dup(0,78)
zukoStandLSrcX dword 2 dup(78,0)

zukoRunRW dword 8 dup(77,88,82,64,77,86,82,66)
zukoRunRSrcX dword 8 dup(0,78,167,250,315,393,480,563)
zukoRunLW dword 8 dup(66,82,86,77,64,82,88,77)
zukoRunLSrcX dword 8 dup(1,68,151,238,316,381,464,553)

zukoShootRW dword 4 dup(64,54,82,82)
zukoShootRSrcX dword 4 dup(0,65,120,203)
zukoShootLW dword 4 dup(64,54,82,82)
zukoShootLSrcX dword 4 dup(222,167,84,1)

zukoFallRW dword 4 dup(76,76,66,66)
zukoFallRSrcX dword 4 dup(0,77,154,221)
zukoFallLW dword 4 dup(66,66,76,76)
zukoFallLSrcX dword 4 dup(1,67,134,211)

zukoPreJumpRW dword 58
zukoPreJumpRSrcX dword 1
zukoJumpRW dword 8 dup(78,78,68,68,68,68,78,78)
zukoJumpRSrcX dword 8 dup(59,138,217,286,286,217,138,59)
zukoPostJumpRW dword 3 dup(58,65,58)
zukoPostJumpRSrcX dword 3 dup(355,414,480)

zukoPreJumpLW dword 58
zukoPreJumpLSrcX dword 481
zukoJumpLW dword 8 dup(78,78,68,68,68,68,78,78)
zukoJumpLSrcX dword 8 dup(402,323,254,185,185,254,323,402)
zukoPostJumpLW dword 3 dup(58,65,58)
zukoPostJumpLSrcX dword 3 dup(126,60,59)

;flags for jump State
PRE_JUMP dword 0
WHILE_JUMP dword 1

START_FIREBALL dword 50

ctrlPts POINT 2 dup(<0,102>,<55,102>)

createBallR POINT <83,11>
createBallL POINT <-136,11>

ballH dword 60
fireBallRW dword 8 dup(136,134,104,106,72,74,46,48)
fireBallRSrcX dword 8 dup(0,137,272,377,484,557,632,679)
fireBallLW dword 8 dup(136,134,104,106,72,74,46,48)
fireBallLSrcX dword 8 dup(591,456,351,244,171,96,49,0)
ballDamage dword 50

digitH dword 72
digitW dword 52
digitSrcX dword 10 dup(0,52,104,156,208,260,312,364,416,468)

TIKRA dword 0
JUMPHCAP dword 1
ENDSCREEN dword 2

COPYMOVES dword 0 ;; 0-not, 1- true

B_Key dword VK_B
ShouldBot dword 0 ;; 0-not, 1-yes
SkipAll dword 0 ;; 0-not, 1-yes
ShouldTurnAway dword 0 ;; 0-not, 1-yes
jumpCounterForBot dword 0 ;; 
gotoTryJump dword 0
memDrc dword 0
counterKeepGoing dword 0

DrcFall dword 0
FallPointL POINT 1 dup(<0,102>)
FallPointR POINT 1 dup(<55,102>)
deadY dword 0
tryTo dword 0 ;;0-not trying, 1- trying to jump, -1 - trying to fall
floorY dword 0
floorPts POINT 2 dup(<0,102>,<55,102>)

