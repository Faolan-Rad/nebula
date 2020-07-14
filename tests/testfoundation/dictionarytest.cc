//------------------------------------------------------------------------------
//  dictionarytest.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "dictionarytest.h"
#include "util/dictionary.h"

namespace Test
{
__ImplementClass(Test::DictionaryTest, 'DCTT', Test::TestCase);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
void
DictionaryTest::Run()
{
    // create a german-to-english dictionary
    Dictionary<String,String> dict;
    VERIFY(dict.IsEmpty());
    VERIFY(dict.Size() == 0);

    // add some values
    dict.Add("Zwei", "Two");
    dict.Add("Eins", "One");
    dict.Add("Drei", "Three");
    dict.Add("Fuenf", "Five");
    dict.Add("Vier", "Four");
    VERIFY(!dict.IsEmpty());
    VERIFY(dict.Size() == 5);
    VERIFY(dict.Contains("Eins"));
    VERIFY(dict.Contains("Drei"));
    VERIFY(dict.Contains("Fuenf"));
    VERIFY(!dict.Contains("Two"));
    VERIFY(!dict.Contains("Sechs"));
    VERIFY(0 == dict.FindIndex("Drei"));
    VERIFY(4 == dict.FindIndex("Zwei"));
    VERIFY(1 == dict.FindIndex("Eins"));
    VERIFY(3 == dict.FindIndex("Vier"));
    VERIFY(2 == dict.FindIndex("Fuenf"));
    VERIFY(dict["Eins"] == "One");
    VERIFY(dict["Zwei"] == "Two");
    VERIFY(dict["Drei"] == "Three");
    VERIFY(dict["Vier"] == "Four");
    VERIFY(dict["Fuenf"] == "Five");
    // NOTE: the following should cause an assertion
    //VERIFY(dict["Sechs"] == "Six");
    VERIFY(dict.KeyAtIndex(0) == "Drei");
    VERIFY(dict.KeyAtIndex(1) == "Eins");
    VERIFY(dict.KeyAtIndex(2) == "Fuenf");
    VERIFY(dict.KeyAtIndex(3) == "Vier");
    VERIFY(dict.KeyAtIndex(4) == "Zwei");
    VERIFY(dict.ValueAtIndex(0) == "Three");
    VERIFY(dict.ValueAtIndex(1) == "One");
    VERIFY(dict.ValueAtIndex(2) == "Five");
    VERIFY(dict.ValueAtIndex(3) == "Four");
    VERIFY(dict.ValueAtIndex(4) == "Two");

    // try changing a value
    dict["Eins"] = "Aaans";
    VERIFY(dict["Eins"] == "Aaans");
    VERIFY(dict.ValueAtIndex(1) == "Aaans");

    // check if Erase() works
    dict.Erase("Eins");
    VERIFY(dict.Size() == 4);
    VERIFY(InvalidIndex == dict.FindIndex("Eins"));
    VERIFY(dict.Contains("Zwei"));
    VERIFY(dict.Contains("Drei"));
    VERIFY(dict.Contains("Vier"));
    VERIFY(dict.Contains("Fuenf"));

    dict.Add("Eins", "One");
    dict.Add("Sechs", "Six");
    dict.Add("Sieben", "Seven");
    dict.Add("Acht", "Eight");
    dict.Add("Neun", "Nine");
    dict.Add("Zehn", "Ten");
    dict.Add("Elf", "Eleven");
    dict.Add("Zwoelf", "Eleven");
    dict.Add("Dreizehn", "Thirteen");
    dict.Add("Vierzehn", "Fourteen");
    dict.Add("Fuenfzehn", "Fifteen");
    dict.Add("Sechzehn", "Sixteen");
    dict.Add("Siebzehn", "Seventeen");
    dict.Add("Achtzehn", "Eighteen");
    dict.Add("Neunzehn", "Nineteen");
    dict.Add("Zwanzig", "Twenty");

    VERIFY(dict.Contains("Eins"));
    VERIFY(dict.Contains("Zwei"));
    VERIFY(dict.Contains("Drei"));
    VERIFY(dict.Contains("Vier"));
    VERIFY(dict.Contains("Fuenf"));
    VERIFY(dict.Contains("Sechs"));
    VERIFY(dict.Contains("Sieben"));
    VERIFY(dict.Contains("Acht"));
    VERIFY(dict.Contains("Neun"));
    VERIFY(dict.Contains("Zehn"));
    VERIFY(dict.Contains("Elf"));
    VERIFY(dict.Contains("Zwoelf"));
    VERIFY(dict.Contains("Dreizehn"));
    VERIFY(dict.Contains("Vierzehn"));
    VERIFY(dict.Contains("Fuenfzehn"));
    VERIFY(dict.Contains("Sechzehn"));
    VERIFY(dict.Contains("Siebzehn"));
    VERIFY(dict.Contains("Achtzehn"));
    VERIFY(dict.Contains("Neunzehn"));
    VERIFY(dict.Contains("Zwanzig"));

    // NOTE: the following should not compile
    // dict.KeyAtIndex(0) = "Zehn";
}

}; // namespace Test

