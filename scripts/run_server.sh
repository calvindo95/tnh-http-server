#!/bin/sh

if [ -f $TNH_EXEC ];
then
    $TNH_EXEC &
else
    echo "$TNH_EXEC not found"
    exit
fi

#if [ -f $EGS_EXEC ];
#then
#    $EGS_EXEC &
#else
#    echo "$EGS_EXEC not found"
#    exit
#fi

# Wait for any processes to exit
wait

# Exit with process exit code
exit $?