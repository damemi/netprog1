# netprog1

Your program must read a routing table from the file routes.txt in the current working directory. Each line in this file represents a routing table entry. The routing table will have three whitespace-delimited columns. Column 1 will be an IPv4 prefix in the format a.b.c.d/x, where a.b.c.d is an IPv4 address and x is the prefix length. Column 2 will be an IPv4 gateway address, again in a.b.c.d format. If the gateway address is 0.0.0.0 this indicates that the network given by the prefix is directly connected. Column 3 will contain an interface identifier (i.e. eth0, eth1, ppp0). Use the routing table entries given to construct a binary search trie of the type described in class. Note that if the gateway address is 0.0.0.0, and the prefix length is 32, this indicates a direct, point-to-point connection.

Our simulation will not include ARP requests or replies; instead, we’ll read the address mappings ARP would provide from a file arp.txt. Each line again represents a table entry and has two whitespace-delimited columns: the first is an IP address and the second is an Ethernet hardware address in xx:xx:xx:xx:xx:xx (hexadecimal) format. Read this file into a data structure of your choice - I’d recommend a map or hash table.

Next, you will read PDU header information from the standard input. Each line represents an IPv4 PDU and contains six whitespace-delimited fields as shown below.

Field	Value
0	Interface on which PDU arrived
1	Source IPv4 address (a.b.c.d)
2	Destination IPv4 address (a.b.c.d format)
3	IPv4 protocol number (decimal integer)
4	Time to live (decimal integer)
5	Source port number
6	Destination port number

For each PDU read from standard input, first use the routing table to determine the next hop destination and interface. Then, if needed, use the ARP table to determine the data-link layer address to be use. (If the routing table entry for the next hop address indicates that the interface is point-to-point, do not use the ARP table. You will need to do a second routing table lookup to determine this.) Decrement the TTL field. Finally, print a line in one of the following formats:

1.2.3.4:11->5.6.7.8:22 via 9.10.11.12(eth0-aa:bb:cc:dd:ee:ff) ttl 11
10.11.12.13:33->20.21.22.23:44 via 30.40.50.60(ppp0) ttl 9
1.2.3.4:11->5.6.7.8:22 discarded (TTL expired)
1.2.3.4:22->99.88.77.66:33 discarded (destination unreachable)

That is:
source:src_port->dest:dest_port via gateway(iface-l2address) ttl x
source:src_port->dest:dest_port via gateway(pppinterface) ttl x

Use the first format for ARP-capable (non-point-to-point) interfaces and the second for point-to-point. If the TTL reaches zero or no route is found in the table, print a message to that effect.
