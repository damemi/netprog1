// Michael Dame, Chris Dower
// Network Programming Project 1

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <arpa/inet.h>
#include <map>

using namespace std;

// Structure for storing routing table entries in search trie
//      also stores non-compressed path nodes
// left: Pointer to left node (0)
// right: Pointer to right node (1)
//
// These variables will be null for non-compressed path nodes:
// ipAddr: IP Address without slash/subnet
// subnet: Subnet mask size without the slash (ie, 32)
// gateway: Gateway for routing entry
// intf: Interface for routing entry
struct Node {
  Node *left = NULL;
  Node *right = NULL;
  string ipAddr = "";
  string subnet = "";
  string gateway = "";
  string intf = "";
};

// Create root node of trie
Node root;
Node *rootNode = &root;

//Create hash map of arp table
map<string, string> arpTable;

bool checkGateway(Node* SourceNode, string destAddr, Node* destNode);
Node* findSourceNode(string sourceAddr);

int main() {

  // Open routing table entries file
  string line;
  ifstream routes("routes.txt");
  if(routes.is_open()) {
    string cidrAddr, subnet, gateway, intf, ipAddr;

    rootNode->left = NULL;
    rootNode->right = NULL;
    while(getline(routes, line)) {
      if(line.size() <= 0)
      continue;

      // Creates a vector to store the data for each line
      // TODO: Should have some error checking
      // 0: Full CIDR-notation address (ie, with the slash)
      // 1: Gateway address
      // 2: Interface
      stringstream ss(line);
      string buf;
      vector<string> route;
      while(ss >> buf) {
        route.push_back(buf);
      }
      string::size_type n;

      // Parse out the slash and separate the subnet size from
      // the actual address for ease-of-use
      cidrAddr = route[0];
      n = cidrAddr.find("/");
      ipAddr = cidrAddr.substr(0,n);
      subnet = cidrAddr.substr(n+1);
      gateway = route[1];
      intf = route[2];

      // Convert IP address dot-notation to binary representation,
      // so we can traverse the trie bit by bit
      struct sockaddr_in sa;
      inet_pton(AF_INET, ipAddr.c_str(), &(sa.sin_addr));
      int addr = htonl(sa.sin_addr.s_addr);

      // DEBUG: Print ip address in binary
      /*
      for(int i=0; i<32; i++) {
      if(addr & (1<<(31-i))) {
      cout << "1";
    } else {
    cout << "0";
  }
}
cout << endl;
*/

// Build the trie
// Make a new pointer node, take the bits of the address and
// make that pointer either the left or right child based on the
// current bit in the address.
// Only goes as deep as the subnet mask size in the node's address
// (See lecture 2 slide 22 for example of what I mean)
Node *ptr = rootNode;
for(int i=0; i<atoi(subnet.c_str()); i++) {
  Node *newNode = new Node();
  if (addr & (1<<(31-i))) { // bit is 1
    if(ptr->right == NULL) {
      ptr->right = newNode;
    } else {
      ptr = ptr->right;
      continue;
    }

  } else { // bit is 0;
    if(ptr->left == NULL) {
      ptr->left = newNode;
    } else {
      ptr = ptr->left;
      continue;
    }
  }
  ptr = newNode;
}

// Set routing table details
ptr->ipAddr = ipAddr;
ptr->subnet = subnet;
ptr->gateway = gateway;
ptr->intf = intf;
}
routes.close();
}

// DEBUG: Just testing the trie with some routing table entries from pdus.txt
/*
struct sockaddr_in sa2;
inet_pton(AF_INET, "10.4.0.1", &(sa2.sin_addr));
int addr2 = htonl(sa2.sin_addr.s_addr);
Node * newPtr = rootNode;
for(int i=0; i<16; i++) {
if(addr2 & (1<<(31-i))) {
if(newPtr->right != NULL)
newPtr = newPtr->right;
} else {
if(newPtr->left != NULL)
newPtr = newPtr->left;
}
if(strlen((newPtr->ipAddr).c_str()) > 0)
cout << newPtr->ipAddr << endl;
}
*/

// Read in ARP data file to build ARP table
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
    arpTable[ip] = mac;

    //cout << "IP: " << ip << endl;
    //cout << "\tMAC: " << arpTable[ip] << endl;
  }
  arp.close();
}

