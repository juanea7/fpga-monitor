#!/bin/bash

for i in {0..2000}
do
   echo -e "\n\nIteration #$i\n\n"
    ./main
    mv CON.BIN "traces/CON_$i.BIN"
    mv SIG.BIN "traces/SIG_$i.BIN"
done

echo "End"
