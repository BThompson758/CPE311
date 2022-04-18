    .model small    ;Small model
    .stack 100h     ;Stack size 100
    .data           ;Start of DS

    .code           ;start of CS
    org 0000h       ;Start at IP=0000


start:  mov ax,@data    ;Initializes DS Register
        mov ds,ax       ;Moves Data Stored in AX into DS 
        mov ah,01h      ;Primes int 21 for displaying AL
        int 21h         ;Displays data in AL
        cmp al,0dh      ;Compares data in AL with enter
        je enterRead    ;if compare is equal, jump to enterRead
        jmp start       ;jumps back to start after key is displayed

enterRead:  mov ah,4ch  ;Primes for clean termination
            int 21h     ;Terminates program
            end         ;For compiler to stop reading


