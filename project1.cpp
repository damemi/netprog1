// Michael Dame, Chris Dower
// Network Programming Project 1

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <arpa/inet.h>

using namespace std;

struct Node {
  Node *left;
  Node *right;
  string ipAddr;
  string subnet;
  string gateway;
  string intf;
};

Node rootNode;

int main() {
  string line;
  ifstream routes("routes.txt");
  if(routes.is_open()) {
    string cidrAddr, subnet, gateway, intf, ipAddr;

    rootNode.left = NULL;
    rootNode.right = NULL;
    while(getline(routes, line)) {
      if(line.size() <= 0)
	continue;

      stringstream ss(line);
      string buf;
      vector<string> route;
      while(ss >> buf) {
	route.push_back(buf);
      }
      string::size_type n;
      cidrAddr = route[0];
      n = cidrAddr.find("/");
      ipAddr = cidrAddr.substr(0,n);
      subnet = cidrAddr.substr(n+1);
      gateway = route[1];
      intf = route[2];

      Node tableEntry;
      tableEntry.ipAddr = ipAddr;
      tableEntry.subnet = subnet;
      tableEntry.gateway = gateway;
      tableEntry.intf = intf;

      struct sockaddr_in sa;
      inet_pton(AF_INET, ipAddr.c_str(), &(sa.sin_addr));
      int addr = htonl(sa.sin_addr.s_addr);
      for(int i=0; i<atoi(subnet.c_str()); i++) {
	if (addr & (1<<(31-i))) { // bit is 1
	  cout << "1";
	} else { // bit is 0;
	  cout << "0";
	}
      }
      cout << endl;

      cout << "ADDRESS: " << ipAddr << endl;
      cout << "\tSUBNET: " << subnet << endl;
      cout << "\tGATEWAY: " << gateway << endl;

    }
    routes.close();
  }

  ifstream arp("arp.txt");
  if(arp.is_open()) {
    string ip, mac;
    while(getline(arp, line)) {
      if(line.size() <= 0)
	continue;

      stringstream ss(line);
      string buf;
      vector<string> route;
      while(ss >> buf) {
	route.push_back(buf);
      }
      ip = route[0];
      mac = route[1];
      
      cout << "IP: " << ip << endl;
      cout << "\tMAC: " << mac << endl;
    }
    arp.close();
  }

  ifstream pdus("pdus.txt");
  if(pdus.is_open()) {
    string intf, sourceAddr, destAddr, protocolNum, ttl, sourcePort, destPort;

    while(getline(pdus,line)) {
      if(line.size() <= 0)
	continue;

      stringstream ss(line);
      string buf;
      vector<string> route;
      while(ss >> buf) {
	route.push_back(buf);
      }
      intf = route[0];
      sourceAddr = route[1];
      destAddr = route[2];
      protocolNum = route[3];
      ttl = route[4];
      sourcePort = route[5];
      destPort = route[6];

      cout << line << endl;
    }
    pdus.close();
  }

  return 0;
}
