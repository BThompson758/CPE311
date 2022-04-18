    .model small
    .stack 100h

    .data
msg	db 'Hello World!$'

	; here’s a comment
    .code
    mov ax, @data		; this a comment
	mov ds, ax			; this is also a comment
	mov dx, offset msg

	mov ah, 9
	int 21h

	mov ah, 4ch
	int 21h

    end
