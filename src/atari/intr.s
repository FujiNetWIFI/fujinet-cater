	;;  Simple interrupt handler for PROCEED
	;;  Sets "trip" variable to 1 when triggered.

	.export _ih
	.import _trip

_ih:	lda #$01		; Assign 1
	sta _trip		; to trip
	pla			; Pull parameter off stack.
	rti			; Return to main program.
	
