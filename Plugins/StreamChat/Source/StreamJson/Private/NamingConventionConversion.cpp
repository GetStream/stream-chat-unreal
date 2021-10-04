
#include "NamingConventionConversion.h"

#include "Internationalization/BreakIterator.h"
#include "Internationalization/IBreakIterator.h"

namespace
{
TSharedPtr<IBreakIterator> NameBreakIterator;
}
FString NamingConventionConversion::ConvertNameToSnakeCase(const FStringView InName)
{
	FString SnakeCaseName;
	SnakeCaseName.Reserve(InName.Len() + 10);

	if (!NameBreakIterator.IsValid())
	{
		NameBreakIterator = FBreakIterator::CreateCamelCaseBreakIterator();
	}

	NameBreakIterator->SetStringRef(InName);
	for (int32 PrevBreak = 0, NameBreak = NameBreakIterator->MoveToNext(); NameBreak != INDEX_NONE;
		 NameBreak = NameBreakIterator->MoveToNext())
	{
		const int32 OrigSnakeCaseNameLen = SnakeCaseName.Len();

		// Append an underscore if this was a break between two parts of the identifier, *and* the previous character isn't already
		// an underscore
		if (OrigSnakeCaseNameLen > 0 && SnakeCaseName[OrigSnakeCaseNameLen - 1] != TEXT('_'))
		{
			SnakeCaseName += TEXT('_');
		}

		// Append this part of the identifier
		SnakeCaseName.AppendChars(&InName[PrevBreak], NameBreak - PrevBreak);

		// Remove any trailing underscores in the last part of the identifier
		while (SnakeCaseName.Len() > OrigSnakeCaseNameLen)
		{
			const int32 CharIndex = SnakeCaseName.Len() - 1;
			if (SnakeCaseName[CharIndex] != TEXT('_'))
			{
				break;
			}
			SnakeCaseName.RemoveAt(CharIndex, 1, false);
		}

		PrevBreak = NameBreak;
	}
	NameBreakIterator->ClearString();

	SnakeCaseName.ToLowerInline();

	return SnakeCaseName;
}

FString NamingConventionConversion::ConvertPropertyNameToSnakeCase(const FStringView InName)
{
	int32 NameOffset = 0;

	for (;;)
	{
		// Strip the "b" prefix from bool names
		if (InName.Len() - NameOffset >= 2 && InName[NameOffset] == TEXT('b') && FChar::IsUpper(InName[NameOffset + 1]))
		{
			NameOffset += 1;
			continue;
		}

		// Strip the "In" prefix from names
		if (InName.Len() - NameOffset >= 3 && InName[NameOffset] == TEXT('I') && InName[NameOffset + 1] == TEXT('n') &&
			FChar::IsUpper(InName[NameOffset + 2]))
		{
			NameOffset += 2;
			continue;
		}

		// Strip the "Out" prefix from names
		// if (InName.Len() - NameOffset >= 4 && InName[NameOffset] == TEXT('O') && InName[NameOffset + 1] == TEXT('u') &&
		// InName[NameOffset + 2] == TEXT('t') && FChar::IsUpper(InName[NameOffset + 3]))
		//{
		//	NameOffset += 3;
		//	continue;
		//}

		// Nothing more to strip
		break;
	}

	return ConvertNameToSnakeCase(NameOffset ? InName.RightChop(NameOffset) : InName);
}
