    .section .text
    .global _yield
    .global _dispatch_task
    .global _get_error_location
    .global _enter_critical
    .global _exit_critical
    
_enter_critical:
  PUSH W0
  ;MOV #0x00E0, W0   ; set IRQL=7
  MOV #0x00A0, W0   ; set IRQL=5 or mask ints
  MOV W0, SR
  POP W0
  RETURN
  
_exit_critical:
  PUSH W0
  CLR W0
  MOV W0, SR
  POP W0
  RETURN
    
_yield:
  PUSH SR;
  PUSH.D W0         ; we need W0, and W1
  ;MOV #0x00E0, W0    ; set IRQL=7
  MOV #0x00A0, W0   ; set IRQL=5
  MOV W0, SR
  ; stack is
  ; W1
  ; W0
  ; SR
  ; Return address (2 words)
  MOV _current_task, W0
  ADD #42, W0	     ; W0-> points w15
  MOV W15, W1        ; W1 = old stack pointer
  MOV W0, W15        ; W15 = context
  SUB #6, W1         ; Remove the PUSHD and SR
  PUSH W1            ; stack pointer
  MOV [W1++], W0     ; SR
  PUSH W0	     ; store SR
  PUSH W14           ; FP
  PUSH.D W12
  PUSH.D W10
  PUSH.D W8
  PUSH.D W6
  PUSH.D W4
  PUSH.D W2
  MOV [W1++], W0     ; get W0 from the old stack
  PUSH W0
  MOV [W1], W0
  PUSH W0          ; actually pushing w1
  SUB #4, W1       ; W1->Stack top
  PUSH RCOUNT
  PUSH TBLPAG
  PUSH	ACCAL
  PUSH	ACCAH
  PUSH	ACCAU
  PUSH	ACCBL
  PUSH	ACCBH
  PUSH	ACCBU
  PUSH	DSRPAG
  PUSH	DSWPAG
  PUSH CORCON
  MOV W1, W15         ; restore old stack, but not W0, W1 or SR
  
_dispatch_task:
  call _schedule
  NOP
  
  ; return current_task in WO
  ADD #40, W0	    ; offset to stack limit
  MOV [W0], W1     ; get the stack limit
  MOV W1, SPLIM
  ADD #58, W0        ; W0-> points regs top
  MOV W0, W15        ; stack points to the state regs
  POP CORCON
  POP DSWPAG
  POP DSRPAG
  POP ACCBU
  POP ACCBH
  POP ACCBL
  POP ACCAU
  POP ACCAH
  POP ACCAL
  POP TBLPAG
  POP RCOUNT
  POP.D W0
  POP.D W2
  POP.D W4
  POP.D W6
  POP.D W8
  POP.D W10
  POP.D W12
  POP W14
  DISI #8
  POP	SR      ; get IPL restored
  POP W15       ; restore user stack
  RETURN
  NOP
  
; Stack Growth from Trap Error
;1. PC[15:0]            <--- Trap Address
;2. SR[7:0]:IPL3:PC[22:16]
;3. RCOUNT
;4. W0
;5. W1
;6. W2
;7. W3
;8. W4
;9. W5
;10. W6
;11. W7
;12. DSRPAG
;13. DSWPAG
;14. OLD FRAME POINTER [W14]
;15. PC[15:0]           <---- W14 
;16. 0:PC[22:16]
;15.                    <---- W15

_get_error_location:
        mov    w14,w2
        sub    w2,#28,w2
        mov    [w2++],w0
        mov    [w2++],w1 
        mov    #0x7f,w3     ; Mask off non-address bits
        and    w1,w3,w1
        return
     
