countErrors=0
countWarnings=0

text=$(gcc -Wall -o "`echo "$1" | rev | cut -c 2- | rev`out" $1 2>&1)

while read -r line
do
    if [ -n "`echo "$line" | grep -E "^$1:[0-9]+:[0-9]+:[^:]*error:"`" ]
    then
        countErrors=`expr $countErrors + 1`
    elif [ -n "`echo "$line" | grep -E "^$1:[0-9]+:[0-9]+:[^:]*warning:"`" ]
    then
        countWarnings=`expr $countWarnings + 1`
    fi
done <<< "$text"

echo "$countErrors"
echo "$countWarnings"