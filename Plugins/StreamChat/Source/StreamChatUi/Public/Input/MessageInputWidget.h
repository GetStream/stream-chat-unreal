// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/Border.h"
#include "Components/EditableTextBox.h"
#include "CoreMinimal.h"
#include "StreamWidget.h"

#include "MessageInputWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UMessageInputWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UMessageInputWidget();
    virtual void SynchronizeProperties() override;

    UFUNCTION(BlueprintCallable, Category = Widget)
    FText GetText() const;

    UFUNCTION(BlueprintCallable, Category = Widget)
    void SetText(FText InText);

    /** Hint text that appears when there is no text in the text box */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Content)
    FText HintText;

    /** Called whenever the text is changed programmatically or interactively by the user */
    UPROPERTY(BlueprintAssignable, Category = "TextBox|Event")
    UEditableTextBox::FOnEditableTextBoxChangedEvent OnTextChanged;

    /** Called whenever the text is committed.  This happens when the user presses enter or the text box loses focus. */
    UPROPERTY(BlueprintAssignable, Category = "TextBox|Event")
    UEditableTextBox::FOnEditableTextBoxCommittedEvent OnTextCommitted;

protected:
    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* TextBox;
    UPROPERTY(meta = (BindWidget))
    UBorder* Border;

private:
    virtual void NativeOnInitialized() override;
    virtual void OnTheme() override;
    virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

    UFUNCTION()
    void OnInputTextChanged(const FText& Text);
    UFUNCTION()
    void OnInputTextCommit(const FText& Text, ETextCommit::Type CommitMethod);
};
