#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "USAGE: $0 host port"
    exit 0
fi

USERNAME="Anonymous"
PASS=""
HOME="/home/ugolc"
ROOT="/"
DIR="3kd4WDl3J"

HOST=$1
PORT=$2
MKFIFO=`which mkfifo`
PIPE=fifo
OUT=outfile
TAIL=`which tail`
NC="`which nc` -C"
TIMEOUT=1 #max time before reading server response

red=`tput setaf 1`
green=`tput setaf 2`
reset=`tput sgr0`

getcode()
{
  sleep $TIMEOUT
  local code=$1
  echo "Waiting for $code reply-code"
  local data=`$TAIL -n 1 $OUT |cat -e |grep "^$code.*[$]$" |wc -l`
  return $data
}

print_failed()
{
    echo "${red}$1 test failed"
    echo "Expected reply-code: $2"
    echo "Received : ["`$TAIL -n 1 $OUT| cat -e`"]"
    echo "KO${reset}"
    echo "--------------------------------------"
}

print_succeeded()
{
  echo "$1 test succeeded"
  echo "--------------------------------------"
  kill_client 2>&1 >/dev/null
}

launch_client()
{
  local host=$1
  local port=$2

  $MKFIFO $PIPE
  ($TAIL -f $PIPE 2>/dev/null | $NC $host $port &> $OUT &) >/dev/null 2>/dev/null

  echo "Connecting to $host : $port"
  sleep $TIMEOUT
  getcode 220
  if [[ $? -eq 1 ]]; then
    echo "${green}Reply-code OK${reset}"
    return 1
  else
    echo "${red}Connection to $host:$port failed"
    echo "Expected reply-code: 220"
    echo "Received : ["`tail -n 1 $OUT |cat -e`"]"
    echo "KO${reset}"
    kill_client
    return 0
  fi  
}

launch_test()
{
  local test_name=$1
  local cmd=$2
  local code=$3

  echo "Sending [$cmd^M$]"
  echo "$cmd" >$PIPE
  getcode $code
  if [[ ! $? -eq 1 ]]; then
    print_failed "$test_name" "$code"
    kill_client
    clean
    return 0
  else
    echo "${green}Reply-code OK${reset}"
    return 1
  fi
}

launch_pasv()
{
  local test_name=$1
  local cmd=$2
  local code=$3

  echo "Sending [$cmd^M$]"
  echo "$cmd" >$PIPE

  local data=`$TAIL -n 1 $OUT |cat -e |sed "s/[^(0-9,]*//g"`
  local p1=`$TAIL -n 1 $OUT |cat -e |sed "s/[^(0-9,]*//g"|grep -Eo "[0-9]+,[0-9]+$"|cut -d, -f1`
  local p2=`$TAIL -n 1 $OUT |cat -e |sed "s/[^(0-9,]*//g"|grep -Eo "[0-9]+,[0-9]+$"|cut -d, -f2`
  PORT2=`expr 256 \* $p1`
  PORT2=`expr $PORT2 + $p2`

  getcode $code
  if [[ ! $? -eq 1 ]]; then
    print_failed "$test_name" "$code"
    kill_client
    clean
    return 0
  else
    echo "${green}Reply-code OK${reset}"
    return 1
  fi
}

kill_client()
{
  local nc=`which nc`

  if [ `pidof $TAIL | wc -l` -ne 0 ]
  then
    killall $TAIL &>/dev/null
  fi
  if [ `pidof $nc | wc -l` -ne 0 ]
  then
    killall $nc &>/dev/null
  fi
  rm -f $PIPE $OUT &> /dev/null
}

clean()
{
  rm -f $PIPE $OUT log &>/dev/null
}



# Simple authentication with USER + PASS command
test00_03()
{
  echo "test00_03"
  local test_name="Simple authentification with USER + PASS command"

  local cmd1="USER $USERNAME"
  local cmd2="PASS $PASS"

  launch_client $HOST $PORT
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd1" 331
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd2" 230
  if [[ ! $? -eq 1 ]]; then return; fi

  print_succeeded "$test_name"
  return
}

# Sending request before authentification
test05_01()
{
  echo "test05_01"
  local test_name="Sending request before authentification"

  local cmd1="PWD"

  launch_client $HOST $PORT
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd1" 530
  if [[ ! $? -eq 1 ]]; then return; fi

  print_succeeded "$test_name"
  return
}

# Wrong user authentification
test05_02()
{
  echo "test05_02"
  local test_name="Wrong user authentification"

  local cmd1="USER tupueducul"
  local cmd2="PASS $PASS"

  launch_client $HOST $PORT
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd1" 331
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd2" 530
  if [[ ! $? -eq 1 ]]; then return; fi

  print_succeeded "$test_name"
  return
}

# test_06_02 Testing in a different directory
test06_02()
{
  echo "test06_02"
  local test_name="PWD in different directory"

  local cmd1="USER $USERNAME"
  local cmd2="PASS $PASS"
  local cmd3="PWD"
  local cmd4="CWD $ROOT"
  local cmd5="PWD"

  launch_client $HOST $PORT
  if [[ ! $? -eq 1 ]]; then return; fi

  launch_test "$test_name" "$cmd1" 331
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd2" 230
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd3" 257
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd4" 250
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd5" 257
  if [[ ! $? -eq 1 ]]; then return; fi

  print_succeeded "$test_name"
  return
}

