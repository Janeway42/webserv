#!/bin/bash

# COLORS
RED='\033[31m'
GRE='\033[32m'
GRY='\033[1;30m'
YEL='\033[33m'
BLU='\033[34m'
RES='\033[0m'


if  ! command -v siege &> /dev/null ; then
  echo -e  "$RED\n siege is not installed $RES \n" 1>&2
  exit 1
fi

mkdir outputFiles
rm -f ./outputFiles/webservOutput.txt
outputOut="./outputFiles/stdOut.txt"
outputErr="./outputFiles/stdErr.txt"
portNr="8080"
NrSeconds=5
TIME="-t${NrSeconds}s"

rm $outputOut
rm $outputErr

echo -e  "$GRE\n ~ ~ ~ TEST SIEGE ~ ~ ~ $RES \n" 1>&2
printf "$GRE Start the Webserv  $RES \n" 1>&2

# Run WebServ
rm -f ./tests/siege/outputFiles/webservOutput.txt
cd ../../ ;  ./webserv >> ./tests/siege/outputFiles/webservOutput.txt & 
# cd ../../ ;  ./webserv >> /dev/null & 
cd tests/siege/
sleep 2     # Wait that the Webserver starts


# !!! It does not work with -p flag: -p 8080 

printf "$GRE Testing Siege: $GRY http://localhost:$portNr/index.html $RES \n" 1>&2

function countdown {
    duration=$NrSeconds
    while [[ $duration -gt 0 ]]; do
        minutes=$((duration / 60))
        seconds=$((duration % 60))
        # echo "Time left: ${minutes}:${seconds}"
        printf "    Time left: %02d:%02d\r" "$minutes" "$seconds"
        sleep 1
        duration=$((duration - 1))
    done
    printf "\n";
}


echo -e "$BLU   TEST: 1 user, $TIME seconds $RES" 1>&2
countdown &
siege -b -c1 $TIME http://localhost:$portNr/index.html >> $outputOut 2>> $outputErr

echo -e "$BLU   TEST: 20 users, $TIME seconds $RES" 1>&2
countdown &
siege -b -c20 $TIME http://localhost:$portNr/index.html >> $outputOut 2>> $outputErr

echo -e "$BLU   TEST: 50 users, $TIME seconds $RES" 1>&2
countdown &
siege -b -c50 $TIME http://localhost:$portNr/index.html >> $outputOut 2>> $outputErr

echo -e "$BLU   TEST: 100 users, $TIME seconds $RES" 1>&2
countdown &
siege -b -c100 $TIME http://localhost:$portNr/index.html >> $outputOut 2>> $outputErr

echo -e "$BLU   TEST: 150 users, $TIME seconds $RES" 1>&2
countdown &
siege -b -c150 $TIME http://localhost:$portNr/index.html >> $outputOut 2>> $outputErr

# echo -e "$BLU   TEST: 200 users, $TIME seconds $RES" 1>&2
# siege -b -c200 $TIME http://localhost:$portNr/index.html >> $outputOut 2>> $outputErr

# echo -e "$BLU   TEST: 250 users, $TIME seconds $RES" 1>&2
# siege -b -c200 $TIME http://localhost:$portNr/index.html >> $outputOut 2>> $outputErr

sleep 0.3
pkill -f webserv
cat ./outputFiles/stdErr.txt | grep Availability 1>&2

echo -e "\n$GRE TEST SIEGE FINISHED, CHECK THE OUTPUT RESULTS IN 'tests/siege/outputFiles/stdOut.txt' $RES\n" 1>&2