﻿#include "StreamJson.h"

#include "Misc/AutomationTest.h"
#include "TestJson.h"

BEGIN_DEFINE_SPEC(
    FJsonSpec,
    "StreamChat.StreamJson",
    EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
END_DEFINE_SPEC(FJsonSpec)

void FJsonSpec::Define()
{
    Describe(
        "Deserialize",
        [this]
        {
            It("should deserialize sparse out-of-order JSON",
               EAsyncExecution::ThreadPool,
               [this]()
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
               });

            It("should deserialize all basic types",
               EAsyncExecution::ThreadPool,
               [this]()
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
               });

            It("should deserialize arrays of all basic types",
               EAsyncExecution::ThreadPool,
               [this]()
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
               });

            It("should deserialize nested objects",
               EAsyncExecution::ThreadPool,
               [this]()
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
               });

            It("should deserialize UpperCamelCase string unions to enums",
               EAsyncExecution::ThreadPool,
               [this]()
               {
                   const FString Json = R"(
{
  "ManyWordsEnum": "SecondEnumValue"
}
					)";
                   const FTestEnum Obj = Json::Deserialize<FTestEnum>(Json);
                   TestEqual("enum", Obj.ManyWordsEnum, ETestEnum::SecondEnumValue);
               });

            It("should deserialize snake_case with number string unions to enums",
               EAsyncExecution::ThreadPool,
               [this]()
               {
                   const FString Json = R"(
{
  "many_words_enum": "number3"
}
					)";
                   const FTestEnum Obj = Json::Deserialize<FTestEnum>(Json);
                   TestEqual("enum", Obj.ManyWordsEnum, ETestEnum::Number3);
               });

            It("should deserialize snake_case string unions to enums",
               EAsyncExecution::ThreadPool,
               [this]()
               {
                   const FString Json = R"(
{
  "many_words_enum": "second_enum_value"
}
					)";
                   const FTestEnum Obj = Json::Deserialize<FTestEnum>(Json);
                   TestEqual("enum", Obj.ManyWordsEnum, ETestEnum::SecondEnumValue);
               });

            It("should deserialize with snake_case property names",
               EAsyncExecution::ThreadPool,
               [this]()
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
               });
        });

    Describe(
        "Serialize",
        [this]
        {
            It("should serialize all basic types and arrays",
               EAsyncExecution::ThreadPool,
               [this]()
               {
                   const FTestJson Obj = {
                       1,
                       2,
                       3.f,
                       4.,
                       true,
                       "hello",
                       {"hi", 1.f},
                       {1, 2, 3},
                       {5, 6},
                       {2.f, 4.f},
                       {8., 16.},
                       {true, false},
                       {"a", "b"},
                       {{"1", 1.f}},
                       ETestEnum::Number3};
                   const FString Json = Json::Serialize(Obj, ENamingConvention::SnakeCase);
                   const FString ExpectedJson =
                       R"({"int32":1,"int64":2,"float":3,"double":4,"boolean":true,"string":"hello","nested_object":{"foo":"hi","bar":1},"array_of_int32":[1,2,3],"array_of_int64":[5,6],"array_of_float":[2,4],"array_of_double":[8,16],"array_of_boolean":[true,false],"array_of_string":["a","b"],"array_of_nested_object":[{"foo":"1","bar":1}],"enum":"number3"})";
                   TestEqual("JSON", Json, ExpectedJson);
               });

            It("should serialize property names and enum values to UpperCamelCase",
               EAsyncExecution::ThreadPool,
               [this]()
               {
                   const FTestJson Obj = {
                       1,
                       2,
                       3.f,
                       4.,
                       true,
                       "hello",
                       {"hi", 1.f},
                       {1, 2, 3},
                       {5, 6},
                       {2.f, 4.f},
                       {8., 16.},
                       {true, false},
                       {"a", "b"},
                       {{"1", 1.f}},
                       ETestEnum::Number3};
                   const FString Json = Json::Serialize(Obj, ENamingConvention::UpperCamelCase);
                   const FString ExpectedJson =
                       R"({"Int32":1,"Int64":2,"Float":3,"Double":4,"Boolean":true,"String":"hello","NestedObject":{"Foo":"hi","bar":1},"ArrayOfInt32":[1,2,3],"ArrayOfInt64":[5,6],"ArrayOfFloat":[2,4],"ArrayOfDouble":[8,16],"ArrayOfBoolean":[true,false],"ArrayOfString":["a","b"],"ArrayOfNestedObject":[{"foo":"1","bar":1}],"Enum":"Number3"})";
                   TestEqual("JSON", Json, ExpectedJson);
               });

            It("should serialize enum value to UpperCamelCase string",
               EAsyncExecution::ThreadPool,
               [this]()
               {
                   constexpr FTestEnum Obj{ETestEnum::SecondEnumValue};
                   const FString Json = Json::Serialize(Obj, ENamingConvention::UpperCamelCase);
                   const FString ExpectedJson = R"({"ManyWordsEnum":"SecondEnumValue"})";
                   TestEqual("JSON", Json, ExpectedJson);
               });

            It("should serialize enum value to snake_case string",
               EAsyncExecution::ThreadPool,
               [this]()
               {
                   constexpr FTestEnum Obj{ETestEnum::SecondEnumValue};
                   const FString Json = Json::Serialize(Obj, ENamingConvention::SnakeCase);
                   const FString ExpectedJson = R"({"many_words_enum":"second_enum_value"})";
                   TestEqual("JSON", Json, ExpectedJson);
               });

            It("should serialize nested enum value to snake_case string",
               EAsyncExecution::ThreadPool,
               [this]()
               {
                   constexpr FTestNestedEnum Obj{{ETestEnum::SecondEnumValue}};
                   const FString Json = Json::Serialize(Obj, ENamingConvention::SnakeCase);
                   const FString ExpectedJson = R"({"nested_enum":{"many_words_enum":"second_enum_value"}})";
                   TestEqual("JSON", Json, ExpectedJson);
               });
        });
}