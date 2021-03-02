include drd.inc
includelib drd.lib
include data.inc
include junks.inc
include funcs.inc
include bots.inc
.code

load proc
	invoke drd_init,wwid,hhigh,0;;open window
	;;load images
	invoke drd_imageLoadFile,offset res.bgpath1,offset res.bg1
	invoke drd_imageLoadFile,offset res.digitsPath,offset res.digits

	invoke drd_imageLoadFile,offset res.win1Path,offset res.win1
	invoke drd_imageLoadFile,offset res.win2Path,offset res.win2

	invoke drd_imageLoadFile,offset res.dig1Path,offset res.dig1
	invoke drd_imageSetTransparent,offset res.dig1,0ffffffh

	invoke drd_imageLoadFile,offset res.dig2Path,offset res.dig2
	invoke drd_imageSetTransparent,offset res.dig2,0ffffffh

	invoke drd_imageLoadFile, offset hd.zuko.standR, offset ram.zuko.standR
	invoke drd_imageSetTransparent,offset ram.zuko.standR,0009600h
	
	invoke drd_imageLoadFile, offset hd.zuko.standL, offset ram.zuko.standL
	invoke drd_imageSetTransparent,offset ram.zuko.standL,0009600h

	invoke drd_imageLoadFile, offset hd.zuko.runR, offset ram.zuko.runR
	invoke drd_imageSetTransparent,offset ram.zuko.runR,0009600h

	invoke drd_imageLoadFile, offset hd.zuko.runL, offset ram.zuko.runL
	invoke drd_imageSetTransparent,offset ram.zuko.runL,0009600h

	invoke drd_imageLoadFile, offset hd.zuko.fallR, offset ram.zuko.fallR
	invoke drd_imageSetTransparent,offset ram.zuko.fallR,0009600h

	invoke drd_imageLoadFile, offset hd.zuko.fallL, offset ram.zuko.fallL
	invoke drd_imageSetTransparent,offset ram.zuko.fallL,0009600h

	invoke drd_imageLoadFile, offset hd.zuko.jumpR, offset ram.zuko.jumpR
	invoke drd_imageSetTransparent,offset ram.zuko.jumpR,0009600h

	invoke drd_imageLoadFile, offset hd.zuko.jumpL, offset ram.zuko.jumpL
	invoke drd_imageSetTransparent,offset ram.zuko.jumpL,0009600h

	invoke drd_imageLoadFile, offset hd.zuko.shootR, offset ram.zuko.shootR
	invoke drd_imageSetTransparent,offset ram.zuko.shootR,0009600h

	invoke drd_imageLoadFile, offset hd.zuko.shootL, offset ram.zuko.shootL
	invoke drd_imageSetTransparent,offset ram.zuko.shootL,0009600h

	invoke drd_imageLoadFile, offset hd.fireBall.moveR, offset ram.fireBall.moveR
	invoke drd_imageSetTransparent,offset ram.fireBall.moveR,0009600h

	invoke drd_imageLoadFile, offset hd.fireBall.moveL, offset ram.fireBall.moveL
	invoke drd_imageSetTransparent,offset ram.fireBall.moveL,0009600h
ret
load endp

update proc
	invoke drd_processMessages;;allow window to be closed

	;;check if someone won the game
	cmp win, 0
	jne SOMEONEWON

	invoke zukoDflt, offset zuko1
	invoke zukoDflt, offset zuko2

	invoke keyboard, offset zuko1

	invoke zukoActByKey, 0, offset B_Key,offset ShouldBot, 1
	mov edx, 0
	mov edi, ShouldBot
	cmp edi, edx
	je NOBOT

	invoke doBot, offset zuko2, offset zuko1
	NOBOT:
	invoke keyboard, offset zuko2

	invoke gravityC, offset zuko1
	invoke gravityC, offset zuko2

	invoke zukoSetAction, offset zuko1
	invoke zukoSetAction, offset zuko2
	
	invoke mapCol, offset zuko1
	invoke mapCol, offset zuko2

	invoke zukoMove, offset zuko1
	invoke zukoMove, offset zuko2

	invoke screenBounds, offset zuko1
	invoke screenBounds, offset zuko2

	invoke hit, offset zuko1, offset zuko2
	invoke hit, offset zuko2, offset zuko1
ret

	SOMEONEWON:
	;;skip update
	invoke zukoActByKey,0,offset Rkey,offset win, 0
	cmp win,0
	je RESET
ret

	RESET:
	invoke resetGame, offset zuko1
	invoke resetGame, offset zuko2
ret
update endp

draw proc
		invoke drd_pixelsClear,0;;clear screen
		
		;;check if someone won
		cmp win,0 
		jne SOMEONEWON

		invoke drd_imageDraw,offset res.bg1,0,0

		invoke drawZuko, offset zuko1
		invoke drawZuko, offset zuko2

		invoke drd_flip	
ret

	SOMEONEWON:
	invoke winScreen, offset zuko1, offset zuko2
	invoke drd_flip
ret
draw endp