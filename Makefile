traceroute: 
	gcc -std=gnu17 -Wall -Wextra -o traceroute icmp_send.h icmp_send.c icmp_receive.h icmp_receive.c main.c

clean:
	$(RM) *.o

distclean:
	$(RM) traceroute *.o
