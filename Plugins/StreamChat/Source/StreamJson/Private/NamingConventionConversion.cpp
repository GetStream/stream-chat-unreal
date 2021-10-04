
#include "NamingConventionConversion.h"

#include "Internationalization/BreakIterator.h"
#include "Internationalization/IBreakIterator.h"

namespace
{
FStringView TrimPropertyNameSuperfluous(const FStringView View)
{
	int32 NameOffset = 0;

	for (;;)
	{
		// Strip the "b" prefix from bool names
		if (View.Len() - NameOffset >= 2 && View[NameOffset] == TEXT('b') && FChar::IsUpper(View[NameOffset + 1]))
		{
			NameOffset += 1;
			continue;
		}

		// Strip the "In" prefix from names
		if (View.Len() - NameOffset >= 3 && View[NameOffset] == TEXT('I') && View[NameOffset + 1] == TEXT('n') &&
			FChar::IsUpper(View[NameOffset + 2]))
		{
			NameOffset += 2;
			continue;
		}

		// Strip the "Out" prefix from names
		// if (View.Len() - NameOffset >= 4 && View[NameOffset] == TEXT('O') && View[NameOffset + 1] == TEXT('u') &&
		// View[NameOffset + 2] == TEXT('t') && FChar::IsUpper(View[NameOffset + 3]))
		//{
		//	NameOffset += 3;
		//	continue;
		//}

		// Nothing more to strip
		break;
	}

	return NameOffset ? View.RightChop(NameOffset) : View;
}
TSharedPtr<IBreakIterator> NameBreakIterator;
TSharedPtr<IBreakIterator> GraphemeIterator;
}	 // namespace
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
	return ConvertNameToSnakeCase(TrimPropertyNameSuperfluous(InName));
}

FString NamingConventionConversion::ConvertPropertyNameToUpperCamelCase(const FStringView InName)
{
	return FString(TrimPropertyNameSuperfluous(InName));
}

FString NamingConventionConversion::ConvertSnakeCaseToUpperCamelCase(const FStringView InString)
{
	FString UpperCamelCase;
	UpperCamelCase.Reserve(InString.Len());

	if (!GraphemeIterator.IsValid())
	{
		GraphemeIterator = FBreakIterator::CreateCamelCaseBreakIterator();
	}

	GraphemeIterator->SetStringRef(InString);
	for (int32 PrevBreak = 0, GraphemeBreak = GraphemeIterator->MoveToNext(); GraphemeBreak != INDEX_NONE;
		 GraphemeBreak = GraphemeIterator->MoveToNext())
	{
		if (InString[PrevBreak] != TEXT('_'))
		{
			// Append first char as upper
			UpperCamelCase.AppendChar(FChar::ToUpper(InString[PrevBreak]));
			// Append rest of word
			UpperCamelCase.AppendChars(&InString[PrevBreak] + 1, GraphemeBreak - PrevBreak - 1);
		}

		PrevBreak = GraphemeBreak;
	}
	GraphemeIterator->ClearString();

	return UpperCamelCase;
}
