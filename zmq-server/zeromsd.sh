#!/bin/bash

# source function library

. /etc/rc.d/init.d/functions

usage(){

  echo " usage:$0 {start|stop|restart} "

}

start(){

  echo "Start."

}

stop(){

 echo "Stop."

}

restart(){

  stop

  start

}

#main function

case "$1" in

  start)

     start

     ;;

  stop)

     stop

     ;;

  restart)

     restart

     ;;

  *)

     usage

     ;;

esac

exit $?