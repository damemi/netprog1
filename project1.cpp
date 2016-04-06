// Michael Dame, Chris Dower
// Network Programming Project 1

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

int main() {
  string line;
  ifstream routes("routes.txt");
  if(routes.is_open()) {
    string address, subnet, gateway, intf;
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
      address = route[0];
      n = address.find("/");
      subnet = address.substr(n+1);
      gateway = route[1];
      intf = route[2];

      cout << "ADDRESS: " << address << endl;
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
