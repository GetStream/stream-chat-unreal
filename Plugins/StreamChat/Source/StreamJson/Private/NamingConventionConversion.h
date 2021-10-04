#pragma once

namespace NamingConventionConversion
{
/** Given a CamelCase name, convert it to snake_case */
FString ConvertNameToSnakeCase(const FStringView InName);

/** Given a CamelCase property name, convert it to snake_case (may remove some superfluous parts of the property name) */
FString ConvertPropertyNameToSnakeCase(const FStringView InName);
}	 // namespace NamingConventionConversion
