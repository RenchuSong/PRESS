//
//  main.cpp
//  press-v3
//
//  Created by Renchu Song on 5/13/18.
//  Copyright © 2018 Renc. All rights reserved.
//

#include <iostream>
#include "io/file_reader.hpp"
#include "io/file_writer.hpp"
#include "topology/graph.hpp"

int main(int argc, const char * argv[]) {
//  FileWriter fw("/Users/songrenchu/Develop/test2.txt", true);
//  fw.writeChar('a');
//  fw.writeInt(123);
//  char* a = new char[4];
//  a[0] = 'A';
//  a[1] = 'B';
//  a[2] = 'C';
//  a[3] = '\0';
//  const char* b = a;
//  fw.writeString(b);
//  fw.writeDouble(3.456);
//  fw.writeChar(',');
//  fw.writeLong(23423);
//  fw.writeChar(',');
//  fw.writeShort(2);
//  
//
  
//  FileReader fr("/Users/songrenchu/Develop/test2.txt", true);
//  std::cout << fr.nextChar() << std::endl;
//  std::cout << fr.nextInt() << std::endl;
//  std::cout << fr.nextString() << std::endl;
//  std::cout << fr.nextDouble() << std::endl;
//  std::cout << fr.nextChar() << std::endl;
//  std::cout << fr.nextLong() << std::endl;
//  std::cout << fr.nextChar() << std::endl;
//  std::cout << fr.nextShort() << std::endl;
  
  Node n(10, 20);
  
  n.print();
  
  n.addEdge(1);
  n.addEdge(5);
  n.addEdge(6);
  n.addEdge(4);
  
  n.print();
  std::cout << n.getEdgeNumber() << std::endl;
  std::cout << n.getEdgeId(1) << std::endl;
  std::cout << n.getEdgeId(3) << std::endl;
  std::cout << n.getEdgeId(5) << std::endl;
  
  
  
  
//  // insert code here...
//  FileReader fp("/Users/songrenchu/Develop/test.txt", (char *)"r");
//  std::cout << fp.isBinary() << std::endl;
//  std::cout << fp.fileOpened() << std::endl;
//  
//  
//  std::cout << fp.nextInt() << std::endl;
//  std::cout << fp.nextChar() << std::endl;
//  std::cout << fp.nextLong() << std::endl;
//  std::cout << fp.nextChar() << std::endl;
//  std::cout << fp.nextDouble() << std::endl;
//  std::cout << fp.nextChar() << std::endl;
//  std::cout << fp.nextString(1024) << std::endl;
  
  
  return 0;
}
