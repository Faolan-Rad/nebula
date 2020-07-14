//------------------------------------------------------------------------------
//  messagereaderwritertest.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "messagereaderwritertest.h"
#include "messaging/messagereader.h"
#include "messaging/messagewriter.h"
#include "io/memorystream.h"

namespace Test
{
__ImplementClass(Test::MessageReaderWriterTest, 'MRWT', Test::TestCase);
__ImplementClass(Test::TestMessage, 'TMSG', Messaging::Message);
__ImplementMsgId(Test::TestMessage);

using namespace Util;
using namespace IO;
using namespace Core;
using namespace Messaging;
using namespace Math;

//------------------------------------------------------------------------------
/**
    Encode method of the test message class.
*/
void
TestMessage::Encode(const Ptr<BinaryWriter>& writer)
{
    Message::Encode(writer);
    writer->WriteString(this->command);
    writer->WriteVec4(this->position);
    writer->WriteFloat(this->velocity);
}

//------------------------------------------------------------------------------
/**
    Decode method of the test message class.
*/
void
TestMessage::Decode(const Ptr<BinaryReader>& reader)
{
    Message::Decode(reader);
    this->command = reader->ReadString();
    this->position = reader->ReadVec4();
    this->velocity = reader->ReadFloat();
}

//------------------------------------------------------------------------------
/**
*/
void
MessageReaderWriterTest::Run()
{
    // create a few test messages
    Ptr<TestMessage> origMsg1 = TestMessage::Create();
    origMsg1->SetCommand("Goto");
    origMsg1->SetPosition(point(1.0f, 2.0f, 3.0f));
    origMsg1->SetVelocity(4.0f);

    Ptr<TestMessage> origMsg2 = TestMessage::Create();
    origMsg2->SetCommand("Flyto");
    origMsg2->SetPosition(point(5.0f, 6.0f, 7.0f));
    origMsg2->SetVelocity(8.0f);
    
    // create a memory stream, and attach a message writer and
    // message reader to it
    Ptr<MemoryStream> stream = MemoryStream::Create();
    Ptr<MessageWriter> writer = MessageWriter::Create();
    Ptr<MessageReader> reader = MessageReader::Create();

    // encode the messages into the stream
    stream->SetAccessMode(Stream::WriteAccess);
    VERIFY(stream->Open());
    writer->SetStream(stream.upcast<Stream>());
    VERIFY(writer->Open());
    writer->WriteMessage(origMsg1.upcast<Message>());
    writer->WriteMessage(origMsg2.upcast<Message>());
    writer->Close();
    stream->Close();

    // open stream for reading and read back messages
    stream->SetAccessMode(Stream::ReadAccess);
    VERIFY(stream->Open());
    reader->SetStream(stream.upcast<Stream>());
    VERIFY(reader->Open());
    Ptr<TestMessage> copyMsg1 = (TestMessage*) reader->ReadMessage();
    Ptr<TestMessage> copyMsg2 = (TestMessage*) reader->ReadMessage();
    reader->Close();
    stream->Close();

    // verify result
    VERIFY(copyMsg1->IsA(TestMessage::RTTI));
    VERIFY(copyMsg2->IsA(TestMessage::RTTI));
    VERIFY(copyMsg1->GetCommand() == "Goto");
    VERIFY(copyMsg1->GetPosition() == point(1.0f, 2.0f, 3.0f));
    VERIFY(copyMsg1->GetVelocity() == 4.0f);
    VERIFY(copyMsg2->GetCommand() == "Flyto");
    VERIFY(copyMsg2->GetPosition() == point(5.0f, 6.0f, 7.0f));
    VERIFY(copyMsg2->GetVelocity() == 8.0f);
}

}; // namespace Test
