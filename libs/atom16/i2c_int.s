    .section .bss
si2c1_stack:    .space 128
s12c2_stack:    .space 128
 
    .section .text
    .extern _I2C_slave_int
    .global _SI2C1Setup
    .global _SI2C2Setup
    .global __SI2C1Interrupt
    .global __SI2C2Interrupt
    

_SI2C1Setup:
    RETURN
    
_SI2C2Setup:
    RETURN
    
    ; 0x000034
__SI2C1Interrupt:
    ; switch stacks to the NMI stack
    PUSH.D W0
    MOV #si2c1_stack + #128, W1
    MOV SPLIM, W0
    MOV W1, SPLIM
    MOV W15, W1
    MOV #si2c1_stack, W15
    PUSH.D W0	    ; push splim, old SP
    PUSH.D W2
    PUSH.D W4
    PUSH.D W6
    PUSH.D W8
    PUSH.D W10
    PUSH.D W12
    PUSH W14
    MOV #0, W0	    ; I2C Channel 1
    call _I2C_slave_int
    POP W14
    POP.D W12
    POP.D W10
    POP.D W8
    POP.D W6
    POP.D W4
    POP.D W2
    POP.D W0
    MOV W0, SPLIM
    MOV W1, W15
    POP.D W0
    
    RETFIE
    
    
    ; 0x000076

__SI2C2Interrupt:
    ; switch stacks to the NMI stack
    PUSH.D W0
    MOV #si2c1_stack + #128, W1
    MOV SPLIM, W0
    MOV W1, SPLIM
    MOV W15, W1
    MOV si2c1_stack, W15
    PUSH.D W0	    ; push splim, old SP
    PUSH.D W2
    PUSH.D W4
    PUSH.D W6
    PUSH.D W8
    PUSH.D W10
    PUSH.D W12
    PUSH W14
    MOV #1, W0	    ; I2C Channel 2
    call _I2C_slave_int
    POP W14
    POP.D W12
    POP.D W10
    POP.D W8
    POP.D W6
    POP.D W4
    POP.D W2
    POP.D W0
    MOV W0, SPLIM
    MOV W1, W15
    POP.D W0
    
    RETFIE
    RETURN
    



