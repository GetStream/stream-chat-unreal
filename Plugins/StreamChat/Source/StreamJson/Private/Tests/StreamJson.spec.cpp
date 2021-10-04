#include "StreamJson.h"

#include "Misc/AutomationTest.h"
#include "TestJson.h"

BEGIN_DEFINE_SPEC(
	FJsonSpec, "StreamChat.StreamJson", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
END_DEFINE_SPEC(FJsonSpec)

void FJsonSpec::Define()
{
	Describe("Deserialize",
		[this]
		{
			It("should deserialize sparse out-of-order JSON", EAsyncExecution::ThreadPool,
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

			It("should deserialize all basic types", EAsyncExecution::ThreadPool,
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
					TestEqual("boolean", Obj.Boolean, true);
					TestEqual("string", Obj.String, "hello");
				});

			It("should deserialize arrays of all basic types", EAsyncExecution::ThreadPool,
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
			It("should deserialize nested objects", EAsyncExecution::ThreadPool,
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

			It("should deserialize PascalCase string unions to enums", EAsyncExecution::ThreadPool,
				[this]()
				{
					const FString Json = R"(
{
  "enum": "SecondEnumValue"
}
					)";
					const FTestJson Obj = Json::Deserialize<FTestJson>(Json);
					TestEqual("enum", Obj.Enum, ETestEnum::SecondEnumValue);
				});

			It("should deserialize CamelCase string unions to enums", EAsyncExecution::ThreadPool,
				[this]()
				{
					const FString Json = R"(
{
  "enum": "number3"
}
					)";
					const FTestJson Obj = Json::Deserialize<FTestJson>(Json);
					TestEqual("enum", Obj.Enum, ETestEnum::Number3);
				});

			It("should deserialize snake_case string unions to enums", EAsyncExecution::ThreadPool,
				[this]()
				{
					const FString Json = R"(
{
  "enum": "second_enum_value"
}
					)";
					const FTestJson Obj = Json::Deserialize<FTestJson>(Json);
					TestEqual("enum", Obj.Enum, ETestEnum::SecondEnumValue);
				});
		});

	Describe("Serialize",
		[this] {

		});
}
