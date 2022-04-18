    .model small        ;Model of memory
    .stack 100h         ;Sets stack size
    .data               ;Start of DS

message db 0dh, 0ah, 'This is a message!$', 0dh, 0ah    ;Resets buffers, loads and reads the string to the message label address 

        .code           ;Start of CS
        org 100h        ;Where to put next piece of code/data

start:  mov ax, @data   ;Initializes DS Register
        mov ds, ax      ;Moves Data Stored in AX into DS

		lea dx, message ;Loads the effective address of message label into DX
        mov ah, 9       ;Moves 9 into AH to prime interrupt for string printing
        int 21h         ;Interrupt to print the string at DX

        mov ah, 4ch     ;Moves 4c into AX to prime for interrupt
        int 21h         ;Interrupt to cleanly terminate code
        end             ;Tells compiler to stop reading
