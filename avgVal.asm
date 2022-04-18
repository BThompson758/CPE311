    .model small                        ;Small model
    .stack 100h                         ;Stack size 100
    .data                               ;Start of DS

prompt      db 0dh, 0ah, 'Please enter a number: $', 0dh, 0ah   ;Text w/ buffer resets on both sides 
result      db 0dh, 0ah, 'The average is: $'                    ;Text w/o buffer resets after the final character
notnum      db 0dh, 0ah, 'Incorrect input$', 0dh, 0ah           ;Text w/ buffer resets on both sides

values 		db 10 dup(?)                ;Array w/ 10 values
            
    .code                               ;Start of CS
    org 000h                            ;Start IP at 0

start:    		mov ax, @data           ;Initializes DS
            	mov ds, ax              ;Moves AX to DS 
            	mov di, 0               ;Initializes DI
                lea di, values          ;Loads EA of array into CX
                mov cx,0                ;Starts counter at 0
                jmp inputLoop           ;Jumps to take input


incorrect:      mov dx, offset notnum   ;Loads message into dx
	            mov ah, 9               ;Primes to display dx
	            int 21h                 ;Shows dx
                jmp inputLoop           ;Jumps to take input

inputLoop:      mov dx, offset prompt   ;Loads prompt into dx
	            mov ah, 9               ;Primes for displaying prompt
	            int 21h                 ;Displays data in dx
                mov ah,01h              ;Primes int 21 for displaying AL
                int 21h                 ;Displays data in AL
                cmp al,3Ah              ;Compares data in AL with 3A
                jg incorrect            ;Displays incorrect
                cmp al,2Fh              ;Compared data in AL with 2F
                jl incorrect
                sub ax,30h              ;Subtracts 30 from value
                mov [di],al             ;Moves data into array
                inc cx                  ;increments cx counter
                inc di                  ;Increments array
                cmp cx,10               ;Compares the location of CX to see # of values
                jl inputLoop            ;if cx has less than 10 elements, it loops
                lea di,values           ;resets array pointer to 1st element
                mov ax,0                ;Resets al to 0 to store values
                mov cx,0                ;Resets counter
                jmp avgCalc             ;Jumps to avgCalc

avgCalc:        add al,[di]             ;Adds value at DI to al
                inc di                  ;Incrementing array
                inc cx                  ;increments cx (Counter)
                cmp cx,10               ;Compares value of cx/counter to 10
                jl avgCalc              ;loops if cx < 10
                mov ah,0                ;Zeroing out AH
                mov bh,0                ;Zeroing out BH
                mov bl,10               ;Loads divisor into bx
                div bl                  ;Divides al by bl
                mov dx, offset result   ;Loads result into dx
	            mov ah, 9               ;Primes for displaying prompt
	            int 21h                 ;Displays dx
                mov dx, ax              ;Moves ax into DX
                add dx,30h              ;Converts back to ASCII
                mov ah, 02h             ;Primes for displaying result
                int 21h                 ;Displays result
                mov ah,4ch              ;Primes for clean termination
                int 21h                 ;Terminates program
                end                     ;For compiler to stop reading
            
                


