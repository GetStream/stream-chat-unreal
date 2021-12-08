// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "StreamJson.h"

#include "Misc/AutomationTest.h"
#include "TestJson.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FJsonTestDeserializeOutOfOrder,
    "StreamChat.StreamJson.Deserialize.OutOfOrder",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FJsonTestDeserializeOutOfOrder::RunTest(const FString& Parameters)
{
    const FString Json = R"(
{
  "int64": 1,
  "float": 1.3,
  "int32": 6
}
					)";
    const FTestJson Obj = Json::Deserialize<FTestJson>(Json);
    TestEqual("int64", Obj.Int64, 1);
    TestEqual("float", Obj.Float, 1.3f);
    TestEqual("int32", Obj.Int32, 6);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FJsonTestDeserializeBasicTypes,
    "StreamChat.StreamJson.Deserialize.BasicTypes",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FJsonTestDeserializeBasicTypes::RunTest(const FString& Parameters)
{
    const FString Json = R"(
{
  "int32": 1,
  "int64": 2,
  "float": 3.14,
  "double": 4.56,
  "boolean": true,
  "string": "hello"
}
					)";
    const FTestJson Obj = Json::Deserialize<FTestJson>(Json);
    TestEqual("int32", Obj.Int32, 1);
    TestEqual("int64", Obj.Int64, 2);
    TestEqual("float", Obj.Float, 3.14f);
    TestEqual("double", Obj.Double, 4.56);
    TestEqual("boolean", Obj.bBoolean, true);
    TestEqual("string", Obj.String, "hello");
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FJsonTestDeserializeArrays,
    "StreamChat.StreamJson.Deserialize.Arrays",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FJsonTestDeserializeArrays::RunTest(const FString& Parameters)
{
    const FString Json = R"(
{
  "arrayOfInt32": [1, 2, 3],
  "arrayOfInt64": [4, 5],
  "arrayOfFloat": [6.7],
  "arrayOfDouble": [8.9, 1.234, 5.678],
  "arrayOfBoolean": [false, false, true, false, true],
  "arrayOfString": ["hello", "world"]
}
					)";
    const FTestJson Obj = Json::Deserialize<FTestJson>(Json);
    TestEqual("int32[]", Obj.ArrayOfInt32, {1, 2, 3});
    TestEqual("int64[]", Obj.ArrayOfInt64, {4, 5});
    TestEqual("float[]", Obj.ArrayOfFloat, {6.7f});
    TestEqual("double[]", Obj.ArrayOfDouble, {8.9, 1.234, 5.678});
    TestEqual("boolean[]", Obj.ArrayOfBoolean, {false, false, true, false, true});
    TestEqual("string[]", Obj.ArrayOfString, {"hello", "world"});
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FJsonTestDeserializeNested,
    "StreamChat.StreamJson.Deserialize.Nested",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FJsonTestDeserializeNested::RunTest(const FString& Parameters)
{
    const FString Json = R"(
{
  "nestedObject": {
    "foo": "hello world",
    "bar": 1.23
  },
  "float": 8.19,
  "arrayOfNestedObject": [
    {
      "bar": 1
    },
    {
      "bar": 2
    },
    {
      "bar": 3,
      "foo": "nice"
    }
  ]
}
					)";
    const FTestJson Obj = Json::Deserialize<FTestJson>(Json);
    TestEqual("nestedObject.foo", Obj.NestedObject.Foo, "hello world");
    TestEqual("nestedObject.bar", Obj.NestedObject.Bar, 1.23f);
    TestEqual("nestedObject.bar", Obj.Float, 8.19f);
    TestEqual("arrayOfNestedObject[0].bar", Obj.ArrayOfNestedObject[0].Bar, 1.f);
    TestEqual("arrayOfNestedObject[1].bar", Obj.ArrayOfNestedObject[1].Bar, 2.f);
    TestEqual("arrayOfNestedObject[3].bar", Obj.ArrayOfNestedObject[2].Bar, 3.f);
    TestEqual("arrayOfNestedObject[3].foo", Obj.ArrayOfNestedObject[2].Foo, "nice");
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FJsonTestDeserializeUpperCamelCaseStringEnums,
    "StreamChat.StreamJson.Deserialize.UpperCamelCaseStringEnums",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FJsonTestDeserializeUpperCamelCaseStringEnums::RunTest(const FString& Parameters)
{
    const FString Json = R"(
{
  "ManyWordsEnum": "SecondEnumValue"
}
					)";
    const FTestEnum Obj = Json::Deserialize<FTestEnum>(Json);
    TestEqual("enum", Obj.ManyWordsEnum, ETestEnum::SecondEnumValue);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FJsonTestDeserializeSnakeCaseStringEnums,
    "StreamChat.StreamJson.Deserialize.SnakeCaseStringEnums",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FJsonTestDeserializeSnakeCaseStringEnums::RunTest(const FString& Parameters)
{
    const FString Json = R"(
{
  "many_words_enum": "number3"
}
					)";
    const FTestEnum Obj = Json::Deserialize<FTestEnum>(Json);
    TestEqual("enum", Obj.ManyWordsEnum, ETestEnum::Number3);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FJsonTestDeserializeSnakeCaseNumEnums,
    "StreamChat.StreamJson.Deserialize.SnakeCaseNumEnums",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FJsonTestDeserializeSnakeCaseNumEnums::RunTest(const FString& Parameters)
{
    const FString Json = R"(
{
  "many_words_enum": "second_enum_value"
}
					)";
    const FTestEnum Obj = Json::Deserialize<FTestEnum>(Json);
    TestEqual("enum", Obj.ManyWordsEnum, ETestEnum::SecondEnumValue);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FJsonTestDeserializeSnakeCasePropNameEnums,
    "StreamChat.StreamJson.Deserialize.SnakeCasePropNameEnums",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FJsonTestDeserializeSnakeCasePropNameEnums::RunTest(const FString& Parameters)
{
    const FString Json = R"(
{
  "array_of_int32": [1, 2, 3],
  "array_of_int64": [4, 5],
  "array_of_float": [6.7],
  "array_of_double": [8.9, 1.234, 5.678],
  "array_of_boolean": [false, false, true, false, true],
  "array_of_string": ["hello", "world"]
}
					)";
    const FTestJson Obj = Json::Deserialize<FTestJson>(Json);
    TestEqual("int32[]", Obj.ArrayOfInt32, {1, 2, 3});
    TestEqual("int64[]", Obj.ArrayOfInt64, {4, 5});
    TestEqual("float[]", Obj.ArrayOfFloat, {6.7f});
    TestEqual("double[]", Obj.ArrayOfDouble, {8.9, 1.234, 5.678});
    TestEqual("boolean[]", Obj.ArrayOfBoolean, {false, false, true, false, true});
    TestEqual("string[]", Obj.ArrayOfString, {"hello", "world"});
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FJsonTestSerializeBasicTypes,
    "StreamChat.StreamJson.Serialize.BasicTypes",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FJsonTestSerializeBasicTypes::RunTest(const FString& Parameters)
{
    const FTestJson Obj = {
        1, 2, 3.f, 4., true, "hello", {"hi", 1.f}, {1, 2, 3}, {5, 6}, {2.f, 4.f}, {8., 16.}, {true, false}, {"a", "b"}, {{"1", 1.f}}, ETestEnum::Number3};
    const FString Json = Json::Serialize(Obj, ENamingConvention::SnakeCase);
    const FString ExpectedJson =
        R"({"int32":1,"int64":2,"float":3,"double":4,"boolean":true,"string":"hello","nested_object":{"foo":"hi","bar":1},"array_of_int32":[1,2,3],"array_of_int64":[5,6],"array_of_float":[2,4],"array_of_double":[8,16],"array_of_boolean":[true,false],"array_of_string":["a","b"],"array_of_nested_object":[{"foo":"1","bar":1}],"enum":"number3"})";
    TestEqual("JSON", Json, ExpectedJson);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FJsonTestSerializeUpperCamelCasePropNames,
    "StreamChat.StreamJson.Serialize.UpperCamelCasePropNames",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FJsonTestSerializeUpperCamelCasePropNames::RunTest(const FString& Parameters)
{
    const FTestJson Obj = {
        1, 2, 3.f, 4., true, "hello", {"hi", 1.f}, {1, 2, 3}, {5, 6}, {2.f, 4.f}, {8., 16.}, {true, false}, {"a", "b"}, {{"1", 1.f}}, ETestEnum::Number3};
    const FString Json = Json::Serialize(Obj, ENamingConvention::UpperCamelCase);
    const FString ExpectedJson =
        R"({"Int32":1,"Int64":2,"Float":3,"Double":4,"Boolean":true,"String":"hello","NestedObject":{"Foo":"hi","bar":1},"ArrayOfInt32":[1,2,3],"ArrayOfInt64":[5,6],"ArrayOfFloat":[2,4],"ArrayOfDouble":[8,16],"ArrayOfBoolean":[true,false],"ArrayOfString":["a","b"],"ArrayOfNestedObject":[{"foo":"1","bar":1}],"Enum":"Number3"})";
    TestEqual("JSON", Json, ExpectedJson);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FJsonTestSerializeUpperCamelCaseEnumValues,
    "StreamChat.StreamJson.Serialize.UpperCamelCaseEnumValues",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FJsonTestSerializeUpperCamelCaseEnumValues::RunTest(const FString& Parameters)
{
    constexpr FTestEnum Obj{ETestEnum::SecondEnumValue};
    const FString Json = Json::Serialize(Obj, ENamingConvention::UpperCamelCase);
    const FString ExpectedJson = R"({"ManyWordsEnum":"SecondEnumValue"})";
    TestEqual("JSON", Json, ExpectedJson);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FJsonTestSerializeSnakeCaseEnumValues,
    "StreamChat.StreamJson.Serialize.SnakeCaseEnumValues",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FJsonTestSerializeSnakeCaseEnumValues::RunTest(const FString& Parameters)
{
    constexpr FTestEnum Obj{ETestEnum::SecondEnumValue};
    const FString Json = Json::Serialize(Obj, ENamingConvention::SnakeCase);
    const FString ExpectedJson = R"({"many_words_enum":"second_enum_value"})";
    TestEqual("JSON", Json, ExpectedJson);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FJsonTestSerializeNestedSnakeCaseEnumValues,
    "StreamChat.StreamJson.Serialize.NestedSnakeCaseEnumValues",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FJsonTestSerializeNestedSnakeCaseEnumValues::RunTest(const FString& Parameters)
{
    constexpr FTestNestedEnum Obj{{ETestEnum::SecondEnumValue}};
    const FString Json = Json::Serialize(Obj, ENamingConvention::SnakeCase);
    const FString ExpectedJson = R"({"nested_enum":{"many_words_enum":"second_enum_value"}})";
    TestEqual("JSON", Json, ExpectedJson);
    return true;
}

#endif
