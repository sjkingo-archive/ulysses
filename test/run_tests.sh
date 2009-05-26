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
    exit 3
fi
exit 0
