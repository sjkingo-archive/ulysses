#!/bin/bash

let total=0

for ext in c h s sh orig ; do
    let count=0
    for i in `find . -name \*.$ext -exec wc -l {} \; | awk '{ print $1 }'` ; do 
        count=$(($count+$i))
    done
    total=$(($total+$count))
    echo "*.$ext: $count"
done

echo "total: $total"