# test_06_03 Empty test
test06_03()
{
  echo "test06_03"
  local test_name="CWD empty test"

  local cmd1="USER $USERNAME"
  local cmd2="PASS $PASS"
  local cmd3="CWD"

  launch_client $HOST $PORT
  if [[ ! $? -eq 1 ]]; then return; fi

  launch_test "$test_name" "$cmd1" 331
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd2" 230
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd3" 550
  if [[ ! $? -eq 1 ]]; then return; fi

  print_succeeded "$test_name"
  return
}

# test_06_05 valid directory
test06_05()
{
  echo "test06_05"
  local test_name="CWD to a valid directory"

  local cmd1="USER $USERNAME"
  local cmd2="PASS $PASS"
  local cmd3="CWD $PWD/$DIR"
  local cmd4="PWD"

  launch_client $HOST $PORT
  launch_test "$test_name" "$cmd1" 331
  launch_test "$test_name" "$cmd2" 230
  launch_test "$test_name" "$cmd3" 250
  launch_test "$test_name" "$cmd4" 257

  print_succeeded "$test_name"
  return
}

# test_06_06 parent directory using CDUP
test06_06()
{
  echo "test06_06"
  local test_name="CDUP"

  local cmd1="USER $USERNAME"
  local cmd2="PASS $PASS"
  local cmd3="CWD $HOME"
  local cmd4="CDUP"
  local cmd5="PWD"

  launch_client $HOST $PORT
  if [[ ! $? -eq 1 ]]; then return; fi

  launch_test "$test_name" "$cmd1" 331
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd2" 230
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd3" 250
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd4" 250
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd5" 257
  if [[ ! $? -eq 1 ]]; then return; fi

  print_succeeded "$test_name"
  return
}

# test_06_07_parent directory using CWD
test06_07()
{
  echo "test06_07"
  local test_name="CWD to parent directory"

  local cmd1="USER $USERNAME"
  local cmd2="PASS $PASS"
  local cmd3="CWD .."
  local cmd4="PWD"

  launch_client $HOST $PORT
  if [[ ! $? -eq 1 ]]; then return; fi

  launch_test "$test_name" "$cmd1" 331
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd2" 230
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd3" 250
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd4" 257
  if [[ ! $? -eq 1 ]]; then return; fi

  print_succeeded "$test_name"
  return
}

# test_07_00 quit
test07_00()
{
  echo "test07_00"
  local test_name="QUIT"

  local cmd1="USER $USERNAME"
  local cmd2="PASS $PASS"
  local cmd3="QUIT"

  launch_client $HOST $PORT
  if [[ ! $? -eq 1 ]]; then return; fi

  launch_test "$test_name" "$cmd1" 331
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd2" 230
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd3" 221
  if [[ ! $? -eq 1 ]]; then return; fi
  print_succeeded "$test_name"
  return
}

# test_08_01 wrong command
test08_01()
{
  echo "test08_01"
  local test_name="Wrong command"

  local cmd1="USER $USERNAME"
  local cmd2="PASS $PASS"
  local cmd3="THISDOESNOTWORK arg"

  launch_client $HOST $PORT
  if [[ ! $? -eq 1 ]]; then return; fi

  launch_test "$test_name" "$cmd1" 331
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd2" 230
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd3" 500
  if [[ ! $? -eq 1 ]]; then return; fi
  print_succeeded "$test_name"
  return
}

# test_08_02 only spaces
test08_02()
{
  echo "test08_02"
  local test_name="Only spaces"

  local cmd1="USER $USERNAME"
  local cmd2="PASS $PASS"
  local cmd3="                    "

  launch_client $HOST $PORT
  if [[ ! $? -eq 1 ]]; then return; fi

  launch_test "$test_name" "$cmd1" 331
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd2" 230
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd3" 500
  if [[ ! $? -eq 1 ]]; then return; fi
  print_succeeded "$test_name"
  return
}

# test_08_03 noop
test08_03()
{
  echo "test08_03"
  local test_name="No operation"

  local cmd1="USER $USERNAME"
  local cmd2="PASS $PASS"
  local cmd3="NOOP"

  launch_client $HOST $PORT
  if [[ ! $? -eq 1 ]]; then return; fi

  launch_test "$test_name" "$cmd1" 331
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd2" 230
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd3" 200
  if [[ ! $? -eq 1 ]]; then return; fi
  print_succeeded "$test_name"
  return
}

# test_08_04 help
test08_04()
{
  echo "test08_04"
  local test_name="Help"

  local cmd1="USER $USERNAME"
  local cmd2="PASS $PASS"
  local cmd3="HELP"

  launch_client $HOST $PORT
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd1" 331
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd2" 230
  if [[ ! $? -eq 1 ]]; then return; fi
  launch_test "$test_name" "$cmd3" 214
  if [[ ! $? -eq 1 ]]; then return; fi
  print_succeeded "$test_name"
  return
}


test00_03
test05_01
test05_02
test06_02
test06_03
test06_05
test06_06
test06_07
test07_00
test08_01
test08_02
test08_03
test08_04
clean
