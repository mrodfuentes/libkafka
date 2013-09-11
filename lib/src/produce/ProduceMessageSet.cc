///////////////////////////////////////////////////////////////////////////
//
// libkafka - C/C++ client for Apache Kafka v0.8+
//
// David Tompkins -- 8/8/2013
// tompkins@adobe_dot_com
//
///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2013 Adobe Systems Incorporated. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
///////////////////////////////////////////////////////////////////////////

#include <string>
#include <iostream>

#include "ProduceMessageSet.h"

using namespace std;

namespace LibKafka {

ProduceMessageSet::ProduceMessageSet(Packet *packet) : WireFormatter(), PacketWriter(packet)
{
  D(cout.flush() << "--------------ProduceMessageSet(buffer)\n";)

  // Kafka Protocol: int partition
  this->partition = this->packet->readInt32();

  // Kafka Protocol: int messageSetSize
  this->messageSetSize = this->packet->readInt32();

  // Kafka Protocol: MessageSet messagesSet
  this->messageSet = new MessageSet(this->messageSetSize, this->packet);

  this->releaseArrays = true;
}

ProduceMessageSet::ProduceMessageSet(int partition, int messageSetSize, MessageSet *messageSet, bool releaseArrays) : WireFormatter(), PacketWriter()
{
  D(cout.flush() << "--------------ProduceMessageSet(params)\n";)

  this->partition = partition;
  this->messageSetSize = messageSetSize;
  this->messageSet = messageSet;
  this->releaseArrays = releaseArrays;
}

ProduceMessageSet::~ProduceMessageSet()
{
  if (this->releaseArrays)
  {
    delete this->messageSet;
  }
}

unsigned char* ProduceMessageSet::toWireFormat(bool updatePacketSize)
{
  D(cout.flush() << "--------------ProduceMessageSet::toWireFormat()\n";)
  
  // Kafka Protocol: int partition
  this->packet->writeInt32(this->partition);

  // Kafka Protocol: int messageSetSize
  this->packet->writeInt32(this->messageSetSize);

  // Kafka Protocol: MessageSet messageSet
  this->messageSet->packet = this->packet;
  this->messageSet->toWireFormat(false);

  if (updatePacketSize) this->packet->updatePacketSize();
  return this->packet->getBuffer();
}

int ProduceMessageSet::getWireFormatSize(bool includePacketSize)
{
  D(cout.flush() << "--------------ProduceMessageSet::getWireFormatSize()\n";)
  
  // Packet.size
  // partition + messageSetSize + messageSet

  int size = 0;
  if (includePacketSize) size += sizeof(int);
  size += sizeof(int) + sizeof(int) + this->messageSetSize;
  return size;
}

ostream& operator<< (ostream& os, const ProduceMessageSet& pm)
{
  os << pm.partition << ":" << pm.messageSetSize << "\n";
  return os;
}

}; // namespace LibKafka
