#!/bin/sh

if [ -f $TNH_EXEC ];
then
    $TNH_EXEC &
else
    echo "$TNH_EXEC not found"
    exit
fi

# Wait for any processes to exit
wait

# Exit with process exit code
exit $?