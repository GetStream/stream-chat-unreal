// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

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

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEditableTextBoxChangedEvent, const FText&, Text);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEditableTextBoxCommittedEvent, const FText&, Text, ETextCommit::Type, CommitMethod);
    /** Called whenever the text is changed programmatically or interactively by the user */
    UPROPERTY(BlueprintAssignable, Category = "TextBox|Event")
    FOnEditableTextBoxChangedEvent OnTextChanged;

    /** Called whenever the text is committed.  This happens when the user presses enter or the text box loses focus. */
    UPROPERTY(BlueprintAssignable, Category = "TextBox|Event")
    FOnEditableTextBoxCommittedEvent OnTextCommitted;

protected:
    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* TextBox;
    UPROPERTY(meta = (BindWidget))
    UBorder* Border;

private:
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

    UFUNCTION()
    void OnInputTextChanged(const FText& Text);
    UFUNCTION()
    void OnInputTextCommit(const FText& Text, ETextCommit::Type CommitMethod);
};
