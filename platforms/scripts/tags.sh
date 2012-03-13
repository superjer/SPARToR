#!/bin/bash

# This script calls ctags and cscope to build tags info for Vim.
# Uses some sed/awk tricks to get tags where they would otherwise be obscured by macros.
# Run at the root of the project.

mv game/game_structs.h{,.bak}
sed -e 's/#define *TYPE */TYPE(/' \
    -e 's/^ *//'                  \
    game/game_structs.h.bak       \
    | awk -F'('                   \
    '                   { nom=1; }
     $1 ~ "^TYPE"       { nom=0; type=$2;
                          print "typedef struct " $2 "_t {"; }
     $1 ~ "^STRUCT" ||    
     $1 ~ "^EXPOSE" ||
     $1 ~ "^HIDE"   ||
     $1 ~ "^)"          { nom=0; print "//" $0; }
     $1 ~ "endstruct.h" { nom=0; print "}",type,";"; }
     nom                { print $0; }
    ' >game/game_structs.h

ctags {engine,game}/*.[ch]
cscope -b -I engine -I game -s engine -s game

mv game/game_structs.h{.bak,}
