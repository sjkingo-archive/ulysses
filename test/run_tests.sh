#!/bin/bash

# test/run_test.sh - test runner
# part of Ulysses, a tiny operating system
# 
# Copyright (C) 2008, 2009 Sam Kingston <sam@sjkwi.com.au>
# 
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option)
# any later version.
# 
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.
# 
# You should have received a copy of the GNU General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>.

# Exit statuses:
#  0 = all tests passed
#  1 = scons failed (so compiler or linker error)
#  2 = could not cd into test module directory
#  3 = one or more tests failed

# -v is verbosity
if [ "$1" == "-v" ] ; then
    verbose=1
else
    verbose=0
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

    # determine where to send test case output - we do this here since it's
    # inside the test case's subdir
    if [ $verbose -eq 1 ] ; then
        exec 3>.output
    else
        exec 3>/dev/null
    fi

    # run the test case, putting its output in a file (or null if no -v), and
    # capture time's output
    time_out="`(time ./main 1>&3 2>&3) 2>&1`"
    status=$?

    # if there is any output, print it now
    if [ -f "./.output" ] ; then
        cat ./.output
        rm -f ./.output
    fi

    # extract the actual amount of time the command took (real)
    time="`echo $time_out | grep real | awk '{ print $2 }'`"

    # output if the test failed or not
    if [ $status -ne 0 ] ; then
        echo "FAILED: $dir/main exited with status $status in $time"
        failed=$((failed+1))
    else 
        echo "SUCCESS: $dir/main in $time"
    fi

    popd >/dev/null
    echo
done

echo "$((total-failed))/$total tests passed"
scons -c >/dev/null

if [ $failed -ne 0 ] ; then
    exit 3
fi
exit 0
