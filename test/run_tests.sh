#!/bin/bash

# -v is verbosity
if [ "$1" == "-v" ] ; then
    exec 3>&1
else
    exec 3>/dev/null
fi

total=0
failed=0

for dir in `ls -l | grep ^d | awk '{ print $8 }'` ; do
    total=$((total+1))
    echo "==================================="
    echo "Test #$total: $dir"
    echo "==================================="
    scons -Q $dir || exit 1
    echo "-----------------------------------"
    
    pushd $dir >/dev/null || exit 2
    ./main >&3
    status=$?
    if [ $status -ne 0 ] ; then
        echo "FAILED: $dir/main exited with status $status"
        failed=$((failed+1))
    else 
        echo "SUCCESS: $dir/main"
    fi
    popd >/dev/null
    echo
done

echo "$((total-failed))/$total tests passed"
scons -c >/dev/null

if [ $failed -ne 0 ] ; then
    exit 1
fi