// Read in PDU file
ifstream pdus("pdus.txt");
if(pdus.is_open()) {
  string intf, sourceAddr, destAddr, protocolNum, ttl, sourcePort, destPort;
  int ttl_i;

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
    ttl_i = stoi(ttl);
    ttl_i--;

    cout << sourceAddr << ":" << sourcePort << "->" << destAddr << ":" << destPort;
    if(ttl_i - 1 <= 0 ){
      cout << " discarded (TTL expired)" << endl;
    }else{
      Node* trackerNode = NULL;
      Node* destNode = rootNode;
      //Find node for destAddr
      struct sockaddr_in da;
      inet_pton(AF_INET, destAddr.c_str(), &(da.sin_addr));
      int addr = htonl(da.sin_addr.s_addr);
      for(int i=0; i<32; i++) {
        if(addr & (1<<(31-i))) {
          if(destNode->ipAddr != ""){
            trackerNode = destNode;
            //cout << "Found trackerNode" << endl;
          }
          if(destNode->right != NULL ){
            destNode = destNode->right;
          }else{ break; }
        } else {
	  if(destNode->ipAddr != ""){
	    trackerNode = destNode;
          }
          if(destNode->left != NULL){
            destNode = destNode->left;
          }else{ break; }
        }
      }
      if(destNode->ipAddr == "" && trackerNode !=NULL){
        destNode = trackerNode;
      }
      Node* SourceNode = findSourceNode(sourceAddr);
      //if(SourceNode!= NULL) { cout << "Source Node Found" << endl; }
      //if(destNode->ipAddr==""){
      if(destNode->intf == "" /*|| destNode->gateway.find("0.0.0.0") != string::npos*/) {
        cout << " discarded (destination unreachable)" << endl;
      }else{
        if(checkGateway(SourceNode, destAddr, destNode)){
          cout << " directly connected (" << destNode->intf << "-" << arpTable[destAddr] << ") ttl " << ttl_i << endl;
          //cout << destNode->ipAddr << endl;
        }else{
          if(destNode==NULL){
            cout << "failure" << endl;
          }
          cout << " via " << destNode->gateway << "(" << destNode->intf;
          if(destNode->intf.find("ppp") != 0){
            cout << "-" << arpTable[destNode->gateway] << ") ttl " << ttl_i << endl;
          }else{ cout << ") ttl " << ttl_i << endl; }
        }
      }
      //cout << SourceNode->gateway << endl;
    }
  }
  pdus.close();
}

return 0;
}

bool checkGateway(Node* SourceNode, string destAddr, Node* destNode){
  bool directConnection = false, gatewayConnection = false;
  struct sockaddr_in ga;
  Node* trackerNode, * gatewayNode = rootNode;
  inet_pton(AF_INET, SourceNode->gateway.c_str(), &(ga.sin_addr));
  //cout << "\nGateway: " << SourceNode->gateway << endl;
  //Check if dest addr has direct connection to sourceAddr
  int sGatewayAddr = htonl(ga.sin_addr.s_addr);
  for(int i=0; i<32; i++) {
    if(sGatewayAddr & (1<<(31-i))) {
      if(gatewayNode->ipAddr != ""){
        trackerNode = gatewayNode;
        //cout << trackerNode->ipAddr << endl;
      }
      if(gatewayNode->right != NULL ){
        gatewayNode = gatewayNode->right;
      }else{ break; }
    } else {
      if(gatewayNode->left != NULL){
        gatewayNode = gatewayNode->left;
      }else{ break; }
    }
  }
  //if(gatewayNode == NULL){ cout << "gatewayNode is null" << endl; }
  /*if(gatewayNode->ipAddr == ""){
    //cout << "gatewayNode empty" << endl;
    if(destNode!=NULL){
      //cout << destNode << endl;
      cout << "\n " << destNode->ipAddr << "\t-->\t" << destNode->gateway << endl;
    }
    //cout << " discarded (destination unreachable)" << endl;

  }*/
  if(trackerNode!=NULL && gatewayNode!=NULL){
    if(destNode->ipAddr == ""){
      destNode = trackerNode;
      //cout <<trackerNode->ipAddr << endl;
    }
    if(gatewayNode == destNode){
      directConnection = true;
    }else{
      gatewayConnection = true;
    }
    if(gatewayConnection){
      destNode = gatewayNode;
    }
  }//else{ cout << "failure" << endl; }
  //cout << ": finished " << endl;
  //cout << "\n" << directConnection + gatewayConnection << endl;
  return directConnection;
}

Node* findSourceNode(string sourceAddr){
  struct sockaddr_in sa;
  string nextGateway, sourceGateway;
  inet_pton(AF_INET, sourceAddr.c_str(), &(sa.sin_addr));
  int addr = htonl(sa.sin_addr.s_addr);
  Node* currNode = rootNode;

  for(int i=0; i<32; i++) {
    if(addr & (1<<(31-i))) {
      if(currNode->right != NULL ){
        currNode = currNode->right;
      }else{ break; }
    } else {
      if(currNode->left != NULL){
        currNode = currNode->left;
      }else{ break; }
    }
  }
  nextGateway = currNode->gateway;
  return currNode;
}
