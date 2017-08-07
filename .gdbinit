define loadgo
 make
 load
 monitor reset halt
end

target remote :3333
monitor arm semihosting enable
monitor reset halt
